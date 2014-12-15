//****************************************************************************
//
//! \file w5500_spi.c
//! \brief WIZnet W5500 EVB spi interface file
//!
//! Copyright (c)  2014, WIZnet Co., LTD.
//! All rights reserved.
//
//****************************************************************************

#include "w5500_spi.h"


/**
  * @brief  Initializes the peripherals used by the W5500 driver.
  * @param  None
  * @retval None
  */

/* Set pin mux for SSP operation */
void Init_SSP_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 6, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));	// SCK
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));	// MISO
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));	// MOSI

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0,  2);	// SSEL(CS) as GPIO output

	//Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 22);	// N_RESET as GPIO output
}

void  wizchip_select(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 2, false);	// SSEL(CS)
}

void  wizchip_deselect(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 2, true);	// SSEL(CS)
}

uint8_t wizchip_read()
{
	uint8_t rb;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP0, &rb, 1);
	return rb;
}

void  wizchip_write(uint8_t wb)
{
	Chip_SSP_WriteFrames_Blocking(LPC_SSP0, &wb, 1);
}

void SPI_Init()
{
	/* SSP initialization */
	Init_SSP_PinMux();
	Chip_SSP_Init(LPC_SSP0);

	Chip_SSP_SetFormat(LPC_SSP0, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE0);
	Chip_SSP_SetMaster(LPC_SSP0, true);
	Chip_SSP_SetBitRate(LPC_SSP0, 10000000); // 10MHz
	Chip_SSP_Enable(LPC_SSP0);
}

void W5500_Init()
{
	uint8_t tmp;
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	Chip_GPIO_SetPinState(LPC_GPIO, 0, 2, true);	// SSEL(CS)

	Chip_GPIO_SetPinState(LPC_GPIO, 0, 22, false);	// N_RESET
	tmp = 0xFF;
	while(tmp--);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 22, true);	// N_RESET

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

	/* wizchip initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized fail.\r\n");
		while (1);
	}
/*
	do {
		if (ctlwizchip(CW_GET_PHYLINK, (void*) &tmp) == -1)
			printf("Unknown PHY Link stauts.\r\n");
	} while (tmp == PHY_LINK_OFF);
*/
}
