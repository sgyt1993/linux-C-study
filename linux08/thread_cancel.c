//线程手动退出
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void *threadHandler(void *arg)
{
	while(1)
	{
		int a;//这个函数没有调用系统内核取消点
	}
}   


int main(int argc,char* argv[])
{
	//创建子线程
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	//                          void *(*start_routine) (void *), void *arg);
	pthread_t thread;
	int res = pthread_create(&thread,NULL,threadHandler,NULL);
	if(res != 0)
	{
		printf("thread create error");
		return -1;
	}   
	
	printf("main thread,pid == [%d],thread_id == [%ld]\n",getpid(),pthread_self());

	//取消子线程，但是子线程必须达到某个取消点，才能自己结束
	pthread_cancel(thread);
	
	pthread_join(thread,NULL);

 	sleep(1);//等待子线程结束

	return 0;   
}
