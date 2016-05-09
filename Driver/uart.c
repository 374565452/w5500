#include "uart.h"
#include "gpio.h"
#define usart_ring_buf usart_buf_512

#define BUFFER_RING BUFFER_512BYTE

static uint8_t usart1_ring_data[BUFFER_RING];
usart_ring_buf usart1_ring_buf;

static uint8_t usart2_ring_data[BUFFER_RING];
usart_ring_buf usart2_ring_buf;

static uint8_t usart3_ring_data[BUFFER_RING];
usart_ring_buf usart3_ring_buf;

//static uint8_t usart4_ring_data[BUFFER_RING];
//usart_ring_buf usart4_ring_buf;


static int usart_read(usart_ring_buf * buf,uint8_t * buffer)
{
	uint16_t len=0;
	if(buf->tx.bit.index==buf->rx.bit.index)
	{
		return 0;
	}
	while(buf->tx.bit.index !=buf->rx.bit.index)
	{
		buffer[len]=buf->data[buf->tx.bit.index];
		buf->tx.bit.index++;
		len++;
	}
	return len;
}

static void usart_send(USART_TypeDef* USARTx,uint8_t dat)
{
	while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));	
	USART_SendData(USARTx,dat);

}
static void usart_receive(USART_TypeDef * USARTx,usart_ring_buf * buf)
{
	//char c;
		//RxBuffer[RxCounter++] = USART_ReceiveData(USART3);   
		//c = (char)(USART_ReceiveData(USART3)&0xff);
		//usart_send(USART3,c);
		//for(i=0; i< RxCounter; i++) TxBuffer[i]	= RxBuffer[i]; 	    
		//rec_f=1;
		//RxCounter=0;								
	uint8_t byte=0x00;
	if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)	   
	{	
			byte = (USART_ReceiveData(USARTx)&0xff);
			if((buf->rx.bit.index+1)!=(buf->tx.bit.index))
			{
				buf->data[buf->rx.bit.index]=byte;
				
			}	
			buf->rx.bit.index++;
	}
	if(USART_GetITStatus(USARTx, USART_IT_TXE) != RESET)                  
	{ 
	 USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);					    
	}	
//	if(USARTx->SR & 0x0F)   
//	{  
//		byte=USARTx->DR;
//		if((buf->rx.bit.index+1)!=(buf->tx.bit.index))
//		{
//			buf->data[buf->rx.bit.index]=byte;
//			buf->rx.bit.index++;
//		}
//	}  
//	else if(USARTx->SR & USART_FLAG_RXNE) //Receive Data Reg Full Flag  
//	{    
//    byte=USARTx->DR;
//		if((buf->rx.bit.index+1)!=(buf->tx.bit.index))
//		{
//			buf->data[buf->rx.bit.index]=byte;
//			buf->rx.bit.index++;
//		}
//	}  
}
static void init_ex(usart_channel e)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	switch(e)
	{
		case usart1:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);
			//串口的IO初始化
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
			GPIO_Init(GPIOA,&GPIO_InitStruct); //TX	

			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
			GPIO_Init(GPIOA,&GPIO_InitStruct); //RX	
			break;
		}
		case usart2:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);
			//串口的IO初始化
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
			GPIO_Init(GPIOA,&GPIO_InitStruct); //TX	

			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
			GPIO_Init(GPIOA,&GPIO_InitStruct); //RX	
			break;
		}
		case usart3:
		{
			
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  ,ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);
			//串口的IO初始化
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
			GPIO_Init(GPIOB,&GPIO_InitStruct); //TX	

			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
			GPIO_Init(GPIOB,&GPIO_InitStruct); //RX	
			break;
		}
		case usart4:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC ,ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 ,ENABLE);
			//串口的IO初始化
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
			GPIO_Init(GPIOC,&GPIO_InitStruct); //TX	

			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
			GPIO_Init(GPIOC,&GPIO_InitStruct); //RX	
			break;
		}
		case usart5:
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD  ,ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 ,ENABLE);
			//串口的IO初始化
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
			GPIO_Init(GPIOC,&GPIO_InitStruct); //TX	

			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
			GPIO_Init(GPIOD,&GPIO_InitStruct); //RX	
			break;
		}
	}
	
	//串口初始
	USART_InitStruct.USART_BaudRate  = 38400;
	USART_InitStruct.USART_WordLength  = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits  = USART_StopBits_1;
	USART_InitStruct.USART_Parity  = USART_Parity_No;
	USART_InitStruct.USART_Mode  = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	switch(e)
	{
		case usart1:
		{
			USART_ClearFlag(USART1, USART_IT_RXNE);
			USART_Init(USART1,&USART_InitStruct);
			//使能接收中断
			USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
			NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStruct);
			USART_Cmd(USART1,ENABLE);
			break;
		}
		case usart2:
		{
			USART_ClearFlag(USART2, USART_IT_RXNE);
			USART_Init(USART2,&USART_InitStruct);
			//使能接收中断
			USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
			NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStruct);
			USART_Cmd(USART2,ENABLE);
			break;
		}
		case usart3:
		{
			//set_gpio_high();
			USART_ClearFlag(USART3, USART_IT_RXNE);
			USART_Init(USART3,&USART_InitStruct);
			//使能接收中断
			USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
			NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStruct);
			USART_Cmd(USART3,ENABLE);
			break;
		}
		case usart4:
		{
			USART_ClearFlag(UART4, USART_IT_RXNE);
			USART_Init(UART4,&USART_InitStruct);
			//使能接收中断
			USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
			NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStruct);
			USART_Cmd(UART4,ENABLE);
			break;
		}
		case usart5:
		{
			USART_ClearFlag(UART5, USART_IT_RXNE);
			USART_Init(UART5,&USART_InitStruct);
			//使能接收中断
			USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
			NVIC_InitStruct.NVIC_IRQChannel = UART5_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStruct);
			USART_Cmd(UART5,ENABLE);
		}
	}
}

