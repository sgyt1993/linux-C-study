//selet 高并发服务器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <poll.h>

int getFree(struct pollfd client[],int size)
{
	int i=0;
	int res = -1;
	for(i=0;i<size;i++)
	{
			if(client[i].fd == -1)
			{
					res = i;
					break;
			}
	}
	return res;
}

int main(int argc,char *argv[])
{
	struct pollfd client[1024];
	
	int ftd = socket(AF_INET,SOCK_STREAM,0);
	if(ftd < 0)
	{
		perror("socket error\n");
		return -1;		
	}
	
	int opt = 1;
	setsockopt(ftd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));
	
	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family =  AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int res = bind(ftd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error \n");
		return -1;	
	}
	
	res = listen(ftd,256);
	if(res < 0)
	{
		perror("listen error\n");
		return -1;
	}
	
	//初始poll数组
	int i = 0;
	for(i=0;i<sizeof(client)/sizeof(struct pollfd);i++)
	{
		client[i].fd = -1;
	}
	
	//返回改变的数量
	int changeNumber;
	
	//返回的client的下标
	int index;
	
	//初始数组
	client[0].fd = ftd;
	client[0].events = POLLIN;
	
	//初始最大描述符
	int maxftd = ftd;
	
	//client的文件描述符
	int cfd;
	
	//client文件地址
	struct sockaddr_in rece;
	socklen_t iplen;
	char ipstr[16];
	
	//client的数据
	char buf[1024];
	int readres;
	
	while(1)
	{
		changeNumber = poll(client,maxftd+1,-1);
		if(changeNumber < 0)
		{
			if(errno == EINTR)//信号中断
			{
				continue;	
			}
			break;
		}
		
		//文件描述符监听到链接的请求
		if(client[0].revents == POLLIN)
		{
			memset(&rece,0x00,sizeof(rece));
			iplen = sizeof(rece);
			cfd = accept(ftd,(struct sockaddr*)&rece,&iplen);
			if(cfd < 0)
			{
				printf("accept error\n");
			}
			
			//打印出详情
			memset(ipstr,0x00,sizeof(ipstr));
			printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&rece.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(rece.sin_port));
			
			//寻找出client的不监控的
			index = getFree(client,sizeof(client)/sizeof(struct pollfd));
			client[index].fd = cfd;
			client[index].events = POLLIN;
			
			//修改maxftd的大小
			if(maxftd < cfd)
			{
				maxftd = cfd;	
			}
			
			if(--changeNumber == 0)
			{
				continue;
			}
		}
		
		//文件描述符中传输描述符
		for(i=1;i<=maxftd;i++)
		{
				if(client[i].fd == -1)
				{
						continue;	
				}
				
				if(client[i].revents == POLLIN)
				{
						memset(buf,0x00,sizeof(buf));
						readres = read(client[i].fd,buf,sizeof(buf));
						if(readres == 0)
						{
							printf("the connect is close\n");
							close(client[i].fd);
							client[i].fd = -1;
						}else
						{
							write(client[i].fd,buf,readres);	
						}
						
						if(--changeNumber == 0)
						{
							break;	
						}
				}
		}
	}
	
	close(ftd);
	return 0;
}