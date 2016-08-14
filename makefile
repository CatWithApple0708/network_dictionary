main:text

text:text.o server_process.o
	gcc -o text server_process.o text.o

text.o:text.c
	gcc -c -o text.o text.c

server_process.o:server_process.h server_process.c
	gcc -c -o server_process.o server_process.c


clean:
	rm *.o