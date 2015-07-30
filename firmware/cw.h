//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef CW_H
#define CW_H

#include <stdint.h>

typedef struct{
	uint8_t on_time;
	uint8_t off_time;
	char* s;
	char s_data[100];
	char* s_original;
	uint16_t pattern;
	uint8_t state;
	uint8_t carrier_time;
}cw_nb_t;


void cw_pack(char* s);
void cw_on(void);
void cw_off(void);
uint8_t cw_nb_tick(cw_nb_t* cw);
void cw_nb_init(char* s, cw_nb_t* cw);

#endif
