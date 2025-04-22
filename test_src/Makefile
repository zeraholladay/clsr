# Compiler and flags
CC := gcc
CFLAGS := -I../include -Wall -Wextra -O2
CPPFLAGS :=
BIN := ../test_bin/

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BIN)%.o,$(SRCS))

CHECK := check

OS := $(shell uname)

ifeq ($(OS), Darwin)
	INCLUDES := $(shell pkg-config --cflags $(CHECK))
endif

# Default target
.PHONY: all
all: $(OBJS)

# Compile .c to $(BIN).o
$(BIN)%.o: %.c | $(BIN)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN):
	@mkdir -p $(BIN)

# Clean up
.PHONY: clean
clean:
	@rm -rf $(BIN)test_*.o
