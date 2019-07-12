//stat的函数测试：获取文件大小，文件属性和组
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char *argv[])
{
	//int stat(const char *pathname, struct stat *buf);
	struct stat st;
	stat(argv[0],&st);
	if(st.st_mode & S_IXOTH){
		printf("other have s_ixoth\n");
	}
	if(S_ISREG(st.st_mode)){
		printf("file is reg\n");
	}
	return 0;
}
