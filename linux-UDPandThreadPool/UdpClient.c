//UDP clientµÄ·þÎñ
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
		
		char buf[1024];
		int readres;
		
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8888);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		while(1)
		{
				memset(buf,0x00,sizeof(buf));
				readres = read(STDIN_FILENO, buf, sizeof(buf));
				
				sendto(ftd,buf,sizeof(buf),0,(struct sockaddr*)&addr,sizeof(addr));
				
				memset(buf,0x00,sizeof(buf));
				readres = recvfrom(ftd,buf,readres,0,NULL,NULL);
				printf("readres = [%d],buf = [%s]",readres,buf);
				
		}
		
		close(ftd);
}