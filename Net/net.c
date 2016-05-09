#include "net.h"

//uint8 server_ip[4]={192,168,0,34};			// ����Զ�̷�����IP��ַ
uint8 server_ip[4]={49,68,217,54};
//uint16 server_port=5000;								// ����Զ�̷������˿�
uint16 server_port=7002;
uint16 local_port=6000;									// ��ʼ��һ�����ض˿�
uint16 net_len=0;


uint8 net_buffer[2048];												// ����һ��2KB�����飬�������Socket��ͨ������

void net_config()
{
	Reset_W5500();
	//debug_out("ccccccccccc!\r\n"); 
	/***** W5500��IP��Ϣ��ʼ�� *****/
	set_default(); 														// ����Ĭ��MAC��IP��GW��SUB��DNS
	//debug_out("ddddddddddddddddd!\r\n"); 
	set_network();														// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
	//debug_out("eeeeeeeeeeeeeeeeeee!\r\n"); 	
	/***** ��W5500��Keepalive���� *****/
	setkeepalive(0);
	
	debug_out("TCP Server IP: %d.%d.%d.%d \r\n",server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
  debug_out("TCP Server Port: %d \r\n",server_port);	
	debug_out("W5500 Init Complete!\r\n");
  debug_out("Start TCP Client Test!\r\n"); 
}

void init_net()
{
	/***** GPIO��SPI��ʼ�� *****/
	GPIO_Configuration();		
//debug_out("bbbbbbbbbbbbbbbbbbbb!\r\n"); 	
	WIZ_SPI_Init();
	//debug_out("aaaaaaaaaaaaa!\r\n"); 
	/***** Ӳ����W5500 *****/
	//debug_out("aaaaaaaaaaaaaaaaaaaaa1212121212!\r\n"); 
	
	net_config();
	
}


void process_net()
{
	switch(getSn_SR(0))														// ��ȡsocket0��״̬
		{
			case SOCK_INIT:															// Socket���ڳ�ʼ�����(��)״̬
				debug_out("socket init .......!\r\n"); 
					connect(0, server_ip,server_port);			// ����Sn_CRΪCONNECT������TCP������������������
			break;
			case SOCK_ESTABLISHED:											// Socket�������ӽ���״̬
				//debug_out("socket connected.......!\r\n"); 
			//net_config();
					if(getSn_IR(0) & Sn_IR_CON)   					
					{
						setSn_IR(0, Sn_IR_CON);								// Sn_IR��CONλ��1��֪ͨW5500�����ѽ���
					}
					// ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ�������
					net_len=getSn_RX_RSR(0);										// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
					if(net_len>0)
					{
						recv(0,net_buffer,net_len);										// W5500�������Է����������ݣ���ͨ��SPI���͸�MCU
						debug_out("%s\r\n",net_buffer);							// ���ڴ�ӡ���յ�������
						send(0,net_buffer,net_len);										// ���յ����ݺ��ٻظ���������������ݻػ�
						//disconnect(0);
					}
					// W5500�Ӵ��ڷ����ݸ��ͻ��˳����������Իس�����
					//if(USART_RX_STA & 0x8000)								// �жϴ��������Ƿ�������
					//{					   
						//len=USART_RX_STA & 0x3fff;						// ��ȡ���ڽ��յ����ݵĳ���
						//send(0,USART_RX_BUF,len);							// W5500��ͻ��˷�������
						//USART_RX_STA=0;												// ���ڽ���״̬��־λ��0
						//memset(USART_RX_BUF,0,len+1);					// ���ڽ��ջ�����0
					//}
			break;
			case SOCK_CLOSE_WAIT:												// Socket���ڵȴ��ر�״̬
				debug_out("socket wait.......!\r\n"); 
					close(0);																// �ر�Socket0
			break;
			case SOCK_CLOSED:														// Socket���ڹر�״̬
				debug_out("socket  closed.......!\r\n"); 
					socket(0,Sn_MR_TCP,0,Sn_MR_ND);		// ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿�
			break;
		}
}
