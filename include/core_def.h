#ifndef CORE_DEF_H
#define CORE_DEF_H

#include <limits.h>
#include <stddef.h>

#include "env.h"
#include "eval_ctx.h"
#include "palloc.h"
#include "prim_fn.h"

#define IS_TYPE(node, kind) ((node) != NULL && (node)->type == (kind))

#define IS_NIL(node)             IS_TYPE((node), TYPE_NIL)
#define IS_SYMBOL(node)          IS_TYPE((node), TYPE_SYMBOL)
#define IS_INTEGER(node)         IS_TYPE((node), TYPE_INTEGER)
#define IS_STRING(node)          IS_TYPE((node), TYPE_STRING)
#define IS_LIST(node)            IS_TYPE((node), TYPE_LIST)
#define IS_PRIMITIVE_FN(node)    IS_TYPE((node), TYPE_PRIMITIVE_FN)
#define IS_CLOSURE(node)         IS_TYPE((node), TYPE_CLOSURE)
#define GET_SYMBOL(node)         ((node)->as.symbol)
#define GET_INTEGER(node)        ((node)->as.integer)
#define GET_STRING(node)         ((node)->as.string)
#define GET_LIST(node)           (&(node)->as.list)
#define GET_PRIMITIVE_FN(node)   ((node)->as.primitive)
#define GET_CLOSURE(node)        (&(node)->as.closure)
#define GET_CLOSURE_PARAMS(node) ((node)->as.closure.params)
#define GET_CLOSURE_BODY(node)   ((node)->as.closure.body)
#define GET_CLOSURE_ENV(node)    ((node)->as.closure.env)

struct Node;
typedef struct Node Node;

// Node type "object"
typedef char *(*StrFn)(Node *);
typedef int (*EqFn)(Node *, Node *);

typedef struct Type {
  const char *type_name;
  StrFn str_fn;
  EqFn eq_fn;
} Type;

// Nodes
typedef enum {
  TYPE_NIL,          // special constant
  TYPE_SYMBOL,       // identifiers
  TYPE_INTEGER,      // literal
  TYPE_STRING,       // literal
  TYPE_LIST,         // cons cells
  TYPE_PRIMITIVE_FN, // builtin fn
  TYPE_CLOSURE       // user-defined fn
} TypeEnum;

typedef long long Integer;

typedef struct {
  Node *first;
  Node *rest;
} List;

typedef struct {
  Node *params;
  Node *body;
  Env *env;
} Closure;

struct Node {
  TypeEnum type;
  union {
    // Literal values
    Integer integer;
    char *string;
    const char *symbol;

    // Composite structures
    List list;

    // Function-like values
    const PrimitiveFn *primitive;
    Closure closure;
  } as;
};

// coredef.c
const Type *type(Node *self);
Node *cons_primfn(Pool *p, const PrimitiveFn *prim_fn);
Node *cons_closure(Pool *p, Node *params, Node *body, Env *env);
Node *cons_integer(Pool *p, Integer i);
Node *cons_list(Pool *p, Node *car, Node *cdr);
Node *cons_string(Pool *p, char *str);
Node *cons_symbol(Pool *p, const char *sym);

#endif
