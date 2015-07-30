//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "adf.h"

#include <xc.h>
#include "platform.h"
#include "spi.h"
#include "uart.h"
#include "stdio.h"
#include "stdint.h"

// Frequencies in KHz!
#define FPFD 10000
#define MINIMUM_VCO_FREQ 2200000
#define MOD_MAX 4096

adf_common_t adf_common;

/* SPI write function */
void adf_write(uint32_t data){
    /* Shift in 32 bits */
    spi((data&0xff000000) >> 24);
    spi((data&0x00ff0000) >> 16);
    spi((data&0x0000ff00) >> 8);
    spi(data&0x000000ff);

    /* Latch "Load Enable" */
    ADF_LE = 1;
    __delay_us(1); // 25 ns minimum
    ADF_LE = 0;
    __delay_us(1); // 20 ns minimum
}

/* Double buffering activated - remember to write REG0 last! */
void adf_set_all(void){
    uint32_t data;

    /* Register 5 */
    data = (uint32_t)((uint32_t)0b01<<ADF_LD_PIN_MODE_bp) | (uint32_t)0b11<<19 | 5;
    adf_write(data);

    /* Register 4 */
    data = ((uint32_t)1<<ADF_FEEDBACK_SEL_bp) |   // Feedback signal taken directly from VCO (fundamental)
           ((uint32_t)adf_common.rf_divider<<ADF_RF_DIV_SEL_bp) | // Divider
			((uint32_t)100<<ADF_BS_CLK_DIV_bp) |   // 10M/1/100 = 100 KHz. BS_CLK not higher than 125 KHz
           ((uint32_t)0<<ADF_VCO_PDN_bp) |   		// Power down VCO
           ((uint32_t)1<<ADF_MTLD_bp) |           // Mute RF output until lock
           ((uint32_t)0<<ADF_AUX_OUT_SEL_bp) |    // AUX outputs divided signal
           ((uint32_t)adf_common.aux_enable<<ADF_AUX_OUT_EN_bp) |     // AUX output disabled
           ((uint32_t)adf_common.aux_power<<ADF_AUX_OUT_PWR_bp) |    // AUX output power -4dBm {-4, -1, +2, +5}
           ((uint32_t)adf_common.rf_enable<<ADF_RF_OUT_EN_bp) |  	// RF output enabled
           ((uint32_t)adf_common.rf_power<<ADF_PWR_bp) |         // RF output power: 3=5dbm, 1=-1dBm
            4;
    adf_write(data);

    /* Register 3 */
    data = (uint32_t)((uint32_t)0<<ADF_BS_CLK_MODE_bp) |    // Slow band select clock mode (for < 125 KHz)
           (uint32_t)((uint32_t)0<<ADF_ABP_bp) |            // Wide antibacklash pulse - for fractional-N operation
           (uint32_t)((uint32_t)0<<ADF_CHARGE_C_bp) |       // Redice spurs in integer-N - not for fractional-N
           (uint32_t)((uint32_t)0<<ADF_CSR_bp) |            // Cycle slip reduction - faster lock time, but PFD must have 50% duty cycle
           (uint32_t)((uint32_t)0b00<<ADF_CLK_DIV_MODE_bp) |    // Resync enable
           (uint32_t)((uint32_t)0<<ADF_CLK_DIV_bp) |        // Clock divider value
            3;
    adf_write(data);

    /* Register 2 */
    data = (uint32_t)((uint32_t)0b00<<ADF_NOISE_MODES_bp) | // Low noise mode or low spur mode?
           (uint32_t)((uint32_t)0b011<<ADF_MUXOUT_bp) |     // Three state output
           (uint32_t)((uint32_t)0<<ADF_REF_DOUBLER_bp) |    // REF_DOUBLER - enabling it degrades phase noise if REF duty cycle is not 50%
           (uint32_t)((uint32_t)0<<ADF_RDIV2_bp) |          // Gives a 50% duty cycle signal at PFD-input - necessary for cycle slip reduction - ALLOWS 20 MHz clock, but must be on for 10 MHz too (?)
           (uint32_t)((uint32_t)1<<ADF_R_COUNTER_bp) |      // R-counter value, 1-1023
           (uint32_t)((uint32_t)1<<ADF_DOUBLE_BUFFER_bp) |  // Double buffering enabled
           (uint32_t)((uint32_t)0b1100<<ADF_CHARGE_PUMP_bp) |   // Sets the charge pump current - 4.06 mA
           (uint32_t)((uint32_t)0<<ADF_LDF_bp) |               // Lock detect function - 0 for fractional-N
           (uint32_t)((uint32_t)0<<ADF_LDP_bp) |            // 10 nS window for lock detection
           (uint32_t)((uint32_t)1<<ADF_PD_POLARITY_bp) |    // Normal polarity - for passive loop filter
           (uint32_t)((uint32_t)0<<ADF_POWER_DOWN_bp) |     // Power down circuit
           (uint32_t)((uint32_t)0<<ADF_CP_TSTATE_bp) |      // Put charge pump in three state
           (uint32_t)((uint32_t)0<<ADF_COUNTER_RST_bp) |    // Hold R- and N-counter in reset
            2;
    adf_write(data);

    /* Register 1 */
    data = (uint32_t)((uint32_t)0<<ADF_PHASE_EN_bp) |       // Phase adjustment disabled
           (uint32_t)((uint32_t)1<<ADF_PRESCALER_bp) |      // Prescaler (4/5 or 8/9) - max 3.6 GHz out
           (uint32_t)((uint32_t)1<<ADF_PHASE_bp) |          // Phase value
           (uint32_t)((uint32_t)adf_common.modulo<<ADF_MOD_bp) |       // Modulo value
            1;
    adf_write(data);

    /* Register 0 */
    data = (uint32_t)((uint32_t)adf_common.integer<<ADF_INT_bp) |      // Integer value
           (uint32_t)((uint32_t)adf_common.fraction<<ADF_FRAC_bp) |    // Fractional value
            0;
    adf_write(data);
}

