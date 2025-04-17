#ifndef OBJECT_H
#define OBJECT_H

enum _object_t_enum {
  OBJ_INT,
  // OBJ_CLOSURE,
  OBJ_SYM,
  // OBJ_NIL
};

typedef struct object {
  enum _object_t_enum type;
  union {
    int i;
    const char *symbol;
  };
} object_t;

#define ALLOC_INT(addr) alloc_object(OBJ_INT, addr)
#define ALLOC_SYM(addr) alloc_object(OBJ_SYM, addr)

object_t *alloc_object(enum _object_t_enum type, int *addr);

#endif
