SRCS = $(shell find src -name "*.c")

CC = gcc
CFLAGS = -Wall -Wextra -Werror

all:
	mkdir -p build
	$(CC) $(SRCS) $(CFLAGS) -o build/lavender