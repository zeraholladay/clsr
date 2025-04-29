#ifndef CLSR_H
#define CLSR_H

#include "common.h"
#include "env.h"
#include "stack.h"

#ifndef OBJ_POOL_CAPACITY
#define OBJ_POOL_CAPACITY 4096
#endif

/* primitive operations */

struct Obj;
struct EvalContext;

typedef struct Obj *(*PrimFunc)(struct Obj *obj, struct EvalContext *ctx);

typedef enum {
  PrimOp_Apply,
  PrimOp_Closure,
  PrimOp_If,
  PrimOp_Lookup,
  PrimOp_Push,
  PrimOp_Return,
  PrimOp_Set
} PrimOpCode;

typedef struct PrimOp {
  int tok;
  PrimOpCode op_code;
  PrimFunc prim_func;
} PrimOp;

/* objects */

#define OBJ_AS(obj_ptr, kind) ((obj_ptr)->as.kind)
#define OBJ_AS_PTR(obj_ptr, kind) (&((obj_ptr)->as.kind))
#define OBJ_KIND(obj_ptr) (obj_ptr)->kind
#define OBJ_ISKIND(obj_ptr, kind) (OBJ_KIND(obj_ptr) == kind)

typedef enum {
  Obj_Literal,
  Obj_List,
  Obj_Call,
  Obj_Closure,
  Obj_If,
} ObjKind;

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
  Env *env;
} ObjClosure;

typedef struct ObjIf {
  struct Obj *then;
  struct Obj *else_;
} ObjIf;

typedef struct Obj {
  ObjKind kind;

  union {
    ObjLiteral literal;
    ObjList list;
    ObjCall call;
    ObjClosure closure;
    ObjIf if_;
  } as;
} Obj;

/* object pool */

typedef struct ObjPoolWrapper {
  struct ObjPoolWrapper *next_free;
  Obj obj;
} ObjPoolWrapper;

typedef struct {
  ObjPoolWrapper *free_list;
  ObjPoolWrapper *pool;
  unsigned int count;
} ObjPool;

/* eval context */

typedef struct EvalContext {
  Stack *stack; // one stack per EvalContext
  Env *env;
} EvalContext;

/* prim_op.gperf */
const PrimOp *prim_op_lookup(register const char *str,
                             register unsigned int len);

/* stack.c (stack helpers) */

#define STACK_INIT(stack) stack_init(stack, STACK_GROWTH)
#define STACK_FREE(stack) stack_free(stack)
#define PUSH(stack, obj) stack_push(stack, (void *)obj)
#define POP(stack) (Obj *)stack_pop(stack)
#define PEEK(stack) (Obj *)stack_peek(stack)
#define ENTER_FRAME(stack) stack_enter_frame(stack)
#define EXIT_FRAME(stack) stack_exit_frame(stack)

/* obj.c */

extern Obj *const obj_true;
extern Obj *const obj_false;

void obj_init_reserved_literals(void);
Obj *obj_new_literal_int(ObjPool *p, int i);
Obj *obj_new_literal_sym(ObjPool *p, const char *sym);
Obj *obj_new_empty_expr_list(ObjPool *p);
Obj *obj_expr_list_append(Obj *obj, Obj *item);
Obj *obj_new_call(ObjPool *p, const PrimOp *prim, Obj *args);
Obj *obj_new_closure(ObjPool *p, Obj *params, Obj *body);
Obj *obj_new_if(ObjPool *p, Obj *then, Obj *else_);
void obj_fprintf(FILE *restrict stream, const Obj *obj);

/* obj_pool.c */

ObjPool *obj_pool_init(unsigned int count);
void obj_pool_destroy(ObjPool **p);
Obj *obj_pool_alloc(ObjPool *p);
void obj_pool_free(ObjPool *p, Obj *obj);
void obj_pool_reset(ObjPool *p);

/* prim_func.c */

Obj *apply(Obj *void_obj, EvalContext *ctx);
Obj *closure(Obj *obj, EvalContext *ctx);
Obj *lookup(Obj *void_obj, EvalContext *ctx);
Obj *push(Obj *obj, EvalContext *ctx);
Obj *return_(Obj *void_obj, EvalContext *ctx);
Obj *set(Obj *void_obj, EvalContext *ctx);
Obj *eval(Obj *obj, EvalContext *ctx);

#endif
