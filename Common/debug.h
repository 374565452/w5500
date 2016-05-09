#ifndef __DEBUG_H
#define __DEBUG_H
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"
#define debug_format  debug_out
void debug_out(char *fmt,...);


#endif