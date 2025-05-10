#ifndef HEAP_LIST_H
#define HEAP_LIST_H

#include <stdlib.h>

typedef struct {
    size_t count;
    size_t capacity;
    void **items;
  } HeapList;

HeapList *hl_alloc(void);
void hl_free(HeapList *hl);
int hl_append(HeapList *hl, void *item);
inline size_t hl_append_strdup(HeapList *hl, char *str);

#endif