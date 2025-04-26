#include "stack.h"
#include "common.h"

static int stack_data_alloc(Stack *s_ptr, unsigned int count) {
  if (REALLOC_N(s_ptr->data, count))
    return 1;
  s_ptr->data_size = count;
  return 0;
}

void stack_init(Stack *s_ptr, unsigned int count) {
  if (stack_data_alloc(s_ptr, count))
    die(LOCATION);
  s_ptr->sp = s_ptr->fp = 0;
}

void stack_free(Stack *s_ptr) { FREE(s_ptr->data); }

void push(Stack *s_ptr, void *value) {
  if (s_ptr->sp >= s_ptr->data_size)
    if (stack_data_alloc(s_ptr, s_ptr->data_size + STACK_GROWTH))
      die(LOCATION);
  s_ptr->data[s_ptr->sp++] = (uintptr_t)value;
}

void *pop(Stack *s_ptr) {
  if (s_ptr->sp <= 0)
    return NULL;
  return (void *)s_ptr->data[--s_ptr->sp];
}

void *peek(Stack *s_ptr) {
  if (s_ptr->sp <= 0)
    return NULL;
  return (void *)(s_ptr->data[s_ptr->sp - 1]);
}

void enter_frame(Stack *s_ptr) {
  push(s_ptr, (void *)s_ptr->fp);
  s_ptr->fp = s_ptr->sp;
}

void exit_frame(Stack *s_ptr) {
  s_ptr->sp = s_ptr->fp;
  s_ptr->fp = (uintptr_t)pop(s_ptr);
}
