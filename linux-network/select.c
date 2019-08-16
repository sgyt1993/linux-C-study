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

struct IFNO
{
	int fd;
	struct sockaddr_in client;
	int flag;	
};

struct IFNO ifno[100];

//初始化client存放数组
void ifnoInit(struct IFNO ifno[],int size)
{
	int i = 0;
	for(i=0;i<size;i++)
	{
		ifno[i].flag = -1;	
	}
}

//从client的数组中取出未用的一个
int getifno(struct IFNO ifno[],int size)
{
		int i = 0;
		int res = -1;
		for(i=0;i<size;i++)
		{
			if(ifno[i].flag == -1)
			{
				res = i;	
				break;
			}	
		}
		printf("ifno i == [%d]\n",res);
		return res;
}

//根据fd找到index
int getIfnobyFd(struct IFNO ifno[],int size,int fd)
{
	int i = 0;
		int res = -1;
		for(i=0;i<size;i++)
		{
			if(ifno[i].fd == fd)
			{
				res = i;	
			}	
		}
		return res;
}

int main(int argc,char* argv[])
{
	//初始化数组
	ifnoInit(ifno,sizeof(ifno)/sizeof(struct IFNO));
	
	//初始化一个socket的文件描述符
	int ftd = socket(AF_INET,SOCK_STREAM,0);
	if(ftd<0)		
	{
		perror("socket error\n");
		return -1;	
	}
	
	//复用端口
	int optval = 1;
	int res = setsockopt(ftd,SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(int));
	
	//ip和端口的绑定
	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res = bind(ftd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error\n");
		return -1;	
	}
	
	//端口监听
	res = listen(ftd, 128);
	if(res < 0)
	{
		perror("listen error\n");
		return -1;	
	}
	
	//初始化监听的文件描述符
	fd_set set;
	fd_set tmpfds;//传入参数
	FD_ZERO(&set);
	FD_ZERO(&tmpfds);
	FD_SET(ftd,&set);
	
	//初始化做大文件描述符
	int maxftd = ftd;
	//变化的文件描述符的个数
	int changeftd;
	
	//client的sock地址
	struct sockaddr_in client;
	socklen_t iplen;
	char ipaddr[16];
	
	//接受的文件描述符
	int cfd;
	
	//数组的下标
	int index;
	int i = 0;
	int k = 0;
	
	//读取传输数据buf
	char buf[1024];
	int readres;
	
	while(1)
	{
			//int select(int nfds, fd_set * readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);	
			//因为readfds这个是一个输入输出参数，所有要有一个专门的量记录
			tmpfds = set;
			
			//绑定监听的文件
			changeftd = select(maxftd+1,&tmpfds,NULL,NULL,NULL);
			if(changeftd < 0)
			{
				if(errno == EINTR)
				{
					continue;
				}
				break;
			}
			
			
			/*printf("isset == [%d]\n",FD_ISSET(ftd,&tmpfds));*/
			//判断是否有clinet请求的链接
			if(FD_ISSET(ftd,&tmpfds))
			{
				index = getifno(ifno,sizeof(ifno)/sizeof(struct IFNO));
				if(index >= 0)
				{
					//创建链接
					memset(&client,0x00,sizeof(client));
					cfd = accept(ftd,(struct sockaddr*)&client,&iplen);
					memset(ipaddr,0x00,sizeof(ipaddr));
					printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,ipaddr,sizeof(ipaddr)),ntohs(client.sin_port));
					ifno[index].fd = cfd;
					ifno[index].client = client;
					ifno[index].flag = 1;

					//把链接的文件描述符放入监听中
					FD_SET(cfd,&set);

					//修改内核监控的文件描述符的范围
		  		if(maxftd<cfd)
		  		{
		  			maxftd = cfd;
		  		}

					//如果监听文件的改变数量为0，从新开始
					if(--changeftd == 0)
					{
						continue;
					}
				}
				else
				{
					printf("链接数已经满了，请等待\n");
				}

			}
						
			//clinet请求的数据，（ftd后面的都是 输出文件描述符）
			for(i=ftd+1;i<=maxftd;i++)
			{
				if(FD_ISSET(i,&tmpfds))
				{
					memset(buf,0x00,sizeof(buf));
					readres = read(i,buf,sizeof(buf));
					index = getIfnobyFd(ifno,sizeof(ifno)/sizeof(struct IFNO),i);
					//如果连接关闭
					if(readres == 0)
					{
						printf("client ip = [%s],port = [%d], close connect\n",inet_ntop(AF_INET,&ifno[index].client.sin_addr.s_addr,ipaddr,sizeof(ipaddr)),ntohs(ifno[index].client.sin_port));
						FD_CLR(i,&set);
						ifno[index].flag = -1;
						close(i);
					}
					else
					{
						memset(ipaddr,0x00,sizeof(ipaddr));
						printf("client ip = [%s],port = [%d],str = [%s]\n",inet_ntop(AF_INET,&ifno[index].client.sin_addr.s_addr,ipaddr,sizeof(ipaddr)),ntohs(ifno[index].client.sin_port),buf);
						for(k=0;k<readres;k++)
						{
							buf[k] = toupper(buf[k]);
						}
						write(i,buf,sizeof(buf));
					}												
					//如果监听文件的改变数量为0，从新开始
					if(--changeftd == 0)
					{
						break;
					}				
				}	
			}
	}
	
	close(ftd);
	return 0;
	
}
