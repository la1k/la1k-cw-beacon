//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "timing.h"

#include <stdint.h>
#include <plib.h>

void timer_init(void){
	// Prescaler 32. 16 MHz / 4 / 32 = 125000 Hz.
	// (Largest prescaler giving a integer relationship with 1 millisecond.)
	T0CONbits.TMR0ON = 0;
	T0CONbits.T08BIT = 0;
	T0CONbits.T0CS = 0;
	T0CONbits.T0SE = 0;
	T0CONbits.PSA = 0;
	T0CONbits.T0PS = 0b100; // 1:32
	T0CONbits.TMR0ON = 1;
}

void pwm_init(void){
	// Set up T2
	PR2 = 63;	// 4*(31+1) = 128, 125 KHz
	T2CONbits.T2OUTPS = 0;	// 1:1 postscale
	T2CONbits.T2CKPS = 0;	// 1:1 prescale
	T2CONbits.TMR2ON = 1;

	// Set up CCP
	CCP1CONbits.CCP1M = 0b1100; // P1A active high, P1B active low
	CCP1CONbits.P1M = 0b00; // Single output
}

void pwm_set(uint8_t level){
	CCPR1L = (level >> 2);
	CCP1CONbits.DC1B = level & 0b11;
}


