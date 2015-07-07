CFLAGS=-Wall -g #show all warnings, debugging

all:
	gcc ${CFLAGS} ex1.c -o ex1
	gcc ${CFLAGS} ex3.c -o ex3
	gcc ${CFLAGS} ex4.c -o ex4
	gcc ${CFLAGS} ex5.c -o ex5
	gcc ${CFLAGS} ex6.c -o ex6
clean:
	rm -f ex1
	rm -f ex3
	rm -f ex4
	rm -f ex5
	rm -f ex6


