/*
 * device.h
 *
 *  Created on: 2014. 3. 20.
 *      Author: Raphael
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <stdint.h>

uint8_t read_eeprom(uint8_t isConfig, void *data, uint16_t size);
uint8_t write_eeprom(uint8_t isConfig, void *data, uint16_t size);

#endif /* DEVICE_H_ */
