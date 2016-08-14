#include <stdio.h>
#include "server_process.h"

int main(int argc, char const *argv[])
{
	printf("this is main\n");
	server_process(0,1);
	return 0;
}