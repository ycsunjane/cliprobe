/*
 * ============================================================================
 *
 *       Filename:  mysql.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年07月28日 10时04分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * ============================================================================
 */
#ifndef DISABLE_MYSQL
#include <stdio.h>
#include <stdint.h>
#include <mysqld_error.h>
#include <string.h>
#include "sql.h"

static void __create_table(SQL *sql, char *cmd)
{
	int ret;
	ret = mysql_query(sql, cmd);
	if(ret) {
		pr_sqlerr();
		exit(-1);
	}
}

int sql_init(SQL *sql)
{
	int ret;
	void *state;

	printf("Mysql version:%s\n", mysql_get_client_info());

	sql = mysql_init(sql);
	if(sql == NULL) {
		pr_sqlerr();
		exit(-1);
	}

	mysql_options(sql, MYSQL_READ_DEFAULT_GROUP, "serprobe");
	state = mysql_real_connect(sql, NULL, NULL, NULL, NULL, 0, NULL, 0);
	if(state == NULL) {
		pr_sqlerr();
		exit(-1);
	}
	
	ret = mysql_query(sql, CLIDB);
	if(ret) {
		pr_sqlerr();
		exit(-1);
	}

	ret = mysql_select_db(sql, DBNAME);
	if(ret) {
		pr_sqlerr();
		exit(-1);
	}

	__create_table(sql, CLITABLE);
	__create_table(sql, USERTABLE);
	return 0;
}

void sql_close(SQL *sql)
{
	mysql_close(sql);
}


static int __mac(char *p, char *mac)
{
	int ret;
	ret = sprintf(p, "'%02x:%02x:%02x:%02x:%02x:%02x',",
		(unsigned char) mac[0],
		(unsigned char) mac[1],
		(unsigned char) mac[2],
		(unsigned char) mac[3],
		(unsigned char) mac[4],
		(unsigned char) mac[5]);
	return ret;
}

static int __double(char *p, double val)
{
	int ret;
	ret = sprintf(p, "%lf,", val);
	return ret;
}

static int __double_end(char *p, double val)
{
	int ret;
	ret = sprintf(p, "%lf);", val);
	return ret;
}

static char insert_buffer[1024] = INSERTINFO;
void sql_insert(SQL *sql, uint64_t climac, 
	double x, double y, double z)
{
	int ret;
	int headlen = strlen(INSERTINFO);
	char *p = &insert_buffer[0] + headlen;
	p += __double(p, climac);
	p += __mac(p, (char *)&climac);
	p += __double(p, x);
	p += __double(p, y);
	p += __double_end(p, z);
	ret = mysql_query(sql, insert_buffer);
	if(ret) {
		if(mysql_errno(sql) == ER_NO_SUCH_TABLE) {
			__create_table(sql, CLITABLE);
			return;
		}
		pr_sqlerr();
	}
	sys_debug("%s\n", insert_buffer);
}

int __datetime(char *p, time_t *time)
{
	int ret;
	char buff[22];
	strftime(buff, 22, 
		"'%Y-%m-%d %H:%M:%S'", localtime(time));;
	ret = sprintf(p, "%s,", buff);
	return ret;
}

int __datetime_end(char *p, time_t *time)
{
	int ret;
	char buff[22];
	strftime(p, 22, 
		"'%Y-%m-%d %H:%M:%S'", localtime(time));;
	ret = sprintf(p, "%s);", buff);
	return ret;
}

static char insert_usr[1024] = INSERTUSER;
void sql_usr_insert(SQL *sql, uint64_t climac, 
	struct timeval first_time, struct timeval last_time)	
{
	time_t ftime, ltime;

	ftime = (time_t)first_time.tv_sec;
	ltime = (time_t)last_time.tv_sec;

	strncpy(insert_usr, INSERTUSER, 1023);
	int headlen = strlen(INSERTUSER);
	char *p = &insert_usr[0] + headlen;

	p += __double(p, climac);
	p += __mac(p, (char *)&climac);
	p += __datetime(p, &ftime);
	p += __datetime_end(p, &ltime);

	int ret;
	ret = mysql_query(sql, insert_usr);
	if(ret) {
		if(mysql_errno(sql) == ER_NO_SUCH_TABLE) {
			__create_table(sql, USERTABLE);
			return;
		}
		pr_sqlerr();
	}
	sys_debug("%s\n", insert_usr);
}
#endif
