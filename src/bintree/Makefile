
COMMON_PATH=../common
CFLAGS+=-I$(COMMON_PATH)

.phony: all test

all: test

test: test.c bintree.c bintree.h $(COMMON_PATH)/common.c $(COMMON_PATH)/common.h
	gcc -c bintree.c -o bintree.o -g -Wall -Wextra $(CFLAGS)
	gcc -c $(COMMON_PATH)/common.c -o $(COMMON_PATH)/common.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc test.o bintree.o $(COMMON_PATH)/common.o -o test -Wall -Wextra $(CFLAGS)