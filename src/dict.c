#include "dict.h"
#include "safe_str.h"

#define EMPTY -1
#define TOMBSTONE -2

#define HASH(str) (djb2 (key))
#define LIST_IDX(d_ptr, i) ((DictEntity *)((d_ptr)->list->items[i]))
#define STR_EQ(s1, s2, len) (!safe_strncmp_minlen (s1, s2, len + 1))

static size_t
get_bins_idx (Dict *dict, unsigned long hash_key, const char *key, size_t len)
{
  size_t start_bin_idx, bin_idx;
  ssize_t first_tombstone = -1;

  start_bin_idx = bin_idx = hash_key & (dict->capacity - 1);

  do
    {
      if (dict->bins[bin_idx] == EMPTY)
        {
          return (first_tombstone >= 0) ? (size_t)first_tombstone : bin_idx;
        }
      else if (dict->bins[bin_idx] == TOMBSTONE)
        {
          if (first_tombstone == -1)
            first_tombstone = bin_idx;
        }
      else if (STR_EQ (key, LIST_IDX (dict, dict->bins[bin_idx])->key, len))
        {
          return bin_idx;
        }
      bin_idx = (bin_idx + 1) & (dict->capacity - 1);
    }
  while (bin_idx != start_bin_idx);

  // TODO & FIXME: all bins are occupied or all bins are tombstones: resize
  return first_tombstone >= 0 ? (size_t)first_tombstone : start_bin_idx;
}

static DictEntity *
remove (Dict *dict, const char *key)
{
}

Dict *
dict_alloc_va_list (const char *key, ...)
{
  Dict *dict = dict_alloc (NULL, 0);
  va_list ap;

  va_start (ap, key);
  for (const char *k = key; k; k = va_arg (ap, const char *))
    {
      dict_insert (dict, k, va_arg (ap, void *));
    }
  va_end (ap);

  return dict;
}

Dict *
dict_alloc (const DictEntity *entity, size_t n)
{
  Dict *dict
      = calloc (1, sizeof *(dict) + (DICT_INIT_CAP * sizeof *(dict->bins)));

  if (!dict)
    {
      return NULL;
    }

  // empty all
  for (size_t i = 0; i < dict->capacity; ++i)
    dict->bins[i] = -1;

  dict->capacity = DICT_INIT_CAP;
  dict->list = list_alloc ();

  if (!dict->list)
    {
      free (dict);
      return NULL;
    }

  for (size_t i = 0; i < n; i++)
    {
      int res = dict_insert (dict, entity[i].key, entity[i].val);

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
}

void
dict_del (Dict *dict, const char *key)
{
}

int
dict_insert (Dict *dict, const char *key, void *val)
{
  if (!dict || !dict->list || !key)
    {
      return -1;
    }

  if ((dict->count + 1) * 5 > dict->capacity * 4)
    {
      grow_dict (&dict);
    }

  unsigned long hash_key = hash (key);
  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  size_t bin_idx = get_bins_idx (dict, hash_key, key, len);

  int bin_val = dict->bins[bin_idx];

  if (bin_val >= 0)
    {
      LIST_IDX (dict, bin_val)->val = val;
      return dict->count;
    }

  // new
  DictEntity *entity = malloc (sizeof *(entity));

  if (!entity)
    {
      return -1;
    }

  entity->hash_key = hash_key;
  entity->key = key;
  entity->val = val;

  int list_idx = list_append (dict->list, entity);

  if (list_idx < 0)
    {
      free (entity);
      return -1;
    }

  dict->bins[bin_idx] = list_idx;

  return ++(dict->count);
}

DictEntity *
dict_lookup (Dict *dict, const char *key)
{
  if (!dict || !dict->list || !key)
    {
      return NULL;
    }

  size_t len = safe_strnlen (key, DICT_STR_MAX_LEN);
  size_t bins_idx = get_bins_idx (dict, hash (key), key, len);
  int bin_val = dict->bins[bins_idx];

  if (bin_val >= 0)
    {
      return LIST_IDX (dict, bin_val);
    }

  return NULL;
}
