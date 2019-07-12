//文件复制指向的文件和open打开的文件是同一个文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char* argv[])
{
	//打开文件
	int ft1 = open(argv[1],O_RDWR);	
	if(ft1<0){
		perror("open error");
		return -1;
	}	

	//使用dup打开相同文件
	int flag = fcntl(ft1,F_GETFL,0);
	flag = flag | O_APPEND;
	fcntl(ft1,F_SETFL,flag);
	write(ft1,"sgyt ok",strlen("sgyt ok"));
	close(ft1);


}

