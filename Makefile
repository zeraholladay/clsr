# src

# Compilers and flags
CC ?= gcc
CFLAGS := -Iinclude -Wall -Wextra -O2

INCLUDE := include
SRC := src

# Add -d for flex debugging
FLEX := flex
BISON := bison
GPERF := gperf

FLEX_SRC := $(SRC)/lexer.l
BISON_SRC := $(SRC)/parser.y
GPERF_SCR := $(SRC)/prim_op.gperf

MAIN_C := $(SRC)/main.c
FLEX_C := $(SRC)/lexer.c
BISON_C := $(SRC)/parser.c
BISON_H := $(INCLUDE)/parser.h
GPERF_C := $(SRC)/prim_op.c

SRC_BIN := bin/

SRC_SRCS := $(wildcard $(SRC)/*.c)
SRC_SRCS += $(FLEX_C) $(BISON_C) $(GPERF_C)
SRC_SRCS_UNIQUE := $(sort $(SRC_SRCS))
SRC_OBJS := $(patsubst $(SRC)/%.c, $(SRC_BIN)%.o, $(SRC_SRCS_UNIQUE))
SRC_BINS := $(patsubst $(SRC)/%.c, $(SRC_BIN)%, $(SRC_SRCS_UNIQUE))

# dependencies
CHECK := check

OS := $(shell uname)

ifeq ($(OS), Darwin)
	CHECK_FLAGS := $(shell pkg-config --libs $(CHECK))
endif

REPL = bin/repl

.PHONY: all
all: build
	@$(MAKE) CFLAGS="-DDEBUG" -C $(SRC)/
	$(CC) $(CFLAGS) -o $(REPL) $(SRC_OBJS) $(FLEX_LIB)

build: $(BISON_C) $(FLEX_C) $(GPERF_C) $(SRC_OBJS)

$(FLEX_C): $(FLEX_SRC) $(BISON_H)
	$(FLEX) -o $(FLEX_C) $(FLEX_SRC)

$(BISON_C): $(BISON_SRC)
	$(BISON) -o $(BISON_C) --defines=$(BISON_H) $(BISON_SRC)

$(GPERF_C): $(GPERF_SCR) $(BISON_H)
	$(GPERF) $(GPERF_SCR) --output-file=$(GPERF_C)

$(SRC_OBJS): src

.PHONY: src
src:
	@$(MAKE) -C $(SRC)/

# test
TEST_BIN := test_bin/
TEST_SRC := test_src

TEST_SRCS := $(wildcard $(TEST_SRC)/*.c)
TEST_OBJS := $(patsubst $(TEST_SRC)/%.c, $(TEST_BIN)%.o, $(TEST_SRCS))
TEST_BINS := $(patsubst $(TEST_SRC)/%.c, $(TEST_BIN)%, $(TEST_SRCS))

.PHONY: test
test: build $(TEST_BINS)
	@echo "Running tests..."
	@for t in $(TEST_BINS); do \
		echo "  $$t"; \
		./$$t || exit 1; \
	done

$(TEST_BINS): $(TEST_BIN)%: $(TEST_BIN)%.o
	$(CC) $(CFLAGS) -o $@ $(SRC_OBJS) $^ $(CHECK_FLAGS)

$(TEST_OBJS): test_src

.PHONY: test_src
test_src:
	@$(MAKE) -C $(TEST_SRC)/

# helpers

lint: clean
	clang-format -i */*.c $(INCLUDE)/*.h
	cppcheck */*.c $(INCLUDE)/*.h

.PHONY: clean
clean:
	@rm -f $(REPL) $(FLEX_C) $(BISON_C) $(BISON_H) $(GPERF_C) $(OBJS)
	@$(MAKE) -C $(SRC)/ clean
	@$(MAKE) -C $(TEST_SRC)/ clean