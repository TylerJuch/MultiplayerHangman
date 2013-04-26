all: server

test: server
	./server

server: server.cpp

clean:
	rm -f *.o server
