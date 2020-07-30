
COMMON_PATH=../common
CFLAGS+=-I$(COMMON_PATH)

.phony: all test

all: test

test: test.c bstream.c bstream.h $(COMMON_PATH)/common.c $(COMMON_PATH)/common.h
	gcc -c bstream.c -o bstream.o -g -Wall -Wextra $(CFLAGS)
	gcc -c $(COMMON_PATH)/common.c -o $(COMMON_PATH)/common.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc test.o bstream.o $(COMMON_PATH)/common.o -o test -Wall -Wextra $(CFLAGS)
