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

  ck_assert_ptr_eq(eval_status, TRUE);

  ck_assert_ptr_eq(POP(&stack), NULL);

  yylex_destroy();
  fclose(yyin);
}
END_TEST

START_TEST(test_push_args) {
  const char *input = "PUSH -1 bar 42 foo\n";
  yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

  int parse_status = yyparse(&parser_ctx);

  ck_assert_int_eq(parse_status, 0);

  uintptr_t sp = eval_ctx.stack->sp;

  Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);

  ck_assert_ptr_eq(eval_status, TRUE);
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

START_TEST(test_set) {
  const char *expressions[] = {
      "PUSH foo 42\n",
      "SET\n",
      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  // SET is a pure consumer

  Obj *obj;

  int status = env_lookup(eval_ctx.env, "foo", (void **)&obj);
  ck_assert_int_eq(status, 0);

  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral obj_literal = OBJ_AS(obj, literal);

  ck_assert(obj_literal.kind == Literal_Int);
  ck_assert_int_eq(obj_literal.integer, 42);
}
END_TEST

START_TEST(test_lookup) {
  const char *expressions[] = {
      "PUSH foo bar\n", "SET\n", "PUSH foo\n", "LOOKUP\n", NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  // symbol value is the same

  Obj *obj = POP(&stack);

  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral obj_literal = OBJ_AS(obj, literal);
  ck_assert(obj_literal.kind == Literal_Sym);
  ck_assert_str_eq(obj_literal.symbol, "bar");

  // same address

  Obj *ob_ptr;

  int status = env_lookup(eval_ctx.env, "foo", (void **)&ob_ptr);

  ck_assert_int_eq(status, 0);
  ck_assert_ptr_eq(obj, ob_ptr);
}
END_TEST

START_TEST(test_ret) {
  void *val_in = (void *)0xDEADBEEF;

  PUSH(&stack, val_in);
  ENTER_FRAME(&stack);

  const char *expressions[] = {
      "PUSH foobar\n",
      "RETURN\n",
      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  Obj *obj = POP(&stack);

  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral obj_literal = OBJ_AS(obj, literal);
  ck_assert(obj_literal.kind == Literal_Sym);
  ck_assert_str_eq(obj_literal.symbol, "foobar");

  ck_assert_ptr_eq((void *)POP(&stack), val_in);
}
END_TEST

START_TEST(test_closure) {
  const char *expressions[] = {
      "CLOSURE ()\n",

      "CLOSURE foo bar ()\n",

      "CLOSURE foo bar (\n"
      "  PUSH 42\n"
      "  RETURN\n"
      ")\n",

      "PUSH foo42\n",

      "SET\n",

      "PUSH foo42\n",

      "LOOKUP\n",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  // Check the last one

  Obj *obj = POP(&stack);

  ck_assert(OBJ_ISKIND(obj, Obj_Closure));
}
END_TEST

START_TEST(test_anonymous_closure_apply) {
  const char *expressions[] = {
      "PUSH 42 1 3\n",

      "CLOSURE i j k(\n"
      "  PUSH i\n"
      "  LOOKUP\n"
      "  RETURN ; return 42\n"
      ")\n",

      "APPLY\n",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  Obj *obj = POP(&stack);
  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral int_literal = OBJ_AS(obj, literal);

  ck_assert(int_literal.kind == Literal_Int);
  ck_assert_int_eq(int_literal.integer, 42);
}
END_TEST

START_TEST(test_named_closure_apply) {
  const char *expressions[] = {
      "CLOSURE barvar (\n"
      "  PUSH barvar\n"
      "  LOOKUP\n"
      "  RETURN\n"
      ")\n",

      "PUSH foo42\n",

      "SET\n",

      "PUSH 42\n",

      "PUSH foo42\n",

      "LOOKUP\n",

      "APPLY\n",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

    int parse_status = yyparse(&parser_ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(parser_ctx.root_obj, &eval_ctx);
    ck_assert_ptr_eq(eval_status, TRUE);

    yylex_destroy();
    fclose(yyin);
  }

  Obj *obj = POP(&stack);
  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral int_literal = OBJ_AS(obj, literal);

  ck_assert(int_literal.kind == Literal_Int);
  ck_assert_int_eq(int_literal.integer, 42);
}
END_TEST

Suite *eval_suite(void) {
  Suite *s = suite_create("Eval");

  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, eval_setup, eval_teardown);

  tcase_add_test(tc_core, test_push);
  tcase_add_test(tc_core, test_push_args);
  tcase_add_test(tc_core, test_set);
  tcase_add_test(tc_core, test_lookup);
  tcase_add_test(tc_core, test_ret);
  tcase_add_test(tc_core, test_closure);
  tcase_add_test(tc_core, test_anonymous_closure_apply);
  tcase_add_test(tc_core, test_named_closure_apply);

  suite_add_tcase(s, tc_core);
  return s;
}
