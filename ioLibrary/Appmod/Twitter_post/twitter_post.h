
#ifndef  _WIZCHIP_TWEET_H_
#define  _WIZCHIP_TWEET_H_

#define _TWEET_DEBUG_

#define MAX_TWEET_MSG_LEN   			140
#define MAX_TWEET_PACKET_SIZE			512

#define	TCP_CONNECT_RETRY_PERIOD_SEC	3
#define MAX_TCP_CONNECT_RETRY_COUNT		3

#define LIB_DOMAIN 		"arduino-tweet.appspot.com"
#define LIB_PORT		80		// HTTP Port

/*
 * @brief Tweet post process initialize
 * @param s   	: Socket number for Tweet
 * @param buf 	: Buffer for Tweet message
 * @param token : Token to post a message using OAuth
 */
void tweet_init(uint8_t s, uint8_t * buf, uint8_t * token);

/*
 * @brief Tweet post process
 * @param msg   : Tweet message to post
 * @param pSip 	: Server IP address
 */
int8_t tweet_post(uint8_t * msg, uint8_t  * pSip);

/*
 * @brief Tweet 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler
 */
void Tweet_time_handler(void);

#endif   // _WIZCHIP_TWEET_H_
