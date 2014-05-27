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
 * @brief Accessing example the On-board Temperature Sensor in W5500EVB
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

//////////////////////////////////////
// ADC: On-board Temperature sensor //
//////////////////////////////////////
// TC1047A - Temperature to Voltage converter
// Vout = ((10mv/℃) * Temperature) + 500mV
// Temperature error range : +-0.2 ~ +-2.0

#define TEMP_SENSOR_CH				ADC_CH0		// ADC channel for on-board Temperature sensor (ADC channel / Pin shared with Potentiometer)
#define TEMP_SENSOR_ADC_PORT_NUM	0
#define TEMP_SENSOR_ADC_PIN_NUM		11
#define SUPPLY_VOLTAGE				3300		// Supply voltage mV for TC1047A (Temperature sensor)

#define ADC_READ_PERIOD_MS			200			// ms

static ADC_CLOCK_SETUP_T 			ADCSetup;
static bool ADC_read_enable = false;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void Init_ADC_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, TEMP_SENSOR_ADC_PORT_NUM, TEMP_SENSOR_ADC_PIN_NUM, FUNC2);
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
    int16_t calc_temp;

    /* Flag for running user's code  */
    bool run_user_applications = true;

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* ADC Init */
	Init_ADC_PinMux();
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, TEMP_SENSOR_CH, ENABLE);

#ifdef _MAIN_DEBUG_
    printf("\r\n=======================================\r\n");
	printf(" WIZnet W5500 EVB\r\n");
	printf(" On-board Temperature sensor demo example v%d.%.2d\r\n", VER_H, VER_L);
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
				while (Chip_ADC_ReadStatus(LPC_ADC, TEMP_SENSOR_CH, ADC_DR_DONE_STAT) != SET) {}

				/* Read ADC value */
				Chip_ADC_ReadValue(LPC_ADC, TEMP_SENSOR_CH, &dataADC);

				/* Calculate ADC value to Celsius temperature */
				calc_temp = (((dataADC * SUPPLY_VOLTAGE) / 1023) - 500) / 10;

				/* Print ADC value */
				printf("ADC value is 0x%x, ", dataADC);
				/* Print Celsius temperature */
				printf("Celsius temperature : %d C\r\n", calc_temp);
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
}
