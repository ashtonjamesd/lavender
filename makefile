SRCS = $(shell find src -name "*.c")

CC = gcc
CFLAGS = -Wall -Wextra -Werror $(shell pkg-config --cflags libmicrohttpd)
LDLIBS = $(shell pkg-config --libs libmicrohttpd)

all:
	mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) $(LDLIBS) -o build/lavender

test:
	mkdir -p build
	$(CC) test/str_test.c $(filter-out src/main.c, $(SRCS)) -Isrc $(CFLAGS) -Wno-unused-function -fsanitize=address -g $(LDLIBS) -o build/str_test
	./build/str_test
	$(CC) test/mem_test.c $(filter-out src/main.c, $(SRCS)) -Isrc $(CFLAGS) -Wno-unused-function -fsanitize=address -g $(LDLIBS) -o build/mem_test
	./build/mem_test

examples:
	mkdir -p build/example
	for f in example/*.c; do $(CC) $$f $(filter-out src/main.c, $(SRCS)) -Isrc $(CFLAGS) $(LDLIBS) -o build/example/$$(basename $$f .c) || exit 1; done

.PHONY: all test examples
