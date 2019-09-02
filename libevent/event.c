//libevent的基础使用
#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <string.h>

int main(int argc,char* argv[])
{
	
	//查看系统支持的方式
	const char **p = event_get_supported_methods();
	int i = 0;
	while(p[i] != NULL)
	{
		printf("%s  ",p[i]);
		i++;
	}
	printf("\n");
	
	//创建树根
	struct event_base *base = event_base_new();
	if(base == NULL)
	{
		printf("create base error\n");
		return -1;
	}
	
	//查看base创建的方式
	printf("%s\n",event_base_get_method(base));

	//释放base的创建的空间
	event_base_free(base);
	return 0;
}
