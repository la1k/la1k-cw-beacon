//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "platform.h"
#include <p18f26k22.h>

void platform_init(){
	// Oscillator setup
	OSCCONbits.IRCF = 0b111;	// 16MHz int osc.
	OSCCONbits.OSTS = 0;		// Internal osc.
	OSCCONbits.SCS = 0b10;		// Int. osc.
    OSCTUNEbits.PLLEN = 0;		// No PLL

	// Port setup
    TRISA   = 0b00000000;
    TRISB   = 0b00000000;
    TRISC   = 0b10000000;
	ANSELA = ANSELB = ANSELC = 0;

	// RB0-3 er GPIO
	// RC2/CCP1 er ADL_GAIN
	// UART_TX RC6, UART_RX RC7

	// SPI config
    SSP1CON1 = 0b00100010;	// SSPEN=1, SSPM=MASTER,FOSC64
    SSP1STAT = 0b01000000;   // Stigende flanke midt i databit!
}
