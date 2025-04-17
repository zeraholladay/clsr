%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <stdlib.h>

#include "parse.tab.h"
#include "op.h"
#include "debug.h"
#include "str_intern.h"

extern int yylineno;
%}

OP (PUSH|LOOKUP|CLOSURE|APPLY|RETURN)
WS [ \t]+
NL \r?\n

%x ARG

%%

<INITIAL>{OP} {
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
<INITIAL>{WS}|{WS}*{NL} {
    ; // ignore leading whitespace or NL
}

<ARG>{WS} {
    ; // ignore
}
<ARG>-*[0-9]+ {
    yylval.num = atoi(yytext);
    return INT_LITERAL;
}
<ARG>[a-zA-Z][a-zA-Z0-9]* {
    // yylval.str = strndup(yytext, yyleng);  //TO DO: Use symbol table
    yylval.str = str_intern(yytext, yyleng);
    return SYM_LITERAL;
}
<ARG>{NL} {
    BEGIN(INITIAL);
    return NL;
}

%%
