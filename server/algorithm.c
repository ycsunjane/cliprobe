/*
 * ============================================================================
 *
 *       Filename:  algorithm.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月06日 09时30分44秒
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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "algorithm.h"

static double average(int n,double *x) 
{
	int i; double av; 
	av=0; 
	for(i=0;i<n;i++) 
		av+=*(x+i); 
	av=av/n; 
	return(av); 
}

static double spfh(int n,double *x) 
{
	int i; double a; 
	a=0; 
	for(i=0;i<n;i++) 
		a+=(*(x+i))*(*(x+i));
	return(a); 
}

static double shpf(int n,double *x) 
{
	int i; 
	double a,b; 
	a=0; 
	for(i=0;i<n;i++) 
		a=a+*(x+i); 
	b=a*a/n; 
	return(b); 
}

static double dcj(int n,double *x,double *y) 
{
	int i; double a; 
	a=0; 
	for(i=0;i<n;i++) 
		a+=(*(x+i))*(*(y+i)); 
	return(a); 
} 

static double djc(int n,double *x,double *y) 
{
	int i; 
	double a=0,b=0; 
	for(i=0;i<n;i++) {
		a=a+*(x+i); 
		b=b+*(y+i); 
	} 
	a=a*b/n; 
	return(a); 
}

static double xsa(int n,double *x,double *y) 
{
	double a,b,c,d,e; 
	a=spfh(n,x); 
	b=shpf(n,x); 
	c=dcj(n,x,y); 
	d=djc(n,x,y); 
	e=(c-d)/(a-b); 
	return(e); 
} 

static double he(int n,double *y) 
{
	int i; double a; 
	a=0; 
	for(i=0;i<n;i++) 
		a=a+*(y+i); 
	return(a); 
} 

static double xsb(int n,double *x,double *y,double a) 
{ 
	double b,c,d; 
	b=he(n,y); 
	c=he(n,x); 
	d=(b-a*c)/n; 
	return(d); 
} 

static void _getpos(int n, double *x, double *y, double *a, double *b) 
{ 
	*a = xsa(n,x,y); 
	*b = xsb(n,x,y,*a); 
	sys_debug("f(x)=%5.4fx+%5.4f\n",*a,*b); 
}


int get_point(struct point_t *pi, int num, double *a, double *b)
{
	int m = num - 1;
	double *x = malloc(sizeof(double) * m * 2);
	if(x == NULL) return -1;

	double *y = x + m;
	double *_x = x;
	double *_y = y;
	double z;

	int i;
	for(i = 0; i < m; i++) {
		*_x = 2 * (pi[i].x - pi[m].x); 
		*_y = 2 * (pi[i].y - pi[m].y);

		z = (pow(pi[m].d, 2) - pow(pi[i].d, 2)
			+ pow(pi[i].x, 2) - pow(pi[m].x, 2)
			+ pow(pi[i].y, 2) - pow(pi[m].y, 2));

		sys_debug("%lf = %lf - %lf + %lf - %lf + %lf - %lf\n", 
			z, pow(pi[m].d, 2), pow(pi[i].d, 2)
			,pow(pi[i].x, 2), pow(pi[m].x, 2)
			,pow(pi[i].y, 2), pow(pi[m].y, 2));
		*_x /= z;
		*_y /= z;
		_x++; _y++;
	}

	double _a, _b;
	_getpos(m, x, y, &_a, &_b);
	if(isnan(_a) || isnan(_b)) {
		free(x);
		sys_warn("cal failed\n");
		return -1;
	}

	*b = 1 / _b;
	*a = -_a * *b;

	free(x);
	return 0;
}

/*
 * pld: dissipation
 * d0: sample point distance
 * pld0: d0 point dissipation
 * */
double _distance(double pld, double d0, double pld0, double r)
{
	double x = (pld - pld0) / (10 * r);
	double y = pow(10, x);
	return d0 * y;
}
