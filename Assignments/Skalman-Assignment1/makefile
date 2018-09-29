CC		= gcc
CFLAGS	= -Wall -O3 --std=c11 -march=native
OBJS 	= main.o
LIBS	= -lm

.PHONY: clean all

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) 

clean:
	rm $(OBJS)


