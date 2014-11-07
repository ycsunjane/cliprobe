/*
 * =====================================================================================
 *
 *       Filename:  cmdarg.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月09日 15时45分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __CMDARG_H__
#define __CMDARG_H__
#include <limits.h>

#define SERCONF 	"/etc/clipos/serprobe.conf"
#define ENTRY_LEN 	(PATH_MAX)

struct cmdarg_t {
#define APCONFFILE 	"/etc/clipos/ap.conf"
#define APCONF 		"ap_sample_config_file"
	char apconf[PATH_MAX];
#define MINSIG 		"minimal_signal"
	char minsig;
#define AGETIME 	"report_age_interval"
	int  agetime;
#define LOSTTIME 	"cli_lost_interval"
	int  losttime;
#define DIS 		"sample_distance"
	int  dis;
#define SIG 		"sample_signal"
	int  sig;
#define FAC 		"env_factory"
	int  fac;
#define DRIFT 		"max_drift"
	int  drift;
};

extern int debug; 
extern int daemon_mode;
#define CONFIG_SET(name)
extern struct cmdarg_t argument;
char *strip(char *tmp);
char *next_entry(char *tmp);
void proc_arg(int argc, char *argv[]);
#endif /* __CMDARG_H__ */
