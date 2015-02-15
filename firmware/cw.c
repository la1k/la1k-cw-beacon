//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include "cw.h"

#include <xc.h>			// delays
#include "string.h"
#include "platform.h"	// XTAL_FREQ
#include "adf.h"
#include "timing.h"

#define DUTY_MAX 220
#define RISE_TIME 200 // 50us = 11ms

void cw_on(){
    // Enable synth output
	adf_common.rf_enable = 1;
	adf_set_all();
	LED = 1;

    // Ramp up power
	uint8_t duty = 0;
	while (duty < DUTY_MAX){
		pwm_set(duty);
		duty+=1;
		__delay_us(RISE_TIME);
	}
}

void cw_off(){
    // Ramp down power
	uint8_t duty = DUTY_MAX;
	while (duty > 0){
		pwm_set(duty);
		duty-=1;
		__delay_us(RISE_TIME);
	}
	LED = 0;

    // Disable synth output
	adf_common.rf_enable = 0;
	adf_set_all();
}

#define SIGNAL_DIT 0x4000
#define SIGNAL_DAH 0xc000
#define CW_UNIT_DELAY() \
__delay_ms(25); \
__delay_ms(25); \
__delay_ms(25); \
__delay_ms(25);

// Each symbol uses two bits (11 = dah, 01 = dit, 00 = stop)
uint16_t cw_table[] = {
0b1101110111110000, // 33 ! – · – · – –
0b0111010111010000, // 34 "
0,					// 35
0b0101011101011100, // 36 $
0,					// 37
0b0111010101000000, // 38 &
0b0111111111010000, // 39 '
0b1101111101000000, // 40 (
0b1101111101110000, // 41 )
0,					// 42
0b0111011101000000, // 43 +
0b1111010111110000, // 44 ,
0b1101010101110000, // 45 -
0b0111011101110000, // 46 .
0b1101011101000000, // 47 /
0b1111111111000000, // 48 0 -->
0b0111111111000000,
0b0101111111000000,
0b0101011111000000,
0b0101010111000000,
0b0101010101000000,
0b1101010101000000,
0b1111010101000000,
0b1111110101000000,
0b1111111101000000, // 57 9
0b1111110101010000, // 58 :
0b1101110111010000, // 59 ;
0,					// 60
0b1101010111000000, // 61 =
0,					// 62
0b0101111101010000, // 63 ?
0b0111110111010000, // 64 @
0b0111000000000000, // A 65 -->
0b1101010100000000,
0b1101110100000000,
0b1101010000000000,
0b0100000000000000,
0b0101110100000000,
0b1111010000000000,
0b0101010100000000,
0b0101000000000000,
0b0111111100000000,
0b1101110000000000,
0b0111010100000000,
0b1111000000000000,
0b1101000000000000,
0b1111110000000000,
0b0111110100000000,
0b1111011100000000,
0b0111010000000000,
0b0101010000000000,
0b1100000000000000,
0b0101110000000000,
0b0101011100000000,
0b0111110000000000,
0b1101011100000000,
0b1101111100000000,
0b1111010000000000, // Z 90
0,
0,
0,
0,
0b0101111101110000 //95 _
};

void cw_pack(char* s){
    uint16_t s_nr;
	char c;
	uint16_t letter, signal; // Letter is the CW-pattern, signal is dit or dah
	const uint16_t mask = 0xc000;

    for (s_nr=0; s[s_nr]; s_nr++){
		c = s[s_nr];
		if (c == ' '){ // If space - only valid for single spaces
			CW_UNIT_DELAY();
			CW_UNIT_DELAY();
			CW_UNIT_DELAY();
			CW_UNIT_DELAY();
			continue;
		}
		if ( (c >= 97) && (c <= 122) ) c -= 32;	// Convert lower case to upper case
		if ( (c >= 33) && (c <= 95) ) letter = cw_table[c-33];
		if (letter){
			while(1){
				signal = letter & 0xc000;
				if (!signal) break;
				cw_on();
				CW_UNIT_DELAY();
				if (signal == SIGNAL_DAH){
					CW_UNIT_DELAY();
					CW_UNIT_DELAY();
				}
				cw_off();
				CW_UNIT_DELAY();
				letter <<= 2;
			}
			CW_UNIT_DELAY();
			CW_UNIT_DELAY();
		}
	}
}

uint8_t cw_nb_tick(cw_nb_t* cw){
	uint16_t signal;
	if ((!cw->on_time) && (!cw->off_time)){	// If current signal is complete

		// If no signal and still at first character, load first pattern
		if ( (!signal) && (cw->s == cw->s_data) ){
			if ( (*(cw->s) >= 97) && (*(cw->s) <= 122) ) cw->pattern = cw_table[*(cw->s)-65];	// Lower case
			else if ( (*(cw->s) >= 33) && (*(cw->s) <= 95) ) cw->pattern = cw_table[*(cw->s)-33];	// Upper case and other characters
		}

		// Load new signal
		signal = cw->pattern & 0xc000;

		// If end of signal and not end of string
		if ( (!signal) && (*(cw->s)) ){	
			(cw->s)++;
			if (*(cw->s) == ' '){			// If a space is reached, load off_time
				cw->off_time = 6;
			}
			else{
				if ( (*(cw->s) >= 97) && (*(cw->s) <= 122) ) cw->pattern = cw_table[*(cw->s)-65];	// Lower case
				else if ( (*(cw->s) >= 33) && (*(cw->s) <= 95) ) cw->pattern = cw_table[*(cw->s)-33];	// Upper case and other characters
				cw->off_time = 2;
			}
		}

		if (signal == SIGNAL_DIT){
			cw->on_time = 1;
			cw->off_time = 1;
			cw->pattern <<= 2;
		}
		else if (signal == SIGNAL_DAH){
			cw->on_time = 3;
			cw->off_time = 1;
			cw->pattern <<= 2;
			
		}

		// If end of string, load carrier time and re-init string
		if (!(*(cw->s))){
			cw->on_time = cw->carrier_time;
			strcpy(cw->s_data, cw->s_original);
			cw->s = cw->s_data;
		}
	}

	if(cw->on_time){
		if (!cw->state){
			
			cw->state = 1;
			cw_on();
		}
		cw->on_time--;
	}
	else if (cw->off_time){
		if (cw->state){
			
			cw->state = 0;
			cw_off();
		}
		cw->off_time--;
	}

	return 0;
}

void cw_nb_init(char* s_input, cw_nb_t* cw){
	cw->on_time = 0;
	cw->off_time = 0;
	cw->pattern = 0;
	cw->state = 0;
	cw->s_original = s_input;
	cw_off();

	// Copy string given as argument to own CW-buffer (double buffering)
	strcpy(cw->s_data, cw->s_original);
	cw->s = cw->s_data;
}