// vco = (int+(frac/mod)) x fpfd - between 2.2 and 4.4 ghz
// RFout = vco / rf_divider
// fpfd = refin x ((1+d)/r*(1+T)) = 20 mhz  ikke over 32 MHz
//	D = 0 // RF REFin doubler
//	T = 0 // Reference divide-by-2
//	R = 1 // RF reference division (1-1023)
//			fpfd = 20MHz
// rf_divider = 200 // band select clock 100 KHz - shoudl be no high er than 125 khz
// INT 23-65535 (4/5 prescaler) or 75-65535 (8/9 prescaler)
// FRAC (0-(MOD-1))
// MOD (2-4095)

void adf_calc_freq(uint32_t f){
	uint32_t approx, error, error_best, error_ceil_frac;
	uint16_t integer, mod, frac, mod_best, frac_best, rf_divider;	// uint32_t is not large enough to hold 4.4 G
	
	// Find RF-divider
	rf_divider = 0;
	while (f < MINIMUM_VCO_FREQ){	// 2.2 GHz is the minimum VCO frequency
		f *= 2;
		rf_divider++;
	}
	
	// f is now VCO-frequency
	// Find integral part and remainder of frequency
	integer = f/FPFD;
	f -= (uint32_t)integer * FPFD;
	
	// f is now VCO-frequency remainder
	// Find modulo and fraction by searching for smallest error
	error_best = 0xffffffff;
	for (mod=2; mod<MOD_MAX; mod++){
		frac = f * mod / FPFD;
		
		// Error based on floored fraction
		approx = (uint32_t)FPFD * frac / mod;
		if (f > approx) error = f - approx;
		else error = approx - f;
		
		// Error based on ceiled fraction
		approx = (uint32_t)FPFD * (frac+1) / mod;
		if (f > approx) error_ceil_frac = f - approx;
		else error_ceil_frac = approx - f;
		
		// Check if error from ceiled fraction is smaller
		if (error_ceil_frac < error){
			error = error_ceil_frac;
			frac++;
		}
		
		// If this error is better than existing best, set new
		if (error < error_best){
			error_best = error;
			mod_best = mod;
			frac_best = frac;
			if (error_best == 0) break;	// Skip preliminarily if zero error
		}
	}			

	uint8_t msg[100];
	uint16_t msg_len, j;
	msg_len = sprintf(msg, "MSG: f_approx=%lu, mod=%u, frac=%u, int=%u, div=%u\r\n", 
			(((uint32_t)FPFD*integer) + ((uint32_t)FPFD * frac_best / mod_best)) / (1 << rf_divider),
			mod_best, 
			frac_best, 
			integer, 
			rf_divider);
	for (j=0; j<msg_len; j++) uart_put(msg[j]);

	// Update settings struct
	adf_common.modulo = mod_best;
	adf_common.fraction = frac_best;
	adf_common.integer = integer;
	adf_common.rf_divider = rf_divider;
}

void adf_init(void){
	ADF_EN = 1;
	ADF_LE = 0;
	__delay_ms(10);
	//adf_set_all();
}
