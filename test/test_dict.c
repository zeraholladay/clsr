#include "dict.h"
#include <check.h>

static Dict *dict;

void
setup (void)
{
  dict = dict_alloc (DICT_HASH, NULL, 0);
  ck_assert_ptr_nonnull (dict);
}

void
teardown (void)
{
  dict_destroy (dict);
}

static const char *keys[] = { "alpha", "beta", "gamma", "delta" };
static int vals_int[] = { 10, 20, 30, 40 };

START_TEST (test_insert_and_lookup)
{
  void *out;

  ck_assert_int_eq (dict_insert (dict, "one", (void *)(intptr_t)1), 0);
  ck_assert_int_eq (dict_lookup (dict, "one", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 1);

  // missing key
  ck_assert_int_eq (dict_lookup (dict, "two", &out), -1);
}
END_TEST

START_TEST (test_insert_override)
{
  void *out;

  ck_assert_int_eq (dict_insert (dict, "key", (void *)(intptr_t)100), 0);
  ck_assert_int_eq (dict_lookup (dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 100);

  // Insert same key with new value
  ck_assert_int_eq (dict_insert (dict, "key", (void *)(intptr_t)200), 0);
  ck_assert_int_eq (dict_lookup (dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 200);
}
END_TEST

START_TEST (test_delete_existing)
{
  void *out;

  dict_insert (dict, "foo", (void *)(intptr_t)123);
  ck_assert_int_eq (dict_lookup (dict, "foo", &out), 0);

  dict_del (dict, "foo");
  ck_assert_int_eq (dict_lookup (dict, "foo", &out), -1);
}
END_TEST

START_TEST (test_delete_nonexistent)
{
  void *out;

  // Deleting a missing key should be safe
  dict_del (dict, "nope");
  // Still returns not found
  ck_assert_int_eq (dict_lookup (dict, "nope", &out), -1);
}
END_TEST

START_TEST (test_multiple_entries)
{
  void *out;
  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_insert (dict, keys[i], &vals_int[i]), 0);
    }

  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_lookup (dict, keys[i], &out), 0);
      ck_assert_ptr_eq (out, &vals_int[i]);
    }

  ck_assert_int_eq (dict_lookup (dict, "epsilon", &out), -1);
}
END_TEST

Suite *
dict_suite (void)
{
  Suite *s = suite_create ("Dict");
  TCase *tc = tcase_create ("Core");
  tcase_add_checked_fixture (tc, setup, teardown);

  tcase_add_test (tc, test_insert_and_lookup);
  tcase_add_test (tc, test_insert_override);
  tcase_add_test (tc, test_delete_existing);
  tcase_add_test (tc, test_delete_nonexistent);
  tcase_add_test (tc, test_multiple_entries);

  suite_add_tcase (s, tc);
  return s;
}
