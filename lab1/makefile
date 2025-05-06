head = lab1.h
srcs = lab1.c
objs = lab1.o
opts = -g -c
all: lab1
lab1: $(objs)
	gcc $(objs) -o lab1 
lab1.o: $(srcs) $(head)
	gcc $(opts) $(srcs) 
clean:
	rm lab1 *.o