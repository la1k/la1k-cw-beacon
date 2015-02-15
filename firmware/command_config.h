//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

#include "config.h"

///**** All system-specific describers and settings list - remember to end lists! ****/

const desc_uint32_t freq_describer = {34375, 4400000};
const desc_uint8_t duty_describer = {10, 255};
//const uint32_t baud_describer[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, END_UINT32_LIST};
////const char* profile_describer[] = {"TNC2_AFSK", "TNC2_GFSK", END_ENUM};
//const desc_uint32_t chg_vmax_desc = {5000, 15000};

const cmd_t cmd_list[] = {
// Name			Type				Set function		Value ptr.				Desc. ptr.
{"FREQ",		TYPE_UINT32,		&config_set_freq,	&config.freq,			&freq_describer},
{"ADL-DUTY",	TYPE_UINT8,			0,					&config.duty,			&duty_describer},
{"HELP",		TYPE_HELP,			0,					0,						0},
{"SAVE",		TYPE_UNDEF,			0,					0,						0},
{"LOAD",		TYPE_UNDEF,			0,					0,						0},
//{"CTIME"
//{"TEXT"
//{"REF-DBL
//{"ADF-PWR
{"",			0,					0,					0,						0}
};

#endif

