CC		= gcc
OPTFLAG	?= -O0
CFLAGS 	= -Wall --std=c11 -march=native -Iinclude

all: time inline locality disk_test

time: time.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

time.o: time.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<


#All files for the inlinging assignment
IL_TARG	= mainfile separatefile inlined
IL_SRC	= mainfile.c separatefile.c inlined.c
IL_OBJS	= mainfile.o separatefile.o inlined.o

inline: $(IL_TARG)

mainfile: mainfile.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

separatefile: separatefile.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

inlined: inlined.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

%.o: %.c %.h
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<

# Make the locality stuff
DBFLAGS	= -pg -fprofile-arcs -ftest-coverage
locality: locality.o
	$(CC) $(CFLAGS) $(OPTFLAG) $(DBFLAGS) -o $@ $<
	
locality.o: locality.c
	$(CC) $(CFLAGS) $(OPTFLAG) $(DBFLAGS) -c -o $@ $<

# Make indirect addressing
addressing: indirect_addressing direct_addressing

indirect_addressing: indirect_addressing.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

indirect_addressing.o: indirect_addressing.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<

direct_addressing: direct_addressing.o
		$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

direct_addressing.o: direct_addressing.c
		$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<

# Make HDD-SDD test
disk_test: disk_test.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

disk_test.o: disk_test.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<

# Make valgrind tests
valgrind_test: valgrind_test.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

valgrind_test.o: valgrind_test.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<

clean: 
	rm *.o
