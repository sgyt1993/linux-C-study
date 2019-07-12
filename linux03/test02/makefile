main:main.o fun1.o fun2.o sum.o
	gcc -o main main.o fun1.o fun2.o sum.o 

main.o:main.c
	gcc -c main.c -I./
fun1.o:fun1.c
	gcc -c fun1.c
fun2.o:fun2.c
	gcc -c fun2.c
sum.o:sum.c
	gcc -c sum.c
