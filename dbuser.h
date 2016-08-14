#ifndef DBUSER_H
#define DBUSER_H

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
/*

CREATE TABLE user_info(name string,passwd string);

name|passwd
string|string

 */



#ifndef USER_S
#define USER_S
typedef struct user
{
  short login_flag;  //当用户登录之后这个位置设置为1
  char  name[20];
  char  passwd[30];
}USER;
#endif


#define USER_TABLE_NAME "user_info"
#define HISTORY_TABLE_NAME "history"
#define HASLOGINED 1



static int
login_callback(void *user, int argc, char **argv, char **azColName);

extern int
Sqlite3_open(const char *filename, sqlite3 **ppDb);

extern int
Sqlite3_exec(sqlite3*db, const char *sql, int (*callback)(void*,int,char**,char**), void *data);

extern int
regis(const USER *user,sqlite3*db);

//成功返回 0失败返回-1
extern int
login(USER *user,sqlite3*db);


extern int
insert_history(sqlite3*db, const USER *user,const char *history);


#endif

















