//这个是scandir获取文件夹信息的demo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc,char *argv[])
{
	//int scandir(const char *dirp, struct dirent ***namelist,
    //          int (*filter)(const struct dirent *),
    //          int (*compar)(const struct dirent **, const struct dirent **));
   struct dirent **namelist;
   int n;

   n = scandir(argv[1], &namelist, NULL, alphasort);
   if (n == -1) {
	   perror("scandir error\n");
	   exit(EXIT_FAILURE);
   }

   while (n--) {
	   printf("%s\n", namelist[n]->d_name);
	   free(namelist[n]);
   }
   free(namelist);

   exit(EXIT_SUCCESS);
}