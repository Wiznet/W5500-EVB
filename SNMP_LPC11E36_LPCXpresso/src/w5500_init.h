
#ifndef __W5500_INIT_H__
#define __W5500_INIT_H__

#include "wizchip_conf.h"

void delay_cnt(volatile unsigned int nCount);
void Net_Conf(wiz_NetInfo netinfo);
void Display_Net_Conf();
void W5500_Init();

#endif
