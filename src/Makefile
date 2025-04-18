# Name of the final executable
TARGET := main

# Compiler and flags
CC := gcc
CFLAGS := -I../include -Wall -Wextra -O2
CPPFLAGS :=

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Default target
$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

# Compile .c to .o
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -f *.o
