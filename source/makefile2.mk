TARGET	=	client1	client2	server
CC	=	gcc
CFLAGS	=	-Wall	-g
normal	:	$(TARGET)

server	:server.c
	$(CC)	$(CFLAGS)	server.c	-o	server

client1	:	client1.c
	$(CC)	$(CFLAGS)	client1.c	-o	client1

client2	:	client2.c
	$(CC)	$(CFLAGS)	client2.c	-o	client2


clean	:
	$(RM) $(TARGET)


