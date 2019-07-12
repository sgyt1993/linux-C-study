#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	read(STDIN_FILENO, buf, sizeof(buf));
	printf("buf = [%s]\n",buf);
}
