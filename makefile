all: struct13.o
	gcc -o program struct13.o

struct13.o: struct13.c
	gcc -c struct13.c

run:
	./program
