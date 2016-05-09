#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"


#define BUFFER_128BYTE  128ul
#define BUFFER_256BYTE  256ul
#define BUFFER_512BYTE  512ul

/***7bit 128byte***/
typedef union
{
	struct
	{
		unsigned  index   : 7 ;
	}bit;
	uint16_t max ;
}buf_128_t;

/***8bit 256byte***/
typedef union
{
	struct
	{
		unsigned  index   : 8 ;
	}bit;
	uint16_t max ;
}buf_256_t;

/***9bit 512byte***/
typedef union
{
	struct
	{
		unsigned  index   : 9 ;
	}bit;
	uint16_t max ;
}buf_512_t;

/***��������***/
typedef struct
{
	uint8_t     *data; //����
	buf_128_t   rx;  //����
	buf_128_t   tx;  //����
	uint16_t    buf_len_max; //��󳤶�
}usart_buf_128;

typedef struct
{
	uint8_t     *data; //����
	buf_256_t   rx;  //����
	buf_256_t   tx;  //����
	uint16_t    buf_len_max; //��󳤶�
}usart_buf_256;

typedef struct
{
	uint8_t     *data; //����
	buf_512_t   rx;  //����
	buf_512_t   tx;  //����
	uint16_t    buf_len_max; //��󳤶�
}usart_buf_512;

typedef enum
{
	usart1,    //gprs
	usart2,    //ip
	usart3,     //pc
	usart4,
	usart5
}usart_channel;
//���ڳ�ʼ��
void init_usart(usart_channel e);
//�Ӵ��ڶ����ݣ��ŵ�buffer�������С����ض�ȡ���ַ���
int receive(usart_channel e ,uint8_t * buffer);
//�򴮿ڷ�������
void send_uart(usart_channel e,uint8_t * buffer,uint16_t len);

void send_string(usart_channel e ,char * buffer,uint16_t len);

void Debug_Out(char *fmt,...);

#endif