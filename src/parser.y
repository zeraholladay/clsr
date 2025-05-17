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

%token ERROR
// Literal singletons
%token NIL_TOKEN
%token T_TOKEN
// Numeric and symbol literals
%token <integer> INTEGER
%token <symbol>  SYMBOL
// primitive procedures (has eval_)
%token <keyword> INTRINSIC_PRIMITIVE
// Built-in special‐forms
%token <keyword> IF_PRIMITIVE
%token <keyword> QUOTE_PRIMITIVE
%token <keyword> LAMBDA_PRIMITIVE
%token <keyword> EVAL_PRIMITIVE
%token <keyword> AND_PRIMITIVE
%token <keyword> OR_PRIMITIVE

%type <node>
  program
%type <node>
  exprs
  expr
%type <node>
  list_expr
  quote_expr
%type <node>
  list_form
  lambda_form
  lambda_param_form
  if_form
%type <node>
  if_atom
  quote_atom
  atom
%type <keyword>
  primitive_atom
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
  | quote_expr
  | atom
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
      $$ = $2;
    }
  ;

quote_expr
  : quote_atom expr
    {
      $$ = LIST2 ($1, $2, ctx);
    }
  ;

quote_atom
  : QUOTE_PRIMITIVE
    {
      $$ = cons_prim (&CTX_POOL (ctx), $1);
    }
  ;

atom
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
  | INTEGER
    {
      $$ = cons_integer (&CTX_POOL (ctx), $1);
    }
  | primitive_atom 
    {
      $$ = cons_prim (&CTX_POOL (ctx), $1);
    }
  ;

primitive_atom
  : INTRINSIC_PRIMITIVE
  | EVAL_PRIMITIVE
  | AND_PRIMITIVE
  | OR_PRIMITIVE
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
  : LAMBDA_PRIMITIVE '(' lambda_param_form ')' exprs
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
  : if_atom expr expr expr
    {
      Node *expr = CONS ( $2, LIST2 ($3, $4, ctx), ctx);
      $$ = CONS ($1, expr, ctx);
    }
  | if_atom expr expr
    {
      Node *expr = LIST2 ($2, $3, ctx);
      $$ = CONS ($1, expr, ctx);
    }
  ;

if_atom
  : IF_PRIMITIVE
    {
      $$ = cons_prim (&CTX_POOL (ctx), $1);
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
