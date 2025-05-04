#include <stdio.h>
#include <stdlib.h>

#include "clsr.h"

Obj *obj_new_literal_int(Pool *p, int i) {
  Obj *obj = pool_alloc(p);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = OBJ_AS_PTR(obj, literal);
  literal_ptr->kind = Literal_Int;
  literal_ptr->integer = i;
  return obj;
}

Obj *obj_new_literal_sym(Pool *p, const char *sym) {
  Obj *obj = pool_alloc(p);
  OBJ_KIND(obj) = Obj_Literal;
  ObjLiteral *literal_ptr = OBJ_AS_PTR(obj, literal);
  literal_ptr->kind = Literal_Sym;
  literal_ptr->symbol = sym;
  return obj;
}

Obj *obj_new_empty_expr_list(Pool *p) {
  Obj *obj = pool_alloc(p);
  OBJ_KIND(obj) = Obj_List;
  ObjList *obj_list = obj_list_new();
  if (!obj_list) {
    return NULL;
  }
  OBJ_AS(obj, list) = obj_list;
  return obj;
}

Obj *obj_expr_list_append(Obj *obj, Obj *item) {
  ObjList *obj_list = OBJ_AS(obj, list);
  int fail = obj_list_append(obj_list, item);
  if (fail) {
    return NULL;
  }
  return obj;
}

Obj *obj_new_call(Pool *p, const PrimOp *prim, Obj *args) {
  Obj *obj = pool_alloc(p);
  OBJ_KIND(obj) = Obj_Call;
  ObjCall *call_ptr = OBJ_AS_PTR(obj, call);
  call_ptr->prim = prim;
  call_ptr->args = args;
  return obj;
}

Obj *obj_new_closure(Pool *p, Obj *params, Obj *body) {
  Obj *obj = pool_alloc(p);
  OBJ_KIND(obj) = Obj_Closure;
  ObjClosure *closure_ptr = OBJ_AS_PTR(obj, closure);
  closure_ptr->params = params;
  closure_ptr->body = body;
  closure_ptr->env = NULL;
  return obj;
}

Obj *obj_new_if(Pool *p, Obj *then, Obj *else_) {
  Obj *obj = pool_alloc(p);
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
      fprintf(stream, "%p::Int::%d", obj, OBJ_AS(obj, literal).integer);
      break;
    case Literal_Sym:
      fprintf(stream, "%p::Symbol::%s", obj, OBJ_AS(obj, literal).symbol);
      break;
    case Literal_Keywrd:
      fprintf(stream, "%p::Keyword::%s", obj, OBJ_AS(obj, literal).symbol);
      break;
    default:
      fprintf(stream, "<unknown literal>");
      break;
    }
    break;

  case Obj_List:
    fprintf(stream, "(");
    for (unsigned int i = 0; i < OBJ_AS(obj, list)->count; ++i) {
      if (i > 0) {
        fprintf(stream, " ");
      }
      obj_fprintf(stream, OBJ_AS(obj, list)->nodes[i]);
    }
    fprintf(stream, ")");
    break;

  case Obj_Call:
    fprintf(stream, "<%p::call ", obj);
    if (OBJ_AS(obj, call).prim) {
      fprintf(stream, "%p ", (void *)OBJ_AS(obj, call).prim);
    } else {
      fprintf(stream, "NULL ");
    }
    obj_fprintf(stream, OBJ_AS(obj, call).args);
    fprintf(stream, ">");
    break;

  case Obj_Closure:
    fprintf(stream, "<%p::closure params=", obj);
    obj_fprintf(stream, OBJ_AS(obj, closure).params);
    fprintf(stream, " body=");
    obj_fprintf(stream, OBJ_AS(obj, closure).body);
    fprintf(stream, ">");
    break;

  case Obj_If:
    fprintf(stream, "<%p::if then=", obj);
    obj_fprintf(stream, OBJ_AS(obj, if_).then);
    fprintf(stream, " else=");
    obj_fprintf(stream, OBJ_AS(obj, if_).else_);
    fprintf(stream, ">");
    break;

  default:
    fprintf(stream, "<unknown object>");
    break;
  }
}
