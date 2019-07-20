#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
		

	//设置定时器
	int sec = alarm(6);
	printf("sec == [%d]\n",sec);//0,开始的时候为0

	sleep(2);
	int sec1 = alarm(8);
	printf("sec == [%d]\n",sec1);//4，剩余的秒数	

	
	int sec3 = alarm(0);
	printf("sec == [%d]\n",sec3);//剩余的描述

	return 0;
}
