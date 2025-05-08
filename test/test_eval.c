#include <check.h>
#include <setjmp.h>
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

jmp_buf eval_error_jmp;

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

// literals

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

// quote

START_TEST(test_quote) {
  Node *eval_result = NULL;
  Node *car = NULL;
  Node *cdr = NULL;

  eval_result = run_eval_program("'()");
  ck_assert(is_empty_list(eval_result));

  eval_result = run_eval_program("'(foo)");
  ck_assert(!is_empty_list(eval_result));
  car = get_car(eval_result);
  cdr = get_cdr(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert(is_empty_list(cdr));

  eval_result = run_eval_program("'(foo bar)");
  ck_assert(!is_empty_list(eval_result));
  car = get_car(eval_result);
  cdr = get_cdr(eval_result);
  ck_assert(!is_empty_list(cdr));
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(get_car(cdr)), "bar");
}
END_TEST

// built-ins

START_TEST(test_cons) {
  Node *eval_result = NULL;
  Node *car = NULL;
  Node *cdr = NULL;

  eval_result = run_eval_program("(cons 'foo 'bar)");
  ck_assert(!is_empty_list(eval_result));
  car = get_car(eval_result);
  cdr = get_cdr(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(cdr), "bar");

  eval_result = run_eval_program("(cons 'foo '(bar))");
  ck_assert(!is_empty_list(eval_result));
  car = get_car(eval_result);
  cdr = get_cdr(eval_result);
  ck_assert(!is_empty_list(cdr));
  ck_assert_str_eq(get_symbol(get_car(cdr)), "bar");
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

START_TEST(test_first) {
  Node *eval_result = NULL;
  // Node *car = NULL;
  // Node *cdr = NULL;

  eval_result = run_eval_program("(first '())");
  ck_assert(is_empty_list(eval_result));

  eval_result = run_eval_program("(first '(foo bar))");
  ck_assert(is_symbol(eval_result));
  ck_assert_str_eq(get_symbol(eval_result), "foo");

  if (setjmp(eval_error_jmp) == 0) {
    eval_result = run_eval_program("(first)");
    ck_assert(0);
  } else {
    ck_assert(1);
  }
}
END_TEST

START_TEST(test_rest) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(rest '())");
  ck_assert(is_empty_list(eval_result));

  eval_result = run_eval_program("(rest '(foo bar))");
  ck_assert(is_list(eval_result));
  ck_assert(is_symbol(get_car(eval_result)));
  ck_assert_str_eq(get_symbol(get_car(eval_result)), "bar");

  if (setjmp(eval_error_jmp) == 0) {
    eval_result = run_eval_program("(rest)");
    ck_assert(0);
  } else {
    ck_assert(1);
  }
}
END_TEST

START_TEST(test_len) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(len '())");
  ck_assert_int_eq(get_integer(eval_result), 0);

  eval_result = run_eval_program("(len '(a))");
  ck_assert_int_eq(get_integer(eval_result), 1);

  eval_result = run_eval_program("(len '(a b))");
  ck_assert_int_eq(get_integer(eval_result), 2);
}
END_TEST

START_TEST(test_pair) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(len (pair '() '()))");
  ck_assert_int_eq(get_integer(eval_result), 0);

  eval_result = run_eval_program("(len (pair '(a) '(1)))");
  ck_assert_int_eq(get_integer(eval_result), 1);

  eval_result = run_eval_program("(len (pair '(a b) '(1 2)))");
  ck_assert_int_eq(get_integer(eval_result), 2);

  eval_result = run_eval_program("(len (pair '(a) '(1 2)))");
  ck_assert_int_eq(get_integer(eval_result), 1);
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
  tcase_add_test(tc_core, test_first);
  tcase_add_test(tc_core, test_rest);
  tcase_add_test(tc_core, test_len);
  tcase_add_test(tc_core, test_pair);
  //(apply (closure '() '()) '())

  suite_add_tcase(s, tc_core);
  return s;
}
