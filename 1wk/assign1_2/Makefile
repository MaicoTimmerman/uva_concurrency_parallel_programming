PROGNAME = assign1_2.out
SRCFILES = assign1_2.c timer.c
TARNAME = assign1_2.tgz

# i_max t_max num_threads
RUNARGS_T1 = 100
RUNARGS_T2 = 500
RUNARGS_T3 = 1000
RUNARGS_T4 = 5000

IMAGEVIEW = display
CC = gcc

WARNFLAGS = -Wall -Werror-implicit-function-declaration -Wshadow \
		  -Wstrict-prototypes -pedantic-errors
CFLAGS = -std=c99 -ggdb -O2 $(WARNFLAGS) -D_POSIX_C_SOURCE=200112
LFLAGS = -lm -lrt -lpthread

# Do some substitution to get a list of .o files from the given .c files.
OBJFILES = $(patsubst %.c,%.o,$(SRCFILES))

.PHONY: all run runlocal plot clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	prun -v -np 1 $(PROGNAME) $(RUNARGS_T3)

cgdb: $(PROGNAME)
	cgdb --args $(PROGNAME) $(RUNARGS_T2)

valgrind: $(PROGNAME)
	valgrind ./$(PROGNAME) $(RUNARGS_T2)

runlocal: $(PROGNAME)
	./$(PROGNAME) $(RUNARGS_T4)


todo:
	-@for file in *.c *.h; do \
		grep -FHnT -e TODO $$file | \
			sed 's/:[ \t]*\/\//: \/\//' | \
			sed 's/:[ \t]*\/\*/: \/\*/'; \
		done; true

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME)
