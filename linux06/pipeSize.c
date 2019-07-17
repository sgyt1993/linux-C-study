#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc,char* argv)
{
	//pipe创建管道
	int fd[2];
	int result = pipe(fd);
	if(result == -1)
	{
		perror("pipe fail");
		return -1;
	}

	//fork出一个进程
	pid_t pid = fork();

	if(pid > 0)
	{
		//关闭读端
		close(fd[0]);
		sleep(5);//测试pipe中的读写是否是堵塞的
		write(fd[1],"hellow",strlen("hellow"));
		printf("pipe size = [%ld]\n",fpathconf(fd[1],_PC_PIPE_BUF));		
	}
	if(pid == 0)
	{
		//关闭写端
		close(fd[1]);
		char buf[1024];
		memset(buf,0x00,sizeof(buf));
		read(fd[0],buf,sizeof(buf));
		printf("this is child ,buf == [%s]\n",buf);
		printf("pipe size = [%ld]\n",fpathconf(fd[0],_PC_PIPE_BUF));		
	}

	
	return 0;

}
