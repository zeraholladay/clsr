#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clsr.h"
#include "parser.h"
#include "repl.h"

extern FILE *yyin;
extern int yyparse(ClsrContext *ctx);
extern void yylex_destroy(void);

extern void clsr_init(ClsrContext *ctx);
extern void clsr_destroy(ClsrContext *ctx);

static Stack stack = {};
static ClsrContext ctx = {};

static void setup(void) {
  ctx.eval_ctx.stack = &stack;
  clsr_init(&ctx);
}

static void teardown(void) { clsr_destroy(&ctx); }

START_TEST(test_push) {
  const char *input = "push ()";
  yyin = fmemopen((void *)input, strlen(input), "r");

  int parse_status = yyparse(&ctx);

  ck_assert_int_eq(parse_status, 0);

  Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);

  ck_assert_ptr_eq(eval_status, obj_true);

  ck_assert_ptr_eq(POP(&stack), NULL);

  yylex_destroy();
  fclose(yyin);
}
END_TEST

START_TEST(test_push_args) {
  const char *input = "push (-1 bar 42 foo)";
  yyin = fmemopen((void *)input, strlen(input), "r");

  int parse_status = yyparse(&ctx);

  ck_assert_int_eq(parse_status, 0);

  uintptr_t sp = ctx.eval_ctx.stack->sp;

  Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);

  ck_assert_ptr_eq(eval_status, obj_true);
  ck_assert_int_eq(sp + 4, ctx.eval_ctx.stack->sp);

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

  ck_assert_ptr_eq(
      (const void *)sym_bar_literal.symbol,
      (const void *)sym_intern(&ctx.parser_ctx.sym_tab, "bar", strlen("bar")));
  ck_assert_ptr_eq(
      (const void *)sym_foo_literal.symbol,
      (const void *)sym_intern(&ctx.parser_ctx.sym_tab, "foo", strlen("foo")));

  yylex_destroy();
  fclose(yyin);
}
END_TEST

START_TEST(test_set) {
  const char *expressions[] = {
      "push (foo 42) set",
      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);
  }

  // set is a pure consumer

  Obj *obj;

  int status = env_lookup(ctx.eval_ctx.env, "foo", (void **)&obj);
  ck_assert_int_eq(status, 0);

  ck_assert(OBJ_ISKIND(obj, Obj_Literal));

  ObjLiteral obj_literal = OBJ_AS(obj, literal);

  ck_assert(obj_literal.kind == Literal_Int);
  ck_assert_int_eq(obj_literal.integer, 42);
}
END_TEST

START_TEST(test_lookup) {
  const char *expressions[] = {
      "push (foo bar) set",
      "push (foo) lookup",
      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

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

  int status = env_lookup(ctx.eval_ctx.env, "foo", (void **)&ob_ptr);

  ck_assert_int_eq(status, 0);
  ck_assert_ptr_eq(obj, ob_ptr);
}
END_TEST

START_TEST(test_ret) {
  void *val_in = (void *)0xDEADBEEF;

  PUSH(&stack, val_in);
  ENTER_FRAME(&stack);

  const char *expressions[] = {
      "push (foobar) return",
      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

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
      "closure ()()",

      "closure (foo bar) ()",

      "closure (foo bar) ("
      "  push (42)"
      "  return"
      ")",

      "push (foo42)",

      "set",

      "push (foo42)",

      "lookup",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);
  }

  // Check the last one

  Obj *obj = POP(&stack);

  ck_assert(OBJ_ISKIND(obj, Obj_Closure));
}
END_TEST

