# src

# Compilers and flags
CC ?= gcc
CFLAGS := -Iinclude -Wall -Wextra -O2
LIBS :=
INCLUDE := include

SRC := src

# Add -d for debugging
FLEX := flex #-d
BISON := bison #-d -v --debug
GPERF := gperf

FLEX_SRC := $(SRC)/lexer.l
BISON_SRC := $(SRC)/parser.y
GPERF_SCR := $(SRC)/prim_op.gperf

MAIN_C := $(SRC)/main.c
FLEX_C := $(SRC)/lexer.c
BISON_C := $(SRC)/parser.c
BISON_H := $(INCLUDE)/parser.h
GPERF_C := $(SRC)/prim_op.c

BIN := bin/

SRC_CFILES := $(wildcard $(SRC)/*.c)
SRC_CFILES += $(FLEX_C) $(BISON_C) $(GPERF_C)
SRC_CFILES_ALL := $(sort $(SRC_CFILES))
SRC_OBJS := $(patsubst $(SRC)/%.c, $(BIN)%.o, $(SRC_CFILES_ALL))

# dependencies
CHECK := check

OS := $(shell uname)

ifeq ($(OS), Darwin)
	TEST_FLAGS := $(shell pkg-config --cflags $(CHECK))
	TEST_LIBS := $(shell pkg-config --libs $(CHECK))
endif

REPL = bin/repl

.PHONY: all
all: src $(REPL)

$(FLEX_C): $(FLEX_SRC) $(BISON_H)
	$(FLEX) -o $(FLEX_C) $(FLEX_SRC)

$(BISON_C): $(BISON_SRC)
	$(BISON) -o $(BISON_C) --defines=$(BISON_H) $(BISON_SRC)

$(GPERF_C): $(GPERF_SCR) $(BISON_H)
	$(GPERF) $(GPERF_SCR) --output-file=$(GPERF_C)

src: $(BISON_C) $(FLEX_C) $(GPERF_C) $(SRC_OBJS)

$(BIN)%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(REPL): src/repl.c
	$(CC) $(CFLAGS) -DREPL_MAIN=1 -o $@ $(SRC_OBJS) $^ $(LIBS)

# test
TEST_BIN := test_bin/
TEST_SRC := test

TEST_CFILES := $(wildcard $(TEST_SRC)/*.c)
TEST_OBJS := $(patsubst $(TEST_SRC)/%.c, $(TEST_BIN)%.o, $(TEST_CFILES))
TEST_BINS := $(patsubst $(TEST_SRC)/%.c, $(TEST_BIN)%, $(TEST_CFILES))

.PHONY: test
test: src $(TEST_BINS)
	@echo "Running tests..."
	@for t in $(TEST_BINS); do \
		echo "  $$t"; \
		./$$t || exit 1; \
	done

$(TEST_BINS):$(TEST_CFILES)
	$(CC) $(CFLAGS) -o $@ $(SRC_OBJS) $^ $(TEST_LIBS) $(TEST_FLAGS)

# linting

lint: clean
	clang-format -i */*.c $(INCLUDE)/*.h
	cppcheck */*.c $(INCLUDE)/*.h

# clean

.PHONY: clean
clean:
	rm -f $(REPL) $(FLEX_C) $(BISON_C) $(BISON_H) $(GPERF_C) $(BIN)* $(TEST_BINS)