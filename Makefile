CC=gcc
CFLAGS=-Wall -pthread
BIN=ckill
all:	ckill
debug: CC += -g
debug: ckill
ckill: 
	$(CC) $(CFLAGS)  ckill.c -o $(BIN)
clean:	
	rm $(BIN)

