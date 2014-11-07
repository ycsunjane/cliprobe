/*
 * ============================================================================
 *
 *       Filename:  apsample.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月09日 15时26分57秒
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
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "cmdarg.h"
#include "log.h"
#include "apsample.h"

static int  number = 0;
struct ap_sample_t *head = NULL;
struct ap_sample_t **ptail = &head;

static void _get_ap(struct ap_sample_t *sample, char *tmp)
{
	char *_tmp = tmp;
	char *apmac = (char *)&(sample->apmac);
	int nread = sscanf(_tmp, "%x:%x:%x:%x:%x:%x", 
		(unsigned int *)(apmac + 0),
		(unsigned int *)(apmac + 1),
		(unsigned int *)(apmac + 2),
		(unsigned int *)(apmac + 3),
		(unsigned int *)(apmac + 4),
		(unsigned int *)(apmac + 5)); 

	if(nread < 6) {
		sys_err("%s[%d],%d  %s process failed\n", 
			argument.apconf, number, nread, tmp);
		exit(-1);
	}

	_tmp = next_entry(_tmp);
	nread = sscanf(_tmp, "%lf", &sample->x);
	if(nread <= 0) {
		sys_err("%s[%d]:%s process failed\n", 
			argument.apconf, number, tmp);
		exit(-1);
	}

	_tmp = next_entry(_tmp);
	nread = sscanf(_tmp, "%lf", &sample->y);
	if(nread <= 0) {
		sys_err("%s[%d]:%s process failed\n", 
			argument.apconf, number, tmp);
		exit(-1);
	}

	_tmp = next_entry(_tmp);
	nread = sscanf(_tmp, "%lf", &sample->z);
	if(nread <= 0) {
		sys_err("%s[%d]:%s process failed\n", 
			argument.apconf, number, tmp);
		exit(-1);
	}

	sample->next = NULL;
}

void ap_sample_init()
{
	FILE *fp;
	fp = fopen(argument.apconf, "r");
	if(fp == NULL) {
		sys_err("Open %s failed: %s\n", 
			argument.apconf, strerror(errno));
		exit(-1);
	}

	int nread;
	char *tmp;
	struct ap_sample_t *sample;
	char *pline = NULL;
	size_t size = 0;
	while(1) {
		if(pline) free(pline);
		pline = NULL;
		size = 0;

		nread = getline(&pline, &size, fp);
		if(nread < 0) goto free;

		tmp = pline;
		tmp = strip(tmp);
		if(*tmp == '#') continue;

		sample = malloc(sizeof(struct ap_sample_t));
		if(sample == NULL) {
			sys_err("Malloc ap sample failed:%s \n",
				strerror(errno));
			exit(-1);
		}

		_get_ap(sample, tmp);
		*ptail = sample;
		ptail = &sample->next;
	}
free:
	if(pline) free(pline);
	fclose(fp);
}

int getap_pos(uint64_t apmac, struct point_t *pi)
{
	struct ap_sample_t *tmp = head;
	while(tmp) {
		if(apmac == tmp->apmac) {
			pi->x = tmp->x;
			pi->y = tmp->y;
			pi->z = tmp->z;
			return 0;
		}
		tmp = tmp->next;
	}
	mac_debug(MAC(apmac), "can not find in /etc/clipos/ap.conf\n");
	return -1;
}
