//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef TIMING_H
#define	TIMING_H

#include <stdint.h>

void timer_init(void);
void pwm_init(void);
void pwm_set(uint8_t level);

#endif

