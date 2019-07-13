#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc,char* argv[])
{
	int i = 0;
	for(i=0;i<3;i++)
	{
		//创造子进程
		int id = fork();
		if(id < 0)
		{
			perror("fork error\n");
			return -1;
		}
		else if(id > 0 )//父进程
		{
			printf("this is father, pid = [%d],ppid = [%d]\n",getpid(),getppid());
			sleep(1);
		}
		else if(id == 0)//子进程
		{	
			//child is not leep
			printf("this is children,pid = [%d],ppid = [%d]\n",getpid(),getppid());
			break;
		}
	}
	
	if(i == 0)
	{
		printf("i = [%d],pid = [%d] ",i,getpid());
	}
	else if(i == 1)
	{
		printf("i = [%d],pid = [%d] ",i,getpid());
	}
	else if(i==2)
	{ 	
		printf("i = [%d],pid = [%d] ",i,getpid());
	}
	else if(i == 3)	
	{	
		printf("i = [%d],pid = [%d] ",i,getpid());
	}	
	return 0;


}
