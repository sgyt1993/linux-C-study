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
		int sfd = socket(AF_UNIX,SOCK_STREAM,0);
		
		int res = unlink("./service.sock");//删除文件，方便下次启动
		if(res < 0)
		{
			perror("unlink is error\n");
			return -1;	
		}
		
		struct sockaddr_un ser;
		memset(&ser,0x00,sizeof(ser));
		ser.sun_family = AF_UNIX;
		memcpy(ser.sun_path,"./service.sock",sizeof("./service.sock"));
		res = bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
		if(res < 0 )
		{
			perror("bind error\n");
			return -1;	
		}
		
		res = listen(sfd,256);
		if(res < 0)
		{
			perror("listen error\n");
			return -1;	
		}
		
		//写最简单的写法了
		struct sockaddr_un client;
		memset(&client,0x00,sizeof(client));
		socklen_t len = sizeof(client);
		int afd = accept(sfd,(struct sockaddr*)&client,&len);
		if(afd <= 0)
		{
			perror("accept error\n");
			return -1;	
		}
		
		printf("client->[%s]\n", client.sun_path);
		
		char buf[1024];
		int readres;
		int i = 0;
		while(1)
		{
			memset(buf,0x00,sizeof(buf));
			readres = read(afd,buf,sizeof(buf));
			printf("readres = [%d],buf = [%s]",readres,buf);
			
			if(readres < 0)
			{
				printf("this connect is close");	
				break;
			}
			
			for(i =0;i<readres;i++)
			{
					buf[i] = toupper(buf[i]);
			}
			
			write(afd,buf,readres);
			
		}
		
		close(afd);
		return 0;	
}