#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

START_TEST(test_push_pop) {
  stack_reset();
  void *val_in = (void *)0xDEADBEEF;
  PUSH(val_in);
  ck_assert_ptr_eq(POP(), val_in);
}
END_TEST

START_TEST(test_lifo_order) {
  stack_reset();
  PUSH((void *)1);
  PUSH((void *)2);
  PUSH((void *)3);
  ck_assert_ptr_eq(POP(), (void *)3);
  ck_assert_ptr_eq(POP(), (void *)2);
  ck_assert_ptr_eq(POP(), (void *)1);
}
END_TEST

START_TEST(test_peek) {
  stack_reset();
  void *val = (void *)0x1234;
  PUSH(val);
  ck_assert_ptr_eq(PEEK(), val);
  ck_assert_ptr_eq(POP(), val);
}
END_TEST

START_TEST(test_enter_exit_frame) {
  stack_reset();
  // uintptr_t old_fp = s_ptr->fp;
  PUSH((void *)100);
  enter_frame();
  // ck_assert_uint_eq(s_ptr->fp, s_ptr->sp);
  PUSH((void *)200);
  EXIT_FRAME();
  // ck_assert_uint_eq(s_ptr->fp, old_fp);
  ck_assert_ptr_eq(PEEK(), (void *)100);
}
END_TEST

Suite *stack_suite(void) {
  Suite *s = suite_create("Parser");

  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_push_pop);
  tcase_add_test(tc_core, test_lifo_order);
  tcase_add_test(tc_core, test_peek);
  tcase_add_test(tc_core, test_enter_exit_frame);

  suite_add_tcase(s, tc_core);
  return s;
}

// int main(void) {
//   int failed = 0;
//   Suite *s = stack_suite();
//   SRunner *sr = srunner_create(s);
//   srunner_run_all(sr, CK_NORMAL);
//   failed = srunner_ntests_failed(sr);
//   srunner_free(sr);
//   return (failed == 0) ? 0 : 1;
// }
