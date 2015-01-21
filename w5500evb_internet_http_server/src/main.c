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
 * @brief HTTP Server example (Web Server using W5500)
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
#include "wizchip_conf.h"

#include "httpServer.h"
#include "webpage.h"
#include "adcHandler.h"

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

#define MAX_HTTPSOCK	6
uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};

////////////////////////////////////////////////
// Shared Buffer Definition  				  //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];

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
static void Net_Conf();
static void Display_Net_Conf();
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

    bool run_user_applications = false;
    uint8_t i;
    uint8_t ain_sel = 0; // Bit for Analog input selection

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

	/* Initialize ADCs */
	//ain_sel |= 0x01;	// A0
	//ain_sel |= 0x02;	// A1
	//ain_sel |= 0x04;	// A2
	//ain_sel |= 0x08;	// A3
	//ain_sel |= 0x10;	// A4
	//ain_sel |= 0x20;	// A5
	ain_sel |= 0x40;	// ADC channel enable for Potentiometer / Temp.Sensor

	ADC_Init(ain_sel);

	/* PHY Status check enable */
	PHYStatus_check_enable = true;

	/* RGB LED blink enable*/
	RGBLED_enable = false;
	Board_LED_Set(0, true);

	/* Flag for running user's code  */
	run_user_applications = true;

#ifdef _MAIN_DEBUG_
	uint8_t tmpstr[6] = {0,};

	ctlwizchip(CW_GET_ID,(void*)tmpstr);

    printf("\r\n=======================================\r\n");
	printf(" WIZnet %s EVB Demos v%d.%.2d\r\n", tmpstr, VER_H, VER_L);
	printf("=======================================\r\n");
	printf(">> W5500 based Web Server Example\r\n");
	printf("=======================================\r\n");

	Display_Net_Conf(); // Print out the network information to serial terminal
#endif

	/* HTTP Server Initialization  */
	httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);		// Tx/Rx buffers (2kB) / The number of W5500 chip H/W sockets in use
	reg_httpServer_cbfunc(NVIC_SystemReset, NULL); 					// Callback: NXP MCU Reset

	{
		/* Web content registration (web content in webpage.h, Example web pages) */

		// Index page and netinfo / base64 image demo
		reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
		reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
		reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)wiz550web_netinfo_js);	// netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
		reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page

		// Example #1
		reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
		reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)wiz550web_dio_js);			// dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)

		// Example #2
		reg_httpServer_webContent((uint8_t *)"ain.html", (uint8_t *)ain_page);					// ain.html 		: Analog input monitor example page
		reg_httpServer_webContent((uint8_t *)"ain.js", (uint8_t *)wiz550web_ain_js);			// ain.js 			: JavaScript for Analog input monitor	(+ ajax.js)

		// Example #3
		reg_httpServer_webContent((uint8_t *)"ain_gauge.html", (uint8_t *)ain_gauge_page);		// ain_gauge.html 	: Analog input monitor example page; using Google Gauge chart
		reg_httpServer_webContent((uint8_t *)"ain_gauge.js", (uint8_t *)ain_gauge_js);			// ain_gauge.js 	: JavaScript for Google Gauge chart		(+ ajax.js)

		// AJAX JavaScript functions
		reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)wiz550web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer

#ifdef _MAIN_DEBUG_
		display_reg_webContent_list();
#endif
	}

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
    		for(i = 0; i < MAX_HTTPSOCK; i++)	httpServer_run(i); 	// HTTP Server handler

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
		// SHOULD BE Added HTTP Server Time Handler to your 1s tick timer
		httpServer_time_handler(); 	// for HTTP server time counter
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
