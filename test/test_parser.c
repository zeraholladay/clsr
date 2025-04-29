#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clsr.h"
#include "common.h"
#include "parser.h"

extern FILE *yyin;
extern int yyparse(ParseContext *ctx);
extern void yylex_destroy(void);

const char *valid_expressions[] = {
    // // Newlines
    // "\n",

    // "\n\n",

    // // Comments
    // ";",
    // ";\n",
    // "; 42\n",
    // ";;\n",

    // PUSH Instructions
    "PUSH ()\n",

    "PUSH (42 life)\n",

    "PUSH (a b c)\n",

    // SET Instruction
    "SET\n",

    "PUSH (a 1)\n"
    "; STACK:\n"
    ";   a   (symbol)\n"
    ";   1   (value)\n"
    "SET\n"
    "; env={ a=1 }\n",

    // LOOKUP Instruction
    "LOOKUP\n",

    "PUSH (a 1)\n"
    "; STACK:\n"
    ";   a   (symbol)\n"
    ";   1   (value)\n"
    "SET\n"
    "; env={ a=1 }\n"
    "PUSH (a)\n"
    "; STACK:\n"
    ";   a   (symbol)\n"
    "LOOKUP\n"
    "; STACK:\n"
    ";   1   (value found)\n",

    // CLOSURE Instructions
    "CLOSURE ()()\n",

    "CLOSURE (a b c) ()\n",

    "CLOSURE (\n"
    ")()\n",

    "CLOSURE () (\n"
    "   RETURN\n"
    ")\n",

    "CLOSURE (a b c) (\n"
    "  PUSH (bar)\n"
    "  RETURN\n"
    ")\n"
    "; STACK:\n"
    ";   (closure)\n"
    "PUSH (foo)\n"
    "; STACK:\n"
    ";   foo   (symbol)\n"
    ";   (closure)\n"
    "SET\n"
    "; env={ foo=closure }\n",

    // APPLY Anonymous Closure
    "PUSH (1 2 3)\n"
    "CLOSURE (a b c) (\n"
    "  ; returns NIL\n"
    ")\n"
    "; STACK:\n"
    ";   (closure)\n"
    ";   1\n"
    ";   2\n"
    ";   3\n"
    "APPLY\n",

    // APPLY Named Closure
    "CLOSURE (a b c) (\n"
    "  PUSH (bar)\n"
    "  RETURN\n"
    ")\n"
    "PUSH (foo)\n"
    "; STACK:\n"
    ";   foo   (symbol)\n"
    ";   (closure)\n"
    "SET\n"
    "; env={ foo=closure }\n"
    "PUSH (1 2 3)\n"
    "PUSH (foo)\n"
    "LOOKUP\n"
    "; STACK:\n"
    ";   (closure)\n"
    ";   1\n"
    ";   2\n"
    ";   3\n"
    "APPLY\n",

    NULL,
};

const char *invalid_expressions[] = {
    // Invalid chars
    "&PUSH\n",

    "PUSH (. & $)\n",

    // Incomplete CLOSURE
    "CLOSURE A B C ( PUSH\n",

    NULL,
};

int run_parser_on(const char *type, const char *input, int expected_result) {
  ParseContext ctx;

  reset_parse_context(&ctx);
  ctx.obj_pool = obj_pool_init(4096);

  yyin = fmemopen((void *)input, strlen(input) + 1, "r"); // + 1 for one unput

  int result = yyparse(&ctx);

  yylex_destroy();
  fclose(yyin);

  obj_pool_reset(ctx.obj_pool);

  if (result != expected_result)
    fprintf(stderr,
            "Expected result does't match result for %s "
            "input:\n\x1b[31m%s\x1b[0m\n",
            type, input);
  return result;
}

START_TEST(test_valid_valid_expressions) {
  int expected_result = 0;
  for (unsigned i = 0; valid_expressions[i]; ++i) {
    const char *exp = valid_expressions[i];
    ck_assert_int_eq(run_parser_on("valid", exp, expected_result),
                     expected_result);
  }
}
END_TEST

START_TEST(test_invalid_expressions) {
  int expected_result = 1;

  for (unsigned i = 0; invalid_expressions[i]; ++i) {
    const char *exp = invalid_expressions[i];
    ck_assert_int_eq(run_parser_on("invalid", exp, expected_result),
                     expected_result);
  }
}
END_TEST

Suite *parser_suite(void) {
  Suite *s = suite_create("Parser");

  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_valid_valid_expressions);
  tcase_add_test(tc_core, test_invalid_expressions);

  suite_add_tcase(s, tc_core);
  return s;
}
