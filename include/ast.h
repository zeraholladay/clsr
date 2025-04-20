#ifndef AST_H
#define AST_H

#include "common.h"
#include "prim_op.h"

typedef enum { AST_Literal, AST_List, AST_Call, AST_Closure } ASTNodeType;

typedef enum {
  Literal_Int,
  Literal_Sym,
} LiteralType;

typedef struct {
  LiteralType type;
  union {
    int integer;
    const char *symbol;
  } as;
} Literal;

typedef struct ASTNode {
  ASTNodeType type;

  union {
    Literal literal;

    struct {
      struct ASTNode **nodes;
      unsigned int count;
    } list;

    struct {
      const PrimOp prim;
      struct ASTNode *args;
    } call;

    struct {
      struct ASTNode *params;
      struct ASTNode *body;
    } closure;
  } as;
} ASTNode;

ASTNode *ast_new_literal_int(int i);
ASTNode *ast_new_literal_sym(const char *sym);

ASTNode *ast_new_expr_list(ASTNode **items, size_t count);
ASTNode *ast_expr_list_append(ASTNode *list, ASTNode *item);

ASTNode *ast_new_call(const char *op_name, ASTNode *args);

ASTNode *ast_new_closure(ASTNode *params, ASTNode *body);

#endif