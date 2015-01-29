/*
 * device.c
 *
 *  Created on: 2014. 3. 20.
 *      Author: Raphael
 */

#include <string.h>
#include "board.h"
#include "eepromHandler.h"

#define BLOCK_SIZE	512

uint8_t read_eeprom(uint8_t isConfig, void *data, uint16_t size)
{
	uint32_t command[5], result[4], address;

	if(isConfig == 1) {
		address = BLOCK_SIZE;
	}
	else {
		address = 0x00000040;
	}

	/* Data to be read from EEPROM */
	command[0] = IAP_EEPROM_READ;
	command[1] = address;	// EEPROM_ADDRESS
	command[2] = (uint32_t) data;
	command[3] = size;
	command[4] = SystemCoreClock / 1000;
	iap_entry(command, result);

	/* Error checking */
	if (result[0] == IAP_CMD_SUCCESS) {
		return 1;
	}

	return 0;
}

uint8_t write_eeprom(uint8_t isConfig, void *data, uint16_t size)
{
	uint32_t command[5], result[4], address;

	if(isConfig == 1) {
		address = BLOCK_SIZE;
	}
	else {
		address = 0x00000040;
	}

	/* Data to be written to EEPROM */
	command[0] = IAP_EEPROM_WRITE;
	command[1] = address;	// EEPROM_ADDRESS
	command[2] = (uint32_t) data;
	command[3] = size;
	command[4] = SystemCoreClock / 1000;
	iap_entry(command, result);

	/* Error checking */
	if (result[0] == IAP_CMD_SUCCESS) {
		return 1;
	}

	return 0;
}
