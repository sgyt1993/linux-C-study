#include "threadpool.h"

ThreadPool *thrPool = NULL;

int beginnum = 1000;

//�߳���ִ�к���
void *thrRun(void *arg)
{
		ThreadPool *pool = (ThreadPool *)arg;
		int taskpos = 0;//����λ��
		
		PoolTask *task = (PoolTask *)malloc(sizeof(PoolTask));
		
		while(1)
		{
			//��ȡ������Ҫ����
			pthread_mutex_lock(&pool -> pool_lock);
		
			while(thrPool -> job_num <= 0 && !thrPool -> shutdown)
			{
				pthread_cond_wait(&thrPool -> empty_task,&thrPool->pool_lock);//û������ͻ�����	
			}
			
			if(thrPool -> job_num)
			{
				//��������Ҫ����
				taskpos = (thrPool -> job_pop++)%thrPool->max_job_num;//�����ģ��ѭ�����У�Ҳ�����൱��job_pop>max_job_numʱ   job_pop
					
				memcpy(task,&thrPool -> task[taskpos],sizeof(PoolTask));
				
				task -> arg = task;
				thrPool -> job_num--;
				
				pthread_cond_signal(&thrPool->empty_task);//֪ͨ������
			}
			
			if(thrPool -> shutdown)
			{
				//�ݻ��̳߳أ������߳��˳�
				pthread_mutex_unlock(&thrPool->pool_lock);
				free(task);
				pthread_exit(NULL);//�߳��˳�
			}
			
			pthread_mutex_unlock(&thrPool->pool_lock); 
			task->task_func(task->arg);//ִ�лص�����
		}
		
}

//�����̳߳�
void create_threadpool(int thrnum,int maxtasknum)
{
	printf("begin call %s----fun\n",__FUNCTION__);
	thrPool = (ThreadPool *)malloc(sizeof(ThreadPool));
	
	//��ʼ���̳߳ز���
	thrPool -> max_job_num  = maxtasknum;
	thrPool -> shutdown = 0;//�Ƿ�ݻ��̣߳�1����ݻ�
	thrPool -> job_push = 0;//����������λ��
	thrPool -> job_pop = 0;//������г���λ��
	thrPool -> job_num = 0;//��ʼ�����������0
	thrPool -> task = (PoolTask *)malloc((sizeof(PoolTask)*maxtasknum));//���������������
	
	thrPool -> thr_num = thrnum;
	pthread_mutex_init(&thrPool -> pool_lock,NULL);//��ʼ��������������
	pthread_cond_init(&thrPool -> empty_task,NULL);
	pthread_cond_init(&thrPool -> not_empty_task,NULL);
	
	thrPool -> threads = (pthread_t *)malloc((sizeof(pthread_t)*thrnum));//����n���߳�����
	
	//�����߳�Ϊ�����߳�
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int i =0;
	
	for(i = 0;i<thrnum;i++)
	{
		pthread_create(&thrPool->threads[i],&attr,thrRun,(void*)thrPool);//����thrnum���߳�
	}
	
};

//��������̳߳�
void addtask(ThreadPool *pool)
{
	printf("begin call %s-----\n",__FUNCTION__);
	
	//����ͬ����
	pthread_mutex_lock(&pool->pool_lock);
	
	//��ʵ���������������������������������
	while(pool -> max_job_num <= pool -> job_num)
	{
		pthread_cond_wait(&pool->empty_task,&pool->pool_lock);
	}
	
	int taskpos = (pool -> job_push++)%pool->max_job_num;
	
	pool -> task[taskpos].tasknum = beginnum++;
	pool -> task[taskpos].arg = (void *)&pool-> task[taskpos];
	pool -> task[taskpos].task_func = taskRun;
	pool -> job_num++;
	
	pthread_mutex_unlock(&pool->pool_lock);
	pthread_cond_signal(&pool->not_empty_task);
};

void taskRun(void *arg)
{
	PoolTask *task = (PoolTask *)arg;
	int num = task -> tasknum;
	printf("task %d is running %lu\n",num,pthread_self());
	
	sleep(1);
	printf("task %d is running %lu\n",num,pthread_self());
};

//�ݻ��̳߳�
void destroy_threadpool(ThreadPool *pool)
{
		pool->shutdown = 1;
		
		pthread_cond_broadcast(&pool->not_empty_task);//��ɱ
		
		int i = 0;
		for(i = 0;i<pool->thr_num;i++)
		{
			pthread_join(pool->threads[i],NULL);	
		}
		
		pthread_cond_destroy(&pool->not_empty_task);
		pthread_cond_destroy(&pool->empty_task);
		pthread_mutex_destroy(&pool->pool_lock);
		
		free(pool->task);
    free(pool->threads);
    free(pool);
		
};

int main()
{
	create_threadpool(3,20);
	int i = 0;
	for(i=0;i<50;i++)
	{
		addtask(thrPool);	
	}
	
	sleep(20);
  destroy_threadpool(thrPool);
  
  return 0;
}