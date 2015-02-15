//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
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

int main(int argc, char** argv) {
	platform_init();
	pwm_init();
	uart_init(38400);
	timer_init();

	// Initialize main settings
	strcpy(config.cw_string, " LA2VHF JP53 ");
	strcpy(config.term_end, "\r\n");
	config.freq = 70062;
	cw.carrier_time = 50;

	// Initialize non-blocking morse code generator
	cw_nb_init(config.cw_string, &cw);

    // Initialize RF hardware
	ADL_EN = 1;
	ADF_EN = 1;
	ADF_LE = 0;
	__delay_ms(10);
	adf_common.aux_enable = 0;
	adf_common.rf_enable = 1;
	config_set_freq();	// Sets all ADF-settings
	
    // Enable interrupts
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

	sch_task_init(SCH_INDEX_CW, 100, &cw_task);		// 300 ms
	sch_task_enable(SCH_INDEX_CW);

	uart_puts("LA1K CW-beacon HW 0.1, SW 0.1");

	adf_set_all();

	while(1){
		while (uart_inwaiting()) cmd_byte(uart_get());
		sch_tick();
		CLRWDT();
	}

	return 0;
}
