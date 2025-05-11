#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_def.h"
#include "palloc.h"

START_TEST(test_palloc) {
  size_t count = 4096;

  Pool *pool = pool_init(count, sizeof(Node));

  ck_assert(pool);

  for (size_t i = 0; i < count + 1; ++i) {
    ck_assert(pool_alloc(pool));
  }

  // never reached
  pool_reset_all(pool);
}
END_TEST

Suite *palloc_suite(void) {
  Suite *s = suite_create("Palloc");

  TCase *tc_core = tcase_create("Core");

  tcase_add_exit_test(tc_core, test_palloc, 1);

  suite_add_tcase(s, tc_core);
  return s;
}
