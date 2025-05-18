#include "dict.h"
#include "safe_str.h"

typedef int (*DictInsertFn) (Dict *, const char *, void *);

static inline unsigned long hash (const char *key);
static void hash_destroy (Dict *dict);
static void tree_destroy (Dict *dict);
static int insert_hash (Dict *dict, const char *key, void *val);
static int insert_tree (Dict *dict, const char *key, void *val);
static KeyValue *hash_lookup (Dict *dict, const char *key);
static rb_node *tree_lookup (Dict *dict, const char *key);
static KeyValue *hash_remove (Dict *dict, const char *key);
static rb_node *tree_remove (Dict *dict, const char *key);

static inline unsigned long
hash (const char *key)
{
  return djb2 (key) % DICT_HASH_SIZE;
}

static void
hash_destroy (Dict *dict)
{
  if (!dict)
    return;

  for (size_t i = 0; i < DICT_HASH_SIZE; ++i)
    {
      List *list = dict->hash[i];

      if (list)
        {
          for (size_t j = 0; j < list->count; ++j)
            {
              if (list->items[j])
                {
                  free (list->items[j]);
                  list->items[j] = NULL;
                }
            }
          list_free (list);
        }
    }
  free (dict->hash);
  free (dict);
}

static void
tree_destroy (Dict *dict)
{
  if (!dict)
    return;

  Stack tmp_stack = {}, stack = {};

  stack_init (&tmp_stack, 32);
  stack_init (&stack, 32);

  rb_post_order_iter (dict->tree, &tmp_stack, &stack);

  while (stack.data_size)
    {
      free (stack_pop (&stack));
    }

  stack_free (&tmp_stack);
  stack_free (&stack);
}

static int
insert_hash (Dict *dict, const char *key, void *val)
{
  KeyValue *kv = hash_lookup (dict, key);

  if (kv)
    {
      kv->val = val;
      return 0;
    }

  unsigned long i = hash (key);

  if (!dict->hash[i])
    {
      dict->hash[i] = list_alloc ();

      if (!dict->hash[i])
        return -1;
    }

  KeyValue *item = malloc (sizeof *(item));

  if (!item)
    {
      free (dict->hash[i]);
      return -1;
    }

  item->key = key;
  item->val = val;

  return list_append (dict->hash[i], item);
}

static int
insert_tree (Dict *dict, const char *key, void *val)
{
  rb_node *node = tree_lookup(dict, key);

  if (node)
    {
      RB_VAL(node) = val;
    }

  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  rb_node *n = rb_alloc ();

  if (!n)
    {
      return -1;
    }

  RB_KEY (n) = key;
  RB_KEY_LEN (n) = len;
  RB_VAL (n) = val;

  rb_insert (&dict->tree, n); // void

  return 0;
}

static KeyValue *
hash_lookup (Dict *dict, const char *key)
{
  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  unsigned long i = hash (key);
  List *list = dict->hash[i];

  if (!list)
    {
      return NULL;
    }

  for (size_t i = 0; i < list->count; ++i)
    {
      KeyValue *kv = list->items[i];

      if (!safe_strncmp_minlen (key, kv->key, len + 1))
        {
          return kv;
        }
    }

  return NULL;
}

static rb_node *
tree_lookup (Dict *dict, const char *key)
{
  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  return rb_lookup (dict->tree, key, len);
}

static KeyValue *
hash_remove (Dict *dict, const char *key)
{
  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  unsigned long i = hash (key);
  List *list = dict->hash[i];

  if (!list)
    {
      return NULL;
    }

  for (size_t i = 0; i < list->count; ++i)
    {
      KeyValue *kv = list->items[i];

      if (!safe_strncmp_minlen (key, kv->key, len + 1))
        {
          list_remove_index (list, i);
          return kv;
        }
    }

  return NULL;
}

static rb_node *
tree_remove (Dict *dict, const char *key)
{
  rb_node *node = tree_lookup (dict, key);

  if (!node)
    {
      return NULL;
    }

  return rb_remove (&dict->tree, node);
}

Dict *
dict_alloc_va_list (DictType type, const char *key, ...)
{
  KeyValue kv[DICT_ALLOC_VA_LIST_MAX] = {};
  size_t i = 0;
  va_list ap;

  va_start (ap, key);
  for (const char *k = key; k; k = va_arg (ap, const char *))
    {
      kv[i].key = key;
      kv[i].val = va_arg (ap, void *);
      if (++i >= DICT_ALLOC_VA_LIST_MAX)
        {
          va_end (ap);
          return NULL;
        }
    }
  va_end (ap);

  return dict_alloc (type, kv, i);
}

Dict *
dict_alloc (DictType type, const KeyValue *kv, size_t n)
{
  DictInsertFn insert_fn;
  Dict *dict = calloc (1, sizeof *(dict));

  if (!dict)
    {
      return NULL;
    }

  dict->type = type;

  switch (type)
    {
    case DICT_HASH:
      insert_fn = insert_hash;
      dict->hash = calloc (DICT_HASH_SIZE, sizeof *(dict->hash));
      break;
    case DICT_TREE:
      insert_fn = insert_tree;
      dict->tree = rb_alloc ();
      break;
    default:
      return NULL;
      break;
    }

  if (!dict->hash || !dict->tree)
    {
      free (dict);
      return NULL;
    }

  for (size_t i = 0; i < n; i++)
    {
      int res = insert_fn (dict, kv[i].key, kv[i].val);

      if (res)
        {
          return NULL;
        }
    }

  return dict;
}

void
dict_destroy (Dict *dict)
{
  if (!dict)
    return;

  if (dict->type == DICT_HASH) // TODO: refactor
    {
      hash_destroy (dict);
    }
  else if (dict->type == DICT_TREE) // TODO: refactor
    {
      tree_destroy (dict);
    }
}

void
dict_del (Dict *dict, const char *key)
{
  KeyValue *kv;
  rb_node *node;

  switch (dict->type)
    {
    case DICT_HASH:
      kv = hash_remove (dict, key);
      if (kv)
        {
          free (kv);
        }
      break;
    case DICT_TREE:
      node = tree_remove (dict, key);
      if (node)
        {
          free (node);
        }
      break;
    default:
      break;
    }
}

int
dict_insert (Dict *dict, const char *key, void *val)
{
  DictInsertFn insert_fn;

  switch (dict->type)
    {
    case DICT_HASH:
      insert_fn = insert_hash;
      break;
    case DICT_TREE:
      insert_fn = insert_tree;
      break;
    default:
      break;
    }

  return insert_fn (dict, key, val);
}

int
dict_lookup (Dict *dict, const char *key, void **val)
{
  KeyValue *kv = NULL;
  rb_node *node = NULL;

  switch (dict->type)
    {
    case DICT_HASH:
      kv = hash_lookup (dict, key);

      if (kv)
        {
          *val = kv->val;
          return 0;
        }
      break;
    case DICT_TREE:
      node = tree_lookup (dict, key);

      if (node)
        {
          *val = RB_VAL (node);
          return 0;
        }
      break;
    default:
      break;
    }

  return -1;
}
