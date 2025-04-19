#ifndef OBJECT_H
#define OBJECT_H

typedef enum ObjectType {
  Object_Int,
  Object_Sym,
} ObjectType;

typedef struct object {
  ObjectType type;
  union {
    int integr;
    const char *symbol;
  } as;
} Object;

#define ALLOC_INT(addr) object_alloc(Object_Int, addr)
#define ALLOC_SYM(addr) object_alloc(Object_Sym, addr)

Object *object_alloc(ObjectType type, int *addr);

#endif
