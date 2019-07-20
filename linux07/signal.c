#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
void  signhandle(int si)
{
	printf("signId == [%d]\n",si);
}

int main(int argc,char* argv[])
{
	int fd[2];
	int pi = pipe(fd);
	if(pi != 0)
	{
		perror("pipe error");
		return -1;
	}
	
	//绑定信号
	//sighandler_t signal(int signum, sighandler_t handler);
	signal(SIGPIPE,signhandle);
	
	//关闭所有读端，然后内核会抛出sigpipe的信号
	close(fd[0]);

	write(fd[1],"helllo",strlen("hello"));

	
	return 0;
}
