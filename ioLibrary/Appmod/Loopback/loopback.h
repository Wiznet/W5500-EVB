
#ifndef  _WIZCHIP_LOOPBACK_H_
#define  _WIZCHIP_LOOPBACK_H_

#define _LOOPBACK_DEBUG_

#define DATA_BUF_SIZE   2048

int32_t loopback_tcps(uint8_t, uint8_t*, uint16_t);
int32_t loopback_udps(uint8_t, uint8_t*, uint16_t);

#endif   // _WIZCHIP_LOOPBACK_H_
