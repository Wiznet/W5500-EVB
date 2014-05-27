/*
===============================================================================
 Name        : main.c
 Author      : WIZnet
 Version     :
 Copyright   : WIZnet Co., Ltd. 2014
 Description : main definition
===============================================================================
*/

/*
 * @brief DNS client example for W5500 EVB
 *
 * @note
 * Copyright(C) WIZnet Co., Ltd. 2014
 * All rights reserved.
 *
 */
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "w5500_spi.h"
#include "../../ioLibrary/Ethernet/wizchip_conf.h"
#include "../../ioLibrary/Internet/DNS/dns.h"


// TODO: insert other definitions and declarations here

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ1 (1000)		/* 1000 ticks per second, for SysTick */
#define TICKRATE_HZ2 (1)		/* 1 ticks per second, for Timer0 */
volatile uint32_t msTicks; 		/* counts 1ms timeTicks */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
///////////////////////////////////////
// Debugging Message Printout enable //
///////////////////////////////////////
#define _MAIN_DEBUG_

///////////////////////////
// Demo Firmware Version //
///////////////////////////
#define VER_H		1
#define VER_L		00

//////////////////////////////////////////////////
// Socket & Port number definition for Examples //
//////////////////////////////////////////////////
#define SOCK_DNS       6

////////////////////////////////////////////////
// Shared Buffer Definition for Loopback test //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];

///////////////////////////
// Network Configuration //
///////////////////////////

/* Well known free DNS Servers */
// Google public DNS (8.8.8.8 , 8.8.4.4)
// OpenDNS (208.67.222.222 , 208.67.220.220)

/*
 * It's the network information for test
 * Your board IP address SHOULD BE the public IP address to be accessible on Internet.
 */
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 1, 2},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 1, 1},
                            .dns = {8, 8, 8, 8},		// Google public DNS (8.8.8.8 , 8.8.4.4), OpenDNS (208.67.222.222 , 208.67.220.220)
                            .dhcp = NETINFO_STATIC };

/////////////////////
// PHYStatus check //
/////////////////////
#define SEC_PHYSTATUS_CHECK 		1		// sec
bool PHYStatus_check_enable = false;
bool PHYStatus_check_flag = true;

/////////////////
// LED Control //
/////////////////
static bool RGBLED_enable = true;

