CC	= gcc
CFLAGS 	+= -std=c11 -lm -lpthread -lOpenCL -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= heat_diffusion.c
OBJS	= heat_diffusion.o
TARGS	= heat_diffusion

.PHONY: all

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $^

test: 
	make

clean: 
	rm $(OBJS)
	rm $(TARGS)

testrun :
	rm -rf reports extracted distances
	rm cell_distance.tar.gz
	tar -czvf cell_distance.tar.gz $(SRCS) ./makefile ./report.md
	/home/hpc2018/a3_grading/check_submission.py cell_distance.tar.gz

