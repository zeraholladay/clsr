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

  env_set(env, key_1, val_1);
  env_set(env, key_2, val_2);
  env_set(env, key_3, val_3);

  rb_node *n1 = env_lookup(env, key_1);
  rb_node *n2 = env_lookup(env, key_2);
  rb_node *n3 = env_lookup(env, key_3);

  ck_assert(n1);
  ck_assert(n2);
  ck_assert(n3);

  ck_assert(RB_VAL(n1) == val_1);
  ck_assert(RB_VAL(n2) == val_2);
  ck_assert(RB_VAL(n3) == val_3);
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

  rb_node *n1;
  rb_node *n2;
  rb_node *n3;

  // fall through from child to parent

  env_set(env_parent, key_1, val_1);
  env_set(env_parent, key_2, val_2);
  env_set(env_parent, key_3, val_3);

  n1 = env_lookup(env_child, key_1);
  n2 = env_lookup(env_child, key_2);
  n3 = env_lookup(env_child, key_3);

  ck_assert(RB_VAL(n1) == val_1);
  ck_assert(RB_VAL(n2) == val_2);
  ck_assert(RB_VAL(n3) == val_3);

  // child overrides

  const char *child_key1 = "FACEFADE";
  const char *child_key2 = "DEADFACE";

  rb_node *child_n1 = NULL;
  rb_node *child_n2 = NULL;

  env_set(env_child, child_key1, child_n1);
  env_set(env_child, child_key2, child_n2);

  child_n1 = env_lookup(env_child, child_key1);
  child_n2 = env_lookup(env_child, child_key2);

  ck_assert(child_n1 == child_n1);
  ck_assert(child_n2 == child_n2);
}
END_TEST

START_TEST(test_env_fail) {
  Env *env = env_new(NULL);

  const char *key_1 = "DEADBEEF";
  const char *key_2 = "DEADFACE";

  void *val_1 = (void *)0xDEADBEEF;
  void *val_2 = (void *)0xDEADFACE;

  env_set(env, key_1, val_1);
  env_set(env, key_2, val_2);

  rb_node *n1 = env_lookup(env, key_1);
  rb_node *n2 = env_lookup(env, key_2);
  rb_node *n3 = env_lookup(env, "BOGUS");

  ck_assert(n1);
  ck_assert(n2);
  ck_assert(n3 == NULL);
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
