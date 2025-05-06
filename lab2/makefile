src = lab2.c
obj = lab2.o 
opt = -g -c
all: lab2
lab2: $(obj)
	gcc $(obj) -l pthread -o lab2
lab2.o: $(src)
	gcc $(opt) $(src)
clean:
	rm lab2 *.o