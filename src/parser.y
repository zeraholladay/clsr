// clang-format off
%{
#include <stdio.h>

#include "eval.h"
#include "parser.h"
#include "types.h"

#define yyerror(ctx, s)                                                       \
  do                                                                          \
    {                                                                         \
      yyerror_handler (ctx, s);                                               \
      YYABORT;                                                                \
    }                                                                         \
  while (0)

int yylex (Context * ctx);
void yyerror_handler (Context * ctx, const char *s);

extern int yylineno;
// clang-format off
%}

%code requires
{
#include "types.h"

void reset_parse_context(Context *ctx);
}

%lex-param
{
Context *ctx
}

%parse-param
{
Context *ctx
}

%union
{
  Integer integer;
  const char *symbol;
  const struct Keyword *keyword;
  struct Node *node;
}

%type <node> program exprs expr list_expr list_form literal_expr
%type <node> lambda_form lambda_param_form // special form
%type <node> if_form  // special form

%token NIL_TOKEN T_TOKEN
%token ERROR LAMBDA QUOTE
%token <keyword> IF PRIMITIVE
%token <integer> INTEGER
%token <symbol>  SYMBOL

%%

program
  : exprs
    {
      CTX_PARSE_ROOT (ctx) = $1;
      YYACCEPT;
    }
  | exprs error
    {
      CTX_PARSE_ROOT (ctx) = NIL;
      yyerror (ctx, "Parse error\n");
      YYABORT;
    }
  ;

exprs
  : /* empty */
    {
      $$ = NIL;
    }
  | expr exprs
    {
      $$ = CONS ($1, $2, ctx);
    }
  ;

expr
  : list_expr
  | literal_expr
  | QUOTE expr
    {
      Node *quote = cons_prim (&CTX_POOL (ctx), KEYWORD (QUOTE));
      $$ = LIST2 (quote, $2, ctx);
    }
  ;

list_expr
  : '(' ')'
    {
      $$ = NIL;
    }
  | '(' list_form ')'
    {
      $$ = $2;
    }
  | '(' lambda_form ')'
    {
      $$ = LIST1 ($2, ctx);
    }
  | '(' if_form ')'
    {
      $$ = LIST1 ($2, ctx);
    }
  ;

literal_expr
  : NIL_TOKEN
    {
      $$ = NIL;
    }
  | T_TOKEN
    {
      $$ = T;
    }
  | SYMBOL
    {
      $$ = cons_symbol (&CTX_POOL (ctx), $1);
    }
  | PRIMITIVE
    {
      $$ = cons_prim (&CTX_POOL (ctx), $1);
    }
  | INTEGER
    {
      $$ = cons_integer (&CTX_POOL (ctx), $1);
    }
  ;

list_form
  : expr
    {
      $$ = LIST1 ($1, ctx);
    }
  | expr list_form
    {
      $$ = CONS ($1, $2, ctx);
    }
  ;

lambda_form
  : LAMBDA '(' lambda_param_form ')' exprs
    {
      $$ = cons_lambda (&CTX_POOL (ctx), $3, $5, CTX_ENV (ctx));
    }
  ;

lambda_param_form
    : /* empty */ {
        $$ = NIL;
    }
    | SYMBOL lambda_param_form {
        Node *sym_node = cons_symbol( &CTX_POOL (ctx), $1);
        $$ = CONS (sym_node, $2, ctx);
    }
  ;

if_form
  : IF expr expr expr
    {
      Node *if_symbol = cons_prim (&CTX_POOL (ctx), $1);
      Node *expr = CONS ( $2, LIST2 ($3, $4, ctx), ctx);
      $$ = CONS (if_symbol, expr, ctx);
    }
  | IF expr expr
    {
      Node *if_symbol = cons_prim (&CTX_POOL (ctx), $1);
      Node *expr = CONS ( $2, LIST1 ($3, ctx), ctx);
      $$ = CONS (if_symbol, expr, ctx);
    }
  ;

%%
    // clang-format on

    void
    reset_parse_context (Context *ctx)
{
  /* assumes pool has already been allocated. */
  CTX_PARSE_ROOT (ctx) = NIL;
}

void
yyerror_handler (Context *ctx, const char *s)
{
  fprintf (stderr, "Syntax error: line %d: %s\n", yylineno, s);
  reset_parse_context (ctx);
}

// clang-format off
