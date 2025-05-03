#include <stdio.h>
#include <stdlib.h>

#include "palloc.h"
#include "stack.h"
#include "sym_save.h"

#include "oom_handlers.h"

static void oom_handler_die(void *void_ptr, const char *msg) {
  (void)void_ptr;
  perror(msg);
  exit(1);
  abort();
}

stack_oom_handler_t stack_oom_handler = (stack_oom_handler_t)oom_handler_die;
palloc_oom_handler_t palloc_oom_handler = (palloc_oom_handler_t)oom_handler_die;
sym_save_oom_handler_t sym_save_oom_handler =
    (sym_save_oom_handler_t)oom_handler_die;
