CC = gcc
CFLAGS = -fopenmp -std=c99 -O0 -ggdb -Wall -pedantic
MKDIR = mkdir -p
RMDIR = rm -rf

SRC = ../color_bruteforcer.c
INCLUDE_DIR = ..

.PHONY: all runtests cleanall
all: testall runtests

testall: testall.c testcolorbruteforcer.c $(SRC)
	$(MKDIR) bin
	$(CC) $(CFLAGS) $(SRC) -I $(INCLUDE_DIR) -o bin/$@ \
		testall.c \
		testcolorbruteforcer.c \
		-lcunit

runtests:
	bin/testall

cleanall:
	$(RMDIR) bin
