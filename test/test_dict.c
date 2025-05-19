#include "dict.h"
#include <check.h>

static Dict *hash_dict;
static Dict *tree_dict;

void
setup (void)
{
  hash_dict = dict_alloc (DICT_HASH, NULL, 0);
  ck_assert_ptr_nonnull (hash_dict);

  tree_dict = dict_alloc (DICT_TREE, NULL, 0);
  ck_assert_ptr_nonnull (tree_dict);
}

void
teardown (void)
{
  dict_destroy (hash_dict);
}

static const char *keys[] = { "alpha", "beta", "gamma", "delta" };
static int vals_int[] = { 10, 20, 30, 40 };

START_TEST (test_insert_and_lookup)
{
  void *out;

  // hash
  ck_assert_int_eq (dict_insert (hash_dict, "one", (void *)(intptr_t)1), 0);
  ck_assert_int_eq (dict_lookup (hash_dict, "one", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 1);

  // missing key
  ck_assert_int_eq (dict_lookup (hash_dict, "two", &out), -1);

  // tree
  ck_assert_int_eq (dict_insert (tree_dict, "one", (void *)(intptr_t)1), 0);
  ck_assert_int_eq (dict_lookup (tree_dict, "one", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 1);

  // missing key
  ck_assert_int_eq (dict_lookup (tree_dict, "two", &out), -1);
}
END_TEST

START_TEST (test_insert_override)
{
  void *out;

  // hash
  ck_assert_int_eq (dict_insert (hash_dict, "key", (void *)(intptr_t)100), 0);
  ck_assert_int_eq (dict_lookup (hash_dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 100);

  // Insert same key with new value
  ck_assert_int_eq (dict_insert (hash_dict, "key", (void *)(intptr_t)200), 0);
  ck_assert_int_eq (dict_lookup (hash_dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 200);

  // tree
  ck_assert_int_eq (dict_insert (tree_dict, "key", (void *)(intptr_t)100), 0);
  ck_assert_int_eq (dict_lookup (tree_dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 100);

  // Insert same key with new value
  ck_assert_int_eq (dict_insert (tree_dict, "key", (void *)(intptr_t)200), 0);
  ck_assert_int_eq (dict_lookup (tree_dict, "key", &out), 0);
  ck_assert_int_eq ((intptr_t)out, 200);
}
END_TEST

START_TEST (test_delete_existing)
{
  void *out;

  // hash
  dict_insert (hash_dict, "foo", (void *)(intptr_t)123);
  ck_assert_int_eq (dict_lookup (hash_dict, "foo", &out), 0);

  dict_del (hash_dict, "foo");
  ck_assert_int_eq (dict_lookup (hash_dict, "foo", &out), -1);

  // tree
  dict_insert (tree_dict, "foo", (void *)(intptr_t)123);
  ck_assert_int_eq (dict_lookup (tree_dict, "foo", &out), 0);

  dict_del (tree_dict, "foo");
  ck_assert_int_eq (dict_lookup (tree_dict, "foo", &out), -1);
}
END_TEST

START_TEST (test_delete_nonexistent)
{
  void *out;

  // hash
  // Deleting a missing key should be safe
  dict_del (hash_dict, "nope");
  // Still returns not found
  ck_assert_int_eq (dict_lookup (hash_dict, "nope", &out), -1);

    // tree
  // Deleting a missing key should be safe
  dict_del (tree_dict, "nope");
  // Still returns not found
  ck_assert_int_eq (dict_lookup (tree_dict, "nope", &out), -1);
}
END_TEST

START_TEST (test_multiple_entries)
{
  void *out;

  // hash
  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_insert (hash_dict, keys[i], &vals_int[i]), 0);
    }

  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_lookup (hash_dict, keys[i], &out), 0);
      ck_assert_ptr_eq (out, &vals_int[i]);
    }

  ck_assert_int_eq (dict_lookup (hash_dict, "epsilon", &out), -1);

  // tree
  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_insert (tree_dict, keys[i], &vals_int[i]), 0);
    }

  for (size_t i = 0; i < 4; i++)
    {
      ck_assert_int_eq (dict_lookup (tree_dict, keys[i], &out), 0);
      ck_assert_ptr_eq (out, &vals_int[i]);
    }

  ck_assert_int_eq (dict_lookup (tree_dict, "epsilon", &out), -1);
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
