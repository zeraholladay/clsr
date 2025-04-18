# Name of the final executable
TARGET := main

# Compiler and flags
CC ?= gcc
CFLAGS ?= -I../include -Wall -Wextra -O2
CPPFLAGS ?=

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,../bin/%.o,$(SRCS))

# Default target
$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

# Compile .c to ../bin/.o
../bin/%.o: %.c | ../bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Ensure ../bin/ exists before compiling
../bin/:
	mkdir -p bin

# Clean up
.PHONY: clean
clean:
	rm -rf bin $(TARGET)
