src = $(wildcard ./*.c)
target = main
object = $(patsubst %.c,%.o,$(src))
cc = gcc
cppflag = -I./
$(target):$(object)
	$(cc) -o $@ $^
%.o:%.c
	$(cc) -c $<  $(cppflag)
.PHONY:clean
clean:
	rm -f  $(object) $(target)
