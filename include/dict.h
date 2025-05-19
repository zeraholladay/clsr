#ifndef DICT_H
#define DICT_H

#include <stdarg.h>

#include "list.h"
#include "rb_tree.h"

#ifndef DICT_HASH_SIZE
#define DICT_HASH_SIZE  8
#endif

#ifndef DICT_STR_MAX_LEN
#define DICT_STR_MAX_LEN 256
#endif

#ifndef DICT_ALLOC_VA_LIST_MAX
#define DICT_ALLOC_VA_LIST_MAX 32
#endif

typedef enum
{
  DICT_HASH,
  DICT_TREE,
} DictType;

typedef struct
{
  const char *key;
  void *val;
} KeyValue;

typedef struct
{
  DictType type;
  union
  {
    rb_node *tree;
    List **hash;
  };
} Dict;

Dict *dict_alloc_va_list (DictType type, const char *key, ...);
Dict *dict_alloc (DictType type, const KeyValue *key_val, size_t n);
void dict_destroy (Dict *dict);

void dict_del (Dict *dict, const char *key);
int dict_insert (Dict *dict, const char *key, void *val);
int dict_lookup (Dict *dict, const char *key, void **val);

#endif
