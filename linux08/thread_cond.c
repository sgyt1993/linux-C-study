//线程生成的demo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

//定义一把互斥锁
pthread_mutex_t mutex;

//定义条件变量
pthread_cond_t cond;

typedef struct node
{
	int data;
	struct node *next;
}NODE;
NODE *head = NULL;

void *threadProduce(void *arg)
{
	 NODE *pNode = NULL;
	 while(1)
	 {
		//生产一个节点
		pNode =(NODE *)malloc(sizeof(NODE));
		if(pNode == NULL)
		{
			perror("malloc fail");
			exit(-1);//连同父线程一起推出
		}
		pNode->data = rand()%3;
		printf("P:[%d]\n",pNode->data);
		
		//枷锁
		pthread_mutex_lock(&mutex);
		
		pNode->next = head;
		head = pNode;

		//解锁
		pthread_mutex_unlock(&mutex);

		//并通知消费者线程解除阻塞
		pthread_cond_signal(&cond);
		
		sleep(1);
	 } 
}   

void *threadCustomer(void *arg)
{
	NODE *pNode = NULL;
	while(1)
	{
		//枷锁
		pthread_mutex_lock(&mutex);

		if(head == NULL)
		{
			//没有货物消费，让其等待
			//若条件不满足，则阻塞等待，并解锁
			pthread_cond_wait(&cond,&mutex);
		}

		printf("C:[%d]\n",head->data);
		pNode = head;
		head = head ->next;

		//解锁
		pthread_mutex_unlock(&mutex);

		free(pNode);
		pNode = NULL;

		sleep(3);
	}
}



int main(int argc,char* argv[])
{
	//初始化锁
	pthread_mutex_init(&mutex,NULL);
	//初始化条件
	pthread_cond_init(&cond,NULL);
	
	//创建子线程
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	//                          void *(*start_routine) (void *), void *arg);
	pthread_t thread1;
	int res = pthread_create(&thread1,NULL,threadProduce,NULL);
	if(res != 0)
	{
		printf("thread create error");
		return -1;
	}   
	
	//创建消费者
	pthread_t thread2;
	res = pthread_create(&thread2,NULL,threadCustomer,NULL);
	if(res != 0)
	{
		printf("thread is fail\n");	
	}

	printf("main thread,pid == [%d],thread_id == [%ld]\n",getpid(),pthread_self());

 	//等待子线程结束
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	//销毁互斥锁
	pthread_mutex_destroy(&mutex);
	//销毁条件
	pthread_cond_destroy(&cond);
	return 0;   
}
