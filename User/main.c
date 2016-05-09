/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#include "uart.h"
#include "rtc.h"
#include "gpio.h"
#include "debug.h"
#include "systick.h"
#include "net.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/
uint8_t buffer[1024];


extern Data_Time  timer;

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
uint16_t len;
uint16_t l=0;
int main(void)
{
	uint8_t t=0;	
  SystemInit();
	systick_init();

	init_usart(usart1);
	
	//init_rtc();
	//
	
	init_net();	
	len=0;
	//buffer[0]=1;
	//send(usart1,buffer,1);
  while (1)
  {
		process_net();
		//if(t!=timer.sec)
		if(rtc_flag==1)
		{
			rtc_flag=0;
			//time_get();
			//t=timer.sec;
			//debug_format("%d-----\r\n",l);
			//l++;
			//debug_format("Time: %0.4dÄê%0.2dÔÂ%0.2dÈÕ %0.2d:%0.2d:%0.2d\r\n",timer.w_year,timer.w_month,timer.w_date,timer.hour,timer.min,timer.sec);
		}
		//send(usart4,buffer,len);
		if(delay>30)
		{
			//time_show();
			len=receive(usart1,buffer);
			if(len>0)
			{
				//send(usart1,buffer,len);
				debug_format("the receive len is %d \r\n",len);
				len=0;
			}
			delay=0;
		}
  }
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
