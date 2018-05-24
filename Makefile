CC = gcc
CFLAGS = -fopenmp -std=c99 -O3 -ggdb -Wall -pedantic
LDFLAGS = -lm
SRCDIR = src
TESTDIR = test
INC = -I$(SRCDIR) -I$(TESTDIR)

OBJS = color_bruteforcer.o argtable3.o
TESTS = testcolorbruteforcer.o

RM = rm -f

color_bruteforcer: $(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $^ $(INC) $(LDFLAGS)

color_bruteforcer_tests: $(OBJS) $(TESTS) testall.o
	$(CC) $(CFLAGS) -o $@ $^ $(INC) $(LDFLAGS) -lcunit

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(INC)

%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

.PHONY: test
test: color_bruteforcer_tests
	./$<

.PHONY: all
all: color_bruteforcer test

.PHONY: clean
clean:
	$(RM) color_burteforcer color_bruteforcer_tests *.o

