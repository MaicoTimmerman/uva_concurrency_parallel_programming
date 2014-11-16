PROGNAME = assign1_1.out
SRCFILES = assign1_1.c file.c timer.c simulate.c
TARNAME = assign1_1.tgz

# i_max t_max num_threads
RUNARGS_T1 = 1000000 10000 8
# RUNARGS_T2 = 1000000 10000 2
# RUNARGS_T3 = 1000000 10000 4
# RUNARGS_T4 = 1000000 10000 8

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
	# prun -np 1 $(PROGNAME) $(RUNARGS_T1)
	# prun -np 1 $(PROGNAME) $(RUNARGS_T2)
	# prun -np 1 $(PROGNAME) $(RUNARGS_T3)
	prun -np 1 $(PROGNAME) $(RUNARGS_T4)

cgdb: $(PROGNAME)
	cgdb --args $(PROGNAME) $(RUNARGS_T3)

valgrind: $(PROGNAME)
	valgrind ./$(PROGNAME) $(RUNARGS_T3)

runlocal: $(PROGNAME)
	./$(PROGNAME) $(RUNARGS_T1)

plot: result.txt
	gnuplot plot.gnp
	$(IMAGEVIEW) plot.png

todo:
	-@for file in *.c *.h; do \
		grep -FHnT -e TODO $$file | \
			sed 's/:[ \t]*\/\//: \/\//' | \
			sed 's/:[ \t]*\/\*/: \/\*/'; \
		done; true

dist:
	tar cvzf $(TARNAME) Makefile *.c *.h data/

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME)
