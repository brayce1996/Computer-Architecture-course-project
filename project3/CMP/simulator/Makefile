CC=gcc
CFLAGS=-g
OBS=./*.o
SRCS=./*.c

CMP : SRC
	$(CC) -Wall -g -o $@ $(OBS)

SRC : $(SRCS)
	$(CC) -Wall -g -c $(SRCS)

clean :
	rm -rf $(OBS)

