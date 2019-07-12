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
	printf("file_size = [%d],file_uid = [%d],file_gid=[%d]\n",st.st_size,st.st_uid,st.st_gid);	
	return 0;
}
