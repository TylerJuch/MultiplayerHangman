

all: server users

test: server
	./server

server: server.o sockhelper.o Users.o
	g++ -lpthread *.o -o server

server.o:
	g++ -Wall -c -lpthread server.cpp

Users.o:
	g++ -Wall -c Users.cpp 

sockhelper.o:
	g++ -Wall -c sockhelper.cpp 

clean:
	rm -f *.o server
