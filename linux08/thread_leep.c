//线程生成的demo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void *threadHandler(void *arg)
{
	int i = *(int *)arg;
	printf("child thread,i == [%d],pid==[%d],thread_id == [%ld]\n",i,getpid(),pthread_self());
}   


int main(int argc,char* argv[])
{

	int i = 0;
	int arg[5];
	memset(arg,0x00,sizeof(arg));
	//循环创建子线程
	for(i=0;i<5;i++)
	{
		//创建子线程
		//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		//                          void *(*start_routine) (void *), void *arg);
		pthread_t thread;
		arg[i] = i;
		int res = pthread_create(&thread,NULL,threadHandler,&arg[i]);
		if(res != 0)
		{
			printf("thread create error");
			return -1;
		}   

	}
	printf("main thread,pid == [%d],thread_id == [%ld]\n",getpid(),pthread_self());

	sleep(1);//等待子线程结束

	return 0;   
}
