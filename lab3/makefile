srcs = lab3.c
objs = lab3.o
opts = -g -c
all: lab3
lab3: $(objs)
	gcc $(objs) -o lab3 -lreadline 
lab3.o: $(srcs)
	gcc $(opts) $(srcs)
clean:
	rm lab3 *.o