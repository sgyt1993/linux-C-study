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
	int sec = alarm(1);
	int i = 0;
	while(1)
	{
		printf("i == [%d]\n",i++);
	}

	return 0;
}
