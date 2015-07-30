//**************************************************************//
// LA1K CW beacon - Jon Petter Skagmo, LA3JPA, 2013.            //
// Licensed under LGPL.                                         //
//**************************************************************//

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCH_INDEX_CW 0
#define SCH_INDEX_BUTTON 1
#define SCH_INDEX_PWM 2

#include <stdint.h>

uint8_t sch_task_init(uint8_t index, uint32_t interval, void (*task)(void));
uint8_t sch_task_enable(uint8_t index);
uint8_t sch_task_disable(uint8_t index);
void sch_tick(void);

#endif
