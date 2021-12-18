CC = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: client_final server_final

client_final: client_final.o utils.o

server_final: server_final.o utils.o

server_final.o: server_final.c utils.h

client_final.o: client_final.c utils.h

utils.o: utils.c utils.h

clean:
	rm *.o
