#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

struct Test{
	int data;
	char name[64];
	
};

void *threadHandle(void *arg)
{
	struct Test *ks = (struct Test *)arg;
	printf("this is child thread,pid = [%d],thread = [%ld]\n",getpid(),pthread_self());
	printf("data:name == [%d]:[%s]\n",ks->data,ks->name);
}

int main(int argc,char* argv[])
{
	struct Test test ;
	memset(&test,0x00,sizeof(struct Test));


	test.data = 90;
	strcpy(test.name,"sgyt");
	
	//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	//                          void *(*start_routine) (void *), void *arg);
	pthread_t thread;
	pthread_create(&thread,NULL,threadHandle,&test);
	
	sleep(1);

	printf("this is father thread,pid = [%d],thread = [%ld]\n",getpid(),pthread_self());
	return 0;	
}
