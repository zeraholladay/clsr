%option noyywrap
%option yylineno
%option noinput
%option nounput

%{
#include <stdlib.h>

#include "eval_ctx.h"
#include "parser.h"
#include "keywords.h"
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
                    const Keyword *keyword = keyword_lookup(yytext, yyleng);
                    if (keyword) {
                        yylval.keyword = keyword;
                        return (keyword != KEYWORD(LAMBDA)) ? PRIMITIVE : LAMBDA;
                    }
                    yylval.symbol = sym_save(&CTX_SYMTAB(ctx), yytext, yyleng);
                    return SYMBOL;
                    }
    [+\-*/]         {
                    int op = yytext[0];
                    switch (op) {
                        case '+':
                            yylval.keyword = KEYWORD(ADD);
                            break;
                        case '-':
                            yylval.keyword = KEYWORD(SUB);
                            break;
                        case '*':
                            yylval.keyword = KEYWORD(MUL);
                            break;
                        case '/':
                            yylval.keyword = KEYWORD(DIV);
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
