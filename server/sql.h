/*
 * =====================================================================================
 *
 *       Filename:  mysql.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年07月28日 10时31分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SQL_H__
#define __SQL_H__

#ifndef DISABLE_MYSQL
#include <my_global.h>
#include <mysql.h>
#include "log.h"

#define SQL 		MYSQL
#define DBNAME 		"cliprobe"
#define TABLENAME 	"clipos"
#define USER 		"user"
#define CLIDB 		"CREATE DATABASE IF NOT EXISTS " DBNAME
#define CLITABLE 	"CREATE TABLE IF NOT EXISTS " TABLENAME"(climac BIGINT NOT NULL, macstr CHAR(17) NOT NULL, x DOUBLE, y DOUBLE, z DOUBLE, time TIMESTAMP);"
#define USERTABLE 	"CREATE TABLE IF NOT EXISTS " USER"(climac BIGINT NOT NULL, macstr CHAR(17) NOT NULL, first_time DATETIME NOT NULL, last_time DATETIME NOT NULL, primary key (climac));"
#define INSERTINFO 	"INSERT INTO " TABLENAME "(climac, macstr, x, y, z) VALUES("
#define INSERTUSER 	"REPLACE INTO " USER"(climac, macstr, first_time, last_time) VALUES("

#define pr_sqlerr()  \
	sys_err("Error %u: %s\n", mysql_errno(sql), mysql_error(sql));


extern MYSQL sql;
int sql_init();
void sql_close(SQL *sql);
void sql_insert(SQL *sql, uint64_t climac, 
	double x, double y, double z);
void sql_usr_insert(SQL *sql, uint64_t climac, 
	struct timeval first_time, struct timeval last_time);
#endif
#endif /* __SQL_H__ */
