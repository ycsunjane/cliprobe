/*
 * ============================================================================
 *
 *       Filename:  clihash.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月04日 15时42分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "clihash.h"
#include "log.h"
#include "wnic.h"
#include "cmdarg.h"

struct cli_t *clihead = NULL;

void cli_hash_init()
{
	assert(clihead == NULL);

	clihead = calloc(1, sizeof(struct cli_t) * MAX_CLI);
	sys_debug("Hash calloc size: %ldm\n",
		sizeof(struct cli_t) * MAX_CLI / 1024 / 1024);
	if(clihead == NULL) {
		sys_err("Malloc for clihash failed: %s\n", 
			strerror(errno));
		exit(-1);
	}

	int i;
	for(i = 0; i < MAX_CLI; i++) {
		clihead[i].key = IDLE_CLI;
	}
}

static unsigned int 
__elfhash(char* str, unsigned int len)  
{  
	unsigned int hash = 0;  
	unsigned int x    = 0;  
	unsigned int i    = 0;  
	for(i = 0; i < len; str++, i++)  
	{  
		hash = (hash << 4) + (*str);  
		if((x = hash & 0xF0000000L) != 0)  
		{  
			hash ^= (x >> 24);  
		}  
		hash &= ~x;  
	}  
	return hash;  
}

static int hash_cli(uint64_t climac)
{
	int total = 0;
	struct cli_t *pcli = NULL;

	char *data = (char *)&climac;
	int key = __elfhash(data, 6) % MAX_CLI;

	do {
		if(pcli)
			pthread_mutex_unlock(&pcli->lock);

		pcli = clihead + key;

		pthread_mutex_lock(&pcli->lock);
		if(pcli->key == IDLE_CLI) {
			pcli->key = key;
			pcli->climac = climac;
			return key;
		}

		total++;
		key = (key + 1) % MAX_CLI;
	} while(total <= MAX_CLI && 
		pcli->climac != climac);
	key = (key + MAX_CLI - 1) % MAX_CLI;

	if(total > MAX_CLI) {
		pthread_mutex_unlock(&pcli->lock);
		sys_warn("Hash bucket have full\n");
		return -1;
	} else {
		/* old ap */
		assert(pcli->key == key);
		return key;
	}
	return -1;
}

void cli_update(struct wnic_t *cli, MYSQL *sql)
{
	assert(cli != NULL);
	struct timeval ti;
	int i;

	int key = hash_cli(cli->climac);
	if(key < 0) return;

	int offset = clihead[key].offset;
	assert(offset < MAX_AP);

	if(cli->signal < argument.minsig)
		return;

	if(clihead[key].key == IDLE_CLI) goto unlock;

	gettimeofday(&ti, NULL);

	/* old ap */
	for(i = 0; i < MAX_AP; i++) {
		if(clihead[key].ap[i].valid &&
			clihead[key].ap[i].apmac == cli->apmac) {
			clihead[key].ap[i].signal = cli->signal;
			clihead[key].ap[i].timestamp = ti;
			clihead[key].timestamp = ti;
#ifndef DISABLE_MYSQL
			sql_usr_insert(sql, cli->climac, 
				clihead[key].firsttime, ti);
#endif
			goto unlock;
		}
	}

	/* new ap */
	i = offset;
	clihead[key].ap[offset].valid = 1;
	clihead[key].ap[offset].apmac = cli->apmac;
	clihead[key].ap[offset].signal = cli->signal;
	clihead[key].ap[offset].timestamp = ti;
	clihead[key].timestamp = ti;
	clihead[key].firsttime = ti;

	clihead[key].offset = (offset + 1) % MAX_AP;
#ifndef DISABLE_MYSQL
	sql_usr_insert(sql, cli->climac, ti, ti);
#endif
unlock:
	/*  
	mac_debug(MAC(clihead[key].climac), "key: %d, i: %d, sig: %d\n",
		key, i, clihead[key].ap[i].signal);
	*/
	pthread_mutex_unlock(&clihead[key].lock);
}

void cli_del(struct cli_t *cli)
{
	mac_debug(MAC(cli->climac), "lost deleting\n");
	pthread_mutex_lock(&cli->lock);
	cli->key = IDLE_CLI;
	memset(&cli->pos, 0, sizeof(cli->pos));
	cli->offset = 0;
	cli->timestamp.tv_sec = 0;
	pthread_mutex_unlock(&cli->lock);
}
