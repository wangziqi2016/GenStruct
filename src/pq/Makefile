
COMMON_PATH=../common
CFLAGS+=-I$(COMMON_PATH)

.phony: all test

all: test

test: test.c pq.c pq.h $(COMMON_PATH)/common.c $(COMMON_PATH)/common.h
	gcc -c pq.c -o pq.o -g -Wall -Wextra $(CFLAGS)
	gcc -c $(COMMON_PATH)/common.c -o $(COMMON_PATH)/common.o -g -Wall -Wextra $(CFLAGS)
	gcc -c test.c -o test.o -g -Wall -Wextra $(CFLAGS)
	gcc test.o pq.o $(COMMON_PATH)/common.o -o test -Wall -Wextra $(CFLAGS)