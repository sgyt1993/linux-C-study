//目录操作测试:opendir readdir closedir
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>

int main(int argc,char* argv[])
{
	//开打文件
	DIR *dirs = NULL;
	dirs = opendir(argv[1]);
	if(dirs == NULL){
		printf("open dir error");
		return -1;
	}
	//循环读取目录中文件名称
	struct dirent *rent=NULL;
	while((rent=readdir(dirs))!= NULL){
		if(strcmp(rent->d_name,".") == 0 || strcmp(rent->d_name,"..") == 0){
			continue;
		}
		printf("文件名词[%s]\n",rent->d_name);
		switch(rent->d_type)
		{
			case DT_REG:
				printf("普通软件");
				break;
			case DT_DIR:
				printf("目录文件");
				break;
			case DT_LNK:
				printf("链接文件");
				break;
			default:
				printf("其他文件");
		}
	}
	closedir(dirs);	
	return 0;
}
