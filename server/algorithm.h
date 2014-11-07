/*
 * =====================================================================================
 *
 *       Filename:  algorithm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月06日 09时30分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __ALGORITHM__
#define __ALGORITHM__

#include "cmdarg.h"

struct point_t {
	double x;
	double y;
	double z;
	double d;
};

/*
 * 4~5 : Dense area
 * 3~4: nomal area
 * 2.5~3: outdoor
 * 1m : 38.45db, 10m: 58.45db
 * */
#define distance(pld) 	_distance((-pld) + 20, argument.dis, argument.sig, argument.fac);
double _distance(double pld, double d0, double pld0, double r);
int get_point(struct point_t *pi, int num, double *a, double *b);

#endif /* __ALGORITHM__ */
