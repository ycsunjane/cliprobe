/*
 * =====================================================================================
 *
 *       Filename:  clihash.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月04日 15时58分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __CLIHASH_H__
#define __CLIHASH_H__
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include "wnic.h"

#ifndef DISABLE_MYSQL
#include "sql.h"
#endif

#define IDLE_CLI 	(-1)
#define MAX_CLI 	(20000)
#define MAX_AP 		(16)
struct ap_t {
	int valid;
	uint64_t apmac;
	char signal;
	struct timeval 	timestamp;
};

struct cli_pos_t {
	double 		x;
	double 		y;
	struct timeval 	timestamp;
};

struct cli_t {
	int 	key;
	pthread_mutex_t lock; 

	uint64_t  	climac;
	struct cli_pos_t pos;
	int 		offset;
	uint32_t 	bitmap;
	struct 		ap_t ap[MAX_AP];
	struct timeval 	timestamp;
	struct timeval 	firsttime;
};

extern struct cli_t *clihead;

void cli_hash_init();
void cli_update(struct wnic_t *cli, MYSQL *sql);
void cli_del(struct cli_t *cli);
#endif /* __CLIHASH_H__ */