START_TEST(test_apply_with_anonymous_closure) {
  const char *expressions[] = {
      "push (42 1 3)",

      "closure (i j k) ("
      "  push (i)"
      "  lookup"
      "  return"
      ") apply",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

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

START_TEST(test_apply_with_named_closure) {
  const char *expressions[] = {
      "closure (barvar) ("
      "  push (barvar)"
      "  lookup"
      "  return"
      ") push (foo42) set",

      "push (foo42 42) lookup apply",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

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

START_TEST(test_if) {
  const char *expressions[] = {
      "push (true) if (push(42)) (push(-1))",

      "push (false) if (push(-1)) (push(42))",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);

    Obj *obj = POP(&stack);
    ck_assert(OBJ_ISKIND(obj, Obj_Literal));

    ObjLiteral int_literal = OBJ_AS(obj, literal);

    ck_assert(int_literal.kind == Literal_Int);
    ck_assert_int_eq(int_literal.integer, 42);
  }
}
END_TEST

START_TEST(test_is) {
  const char *true_expressions[] = {
      "true true is",
      "false false is",
      "push (a c b c) set set push(a) lookup push(b) lookup is",
      NULL,
  };
  const char *false_expressions[] = {
      "true false is",
      "true false is",
      "push (a c b d) set set push(a) lookup push(b) lookup is",
      "push (a 42 b 42) set set push(a) lookup push(b) lookup is",
      NULL,
  };

  for (unsigned i = 0; true_expressions[i]; ++i) {
    const char *input = true_expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);

    Obj *obj = POP(&stack);
    ck_assert(OBJ_ISKIND(obj, Obj_Literal));
    ck_assert_ptr_eq(obj, obj_true);

    ObjLiteral *obj_bool = OBJ_AS_PTR(obj, literal);
    ck_assert(obj_bool->kind == Literal_Keywrd);
  }

  for (unsigned i = 0; false_expressions[i]; ++i) {
    const char *input = false_expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);

    Obj *obj = POP(&stack);
    ck_assert(OBJ_ISKIND(obj, Obj_Literal));
    ck_assert_ptr_eq(obj, obj_false);

    ObjLiteral *obj_bool = OBJ_AS_PTR(obj, literal);
    ck_assert(obj_bool->kind == Literal_Keywrd);
  }
}
END_TEST

START_TEST(test_eq) {
  const char *true_expressions[] = {
      "true true eq",
      "false false eq",
      "push (a c b c) set set push(a) lookup push(b) lookup eq",
      "push (a 42 b 42) set set push(a) lookup push(b) lookup eq",
      NULL,
  };
  const char *false_expressions[] = {
      "true false eq",
      "true false eq",
      "push (a c b d) set set push(a) lookup push(b) lookup eq",
      NULL,
  };

  for (unsigned i = 0; true_expressions[i]; ++i) {
    const char *input = true_expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);

    Obj *obj = POP(&stack);
    ck_assert(OBJ_ISKIND(obj, Obj_Literal));
    ck_assert_ptr_eq(obj, obj_true);

    ObjLiteral *obj_bool = OBJ_AS_PTR(obj, literal);
    ck_assert(obj_bool->kind == Literal_Keywrd);
  }

  for (unsigned i = 0; false_expressions[i]; ++i) {
    const char *input = false_expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);

    Obj *obj = POP(&stack);
    ck_assert(OBJ_ISKIND(obj, Obj_Literal));
    ck_assert_ptr_eq(obj, obj_false);

    ObjLiteral *obj_bool = OBJ_AS_PTR(obj, literal);
    ck_assert(obj_bool->kind == Literal_Keywrd);
  }
}
END_TEST

START_TEST(test_math) {
  const char *expressions[] = {
      "push (0 0) add\n"
      "push(0) eq",

      "push (42 1) add\n"
      "push(42) eq",

      "push (0 1) sub\n"
      "push(-1) eq",

      "push (1 0) sub\n"
      "push(1) eq",

      "push (1 0) mul\n"
      "push(0) eq",

      "push (1 42) mul\n"
      "push(42) eq",

      "push (42 6) div\n"
      "push(7) eq",

      "push (42 1) div\n"
      "push(1) eq",

      NULL,
  };

  for (unsigned i = 0; expressions[i]; ++i) {
    const char *input = expressions[i];

    yyin = fmemopen((void *)input, strlen(input), "r");

    int parse_status = yyparse(&ctx);
    ck_assert_int_eq(parse_status, 0);

    Obj *eval_status = eval(ctx.parser_ctx.root_obj, &ctx);
    ck_assert_ptr_eq(eval_status, obj_true);

    yylex_destroy();
    fclose(yyin);
  }
}
END_TEST

Suite *eval_suite(void) {
  Suite *s = suite_create("Eval");

  TCase *tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_push);
  tcase_add_test(tc_core, test_push_args);
  tcase_add_test(tc_core, test_set);
  tcase_add_test(tc_core, test_lookup);
  tcase_add_test(tc_core, test_ret);
  tcase_add_test(tc_core, test_closure);
  tcase_add_test(tc_core, test_apply_with_anonymous_closure);
  tcase_add_test(tc_core, test_apply_with_named_closure);
  tcase_add_test(tc_core, test_if);
  tcase_add_test(tc_core, test_is);
  tcase_add_test(tc_core, test_eq);
  tcase_add_test(tc_core, test_math);

  suite_add_tcase(s, tc_core);
  return s;
}
