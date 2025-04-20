#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"
#include "environ.h"

typedef enum ObjectKind {
  Object_AST,
  Object_Int,
  Object_Sym,
} ObjectKind;

typedef struct object {
  ObjectKind kind;

  union {
    ASTNode *ast;
    int integr;
    const char *symbol;

  } as;
} Object;

#define ALLOC_INT(addr) object_alloc(Object_Int, addr)
#define ALLOC_SYM(addr) object_alloc(Object_Sym, addr)

Object *object_alloc(ObjectKind kind, int *addr);

#endif
