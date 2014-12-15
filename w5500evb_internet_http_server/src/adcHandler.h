/**
 * @file	adcHandler.h
 * @brief	Header File for ADC handling Example
 * @version 1.0
 * @date	2014/12/11
 * @par Revision
 *			2014/12/11 - 1.0 Release
 * @author
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __ADCHANDLER_H
#define __ADCHANDLER_H

void ADC_Init(uint8_t adc_sel);
uint16_t get_ADC_val(uint8_t ain_idx);

#endif
