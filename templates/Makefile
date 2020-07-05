
CFLAGS=

.phony: all test

all: test

test: test.c
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc test.o -o test -Wall -Wextra $(CFLAGS)