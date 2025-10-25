CC = gcc
CFLAGS = -Wall -pedantic -O3 -lm -ggdb -march=native

ifdef OMP
	CFLAGS += -fopenmp -DOPENMP
ifdef N
	CFLAGS += -DNUM_THREADS=$(N)
endif
endif

all: little

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

littleR: littleR.o
	$(CC) $(CFLAGS) -o $@ $^

little: main.o configuration.o util.o little.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f *.o littleR little main.exe