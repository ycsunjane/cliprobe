/*
 * =====================================================================================
 *
 *       Filename:  wnic.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年07月25日 09时44分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __WNIC__
#define __WNIC__

#include "stdint.h"

#define SERPORT (7231)
#define SENDSIZE (10)

#define CREATE_MON_DEV 	"iw dev %s interface add %s type monitor flags none"
#define DEL_MON_DEV 	"iw dev %s del"
#define UP_MON_DEV 	"ifconfig %s up"

struct wnic_t {
	char signal;
	uint64_t apmac;
	uint64_t climac;
}__attribute__((packed));

struct probe_pkt_t {
	uint8_t type;
	uint8_t flag;
	uint16_t dur;
	char dmac[6];
	char smac[6];
	char bssid[6];
};

#endif /* __WNIC__ */
