//pipe管道的读写行为
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
	//关闭所有的读端
	close(fd[0]);
	write(fd[1],"hello",strlen("hello"));
	perror("write error");
	return 0;

}
