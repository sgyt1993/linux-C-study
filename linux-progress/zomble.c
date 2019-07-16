#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc,char* argv[])
{
	printf("befor fork,pid = [%d],ppid = [%d]\n",getpid(),getppid());
	//创造子进程
	int id = fork();
	if(id < 0)
	{
		perror("fork error\n");
		return -1;
	}
	else if(id > 0 )//父进程
	{
		sleep(100);
		printf("this is father, pid = [%d],ppid = [%d]\n",getpid(),getppid());
	}
	else if(id == 0)//子进程
	{
		printf("this is children,pid = [%d],ppid = [%d]\n",getpid(),getppid());
	}

	printf("after fork ,pid = [%d],ppid = [%d]\n",getpid(),getppid());

	return 0;


}
