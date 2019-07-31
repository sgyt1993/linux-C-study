//线程分离
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
	
	//设置线程分离
	pthread_detach(thread);

	
	int joinId = pthread_join(thread,&resval);//当前的线程已经分离，不能再用主线程去等待，没有等到的线程，会失败
	
	if(joinId != 0)
	{
		printf("pthread_join fail mes = [%s]",strerror(joinId));		
	}

	struct Test *p = (struct Test *)resval;
	printf("[%d],[%s]\n",p->data,p->name);
	return 0;   
}
