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
 * @brief SNMPv1 example
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
#include "w5500_init.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "spi_handler.h"
#include "snmp.h"

#include "loopback.h"

// TODO: insert other definitions and declarations here

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ1 (1000)		/* 1 ticks per millisecond, for SysTick */
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
#define SOCK_TCPS       0
#define SOCK_UDPS       1
#define PORT_TCPS		5000
#define PORT_UDPS       3000

#define SOCK_SNMP_AGENT	2
#define SOCK_SNMP_TRAP	3

////////////////////////////////////////////////
// Shared Buffer Definition  				  //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
uint8_t DATA_BUF[DATA_BUF_SIZE];

///////////////////////////
// Network Configuration //
///////////////////////////
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 0, 112},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 0, 1},
                            .dns = {8, 8, 8, 8},
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
static bool RGBLED_enable = false;

////////////////////
// Button Control //
////////////////////
#define BUTTONS_PRESSED_TICKS		10		// msec
bool button1_enable = false;
bool button1_pressed_flag = false;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
//static void Net_Conf();
//static void Display_Net_Conf();
static void PHYStatus_Check(void);
static uint8_t Check_Buttons_Pressed(void);

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

    //uint8_t i;
    bool run_user_applications = false;

    /* Network Initialization */
	SPI_Init();
	W5500_Init();
	Net_Conf(gWIZNETINFO);

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* Enable and setup 32-bit Timer 0 */
	Timer32_0_Init(TICKRATE_HZ2);

	/* Initialize buttons on the W5500 EVB board */
	Board_Buttons_Init();

	/* PHY Status check enable */
	PHYStatus_check_enable = true;

	/* RGB LED blink enable*/
	RGBLED_enable = false;
	Board_LED_Set(0, true); // Board operation indicator

	/* Flag for running user's code  */
	run_user_applications = true;

#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};

	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    printf("\r\n=======================================\r\n");
	printf(" WIZnet %s EVB Demos v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	printf("=======================================\r\n");
	printf(">> W5500 based SNMPv1 Agent Example\r\n");
	printf("=======================================\r\n");

	Display_Net_Conf(); // Print out the network information to serial terminal
#endif

	/* SNMP(Simple Network Management Protocol) Agent Initialize */
	uint8_t manager_ip[4] = {192, 168, 0, 214}; // NMS (SNMP manager) IP address
	snmpd_init(manager_ip, gWIZNETINFO.ip, SOCK_SNMP_AGENT, SOCK_SNMP_TRAP);

	/* Main loop ***************************************/
	while(1)
	{
	   	/* PHY Status checker: [PHY status] checked every 'SEC_PHYSTATUS_CHECK' seconds */
		if(PHYStatus_check_flag)
		{
			PHYStatus_check_flag = false;
			PHYStatus_Check();
		}

	   	/* Button: SW1 */
		if(Check_Buttons_Pressed() == BUTTONS_BUTTON1)
		{
			// RGB LED blink On/Off
			RGBLED_enable = !RGBLED_enable;
#ifdef _MAIN_DEBUG_
			printf("%s%s\r\n", "LED Blinky ", RGBLED_enable ? "On" : "Off");
#endif
			if(!RGBLED_enable)
			{
				Board_LED_Set(0, false);
				Board_LED_Set(1, false);
				Board_LED_Set(2, false);
			}
		}

    	// TODO: insert user's code here
    	if(run_user_applications)
    	{
    		/* Loopback Test: TCP Server and UDP */
    		// Test for Ethernet data transfer validation
    		{
    			//loopback_tcps(SOCK_TCPS, DATA_BUF, PORT_TCPS);
    			//loopback_udps(SOCK_UDPS, DATA_BUF, PORT_UDPS);
    		}

    		/* SNMP Agent Handler */
    		// SNMP Agent daemon process : User can add the OID and OID mapped functions to snmpData[] array in snmprun.c/.h
			// [net-snmp version 5.7 package for windows] is used for this demo.
			// * Command example
    		// [Command] Get:			snmpget -v 1 -c public 192.168.0.112 .1.3.6.1.2.1.1.1.0 			// (sysDescr)
    		// [Command] Get: 			snmpget -v 1 -c public 192.168.0.112 .1.3.6.1.4.1.6.1.0 			// (Custom, get LED status)
    		// [Command] Get-Next: 		snmpwalk -v 1 -c public 192.168.0.112 .1.3.6.1
			// [Command] Set: 			snmpset -v 1 -c public 192.168.0.112 .1.3.6.1.4.1.6.2.0 i 1			// (Custom, LED 'On')
    		// [Command] Set: 			snmpset -v 1 -c public 192.168.0.112 .1.3.6.1.4.1.6.2.0 i 0			// (Custom, LED 'Off')
    		snmpd_run();

    	} // End of user's code
	} // End of Main loop

    return 0;
}


/* Private functions *********************************************************/

#if 0
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
#endif

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

static uint8_t Check_Buttons_Pressed(void)
{
	static uint8_t buttons_status;
	static uint8_t ret;

	buttons_status = Buttons_GetStatus();

	if((buttons_status & BUTTONS_BUTTON1) == BUTTONS_BUTTON1) button1_enable = true; // button pressed check timer enable
	else button1_enable = false;

	if(button1_pressed_flag)	// button1 pressed (Specified time elapsed, enabled by sysTick_Handler function)
	{
		button1_pressed_flag = false; // pressed button clear
		ret = BUTTONS_BUTTON1; // return pressed button status
	}
	else
	{
		ret = 0;
	}

	return ret;
}


/* Public functions **********************************************************/

/**
 * @brief    Handle interrupt from SysTick timer
 * @return    Nothing
 */
void SysTick_Handler(void)
{

	static uint16_t phystatus_check_cnt = 0;
	static uint16_t button1_pressed_check_cnt = 0;
	static uint8_t led_state = 0;
	static bool button1_press_detected = false;

	msTicks++; // increment counter

	//SNMP_time_handler();	// SNMP time counter (which is called every 1 milliseconds)
	if (msTicks % 10 == 0) SNMP_time_handler(); // SNMP time counter (which is called every 10 milliseconds)

	// Button1 control
	if(button1_enable == true)
	{
		if(!button1_press_detected)
		{
			button1_pressed_check_cnt++;
			if(button1_pressed_check_cnt >= BUTTONS_PRESSED_TICKS)
			{
				button1_pressed_flag = true;
				button1_pressed_check_cnt = 0;
				button1_enable = false;

				button1_press_detected = true;
			}
		}
	}
	else
	{
		button1_pressed_check_cnt = 0;
		button1_press_detected = false;
	}

	// RGB LED Blinky
	if (msTicks % 100 == 0)
	{
		if (RGBLED_enable == true)
		{
			switch (led_state)
			{
				case 0:
					Board_LED_Set(0, true);
					Board_LED_Set(1, false);
					Board_LED_Set(2, false);
					break;

				case 1:
					Board_LED_Set(0, false);
					Board_LED_Set(1, true);
					Board_LED_Set(2, false);
					break;

				case 2:
					Board_LED_Set(0, false);
					Board_LED_Set(1, false);
					Board_LED_Set(2, true);
					break;

				default:
					led_state = 0;
					break;
			}

			led_state++;
			if (led_state > 2) led_state = 0;
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
		// To do: added - 1sec time counter (if you need)
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
