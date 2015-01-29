#ifndef _SNMP_CUSTOM_H_
#define _SNMP_CUSTOM_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "snmp.h"

extern dataEntryType snmpData[];
extern const int32_t maxData;



#define COMMUNITY					"public\0"
#define COMMUNITY_SIZE				(strlen(COMMUNITY))

/* Predefined function: Response value control */
void initTable();

/* User defined functions: LED control examples */
void get_LEDStatus_all(void *ptr, uint8_t *len);
void set_LEDStatus_R(int32_t val);
void set_LEDStatus_G(int32_t val);
void set_LEDStatus_B(int32_t val);

#endif
