CC = gcc
CFLAGS = -Wall -pedantic -O3 -lm -ggdb -fopenmp

ifdef OMP
	CFLAGS += -fopenmp -DOPENMP
ifdef N
	CFLAGS += -DNUM_THREADS=$(N)
endif
endif


%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

littleR: littleR.o
	$(CC) $(CFLAGS) -o $@ $^

main: main.o configuration.o util.o little.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f *.o littleR main