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
	
	//调用dup2拷贝oldfile 到 newfile
	dup2(oldfile,1);
	//也可以写成
	//dup2(oldfile,STDOUT_FILENO);
	printf("STDOUT_FILENO IS OUT");	
	close(oldfile);
	
}
