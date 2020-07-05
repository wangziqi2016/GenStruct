
CFLAGS=

.phony: all test

all: test

test: test.c error.c error.h
	gcc -c error.c -o error.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc error.o test.o -o test -Wall -Wextra $(CFLAGS)
