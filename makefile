LINKFLAGS = -l sqlite3
main:text

text:text.o server_process.o dbuser.o
	gcc -o text server_process.o text.o dbuser.o $(LINKFLAGS)

text.o:text.c
	gcc -c -o text.o text.c

server_process.o:server_process.h server_process.c
	gcc -c -o server_process.o server_process.c

dbuser.o:dbuser.c dbuser.h
	gcc -c -o dbuser.o dbuser.c



clean:
	rm *.o text