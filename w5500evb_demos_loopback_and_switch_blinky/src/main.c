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
 * @brief Network Loopback Test and Blinky example using sysTick
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
#include "../../ioLibrary/Appmod/Loopback/loopback.h"


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
#define SOCK_TCPS       0
#define SOCK_UDPS       1
#define PORT_TCPS		5000
#define PORT_UDPS       3000

////////////////////////////////////////////////
// Shared Buffer Definition for Loopback test //
////////////////////////////////////////////////
//#define DATA_BUF_SIZE   2048 	// defined in loopback.h
uint8_t gDATABUF[DATA_BUF_SIZE];

///////////////////////////
// Network Configuration //
///////////////////////////
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 1, 2},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0, 0, 0, 0},
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
#define BUTTONS_PRESSED_TICKS		10		// ms
bool button1_enable = false;
bool button1_pressed_flag = false;


/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void Net_Conf();
static void Display_Net_Conf();
static void PHYStatus_Check(void);
static uint8_t Check_Buttons_Pressed(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
void SysTick_Handler(void);

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

	/* Initialize buttons on the W5500 EVB board */
	Board_Buttons_Init();

	/* PHY Status check enable */
	PHYStatus_check_enable = true;

	/* RGB LED blink enable*/
	RGBLED_enable = true;

	/* Flag for running user's code  */
	run_user_applications = true;

#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};

	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    printf("\r\n=======================================\r\n");
	printf(" WIZnet %s EVB Demos v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	printf("=======================================\r\n");
	printf(">> TCP server and UDP loopback test\r\n");
	printf(">> LED Blinky using SysTick: [SW1] On/Off\r\n");
	printf("=======================================\r\n");

	Display_Net_Conf(); // Print out the network information to serial terminal
#endif

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
			//printf("SW1 ");
			printf("%s\r\n", RGBLED_enable ? "On" : "Off");
#endif
		}

    	// TODO: insert user's code here
    	if(run_user_applications)
    	{
    		/* Loopback test : TCP Server / UDP */
    		if ((ret = loopback_tcps(SOCK_TCPS, gDATABUF, PORT_TCPS)) < 0) // TCP server loopback test
    		{
#ifdef _MAIN_DEBUG_
    			printf("SOCKET ERROR : %ld\r\n", ret);
#endif
    		}

			if ((ret = loopback_udps(SOCK_UDPS, gDATABUF, PORT_UDPS)) < 0) // UDP loopback test
			{
#ifdef _MAIN_DEBUG_
				printf("SOCKET ERROR : %ld\r\n", ret);
#endif
			}
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
//////////////////////////////////////////////////////
/*
				case 3:
					Board_LED_Set(0, true);
					Board_LED_Set(1, true);
					Board_LED_Set(2, false);
					break;

				case 4:
					Board_LED_Set(0, true);
					Board_LED_Set(1, false);
					Board_LED_Set(2, true);
					break;

				case 5:
					Board_LED_Set(0, false);
					Board_LED_Set(1, true);
					Board_LED_Set(2, true);
					break;

				case 6:
					Board_LED_Set(0, true);
					Board_LED_Set(1, true);
					Board_LED_Set(2, true);
					break;
*/
/////////////////////////////////////////////////////
				default:
					led_state = 0;
					break;
			}

			led_state++;
			if (led_state > 2) led_state = 0;
			//if (led_state > 6) led_state = 0;
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
