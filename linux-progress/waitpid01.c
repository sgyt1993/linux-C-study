#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc,char* argv[])
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
		int status;
		while(1)
		{
			//不阻塞
			pid_t waitid = waitpid(0,&status,WNOHANG);
			if(waitid == 0)
			{	
				//printf("子进程正在进行中\n");
			}
			else if(waitid > 0 )
			{
				printf("子进程被回收，waitid == [%d]",waitid);
		        	printf("this is father,forkId = [%d], pid = [%d], ppid = [%d], waitId = [%d]\n",id,getpid(),getppid(),waitid);
			
				if(WIFEXITED(status))
				{

					printf("正常退出 status == [%d]\n",WEXITSTATUS(status));
				}	
				else if(WIFSIGNALED(status))
				{
			 		printf("异常终止 status == [%d]\n",WTERMSIG(status));
				}
			}
			else if(waitid == -1)
			{
				printf("子线程结束\n");
				break;
			}
		}	
	}
	else if(id == 0)//子进程
	{
		printf("this is children,forkId = [%d],pid = [%d],ppid = [%d]\n",id,getpid(),getppid());
		sleep(20);//看清楚状态
	}


	return 0;


}
