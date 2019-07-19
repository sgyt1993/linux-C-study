//使用mmap建立匿名映射
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/mman.h>

int main(int argc,char* argv[])
{
	//使用mmap匿名建立映射关系
	void * mm = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	//建立线程
	pid_t pid  = fork();

	if(pid < 0)
	{
		perror("fork fail");
		return -1;
	}
	else if(pid > 0)
	{
		memcpy(mm,"hello",strlen("hello"));
		int status;
		wait(&status);
	}
	else if(pid == 0)
	{
		char *p = (char *)mm;
		printf("[%s]\n",p);
	}
	return 0;

}
