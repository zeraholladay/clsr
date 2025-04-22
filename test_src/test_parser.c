#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yyparse(void);
// extern int parse_success;
extern void yy_scan_string(const char *);
extern void yylex_destroy(void);

// Run parser on a string and return whether it succeeded
int run_parser_on(const char *input) {
  // parse_success = 0; // reset
  yy_scan_string(input);
  yyparse();
  yylex_destroy(); // clean up lexer buffer
  return 0;
}

START_TEST(test_valid_expression) {
  ck_assert_int_eq(run_parser_on("1 + 2\n"), 1);
}
END_TEST

START_TEST(test_invalid_expression) {
  ck_assert_int_eq(run_parser_on("1 +\n"), 0);
}
END_TEST

Suite *parser_suite(void) {
  Suite *s = suite_create("Parser");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_valid_expression);
  tcase_add_test(tc_core, test_invalid_expression);

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
