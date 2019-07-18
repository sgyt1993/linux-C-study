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
	int openid = open("mkfile",O_RDWR | O_CREAT,0777);
	//fifo使文件变成链接文件
	mkfifo("mkfile",0777);

	pid_t pid = fork();
	if(pid >0 )
	{
		write(openid,"hellow",strlen("hellow"));	
	}
	else if(pid == 0 )
	{
		char buf[1024];
		memset(buf,0x00,sizeof(buf));
		read(openid,buf,sizeof(buf));
		printf("mkfile  == [%s]\n",buf);
	}

}
