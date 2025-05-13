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

static Context ctx = {};

jmp_buf eval_error_jmp;

static void setup(void) { clsr_init(&ctx); }

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
  ck_assert(get_integer(eval_result) == 42);

  eval_result = run_eval_program("-42");
  ck_assert(get_integer(eval_result) == -42);

  eval_result = run_eval_program("T");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("NIL");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("'foo");
  ck_assert_str_eq(get_symbol(eval_result), "foo");
}
END_TEST

// quote

START_TEST(test_quote) {
  Node *eval_result = NULL;
  Node *car = NULL;
  Node *cdr = NULL;

  // NULL program
  eval_result = run_eval_program("()");
  ck_assert(!eval_result);

  eval_result = run_eval_program("'()");
  ck_assert(is_empty_list(eval_result));

  eval_result = run_eval_program("'(foo)");
  ck_assert(!is_empty_list(eval_result));
  car = FIRST(eval_result);
  cdr = REST(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert(is_empty_list(cdr));

  eval_result = run_eval_program("'(foo bar)");
  ck_assert(!is_empty_list(eval_result));
  car = FIRST(eval_result);
  cdr = REST(eval_result);
  ck_assert(!is_empty_list(cdr));
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(FIRST(cdr)), "bar");
}
END_TEST

// built-ins

START_TEST(test_cons) {
  Node *eval_result = NULL;
  Node *car = NULL;
  Node *cdr = NULL;

  eval_result = run_eval_program("(cons 'foo 'bar)");
  ck_assert(!is_empty_list(eval_result));
  car = FIRST(eval_result);
  cdr = REST(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(cdr), "bar");

  eval_result = run_eval_program("(cons 'foo '(bar))");
  ck_assert(!is_empty_list(eval_result));
  car = FIRST(eval_result);
  cdr = REST(eval_result);
  ck_assert(!is_empty_list(cdr));
  ck_assert_str_eq(get_symbol(FIRST(cdr)), "bar");
}
END_TEST

START_TEST(test_set_and_lookup) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(set 'foo 42)");
  ck_assert(get_integer(eval_result) == 42);

  eval_result = run_eval_program("foo");
  ck_assert(get_integer(eval_result) == 42);

  eval_result = run_eval_program("(set 'bar 'foo)");
  ck_assert(is_symbol(eval_result));

  eval_result = run_eval_program("(set 'bar '(1 2 3))");
  ck_assert(is_list(eval_result));

  eval_result = run_eval_program("(set 'bar (closure '() '()))");
  ck_assert(is_closure_fn(eval_result));
}
END_TEST

START_TEST(test_first) {
  Node *eval_result = NULL;

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
  ck_assert(is_symbol(FIRST(eval_result)));
  ck_assert_str_eq(get_symbol(FIRST(eval_result)), "bar");

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
  ck_assert(get_integer(eval_result) == 0);

  eval_result = run_eval_program("(len '(a))");
  ck_assert(get_integer(eval_result) == 1);

  eval_result = run_eval_program("(len '(a b))");
  ck_assert(get_integer(eval_result) == 2);
}
END_TEST

START_TEST(test_pair) {
  Node *eval_result = NULL;

  eval_result = run_eval_program("(pair '() '())");
  ck_assert(get_integer(eval_result) == 0);

  eval_result = run_eval_program("(len '(a))");
  ck_assert(get_integer(eval_result) == 1);

  eval_result = run_eval_program("(len '(a b))");
  ck_assert(get_integer(eval_result) == 2);
}
END_TEST

START_TEST(test_closure) {
  Node *eval_result = NULL;

  // define
  eval_result = run_eval_program("(closure '() '())");
  ck_assert(is_closure_fn(eval_result));

  // run
  eval_result = run_eval_program("((closure '() '()))");
  ck_assert(is_empty_list(eval_result));

  // run body with a=42
  eval_result = run_eval_program("((closure '(a) 'a) 42)");
  ck_assert(is_integer(eval_result) && get_integer(eval_result) == 42);

  // define 'foo and run
  eval_result =
      run_eval_program("(set 'foo (closure '() '(cons 'a 'b))) (foo)");
  ck_assert(is_list(eval_result));

  eval_result = run_eval_program(
      "(set 'foo (closure '(a b) '(cons a b))) (foo 'bar 'biz)");
  ck_assert(is_list(eval_result));
  ck_assert_str_eq(get_symbol(FIRST(eval_result)), "bar");
}
END_TEST

START_TEST(test_eq) {
  Node *eval_result = NULL;
  char *test_program;

  // True statements
  eval_result = run_eval_program("(eq T T)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq NIL NIL)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq 0 0)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq 42 42)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq '() '())");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq 'foo 'foo)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  test_program = "(set 'foo (closure '() '()))"
                 "(set 'bar foo)"
                 "(eq foo bar)";
  eval_result = run_eval_program(test_program);
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  test_program = "(set 'foo '(1 2 3 4))"
                 "(set 'bar foo)"
                 "(eq foo bar)";
  eval_result = run_eval_program(test_program);
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq (str 'foo) (str 'foo))");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  eval_result = run_eval_program("(eq rest rest)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(T));

  // False statements
  eval_result = run_eval_program("(eq T NIL)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq NIL T)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq 0 1)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq -42 42)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq '() '(1))");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq 'foo 'bar)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  test_program = "(set 'foo (closure '() '()))"
                 "(set 'bar (closure '() '()))"
                 "(eq foo bar)";
  eval_result = run_eval_program(test_program);
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  test_program = "(set 'foo '(1 2 3 4))"
                 "(set 'bar '(1 2 3 4))"
                 "(eq foo bar)";
  eval_result = run_eval_program(test_program);
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq (str 'foo) (str 'bar))");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));

  eval_result = run_eval_program("(eq first rest)");
  ck_assert_ptr_eq(get_prim_op(eval_result), PRIMITIVE(NIL));
}
END_TEST

START_TEST(test_apply) {
  Node *eval_result = NULL;
  Node *car = NULL;
  Node *cdr = NULL;

  eval_result = run_eval_program("(apply (closure '() '()))");
  ck_assert(is_empty_list(eval_result));

  eval_result =
      run_eval_program("(apply (closure '(a b) '(cons a b)) 'foo 'bar)");
  ck_assert(!is_empty_list(eval_result));
  car = FIRST(eval_result);
  cdr = REST(eval_result);
  ck_assert_str_eq(get_symbol(car), "foo");
  ck_assert_str_eq(get_symbol(cdr), "bar");

  eval_result = run_eval_program("(apply set 'a 42)");
  ck_assert(get_integer(eval_result) == 42);

  eval_result = run_eval_program("(apply set 'a 42)");
  ck_assert(get_integer(eval_result) == 42);
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
  tcase_add_test(tc_core, test_closure);
  tcase_add_test(tc_core, test_eq);
  tcase_add_test(tc_core, test_apply);

  suite_add_tcase(s, tc_core);
  return s;
}
