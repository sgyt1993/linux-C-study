#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

//pooltask�̳߳������װ
typedef	struct _PoolTaskP
{
	int tasknum;//ģ��������
	void *arg;//�ص���������
	void (*task_func)(void *arg);//����ص�����
}PoolTask;

typedef struct _ThreadPool
{
	int	max_job_num;//����������
	int job_num;//ʵ���������
	PoolTask *task;//�����������
	int job_push;//���λ��
	int job_pop;//����λ��
	
	int thr_num;//�̳߳����̸߳���
	pthread_t *threads;//�̳߳����߳�����
	int shutdown;//�Ƿ�ر��̳߳�
	pthread_mutex_t pool_lock;//�̳߳ػ�����
	pthread_cond_t empty_task;//�������Ϊ�յ�����
	pthread_cond_t not_empty_task;//������в�Ϊ�յ�����
}ThreadPool;

void create_threadpool(int thrnum,int maxtasknum);//�����̳߳�   thrnum�����̸߳�����maxtasknum ����������

void destory_threadpool(ThreadPool *pool);//�ݻ��̳߳�

void addtask(ThreadPool *pool);//��������̳߳�

void taskRun(void *arg);//����ص�����

#endif