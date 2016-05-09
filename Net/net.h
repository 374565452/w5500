#ifndef __NET_H
#define __NET_H

#include "device.h"
#include "debug.h"
#include "stm32f10x.h"
#include "types.h"
#include "spi2.h"
#include "w5500.h"
#include "socket.h"

void init_net();

void process_net();

#endif