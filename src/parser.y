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
%type <node> lambda_form lambda_param_form
%type <node> if_form

%token ERROR                          // error state
%token NIL_TOKEN                      // NIL singleton
%token T_TOKEN                        // T singleton
%token LAMBDA_PRIMITIVE               // Lambda type and special form
%token <keyword> IF_PRIMITIVE         // Symbol and special form
%token <keyword> QUOTE_PRIMITIVE      // Symbol and special form
%token <keyword> INTRINSIC_PRIMITIVE  // Primitive types (callable)
%token <keyword> EVAL_PRIMITIVE       // Special Code for Eval
%token <integer> INTEGER              // Integers
%token <symbol>  SYMBOL               // Symbols

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
  | QUOTE_PRIMITIVE expr
    {
      Node *quote = cons_prim (&CTX_POOL (ctx), $1);
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
      $$ = $2;
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
  | INTRINSIC_PRIMITIVE
    {
      $$ = cons_prim (&CTX_POOL (ctx), $1);
    }
  | EVAL_PRIMITIVE
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
  : IF_PRIMITIVE expr expr expr
    {
      Node *if_symbol = cons_prim (&CTX_POOL (ctx), $1);
      Node *expr = CONS ( $2, LIST2 ($3, $4, ctx), ctx);
      $$ = CONS (if_symbol, expr, ctx);
    }
  | IF_PRIMITIVE expr expr
    {
      Node *if_symbol = cons_prim (&CTX_POOL (ctx), $1);
      Node *expr = LIST2 ($2, $3, ctx);
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
