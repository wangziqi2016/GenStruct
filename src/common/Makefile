
CFLAGS+=-lm

.phony: all test

all: test

test: test.c common.c common.h
	gcc -c common.c -o common.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc common.o test.o -o test -Wall -Wextra $(CFLAGS)
