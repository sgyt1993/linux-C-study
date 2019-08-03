//线程信号锁
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct node
{
	int data;
	struct node *next;
}NODE;
NODE *head = NULL;

sem_t sem_produce;
sem_t sem_consumer;

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
		sem_wait(&sem_produce);//--

		pNode->next = head;
		head = pNode;

		//解锁
		sem_post(&sem_consumer);//++
		
		sleep(1);
	 } 
}   

void *threadCustomer(void *arg)
{
	NODE *pNode = NULL;
	while(1)
	{
		//枷锁
		sem_wait(&sem_consumer);//--

		printf("C:[%d]\n",head->data);
		pNode = head;
		head = head ->next;

		//解锁
		sem_post(&sem_produce);

		free(pNode);
		pNode = NULL;

		sleep(3);
	}
}



int main(int argc,char* argv[])
{
	//初始化锁
	sem_init(&sem_produce,0,5);
	sem_init(&sem_consumer,0,0);


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

	//销毁信号量
	sem_destroy(&sem_produce);
	sem_destroy(&sem_consumer);
	return 0;   
}
