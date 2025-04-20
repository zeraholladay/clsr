#ifndef OBJ_H
#define OBJ_H

#include "common.h"
#include "prim_op.h"

#define OBJ_AS(obj_ptr, kind) ((obj_ptr)->as.kind)

#define OBJ_ISKIND(obj_ptr, kind) (OBJ_KIND(obj_ptr) == kind)
#define OBJ_KIND(obj_ptr) (obj_ptr)->kind

typedef enum { Obj_Literal, Obj_List, Obj_Call, Obj_Closure } ObjKind;

typedef enum {
  Literal_Int,
  Literal_Sym,
} ObjLiteralKind;

typedef struct {
  ObjLiteralKind kind;
  union {
    int integer;
    const char *symbol;
  };
} ObjLiteral;

typedef struct ObjList {
  struct Obj **nodes;
  unsigned int count;
} ObjList;

typedef struct ObjCall {
  const PrimOp *prim;
  struct Obj *args;
} ObjCall;

typedef struct ObjClosure {
  struct Obj *params;
  struct Obj *body;
} ObjClosure;

typedef struct Obj {
  ObjKind kind;

  union {
    ObjLiteral literal;
    ObjList list;
    ObjCall call;
    ObjClosure closure;
  } as;
} Obj;

Obj *obj_new_literal_int(int i);
Obj *obj_new_literal_sym(const char *sym);
Obj *obj_new_empty_expr_list(void);
Obj *obj_expr_list_append(Obj *list, Obj *item);
Obj *obj_new_call(const PrimOp *prim, Obj *args);
Obj *obj_new_closure(Obj *params, Obj *body);

#endif