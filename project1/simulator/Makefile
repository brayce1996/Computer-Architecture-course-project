CC=gcc
CFLAGS=-g
OBS=./*.o
SRCS=./*.c

single_cycle : SRC
	$(CC) -Wall -g -o $@ $(OBS)

SRC : $(SRCS)
	$(CC) -Wall -g -c $(SRCS)

clean :
	rm -rf $(OBS)

