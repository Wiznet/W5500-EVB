//****************************************************************************
//
//! \file w5500_spi.h
//! \brief WIZnet W5500 EVB spi interface file
//!
//! Copyright (c)  2014, WIZnet Co., LTD.
//! All rights reserved.
//
//****************************************************************************

#ifndef _W5500_SPI_H
#define _W5500_SPI_H

#include "../../ioLibrary/Ethernet/wizchip_conf.h"

void Init_SSP_PinMux(void);
void  wizchip_select(void);
void  wizchip_deselect(void);
uint8_t wizchip_read();
void  wizchip_write(uint8_t wb);
void SPI_Init();

void W5500_Init();

#endif
