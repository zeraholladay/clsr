#include <check.h>
#include <stdlib.h>

#ifdef TEST_MAIN

extern Suite *parser_suite(void);
extern Suite *stack_suite(void);
extern Suite *env_suite(void);
extern Suite *eval_suite(void);

int main(void) {
  SRunner *sr = srunner_create(stack_suite());

  srunner_add_suite(sr, stack_suite());
  srunner_add_suite(sr, env_suite());
  srunner_add_suite(sr, parser_suite());
  srunner_add_suite(sr, eval_suite());

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif