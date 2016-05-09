#ifndef __UTIL_H
#define __UTIL_H

#include "stm32f10x.h"

extern __IO uint32_t timing_delay;

void delay_ms(__IO uint32_t time);

void delay_ns(uint16_t delay);

#endif