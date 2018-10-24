CC	= mpicc
CFLAGS 	+= -std=c11 -lm -lpthread -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= dijkstra.c
OBJS	= dijkstra.o
TARGS	= dijkstra

.PHONY: all clean test testrun

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $^

test: cell_distance
	make
	./dijkstra -t1

clean: 
	rm $(OBJS)
	rm $(TARGS)

testrun :
	rm -rf reports extracted distances
	rm cell_distance.tar.gz
	tar -czvf cell_distance.tar.gz $(SRCS) ./makefile ./report.md
	/home/hpc2018/a3_grading/check_submission.py cell_distance.tar.gz

