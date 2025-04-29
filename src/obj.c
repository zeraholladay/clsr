#include "clsr.h"

#include <stdio.h>

static Obj _obj_true = {.kind = Obj_Literal,
                        .as.literal = {
                            .kind = Literal_Sym,
                            .symbol = NULL,
                        }};

static Obj _obj_false = {.kind = Obj_Literal,
                         .as.literal = {
                             .kind = Literal_Sym,
                             .symbol = NULL,
                         }};

Obj *const obj_true = &_obj_true;
Obj *const obj_false = &_obj_false;

void obj_init_reserved_literals(void) {
  const char *true_str = "true";
  const char *false_str = "false";

  OBJ_AS(obj_true, literal).symbol = str_intern(true_str, strlen(true_str));
  OBJ_AS(obj_false, literal).symbol = str_intern(false_str, strlen(false_str));
}

Obj *obj_new_literal_int(ObjPool *p, int i) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = OBJ_AS_PTR(obj, literal);
  literal_ptr->kind = Literal_Int;
  literal_ptr->integer = i;
  return obj;
}

Obj *obj_new_literal_sym(ObjPool *p, const char *sym) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = OBJ_AS_PTR(obj, literal);
  literal_ptr->kind = Literal_Sym;
  literal_ptr->symbol = sym;
  return obj;
}

Obj *obj_new_empty_expr_list(ObjPool *p) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_List;
  ObjList *list_ptr = OBJ_AS_PTR(obj, list);
  list_ptr->nodes = NULL; // XXX: nodes
  list_ptr->count = 0;
  return obj;
}

Obj *obj_expr_list_append(Obj *obj, Obj *item) {
  size_t count = OBJ_AS(obj, list).count;
  Obj **new_objs = OBJ_AS(obj, list).nodes; // XXX: nodes
  if (REALLOC_N(new_objs, count + 1))       // TODO: FIX
    die(LOCATION);
  new_objs[count] = item;
  ObjList *list_ptr = OBJ_AS_PTR(obj, list);
  list_ptr->nodes = new_objs; // XXX: nodes
  list_ptr->count++;
  return obj;
}

Obj *obj_new_call(ObjPool *p, const PrimOp *prim, Obj *args) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_Call;
  ObjCall *call_ptr = OBJ_AS_PTR(obj, call);
  call_ptr->prim = prim;
  call_ptr->args = args;
  return obj;
}

Obj *obj_new_closure(ObjPool *p, Obj *params, Obj *body) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_Closure;
  ObjClosure *closure_ptr = OBJ_AS_PTR(obj, closure);
  closure_ptr->params = params;
  closure_ptr->body = body;
  closure_ptr->env = NULL;
  return obj;
}

Obj *obj_new_if(ObjPool *p, Obj *then, Obj *else_) {
  Obj *obj = obj_pool_alloc(p);
  OBJ_KIND(obj) = Obj_If;
  ObjIf *if_ptr = OBJ_AS_PTR(obj, if_);
  if_ptr->then = then;
  if_ptr->else_ = else_;
  return obj;
}

void obj_fprintf(FILE *restrict stream, const Obj *obj) {
  if (obj == NULL) {
    fprintf(stream, "NULL");
    return;
  }

  switch (OBJ_KIND(obj)) {
  case Obj_Literal:
    switch (OBJ_AS(obj, literal).kind) {
    case Literal_Int:
      fprintf(stream, "%d", OBJ_AS(obj, literal).integer);
      break;
    case Literal_Sym:
      fprintf(stream, "%s", OBJ_AS(obj, literal).symbol);
      break;
    default:
      fprintf(stream, "<unknown literal>");
      break;
    }
    break;

  case Obj_List:
    fprintf(stream, "(");
    for (unsigned int i = 0; i < OBJ_AS(obj, list).count; ++i) {
      if (i > 0) {
        fprintf(stream, " ");
      }
      obj_fprintf(stream, OBJ_AS(obj, list).nodes[i]);
    }
    fprintf(stream, ")");
    break;

  case Obj_Call:
    fprintf(stream, "<call ");
    if (OBJ_AS(obj, call).prim) {
      fprintf(stream, "%p ", (void *)OBJ_AS(obj, call).prim);
    } else {
      fprintf(stream, "NULL ");
    }
    obj_fprintf(stream, OBJ_AS(obj, call).args);
    fprintf(stream, ">");
    break;

  case Obj_Closure:
    fprintf(stream, "<closure params=");
    obj_fprintf(stream, OBJ_AS(obj, closure).params);
    fprintf(stream, " body=");
    obj_fprintf(stream, OBJ_AS(obj, closure).body);
    fprintf(stream, ">");
    break;

  default:
    fprintf(stream, "<unknown object>");
    break;
  }
}
