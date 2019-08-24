// epoll基于非阻塞I/O事件驱动
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#define MAX_EVENTS 1024
#define BUFLEN 4096

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

//描述就绪文件描述符相关信息
struct myevent_s{
	int fd;																						//监听的文件描述符
	int events;																				//对应的监听事件
	void *arg;																				//泛型参数
	void (*call_back)(int fd,int events,void *arg);		//回调函数
	int status;																				//是否在监听：1->在树上，0->不在树上
	char buf[BUFLEN];
	int len;
	long last_active;																	//记录每次加入树的时间值		
};





//全局变量
int g_efd;																					//保存epoll_create返回的文件描述符
int g_lfd;																					//保存监听的文件描述符
int res;																						//最后的结果
struct myevent_s g_events[MAX_EVENTS + 1];					//自定义结构体类型数组。 +1 ->listen fd
struct sockaddr_in cin;															//client的地址
char ipstr[16];																			//client的地址，ipstr


/*将结构体myevent_s成员变量初始化*/
void eventset(struct myevent_s *ev,int fd,void (*call_back)(int, int, void *),void *arg)
{
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	ev->last_active = time(NULL);
	return ; 
}


/*从epoll监听的红黑树中删除一个文件描述符*/
void eventdel(int efd,struct myevent_s *ev)
{
	struct epoll_event epv;
	memset(&epv,0x00,sizeof(epv));
	
	if(ev->status != 1)
	{
			return;	
	}	
	
	epv.data.ptr = ev;
	ev->status = 0; 
	epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);                //从红黑树 efd 上将 ev->fd 摘除
	
	printf("event del success [fd=%d]\n", ev->fd);
	return;
}


/*向epoll监听的红黑树，添加一个文件描述符*/
void eventadd(int efd, int events, struct myevent_s *ev)
{
	struct epoll_event epv;
	memset(&epv,0x00,sizeof(epv));
	
	int op;
	epv.data.ptr = ev;
	epv.events = ev -> events = events;
	
	if(ev -> status == 1)
	{
		op = 	EPOLL_CTL_MOD;                    //修改其属性
	}else 
	{                                //不在红黑树里
    op = EPOLL_CTL_ADD;          //将其加入红黑树 g_efd, 并将status置1
    ev->status = 1;
  }
  
  if(epoll_ctl(efd, op, ev->fd, &epv) < 0)
  {
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
	}
    else
	{
        printf("event add OK [fd=%d], op=%d, events[%0X]\n", ev->fd, op, events);
	}
		
	return;
		
}


/*  当有文件描述符就绪, epoll返回, 调用该函数 与客户端建立链接 */    //回调函数-监听的文件描述符发送读事件被调用
void acceptconn(int lfd,int events,void *arg)
{
	printf("%s: fcntl is start\n",__func__);
	memset(&cin,0x00,sizeof(cin));
	memset(ipstr,0x00,sizeof(ipstr));
	socklen_t len = sizeof(cin);
	int cfd, i;
	cfd = accept(lfd,(struct sockaddr*)&cin,&len);
	printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&cin.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(cin.sin_port));
	
	
	do 
	{
		for(i=0;i<MAX_EVENTS;i++)
		{
			if(g_events[i].status == 0)
			{
				break;	//找到空闲的元素
			}	
		}
		
		if(i == MAX_EVENTS)
		{
			printf("%s: max connect limit[%d]\n", __func__, strerror(errno));	
			break;//数据已经满了，不能加入
		}			
		
		//将cfd设置为非阻塞
		int flags = 0;
			flags = fcntl(cfd, F_GETFL, 0);
		flags = flags|O_NONBLOCK;
		if((flags = fcntl(cfd, F_SETFL, flags)) < 0)
		{
				 printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
	       break;//避免goto
		}
		
		/*将cfd 设置一个myevent_s 结构体，回调函数 设置为recvdata*/
		eventset(&g_events[i],cfd,recvdata,&g_events[i]);
		
		//将cfd添加树上g_efd中，监听读事件
		eventadd(g_efd,EPOLLIN, &g_events[i]);
	
	}while(0);
			
	return;
}

