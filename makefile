SRCS = $(shell find src -name "*.c")

CC = gcc
CFLAGS = -Wall -Wextra -Werror $(shell pkg-config --cflags libmicrohttpd)
LDLIBS = $(shell pkg-config --libs libmicrohttpd)

all:
	mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) $(LDLIBS) -o build/lavender
