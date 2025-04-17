OS:=$(shell uname)

LEX=flex
YACC=bison
GPERF=gperf

LEX_SRC=parser.l
YACC_SRC=parser.y
GPERF_SCR=operator.gperf

LEX_OUT=lex.yy.c
YACC_OUT=parser.tab.c
YACC_HEADER=parser.tab.h
GPERF_OUT=operator.c

CFILES=mach.c sym_intern.c object.c stack.c environ.c
OBJS=$(CFILES:.c=.o)

BIN=vm

ifeq ($(OS), Darwin)
    FLEX_LIB=-ll
else
    FLEX_LIB=-lfl
endif

all: $(BIN)

$(BIN): $(YACC_OUT) $(LEX_OUT) $(GPERF_OUT) $(OBJS)
	$(CC) -o $(BIN) $(YACC_OUT) $(LEX_OUT) $(GPERF_OUT) $(OBJS) $(FLEX_LIB)

$(LEX_OUT): $(LEX_SRC) $(YACC_HEADER)
	$(LEX) $(LEX_SRC)

$(YACC_OUT): $(YACC_SRC)
	$(YACC) -d $(YACC_SRC)

$(GPERF_OUT): $(GPERF_SCR) $(YACC_HEADER)
	$(GPERF) $(GPERF_SCR) > $(GPERF_OUT)

.c.o:
	$(CC) $(CFLAGS) -c $<

lint: clean
	clang-format -i $(CFILES) *.h
	cppcheck $(CFILES) *.h

clean:
	rm -f $(BIN) $(LEX_OUT) $(YACC_OUT) $(YACC_HEADER) $(GPERF_OUT) $(OBJS)
