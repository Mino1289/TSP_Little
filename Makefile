CC = gcc
CFLAGS = -Wall -pedantic -O3 -lm -ggdb -fopenmp


%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

littleR: littleR.o
	$(CC) $(CFLAGS) -o $@ $^

main: main.o util.o little.o configuration.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f *.o littleR main