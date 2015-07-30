//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2015.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <plib.h>

#include "platform.h"
#include "adf.h"
#include "cw.h"
#include "uart.h"
#include "command.h"
#include "config.h"
#include "scheduler.h"
#include "timing.h"

cw_nb_t cw;
void cw_task(void){
	cw_nb_tick(&cw);
}

void button_task(void){
	static uint8_t standby_state;
	if (STANDBY_BTN != standby_state){
		standby_state = STANDBY_BTN;
		if (standby_state){
			uart_puts("MSG: In standby mode\r\n");
			sch_task_disable(SCH_INDEX_CW);
		}
		else{
			uart_puts("MSG: In beacon mode\r\n");
			cw_nb_init(config.cw_string, &cw);
			sch_task_enable(SCH_INDEX_CW);
		}
	}
	
	// If standby mode is on...
	if (standby_state){
		// ...check if "key"-button is pressed (active low) and RF-output is off...
		if ( (!MANUAL_KEY_INV_BTN) && (!adf_common.rf_enable) ) cw_on();
		// ...then check if "key"-button is released and RF-output is on
		else if ( MANUAL_KEY_INV_BTN && adf_common.rf_enable ) cw_off();
	}
}

int main(int argc, char** argv) {
	platform_init();
	pwm_init();
	uart_init(38400);
	timer_init();

	// Initialize main settings
	strcpy(config.cw_string, " LA2UHF JP53EG ");
	config.freq = 432463; // Frequency in KHz, 432463 for LA2UHF
	config.duty = 195; // 0=0V=-40dB gain, 0.9V=0dB, 220=1.4V=23dB
	config.ramp_time = 30;
	cw.carrier_time = 50;
	adf_common.aux_enable = 0;
	adf_common.aux_power = 0;
	adf_common.rf_enable = 1;
	adf_common.rf_power = 0; // -4, -1, 2, 5 dBm - -1 gives the most out of VGA (~10 dBm, other just distorts more)
	
	// Initialize non-blocking morse code generator
	cw_nb_init(config.cw_string, &cw);

	// Initialize RF hardware
	ADL_EN = 1;
	ADF_EN = 1;
	ADF_LE = 0;
	__delay_ms(10);

	// Enable interrupts
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

	// Run CW-generator at 100 ms intervals
	sch_task_init(SCH_INDEX_CW, 100, &cw_task);			
	sch_task_enable(SCH_INDEX_CW);
	
	// Disable button task if buttons/switches are not connected!
	sch_task_init(SCH_INDEX_BUTTON, 10, &button_task);	
	sch_task_enable(SCH_INDEX_BUTTON);

	adf_calc_freq(config.freq);
	adf_set_all();
	uart_puts("\r\nLA1K CW-beacon HW 0.1, SW 0.11\r\n");
	
	while(1){
		while (uart_inwaiting()) cmd_byte(uart_get());
		sch_tick();
		CLRWDT();
	}

	return 0;
}
