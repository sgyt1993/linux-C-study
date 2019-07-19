//mmap 测试文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char* argv[])
{
	//打开一个文件,文件必须有长度
	int fd = open("mmapfile",O_RDWR);
	if(fd < 0)
	{
		perror("open file error");
		return -1;
	}
	
	int size = lseek(fd,0,SEEK_END);
	//mmap映射文件到系统内核区，成为进程通信文件
	//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	void * mm = mmap(NULL,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	if(mm == MAP_FAILED)
	{
		perror("mmap error");
		return -1;
	}
		
	
	//这里一定要将文件符位移到映射的size区，不然会读不到
	lseek(fd,0,SEEK_SET);
	
	//读取共享数据
	char buf[1024];
	memset(buf,0x00,sizeof(buf));
	int res = read(fd,buf,sizeof(buf));
	printf("res == [%d],buf == [%s]",res,buf);	

	return 0;
}
