#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

void signhandler(int signCount)
{
	printf("sign == [%d]\n",signCount);
	sleep(3);
}

int main(int argc,char* argv[])
{
	//sigaction绑定 SIGINT 	信号
	struct sigaction act;
	act.sa_handler = signhandler;
	sigemptyset(&act.sa_mask);//初始化信号集
	sigaddset(&act.sa_mask,SIGQUIT);//添加在信号处理时，需要屏蔽的信号
	act.sa_flags = 0;
	sigaction(SIGINT,&act,NULL);
	
	signal(SIGQUIT,signhandler);//在处理sigaction中的信号时，signal的信号会等待sigaction处理完毕再执行处理

	while(1)
	{
		sleep(1);	
	}
	
	return 0;
}
