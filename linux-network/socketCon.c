//客户端程序tcp的socket
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
	//int socket(int domain, int type, int protocol);客户端的返回的是通信描述符
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(socketfd == -1)
	{
		perror("socket fail\n");
		return -1;
	}

	//int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	struct sockaddr_in addr;
	addr.sin_family =  AF_INET;
	addr.sin_port = htons(8083);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int res = connect(socketfd,(struct sockaddr*)&addr,sizeof(addr));
	if(res == -1)
	{
		perror("connect fail\n");
		return -1;
	}
	
	//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	strcpy(buf,"sgyt 1024");
	res = send(socketfd,buf,sizeof(buf),0);
	printf("res = [%d],buf = [%s]\n",res,buf);
	
	//ssize_t recv(int sockfd, void *buf, size_t len, int flags);
	char buf2[1024];
	memset(buf2,0x00,sizeof(buf2));
	res =  recv(socketfd,buf2,sizeof(buf2),0);
	printf("res = [%d],buf2 = [%s]\n",res,buf2);

	printf("this is wait,now is start\n");


	close(socketfd);

}

