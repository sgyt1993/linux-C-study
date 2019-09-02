//bufferevent clinet  基本bufferevent函数写的一个客户端
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <sys/types.h>
#include <sys/socket.h>

//终端输入事件
//typedef void (*event_callback_fn)(evutil_socket_t fd, short events, void *arg)
void cmd_msg_cb(evutil_socket_t fd, short events, void *arg)
{
	char msg[1024];
	int readres;
	memset(msg,0x00,sizeof(msg));

	readres = read(fd,msg,sizeof(msg));
	if(readres < 0)
	{
		printf("read STDIN_FILENO error\n");
		exit(1);
	}

	//拿到bufferevent指针，目的的写到bufferevent的写缓存区
	struct bufferevent* bev = (struct  bufferevent*)arg;
	
	//int bufferevent_write(struct bufferevent *bufev, const void *data, size_t size);
	bufferevent_write(bev,msg,readres);

}

//bufferevet中的读回调
//typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);
void server_msg_cb(struct bufferevent *bev, void *ctx)
{
	char buf[1024];
	memset(buf,0x00,sizeof(buf));

	//size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size);
	size_t len = bufferevent_read(bev,buf,sizeof(buf));
	buf[len] = '\0';
	printf("buf = [%s]",buf);
}

//bufferevet中的事件回调
//typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);
void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if(what & BEV_EVENT_EOF)
	{
		printf("connect is closed\n");
	}else if(what & BEV_EVENT_ERROR)
	{
		printf("some other is error \n");
	}else if(what & BEV_EVENT_CONNECTED)
	{
		printf("connect is conneted \n");
		return;
	}

	//如果是释放链接，或者链接错误，释放缓冲器
	bufferevent_free(bev);
	//释放event事件 监控读终端
	struct event* ev = (struct event*)ctx;
	event_free(ev);
}

int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		//两个参数依次是服务器端的IP地址、端口号
        printf("please input 2 parameter\n");
        return -1;
	}

	//创建根节点
	struct event_base *base = event_base_new();

	//创建初始化buffer缓冲区
	//struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options);
	struct bufferevent* bev = bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE );

	//监听终端的输入
	struct event* ev_cmd = event_new(base,STDIN_FILENO,EV_READ | EV_PERSIST,cmd_msg_cb,(void *)bev);

	//事件上树
	event_add(ev_cmd,NULL);

	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//链接到服务器ip地址和端口初始化  socket文件描述符 socket +connect
	bufferevent_socket_connect(bev,(struct sockaddr*)&addr,sizeof(addr));

	//设置buffer的回调函数，主要设置读回调,传入参数时标准输入的读事件
	//void bufferevent_setcb(struct bufferevent *bufev,bufferevent_data_cb readcb,bufferevent_data_cb writecb,bufferevent_event_cb eventcb,void *cbarg);
	bufferevent_setcb(bev,server_msg_cb,NULL,event_cb,(void *)ev_cmd);

	//设置生效
	bufferevent_enable(bev, EV_READ | EV_PERSIST);

	event_base_dispatch(base);

	//释放
	event_free(ev_cmd);

	bufferevent_free(bev);

	event_base_free(base);

	printf("finish \n");
	return 0;
}