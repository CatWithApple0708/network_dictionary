#include "dbuser.h"
#include <sqlite3.h>


int
Sqlite3_open(const char *filename, sqlite3 **ppDb)
{
   int rc = sqlite3_open(filename, ppDb);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*ppDb));
      return rc;
   }else{
      // fprintf(stdout, "Opened database successfully\n");
   }
   return 0;
}


int
Sqlite3_exec(sqlite3*db, const char *sql, int (*callback)(void*,int,char**,char**), void *data)
{
   char *zErrMsg=NULL;
   int rc = sqlite3_exec(db, sql, callback, data, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return rc;
   }else{
      // fprintf(stdout, "Table created successfully\n");
      return 0;
   }
}

static int
login_callback(void *user, int argc, char **argv, char **azColName){
   // printf("login_callback is do\n");
   ( (USER *)user)->login_flag=HASLOGINED;
   return 0;
}


//return  values of the
int
regis(const USER *user,sqlite3*db)
{
   char buff[200];
   char *table_name=USER_TABLE_NAME;
   sprintf(buff,"insert into %s values('%s','%s');",table_name,user->name,user->passwd);
   // printf("%s\n", buff);
   int ret = Sqlite3_exec(db,buff,NULL,NULL);
   // if (ret==)
   // {
   //   /* code */
   // }
   if (ret!=SQLITE_OK)
      return -1;
   else
      return SQLITE_OK;
}

//login success return 0 if login not
int
login( USER *user,sqlite3*db)
{
  char buff[200];
  char *table_name=USER_TABLE_NAME;
  sprintf(buff,"select * from %s where name = '%s' and passwd = '%s' ",table_name,user->name,user->passwd);
  int ret = Sqlite3_exec(db,buff,login_callback,(void *)user);
  if (ret!=SQLITE_OK)
  {
     //do something
     return -1;
  }

  if (user->login_flag==-1)
  {
     return -1;
  }
  return 0;
}

int
insert_history(sqlite3*db,const USER *user,const char *history)
{
   if (user->login_flag==HASLOGINED)
   {
   char buff[200];
   char *table_name=HISTORY_TABLE_NAME;
   sprintf(buff,"insert into %s values('%s','%s');",table_name,user->name,history);
   // printf("%s\n", buff);
   int ret = Sqlite3_exec(db,buff,NULL,NULL);
   if (ret!=SQLITE_OK)
      return ret;
   else
      return ret;
   }
   else
      return -1;

}































