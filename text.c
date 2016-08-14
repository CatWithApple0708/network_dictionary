#include <stdio.h>
#include "dbuser.h"
#include "server_process.h"
int main(int argc, char* argv[])
{
   server_process(0,1);
   return 0;
}
