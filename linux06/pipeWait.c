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
		pid_t cpid = fork();
		//父进程
		if(cpid>0)
		{
			int status;
			int waitid = waitpid(pid,&status,0);
			if(WIFEXITED(status))
			{
				printf("正常退出，status = [%d]",WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status))
			{
				printf("异常退出，status = [%d]",WTERMSIG(status));
			}	
		}
		else if(cpid == 0)//子进程execlp直接execlp
		{
			execlp("ps","ps","aux",NULL);
			printf("ps aux");
			perror("children1 execpl fail");
			return -1;
		}

	}
	if(pid == 0)
	{
		//关闭写端
		close(fd[1]);
		dup2(fd[0],0);
		sleep(5);
		printf("children0 grep bash");
		execlp("grep","grep","bash",NULL);
	}

	
	return 0;

}
