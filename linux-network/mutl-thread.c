#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

struct IFNO
{
	int cfd;
	pthread_t threadId;
	struct sockaddr_in client;
	int flag;
};

struct IFNO ifno[100];

int getFreeIfno(struct IFNO ifno[],int length)
{
	int i = 0;
	int res;
	for(i=0;i<length;i++)
	{
		if(ifno[i].flag == -1)
		{
			res = i;
			printf("free info index = [%d]\n",i);
			break;
		}	
	}
	return res;
}

int getIfnoByCfd(struct IFNO ifno[],int length,int cfd)
{
		int i = 0;
		int res;
		for(i=0;i<length;i++)
		{
			if(ifno[i].cfd == cfd)
			{
				res = i;
				break;
			}	
		}
		return res;
}

//线程处理函数
void *sockHander(void *arg)
{
		struct IFNO ifns = *(struct IFNO*)arg;
		
		int i = 0;
		char buf[1024];
		int readres;
		int resInfo;
		
		char iplen[16];
		memset(iplen,0x00,sizeof(iplen));
		inet_ntop(AF_INET,&ifns.client.sin_addr.s_addr,iplen,sizeof(iplen));
		
		while(1)
		{
			memset(buf,0x00,sizeof(buf));
			readres = read(ifns.cfd,buf,sizeof(buf));
			if(readres == 0)
			{
				printf("ip = [%s],connect is close\n",iplen);
				break;					
			}
			
			printf("ip = [%s],send message = [%s]\n",iplen,buf);
			
			for(i = 0;i<readres;i++)
			{
				buf[i] = toupper(buf[i]);
			}
			write(ifns.cfd,buf,sizeof(buf));
		}
		
		resInfo = getIfnoByCfd(ifno,100,ifns.cfd);
		close(ifns.cfd);
		ifno[resInfo].flag = -1;
}

int main(int argc,char *argv[])
{
		//初始化数据
		int i = 0;
		for(i=0;i<100;i++)
		{
				ifno[i].flag = -1; 
		}
		
		int ftd = socket(AF_INET,SOCK_STREAM,0);
		if(ftd < 0)
		{
			perror("socket error\n");		
			return -1;
		}
		
		struct sockaddr_in addr;
		memset(&addr,0x00,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8888);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		int res = bind(ftd,(struct sockaddr*)&addr,sizeof(addr));
		if(res < 0 )
		{
			perror("bind error\n");
			return -1;
		}
		
		res = listen(ftd,256);
		if(res<0)
		{
			perror("listen error\n");
			return -1;	
		}
		
		int cfd;
		struct sockaddr_in client;
		socklen_t len;
		int freeInfo;
		char iplen[16];
		while(1)
		{
			memset(&client,0x00,sizeof(client));
			memset(iplen,0x00,sizeof(iplen));
			len = sizeof(client);
			cfd = accept(ftd,(struct sockaddr*)&client,&len);
			
			//打印出链接的ip和端口
			printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,iplen,sizeof(iplen)),ntohs(client.sin_port));
			
			//获取数组中空闲
			freeInfo = getFreeIfno(ifno,100);
			
			//初始化数据
			ifno[freeInfo].cfd = cfd;
			ifno[freeInfo].client = client;
			ifno[freeInfo].flag = 1;
			
			//创建线程
			int threadId = pthread_create(&ifno[freeInfo].threadId,NULL,sockHander,&ifno[freeInfo]);
			if(threadId != 0)
			{
				printf("thread create error\n");
				return -1;
			}
			
			
		}
		
		return 0;
		
}