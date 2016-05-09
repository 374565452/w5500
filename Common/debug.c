#include "debug.h"



void debug_out(char *fmt,...)
{
	va_list ap;
	char prnt[800]="";
	//#if( DTU_DEBUG_TIME )
	//char msg[1024] = "";
	//#endif
	

	//#ifdef DTU_DEBUG_UART
	//debug_uart = DTU_DEBUG_UART;
	//#endif

	 va_start(ap,fmt);
   vsprintf(prnt,fmt,ap);
	
	//#if( DTU_DEBUG_TIME )
	 //sprintf(msg,"[%02d:%02d:%02d] %s",LPC_DateTime.Hour, LPC_DateTime.Minute, LPC_DateTime.Second, (char *)prnt);
	 //UART_SendData(debug_uart, (uint8_t *)msg, strlen( msg ));
	//#else
	 send_string(usart1, (uint8_t *)prnt, strlen( prnt ));
	//#endif

  va_end(ap);
}