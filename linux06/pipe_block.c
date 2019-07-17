#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>

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
		close(fd[1]);
		//sleep(5);//测试pipe中的读写是否是堵塞的
		//write(fd[1],"hellow",strlen("hellow"));
			
	}
	if(pid == 0)
	{
		//关闭写端
		close(fd[1]);
		int flag = fcntl(fd[0], F_GETFL, 0);
		flag |= O_NONBLOCK;
		fcntl(fd[0], F_SETFL, flag);
		char buf[1024];
		memset(buf,0x00,sizeof(buf));
		int res = read(fd[0],buf,sizeof(buf));
		printf("this is child,res = [%d],buf == [%s]\n",res,buf);
	}

	
	return 0;

}
