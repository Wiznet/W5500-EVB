//****************************************************************************
//
//! \file board.h
//! \brief WIZnet W5500 EVB board file
//!
//! Copyright (c)  2014, WIZnet Co., LTD.
//! All rights reserved.
//
//****************************************************************************

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
	out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
	semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
	Board_UARTPutSTR functions.
 */
#define DEBUG_UART LPC_USART

/**
 * @}
 */

/* Board name */
#define BOARD_WIZNET_W5500_EVB

/**
 * Button defines
 */
#define BUTTONS_BUTTON1     0x01
#define BUTTONS_BUTTON2     0x02
#define NO_BUTTON_PRESSED   0x00

#define DIOn			16
#define AINn			7 	// AIN for PTM/Temp.Sensor + A0 ~ A5
#define LEDn            3

// LED
typedef enum
{
	LED_R = 0,
	LED_G = 1,
	LED_B = 2
} Led_TypeDef;

// ADC
typedef enum
{
	A0 = 0,
	A1 = 1,
	A2 = 2,
	A3 = 3,
	A4 = 4,
	A5 = 5,
	AIN = 6
} ADC_Type;

//Data IO
typedef enum
{
	D0 = 0,
	D1 = 1,
	D2 = 2,
	D3 = 3,
	D4 = 4,
	D5 = 5,
	D6 = 6,
	D7 = 7,
	D8 = 8,
	D9 = 9,
	D10 = 10,
	D11 = 11,
	D12 = 12,
	D13 = 13,
	D14 = 14,
	D15 = 15
} IO_Type;

// Data IO Direction
typedef enum
{
	NotUsed	= 0,
	Input	= 1,
	Output	= 2
} IO_Direction_Type;

// Data IO Status
typedef enum
{
	Off	= 0,
	On 	= 1
} IO_Status_Type;

// Analog Inputs
#define A0_PIN							22
#define A1_PIN							12
#define A2_PIN							13
#define A3_PIN							14	// Shared Pin with D4_PIN
#define A4_PIN							16	// Shared Pin with D5_PIN
#define A5_PIN							23	// Shared Pin with D6_PIN
#define AIN_PTM_TEMP					11	// On-board ADC for Potentiometer / Temperature sensor (shared, select by the on-board switch)

#define ADC_PORT						0

// Digital I/O
#define D0_PIN							18	// NotUSED: UART RX Pin for serial printout
#define D0_GPIO_PORT					0

#define D1_PIN							19	// NotUSED: UART TX Pin for serial printout
#define D1_GPIO_PORT					0

#define D2_PIN							15
#define D2_GPIO_PORT					1

#define D3_PIN							17
#define D3_GPIO_PORT					0

#define D4_PIN							14	// Shared Pin with A3_PIN
#define D4_GPIO_PORT					0

#define D5_PIN							16	// Shared Pin with A4_PIN
#define D5_GPIO_PORT					0

#define D6_PIN							23	// Shared Pin with A5_PIN
#define D6_GPIO_PORT					0

#define D7_PIN							20
#define D7_GPIO_PORT					0

#define D8_PIN							21	// RGBLED: LEDR
#define D8_GPIO_PORT					0

#define D9_PIN							23	// RGBLED: LEDG
#define D9_GPIO_PORT					1

#define D10_PIN							24	// RGBLED: LEDB
#define D10_GPIO_PORT					1

#define D11_PIN							9	// NotUSED: SPI MOSI Pin for W5500
#define D11_GPIO_PORT					0

#define D12_PIN							8	// NotUSED: SPI MISO Pin for W5500
#define D12_GPIO_PORT					0

#define D13_PIN							6	// NotUSED: SPI SCK Pin for W5500
#define D13_GPIO_PORT					0

#define D14_PIN							5	// On-board SW2
#define D14_GPIO_PORT					0

#define D15_PIN							4	// On-board SW1
#define D15_GPIO_PORT					0

extern const uint8_t dio_pins[];
extern const uint8_t dio_ports[];
extern const uint8_t ain_pins[];
extern const uint8_t adc_channels[];

/**
 * @brief	Initialize buttons on the board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint32_t Buttons_GetStatus(void);


/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
