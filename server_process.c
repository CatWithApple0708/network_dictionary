#include "server_process.h"
#include "dbuser.h"

extern int server_process(int readfd,int writefd){


  USER user={-1,"",""};
  char order_buff[ORDER_MAX_LEN]="";
  wrong_table_enum_t wrong_number=0;

  //open database;
  sqlite3 *db=NULL;
  Sqlite3_open(DB_FILE_PATH,&db);

  //open file
  FILE*filedb=NULL;
  filedb=fopen(DIC_FILE_PATH,"r");

  int whileflag=1;
  do
  {
    order_table_enum_t order=get_order(readfd,order_buff,sizeof(order_buff));
    switch(order)
    {
      case OT_LOGIN: wrong_number=login_process(db,order_buff,&user); break;

      case OT_REGIS: wrong_number=regis_process(db,order_buff); break;

      case OT_FIND: wrong_number=find_process(filedb,writefd,db,order_buff,&user); break;

      case OT_HISTORY: wrong_number=history_process(db,order_buff,&user); break;

      case OT_HELP: wrong_number=help_process(writefd); break;

      case OT_NETWORK_WRONG: wrong_number=WT_NETWORK;break;

      case OT_ORDER_WRONG: wrong_number=WT_FORMAT_WRONG;break;

      case OT_EXIT: wrong_number=WT_EXIT; break;

      default:printf("DEBUG: 未知switch 错误\n"); break;
    }
    fseek(filedb,0,SEEK_SET);
    whileflag=wrong_process(writefd,wrong_number);
  }
  while(whileflag);

  //关闭数据库资源 关闭文件
  fclose(filedb);
  sqlite3_close(db);

  return 1;

}


static order_table_enum_t get_order(int readfd,char *order_buff,int len)
{
  order_table_enum_t ret=OT_LOGIN;
  //如果网络读入错误，返回网络读入错误参数
  int read_ret=network_read(readfd,order_buff,len);
    if (read_ret<0)
    {
      ret=OT_NETWORK_WRONG;
      return ret;
    }
    order_buff[read_ret]='\0';

  //comp order
  for (int i = 0; order_table_string[i] != NULL; ++i)
  {
    if ( equal_order(order_table_string[i],order_buff)==1 )
    {
      return (order_table_enum_t)i;
    }
  }

  ret=OT_ORDER_WRONG;
  return ret;
}



static wrong_table_enum_t
login_process(sqlite3 *db,char *order_buff,USER *user)
{
  printf("DEBUG:login_process\n");
  char *buff[3];
  buff[0]=strtok(order_buff," ");
  buff[1]=strtok(NULL," ");
  buff[2]=strtok(NULL," ");

  if (buff[0]==NULL||buff[1]==NULL||buff[2]==NULL)
  {
    return WT_FORMAT_WRONG;
  }
  if (strlen(buff[1])>=20)
  {
    return WT_USER_NAME_TOO_LONG;
  }
  if (strlen(buff[2])>=30)
  {
    return WT_PASSWD_TOO_LONG;
  }
  strcpy( &(user->name[0]),buff[1]);
  strcpy( &(user->passwd[0]),buff[2]);

  fprintf(stderr,"DEBUG:user name:%s user passwd:%s\n",user->name,user->passwd);

  if(login(user,db)<0)
    return WT_PASSWD_WRONG;

  return WT_RIGHT;
}

static wrong_table_enum_t
regis_process(sqlite3 *db,char *order_buff)
{
  USER user;
  char *buff[3];
  buff[0]=strtok(order_buff," ");
  buff[1]=strtok(NULL," ");
  buff[2]=strtok(NULL," ");

  if (buff[0]==NULL||buff[1]==NULL||buff[2]==NULL)
  {
    return WT_FORMAT_WRONG;
  }
  if (strlen(buff[1])>=20)
  {
    return WT_USER_NAME_TOO_LONG;
  }
  if (strlen(buff[2])>=30)
  {
    return WT_PASSWD_TOO_LONG;
  }

  strcpy( &(user.name[0]),buff[1]);
  strcpy( &(user.passwd[0]),buff[2]);

  int regis_ret=regis(&user,db);
  if (regis_ret!=0)
  {
    return WT_USER_NAME_REDUDANT;
  }

  printf("DEBUG:regis_process\n");
  wrong_table_enum_t ret=WT_RIGHT;
  return ret;
}


