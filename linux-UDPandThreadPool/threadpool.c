#include "threadpool.h"

ThreadPool *thrPool = NULL;

int beginnum = 1000;

//线程中执行函数
void *thrRun(void *arg)
{
		ThreadPool *pool = (ThreadPool *)arg;
		int taskpos = 0;//任务位置
		
		PoolTask *task = (PoolTask *)malloc(sizeof(PoolTask));
		
		while(1)
		{
			//获取任务，先要加锁
			pthread_mutex_lock(&pool -> pool_lock);
		
			while(thrPool -> job_num <= 0 && !thrPool -> shutdown)
			{
				pthread_cond_wait(&thrPool -> empty_task,&thrPool->pool_lock);//没有任务就会阻塞	
			}
			
			if(thrPool -> job_num)
			{
				//有任务需要处理
				taskpos = (thrPool -> job_pop++)%thrPool->max_job_num;//这个是模拟循环队列，也就是相当于job_pop>max_job_num时   job_pop
					
				memcpy(task,&thrPool -> task[taskpos],sizeof(PoolTask));
				
				task -> arg = task;
				thrPool -> job_num--;
				
				pthread_cond_signal(&thrPool->empty_task);//通知生产者
			}
			
			if(thrPool -> shutdown)
			{
				//摧毁线程池，此事线程退出
				pthread_mutex_unlock(&thrPool->pool_lock);
				free(task);
				pthread_exit(NULL);//线程退出
			}
			
			pthread_mutex_unlock(&thrPool->pool_lock); 
			task->task_func(task->arg);//执行回调函数
		}
		
}

//创建线程池
void create_threadpool(int thrnum,int maxtasknum)
{
	printf("begin call %s----fun\n",__FUNCTION__);
	thrPool = (ThreadPool *)malloc(sizeof(ThreadPool));
	
	//初始化线程池参数
	thrPool -> max_job_num  = maxtasknum;
	thrPool -> shutdown = 0;//是否摧毁线程，1代表摧毁
	thrPool -> job_push = 0;//任务队列添加位置
	thrPool -> job_pop = 0;//任务队列出队位置
	thrPool -> job_num = 0;//初始化的任务个数0
	thrPool -> task = (PoolTask *)malloc((sizeof(PoolTask)*maxtasknum));//申请最大的任务队列
	
	thrPool -> thr_num = thrnum;
	pthread_mutex_init(&thrPool -> pool_lock,NULL);//初始化锁和条件变量
	pthread_cond_init(&thrPool -> empty_task,NULL);
	pthread_cond_init(&thrPool -> not_empty_task,NULL);
	
	thrPool -> threads = (pthread_t *)malloc((sizeof(pthread_t)*thrnum));//申请n个线程数组
	
	//设置线程为独立线程
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int i =0;
	
	for(i = 0;i<thrnum;i++)
	{
		pthread_create(&thrPool->threads[i],&attr,thrRun,(void*)thrPool);//创建thrnum个线程
	}
	
};

//添加任务到线程池
void addtask(ThreadPool *pool)
{
	printf("begin call %s-----\n",__FUNCTION__);
	
	//加入同步锁
	pthread_mutex_lock(&pool->pool_lock);
	
	//当实际任务数量大于最大任务数量，就阻塞
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

//摧毁线程池
void destroy_threadpool(ThreadPool *pool)
{
		pool->shutdown = 1;
		
		pthread_cond_broadcast(&pool->not_empty_task);//诱杀
		
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