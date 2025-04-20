#include "obj.h"

Obj *obj_new_literal_int(int i) {
  Obj *obj = NULL;
  if (ALLOC(obj))
    die(LOCATION);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = &OBJ_AS(obj, literal);
  literal_ptr->kind = Literal_Int;
  literal_ptr->integer = i;
  return obj;
}

Obj *obj_new_literal_sym(const char *sym) {
  Obj *obj = NULL;
  if (ALLOC(obj))
    die(LOCATION);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = &OBJ_AS(obj, literal);
  literal_ptr->kind = Literal_Sym;
  literal_ptr->symbol = sym;
  return obj;
}

Obj *obj_new_empty_expr_list(void) {
  Obj *obj = NULL;
  if (ALLOC(obj))
    die(LOCATION);
  OBJ_KIND(obj) = Obj_List;
  ObjList *list_ptr = &OBJ_AS(obj, list);
  list_ptr->nodes = NULL;
  list_ptr->count++;
  return obj;
}

Obj *obj_expr_list_append(Obj *obj, Obj *item) {
  size_t count = OBJ_AS(obj, list).count;
  Obj **new_objs = OBJ_AS(obj, list).nodes;
  if (REALLOC_N(new_objs, count + 1))
    die(LOCATION);
  new_objs[count] = item;
  ObjList *list_ptr = &OBJ_AS(obj, list);
  list_ptr->nodes = new_objs;
  list_ptr->count++;
  return obj;
}

Obj *obj_new_call(const PrimOp *prim, Obj *args) {
  Obj *obj = NULL;
  if (ALLOC(obj))
    die(LOCATION);
  OBJ_KIND(obj) = Obj_Call;
  ObjCall *call_ptr = &OBJ_AS(obj, call);
  call_ptr->prim = prim;
  call_ptr->args = args;
  return obj;
}

Obj *obj_new_closure(Obj *params, Obj *body) {
  Obj *obj = NULL;
  if (ALLOC(obj))
    die(LOCATION);
  OBJ_KIND(obj) = Obj_Closure;
  ObjClosure *closure_ptr = &OBJ_AS(obj, closure);
  closure_ptr->params = params;
  closure_ptr->body = body;
  return obj;
}
