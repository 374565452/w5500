#include "net.h"

//uint8 server_ip[4]={192,168,0,34};			// 配置远程服务器IP地址
uint8 server_ip[4]={49,68,217,54};
//uint16 server_port=5000;								// 配置远程服务器端口
uint16 server_port=7002;
uint16 local_port=6000;									// 初始化一个本地端口
uint16 net_len=0;


uint8 net_buffer[2048];												// 定义一个2KB的数组，用来存放Socket的通信数据

void net_config()
{
	Reset_W5500();
	//debug_out("ccccccccccc!\r\n"); 
	/***** W5500的IP信息初始化 *****/
	set_default(); 														// 设置默认MAC、IP、GW、SUB、DNS
	//debug_out("ddddddddddddddddd!\r\n"); 
	set_network();														// 配置初始化IP信息并打印，初始化8个Socket
	//debug_out("eeeeeeeeeeeeeeeeeee!\r\n"); 	
	/***** 打开W5500的Keepalive功能 *****/
	setkeepalive(0);
	
	debug_out("TCP Server IP: %d.%d.%d.%d \r\n",server_ip[0],server_ip[1],server_ip[2],server_ip[3]);
  debug_out("TCP Server Port: %d \r\n",server_port);	
	debug_out("W5500 Init Complete!\r\n");
  debug_out("Start TCP Client Test!\r\n"); 
}

void init_net()
{
	/***** GPIO、SPI初始化 *****/
	GPIO_Configuration();		
//debug_out("bbbbbbbbbbbbbbbbbbbb!\r\n"); 	
	WIZ_SPI_Init();
	//debug_out("aaaaaaaaaaaaa!\r\n"); 
	/***** 硬重启W5500 *****/
	//debug_out("aaaaaaaaaaaaaaaaaaaaa1212121212!\r\n"); 
	
	net_config();
	
}


void process_net()
{
	switch(getSn_SR(0))														// 获取socket0的状态
		{
			case SOCK_INIT:															// Socket处于初始化完成(打开)状态
				debug_out("socket init .......!\r\n"); 
					connect(0, server_ip,server_port);			// 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求
			break;
			case SOCK_ESTABLISHED:											// Socket处于连接建立状态
				//debug_out("socket connected.......!\r\n"); 
			//net_config();
					if(getSn_IR(0) & Sn_IR_CON)   					
					{
						setSn_IR(0, Sn_IR_CON);								// Sn_IR的CON位置1，通知W5500连接已建立
					}
					// 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器
					net_len=getSn_RX_RSR(0);										// len=Socket0接收缓存中已接收和保存的数据大小
					if(net_len>0)
					{
						recv(0,net_buffer,net_len);										// W5500接收来自服务器的数据，并通过SPI发送给MCU
						debug_out("%s\r\n",net_buffer);							// 串口打印接收到的数据
						send(0,net_buffer,net_len);										// 接收到数据后再回给服务器，完成数据回环
						//disconnect(0);
					}
					// W5500从串口发数据给客户端程序，数据需以回车结束
					//if(USART_RX_STA & 0x8000)								// 判断串口数据是否接收完成
					//{					   
						//len=USART_RX_STA & 0x3fff;						// 获取串口接收到数据的长度
						//send(0,USART_RX_BUF,len);							// W5500向客户端发送数据
						//USART_RX_STA=0;												// 串口接收状态标志位清0
						//memset(USART_RX_BUF,0,len+1);					// 串口接收缓存清0
					//}
			break;
			case SOCK_CLOSE_WAIT:												// Socket处于等待关闭状态
				debug_out("socket wait.......!\r\n"); 
					close(0);																// 关闭Socket0
			break;
			case SOCK_CLOSED:														// Socket处于关闭状态
				debug_out("socket  closed.......!\r\n"); 
					socket(0,Sn_MR_TCP,0,Sn_MR_ND);		// 打开Socket0，并配置为TCP无延时模式，打开一个本地端口
			break;
		}
}
