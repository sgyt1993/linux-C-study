//这个是libevent的一个service
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <unistd.h>

typedef struct 
{
	int flag;
	int ftd;
	struct event *ev;
}Client;

Client client[20];

void initCli(Client client[],int length)
{
	int i = 0;
	for(i= 0;i<length;i++)
	{
		client[i].flag = -1;
	} 
}

int getCli(Client client[],int length)
{
	int i = 0;
	int res = -1;
	for(i= 0;i<length;i++)
	{
		if(client[i].flag == -1)
		{
			res = i;
			break;
		}
	} 

	return res;
}

int getCliByFd(Client client[],int length,int fd)
{
	int i = 0;
	int res = -1;
	for(i= 0;i<length;i++)
	{
		if(client[i].ftd == fd)
		{
			res = i;
			break;
		}
	} 

	return res;
}

//数据传输
void readcb(evutil_socket_t fd, short events, void *arg)
{
	struct event_base *base = (struct event_base*)arg;
	
	char buf[1024];
	int readres;
	memset(buf,0x00,sizeof(buf));

	readres = read(fd,buf,sizeof(buf));
	if(readres <= 0)
	{
		//释放链接
		printf("connect is close\n");
		//下树
		int evid = getCliByFd(client,sizeof(client)/sizeof(Client),fd);
		if(evid > 0)
		{
			event_del(client[evid].ev);
		}
		close(fd);
		return;
	}
	printf("res = [%d],buf = [%s]\n",readres,buf);

	//写回数据
	write(fd,buf,sizeof(buf));
}

//链接事件
//typedef void (*event_callback_fn)(evutil_socket_t fd, short events, void *arg);
void conncb(evutil_socket_t fd, short events, void *arg)
{
	struct event_base *base = (struct event_base*)arg;
	
	int freeId = getCli(client,sizeof(client)/sizeof(Client));
	
	if(freeId != -1)
	{
		struct sockaddr_in cl;
		char clip[16];
		memset(&cl,0x00,sizeof(cl));
		memset(clip,0x00,sizeof(clip));
		socklen_t clen = sizeof(cl);
		int cfd = accept(fd,(struct sockaddr*)&cl,&clen);
		if(cfd < 0)
		{
			perror("accept error\n");
			return;
		}
		printf("clinet ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&cl.sin_addr.s_addr,clip,sizeof(clip)),ntohs(cl.sin_port));
	
		//将cfd上到树上
		struct event *ev = event_new(base,cfd,EV_READ|EV_PERSIST,readcb,base);
		int res = event_add(ev,NULL);
		if(res < 0)
		{
			printf("event_add error\n");
			return ;
		}

		client[freeId].flag = 1;
		client[freeId].ftd = cfd;
		client[freeId].ev = ev;
		printf("array index = [%d]\n",freeId);
	}
	
	

}


int main(int argc,char* argv[])
{
	//初始化数组
	initCli(client,sizeof(client)/sizeof(Client));
	
	//建立socket
	int lfd = socket(AF_INET,SOCK_STREAM,0);

	//端口复位
	int opt = 1;
	int res = setsockopt(lfd,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(int));
	if(res < 0)
	{
		perror("setsocketopt error\n");
		return -1;
	}

	//ip,端口绑定
	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res = bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error\n");
		return -1;
	}

	//端口监听
	res = listen(lfd,256);
	if(res < 0)
	{
		perror("listen error\n");
		return -1;
	}

	//创建地基
	struct event_base *base = event_base_new();
	
	//创建lfd的对应事件
	//struct event *event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn cb, void *arg);
	struct event *ev = event_new(base,lfd,EV_READ|EV_PERSIST,conncb,base);
	
	//加事件加入到地基上
	//int event_add(struct event *ev, const struct timeval *timeout);
	res = event_add(ev,NULL);
	if(res < 0)
	{
		perror("event_add error\n");
		return -1;
	}

	//进入循环事件中
	event_base_dispatch(base);
	
	//释放资源
	event_base_free(base);
	event_free(ev);

	close(lfd);
	return 0;
}
