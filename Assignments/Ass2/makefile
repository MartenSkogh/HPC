CC	= gcc
CFLAGS 	+= -lm -Wall -march=native -O3
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
