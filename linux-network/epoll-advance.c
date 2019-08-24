// epoll���ڷ�����I/O�¼�����
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

//���������ļ������������Ϣ
struct myevent_s{
	int fd;																						//�������ļ�������
	int events;																				//��Ӧ�ļ����¼�
	void *arg;																				//���Ͳ���
	void (*call_back)(int fd,int events,void *arg);		//�ص�����
	int status;																				//�Ƿ��ڼ�����1->�����ϣ�0->��������
	char buf[BUFLEN];
	int len;
	long last_active;																	//��¼ÿ�μ�������ʱ��ֵ		
};





//ȫ�ֱ���
int g_efd;																					//����epoll_create���ص��ļ�������
int g_lfd;																					//����������ļ�������
int res;																						//���Ľ��
struct myevent_s g_events[MAX_EVENTS + 1];					//�Զ���ṹ���������顣 +1 ->listen fd
struct sockaddr_in cin;															//client�ĵ�ַ
char ipstr[16];																			//client�ĵ�ַ��ipstr


/*���ṹ��myevent_s��Ա������ʼ��*/
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


/*��epoll�����ĺ������ɾ��һ���ļ�������*/
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
	epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);                //�Ӻ���� efd �Ͻ� ev->fd ժ��
	
	printf("event del success [fd=%d]\n", ev->fd);
	return;
}


/*��epoll�����ĺ���������һ���ļ�������*/
void eventadd(int efd, int events, struct myevent_s *ev)
{
	struct epoll_event epv;
	memset(&epv,0x00,sizeof(epv));
	
	int op;
	epv.data.ptr = ev;
	epv.events = ev -> events = events;
	
	if(ev -> status == 1)
	{
		op = 	EPOLL_CTL_MOD;                    //�޸�������
	}else 
	{                                //���ں������
    op = EPOLL_CTL_ADD;          //����������� g_efd, ����status��1
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


/*  �����ļ�����������, epoll����, ���øú��� ��ͻ��˽������� */    //�ص�����-�������ļ����������Ͷ��¼�������
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
				break;	//�ҵ����е�Ԫ��
			}	
		}
		
		if(i == MAX_EVENTS)
		{
			printf("%s: max connect limit[%d]\n", __func__, strerror(errno));	
			break;//�����Ѿ����ˣ����ܼ���
		}			
		
		//��cfd����Ϊ������
		int flags = 0;
			flags = fcntl(cfd, F_GETFL, 0);
		flags = flags|O_NONBLOCK;
		if((flags = fcntl(cfd, F_SETFL, flags)) < 0)
		{
				 printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
	       break;//����goto
		}
		
		/*��cfd ����һ��myevent_s �ṹ�壬�ص����� ����Ϊrecvdata*/
		eventset(&g_events[i],cfd,recvdata,&g_events[i]);
		
		//��cfd�������g_efd�У��������¼�
		eventadd(g_efd,EPOLLIN, &g_events[i]);
	
	}while(0);
			
	return;
}

// �ص����� - ͨ�ŵ��ļ��������������¼�ʱ�򱻵���
void recvdata(int fd,int events,void *arg)
{
		printf("%s: fcntl is start\n",__func__);
		int len;
		struct myevent_s *ev = (struct myevent_s *)arg;
		
		//��ȡ�ͻ��˷���������
		memset(ev->buf,0x00,sizeof(ev->buf));
		len = read(ev->fd,ev->buf,sizeof(ev->buf));//���ļ������������ݴ���myevent_s��Աbuf��
		
		eventdel(g_efd, ev);//���ýڵ�Ӻ������ժ��
		
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

//�ص����� - ͨ�ŵ��ļ�����������д�¼�������
void senddata(int fd, int events, void *arg)
{
	int len;
	struct myevent_s *ev = (struct myevent_s *)arg;
	
	len = write(fd,ev->buf,sizeof(ev->buf));
	
	if(len > 0)
	{
			printf("send[fd=%d]-->[%d]:[%s]\n", fd, len, ev->buf);
			eventdel(g_efd, ev);                                //�Ӻ����g_efd���Ƴ�
			eventset(ev, fd, recvdata, ev);                     //����fd�� �ص�������Ϊ recvdata
      eventadd(g_efd, EPOLLIN, ev);                       //������ӵ�������ϣ� ��Ϊ�������¼�
	}else
	{
				close(ev->fd);                                      //�ر�����
        eventdel(g_efd, ev);                                //�Ӻ����g_efd���Ƴ�
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
	}
	
		
}


//����socket,��ʼ��lfd
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
	
	
	//���¼��ṹ�帳ֵ
	eventset(&g_events[MAX_EVENTS],g_lfd,acceptconn, &g_events[MAX_EVENTS]);//�����Ƕ�g_events[MAX_EVENTS]��������
	
	//�������ļ�����������
	eventadd(g_efd, EPOLLIN, &g_events[MAX_EVENTS]);
	
	return;
}

int  main(int argc,char *argv[])
{
		//������
		g_efd = epoll_create(MAX_EVENTS + 1);
		if(g_efd < 0)
		{
			perror("create epoll  error\n");
			return -1;	
		}
		
		//socket_bind_listen �������ļ�����������
		initlistensocket();
		
		struct epoll_event events[MAX_EVENTS+1];     //�����Ѿ���������¼����ļ����������� 
		
		int checkpos = 0,i;
		
		while(1)
		{
				/* ��ʱ��֤��ÿ�β���100�����ӣ�������listenfd ���ͻ���60����û�кͷ�����ͨ�ţ���رմ˿ͻ������� */
				long now = time(NULL);
				
				//ÿ��ѭ�����100����ʹ��checkpos���Ƽ�����
				for(i = 0;i<100;i++,checkpos++)
				{
						if (checkpos == MAX_EVENTS)
						{
			                checkpos = 0;
						}
						
						if (g_events[checkpos].status != 1)         //���ں���� g_efd ��
						{
			                continue;
						}
						
						long duration = now - g_events[checkpos].last_active;       //�ͻ��˲���Ծ������
						
						if (duration >= 60) 
						{
                close(g_events[checkpos].fd);                           //�ر���ÿͻ�������
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);                   //���ÿͻ��� �Ӻ���� g_efd�Ƴ�
            }
				}
				
				/*���������g_efd, ��������¼����ļ�����������events������, 1��û���¼�����, ���� 0*/
        int nfd = epoll_wait(g_efd, events, MAX_EVENTS+1, 1000);
        
        if(nfd < 0)
        {
        		printf("epoll_wait error, exit\n");
            break;
        }
        
        for(i = 0;i<nfd;i++)
        {
        		/*ʹ���Զ���ṹ��myevent_s����ָ��*/
        		struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
        		
        		//�������¼�
        		if((events[i].events && EPOLLIN) && (ev ->events && EPOLLIN))
        		{
        			printf("i == [%d],events =[%d],ev = [%d],EPOLLIN =[%d]\n",i,events[i].events,ev ->events,EPOLLIN);
        			ev->call_back(ev->fd, events[i].events, ev);
        		}
        		
        		//д�����¼�
        		if((events[i].events & EPOLLOUT) && (ev ->events & EPOLLOUT))
        		{
        			printf("i == [%d],fd =[%d],events =[%d],ev = [%d],EPOLLOUT = [%d]\n",i,ev->fd,events[i].events,ev ->events,EPOLLOUT);
        			ev->call_back(ev->fd, events[i].events, ev);
        		}
        		
        }
				
		}
		
		  /*�ر��ļ������� */
		close(g_efd);
		close(g_lfd);
    return 0;
}