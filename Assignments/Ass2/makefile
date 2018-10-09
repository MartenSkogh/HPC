CC	= gcc
CFLAGS 	+= -std=gnu11 -lm -lpthread -march=native -O3
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
SRCS	= newton.c
DEPS 	?= newton.h
OBJS	= newton.o
TARGS	= newton

all: $(TARGS)

$(TARGS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $^
test: newton
	make
	./newton -t5 -l100 5

clean: 
	rm *.o
	rm newton
testRun :
	rm -rf pictures
	rm -rf extracted
	rm -rf reports
	tar -czvf newton.tar.gz ./newton.c ./makefile ./report.md
	/home/hpc2018/a2_grading/check_submission.py newton.tar.gz
