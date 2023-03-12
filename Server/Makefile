all: server
test: test.c room.o user.o
	gcc test.c room.o user.o -o test 
server: server.o user.o authenticate.o room.o handler.o
	gcc server.o user.o authenticate.o handler.o room.o -o server -lpthread
room.o: room.c room.h
	gcc -c room.c -o room.o
user.o: user.c user.h
	gcc -c user.c -o user.o
authenticate.o: authenticate.c authenticate.h
	gcc -c authenticate.c -o authenticate.o
server.o: server.c user.h authenticate.h room.h handler.h
	gcc -c server.c -o server.o
handler.o: handler.c handler.h
	gcc -c handler.c -o handler.o
clean:
	rm -f *.o server