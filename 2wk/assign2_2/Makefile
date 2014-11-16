PROGNAME = assign2_2.out
SRCFILES = reduction.c timer.c
TARNAME = assign2_2.tgz

RUNARGS = 4 1000000 #num_threads vec_size

CC = gcc

WARNFLAGS = -Wall -Werror-implicit-function-declaration -Wshadow \
		  -Wstrict-prototypes -pedantic-errors
CFLAGS = -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112 -fopenmp
LFLAGS = -lm -lrt

# Do some substitution to get a list of .o files from the given .c files.
OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))

.PHONY: all run runlocal plot clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS)

runlocal: $(PROGNAME)
	./$(PROGNAME) $(RUNARGS)

dist:
	tar cvzf $(TARNAME) Makefile *.c *.h

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME) result.txt plot.png
