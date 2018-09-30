CC		= gcc
OPTFLAG	= -O3
CFLAGS 	= -Wall --std=c11 -march=native -Iinclude

time: time.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

time.o: time.c
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<


#All files for the inlinging assignment
IL_TARG	= mainfile separatefile inlined
IL_SRC	= mainfile.c separatefile.c inlined.c
IL_OBJS	= mainfile.o separatefile.o inlined.o
OPTFLAG = -O2

inline: $(IL_TARG)

mainfile: mainfile.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

separatefile: separatefile.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

inlined: inlined.o
	$(CC) $(CFLAGS) $(OPTFLAG) -o $@ $<

%.o: %.c %.h
	$(CC) $(CFLAGS) $(OPTFLAG) -c -o $@ $<
	
clean: 
	rm *.o
