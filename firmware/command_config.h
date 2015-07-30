//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef COMMAND_CONFIG_H
#define COMMAND_CONFIG_H

#include "config.h"

#include "adf.h"

///**** All system-specific describers and settings list - remember to end lists! ****/

const desc_uint32_t freq_describer = {34375, 4400000};
const uint8_t uint8_1 = 1;
const desc_uint8_t duty_describer = {10, 255};
const desc_uint8_t power_describer = {0, 3};
const desc_uint8_t ramp_time_describer = {10, 250};
//const uint32_t baud_describer[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, END_UINT32_LIST};
//const desc_uint32_t chg_vmax_desc = {5000, 15000};

const cmd_t cmd_list[] = {
// Name			Type				Set function		Value ptr.				Desc. ptr.
{"FREQ",		TYPE_UINT32,		&config_set_freq,	&config.freq,			&freq_describer},
{"ADF-POWER",	TYPE_UINT8,			&adf_set_all,		&adf_common.rf_power,	&power_describer},
{"ADL-DUTY",	TYPE_UINT8,			0,					&config.duty,			&duty_describer},
{"RAMP-TIME",	TYPE_UINT8,			0,					&config.ramp_time,		&ramp_time_describer},
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

