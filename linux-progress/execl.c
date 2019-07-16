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
		printf("this is father, pid = [%d],ppid = [%d]\n",getpid(),getppid());
		sleep(1);
	}
	else if(id == 0)//子进程
	{
		printf("this is children,pid = [%d],ppid = [%d]\n",getpid(),getppid());
		//子进程重新开启程序
		//execl("/bin/ls","ls","-1",NULL);
		//execlp("ls","ls","-1",NULL); 直接调用环境变量
		//execl("/opt/linuxCDemo/linux-C-study/linux-progress/testDemo","testDemo","Hello","World","ni mei",NULL);
		execlp("/opt/linuxCDemo/linux-C-study/linux-progress/testDemo","testDemo","Hello","World","ni mei",NULL);
		perror("execl error");
	}

	printf("after fork ,pid = [%d],ppid = [%d]\n",getpid(),getppid());

	return 0;


}
