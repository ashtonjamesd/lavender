SRCS = $(shell find src -name "*.c")
LIB_SRCS = $(filter-out src/main.c, $(SRCS))

CC = gcc
CFLAGS = -Wall -Wextra -Werror $(shell pkg-config --cflags libmicrohttpd sqlite3)
LDLIBS = $(shell pkg-config --libs libmicrohttpd sqlite3)
TEST_FLAGS = -Wno-unused-function -fsanitize=address -g

all:
	mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) $(LDLIBS) -o build/lavender

test:
	mkdir -p build
	for t in test/*_test.c; do $(CC) $$t $(LIB_SRCS) -Isrc $(CFLAGS) $(TEST_FLAGS) $(LDLIBS) -o build/$$(basename $$t .c) && ./build/$$(basename $$t .c) || exit 1; done

examples:
	mkdir -p build/example
	for f in example/*.c; do $(CC) $$f $(LIB_SRCS) -Isrc $(CFLAGS) $(LDLIBS) -o build/example/$$(basename $$f .c) || exit 1; done

.PHONY: all test examples
