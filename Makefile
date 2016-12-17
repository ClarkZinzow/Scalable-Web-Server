#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o request.o networks.o client.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client output.cgi

server: server.o request.o networks.o
	$(CC) $(CFLAGS) -o server server.o request.o networks.o $(LIBS)

client: client.o networks.o
	$(CC) $(CFLAGS) -o client client.o networks.o

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) server client output.cgi