// 回调函数 - 通信的文件描述符发生读事件时候被调用
void recvdata(int fd,int events,void *arg)
{
		printf("%s: fcntl is start\n",__func__);
		int len;
		struct myevent_s *ev = (struct myevent_s *)arg;
		
		//读取客户端发来的数据
		memset(ev->buf,0x00,sizeof(ev->buf));
		len = read(ev->fd,ev->buf,sizeof(ev->buf));//读文件描述符，数据存入myevent_s成员buf中
		
		eventdel(g_efd, ev);//将该节点从红黑树上摘除
		
		if(len > 0)
		{
			ev->len = len;
			ev ->buf[len] = '\0';
			printf("C[%d]:%s\n", fd, ev->buf);
			
			eventset(ev,fd,senddata,ev);
			eventadd(g_efd, EPOLLOUT, ev);
		}
		else if(len = 0)
		{
			close(ev->fd);
			printf("[fd=%d] pos[%ld], closed\n", fd, ev-g_events);
		}else 
		{
        close(ev->fd);
        printf("read [fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }

    return;
		
}

//回调函数 - 通信的文件描述符发生写事件被调用
void senddata(int fd, int events, void *arg)
{
	int len;
	struct myevent_s *ev = (struct myevent_s *)arg;
	
	len = write(fd,ev->buf,sizeof(ev->buf));
	
	if(len > 0)
	{
			printf("send[fd=%d]-->[%d]:[%s]\n", fd, len, ev->buf);
			eventdel(g_efd, ev);                                //从红黑树g_efd中移除
			eventset(ev, fd, recvdata, ev);                     //将该fd的 回调函数改为 recvdata
      eventadd(g_efd, EPOLLIN, ev);                       //从新添加到红黑树上， 设为监听读事件
	}else
	{
				close(ev->fd);                                      //关闭链接
        eventdel(g_efd, ev);                                //从红黑树g_efd中移除
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
	}
	
		
}


//创建socket,初始化lfd
void initlistensocket()
{
	g_lfd = socket(AF_INET,SOCK_STREAM,0);
	if(g_lfd < 0)
	{
		perror("socket error\n");
	}
	
	int opt = 1;
	setsockopt(g_lfd,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(int));
	
	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res = bind(g_lfd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error\n");
	}
	
	res = listen(g_lfd,20);
	
	
	//对事件结构体赋值
	eventset(&g_events[MAX_EVENTS],g_lfd,acceptconn, &g_events[MAX_EVENTS]);//仅仅是对g_events[MAX_EVENTS]进行设置
	
	//将监听文件描述符上树
	eventadd(g_efd, EPOLLIN, &g_events[MAX_EVENTS]);
	
	return;
}

int  main(int argc,char *argv[])
{
		//创建树
		g_efd = epoll_create(MAX_EVENTS + 1);
		if(g_efd < 0)
		{
			perror("create epoll  error\n");
			return -1;	
		}
		
		//socket_bind_listen 将监听文件描述符上树
		initlistensocket();
		
		struct epoll_event events[MAX_EVENTS+1];     //保存已经满足就绪事件的文件描述符数组 
		
		int checkpos = 0,i;
		
		while(1)
		{
				/* 超时验证，每次测试100个链接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭此客户端链接 */
				long now = time(NULL);
				
				//每次循环检查100个，使用checkpos控制检查对象
				for(i = 0;i<100;i++,checkpos++)
				{
						if (checkpos == MAX_EVENTS)
						{
			                checkpos = 0;
						}
						
						if (g_events[checkpos].status != 1)         //不在红黑树 g_efd 上
						{
			                continue;
						}
						
						long duration = now - g_events[checkpos].last_active;       //客户端不活跃的世间
						
						if (duration >= 60) 
						{
                close(g_events[checkpos].fd);                           //关闭与该客户端链接
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);                   //将该客户端 从红黑树 g_efd移除
            }
				}
				
				/*监听红黑树g_efd, 将满足的事件的文件描述符加至events数组中, 1秒没有事件满足, 返回 0*/
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000);
        
        if(nfd < 0)
        {
        		printf("epoll_wait error, exit\n");
            break;
        }
        
        for(i = 0;i<nfd;i++)
        {
        		/*使用自定义结构体myevent_s类型指针*/
        		struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
        		
        		//读就绪事件
        		if((events[i].events && EPOLLIN) && (ev ->events && EPOLLIN))
        		{
        			printf("i == [%d],events =[%d],ev = [%d],EPOLLIN =[%d]\n",i,events[i].events,ev ->events,EPOLLIN);
        			ev->call_back(ev->fd, events[i].events, ev);
        		}
        		
        		//写就绪事件
        		if((events[i].events & EPOLLOUT) && (ev ->events & EPOLLOUT))
        		{
        			printf("i == [%d],fd =[%d],events =[%d],ev = [%d],EPOLLOUT = [%d]\n",i,ev->fd,events[i].events,ev ->events,EPOLLOUT);
        			ev->call_back(ev->fd, events[i].events, ev);
        		}
        		
        }
				
		}
		
		  /*关闭文件描述符 */
		close(g_efd);
		close(g_lfd);
    return 0;
}