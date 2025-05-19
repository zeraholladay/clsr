#include <stdlib.h>

#include "list.h"
#include "oom_handlers.h"
#include "safe_str.h"

#ifndef HEAP_LIST_INIT_CAPACITY
#define HEAP_LIST_INIT_CAPACITY 4
#endif

extern oom_handler_t heap_list_oom_handler;

// thanks python
List *
list_alloc (void)
{
  List *list = calloc (1, sizeof (List));

  if (!list)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return NULL;
    }

  list->items = calloc (HEAP_LIST_INIT_CAPACITY, sizeof *(list->items));

  if (!list->items)
    {
      free (list), list = NULL;
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return NULL;
    }

  list->capacity = HEAP_LIST_INIT_CAPACITY;
  list->count = 0;

  return list;
}

void
list_free (List *list)
{
  if (!list)
    return;
  free (list->items);
  free (list);
}

static int
_list_resize (List *list, size_t min_capacity)
{
  size_t new_capacity = list->capacity;

  while (new_capacity < min_capacity)
    {
      new_capacity += (new_capacity >> 3) + (new_capacity < 9 ? 3 : 6);
    }

  void **new_nodes = realloc (list->items, new_capacity * sizeof (void *));

  if (!new_nodes)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return -1;
    }

  list->items = new_nodes;
  list->capacity = new_capacity;

  return 0;
}

int
list_append (List *list, void *item)
{
  if ((list->count >= list->capacity)
      && (_list_resize (list, list->count + 1) != 0))
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return -1;
    }
  list->items[list->count++] = item;
  return 0;
}

size_t
list_append_strdup (List *list, char *str)
{
  size_t len = NULLABLE_STRLEN (str);
  char *dup = safe_strndup (str, len);

  if (!dup)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      free (dup);
      return 0;
    }

  if (list_append (list, dup))
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return 0;
    }

  return len;
}

void
list_remove_index (List *list, size_t i)
{
  if (i >= list->count)
    return;

  for (size_t j = i + 1; j < list->count; ++j)
    {
      list->items[j - 1] = list->items[j];
    }

  list->count--;
}
