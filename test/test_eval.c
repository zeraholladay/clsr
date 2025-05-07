#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_def.h"
#include "eval.h"
#include "parser.h"
#include "repl.h"

extern FILE *yyin;
extern int yyparse(Context *ctx);
extern void yylex_destroy(void);

extern void clsr_init(Context *ctx);
extern void clsr_destroy(Context *ctx);

static Stack stack = {};
static Context ctx = {};

static void setup(void) {
  CTX_STACK(&ctx) = &stack;
  clsr_init(&ctx);
}

static void teardown(void) { clsr_destroy(&ctx); }

static Node *run_eval_program(const char *input) {
  yyin = fmemopen((void *)input, strlen(input), "r");

  int parse_status = yyparse(&ctx);
  ck_assert_int_eq(parse_status, 0);

  Node *program = CTX_PARSE_ROOT(&ctx);
  Node *eval_result = eval_program(program, &ctx);

  yylex_destroy();
  fclose(yyin);

  return eval_result;
}

START_TEST(test_literal_expressions) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("42");
  ck_assert_int_eq(get_integer(eval_result), 42);

  eval_result = run_eval_program("-42");
  ck_assert_int_eq(get_integer(eval_result), -42);

  eval_result = run_eval_program("'foo");
  ck_assert_str_eq(get_symbol(eval_result), "foo");
}
END_TEST

START_TEST(test_quote) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("'()");
  ck_assert(is_empty_list(eval_result));
}
END_TEST

START_TEST(test_cons) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(cons 'foo 'bar)");
  ck_assert(!is_empty_list(eval_result));
  Node *car = get_car(eval_result);
  Node *cdr = get_cdr(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(cdr), "bar");
}
END_TEST

START_TEST(test_first) {
  // Node *eval_result = NULL;

  // eval_result = run_eval_program("'()");
  // ck_assert_int_eq(is_empty_list(eval_result), 1);
}
END_TEST

START_TEST(test_rest) {
  // Node *eval_result = NULL;

  // eval_result = run_eval_program("'()");
  // ck_assert_int_eq(is_empty_list(eval_result), 1);
}
END_TEST

START_TEST(test_set_and_lookup) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(set 'foo 42)");
  ck_assert_int_eq(get_integer(eval_result), 42);
  eval_result = run_eval_program("foo");
  ck_assert_int_eq(get_integer(eval_result), 42);
}
END_TEST

Suite *eval_suite(void) {
  Suite *s = suite_create("Eval");

  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_literal_expressions);
  tcase_add_test(tc_core, test_quote);
  tcase_add_test(tc_core, test_cons);
  tcase_add_test(tc_core, test_set_and_lookup);
  //(apply (closure '() '()) '())

  suite_add_tcase(s, tc_core);
  return s;
}
