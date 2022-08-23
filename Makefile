CFLAGS=-g -Wall
CC=gcc

all: server client clean

clean:
	rm *.o

server: server.o
	$(CC) -o server server.o 

server.o: server.c network.h
	$(CC) $(CFLAGS) -c server.c

client: client.o
	$(CC) -o client client.o 

client.o: client.c network.h
	$(CC) $(CFLAGS) -c client.c
