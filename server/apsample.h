/*
 * =====================================================================================
 *
 *       Filename:  apsample.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月09日 15时27分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __APSAMPLE__
#define __APSAMPLE__
#include <stdint.h>
#include "algorithm.h"

struct ap_sample_t {
	uint64_t apmac;
	double x;
	double y;
	double z;
	struct ap_sample_t *next;
};

extern struct ap_sample_t *head;
void ap_sample_init();
int getap_pos(uint64_t apmac, struct point_t *pi);
#endif /* __APSAMPLE__ */
