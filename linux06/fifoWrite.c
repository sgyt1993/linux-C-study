#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include <fcntl.h>
int main(int argc,char* argv[])
{
	//创建一个fifo文件
	int ret = access("mkfile",F_OK);
	if(ret != 0)
	{
		int mk = mkfifo("mkfile",0777);
		if(mk<0)
		{
			perror("mkfife error");
		}
	}
	
	//打开文件
	int openid = open("mkfile",O_RDWR | O_CREAT,0777);
	if(openid < 0 )
	{
		perror("open error");
	}

	pid_t pid = fork();
	if(pid >0 )
	{
		write(openid,"hellow",strlen("hellow"));	
		int status;
		pid_t pid = wait(&status);
		if(WIFEXITED(status))
		{
			printf("正常退出进程  status == [%d]\n",WEXITSTATUS(status));
		}
		else if(WIFSIGNALED(status))
		{
			printf("异常退出进程  status == [%d]\n",WTERMSIG(status));
		}
	}
	else if(pid == 0 )
	{
		sleep(1);
		char buf[1024];
		memset(buf,0x00,sizeof(buf));
		read(openid,buf,sizeof(buf));
		printf("mkfile  == [%s]\n",buf);
	}

}
