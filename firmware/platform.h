//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef PLATFORM_H
#define PLATFORM_H

#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
//#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config WDTEN = OFF, LVP = OFF     // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#define _XTAL_FREQ 16000000
#define ADL_EN LATBbits.LATB4
#define ADF_EN LATCbits.LATC0
#define ADF_LE LATCbits.LATC1
#define ADF_CLK LATCbits.LATC3
#define LED	LATBbits.LATB0

void platform_init();

#endif
