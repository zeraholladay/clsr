%option noyywrap
%option yylineno
%option noinput
%option nounput

%{
#include "core_def.h"
#include "parser.h"
#include "sym_save.h"

#define YY_DECL int yylex(Context *ctx)
%}

NL       \r?\n
WS       [ \t]+
NUM      -?[0-9]+
ID       [a-zA-Z_][a-zA-Z0-9_]*

%%

<INITIAL>{
    "'"             { return QUOTE; }
    "("             { return '('; }
    ")"             { return ')'; }
    {NUM}           { yylval.integer = atoll(yytext); return INTEGER; }
    {ID}            {
                    const PrimOp *prim_op = prim_op_lookup(yytext, strlen(yytext));
                    if (prim_op) {
                        yylval.prim_op = prim_op;
                        return PRIM_OP;
                    }
                    yylval.symbol = sym_save(&CTX_SYMTAB(ctx), yytext, yyleng);
                    return SYMBOL;
                    }
    {WS}            { /* ignore */ }
    {NL}            { /* ignore */ }
    ;[^\n]*         { /* ignore */ }
    .               { return ERROR; }
}

%%
