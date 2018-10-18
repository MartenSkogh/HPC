CC	= gcc
CFLAGS 	+= -std=gnu11 -lm -lpthread -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= cell_distance.c
OBJS	= cell_distance.o
TARGS	= cell_distance

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -lgomp -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -fopenmp -c -o $@ $^
test: cell_distance
	make
	./cell_distance -t5 -l100 5

clean: 
	rm *.o
	rm cell_distance
