OS := $(shell uname)

LEX = flex
YACC = bison
GPERF = gperf

LEX_SRC = parse.l
YACC_SRC = parse.y
GPERF_SCR = op.gperf

LEX_OUT = lex.yy.c
YACC_OUT = parse.tab.c
YACC_HEADER = parse.tab.h
GPERF_OUT = op.c

CFILES=mach.c str_intern.c
OBJS=$(CFILES:.c=.o)

BIN = vm

ifeq ($(OS), Darwin)
    FLEX_LIB = -ll
else
    FLEX_LIB = -lfl
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

clean:
	rm -f $(BIN) $(LEX_OUT) $(YACC_OUT) $(YACC_HEADER) $(GPERF_OUT) $(OBJS)
