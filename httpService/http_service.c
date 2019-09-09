//基于http的容器
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


//封装的对象，放入epoll的参数中
typedef struct CLIENT
{
	int type;//type=1是socket类型，type=2 是clinet类型
	struct sockaddr *cli;
	int ftd;
}client;


void  http_service(client *c,int epfd);
ssize_t  readLine(int fd,void *vptr,size_t maxlen);
static ssize_t  char_read(int fd,char *c); 
int send_header(int fd,char *code,char *msg,char *fileType, int len);
int send_file(int fd,char *fileName);
char *get_mime_type(char *name);
void getDirstr(int fd,char *fileName,char *buffer,int epfd);


int main(int argc,char* argv[])
{
	//建立socket
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		perror("socket error\n");
		return -1;
	}

	//端口复用
	int opt = 1;
	int res = setsockopt(sfd,SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt));
	if(res < 0)
	{
		perror("setsockopt error\n");
		return -1;
	}

	//socket绑定端口和ip
	struct sockaddr_in addr;
	memset(&addr,0x00,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res = bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
	if(res < 0)
	{
		perror("bind error\n");
		return -1;
	}

	//监听文件描述符
	res = listen(sfd,256);
	if(res < 0)
	{
		perror("listen error\n");
		return 0;
	}

	//创建epoll树
	int epfd = epoll_create(1024);
	if(epfd < 0)
	{
		perror("epoll_create error\n");
		return -1;
	}

	//开始监听文件,开始监听
	struct epoll_event con_event;
	con_event.events = EPOLLIN;
	//将数据存入
	client soc;
	soc.type = 1;
	soc.ftd = sfd;
	con_event.data.ptr = &soc;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&con_event);

	struct epoll_event events[1024];
	int nread;
	int i=0;

	//链接的cliaddr
	struct sockaddr_in cliaddr;
	socklen_t addrlen;
	struct epoll_event cli_event;
	client cli_con;
	char cliip[16];
	int sockcli;
	client *c;
	while(1)
	{	
		memset(events,0x00,sizeof(events));
		nread = epoll_wait(epfd,events,1024,-1);
		if(nread < 0)
		{
			printf("nread == %d",nread);
			if(errno = EINTR)//被信号中断
			{
				continue;
			}
			break;
		}
	
		for(i=0;i<nread;i++)
		{
			//printf("data.fd == %d,sfd == %d\n",events[i].data.fd,sfd);
			c = (client*)events[i].data.ptr;
			if(c->ftd == sfd)
			{
				//说明stf改变，有链接进来
				memset(&cliaddr,0x00,sizeof(cliaddr));
				addrlen = sizeof(cliaddr);
				int cfd = accept(sfd,(struct sockaddr*)&cliaddr,&addrlen);
				if(cfd < 0)
				{
					perror("accept error\n");
					return -1;
				}
				
				//设置文件为非阻塞
				int flag = fcntl(cfd,F_GETFL);
				flag = flag|O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);

				//打印出链接信息
				memset(cliip,0x00,sizeof(cliip));
				printf("client ip = [%s],port = [%d]\n",inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,cliip,sizeof(cliip)),ntohs(cliaddr.sin_port));
				
				//清楚内存
				memset(&cli_event,0x00,sizeof(cli_event));
				memset(&cli_con,0x00,sizeof(cli_con));
				//将cfd上树
				cli_event.events = EPOLLIN;
				cli_con.type = 2;
				cli_con.cli = (struct sockaddr*)&cliaddr;
				cli_con.ftd = cfd;
				con_event.data.ptr = &cli_con;
				epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&con_event);
			}else{
				//处理请求
				http_service(c,epfd);
			}
		}
	}

	return 0;
}

//http请求处理
void  http_service(client *c,int epfd)
{
	int readres;
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	//读取请求行数据
	readres = readLine(c->ftd,buf,sizeof(buf));
	if(readres <= 0)
	{
		//printf("read error or client closed, n==[%d]\n", n);
		//关闭连接
		close(c->ftd);
		
		//将文件描述符从epoll树上删除
		epoll_ctl(epfd, EPOLL_CTL_DEL, c->ftd, NULL);
		return ;	
	}
	printf("%s\n",buf);
	//GET /hanzi.c HTTP/1.1  解析出请求的文件
	char reqType[16] = {0};
	char fileName[255] = {0};
	char protocal[16] = {0};
	sscanf(buf,"%[^ ] %[^ ] %[^ \r\n]", reqType, fileName, protocal);
	printf("reqType == %s\n",reqType);
	printf("fileName == %s\n",fileName);
	printf("protocal == %s\n",protocal);
	
	char *pFile = fileName+1;
	if(strlen(fileName)<=1)
	{
		strcpy(pFile, "./");
	}else
	{
		printf("[%s]\n", pFile);//去掉/的剩下的文件名称
	}
	
	//循环读取完剩余的数据
	while((readres=readLine(c->ftd, buf, sizeof(buf)))>0);
	
	//判断文件是否存在
	struct stat st;
	int res = stat(pFile,&st);
	if(res < 0)
	{
		//表示文件不存在
		printf("file is not exit\n");
		
		//发送头信息
		send_header(c->ftd,"404","NOT FOUND",get_mime_type(".html"), 0);//文件类型转换

		//发送文件能容
		send_file(c->ftd,"error.html");
	}else
	{
		//判断文件类型
		//普通文件
		if(S_ISREG(st.st_mode))
		{
			printf("type is file\n");
			
			//发送头部信息
			send_header(c->ftd, "200", "OK", get_mime_type(pFile), st.st_size);
			
			//发送文件内容
			send_file(c->ftd, pFile);
		}else if(S_ISDIR(st.st_mode))
		{
			char buffer[2048];
			memset(buffer,0x00,sizeof(buffer));

			printf("type is dir\n");
			//发送头部信息
			send_header(c->ftd, "200", "OK", get_mime_type(".html"), 0);

			//这个是目录文件
			//循环拿到每个文件下的名称放在两个文件中间
			send_file(c->ftd,"http/dir_header.html");
			
			getDirstr(c->ftd,pFile,buffer,epfd);
			printf("buffer == [%s]\n",buffer);
			write(c->ftd,buffer,strlen(buffer));
			
			send_file(c->ftd,"http/dir_tail.html");
		}
	}

}

