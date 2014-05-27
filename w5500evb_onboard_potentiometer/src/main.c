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
 * @brief Accessing example the On-board Potentiometer in W5500EVB
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

// TODO: insert other definitions and declarations here

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ1 (1000)		/* 1000 ticks per second, for SysTick */
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

/////////////////
// LED Control //
/////////////////
static bool RGBLED_enable = true;

/////////////////////////////////
// ADC: On-board Potentiometer //
/////////////////////////////////

#define POTENTIOMETER_CH				ADC_CH0		// ADC channel for on-board Potentiometer (ADC channel / Pin shared with Temperature sensor)
#define POTENTIOMETER_ADC_PORT_NUM		0
#define POTENTIOMETER_ADC_PIN_NUM		11

#define ADC_READ_PERIOD_MS				10			// ms

static ADC_CLOCK_SETUP_T 				ADCSetup;
static bool ADC_read_enable = false;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void Init_ADC_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, POTENTIOMETER_ADC_PORT_NUM, POTENTIOMETER_ADC_PIN_NUM, FUNC2);
}


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

    uint16_t dataADC;
    uint16_t previous_dataADC = 0;

    /* Flag for running user's code  */
    bool run_user_applications = true;

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* ADC Init */
	Init_ADC_PinMux();
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, POTENTIOMETER_CH, ENABLE);

#ifdef _MAIN_DEBUG_
    printf("\r\n=======================================\r\n");
	printf(" WIZnet W5500 EVB\r\n");
	printf(" On-board Potentiometer demo example v%d.%.2d\r\n", VER_H, VER_L);
	printf("=======================================\r\n");
	printf(">> This example using ADC, SysTick\r\n");
	printf("=======================================\r\n");
#endif

	/* Main loop ***************************************/
	while(1)
	{
    	// TODO: insert user's code here
    	if(run_user_applications)
    	{
    		if(ADC_read_enable)
    		{
    			ADC_read_enable = false;

				/* Start A/D conversion */
    			Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

				/* Waiting for A/D conversion complete */
				while (Chip_ADC_ReadStatus(LPC_ADC, POTENTIOMETER_CH, ADC_DR_DONE_STAT) != SET) {}

				/* Read ADC value */
				Chip_ADC_ReadValue(LPC_ADC, POTENTIOMETER_CH, &dataADC);
    		}

    		//if(previous_dataADC != dataADC)
    		if((previous_dataADC/10) != (dataADC/10))
			{
				previous_dataADC = dataADC;
				/* Print ADC value */
				printf("Read Potentiometer value is %d\r\n", dataADC);
			}
    	} // End of user's code
	} // End of Main loop

    return 0;
}

/* Public functions **********************************************************/

/**
 * @brief    Handle interrupt from SysTick timer
 * @return    Nothing
 */
void SysTick_Handler(void)
{
	msTicks++; // tick counter increment

	if (msTicks % ADC_READ_PERIOD_MS == 0) ADC_read_enable = true;

	// RGB LED Blinky
	if (msTicks % 100 == 0)
	{
		if (RGBLED_enable == true)
		{
			Board_LED_Toggle(0);
			Board_LED_Set(1, false);
			Board_LED_Set(2, false);
		}
		else
		{
			Board_LED_Set(0, false);
			Board_LED_Set(1, false);
			Board_LED_Set(2, false);
		}
	}
}
