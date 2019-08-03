//线程读写锁测试成功
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

int number = 0;

//定义一个读写锁
pthread_rwlock_t rwlock;

void *thread_write(void *arg)
{
	int a = *(int *)arg;
	int i;
	while(1)
	{
		//加入写锁
		pthread_rwlock_wrlock(&rwlock);

		i = number;
		i++;
		number = i;
		printf("[%d]-w-[%d]\n",a,i);
		sleep(1);

		//释放写锁
		pthread_rwlock_unlock(&rwlock);
	
	}
}

void *thread_read(void *arg)
{
	int c = *(int *)arg;
	int i;
	while(1)
	{
		//加入读锁
		pthread_rwlock_rdlock(&rwlock);

		i = number;
		printf("[%d]-r-[%d]\n",c,i);
	
		//释放读锁
		pthread_rwlock_unlock(&rwlock);
		sleep(2);
	}
}



int main(int argc,char* argv[])
{
	//创建4个读线程，4个写线程
	int arr[8];
	int i = 0;
	int k = 0;
	int res;
	
	//初始化读写锁
	pthread_rwlock_init(&rwlock,NULL);

	pthread_t thread[8];
	for(i=0;i<4;i++)
	{
		arr[i] = i;
		res = pthread_create(&thread[i],NULL,thread_write,&arr[i]);
		if(res != 0)
		{
			printf("create thread is fail\n");	
		}
	}

	for(k=4;k<8;k++)
	{
		arr[k] = k;
		res = pthread_create(&thread[k],NULL,thread_read,&arr[k]);
		if(res != 0)
		{
			printf("create thread is fail\n");	
		}
	}
	
	int j=0;
	for(j=0;j<8;j++)
	{
		//主线程等待子线程结束
		pthread_join(thread[j],NULL);
	}

	//销毁读写锁
	pthread_rwlock_destroy(&rwlock);
	return 0;   
}
