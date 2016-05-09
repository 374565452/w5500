#include "gpio.h"

void init_gpio()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_4;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 	 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
  GPIO_Init(GPIOC, &GPIO_InitStructure);           
}

void set_gpio_high()
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
}
void set_gpio_low()
{
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
}