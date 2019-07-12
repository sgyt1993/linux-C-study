target = main
object = main.o fun1.o fun2.o sum.o
cc = gcc
cppflag = -I./
$(target):$(object)
	$(cc) -o $@  $^

%.o:%.c
	$(cc) -c $<  $(cppflag)
