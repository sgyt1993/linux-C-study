#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
	int fd = open("./test.log", O_RDWR | O_CREAT ,0777);
	if(fd<0){
		perror("open error");
		return -1;
	}
	
	write(fd,"hello world",strlen("hellow world"));
	close(fd);	

	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	int n = read(fd,buf,sizeof(buf));
	printf(buf);
	close(fd);
}
