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
		dup2(fd[1],1);
		execlp("ps","ps","aux",NULL);
		perror("execpl fail");
	}
	if(pid == 0)
	{
		//关闭写端
		close(fd[1]);
		dup2(fd[0],0);
		execlp("grep","grep","bash",NULL);
	}

	
	return 0;

}
