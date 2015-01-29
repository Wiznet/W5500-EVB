//****************************************************************************
//
//! \file board.c
//! \brief WIZnet W5500 EVB board file
//!
//! Copyright (c)  2014, WIZnet Co., LTD.
//! All rights reserved.
//
//****************************************************************************


#include "board.h"
#include "retarget.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define BUTTONS_BUTTON1_GPIO_PORT_NUM			0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM			4
#define BUTTONS_BUTTON2_GPIO_PORT_NUM			0
#define BUTTONS_BUTTON2_GPIO_BIT_NUM			5

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

const uint8_t dio_pins[DIOn] 	= {D0_PIN, D1_PIN, D2_PIN, D3_PIN,
								   D4_PIN, D5_PIN, D6_PIN, D7_PIN,
								   D8_PIN, D9_PIN, D10_PIN, D11_PIN,
								   D12_PIN, D13_PIN, D14_PIN, D15_PIN
								  };

const uint8_t dio_ports[DIOn] 	= {D0_GPIO_PORT, D1_GPIO_PORT, D2_GPIO_PORT, D3_GPIO_PORT,
								   D4_GPIO_PORT, D5_GPIO_PORT, D6_GPIO_PORT, D7_GPIO_PORT,
								   D8_GPIO_PORT, D9_GPIO_PORT, D10_GPIO_PORT, D11_GPIO_PORT,
								   D12_GPIO_PORT, D13_GPIO_PORT, D14_GPIO_PORT, D15_GPIO_PORT
								  };

const uint8_t ain_pins[AINn] 	= {A0_PIN, A1_PIN, A2_PIN, A3_PIN, A4_PIN, A5_PIN, AIN_PTM_TEMP};

const uint8_t adc_channels[AINn] 	= {ADC_CH6, ADC_CH1, ADC_CH2, ADC_CH3, ADC_CH5, ADC_CH7, ADC_CH0};

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	Chip_UART_SendBlocking(DEBUG_UART, &ch, 1);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	uint8_t data;

	if (Chip_UART_Read(DEBUG_UART, &data, 1) == 1) {
		return (int) data;
	}
#endif
	return EOF;
}

/* Gets a character from the UART on blocking mode, returns EOF if no character is ready */
int Board_UARTGetCharBlocking(void)
{
#if defined(DEBUG_UART)
	uint8_t data;

	if (Chip_UART_ReadBlocking(DEBUG_UART, &data, 1) == 1) {
		return (int) data;
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_18 used for RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT); /* PIO0_19 used for TXD */

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(LPC_USART);
	Chip_UART_SetBaud(LPC_USART, 115200);
	Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART);
#endif
}

#define MAXLEDS 3
static const uint8_t ledports[MAXLEDS] = {0, 1, 1};
static const uint8_t ledpins[MAXLEDS] = {21, 23, 24};

/* Initializes board LED(s) */
static void Board_LED_Init(void)
{
	int idx;

	for (idx = 0; idx < MAXLEDS; idx++) {
		/* Set the GPIO as output with initial state off (low) */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, ledports[idx], ledpins[idx]);
		Chip_GPIO_SetPinState(LPC_GPIO, ledports[idx], ledpins[idx], false);
	}
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < MAXLEDS) {
		/* Toggle state, low is on, high is off */
		Chip_GPIO_SetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber], !On);
	}
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	bool state = false;

	if (LEDNumber < MAXLEDS) {
		state = !Chip_GPIO_GetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}

	return state;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	if (LEDNumber < MAXLEDS) {
		Chip_GPIO_SetPinToggle(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]);
	}
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initialize GPIO */
	Chip_GPIO_Init(LPC_GPIO);

	/* Initialize LEDs */
	Board_LED_Init();

	/* LEDs Off */
	Board_LED_Set(0, false);
	Board_LED_Set(1, false);
	Board_LED_Set(2, false);
}

/* Initialize buttons on the board */
void Board_Buttons_Init(void)
{
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM);
}

/* Get button status */
uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	if (!Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM)) {
		ret |= BUTTONS_BUTTON1;
	}
	if (!Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM)) {
		ret |= BUTTONS_BUTTON2;
	}
	return ret;
}

