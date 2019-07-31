//线程生互斥锁的使用
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

//定义一个互斥锁
pthread_mutex_t mutex;

//使用一个
int i = 0;
int size = 2000;

void *threadHandler(void *arg)
{



	 int n ;
	 int k = 0;
	 for(k = 0;k<size;k++)
	 {
	 	//加入lock锁住
	 	pthread_mutex_lock(&mutex);
	 	n = i;
		n++;
		i = n;
	 	//解锁
	 	pthread_mutex_unlock(&mutex);
	 
	 }



}   


int main(int argc,char* argv[])
{
	//创建子线程
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	//                          void *(*start_routine) (void *), void *arg);
	pthread_t thread1;
	pthread_t thread2;
	
	//初始化
	pthread_mutex_init(&mutex,NULL);
	

	int res = pthread_create(&thread1,NULL,threadHandler,NULL);
	if(res != 0)
	{
		printf("thread create error");
		return -1;
	}

	res = pthread_create(&thread2,NULL,threadHandler,NULL);
	if(res != 0)
	{
		printf("thread create error");
		return -1;
	}
	
	printf("main thread,pid == [%d],thread_id == [%ld]\n",getpid(),pthread_self());

	//等待线程服务
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	//销毁互斥锁
	pthread_mutex_destroy(&mutex);
	
	//线程完成看i的数量
	printf("i == [%d]\n",i);

	return 0;   
}
