#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clsr.h"
#include "parser.h"
#include "repl.h"

extern FILE *yyin;
extern int yyparse(ParseContext *ctx);
extern void yylex_destroy(void);

ParseContext parser_ctx = {};
EvalContext eval_ctx = {};
Stack stack = {};

void eval_setup(void) {
  reset_parse_context(&parser_ctx);
  parser_ctx.obj_pool = obj_pool_init(4096);

  STACK_INIT(&stack);
  eval_ctx.stack = &stack;
  eval_ctx.env = env_new(NULL);
}

void eval_teardown(void) {
  obj_pool_destroy(&(parser_ctx.obj_pool));

  stack_free(&stack);
  eval_ctx.stack = NULL;
  FREE(eval_ctx.env);
}

START_TEST(test_push) {
  const char *input = "PUSH\n";
  yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

  int parse_status = yyparse(&parser_ctx);

  ck_assert_int_eq(parse_status, 0);

  Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);

  ck_assert_ptr_eq(eval_status, NULL);

  ck_assert_ptr_eq(POP(&stack), NULL);

  yylex_destroy();
  fclose(yyin);
}
END_TEST

START_TEST(test_push_args) {
  const char *input = "PUSH foo 42 bar -1\n";
  yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

  int parse_status = yyparse(&parser_ctx);

  ck_assert_int_eq(parse_status, 0);

  uintptr_t sp = eval_ctx.stack->sp;

  Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);

  ck_assert_ptr_eq(eval_status, NULL);
  ck_assert_int_eq(sp + 4, eval_ctx.stack->sp);

  Obj *neg_int = POP(&stack);
  Obj *sym_bar = POP(&stack);
  Obj *pos_int = POP(&stack);
  Obj *sym_foo = POP(&stack);

  ck_assert(OBJ_ISKIND(neg_int, Obj_Literal));
  ck_assert(OBJ_ISKIND(sym_bar, Obj_Literal));
  ck_assert(OBJ_ISKIND(pos_int, Obj_Literal));
  ck_assert(OBJ_ISKIND(sym_foo, Obj_Literal));

  ObjLiteral neg_int_literal = OBJ_AS(neg_int, literal);
  ObjLiteral sym_bar_literal = OBJ_AS(sym_bar, literal);
  ObjLiteral pos_int_literal = OBJ_AS(pos_int, literal);
  ObjLiteral sym_foo_literal = OBJ_AS(sym_foo, literal);

  ck_assert(neg_int_literal.kind == Literal_Int);
  ck_assert(sym_bar_literal.kind == Literal_Sym);
  ck_assert(pos_int_literal.kind == Literal_Int);
  ck_assert(sym_foo_literal.kind == Literal_Sym);

  ck_assert_int_eq(neg_int_literal.integer, -1);
  ck_assert_str_eq(sym_bar_literal.symbol, "bar");
  ck_assert_int_eq(pos_int_literal.integer, 42);
  ck_assert_str_eq(sym_foo_literal.symbol, "foo");

  ck_assert_ptr_eq((const void *)sym_bar_literal.symbol,
                   (const void *)str_intern("bar", strlen("bar")));
  ck_assert_ptr_eq((const void *)sym_foo_literal.symbol,
                   (const void *)str_intern("foo", strlen("foo")));

  yylex_destroy();
  fclose(yyin);
}
END_TEST

Suite *eval_suite(void) {
  Suite *s = suite_create("Eval");

  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, eval_setup, eval_teardown);

  tcase_add_test(tc_core, test_push);
  tcase_add_test(tc_core, test_push_args);

  suite_add_tcase(s, tc_core);
  return s;
}
