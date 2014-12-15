//*****************************************************************************
//
//! \file ntp.c
//! \brief NTP APIs Implement file.
//! \details Send NTP request & Receive NTP response.
//!          It depends on stdlib.h & string.h in ansi-c library
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
#include <string.h>

#include "ntp.h"
#include "../../Ethernet/socket.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
uint8_t* pNTPMSG; // NTP client message receive buffer
ntpformat NTPformat; // NTP message structure
uint8_t ntpmessage[NTP_REQUEST_SIZE];

uint8_t NTP_SOCKET;
uint32_t ntp_1s_tick;   // SecTick counter for ntp process timeout

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void make_ntp_request(uint8_t * ip, uint8_t * msg);
static tstamp get_seconds_from_ntp_server(uint8_t* buf, uint16_t idx, uint8_t timezone);
static void calcdatetime(tstamp seconds, NTP_TIMEINFO * ntp_info);
static void get_time_zone(NTP_TIMEINFO *ntp_info);
static int8_t check_NTP_timeout(void);
static void NTP_time_clear(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* NTP client initialization */
void NTP_init(uint8_t s, uint8_t * buf, NTP_TIMEINFO  * ntp_info)
{
	NTP_SOCKET = s; // SOCK_NTP
	pNTPMSG = buf; 	// User's shared buffer

	ntp_info->gmt_hour = UTC_HOUR;
	ntp_info->gmt_min = UTC_MINUTE;
	ntp_info->ntp_flag = 1;	// Set 'NTP used' flag

	get_time_zone(ntp_info); // set ntp_info.time_zone
}

/* NTP client process */
uint8_t NTP_run(uint8_t * server_ip, NTP_TIMEINFO * ntp_info)
{
	uint8_t ret;

	uint8_t ip[4];
	uint16_t len, port;
	int8_t ret_check_timeout;

	tstamp seconds;
	uint16_t startindex = 40; // last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40

	socket(NTP_SOCKET, Sn_MR_UDP, PORT_NTP, 0x00);

#ifdef	_NTP_DEBUG_
	printf("> Try to connect to NTP TimeServer %d.%d.%d.%d ...\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
#endif

	make_ntp_request(server_ip, ntpmessage);
	sendto(NTP_SOCKET, (uint8_t*)ntpmessage, sizeof(ntpmessage), server_ip, PORT_NTP);

	NTP_time_clear();

	while(1)
	{
		if ((len = getSn_RX_RSR(NTP_SOCKET)) > 0)
		{
			if (len > MAX_NTP_MSG_SIZE) len = MAX_NTP_MSG_SIZE; 	// If received data size is lager than defined buffer size.
			len = recvfrom(NTP_SOCKET, pNTPMSG, len, ip, &port);
#ifdef	_NTP_DEBUG_
	printf("> NTP Response message received from NTP TimeServer %d.%d.%d.%d\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
#endif

			seconds = get_seconds_from_ntp_server(pNTPMSG, startindex, ntp_info->time_zone); // make seconds info based on timestamp
			calcdatetime(seconds, ntp_info); // Calculation date / time

#ifdef	_NTP_DEBUG_
	printf("> NTP Success\r\n");
#endif
			ret = NTP_SUCCESS;
			break;
		}

		// Check Timeout
		ret_check_timeout = check_NTP_timeout();
		if (ret_check_timeout < 0) // timeout occurred and retry count over
		{
#ifdef _NTP_DEBUG_
		printf("> NTP Time Server is not responding : %d.%d.%d.%d\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
#endif
			close(NTP_SOCKET);
			return ret = NTP_FAILED;
		}
		else if (ret_check_timeout == 0) // timeout occurred
		{
#ifdef _NTP_DEBUG_
		printf("> NTP client timeout occurred. Retry to connect to NTP Time Server %d.%d.%d.%d ...\r\n", server_ip[0], server_ip[1], server_ip[2], server_ip[3]);
#endif
			sendto(NTP_SOCKET, (uint8_t*)ntpmessage, sizeof(ntpmessage), server_ip, PORT_NTP);
		}

	}
	ntp_info->ntp_flag = 1;	// Clear 'NTP used' flag
	close(NTP_SOCKET);

	// Return value
	// 0 > :  Failed / 1 - Success
	return ret;
}

/* NTP client timer handler */
void NTP_time_handler(void)
{
	ntp_1s_tick++;
}

/* Clear the NTP client timer count */
static void NTP_time_clear(void)
{
	ntp_1s_tick = 0;
}

/* Make the NTP Request message and store to message buffer */
static void make_ntp_request(uint8_t * ip, uint8_t * msg)
{
	uint8_t flag;

	NTPformat.dstaddr[0] = ip[0]; /* Destination IP address */
	NTPformat.dstaddr[1] = ip[1];
	NTPformat.dstaddr[2] = ip[2];
	NTPformat.dstaddr[3] = ip[3];

	NTPformat.leap = 0;           /* leap indicator */
	NTPformat.version = 4;        /* version number */
	NTPformat.mode = 3;           /* mode */
	NTPformat.stratum = 0;        /* stratum */
	NTPformat.poll = 0;           /* poll interval */
	NTPformat.precision = 0;      /* precision */
	NTPformat.rootdelay = 0;      /* root delay */
	NTPformat.rootdisp = 0;       /* root dispersion */
	NTPformat.refid = 0;          /* reference ID */
	NTPformat.reftime = 0;        /* reference time */
	NTPformat.org = 0;            /* origin timestamp */
	NTPformat.rec = 0;            /* receive timestamp */
	NTPformat.xmt = 1;            /* transmit timestamp */

	flag = (NTPformat.leap<<6)+(NTPformat.version<<3)+NTPformat.mode; //one byte flag
	memcpy(msg, (void const*)(&flag), 1);
}

/*
 *              CHECK NTP TIMEOUT
 *
 * Description : This function check the NTP timeout
 * Arguments   : None.
 * Returns     : -1 - timeout occurred, 0 - timer over, but no timeout, 1 - no timer over, no timeout occur
 * Note        : timeout : retry count and timer both over.
 */

static int8_t check_NTP_timeout(void)
{
	static uint8_t retry_count;

	if(ntp_1s_tick >= NTP_WAIT_TIME)
	{
		ntp_1s_tick = 0;
		if(retry_count >= MAX_NTP_RETRY) {
			retry_count = 0;
			return -1; // timeout occurred
		}
		retry_count++;
		return 0; // timer over, but no timeout
	}

	return 1; // no timer over, no timeout occur
}

static tstamp get_seconds_from_ntp_server(uint8_t* buf, uint16_t idx, uint8_t timezone)
{
    tstamp seconds = 0;
    uint8_t i=0;

    for (i = 0; i < 4; i++)
    {
      seconds = (seconds << 8) | buf[idx + i];
    }

    switch (timezone)
    {
      case 0:
        seconds -=  12*3600;
        break;
      case 1:
        seconds -=  11*3600;
        break;
      case 2:
        seconds -=  10*3600;
        break;
      case 3:
        seconds -=  (9*3600+30*60);
        break;
      case 4:
        seconds -=  9*3600;
        break;
      case 5:
      case 6:
        seconds -=  8*3600;
        break;
      case 7:
      case 8:
        seconds -=  7*3600;
        break;
      case 9:
      case 10:
        seconds -=  6*3600;
        break;
      case 11:
      case 12:
      case 13:
        seconds -= 5*3600;
        break;
      case 14:
        seconds -=  (4*3600+30*60);
        break;
      case 15:
      case 16:
        seconds -=  4*3600;
        break;
      case 17:
        seconds -=  (3*3600+30*60);
        break;
      case 18:
        seconds -=  3*3600;
        break;
      case 19:
        seconds -=  2*3600;
        break;
      case 20:
        seconds -=  1*3600;
        break;
      case 21:
      case 22:
        break;
      case 23:
      case 24:
      case 25:
        seconds +=  1*3600;
        break;
      case 26:
      case 27:
        seconds +=  2*3600;
        break;
      case 28:
      case 29:
        seconds +=  3*3600;
        break;
      case 30:
        seconds +=  (3*3600+30*60);
        break;
      case 31:
        seconds +=  4*3600;
        break;
      case 32:
        seconds +=  (4*3600+30*60);
        break;
      case 33:
        seconds +=  5*3600;
        break;
      case 34:
        seconds +=  (5*3600+30*60);
        break;
      case 35:
        seconds +=  (5*3600+45*60);
        break;
      case 36:
        seconds +=  6*3600;
        break;
      case 37:
        seconds +=  (6*3600+30*60);
        break;
      case 38:
        seconds +=  7*3600;
        break;
      case 39:
        seconds +=  8*3600;
        break;
      case 40:
        seconds +=  9*3600;
        break;
      case 41:
        seconds +=  (9*3600+30*60);
        break;
      case 42:
        seconds +=  10*3600;
        break;
      case 43:
        seconds +=  (10*3600+30*60);
        break;
      case 44:
        seconds +=  11*3600;
        break;
      case 45:
        seconds +=  (11*3600+30*60);
        break;
      case 46:
        seconds +=  12*3600;
        break;
      case 47:
        seconds +=  (12*3600+45*60);
        break;
      case 48:
        seconds +=  13*3600;
        break;
      case 49:
        seconds +=  14*3600;
        break;
    }

    return seconds;
}

static void calcdatetime(tstamp seconds, NTP_TIMEINFO * ntp_info)
{
    uint8_t yf = 0;
    uint16_t y = 0, r = 0, yr = 0;
    tstamp n = 0, d = 0, total_d = 0, rz = 0;
    signed long long yd = 0;
    
    n = seconds;
    total_d = seconds/(SECS_PERDAY);
    d=0;
    uint32_t p_year_total_sec=SECS_PERDAY*365;
    uint32_t r_year_total_sec=SECS_PERDAY*366;
    while(n>=p_year_total_sec) 
    {
      if((EPOCH+r)%400==0 || ((EPOCH+r)%100!=0 && (EPOCH+r)%4==0))
      {
        n = n -(r_year_total_sec);
        d = d + 366;
      }
      else
      {
        n = n - (p_year_total_sec);
        d = d + 365;
      }
      r+=1;
      y+=1;
    
    }
    
    y += EPOCH;
 
    ntp_info->date.year[0] = (uint8_t)((y & 0xff00)>>8);
    ntp_info->date.year[1] = (uint8_t)(y & 0xff);
    
    yd=0;
    yd = total_d - d;
    
    yf=1;
    while(yd>=28) 
    {
        
        if(yf==1 || yf==3 || yf==5 || yf==7 || yf==8 || yf==10 || yf==12)
        {
          yd -= 31;
          if(yd<0)break;
          rz += 31;
        }
    
        if (yf==2)
        {
          if (y%400==0 || (y%100!=0 && y%4==0)) 
          {
            yd -= 29;
            if(yd<0)break;
            rz += 29;
          }
          else 
          {
            yd -= 28;
            if(yd<0)break;
            rz += 28;
          }
        } 
        if(yf==4 || yf==6 || yf==9 || yf==11 )
        {
          yd -= 30;
          if(yd<0)break;
          rz += 30;
        }
        yf += 1;
        
    }
    ntp_info->date.month=yf;
    yr = total_d-d-rz;
   
    yr += 1;

    ntp_info->date.day=yr;
    
    //calculation for time
    seconds = seconds % SECS_PERDAY;
    ntp_info->date.hour = seconds / 3600;
    ntp_info->date.minute = (seconds % 3600) / 60;
    ntp_info->date.second = (seconds % 3600) % 60;
}

static void get_time_zone(NTP_TIMEINFO *ntp_info)
{
      
    switch (ntp_info->gmt_hour)
    {
      case -12:
        ntp_info->time_zone = 0;
        break;
      case -11:
        ntp_info->time_zone = 1;
        break;
      case -10:
        ntp_info->time_zone = 2;
        break;
      case -9:
        if(ntp_info->gmt_min > 0) ntp_info->time_zone = 3;
        else ntp_info->time_zone = 4;
        break;
      case -8:
        ntp_info->time_zone = 5;   // 5, 6
        break;
      case -7:
        ntp_info->time_zone = 7;   // 7, 8
        break;
      case -6:
        ntp_info->time_zone = 9;   // 9, 10
        break;
      case -5:
        ntp_info->time_zone = 11;  // 11, 12, 13
        break;
      case -4:
        if(ntp_info->gmt_min > 0) ntp_info->time_zone = 14;
        else ntp_info->time_zone = 15; // 15, 16
        break;
      case -3:
        if(ntp_info->gmt_min > 0) ntp_info->time_zone = 17;
        else ntp_info->time_zone = 18;
        break;
      case -2:
        ntp_info->time_zone = 19;
        break;
      case -1:
        ntp_info->time_zone = 20;
        break;
      case 0:
        ntp_info->time_zone = 21;  // 21, 22
        break;
      case 1:
        ntp_info->time_zone = 23;  // 23, 24, 25
        break;
      case 2:
        ntp_info->time_zone = 26;  // 26, 27
        break;
      case 3:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 28;  // 28, 29
        else ntp_info->time_zone = 30;
        break;
      case 4:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 31;
        else ntp_info->time_zone = 32;
        break;
      case 5:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 33;
        else if(ntp_info->gmt_min == 30) ntp_info->time_zone = 34;
        else ntp_info->time_zone = 35;
        break;
      case 6:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 36;
        else ntp_info->time_zone = 37;
        break;
      case 7:
        ntp_info->time_zone = 38;
        break;
      case 8:
        ntp_info->time_zone = 39;
        break;
      case 9:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 40;
        else ntp_info->time_zone = 41;
        break;
      case 10:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 42;
        else ntp_info->time_zone = 43;
        break;
      case 11:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 44;
        else ntp_info->time_zone = 45;
        break;
      case 12:
        if(ntp_info->gmt_min == 0) ntp_info->time_zone = 46;
        else ntp_info->time_zone = 47;
        break;
      case 13:
        ntp_info->time_zone = 48;
        break;
      case 14:
        ntp_info->time_zone = 49;
        break;
      default :
        ntp_info->time_zone = 40;  // Seoul, GMT +9:00
        break;
    }
}


// Reserved function, not used.
/*
tstamp changedatetime_to_seconds(NTP_TIMEINFO *ntp_info)
{
  tstamp seconds = 0;

  uint32_t total_day = 0;
  uint16_t i = 0, run_year_cnt = 0 , l = 0;

  l = ntp_info->date.year[0]; // high

  l = (l<<8);

  l = l + ntp_info->date.year[1]; // low


  for(i=EPOCH;i<l;i++)
  {
    if((i%400==0) || ((i%100!=0) && (i%4==0)))
    {
      run_year_cnt += 1;
    }
  }

  total_day=(l-EPOCH-run_year_cnt)*365+run_year_cnt*366;

  for(i=1;i<=ntp_info->date.month;i++)
  {
    if(i==5 || i==7 || i==10 || i==12)
    {
      total_day += 30;
    }
    if (i==3)
    {
      if (l%400==0 && l%100!=0 && l%4==0)
      {
        total_day += 29;
      }
      else
      {
        total_day += 28;
      }
    }
    if(i==2 || i==4 || i==6 || i==8 || i==9 || i==11)
    {
      total_day += 31;
    }
  }

  seconds = (total_day+ntp_info->date.day-1)*24*3600;
  seconds += ntp_info->date.second;//seconds
  seconds += ntp_info->date.minute*60;//minute
  seconds += ntp_info->date.hour*3600;//hour

  return seconds;
}
*/
