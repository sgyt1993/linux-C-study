//epoll -et模式，
//缓冲区中有数据只会通知一次, 
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
#include <sys/epoll.h>

int main(int argc,char *argv[])
{
		int ftd = socket(AF_INET,SOCK_STREAM,0);
		if(ftd < 0)
		{
			perror("socket error\n");
			return -1;	
		}
		
		int opt = 1;
		setsockopt(ftd,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(int));
		
		struct sockaddr_in addr;
		memset(&addr,0x00,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8888);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		int res = bind(ftd,(struct sockaddr*)&addr,sizeof(addr));
		if(res < 0)
		{
			perror("bind error\n");
			return -1;	
		}
		
		res = listen(ftd,256);
		if(res < 0)
		{
			perror("listen error\n");
			return -1;	
		}
		
		//创建epoll树根,epl为创建的树根
		int epl = epoll_create(1024);
		if(epl < 0)
		{
			perror("epoll error\n");
			return -1;	
		}
		
		//加入ftd的文件监听
		struct epoll_event ev;
		memset(&ev,0x00,sizeof(ev));
		ev.events = EPOLLIN;
		ev.data.fd = ftd;
		res = epoll_ctl(epl,EPOLL_CTL_ADD,ftd,&ev);
		if(res < 0)
		{
			perror("epoll_ctl error\n");
			return -1;	
		}
		
		//接受的event
		struct epoll_event rece[1024];
		
		//监听改变的值
		int changeNum;
		
		//循环变量
		int i=0;
		
		//client的链接
		int cfd;
		struct sockaddr_in client;
		socklen_t recelen;
		char ipstr[16];
		
		//传输的参数
		char buf[2];
		int readres;
		
		while(1)
		{
				changeNum = epoll_wait(epl,rece,1024,-1);
				if(changeNum < 0)
				{
					if(errno==EINTR)
					{
						continue;	
					}	
					break;
				}
				
				
				for(i=0;i<changeNum;i++)
				{
					//client接受的改变的文件描述
					if(rece[i].data.fd == ftd)
					{
						memset(&client,0x00,sizeof(client));
						recelen = sizeof(client);
						cfd = accept(ftd,(struct sockaddr*)&client,&recelen);
						if(cfd < 0)
						{
							perror("accept error\n");
							return -1;	
						}
						
						memset(ipstr,0x00,sizeof(ipstr));
						printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(client.sin_port));
						
						//将cfd加入epoll树
						memset(&ev,0x00,sizeof(ev));
						ev.data.fd = cfd;
						//将监控模式改为ET模式
						ev.events = EPOLLET | EPOLLIN;
						epoll_ctl(epl,EPOLL_CTL_ADD,cfd,&ev);
						
						continue;//跳过传输	
					}		
					
					//client传输的改变的文件描述
					memset(buf,0x00,sizeof(buf));
					readres = read(rece[i].data.fd,buf,sizeof(buf));
					
					//client断开链接
					if(readres == 0)
					{
						printf("connect is close\n");
						//将传输文件符从epoll树上删除
						epoll_ctl(epl,EPOLL_CTL_DEL,rece[i].data.fd,NULL);
					}else
					{
						printf("i == [%d],clinet str = [%s]\n",i,buf);
						write(rece[i].data.fd,buf,sizeof(buf));		
					}					
				}
				
				
		}	
		
		close(epl);
		close(ftd);
		return 0;	
}
