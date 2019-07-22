#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc ,char * argv[])
{
	//发送 SIGINT 信号到sigCom01.c中
	kill(atoi(argv[1]),SIGINT);
	return 0;

}

