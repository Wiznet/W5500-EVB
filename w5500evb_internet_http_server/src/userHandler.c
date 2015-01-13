/**
 * @file	userHandler.c
 * @brief	User Control Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "httpUtil.h"
#include "userHandler.h"
#include "adcHandler.h"
#include "wizchip_conf.h"

/* Unavailable Pins  (W5500-EVB component preempted) */
// >> UART Rx/Tx 		: D0 (Rx), D1 (Tx)
// >> W5500 SPI(SPI0)	: D11 (MOSI), D12 (MISO), D13 (SCK)

/* On-board Devices */
// >> Input		: D14 (SW1) / D15 (SW2)
// >> Input		: AIN (Potentiometer / TEMP.Sensor)
// >> Output	: D8 (LED R) / D9 (LED G) / D10 (LED B)

/* NXP LPC11Exx GPIO functions */
// GPIO: Pin state
//Chip_GPIO_GetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin]);
//Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], true);
//Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], false);

// GPIO: Pin direction
//Chip_GPIO_GetPinDIR((LPC_GPIO, dio_ports[pin], dio_pins[pin]);
//Chip_GPIO_SetPinDIROutput(LPC_GPIO, dio_ports[pin], dio_pins[pin]);
//Chip_GPIO_SetPinDIRInput(LPC_GPIO, dio_ports[pin], dio_pins[pin]);

// Pre-defined Get CGI functions
void make_json_dio(uint8_t * buf, uint16_t * len, uint8_t pin);
void make_json_ain(uint8_t * buf, uint16_t * len, uint8_t pin);
void make_json_netinfo(uint8_t * buf, uint16_t * len);

// Pre-defined Set CGI functions
int8_t set_diodir(uint8_t * uri);
int8_t set_diostate(uint8_t * uri);

uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = 1 means 'uri_name' matched
	uint8_t cgibuf[14] = {0, };
	int8_t cgi_dio = -1;
	int8_t cgi_ain = -1;

	uint8_t i;

	if(strcmp((const char *)uri_name, "todo.cgi") == 0)
	{
		// to do
		;//make_json_todo(buf, len);
	}
	else if(strcmp((const char *)uri_name, "get_netinfo.cgi") == 0)
	{
		make_json_netinfo(buf, len);
	}
	else
	{
		// get_dio0.cgi ~ get_dio15.cgi
		for(i = 0; i < DIOn; i++)
		{
			memset(cgibuf, 0x00, 14);
			sprintf((char *)cgibuf, "get_dio%d.cgi", i);
			if(strcmp((const char *)uri_name, (const char *)cgibuf) == 0)
			{
				make_json_dio(buf, len, i);
				cgi_dio = i;
				break;
			}
		}

		if(cgi_dio < 0)
		{
			// get_ain0.cgi ~ get_ain5.cgi (A0 - A5), get_ain6.cgi for on-board potentiometer / Temp.Sensor
			for(i = 0; i < AINn; i++)
			{
				memset(cgibuf, 0x00, 14);
				sprintf((char *)cgibuf, "get_ain%d.cgi", i);
				if(strcmp((const char *)uri_name, (const char *)cgibuf) == 0)
				{
					make_json_ain(buf, len, i);
					cgi_ain = i;
					break;
				}
			}
		}

		if((cgi_dio < 0) && (cgi_ain < 0)) ret = 0;
	}

	return ret;
}


uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = '1' means 'uri_name' matched
	uint8_t val = 0;

	if(strcmp((const char *)uri_name, "todo.cgi") == 0)
	{
		// to do
		;//val = todo(uri);
		//*len = sprintf((char *)buf, "%d", val);
	}
	// Digital I/O; dio_s, dio_d
	else if(strcmp((const char *)uri_name, "set_diodir.cgi") == 0)
	{
		val = set_diodir(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else if(strcmp((const char *)uri_name, "set_diostate.cgi") == 0)
	{
		val = set_diostate(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		ret = 0;
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Get CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_json_dio(uint8_t * buf, uint16_t * len, uint8_t pin)
{
	uint8_t pin_state 	= Chip_GPIO_GetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin]);
	uint8_t pin_dir 	= Chip_GPIO_GetPinDIR(LPC_GPIO, dio_ports[pin], dio_pins[pin]);

	*len = sprintf((char *)buf, "DioCallback({\"dio_p\":\"%d\",\
											\"dio_s\":\"%d\",\
											\"dio_d\":\"%d\"\
											});",
											pin,					// Digital io pin number
											pin_state,				// Digital io status
											pin_dir					// Digital io directions
											);
}

void make_json_ain(uint8_t * buf, uint16_t * len, uint8_t pin)
{
	*len = sprintf((char *)buf, "AinCallback({\"ain_p\":\"%d\",\
											\"ain_v\":\"%d\"\
											});",
											pin,					// ADC input pin number
											get_ADC_val(pin)		// ADC input value
											);
}

void make_json_netinfo(uint8_t * buf, uint16_t * len)
{
	wiz_NetInfo netinfo;
	ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);

	// DHCP: 1 - Static, 2 - DHCP
	*len = sprintf((char *)buf, "NetinfoCallback({\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\
											\"ip\":\"%d.%d.%d.%d\",\
											\"gw\":\"%d.%d.%d.%d\",\
											\"sn\":\"%d.%d.%d.%d\",\
											\"dns\":\"%d.%d.%d.%d\",\
											\"dhcp\":\"%d\"\
											});",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.dhcp
											);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Set CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int8_t set_diodir(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	if((param = get_http_param_value((char *)uri, "pin"))) // GPIO; D0 ~ D15
	{
		pin = (uint8_t)ATOI(param, 10);
		if(pin > 15) return -1;

		if((param = get_http_param_value((char *)uri, "val")))  // Direction; NotUsed/Input/Output
		{
			val = (uint8_t)ATOI(param, 10);
			if(val > Output) val = Output;
		}
	}

	if(val == Input) 		Chip_GPIO_SetPinDIRInput(LPC_GPIO, dio_ports[pin], dio_pins[pin]);	// Input
	else 					Chip_GPIO_SetPinDIROutput(LPC_GPIO, dio_ports[pin], dio_pins[pin]); // Output

	return pin;
}

int8_t set_diostate(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	if((param = get_http_param_value((char *)uri, "pin"))) // GPIO; D0 ~ D15
	{
		pin = (uint8_t)ATOI(param, 10);
		if(pin > 15) return -1;

		if((param = get_http_param_value((char *)uri, "val")))  // State; high(on)/low(off)
		{
			val = (uint8_t)ATOI(param, 10);
			if(val > On) val = On;
		}

		if(val == On) 		Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], true); 	// High
		else				Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], false);	// Low
	}

	return pin;
}
