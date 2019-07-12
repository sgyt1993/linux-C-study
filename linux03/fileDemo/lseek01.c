//lseek  file size
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
	
	int fileSize = lseek(ft,0,SEEK_END);
	
	printf("fileSize=[%d]\n",fileSize);

	return 0;
}

