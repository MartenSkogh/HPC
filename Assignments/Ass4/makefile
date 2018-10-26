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
	./heat_diffusion 3 3 -i1e10 -d0.02 -n2
clean: 
	rm $(OBJS)
	rm $(TARGS)


