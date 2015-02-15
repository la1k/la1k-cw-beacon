//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#include <stdint.h>

#include "config.h"
#include "adf.h"

config_t config;

unsigned int cmd_buf_len;
uint8_t cmd_buf[CMD_BUF_SIZE];

void config_set_freq(){
	adf_calc_freq(config.freq);
	adf_set_all();
}
