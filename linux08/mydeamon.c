#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>

void sighandler(int signo)
{
	//打开文件
	pid_t openId = open("deamno.log", O_RDWR | O_CREAT | O_APPEND , 0777);
	
	//写入时间
	time_t t;
	time(&t);
	char *p = ctime(&t);
	
	write(openId,p,strlen(p));

	close(openId);
}

int main(int argc, char* argv[])
{
	pid_t pid = fork();
	if(pid < 0 )
	{
		perror("fork error");	
	}
	
	if(pid > 0)
	{
		return 0;	
	}

	if(pid == 0 )
	{
		//创建新的会话组
		setsid();

		//改变当前工作目录
		chdir("/usr/sgyt/");

		//重新设置文件掩码
		umask(0000);

		//关闭文件描述符
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		//执行核心代码
		//绑定时间事件
		struct sigaction act;
		act.sa_handler = sighandler;
		act.sa_flags  = 0;
		sigemptyset(&act.sa_mask);
		sigaction(SIGALRM,&act,NULL);

		//设置时间计时器
		struct itimerval val;
		val.it_interval.tv_sec = 3;
		val.it_interval.tv_usec = 0;
		val.it_value.tv_sec = 2;
		val.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL,&val,NULL);

		printf("hello world");//判断守护进程是否与控制台交互

		while(1)
		{
			sleep(1);	
		}
	}
}
