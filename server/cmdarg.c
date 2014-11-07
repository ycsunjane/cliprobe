/*
 * ============================================================================
 *
 *       Filename:  arg.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年08月19日 10时04分12秒
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
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "cmdarg.h"
#include "log.h"

int debug, daemon_mode = 0;
struct cmdarg_t argument;

#define has_arg (1)
static struct option long_arg[] = {
	{"apconf", has_arg, 0, 'a'},
	{"minsig", has_arg, 0, 'm'},
	{"agetime", has_arg, 0, 't'},
	{"losttime", has_arg, 0, 'l'},
	{"dis", has_arg, 0, 'i'},
	{"sig", has_arg, 0, 's'},
	{"fac", has_arg, 0, 'f'},
	{"drift", has_arg, 0, 'r'},
	{"daemon", 0, 0, 'd'},
	{"debug", has_arg, 0, 'b'},
	{"help", 0, 0, 'h'},
	{0, 0, 0 , 0},
};

#define SHORT_STR 	"ha:m:t:dl:b:p:d:s:f:"

static char *help_array[] = {
	"USAGE: serprobe [options]",
	"  -a, --apconf \t\t ap position config file (default /etc/clipos/ap.conf)",
	"  -t, --agetime \t ap report age time (default 5s)",
	"  -l, --losttime \t client lost age time (default 300s)",
	"  -i, --dis \t\t sample distance  (default 1m)",
	"  -s, --sig \t\t sample distance signal (default 53)",
	"  -f, --fac \t\t default factory 2.5 ~ 5 (default 4.5)",
	"  -r, --drift \t\t default drift (default 1)",
	"  -m, --minsig \t\t min signal get in calculate (default -70)",
	"  -d, --daemon \t\t daemon mode",
	"  -b, --debug 	\t enable debug will auto disable daemon_mode(debug: 2 verbose mode)",
	"  --help \t\t help info",
	NULL,
};

void help()
{
	char **ptr = &help_array[0];
	while(*ptr != NULL) {
		printf("%s\n", *ptr);
		ptr++;
	}
}

static void __early_is_debug(int argc,char *argv[])
{
	int i, ret;
	for(i = 0; i < argc; i++) {
		ret = strcmp(argv[i], "-b");
		if(ret == 0)  {
			debug = 1;
			return;
		}

		ret = strcmp(argv[i], "--debug");
		if(ret == 0) {
			debug = 1;
			return;
		}
	}

	debug = 0;
	return;
}

static void __early_init(int argc, char *argv[])
{
	memset(&argument, 0, sizeof(argument));
	__early_is_debug(argc, argv);
	strncpy(&argument.apconf[0], APCONFFILE, strlen(APCONFFILE));
	argument.minsig= -70;
	argument.agetime = 5;
	argument.losttime = 300;
	argument.dis = 1;
	argument.sig = 53;
	argument.fac = 4.5;
	argument.drift = 1;
}

static long int __strtol(const char *nptr, char **endptr, int base)
{
	long int ret;
	errno = 0;
	ret = strtol(nptr, endptr, base);
	if(errno != 0) {
		sys_err("argument error:%s\n", nptr);
		exit(-1);
	}
	return ret;
}

static void proc_cmdarg(int argc, char *argv[])
{
	int short_arg;

	while((short_arg = getopt_long(argc, argv, SHORT_STR, long_arg, NULL)) 
		!= -1) {
		switch(short_arg) {
		case 'a':
			strncpy(&argument.apconf[0], optarg, PATH_MAX-1);
			break;
		case 'm':
			argument.minsig = __strtol(optarg, NULL, 10);
			break;
		case 't':
			argument.agetime = __strtol(optarg, NULL, 10);
			break;
		case 'l':
			argument.losttime = __strtol(optarg, NULL, 10);
			break;
		case 'i':
			argument.dis = __strtol(optarg, NULL, 10);
			break;
		case 's':
			argument.sig = __strtol(optarg, NULL, 10);
			break;
		case 'f':
			argument.fac = __strtol(optarg, NULL, 10);
			break;
		case 'r':
			argument.drift = __strtol(optarg, NULL, 10);
			break;
		case 'd':
			if(!debug)
				daemon_mode = 1;
			break;
		case 'b':
			debug = __strtol(optarg, NULL, 10);
			break;
		case '?':
			break;
		case 'h':
			help();
			exit(0);
		case '*':
			break;
		default:
			fprintf(stderr, "argument: %c error\n", short_arg);
			break;
		}
	}
}

char *strip(char *tmp)
{
	while(*tmp == ' ' || *tmp == '\t')
		tmp++;
	return tmp;
}

char *next_entry(char *tmp)
{
	while(*tmp != ' ' && *tmp != '\t' && *tmp != EOF)
		tmp++;
	return strip(tmp);
}

char entry[ENTRY_LEN] = {0};
char value[ENTRY_LEN] = {0};
static int  number = 0;
static void proc_confcmd()
{
	FILE *fp;
	fp = fopen(SERCONF, "r");
	if(fp == NULL) {
		sys_err("Open %s failed: %s\n", 
			SERCONF, strerror(errno));
		exit(-1);
	}

	int nread;
	char *tmp;
	char *pline = NULL;
	size_t size;

	while(1) {
		number++;
		if(pline) free(pline);
		pline = NULL;
		size = 0;

		nread = getline(&pline, &size, fp);
		if(nread < 0) goto free;

		tmp = pline;

		/* skip blank */
		tmp = strip(tmp);

		/* skip # */
		if(*tmp == '#' || *tmp == '\n') continue;

		/* get entry */
		nread = sscanf(tmp, "%s", entry);
		if(nread != 1) goto error;
		tmp = next_entry(tmp);
		if(*tmp != '=') goto error;
		tmp = next_entry(tmp);
		nread = sscanf(tmp, "%s", value);
		if(nread != 1) goto error;
		sys_debug("%s = %s \n", entry, value);

		if(!strcmp(entry, APCONF)) {
			if(value[strlen(value) - 1] == '"') 
				value[strlen(value) - 1] = 0;
			if(value[0] == '"')
				strcpy(&argument.apconf[0], &value[1]);
			else
				strcpy(&argument.apconf[0], &value[0]);
			continue;
		}

		if(!strcmp(entry, MINSIG)) {
			argument.minsig = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, AGETIME)) {
			argument.agetime = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, LOSTTIME)) {
			argument.losttime = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, DIS)) {
			argument.dis = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, SIG)) {
			argument.sig = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, FAC)) {
			argument.fac = strtol(value, NULL, 10);
			continue;
		}

		if(!strcmp(entry, DRIFT)) {
			argument.drift = strtol(value, NULL, 10);
			continue;
		}
	}

free:
	if(pline) free(pline);
	fclose(fp);
	return;

error:
	sys_err("%s[%d]: process failed\n", 
		SERCONF, number);
	exit(-1);
}

void proc_arg(int argc, char *argv[])
{
	__early_init(argc, argv);
	proc_confcmd();
	proc_cmdarg(argc, argv);
}