//发送请求头
int send_header(int fd,char *code,char *msg,char *fileType, int len)
{
	char buf[1024] = {0};
	sprintf(buf,"HTTP/1.1 %s %s\r\n",code,msg);//请求行
	sprintf(buf+strlen(buf),"Content-Type:%s\r\n",fileType);//请求头中的文件类型
	if(len >0)
	{
		sprintf(buf+strlen(buf), "Content-Length:%d\r\n", len);//请求头中的文件长度
	}

	strcat(buf,"\r\n");
	write(fd,buf,strlen(buf));
	return 0;
}

//发送请求文件
int send_file(int fd,char *fileName)
{
	int fileid = open(fileName,O_RDONLY);
	if(fileid < 0)
	{
		perror("open file error\n");
		return -1;
	}

	char buf[1024];
	int readres;
	while(1)
	{
		memset(buf,0x00,sizeof(buf));
		readres = read(fileid,buf,sizeof(buf));
		if(readres <= 0)
		{
			break;
		}
		write(fd,buf,readres);
	}
	close(fileid);
}

//读取每个字符（带缓存）
//return -1  出错，0 结束，1 正常
static ssize_t  char_read(int fd,char *c)
{
	//printf("%s\n", __FUNCTION__);
	static char buf[100];
	static int read_cnt = 0;//这个是当前100个数据的数量，读取一次就删除一次
	static char *read_ptr;
	
	if(read_cnt <= 0)
	{
	again:
		if((read_cnt=read(fd,buf,sizeof(buf))) < 0)
		{
				if(errno == EINTR)
				{
					goto again;
				}
				return -1;
		}else if(read_cnt == 0){
			return 0;
		}
		//第一个字符
		read_ptr = buf;
	}

	read_cnt--;
	*c = *read_ptr++;//这边是后加，这里不能穿地址，如果串地址全在变
	return 1;

}

//读取一行数据
ssize_t  readLine(int fd,void *vptr,size_t maxlen)
{
	//printf("%s\n", __FUNCTION__);
	//读取每个字符，读取到\n的位置
	int i=0 ,res;
	char c,*ptr;

	ptr = vptr;//使地址相等

	for(i=0;i<maxlen;i++)
	{
		if((res = char_read(fd,&c))==1)
		{
			*ptr++ = c;
			if(c == '\n')
			{
				break;
			}
		}else if(res == 0)
		{
			*ptr = 0;//0 == '\0';在结束后面加上\0表示是字符串 
			return i-1;
		}else{
			return -1;
		}
	}

	*ptr = 0;//在结束后面加上\0表示是字符串 
	
	return i;
}

//读取文件夹下面的所有文件
void getDirstr(int fd,char *fileName,char *buffer,int epfd)
{
	struct dirent **namelist;
	int n;
	n = scandir(fileName,&namelist, NULL, alphasort);
	if(n == -1)
	{
		perror("scandir is error\n");
		close(fd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
		return;
	}
	
	char buf[1024];
	while(n--)
	{
		memset(buf,0x00,sizeof(buf));
		printf("%s\n",namelist[n]->d_name);
		//这里要判断是否是文件夹，如果是文件夹要加/
		if(namelist[n]->d_type == DT_REG)
		{
			sprintf(buf,"<li><a href=%s>%s</a></li>",namelist[n]->d_name,namelist[n]->d_name);
		}else if(namelist[n]->d_type == DT_DIR)
		{
			sprintf(buf,"<li><a href=%s/>%s</a></li>",namelist[n]->d_name,namelist[n]->d_name);
		}
		strcat(buffer,buf);
		free(namelist[n]);
	}

	free(namelist);
}

//文件转换
char *get_mime_type(char *name)
{
    char* dot;

    dot = strrchr(name, '.');	//自右向左查找‘.’字符;如不存在返回NULL
    /*
     *charset=iso-8859-1	西欧的编码，说明网站采用的编码是英文；
     *charset=gb2312		说明网站采用的编码是简体中文；
     *charset=utf-8			代表世界通用的语言编码；
     *						可以用到中文、韩文、日文等世界上所有语言编码上
     *charset=euc-kr		说明网站采用的编码是韩文；
     *charset=big5			说明网站采用的编码是繁体中文；
     *
     *以下是依据传递进来的文件名，使用后缀判断是何种文件类型
     *将对应的文件类型按照http定义的关键字发送回去
     */
    if (dot == (char*)0)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp( dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}




