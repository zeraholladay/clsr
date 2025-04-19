# Compiler and flags
CC ?= gcc
CFLAGS ?= -I../include -Wall -Wextra -O2
CPPFLAGS ?=
BIN = ../bin/

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)%.o,$(SRCS))

OS := $(shell uname)

ifeq ($(OS), Darwin)
	INCLUDES := $(shell pkg-config --cflags glib-2.0)
endif

# Default target
.PHONY: all
all: $(OBJS)

# Compile .c to $(BIN).o
$(BIN)%.o: %.c | $(BIN)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Ensure $(BIN) exists before compiling
$(BIN):
	@mkdir -p ../bin

# Clean up
.PHONY: clean
clean:
	@rm -rf $(BIN)*.o
