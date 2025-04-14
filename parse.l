%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "parse.tab.h"
#include "op.h"
#include "debug.h"

extern int yylineno;
%}

OPCODE      [A-Za-z]+
WS          [ \t]+
EOL         \r?\n

%x ARG

%%

<INITIAL>{OPCODE} {
    DEBUG("[LEX] opcode matched: yytext = '%s'\n", yytext);
    const struct op *op = lookup_op(yytext, strlen(yytext));
    if (op) {
        BEGIN(ARG);
        yylval.op_ptr = op;
        DEBUG("[LEX] returning opcode = '%d'\n", op->op_code);
        return op->op_code;
    } else {
        DEBUG("[GPERF] opcode not found = '%s'\n", yytext);
        return ERROR;  // fallback if not found
    }
}
<INITIAL>{WS}|{WS}*{EOL} {
    ; // ignore leading whitespace or EOLs
}

<ARG>{WS} {
    ; // ignore
}
<ARG>-*[0-9]+ {
    yylval.num = atoi(yytext);
    return INT_LITERAL;
}
<ARG>{EOL} {
    BEGIN(INITIAL);
    return EOL;
}

%%
