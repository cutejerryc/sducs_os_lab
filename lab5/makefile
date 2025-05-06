hdrs = ipc.h
c_src = consumer.c ipc.c 
c_obj = consumer.o ipc.o 
b_src = barber.c ipc.c 
b_obj = barber.o ipc.o 
opts = -g -c
all: consumer barber 
consumer: $(c_obj)
	gcc $(c_obj) -o consumer 
consumer.o: $(c_src) $(hdrs)
	gcc $(opts) $(c_src)
barber: $(b_obj)
	gcc $(b_obj) -o barber 
barber.o:$(b_src) $(hdrs)
	gcc $(opts) $(b_src)
clean:
	rm consumer barber *.o