////////////////
// DNS client //
////////////////
uint8_t DNS_2nd[4]    = {208, 67, 222, 222};      	// Secondary DNS server IP
uint8_t Domain_name[] = "www.google.com";    		// for Example domain name
uint8_t Domain_IP[4]  = {0, };               		// Translated IP address by DNS Server

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void Net_Conf();
static void Display_Net_Conf();
static void PHYStatus_Check(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
void SysTick_Handler(void);
void Timer32_0_Init(uint32_t tickrate);
void TIMER32_0_IRQHandler(void);

/**
 * @brief    Main routine for W5500 EVB firmware
 * @return   Function should not exit.
 */
int main(void) {

#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
#endif
#endif

    bool run_user_applications = false;
    int32_t ret;

    /* Network Initialization */
	SPI_Init();
	W5500_Init();
	Net_Conf();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* Enable and setup 32-bit Timer 0 */
	Timer32_0_Init(TICKRATE_HZ2);

	/* Initialize buttons on the W5500 EVB board */
	Board_Buttons_Init();

	/* PHY Status check enable */
	PHYStatus_check_enable = true;

	/* Flag for RGB LED blink enable*/
	RGBLED_enable = true;

	/* Flag for running user's code  */
	run_user_applications = true;

#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};

	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    printf("\r\n=======================================\r\n");
	printf(" WIZnet %s EVB - DNS client v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	printf("=======================================\r\n");

	Display_Net_Conf(); // Print out the network information to serial terminal
#endif

#ifdef _MAIN_DEBUG_
	printf("\r\n===== DNS Servers =====\r\n");
	printf("> DNS 1st : %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
	printf("> DNS 2nd : %d.%d.%d.%d\r\n", DNS_2nd[0], DNS_2nd[1], DNS_2nd[2], DNS_2nd[3]);
	printf("=======================================\r\n");
	printf("> [Example] Target Domain Name : %s\r\n", Domain_name);
#endif

	/* DNS client Initialization */
    DNS_init(SOCK_DNS, gDATABUF);

    /* DNS processing */
    if ((ret = DNS_run(gWIZNETINFO.dns, Domain_name, Domain_IP)) > 0) // try to 1st DNS
    {
#ifdef _MAIN_DEBUG_
       printf("> 1st DNS Respond\r\n");
#endif
    }
    else if ((ret != -1) && ((ret = DNS_run(DNS_2nd, Domain_name, Domain_IP))>0))     // retry to 2nd DNS
    {
#ifdef _MAIN_DEBUG_
       printf("> 2nd DNS Respond\r\n");
#endif
    }
    else if(ret == -1)
    {
#ifdef _MAIN_DEBUG_
       printf("> MAX_DOMAIN_NAME is too small. Should be redefined it.\r\n");
#endif
       ;
    }
    else
    {
#ifdef _MAIN_DEBUG_
       printf("> DNS Failed\r\n");
#endif
       ;
    }

    if(ret > 0)
    {
#ifdef _MAIN_DEBUG_
       printf("> Translated %s to [%d.%d.%d.%d]\r\n",Domain_name,Domain_IP[0],Domain_IP[1],Domain_IP[2],Domain_IP[3]);
#endif
       //
       // TODO: To be executed User's code after a successful DNS process
       //
    }

	/* Main loop ***************************************/
	while(1)
	{
	   	/* PHY Status checker: Check every 'SEC_PHYSTATUS_CHECK' seconds */
		if(PHYStatus_check_flag)
		{
			PHYStatus_check_flag = false;
			PHYStatus_Check();
		}

    	// TODO: insert user's code here
    	if(run_user_applications)
    	{
    		;
    	} // End of user's code
	} // End of Main loop

    return 0;
}


/* Private functions *********************************************************/

static void Net_Conf()
{
	/* wizchip netconf */
	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
}

static void Display_Net_Conf()
{
#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};
#endif

	ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);

#ifdef _MAIN_DEBUG_
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);

	if(gWIZNETINFO.dhcp == NETINFO_DHCP) printf("\r\n===== %s NET CONF : DHCP =====\r\n",(char*)tmpstr);
		else printf("\r\n===== %s NET CONF : Static =====\r\n",(char*)tmpstr);
	printf(" MAC : %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	printf(" IP : %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	printf(" GW : %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	printf(" SN : %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	printf("=======================================\r\n");
#endif
}

static void PHYStatus_Check(void)
{
	uint8_t tmp;
	static bool LED_status_backup;

	LED_status_backup = RGBLED_enable;

	do
	{
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		// Error indicator: LED Green ON when no PHY link detected
		if(tmp == PHY_LINK_OFF)
		{
			RGBLED_enable = false;
			Board_LED_Set(2, true);
		}
	}while(tmp == PHY_LINK_OFF);

	RGBLED_enable = LED_status_backup;
}

/* Public functions **********************************************************/

/**
 * @brief    Handle interrupt from SysTick timer
 * @return    Nothing
 */
void SysTick_Handler(void)
{
	static uint16_t phystatus_check_cnt = 0;

	msTicks++; // increment counter

	// RGB LED Blinky
	if (msTicks % 100 == 0)
	{
		if (RGBLED_enable == true)
		{
			Board_LED_Set(0, false);
			Board_LED_Toggle(1);
			Board_LED_Set(2, false);
		}
		else
		{
			Board_LED_Set(0, false);
			Board_LED_Set(1, false);
			Board_LED_Set(2, false);
		}
	}

	// PHY status check counter
	if(PHYStatus_check_enable)
	{
		if (phystatus_check_cnt++ > (1000 * SEC_PHYSTATUS_CHECK))
		{
			PHYStatus_check_flag = true;
			phystatus_check_cnt = 0;
		}
	}
	else
	{
		phystatus_check_cnt = 0;
	}
}

/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
void TIMER32_0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
		////////////////////////////////////////////////////////
		// SHOULD BE Added DNS Timer Handler your 1s tick timer
		DNS_time_handler(); 	// for DNS timeout counter
		////////////////////////////////////////////////////////
	}
}

/**
 * @brief	Initialization 32-bit Timer 0
 * @return	Nothing
 */
void Timer32_0_Init(uint32_t tickrate)
{
	uint32_t timerFreq;

	/* Initialize 32-bit timer 0 clock */
	Chip_TIMER_Init(LPC_TIMER32_0);

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER32_0);
	/* Enable timer to generate interrupts when time matches */
	Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
	/* Setup 32-bit timer's duration (32-bit match time) */
	Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / tickrate));
	/* Setup timer to restart when match occurs */
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
	/* Start timer */
	Chip_TIMER_Enable(LPC_TIMER32_0);

	/* Clear timer of any pending interrupts */
	NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
	/* Enable timer interrupt */
	NVIC_EnableIRQ(TIMER_32_0_IRQn);
}
