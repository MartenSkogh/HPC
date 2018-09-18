CC 	= gcc
CFLAGS += -O3 --std=c11

.PHONY : clean test

fileIO : fileIO.c
	$(CC) $(CFLAGS) -o fileIO fileIO.c

clean :
	rm fileIO.o

test :
	make
	./fileIO
