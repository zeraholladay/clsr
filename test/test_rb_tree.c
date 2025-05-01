#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rb_tree.h"

rb_node *make_node(const char *key) {
  rb_node *n = rb_alloc();
  size_t len = strlen(key);
  n->key = malloc(len + 1);
  memcpy(n->key, key, len + 1);
  return n;
}

START_TEST(test_insert_and_lookup) {
  rb_node *root = NULL;

  rb_node *n1 = make_node("apple");
  rb_node *n2 = make_node("banana");
  rb_node *n3 = make_node("cherry");

  rb_insert(&root, n1);
  rb_insert(&root, n2);
  rb_insert(&root, n3);

  rb_node *result;

  result = rb_lookup(root, "apple");
  ck_assert_ptr_nonnull(result);
  ck_assert_str_eq(result->key, "apple");

  result = rb_lookup(root, "banana");
  ck_assert_ptr_nonnull(result);
  ck_assert_str_eq(result->key, "banana");

  result = rb_lookup(root, "cherry");
  ck_assert_ptr_nonnull(result);
  ck_assert_str_eq(result->key, "cherry");

  result = rb_lookup(root, "date");
  ck_assert_ptr_null(result);

  free(n1->key);
  free(n2->key);
  free(n3->key);
  free(n1);
  free(n2);
  free(n3);
}
END_TEST

START_TEST(test_remove) {
  rb_node *root = NULL;

  rb_insert(&root, make_node("a"));
  rb_insert(&root, make_node("b"));
  rb_insert(&root, make_node("c"));

  // Test removal
  rb_node *found_b = rb_lookup(root, "b");
  ck_assert_ptr_nonnull(found_b);
  ck_assert_str_eq(found_b->key, "b");

  char *removed_key = found_b->key; // ie removed may be a different node.
  rb_node *removed = rb_remove(&root, found_b);

  ck_assert_ptr_nonnull(removed);
  ck_assert_str_eq(removed_key, "b");
  ck_assert_ptr_null(rb_lookup(root, "b"));

  free(removed_key);
  free(removed);

  // Verify the rest and cleanup
  rb_node *found_a = rb_lookup(root, "a");
  ck_assert_ptr_nonnull(found_a);
  ck_assert_str_eq(found_a->key, "a");

  rb_node *found_c = rb_lookup(root, "c");
  ck_assert_ptr_nonnull(found_c);
  ck_assert_str_eq(found_c->key, "c");

  free(found_a->key);
  free(found_c->key);
  free(found_a);
  free(found_c);
}
END_TEST

Suite *rb_tree_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("RedBlackTree");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_insert_and_lookup);
  tcase_add_test(tc_core, test_remove);
  suite_add_tcase(s, tc_core);

  return s;
}