static wrong_table_enum_t
find_process(FILE * dictionary,int writefd,sqlite3*db, char *order_buff,const USER *user)
{
  printf("DEBUG:find_process\n");
  if (user->login_flag!=HASLOGINED)
  {
    return WT_NOT_REGISTERTED;
  }

  char *p[3];
  p[0]=strtok(order_buff," ");
  p[1]=strtok(NULL," ");
  p[2]=strtok(NULL," ");
  if (p[0]==NULL||p[1]==NULL)
  {
    return WT_FORMAT_WRONG;
  }
  fprintf(stderr,"DEBUG:order:%s word:%s\n", p[0],p[1]);

  char description[1024];
  wrong_table_enum_t ret=find(p[1],description,sizeof(description)-2,dictionary);
  if (ret==WT_RIGHT)
  {
    insert_history(db,user,p[1]);
    network_write(writefd, description,strlen(description));
    // network_write(writefd,\n,1);
  }
  // printf("here\n");

  return ret;
}

static wrong_table_enum_t
history_process(sqlite3 *db,char *order_buff,const USER *user)
{
  printf("DEBUG:history_process\n");
  wrong_table_enum_t ret=WT_RIGHT;
  return ret;
}

static wrong_table_enum_t
help_process(int writefd)
{
  printf("DEBUG:help_process\n");
  printf("指令:\nregis username passwd\nlogin username passwd\nfind word\nhistory(没有实现 但histoy可以插入 查看没有实现)\nhelp\n");
  wrong_table_enum_t ret=WT_RIGHT;
  return ret;
}


static wrong_table_enum_t
wrong_process(int writefd,wrong_table_enum_t wrong_number)
{
  network_write(  writefd,
                  wrong_table_string[wrong_number],
                  strlen(wrong_table_string[wrong_number])
                );
//当发生网络问题的时候 可能是信息的格式不正确，可能是网络断开
//此时服务器结束服务 断开网络
  if (wrong_number==WT_NETWORK||wrong_number==WT_EXIT)
    return 0;
  else
    return 1;
}





//two layer
static int
equal_order(const char *order,const char *buff)
{
  int i=0;
  for (i = 0; i < strlen(order); ++i)
  {
    if (order[i]!=buff[i])
    {
      return 0;
    }
  }

  return 1;
}

/*
@
@src word
@des description
@len the length of des
系统错误直接输出到stderr且返回－1  用户错误 返回相应的错误编号
 */


static wrong_table_enum_t
find(const char *word,char *description,size_t len,FILE *file)
{

  char buff[1024];
  char *p[3];
  while(1)
  {
    char * ret=fgets(buff,1023,file);
    if (ret==NULL)
       return WT_NOSUCH_WORD;
    int string_len=strlen(ret);

     p[0]=strtok(buff," ");
     p[1]=strtok(NULL," ");

     if (p[1]!=NULL&&p[1]+strlen(p[1])!=p[0]+string_len)
     {
       *( p[1]+strlen(p[1]))=' ';
     }


    if (strcmp(p[0],word) != 0)
      continue;
    else
      if( strncpy(description,p[1],len) != description )
        {fprintf(stderr, "the len of des is not vary big!\n");return WT_SYSTEM_CRASH;}
      else
        { return WT_RIGHT;}
  }
}

//network

static ssize_t
network_read(int fildes, void *buf, size_t nbyte)
{
  ssize_t ret;
  ret=read(fildes,buf,nbyte);
  if (ret<0)
  {
    perror("network_read wrong");
  }
  return ret-1;
}

static ssize_t
network_write(int fildes, void *buf, size_t nbyte)
{
  ssize_t ret;
  ret=write(fildes,buf,nbyte);
  // write(fildes,"\n",1);
  if (ret<0)
  {
    perror("network_write wrong");
  }
  return ret;
}

























