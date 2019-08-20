//epoll �߲�������
//ET��Եģʽ��whileѭ����д
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
#include <fcntl.h>

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
		
		//����epoll����,eplΪ����������
		int epl = epoll_create(1024);
		if(epl < 0)
		{
			perror("epoll error\n");
			return -1;	
		}
		
		//����ftd���ļ�����
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
		
		//���ܵ�event
		struct epoll_event rece[1024];
		
		//�����ı��ֵ
		int changeNum;
		
		//ѭ������
		int i=0;
		
		//client������
		int cfd;
		struct sockaddr_in client;
		socklen_t recelen;
		char ipstr[16];
		
		//����Ĳ���
		char buf[2];
		int readres;
		
		//�޸��ļ�������״̬
		int flag;
		
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
					//client���ܵĸı���ļ�����
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
							
						//����cfd�ļ�������������
						flag = fcntl(cfd,F_GETFL,0);
						flag = flag |  O_NONBLOCK;
						fcntl(cfd,F_SETFL,flag);
						
						memset(ipstr,0x00,sizeof(ipstr));
						printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(client.sin_port));
						
						//��cfd����epoll��
						memset(&ev,0x00,sizeof(ev));
						ev.data.fd = cfd;
						ev.events = EPOLLIN | EPOLLET;
						epoll_ctl(epl,EPOLL_CTL_ADD,cfd,&ev);
						
						continue;//��������	
					}		
					while(1)
					{
							//client����ĸı���ļ�����
						memset(buf,0x00,sizeof(buf));
						readres = read(rece[i].data.fd,buf,sizeof(buf));
						
						//û�����ݴ������
						if(readres == -1)
						{
							break;	
						}
						
						//client�Ͽ����ӻ��߶��쳣
						if(readres == 0 || (readres<0 && readres != -1))
						{
							printf("connect is close\n");
							//�������ļ�����epoll����ɾ��
							epoll_ctl(epl,EPOLL_CTL_DEL,rece[i].data.fd,NULL);
						}else
						{
							printf("i == [%d],clinet str = [%s]\n",i,buf);
							write(rece[i].data.fd,buf,sizeof(buf));		
						}					
					}
					
				}
				
				
		}	
		
		close(epl);
		close(ftd);
		return 0;	
}
