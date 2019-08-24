//UDPDe 服务区端
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc,char* argv[])
{
		int ftd = socket(AF_INET,SOCK_DGRAM,0);
		if(ftd < 0)
		{
			perror("socket error\n");
			return -1;	
		}	
		
		//端口复用
		int opt = 0;
		int res = setsockopt(ftd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));
		if(res<0)
		{
			perror("sockopt error\n");
			return -1;	
		}
		
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8888);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		res = bind(ftd,(struct sockaddr*)&addr,sizeof(addr));
		if(res < 0)
		{
			perror("bind error\n");
			return -1;	
		}
		
		struct sockaddr_in client;
		socklen_t addrlen;
		char iplen[16];
		char buf[1024];
		int readres;
		
		int i;
		
		while(1)
		{
				
				memset(&client,0x00,sizeof(client));
				memset(buf,0x00,sizeof(buf));
				memset(iplen,0x00,sizeof(iplen));
				addrlen = sizeof(client);
				//接受信息
				readres = recvfrom(ftd,buf,sizeof(buf),0,(struct sockaddr*)&client,&addrlen);
				
				if(readres == 0)
				{
					printf("client ip = [%s],port = [%d],readres = [%d],str = [%s]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,iplen,sizeof(iplen)),ntohs(client.sin_port),readres,buf);
					continue;
				}
				
				
				printf("client ip = [%s],port = [%d],readres = [%d],str = [%s]\n",inet_ntop(AF_INET,&client.sin_addr.s_addr,iplen,sizeof(iplen)),ntohs(client.sin_port),readres,buf);
				
			
				
				//发送数据
				for(i=0;i<readres;i++)
				{
					buf[i] = toupper(buf[i]);	
				}
				sendto(ftd,buf,readres,0,(struct sockaddr*)&client,sizeof(client));
				
		}
			
		close(ftd);
}