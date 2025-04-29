#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"

START_TEST(test_env) {
  Env *env = env_new(NULL);

  const char *key_1 = "DEADBEEF";
  const char *key_2 = "DEADFACE";
  const char *key_3 = "DEADCAFE";

  void *val_1 = (void *)0xDEADBEEF;
  void *val_2 = (void *)0xDEADFACE;
  void *val_3 = (void *)0xDEADCAFE;

  void *rval_1;
  void *rval_2;
  void *rval_3;

  env_set(env, key_1, val_1);
  env_set(env, key_2, val_2);
  env_set(env, key_3, val_3);

  int status_1 = env_lookup(env, key_1, &rval_1);
  int status_2 = env_lookup(env, key_2, &rval_2);
  int status_3 = env_lookup(env, key_3, &rval_3);

  ck_assert_int_eq(status_1, 0);
  ck_assert_int_eq(status_2, 0);
  ck_assert_int_eq(status_3, 0);

  ck_assert_ptr_eq(rval_1, val_1);
  ck_assert_ptr_eq(rval_2, val_2);
  ck_assert_ptr_eq(rval_3, val_3);
}
END_TEST

START_TEST(test_env_frame) {
  Env *env_parent = env_new(NULL);
  Env *env_child = env_new(env_parent);

  const char *key_1 = "DEADBEEF";
  const char *key_2 = "DEADFACE";
  const char *key_3 = "DEADCAFE";

  void *val_1 = (void *)0xDEADBEEF;
  void *val_2 = (void *)0xDEADFACE;
  void *val_3 = (void *)0xDEADCAFE;

  void *rval_1;
  void *rval_2;
  void *rval_3;

  // fall through from child to parent

  env_set(env_parent, key_1, val_1);
  env_set(env_parent, key_2, val_2);
  env_set(env_parent, key_3, val_3);

  env_lookup(env_child, key_1, &rval_1);
  env_lookup(env_child, key_2, &rval_2);
  env_lookup(env_child, key_3, &rval_3);

  ck_assert_ptr_eq(rval_1, val_1);
  ck_assert_ptr_eq(rval_2, val_2);
  ck_assert_ptr_eq(rval_3, val_3);

  // child overrides

  const char *child_key1 = "FACEFADE";
  const char *child_key2 = "DEADFACE";

  void *child_rval_1 = NULL;
  void *child_rval_2 = NULL;

  env_set(env_child, child_key1, child_rval_1);
  env_set(env_child, child_key2, child_rval_2);

  env_lookup(env_child, child_key1, &child_rval_1);
  env_lookup(env_child, child_key2, &child_rval_2);

  ck_assert_ptr_eq(child_rval_1, child_rval_1);
  ck_assert_ptr_eq(child_rval_2, child_rval_2);
}
END_TEST

START_TEST(test_env_fail) {
  Env *env = env_new(NULL);

  const char *key_1 = "DEADBEEF";
  const char *key_2 = "DEADFACE";

  void *val_1 = (void *)0xDEADBEEF;
  void *val_2 = (void *)0xDEADFACE;

  void *rval_1;
  void *rval_2;
  void *rval_3;

  env_set(env, key_1, val_1);
  env_set(env, key_2, val_2);

  int status_1 = env_lookup(env, key_1, &rval_1);
  int status_2 = env_lookup(env, key_2, &rval_2);
  int status_3 = env_lookup(env, "BOGUS", &rval_3);

  ck_assert_int_eq(status_1, 0);
  ck_assert_int_eq(status_2, 0);
  ck_assert_int_eq(status_3, -1);
}
END_TEST

Suite *env_suite(void) {
  Suite *s = suite_create("Env");

  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_env);
  tcase_add_test(tc_core, test_env_frame);
  tcase_add_test(tc_core, test_env_fail);

  suite_add_tcase(s, tc_core);
  return s;
}
