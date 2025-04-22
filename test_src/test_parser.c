#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "eval.h"
#include "obj.h"
#include "parser.h"

extern int yyparse(ParseContext *ctx);
extern void yy_scan_string(const char *);
extern void yylex_destroy(void);

const char *valid_expressions[] = {
    // 1. PUSH Instruction
    "PUSH a b c\n"
    "; STACK:\n"
    ";   c\n"
    ";   b\n"
    ";   a\n",

    // 2. SET Instruction
    "PUSH a 1\n"
    "; STACK:\n"
    ";   1\n"
    ";   a\n"
    "SET\n"
    "; env={ a=1 }\n",

    // 3. LOOKUP Instruction
    "PUSH a 1\n"
    "; STACK:\n"
    ";   1\n"
    ";   a\n"
    "SET\n"
    "; env={ a=1 }\n"
    "PUSH a\n"
    "; STACK:\n"
    ";   a\n"
    "LOOKUP\n"
    "; STACK:\n"
    ";   1\n",

    // 4. CLOSURE Instruction
    "PUSH foo\n"
    "CLOSURE a b c (\n"
    "  PUSH bar\n"
    "  RETURN\n"
    ")\n"
    "; STACK:\n"
    ";   #clsr-id\n"
    "PUSH foo\n"
    "; STACK:\n"
    ";   #clsr-id\n"
    ";   foo\n"
    "SET\n"
    "; env={ foo=#clsr-id }\n",

    // 5. APPLY Anonymous Closure
    "CLOSURE a b c (\n"
    "  ; returns NIL\n"
    ")\n"
    "; STACK:\n"
    ";   #clsr-id\n"
    "PUSH 1 2 3\n"
    "; STACK:\n"
    ";   3\n"
    ";   2\n"
    ";   1\n"
    ";   #clsr-id\n"
    "APPLY\n",

    // 6. APPLY Named Closure
    "PUSH foo\n"
    "CLOSURE a b c (\n"
    "  PUSH bar\n"
    "  RETURN\n"
    ")\n"
    "; STACK:\n"
    ";   #clsr-id\n"
    ";   foo\n"
    "SET\n"
    "; env={ foo=#clsr-id }\n"
    "; call foo(1,2,3)\n"
    "LOOKUP\n"
    "; STACK:\n"
    ";   3\n"
    ";   2\n"
    ";   1\n"
    ";   #clsr-id\n"
    "PUSH 1 2 3\n"
    "APPLY\n",

    NULL,
};

int run_parser_on(const char *input) {
  ParseContext ctx;

  reset_parse_context(&ctx);
  ctx.obj_pool = obj_pool_init(4096);

  yy_scan_string(input);
  yyparse(&ctx);
  yylex_destroy();
  return 0;
}

START_TEST(test_valid_valid_expressions) {
  for (unsigned i = 0; valid_expressions[i]; ++i) {
    const char *exp = valid_expressions[i];
    fprintf(stderr, "Running expressions:\n%s\n", exp);
    ck_assert_int_eq(run_parser_on(exp), 0);
  }
}
END_TEST

START_TEST(test_invalid_push_expression_nargs) {
  const char *exp = "PUSH MEANINGOFLIFETHEUNIVERSEANDEVERYTHING 42\n";
  ck_assert_int_eq(run_parser_on(exp), 0);
}
END_TEST

Suite *parser_suite(void) {
  Suite *s = suite_create("Parser");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_valid_valid_expressions);
  // tcase_add_test(tc_core, test_invalid_push_expression_nargs);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  int failed = 0;
  Suite *s = parser_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failed == 0) ? 0 : 1;
}
