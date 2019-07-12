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
	lstat(argv[1],&st);
	printf("file_size = [%d],file_uid = [%d],file_gid=[%d]\n",st.st_size,st.st_uid,st.st_gid);	
	if((st.st_mode & S_IFLNK) == S_IFLNK){
		printf("this is link file\n");
	}else if((st.st_mode & S_IFREG) == S_IFREG ){
		printf("this is reg file\n");
	}
	return 0;

}
