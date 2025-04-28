%option noyywrap
%option yylineno
%option noinput

%{
#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"
#include "parser.h"
#include "common.h"

#define YY_DECL int yylex(ParseContext *ctx)

%}

COMMENT ;[^\n]*

OP (APPLY|CLOSURE|LOOKUP|PUSH|RETURN|SET)
WS [ \t]+
NL \r?\n

%x ARGS END_CLOSURE

%%

<INITIAL>{
    {WS}*{NL} {
        if (ctx->lexer_state.parens == 0) return '\n';
    }
    {OP} {
        const PrimOp *p = prim_op_lookup(yytext, strlen(yytext));
        if (p) {
            yylval.prim = p;
            BEGIN(ARGS);
            return p->tok;
        } else {
            // TODO: Handle error
            return 1;
        }
    }
    \) {
        ctx->lexer_state.parens--;
        BEGIN(END_CLOSURE);
        return ')';
    }
}

<END_CLOSURE>{
    {NL} {
        BEGIN(INITIAL);
        return '\n';
    }
}

<ARGS>{
    \( {
        ctx->lexer_state.parens++;
        BEGIN(INITIAL);
        return '(';
    }
    {NL} {
        BEGIN(INITIAL);
        return '\n';
    }
    -*[0-9]+ {
        yylval.num = atoi(yytext);
        return TOK_INT_LITERAL;
    }
    [a-zA-Z][a-zA-Z0-9]* {
        yylval.sym = str_intern(yytext, yyleng);
        return TOK_SYM_LITERAL;
    }
}

<*>{
    {COMMENT}|{WS} {
        /* ignore comments or whitespace */
    }
    <<EOF>> {
        if (!ctx->lexer_state.eof && (ctx->lexer_state.eof = 1)) {
            unput('\n');
            BEGIN(INITIAL);  // Give BEGIN a \n once
        } else
            return 0;  // Otherwise, terminate parsing.
    }
    . {
        return TOK_ERROR;
    }
}

%%
