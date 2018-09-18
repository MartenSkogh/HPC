CC = gcc
FLAGS = -O1

heap_alloc.o : heap_alloc.c
	$(CC) $(FLAGS) -o heap_alloc.o heap_alloc.c

clean :
	rm heap_alloc.o heap_alloc.o

test :
	make
	./heap_alloc.o | python tester.py
