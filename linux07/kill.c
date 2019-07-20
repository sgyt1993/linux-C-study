#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
	

	kill(getpid(),SIGKILL);

	perror("error");
	
	return 0;
}
