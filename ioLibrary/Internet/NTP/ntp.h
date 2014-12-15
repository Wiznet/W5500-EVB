//*****************************************************************************
//
//! \file ntp.h
//! \brief NTP APIs Header file.
//! \details Send NTP request & Receive NTP response.
//! \version 1.0.0
//! \date 2014/05/29
//! \par  Revision history
//!       <2014/05/29> 1st Release
//!
//! \author Eric Jung
//! \copyright
//!
//! Copyright (c)  2014, WIZnet Co., Ltd.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef	_NTP_H
#define	_NTP_H

#include <stdint.h>

/*
 * @brief Define it for Debug & Monitor NTP processing.
 * @note If defined, it depends on <stdio.h>
 */

//#define _NTP_DEBUG_

/* type for NTP client */
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;

typedef struct _ntpformat
{
   
        uint8_t  dstaddr[4];      /* destination (local) address */
        char    version;        /* version number */
        char    leap;           /* leap indicator */
        char    mode;           /* mode */
        char    stratum;        /* stratum */
        char    poll;           /* poll interval */
        s_char  precision;      /* precision */
        tdist   rootdelay;      /* root delay */
        tdist   rootdisp;       /* root dispersion */
        char    refid;          /* reference ID */
        tstamp  reftime;        /* reference time */
        tstamp  org;            /* origin timestamp */
        tstamp  rec;            /* receive timestamp */
        tstamp  xmt;            /* transmit timestamp */
      

} ntpformat;

typedef struct _DATE_TIME
{
  uint8_t year[2];
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
}DATE_TIME;

typedef struct _NTP_TIMEINFO
{ 
  // NTP information structure
  int8_t gmt_hour;            	// -12 ~ +13
  uint8_t gmt_min;            	// 0 ~ 59
  uint8_t time_zone;
  uint8_t ntp_flag;           	// NTP flag indicates NTP client is used or not
  DATE_TIME date;           	// Manual input date/time when NTP is not available
}NTP_TIMEINFO;

//------------------------------ User changeable NTP Settings ------------------------------
#define UTC_HOUR                    9   // SEOUL : GMT+9
#define UTC_MINUTE                  0	// 0 or 30

#define MAX_NTP_RETRY       		3   // Maximum NTP retry count
#define NTP_WAIT_TIME       		2   // Delay second after NTP failed
//------------------------------------------------------------------------------------------

#define	NTP_REQUEST_SIZE			48						// Size of NTP request message
#define MAX_NTP_MSG_SIZE			512						// Max size of NTP response message

#define PORT_NTP                	123                     // NTP server port number
#define SECS_PERDAY     	    	86400UL             	// Seconds in a day = 60*60*24
#define EPOCH                   	1900                    // NTP start year

#define NTP_SUCCESS					1
#define NTP_FAILED					0

/*
 * @brief NTP client initialization (outside of the main loop)
 * @param s   		- socket number
 * @param buf 		- buffer for processing DHCP message
 * @param ntp_info 	- NTP client information structure
 */
void NTP_init(uint8_t s, uint8_t * buf, NTP_TIMEINFO  * ntp_info);

/*
 * @brief NTP process
 * @details Send NTP message and receive response(timestamp), the timestamp changes to the date/time info.
 * @param ip_timeserver - IP address of NTP Time server
 * @param datetime   	- Received date / time Information
 * @param s   - socket number
 * @return    The value is as the follow \n
 *            @ref NTP_FAILED     \n
 *            @ref NTP_SUCCESS    \n
 *
 * @note This function is called by main task when needed.
 */
uint8_t NTP_run(uint8_t * server_ip, NTP_TIMEINFO * ntp_info);

/*
 * @brief NTP 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler
 */
void NTP_time_handler(void);

#endif //_NTP_H
