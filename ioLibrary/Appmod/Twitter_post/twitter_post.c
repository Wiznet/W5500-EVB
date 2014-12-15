#include <stdint.h>
#include <string.h>

#include "../../Ethernet/socket.h"
#include "twitter_post.h"

#ifdef _TWEET_DEBUG_
   #include <stdio.h>
#endif

uint8_t * pTWEETBUF;       				// Tweet message buffer
uint8_t  TWEET_SOCKET;    				// SOCKET number for Tweet
uint8_t * OAuth_TOKEN;					// Token to post a message using OAuth

uint16_t tcpclient_port	= 50002;

bool flag_tweet_init = FALSE;			// flag: check the tweet_init() process run preceded

bool enable_tweet_1s_tick = FALSE;		// SecTick counter enable
volatile uint32_t tweet_1s_tick = 0;   	// SecTick counter

bool enable_tcp_connect = TRUE;			// TCP connect enable
uint8_t tcp_connect_retry_count = 0;	// TCP connect retry counter

/* Generates post message packet to Twitter (tweet) includes tweet message and OAuth token */
uint16_t tweet_packet_gen(uint8_t * msg, uint8_t * token, uint8_t * buf);

void tweet_init(uint8_t s, uint8_t * buf, uint8_t * token)
{
	TWEET_SOCKET = s; 		// SOCK_TWEET
	pTWEETBUF = buf; 		// User's shared buffer
	OAuth_TOKEN = token;

	flag_tweet_init = TRUE;
}

