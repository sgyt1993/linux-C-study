#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void signalhandler(int sign)
{
	printf("signal == [%d]\n",sign);	
}

int main(int argc,char* argv[])
{
	//signal注册信号
	signal(SIGINT,signalhandler);
	signal(SIGQUIT,signalhandler);
	
	//初始化signal
	// int sigemptyset(sigset_t *set);
    sigset_t set;
	sigset_t old;
	sigemptyset(&set);

	//加入信号
	sigaddset(&set,SIGINT);
	sigaddset(&set,SIGQUIT);

	//信号加入阻塞
	//sigprocmask(SIG_BLOCK,&set,NULL);
	sigprocmask(SIG_BLOCK,&set,&old);

	int i = 0;
	int j = 1;
	sigset_t pand;
	//打印出阻塞的信号,每打印10次，唤醒这个信号
	while(1)
	{
		
		//拿到当前进程的未决信号
		sigpending(&pand);
		for(i =1;i<31;i++)
		{
			if(sigismember(&pand,i))
			{
				printf("1");	
			}
			else
			{
				printf("0");
			}
		}
		
		printf("\n");
		
		//当循环10次，唤醒信号
		if(j++%10 == 0)
		{
			//sigprocmask(SIG_UNBLOCK,&set,NULL);	
			sigprocmask(SIG_SETMASK,&old,NULL);
		}else
		{
			sigprocmask(SIG_BLOCK,&set,NULL);	
		}

		
		sleep(1);
	}

	
	return 0;
}
