#ifndef CLSR_H
#define CLSR_H

#include "common.h"
#include "env.h"
#include "palloc.h"
#include "rb_tree.h"
#include "stack.h"

#ifndef SYMTAB_POOL_COUNT
#define SYMTAB_POOL_COUNT 4096
#endif

#ifndef SYM_SAVE_BUMP_SIZE
#define SYM_SAVE_BUMP_SIZE 4096
#endif

#ifndef OBJ_POOL_COUNT
#define OBJ_POOL_COUNT 4096
#endif

/* primitive operations */

struct Obj;
struct ClsrContext;

extern struct Obj *const obj_true;
extern struct Obj *const obj_false;

typedef struct Obj *(*PrimFunc)(struct Obj *obj, struct ClsrContext *ctx);

typedef struct PrimOp {
  int tok;
  PrimFunc prim_fun;
} PrimOp;

/* objects */

#define OBJ_AS(obj_ptr, kind) ((obj_ptr)->as.kind)
#define OBJ_AS_PTR(obj_ptr, kind) (&((obj_ptr)->as.kind))
#define OBJ_KIND(obj_ptr) (obj_ptr)->kind
#define OBJ_ISKIND(obj_ptr, kind) (OBJ_KIND(obj_ptr) == kind)
#define OBJ_IS_LITERAL_INT(obj_ptr)                                            \
  ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_Literal)) &&                          \
   (OBJ_AS(obj_ptr, literal).kind == Literal_Int))
#define OBJ_IS_LITERAL_KEYWRD(obj_ptr)                                         \
  ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_Literal)) &&                          \
   (OBJ_AS(obj_ptr, literal).kind == Literal_Keywrd))
#define OBJ_IS_LITERAL_SYM(obj_ptr)                                            \
  ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_Literal)) &&                          \
   (OBJ_AS(obj_ptr, literal).kind == Literal_Sym))
#define OBJ_IS_LIST(obj_ptr) ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_List)))
#define OBJ_IS_CALL(obj_ptr) ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_Call)))
#define OBJ_IS_CLOSURE(obj_ptr)                                                \
  ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_Closure)))
#define OBJ_IS_IF(obj_ptr) ((obj_ptr) && (OBJ_ISKIND(obj_ptr, Obj_If)))

typedef enum {
  Obj_Literal,
  Obj_List,
  Obj_Call,
  Obj_Closure,
  Obj_If,
} ObjKind;

typedef enum {
  Literal_Int,
  Literal_Keywrd,
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

/* clsr context */

#define CTX_POOL(ctx) ((ctx)->obj_pool)
#define CTX_ENV(ctx) ((ctx)->eval_ctx.env)
#define CTX_STACK(ctx) ((ctx)->eval_ctx.stack)
#define CTX_SYMTAB(ctx) ((ctx)->parser_ctx.sym_tab)
#define CTX_PARSE_ROOT(ctx) ((ctx)->parser_ctx.root_obj)
#define CTX_PARSE_MARK(ctx) ((ctx)->parser_ctx.parse_mark)
#define CTX_PEEK(ctx) PEEK(CTX_STACK(ctx))
#define CTX_POP(ctx) POP(CTX_STACK(ctx))
#define CTX_PUSH(ctx, obj) PUSH(CTX_STACK(ctx), obj)

typedef struct EvalContext {
  Env *env;
  Stack *stack;
} EvalContext;

typedef struct ParserContext {
  rb_node *sym_tab;
  Obj *root_obj;
  Wrapper *parse_mark;
} ParserContext;

typedef struct ClsrContext {
  Pool *obj_pool;
  EvalContext eval_ctx;
  ParserContext parser_ctx;
} ClsrContext;

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

Obj *obj_new_literal_int(Pool *p, int i);
Obj *obj_new_literal_sym(Pool *p, const char *sym);
Obj *obj_new_empty_expr_list(Pool *p);
Obj *obj_expr_list_append(Obj *obj, Obj *item);
Obj *obj_new_call(Pool *p, const PrimOp *prim, Obj *args);
Obj *obj_new_closure(Pool *p, Obj *params, Obj *body);
Obj *obj_new_if(Pool *p, Obj *then, Obj *else_);
void obj_fprintf(FILE *restrict stream, const Obj *obj);

/* eval.c */

Obj *apply(Obj *void_obj, ClsrContext *ctx);
Obj *closure(Obj *obj, ClsrContext *ctx);
Obj *eq(Obj *obj, ClsrContext *ctx);
Obj *if_(Obj *obj, ClsrContext *ctx);
Obj *is(Obj *obj, ClsrContext *ctx);
Obj *lookup(Obj *void_obj, ClsrContext *ctx);
Obj *push(Obj *obj, ClsrContext *ctx);
Obj *return_(Obj *void_obj, ClsrContext *ctx);
Obj *set(Obj *void_obj, ClsrContext *ctx);
Obj *eval(Obj *obj, ClsrContext *ctx);

/* math.c */

Obj *add(Obj *void_obj, ClsrContext *ctx);
Obj *sub(Obj *void_obj, ClsrContext *ctx);
Obj *mul(Obj *void_obj, ClsrContext *ctx);
Obj *div_(Obj *void_obj, ClsrContext *ctx);

#endif
