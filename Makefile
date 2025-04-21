.PHONY: all src clean

# Compilers and flags
CC := gcc
CFLAGS := -Iinclude -Wall
# Add -d for flex debugging
FLEX := flex
BISON := bison
GPERF := gperf

FLEX_SRC = src/lexer.l
BISON_SRC = src/parser.y
GPERF_SCR = src/prim_op.gperf

LEX_OUT = src/lexer.c
BISON_OUT = src/parser.c
BISON_HEADER = include/parser.h
GPERF_OUT = src/prim_op.c

# GLIB := glib-2.0

OS := $(shell uname)
LIBS = -lreadline

# ifeq ($(OS), Darwin)
# 	LIBS := $(shell pkg-config --libs $(GLIB)) -ll
# else
#     LIBS = -lfl
# endif

REPL = bin/repl

all: $(REPL)

$(REPL): $(BISON_OUT) $(LEX_OUT) $(GPERF_OUT) src
	$(CC) $(CFLAGS) -o $(REPL) bin/*.o $(FLEX_LIB) $(LIBS)

$(LEX_OUT): $(FLEX_SRC) $(BISON_HEADER)
	$(FLEX) -o $(LEX_OUT) $(FLEX_SRC)

$(BISON_OUT): $(BISON_SRC)
	$(BISON) -o $(BISON_OUT) --defines=$(BISON_HEADER) $(BISON_SRC)

$(GPERF_OUT): $(GPERF_SCR) $(BISON_HEADER)
	$(GPERF) $(GPERF_SCR) --output-file=$(GPERF_OUT)

src:
	@$(MAKE) -C src/

lint: clean
	clang-format -i src/*.c include/*.h
	cppcheck src/*.c include/*.h

clean:
	@rm -f $(REPL) $(LEX_OUT) $(BISON_OUT) $(BISON_HEADER) $(GPERF_OUT) $(OBJS)
	@$(MAKE) -C src/ clean
