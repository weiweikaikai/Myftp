CC=gcc
CFLAGS= -g #-Wall
BIN=myftp
LIB=-lcrypt
OBJS=main.o sysutil.o session.o ftpproto.o privparent.o str_op.o parseconfig.o config.o privsock.o
$(BIN):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)
%.o:%.c
	$(CC) -c $< -o $@  $(CFLAGS)
clean:
	rm -f *.o $(BIN)
