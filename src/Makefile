# Compiler and flags
CC ?= gcc
CFLAGS := -I../include -Wall -Wextra -O2
BIN = ../bin/

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)%.o,$(SRCS))

# Default target
.PHONY: all
all: $(OBJS)

# Compile .c to $(BIN).o
$(BIN)%.o: %.c | $(BIN)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Ensure $(BIN) exists before compiling
$(BIN):
	@mkdir -p ../bin

# Clean up
.PHONY: clean
clean:
	rm -rf $(BIN)*.o
