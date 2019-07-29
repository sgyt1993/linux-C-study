#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>


void *threadHandle(void *arg)
{
	int *ks = (int *)arg;
	printf("this is child thread,pid = [%d],thread = [%ld]\n",getpid(),pthread_self());
	printf("[%d]",*ks);
}

int main(int argc,char* argv[])
{
	
	int k = 60;
	
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	//                          void *(*start_routine) (void *), void *arg);
	pthread_t thread;
	pthread_create(&thread,NULL,threadHandle,&k);
	
	sleep(1);

	printf("this is father thread,pid = [%d],thread = [%ld]\n",getpid(),pthread_self());
	return 0;	
}
