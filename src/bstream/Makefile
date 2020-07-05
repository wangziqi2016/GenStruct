
CFLAGS+=

.phony: all test

all: test

test: test.c
	gcc -c bstream.c -o bstream.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc test.o bstream.o -o test -Wall -Wextra $(CFLAGS)
