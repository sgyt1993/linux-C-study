//lseek 100 add H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h> 

int main()
{
	int ft = open("./test.log",O_RDWR | O_CREAT );

	lseek(ft,100,SEEK_SET);

	write(ft,"H",strlen("H"));

	close(ft);

	return 0;
}
