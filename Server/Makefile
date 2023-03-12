all: server
server: server.o user.o authenticate.o
	gcc server.o user.o authenticate.o -o server -lpthread
user.o: user.c user.h
	gcc -c user.c -o user.o
authenticate.o: authenticate.c authenticate.h
	gcc -c authenticate.c -o authenticate.o
server.o: server.c user.h authenticate.h
	gcc -c server.c -o server.o
clean:
	rm -f *.o server