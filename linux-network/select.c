//selet �߲���������
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

//��ʼ��client�������
void ifnoInit(struct IFNO ifno[],int size)
{
	int i = 0;
	for(i=0;i<size;i++)
	{
		ifno[i].flag = -1;	
	}
}

//��client��������ȡ��δ�õ�һ��
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

//����fd�ҵ�index
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
	//��ʼ������
	ifnoInit(ifno,sizeof(ifno)/sizeof(struct IFNO));
	
	//��ʼ��һ��socket���ļ�������
	int ftd = socket(AF_INET,SOCK_STREAM,0);
	if(ftd<0)		
	{
		perror("socket error\n");
		return -1;	
	}
	
	//���ö˿�
	int optval = 1;
	int res = setsockopt(ftd,SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(int));
	
	//ip�Ͷ˿ڵİ�
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
	
	//�˿ڼ���
	res = listen(ftd, 128);
	if(res < 0)
	{
		perror("listen error\n");
		return -1;	
	}
	
	//��ʼ���������ļ�������
	fd_set set;
	fd_set tmpfds;//�������
	FD_ZERO(&set);
	FD_ZERO(&tmpfds);
	FD_SET(ftd,&set);
	
	//��ʼ�������ļ�������
	int maxftd = ftd;
	//�仯���ļ��������ĸ���
	int changeftd;
	
	//client��sock��ַ
	struct sockaddr_in client;
	socklen_t iplen;
	char ipaddr[16];
	
	//���ܵ��ļ�������
	int cfd;
	
	//������±�
	int index;
	int i = 0;
	int k = 0;
	
	//��ȡ��������buf
	char buf[1024];
	int readres;
	
	while(1)
	{
			//int select(int nfds, fd_set * readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);	
			//��Ϊreadfds�����һ�������������������Ҫ��һ��ר�ŵ�����¼
			tmpfds = set;
			
			//�󶨼������ļ�
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
			//�ж��Ƿ���clinet���������
			if(FD_ISSET(ftd,&tmpfds))
			{
				index = getifno(ifno,sizeof(ifno)/sizeof(struct IFNO));
				if(index >= 0)
				{
					//��������
					memset(&client,0x00,sizeof(client));
					cfd = accept(ftd,(struct sockaddr*)&client,&iplen);
					memset(ipaddr,0x00,sizeof(ipaddr));
					printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,ipaddr,sizeof(ipaddr)),ntohs(client.sin_port));
					ifno[index].fd = cfd;
					ifno[index].client = client;
					ifno[index].flag = 1;

					//�����ӵ��ļ����������������
					FD_SET(cfd,&set);

					//�޸��ں˼�ص��ļ��������ķ�Χ
		  		if(maxftd<cfd)
		  		{
		  			maxftd = cfd;
		  		}

					//��������ļ��ĸı�����Ϊ0�����¿�ʼ
					if(--changeftd == 0)
					{
						continue;
					}
				}
				else
				{
					printf("�������Ѿ����ˣ���ȴ�\n");
				}

			}
						
			//clinet��������ݣ���ftd����Ķ��� ����ļ���������
			for(i=ftd+1;i<=maxftd;i++)
			{
				if(FD_ISSET(i,&tmpfds))
				{
					memset(buf,0x00,sizeof(buf));
					readres = read(i,buf,sizeof(buf));
					index = getIfnobyFd(ifno,sizeof(ifno)/sizeof(struct IFNO),i);
					//������ӹر�
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
					//��������ļ��ĸı�����Ϊ0�����¿�ʼ
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
