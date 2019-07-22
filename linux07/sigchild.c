#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int signo)
{
	pid_t waitid = waitpid(-1,NULL,WNOHANG);
	
	if(waitid > 0)
	{
		printf("this is quit , pid  = [%d]\n",waitid);
	}
	else if(waitid == 0)
	{
		printf("this is living , pid = [%d]\n",waitid);
	}
	else if(waitid < 0 )
	{
		printf("this is no child \n");	
	}
}

int main(int argc,char* argv[])
{
	
	//加入子信号退出的捕获
	struct sigaction act;
	act.sa_handler = sighandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGCHLD,&act,NULL);
	
	//循环创造出3个字线程
	int i = 0;
	for(i = 0; i<3 ; i++)
	{
		pid_t pid = fork();
		if(pid < 0)
		{
			perror("fork error");
			return -1;
		}
		else if(pid > 0)
		{
			printf("this is fater , p_id = [%d],pp_id = [%d]\n",getpid(),getppid());	
		}
		else if(pid == 0)
		{
			printf("this is child , p_id = [%d],pp_id = [%d]\n",getpid(),getppid());
			break;	
		}

	}

	if(i == 0)
	{
		printf("i == [%d],pid == [%d]\n",i,getpid());
		sleep(2);
	}
	if(i == 1)
	{
		printf("i == [%d],pid == [%d]\n",i,getpid());
		sleep(2);
	}
	if(i == 2)
	{	
		printf("i == [%d],pid == [%d]\n",i,getpid());
		sleep(2);
	}
	if(i == 3)
	{
		printf("i == [%d],pid == [%d]\n",i,getpid());
		while(1)
		{
			sleep(1);
		}
	}

	
	return 0;	
}

