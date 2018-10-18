CC	= gcc
CFLAGS 	+= -std=c11 -lm -lpthread -fopenmp -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= cell_distance.c
DEPS 	?= cell_distance.h
OBJS	= cell_distance.o
TARGS	= cell_distance

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $^
test: cell_distance
	make
	./cell_distance -t5 -l100 5

clean: 
	rm *.o
	rm cell_distance
