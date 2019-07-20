#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void signalhandle(int sign)
{
		printf("sign == [%d]\n",sign);
}

int main(int argc,char* argv[])
{
	
	//绑定定时器的信号
	signal(SIGALRM,signalhandle);
	
	//int setitimer(int which, const struct itimerval *new_value,
	//                     struct itimerval *old_value);
	struct itimerval it;

	//设置第一次执行时间
	it.it_value.tv_sec = 3;
	it.it_value.tv_usec = 0;

	//设置闹钟触发周期
	it.it_interval.tv_sec = 5;
	it.it_interval.tv_usec = 0;

	setitimer(ITIMER_REAL,&it,NULL);

	//防止程序结束
	while(1)
	{
		sleep(1);
	}	
	return 0;
}
