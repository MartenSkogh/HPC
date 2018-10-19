CC	= gcc
CFLAGS 	+= -std=c11 -lm -lpthread -fopenmp -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= cell_distanceSUCKYPHILIPVERSION.c
OBJS	= cell_distance.o
TARGS	= cell_distance

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -lgomp -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -fopenmp -c -o $@ $^
test: cell_distance
	make
	./cell_distance -t1

clean: 
	rm *.o
	rm cell_distance
