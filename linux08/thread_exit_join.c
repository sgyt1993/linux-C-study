//线程生成的demo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

struct Test
{
	int data;
	char name[64];
};


struct Test test;


void *threadHandler(void *arg)
{
	 printf("child thread,pid==[%d],thread_id == [%ld]\n",getpid(),pthread_self());
	 
	 //只能让堆数据，放栈数据是共享不了的
	 memset(&test,0x00,sizeof(test));
	 test.data = 1;
	 strcpy(test.name,"sgyt");


	 pthread_exit(&test);
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
	
	void *resval;
	pthread_join(thread,&resval);
	
	struct Test *p = (struct Test *)resval;
	printf("[%d],[%s]\n",p->data,p->name);
	return 0;   
}
