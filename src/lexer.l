%option noyywrap
%option yylineno
%option noinput
%option nounput

%{
#include <stdlib.h>

#include "eval_ctx.h"
#include "parser.h"
#include "prim_fn.h"
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
                    const PrimitiveFn *prim_fn = prim_fn_lookup(yytext, yyleng);
                    if (prim_fn) {
                        yylval.prim_fn = prim_fn;
                        return PRIMITIVE;
                    }
                    yylval.symbol = sym_save(&CTX_SYMTAB(ctx), yytext, yyleng);
                    return SYMBOL;
                    }
    [+\-*/]         {
                    int op = yytext[0];
                    switch (op) {
                        case '+':
                            yylval.prim_fn = PRIM_FN(ADD);
                            break;
                        case '-':
                            yylval.prim_fn = PRIM_FN(SUB);
                            break;
                        case '*':
                            yylval.prim_fn = PRIM_FN(MUL);
                            break;
                        case '/':
                            yylval.prim_fn = PRIM_FN(DIV);
                            break;
                        default:
                            return ERROR;
                            break;
                        }
                    return PRIMITIVE;
                    }
    {WS}            { /* ignore */ }
    {NL}            { /* ignore */ }
    ;[^\n]*         { /* ignore */ }
    .               { return ERROR; }
}

%%
