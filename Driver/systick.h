#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

extern int16_t delay;

void systick_init(void);

#endif