#include <stdlib.h>

#include "heap_list.h"
#include "oom_handlers.h"
#include "safe_str.h"

#ifndef HEAP_LIST_INIT_CAPACITY
#define HEAP_LIST_INIT_CAPACITY 4
#endif

extern oom_handler_t heap_list_oom_handler;

// thanks python
HeapList *
hl_alloc (void)
{
  HeapList *hl = calloc (1, sizeof (HeapList));

  if (!hl)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return NULL;
    }

  hl->items = calloc (HEAP_LIST_INIT_CAPACITY, sizeof *(hl->items));

  if (!hl->items)
    {
      free (hl), hl = NULL;
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return NULL;
    }

  hl->capacity = HEAP_LIST_INIT_CAPACITY;
  hl->count = 0;

  return hl;
}

void
hl_free (HeapList *hl)
{
  if (!hl)
    return;
  free (hl->items);
  free (hl);
}

static int
_hl_resize (HeapList *hl, size_t min_capacity)
{
  size_t new_capacity = hl->capacity;

  while (new_capacity < min_capacity)
    {
      new_capacity += (new_capacity >> 3) + (new_capacity < 9 ? 3 : 6);
    }

  void **new_nodes = realloc (hl->items, new_capacity * sizeof (void *));

  if (!new_nodes)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return -1;
    }

  hl->items = new_nodes;
  hl->capacity = new_capacity;

  return 0;
}

int
hl_append (HeapList *hl, void *item)
{
  if ((hl->count >= hl->capacity) && (_hl_resize (hl, hl->count + 1) != 0))
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return -1;
    }
  hl->items[hl->count++] = item;
  return 0;
}

size_t
hl_append_strdup (HeapList *hl, char *str)
{
  size_t len = NULLABLE_STRLEN (str);
  char *dup = safe_strndup (str, len);

  if (!dup)
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      free (dup);
      return 0;
    }

  if (hl_append (hl, dup))
    {
      heap_list_oom_handler (NULL, OOM_LOCATION);
      return 0;
    }

  return len;
}

void
hl_remove_index (HeapList *hl, size_t i)
{
  if (i >= hl->count)
    return;

  for (size_t j = i + 1; j < hl->count; ++j)
    {
      hl->items[j - 1] = hl->items[j];
    }

  hl->count--;
}
