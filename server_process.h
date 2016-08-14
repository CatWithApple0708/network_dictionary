#ifndef SERVER_PROCESS_H
#define SERVER_PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>


#define ORDER_MAX_LEN 100
#define DB_FILE_PATH "."

// 命令

typedef enum order_table_enum{
OT_LOGIN=0,
OT_REGIS,
OT_FIND,
OT_HISTORY,
OT_HELP,
OT_EXIT,
//wrong return no specific order string
OT_ORDER_WRONG,
OT_NETWORK_WRONG
}order_table_enum_t;// number table

static char *order_table_string[]={
  "login ",
  "regis ",
  "find ",
  "history",
  "help",
  "exit",
  NULL
};



// 错误信息
//单词 重复:redudant
typedef enum wrong_table_enum{
  WT_RIGHT=0,
  WT_FORMAT_WRONG,
  WT_PASSWD_WRONG,
  WT_USER_NAME_REDUDANT,
  WT_NETWORK,
  WT_EXIT
}wrong_table_enum_t;// number table

static char *wrong_table_string[]={
  "",
  "order format_wrong",
  "passwd or username is wrong",
  "User name has been registered",
  "Please restart net work",
  "Exit success",
  NULL
};


// 用户结构体

typedef struct user
{
  short login_flag;  //当用户登录之后这个位置设置为1
  char  name[20];
  char  passwd[30];
}USER;



extern int server_process(int readfd,int writefd);


static wrong_table_enum_t
login_process(sqlite3 *db,char *order_buff,USER *user);

static wrong_table_enum_t
regis_process(sqlite3 *db,char *order_buff,const USER *user);

static wrong_table_enum_t
find_process(int fd,char *order_buff,const USER *user);

static wrong_table_enum_t
history_process(sqlite3 *db,char *order_buff,const USER *user);

static wrong_table_enum_t
help_process(int writefd);




static order_table_enum_t
get_order(int readfd,char *order_buff,int len);

static wrong_table_enum_t
wrong_process(int writefd,wrong_table_enum_t wrong_number);



//two layer
static ssize_t
network_read(int fildes, void *buf, size_t nbyte);


static ssize_t
network_write(int fildes, void *buf, size_t nbyte);

static int
equal_order(const char *order,const char *buff);



#endif

