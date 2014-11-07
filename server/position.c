/*
 * ============================================================================
 *
 *       Filename:  position.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月04日 17时31分28秒
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
#include <pthread.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "clihash.h"
#include "cmdarg.h"
#include "algorithm.h"
#include "log.h"
#include "apsample.h"
#include "sql.h"

static void __create_pthread(void *(*start_routine) (void *), void *arg)
{
	int ret;
	pthread_t pid;
	ret = pthread_create(&pid, NULL, start_routine, arg);
	if(ret != 0) {
		sys_err("Create pthread failed: %s\n", strerror(errno));
		exit(-1);
	}

	ret = pthread_detach(pid);
	if(ret != 0) {
		sys_err("Create pthread failed: %s\n", strerror(errno));
		exit(-1);
	}
}

static void now(struct timeval *time)
{
	gettimeofday(time, NULL);
}

static void  set_bit(uint32_t *bitmap, int offset)
{
	*bitmap |= 1ul << offset;
}

static void  clr_bit(uint32_t *bitmap)
{
	*bitmap = 0;
}

static int isset(uint32_t *bitmap, int offset)
{
	return *bitmap & (1 << offset);
}

static void ajust_pos(struct cli_t *pcli, struct timeval *ti, double *a, double *b)
{
	mac_debug(MAC(pcli->climac), "drift: %d cur: (%lf, %lf) last: (%lf, %lf)\n",
			argument.drift, *a, *b, pcli->pos.x, pcli->pos.y);

	if(pcli->pos.x == 0 && pcli->pos.y == 0)
		goto update;
	/* 
	if(ti->tv_sec - pcli->pos.timestamp.tv_sec > argument.agetime)
		goto update;
		*/

	double _x = *a - pcli->pos.x;
	double _y = *b - pcli->pos.y;
	if(abs(_x) > argument.drift)
		*a = _x > 0 ? pcli->pos.x + argument.drift : 
			pcli->pos.x - argument.drift;
	if(abs(_y) > argument.drift)
		*b = _y > 0 ? pcli->pos.y + argument.drift : 
			pcli->pos.y - argument.drift;
update:
	pcli->pos.x = *a;
	pcli->pos.y = *b;
	//pcli->pos.timestamp.tv_sec = ti->tv_sec;
	return;
}

void *get_pos(void *arg)
{
	struct timeval ti;

	int ret;
	struct ap_t *ap;
	int i, j, k, num = 0;
	long interval;
	struct point_t *pi = NULL;
	double a,b;

replay:
	for(i = 0; i < MAX_CLI; i++) {
		if(clihead[i].key == IDLE_CLI)
			continue;

		now(&ti);
		if((ti.tv_sec - clihead[i].timestamp.tv_sec) > 
			argument.losttime) {
			cli_del(&clihead[i]);
			continue;
		}

		pthread_mutex_lock(&clihead[i].lock);
		if(clihead[i].key == IDLE_CLI)
			goto unlock;

		clr_bit(&clihead[i].bitmap);
		num = 0;

		for(j = 0; j < MAX_AP; j++) {
			ap = &clihead[i].ap[j];
			interval = ti.tv_sec - ap->timestamp.tv_sec;

			if((ap->valid) && (interval < argument.agetime)) {
				num++;
				set_bit(&clihead[i].bitmap, j);
			}
		}

		if(num < 3) goto unlock;

		pi = malloc(sizeof(struct point_t) * num);
		if(pi == NULL) {
			sys_warn("Malloc memory failed: %s\n",
				strerror(errno));
			continue;
		}

		for(j = 0, k = 0; j < MAX_AP && k < num; j++) {
			if(isset(&clihead[i].bitmap, j)) {
				ret = getap_pos(clihead[i].ap[j].apmac, pi + k);
				if(ret < 0) goto free;
				pi[k].d = distance(clihead[i].ap[j].signal);
				mac_debug(MAC(clihead[i].climac), 
					"(%lf, %lf, %lf) sig: %d dist: %lf\n", 
					pi[k].x, pi[k].y, pi[k].z,
					clihead[i].ap[j].signal, pi[k].d);
				k++;
			}
		}

		ret = get_point(pi, num, &a, &b);
		if(ret < 0 || a < 0 || b < 0) goto free;
		ajust_pos(&clihead[i], &ti, &a, &b);
		mac_debug(MAC(clihead[i].climac), "%lf %lf %lf\n", a, b, pi->z);
#ifndef DISABLE_MYSQL
		sql_insert(&sql, clihead[i].climac, a, b, pi->z);
#endif
free:
		free(pi);
unlock:
		pthread_mutex_unlock(&clihead[i].lock);
	}
	sleep(2);
	goto replay;
	return NULL;
}

void pthread_pos_init()
{
	__create_pthread(get_pos, NULL);
}
