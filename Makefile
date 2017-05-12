CC = gcc
CFLAGS = -fopenmp -std=c99 -O3 -ggdb -Wall -pedantic
RM = rm -f

all: color_bruteforcer

color_bruteforcer: color_bruteforcer.o argtable3.o main.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c

color_bruteforcer.o: color_bruteforcer.c

argtable3.o: argtable3.c

clean:
	$(RM) color_burteforcer *.o