//本地socket通信
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
		int cfd = socket(AF_UNIX,SOCK_STREAM,0);
		
		int res;
		int check = access("./client.sock",F_OK);
		if(check != -1)//表示文件存在
		{
			res = unlink("./client.sock");//删除文件，方便下次启动
			if(res < 0)
			{
				perror("unlink is error\n");
				return -1;	
			}
		}
		
		
		struct sockaddr_un client;
		memset(&client,0x00,sizeof(client));
		client.sun_family = AF_UNIX;
		memcpy(client.sun_path,"./client.sock",sizeof("./client.sock"));
		res = bind(cfd,(struct sockaddr*)&client,sizeof(client));
		if(res < 0 )
		{
			perror("bind error\n");
			return -1;	
		}
		
		
		//写最简单的写法了
		struct sockaddr_un service;
		memset(&service,0x00,sizeof(service));
		service.sun_family = AF_UNIX;
		strcpy(service.sun_path,"./service.sock");
		int afd = connect(cfd,(struct sockaddr*)&service,sizeof(service));
		if(afd < 0)
		{
			perror("connect error\n");
			return -1;	
		}
		
		
		char buf[1024];
		int readres;
		int i = 0;
		while(1)
		{
			memset(buf,0x00,sizeof(buf));
			readres = read(STDIN_FILENO,buf,sizeof(buf));
			write(cfd,buf,readres);
			
			
			memset(buf,0x00,sizeof(buf));
			readres = read(cfd,buf,sizeof(buf));
			if(readres < 0)
			{
				printf("this connect is close");	
				break;
			}
			printf("readres = [%d],buf = [%s]\n",readres,buf);
			
		}
		
		close(afd);
		return 0;	
}