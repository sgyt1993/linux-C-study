#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc,char* argv[])
{
	int ftd = socket(AF_INET,SOCK_STREAM,0);
	if(ftd < 0 )
	{
		perror("socket errror\n");
		return -1;
	}

	
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
	printf("listen......\n");
	
	int afd;
	struct sockaddr_in rece;
	socklen_t len;
	char iplen[16];
	while(1)
	{
		memset(&rece,0x00,sizeof(rece));
		memset(iplen,0x00,sizeof(iplen));
		len = sizeof(rece);
		afd = accept(ftd,(struct sockaddr*)&rece,&len);
		if(afd<0)
		{
			perror("accept error\n");
			return -1;
		}
	
		//打印出链接的对象
		inet_ntop(AF_INET,&rece.sin_addr.s_addr,iplen,sizeof(iplen));
		printf("clent ip = [%s],port = [%d] is connet\n",iplen,ntohs(rece.sin_port));
		
		//开启线程处理
		pid_t pid = fork();
		if(pid<0)
		{
			perror("fork error\n");
			return -1;
		}
		else if(pid > 0 )
		{
			close(afd);	
			
		}
		else if(pid == 0)
		{
			char buf[1024];
			memset(buf,0x00,sizeof(buf));
			int readres;
			int i = 0;
			while(1)
			{
				readres = read(afd,buf,sizeof(buf));
				if(readres == 0)
				{
					//结束对话
					break;
				}

				printf("client ip = [%s],readres = [%d], say = [%s]\n",iplen,readres,buf);
				
				for(i = 0;i<readres;i++)
				{
					buf[i] = toupper(buf[i]);
				}
				write(afd,buf,readres);
			}

			close(afd);
			exit(0);//结束这个进程		
		}

	
	}
	return 0;
}
