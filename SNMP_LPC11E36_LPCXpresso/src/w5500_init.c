#include <stdio.h>

#include "board.h"
#include "spi_handler.h"
#include "socket.h"

/* W5500 Call Back Functions */
static void  wizchip_select(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, GPIO_SPI0_PORT, GPIO_SPI0_CS0, false);	// SSEL(CS)
}

static void  wizchip_deselect(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, GPIO_SPI0_PORT, GPIO_SPI0_CS0, true);	// SSEL(CS)
}

static uint8_t wizchip_read()
{
	uint8_t rb;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP0, &rb, 1);
	return rb;
}

static void  wizchip_write(uint8_t wb)
{
	Chip_SSP_WriteFrames_Blocking(LPC_SSP0, &wb, 1);
}

static void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	Chip_SSP_ReadFrames_Blocking(LPC_SSP0, pBuf, len);
}

static void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	Chip_SSP_WriteFrames_Blocking(LPC_SSP0, pBuf, len);
}

void delay_cnt(volatile unsigned int nCount)
{
	for(; nCount!= 0;nCount--);
}

void Display_Net_Conf()
{
	uint8_t tmpstr[6] = {0,};
	wiz_NetInfo gWIZNETINFO;

	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
	ctlwizchip(CW_GET_ID,(void*)tmpstr);

	// Display Network Information
	if(gWIZNETINFO.dhcp == NETINFO_DHCP) printf("\r\n===== %s NET CONF : DHCP =====\r\n",(char*)tmpstr);
		else printf("\r\n===== %s NET CONF : Static =====\r\n",(char*)tmpstr);

	printf("\r\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	printf("IP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	printf("GW: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	printf("SN: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
}

void Net_Conf(wiz_NetInfo netinfo)
{
	/*
	wiz_NetInfo gWIZNETINFO = {
		{ 0x00, 0x08, 0xDC, 0x44, 0x55, 0x66 },				// Mac address
		{ 192, 168, 1, 91 },								// IP address
		{ 255, 255, 255, 0},								// Subnet mask
		{ 192, 168, 1, 1},									// Gateway
		{ 8, 8, 8, 8},										// DNS Server
	};
	*/
	ctlnetwork(CN_SET_NETINFO, (void*) &netinfo);

	//display_net_info();
}

void W5500_Init()
{
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	/* W5500 Chip Reset */
	Chip_GPIO_SetPinState(LPC_GPIO, GPIO_W5500_RST_PORT, GPIO_W5500_RST, false);
	delay_cnt(5000);
	Chip_GPIO_SetPinState(LPC_GPIO, GPIO_W5500_RST_PORT, GPIO_W5500_RST, true);
	delay_cnt(10000);

	/* Register Call back function */
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);

	/* W5500 Chip Initialization */
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized fail.\r\n");
		while (1);
	}
}
