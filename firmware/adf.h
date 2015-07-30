//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef ADF_H
#define ADF_H

#include <stdint.h>

/* Register 0 */
#define ADF_INT_bp 15
#define ADF_FRAC_bp 3

/* Register 1 */
#define ADF_PHASE_EN_bp 28
#define ADF_PRESCALER_bp 27
#define ADF_PHASE_bp 15
#define ADF_MOD_bp 3

/* Register 2 */
#define ADF_NOISE_MODES_bp 29
#define ADF_MUXOUT_bp 26
#define ADF_REF_DOUBLER_bp 25
#define ADF_RDIV2_bp 24
#define ADF_R_COUNTER_bp 14
#define ADF_DOUBLE_BUFFER_bp 13
#define ADF_CHARGE_PUMP_bp 9
#define ADF_LDF_bp 8
#define ADF_LDP_bp 7
#define ADF_PD_POLARITY_bp 6
#define ADF_POWER_DOWN_bp 5
#define ADF_CP_TSTATE_bp 4
#define ADF_COUNTER_RST_bp 3

/* Register 3 */
#define ADF_BS_CLK_MODE_bp 23
#define ADF_ABP_bp 22
#define ADF_CHARGE_C_bp 21
#define ADF_CSR_bp 18
#define ADF_CLK_DIV_MODE_bp 15
#define ADF_CLK_DIV_bp 3

/* Register 4 */
#define ADF_FEEDBACK_SEL_bp 23
#define ADF_RF_DIV_SEL_bp 20
#define ADF_BS_CLK_DIV_bp 12
#define ADF_VCO_PDN_bp 11
#define ADF_MTLD_bp 10
#define ADF_AUX_OUT_SEL_bp 9
#define ADF_AUX_OUT_EN_bp 8
#define ADF_AUX_OUT_PWR_bp 6
#define ADF_RF_OUT_EN_bp 5
#define ADF_PWR_bp 3

/* Register 5 */
#define ADF_LD_PIN_MODE_bp 22

// Common settings struct
typedef struct{
	uint16_t modulo;
	uint16_t integer;
	uint16_t fraction;
	uint8_t rf_divider;
	uint8_t rf_enable;
	uint8_t aux_enable;
	uint8_t aux_power;
	uint8_t rf_power;
}adf_common_t;

extern adf_common_t adf_common;

void adf_write(uint32_t data);
void adf_init(void);
void adf_calc_freq(uint32_t f);
void adf_calc_and_set_freq(uint32_t f);
void adf_set_all(void);

#endif
