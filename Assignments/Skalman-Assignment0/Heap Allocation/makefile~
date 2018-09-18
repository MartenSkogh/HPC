CC = gcc
FLAGS = -O1

stack_alloc.o : stack_alloc.c
	$(CC) $(FLAGS) -o stack_alloc.o stack_alloc.c

clean :
	rm stack_alloc.o heap_alloc.o

test :
	make
	./stack_alloc.o | python tester.py
