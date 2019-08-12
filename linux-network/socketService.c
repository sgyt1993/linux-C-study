//服务端程序tcp的socket
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
	//int socket(int domain, int type, int protocol);
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(socketfd == -1)
	{
		perror("socket fail\n");
		return -1;
	}

	//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));//先清空对象
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8083);//这个java不一样，内部会自动转换，从低端字节序-》网络字节序
	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr);//先把10转换成16进制,再转成网络字节序
	int res = bind(socketfd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error\n");
		return -1;
	};

	// int listen(int sockfd, int backlog);
	res = listen(socketfd,256);
	if(res == -1)
	{
		perror("listen error\n");
		return -1;
	};

	//int accept(int socket, struct sockaddr *restrict address,socklen_t *restrict address_len);
	struct sockaddr_in rece;
	memset(&rece,0x00,sizeof(rece));
	socklen_t len = sizeof(rece);
	int acpfd  = accept(socketfd,(struct sockaddr*)&rece,&len);
	if(acpfd == -1)
	{
		perror("accept error\n");	
	};

	//打印出链接的ip地址
	//const char *inet_ntop(int af, const void *src,char *dst, socklen_t size);
	char ds[16];
	memset(ds,0x00,sizeof(ds));
	inet_ntop(AF_INET,&rece.sin_addr.s_addr,ds,sizeof(ds));
	printf("client ip = [%s],client port=[%d]",ds,ntohs(rece.sin_port));


	//验证accept是一个阻塞函数
	printf("accept is wait,now is start\n");

	char buf[1024];
	char buf2[1204];
	memset(buf2,0x00,sizeof(buf2));
	memset(buf,0x00,sizeof(buf));

	strcpy(buf2,"connect is ok");
	while(1)
	{
		int readres = read(acpfd,buf,sizeof(buf));
		if(readres == 0)
		{
			break;	
		}
		printf("readres = [%d],buf = [%s]\n",readres,buf);
		//当接收到信息的时候传送给客户端
		sleep(10);
		send(acpfd,buf2,sizeof(buf2),0);
	}

	close(acpfd);
	close(socketfd);

}

