//线程设置同步，创建前
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void *threadHandler(void *arg)
{
	 printf("child thread,pid==[%d],thread_id == [%ld]\n",getpid(),pthread_self());
	 sleep(3);
}   


int main(int argc,char* argv[])
{
	
	//定义一个线程属性类型
	pthread_attr_t attr;
	//对attr进行初始化
	int ret = pthread_attr_init(&attr);
	//设置线程分离属性
	ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);


	//创建子线程
	pthread_t thread;
	int res = pthread_create(&thread,&attr,threadHandler,NULL);
	if(res != 0)
	{
		printf("thread create error");
		return -1;
	}   
	
	//释放线程资源属性
	pthread_attr_destroy(&attr);

	printf("main thread,pid == [%d],thread_id == [%ld]\n",getpid(),pthread_self());
	
	//用线程等待函数
	ret = pthread_join(thread,NULL);
	if(ret != 0)
	{
		printf("pthread_join msg == [%s]\n",strerror(ret));
	}

 	sleep(1);//等待子线程结束

	return 0;   
}
