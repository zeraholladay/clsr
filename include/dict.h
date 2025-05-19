#ifndef DICT_H
#define DICT_H

#include <stdarg.h>

#include "list.h"

#ifndef DICT_INIT_CAP
#define DICT_INIT_CAP 8
#endif

#ifndef DICT_STR_MAX_LEN
#define DICT_STR_MAX_LEN 256
#endif

#define DICT_ENTITY(k, v) { .hash_key = 0, .key = key, .val = val }

typedef struct
{
  unsigned long hash_key;
  const char *key;
  void *val;
} DictEntity;

typedef struct
{
  size_t count, capacity;
  List *list;
  int *bins;
} Dict;

Dict *dict_alloc_va_list (const char *key, ...);
Dict *dict_alloc (const DictEntity *key_val, size_t n);
void dict_destroy (Dict *dict);

void dict_del (Dict *dict, const char *key);
int dict_insert (Dict *dict, const char *key, void *val);
int dict_lookup (Dict *dict, const char *key, void **val);

#endif
