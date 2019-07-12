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
	int ft1 = open(argv[1],O_RDWR | O_APPEND);	
	if(ft1<0){
		perror("open error");
		return -1;
	}	

	write(ft1,"hello world",strlen("hello world"));

	//使用dup打开相同文件
	int ft2 = fcntl(ft1,F_DUPFD,0);
	
	//lseek使文件操作符移动回文件开始的位置
	lseek(ft2,0,SEEK_SET);

	//使用ft2读取文件
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	int n = read(ft2,buf,sizeof(buf));
  	printf("ft1=[%d],ft2=[%d]\n",ft1,ft2);
	printf("[%s]\n",buf);
	
	close(ft1);
	close(ft2);

}

