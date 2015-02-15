//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef CONFIG_H
#define	CONFIG_H

#include "stdint.h"

typedef struct{
	char cw_string[100];
	uint32_t freq;
	uint8_t duty;

	char term_end[3];
}config_t;

extern config_t config;

#define CMD_BUF_SIZE 64
extern unsigned int cmd_buf_len;
extern uint8_t cmd_buf[CMD_BUF_SIZE];

void config_set_freq();

#endif