/* tweet_post() function return values */
// Tweet succeed : 1 (one)
// Tweet failed :
//		socket error - a negative number (predefined socket error codes)
//		initialization was not performed / invalid generated packet length / connection failed - 0 (zero)
//
int8_t tweet_post(uint8_t * msg, uint8_t  * pSip)
{
	int8_t ret, i;
	uint16_t len;

	uint8_t flag_tweet_post_process_end = 0;

#ifdef _TWEET_DEBUG_
	uint8_t destip[4] = {0, };
	uint16_t destport;
#endif

	// Check the tweet_init() process run preceded
	if(!flag_tweet_init)
	{
#ifdef _TWEET_DEBUG_
		printf("%d:tweet_init() function was not executed yet\r\n", TWEET_SOCKET);
#endif
		return 0;
	}

	while(!flag_tweet_post_process_end)
	{
		switch(getSn_SR(TWEET_SOCKET))
		{
			case SOCK_ESTABLISHED :
				if(getSn_IR(TWEET_SOCKET) & Sn_IR_CON)
				{
#ifdef _TWEET_DEBUG_
					 getSn_DIPR(TWEET_SOCKET, destip);
					 destport = getSn_DPORT(TWEET_SOCKET);

					 printf("%d:Connected to - %d.%d.%d.%d : %d\r\n", TWEET_SOCKET, destip[0], destip[1], destip[2], destip[3], destport);
#endif
					 setSn_IR(TWEET_SOCKET, Sn_IR_CON);
				}

				// Generates the Tweet message packet
				len = tweet_packet_gen(msg, OAuth_TOKEN, pTWEETBUF);

				// Check packet length validation
				if(len <= 0)
				{
#ifdef _TWEET_DEBUG_
					printf("%d:Wrong Packet length value - %d\r\n", TWEET_SOCKET, len);
#endif
					return 0;
				}

				send(TWEET_SOCKET, pTWEETBUF, len);

				for(i = 0; i < 10; i++) getMR(); // for delay

				close(TWEET_SOCKET);
				while(getSn_SR(TWEET_SOCKET) != SOCK_CLOSED);

				// Tweet succeed
				flag_tweet_post_process_end = 1;
				ret = 1;
				break;

			case SOCK_CLOSE_WAIT :
				if((ret=disconnect(TWEET_SOCKET)) != SOCK_OK) return ret;
#ifdef _TWEET_DEBUG_
				printf("%d:Socket closed\r\n", TWEET_SOCKET);
#endif
				break;

			case SOCK_INIT :
/*
=========================================================
// Before
			case SOCK_INIT :
#ifdef _TWEET_DEBUG_
				printf("%d:Try to TCP connect to - %d.%d.%d.%d : %d\r\n", TWEET_SOCKET, pSip[0], pSip[1], pSip[2], pSip[3], LIB_PORT);
#endif
				if((ret=connect(TWEET_SOCKET, pSip, LIB_PORT)) != SOCK_OK) return ret;
				break;
=========================================================
*/
				if(!enable_tweet_1s_tick) enable_tweet_1s_tick = TRUE;

				if(tcp_connect_retry_count <= MAX_TCP_CONNECT_RETRY_COUNT)
				{
					if(enable_tcp_connect)
					{
#ifdef _TWEET_DEBUG_
						printf("%d:Try to TCP connect to - %d.%d.%d.%d : %d\r\n", TWEET_SOCKET, pSip[0], pSip[1], pSip[2], pSip[3], LIB_PORT);
#endif
						if((ret=connect(TWEET_SOCKET, pSip, LIB_PORT)) != SOCK_OK) return ret;

						enable_tcp_connect = FALSE;
						tcp_connect_retry_count++;
					}

					if(tweet_1s_tick >= TCP_CONNECT_RETRY_PERIOD_SEC) // tweet_1s_tick: increase automatically by MCU Timer
					{
						enable_tcp_connect = TRUE;
						tweet_1s_tick = 0;
					}
				}
				else // TCP connection failed: Retry count
				{
#ifdef _TWEET_DEBUG_
					printf("%d:TCP connection failed\r\n", TWEET_SOCKET);
#endif
					// Tweet failed
					flag_tweet_post_process_end = 1;
					ret = 0;
				}
				break;

			case SOCK_CLOSED:
#ifdef _TWEET_DEBUG_
				printf("%d:TCP client for Tweet - Start\r\n", TWEET_SOCKET);
#endif
				if((ret=socket(TWEET_SOCKET, Sn_MR_TCP, tcpclient_port++, 0x00)) != TWEET_SOCKET) return ret;

#ifdef _TWEET_DEBUG_
				printf("%d:Socket opened\r\n", TWEET_SOCKET);
#endif
				break;

			default:
				break;
		} // end of switch-case statement
	} // end of while loop

	// Time/retry counter value clear
	enable_tweet_1s_tick = FALSE;
	tweet_1s_tick = 0;
	enable_tcp_connect = FALSE;
	tcp_connect_retry_count = 0;

	return ret;
}

uint16_t tweet_packet_gen(uint8_t * msg, uint8_t * token, uint8_t * buf)
{
	uint16_t len;
	uint16_t message_len, token_len;

	message_len = strlen((void *)msg);
	token_len = strlen((void *)token);

	// check the maximum tweet length (140)
	if(message_len > MAX_TWEET_MSG_LEN) return 0;

	// Make the Tweet message packet
	memset(buf, '\0', MAX_TWEET_PACKET_SIZE);
	len = sprintf(buf, "POST http://%s/update HTTP/1.0\r\n", LIB_DOMAIN);
	len += sprintf(buf+len, "Content-Length: %d\r\n\r\n", message_len + token_len + 14);
	len += sprintf(buf+len, "token=%s", token);
	len += sprintf(buf+len, "&status=%s\r\n", msg);

#ifdef _TWEET_DEBUG_
	printf("\r\nTweet Packet length: %d\r\n", len);
	printf("Tweet Packet:\r\n");
	printf("======================================================\r\n");
	printf("%s\r\n", buf);
	printf("======================================================\r\n");
#endif

	return len;
}

/* Tweet time handler */
void Tweet_time_handler(void)
{
	if(enable_tweet_1s_tick) tweet_1s_tick++;
	else ;
}
