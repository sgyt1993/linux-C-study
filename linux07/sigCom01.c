#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int signo)
{
	printf("signo == [%d]\n",signo);	
}

int main(int argc ,char * argv[])
{
	struct sigaction act;
	act.sa_handler = sighandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT,&act,NULL);
	

	while(1)
	{
		sleep(1);	
	}
}
