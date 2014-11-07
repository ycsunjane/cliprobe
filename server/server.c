/*
 * ============================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年07月26日 10时58分33秒
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
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

#include "cmdarg.h"
#include "log.h"
#include "wnic.h"
#include "clihash.h"
#include "apsample.h"
#include "position.h"

#ifndef DISABLE_MYSQL
#include "sql.h"
MYSQL sql;
#endif

static void term_ser(int signum)
{
	sys_debug("SIGTERM received\n");
#ifndef DISABLE_MYSQL
	sql_close(&sql);
#endif
	exit(0);
}

static void init_signal()
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = term_ser;
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL); 
}

static int init_udp()
{
	int recvsock = socket(AF_INET, SOCK_DGRAM, 0);
	if(recvsock < 0) {
		sys_err("Create socket failed: %s\n", strerror(errno));
		return -1;
	}

	int ret;
	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_port = htons(SERPORT);
	ret = bind(recvsock, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	if( ret < 0) {
		sys_err("Bind addr failed: %s\n", strerror(errno));
		return -1;
	}
	return recvsock;
}

int main(int argc, char *argv[])
{
	int nread, num, i;
	int recvsock;

	proc_arg(argc, argv);
	ap_sample_init();
	recvsock = init_udp();
	cli_hash_init();
#ifndef DISABLE_MYSQL
	sql_init(&sql);
#endif
	init_signal();
	pthread_pos_init();

	int ret;
	if(daemon_mode)  {
		ret = daemon(0, 0);
		if(ret < 0) {
			sys_err("daemon mode failed: %s\n", strerror(errno));
			exit(-1);
		}
	}

	struct wnic_t *wnic = malloc(sizeof(struct wnic_t) * SENDSIZE);
	if(wnic == NULL) {
		sys_err("Malloc failed\n");
		return -1;
	}

	while(1) {
		nread = recvfrom(recvsock, wnic, 
			sizeof(struct wnic_t) * SENDSIZE, 0, NULL, NULL);
		if(nread < 0) 
			sys_err("Recive failed\n");
		if(nread == 0) continue;
		
		assert((nread % sizeof(struct wnic_t)) == 0);

		num = nread / sizeof(struct wnic_t);

		for(i = 0; i < num; i++) {
			cli_update(&wnic[i], &sql);
			if(debug > 1) {
				pr_mac1((unsigned char *)&wnic[i].apmac);
				pr_mac1((unsigned char *)&wnic[i].climac);
				pure_info("signal:%d\n", wnic[i].signal);
			}
		}
	}

	return 0;
}
