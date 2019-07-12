#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char* argv[])
{
	//打开oldfile,newfile
	int oldfile = open(argv[1],O_RDWR | O_CREAT,0755);
	if(oldfile<0){
		perror("open oldfile error");
		return -1;
	}
	int newfile = open(argv[2],O_RDWR | O_CREAT,0755);
	if(newfile<0)
	{
		perror("open newfile error");
		return -1;
	}
	
	//调用dup2拷贝oldfile 到 newfile
	//int dupNum = dup2(oldfile,newfile);
	dup2(oldfile,newfile);
	
	//在newfile中写入，dupnu读
	write(newfile,"sgyt come",strlen("sgyt come"));
	
	//将文件操作符移动回头部
	lseek(newfile,0,SEEK_SET);
	
	//使用dupNum读入
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	read(oldfile,buf,sizeof(buf));
	printf("read [%s]",buf);
	
	//关闭文件
	close(oldfile);
	close(newfile);
	
}
