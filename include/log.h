/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年07月25日 10时13分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __LOG_H__
#define __LOG_H__
#include <stdio.h>
#include <syslog.h>
#include <stdint.h>
#include <pthread.h>

extern int debug;

#define SYSLOG_ERR 	(LOG_ERR|LOG_USER)
#define SYSLOG_WARN 	(LOG_WARNING|LOG_USER)
#define SYSLOG_DEBUG 	(LOG_DEBUG|LOG_USER)
#define SYSLOG_INFO 	(LOG_INFO|LOG_USER)
#define SYSLOG_LOCK 	(LOG_NOTICE|LOG_USER)

#define __sys_log2(fmt, ...) 						\
	do { 								\
		syslog(SYSLOG_INFO, fmt, ##__VA_ARGS__); 		\
		if(debug) fprintf(stderr, fmt,  ##__VA_ARGS__); 	\
	} while(0)

#define __sys_log(LEVEL, fmt, ...) 										\
	do { 													\
		syslog(LEVEL, "(%u)%s +%d %s(): "fmt,  								\
			(unsigned int)pthread_self(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); 	\
		if(debug || LEVEL == SYSLOG_ERR) 											\
			fprintf(stderr, "(%u)%s +%d %s(): "fmt, 							\
				(unsigned int)pthread_self(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); 	\
	} while(0)

#define sys_err(fmt, ...) 	__sys_log(SYSLOG_ERR, "ERR: "fmt, ##__VA_ARGS__)

#ifdef DEBUG
#define sys_warn(fmt, ...) 	__sys_log(SYSLOG_WARN, "WARNING: "fmt, ##__VA_ARGS__)
#define sys_debug(fmt, ...) 	__sys_log(SYSLOG_DEBUG, "DEBUG: "fmt, ##__VA_ARGS__)
#define sys_lock(fmt, ...) 	__sys_log(SYSLOG_LOCK, "LOCK: "fmt, ##__VA_ARGS__)
#define pure_info(fmt, ...) 	__sys_log2(fmt, ##__VA_ARGS__)
#else
#define sys_warn(fmt, ...) 	NULL
#define sys_debug(fmt, ...)  	NULL
#define sys_lock(fmt, ...) 	NULL
#define pure_info(fmt, ...) 	NULL
#endif

#ifdef DEBUG
#define pr_mac1(mac) pr_mac(mac, 1)
#define pr_mac2(mac) pr_mac(mac, 2)
#define MAC(mac) _cmac(mac)
#define mac_debug(mac, fmt, ...) 	__sys_log2("%s "fmt, mac, ##__VA_ARGS__)
#else
#define pr_mac1(mac) NULL
#define pr_mac2(mac) NULL
#define MAC(mac) NULL
#define mac_debug(mac, fmt, ...) 	NULL
#endif

#ifdef DEBUG
static 	char macstr[18] = {0};
static char *_cmac(uint64_t lmac)
{
	unsigned char *mac = (unsigned char *)&lmac;
	sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return &macstr[0];
}

static void pr_mac(unsigned char *mac, int b)
{
	fprintf(stderr,"%02x:%02x:%02x:%02x:%02x:%02x",
		(unsigned char) mac[0],
		(unsigned char) mac[1],
		(unsigned char) mac[2],
		(unsigned char) mac[3],
		(unsigned char) mac[4],
		(unsigned char) mac[5]);
	switch(b) {
	case 1:
		fprintf(stderr, "\t");
		break;
	case 2:
		fprintf(stderr, "\n");
		break;
	}
}
#endif
#endif /* __LOG_H__ */
