#ifndef __RTC_H
#define __RTC_H
#include "stm32f10x.h"

typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;			
	//公历日月年周
	uint16_t w_year;
	uint8_t  w_month;
	uint8_t  w_date;
	uint8_t  week;
}Data_Time;

extern uint8_t rtc_flag;

uint8_t init_rtc(void);
uint8_t is_leap_year(uint16_t year);
uint8_t time_update(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec);
uint8_t time_get(void);
uint8_t rtc_get_week(uint16_t year,uint8_t month,uint8_t day);

void time_set(void);


#endif