void init_usart(usart_channel e)
{
	usart_ring_buf * buf;
	switch(e)
	{
		case usart1:
		{
			buf=&usart1_ring_buf;
			buf->data=usart1_ring_data;
			break;
		}
		case usart2:
		{
			buf=&usart2_ring_buf;
			buf->data=usart2_ring_data;
			break;
		}
		case usart3:
		{
			
			buf=&usart3_ring_buf;
			buf->data=usart3_ring_data;
			break;
		}
		case usart4:
		{
			//buf=&usart4_ring_buf;
			//buf->data=usart4_ring_data;
			break;
		}
		case usart5:
		{
			break;
		}
	}
	buf->rx.bit.index=0;
	buf->tx.bit.index=0;
	buf->buf_len_max=1+buf->rx.max;
	init_ex(e);
}



void USART1_IRQHandler(void)
{
	usart_receive(USART1,&usart1_ring_buf);
}

void USART2_IRQHandler(void)
{
	usart_receive(USART2,&usart2_ring_buf);
}

void USART3_IRQHandler(void)
{
	//set_gpio_high();
	usart_receive(USART3,&usart3_ring_buf);
	//char c;
		//RxBuffer[RxCounter++] = USART_ReceiveData(USART3);   
		//c = (char)(USART_ReceiveData(USART3)&0xff);
		//usart_send(USART3,c);
}
void UART4_IRQHandler(void)
{
	//set_gpio_high();
	//usart_receive(UART4,&usart4_ring_buf);
}
void UART5_IRQHandler(void)
{
	
}

void send_uart(usart_channel e,uint8_t * buffer,uint16_t len)
{
	USART_TypeDef *usart;
	uint16_t i=0;
	switch(e)
	{
		case usart1:
		{
			usart=USART1;
			break;
		}
		case usart2:
		{
			usart=USART2;
			break;
		}
		case usart3:
		{
			usart=USART3;
			break;
		}
		case usart4:
		{
			usart=UART4;
			break;
		}
		case usart5:
		{
			break;
		}
	}
	for(i=0;i<len;i++)
	{
		usart_send(usart,buffer[i]);
	}
}
void send_string(usart_channel e ,char * buffer,uint16_t len)
{
	USART_TypeDef *usart;
	uint16_t i=0;
	switch(e)
	{
		case usart1:
		{
			usart=USART1;
			break;
		}
		case usart2:
		{
			usart=USART2;
			break;
		}
		case usart3:
		{
			usart=USART3;
			break;
		}
		case usart4:
		{
			usart=UART4;
			break;
		}
		case usart5:
		{
			break;
		}
	}
	for(i=0;i<len;i++)
	{
		usart_send(usart,buffer[i]);
	}
}

int receive(usart_channel e ,uint8_t * buffer)
{
	switch(e)
	{
		case usart1:
		{
			return usart_read(&usart1_ring_buf,buffer);
			break;
		}
		case usart2:
		{
			return usart_read(&usart2_ring_buf,buffer);
			break;
		}
		case usart3:
		{
			return usart_read(&usart3_ring_buf,buffer);
			break;
		}
		case usart4:
		{
			//usart_read(&usart4_ring_buf,buffer);
			break;
		}
		case usart5:
		{
			break;
		}
	}
}


