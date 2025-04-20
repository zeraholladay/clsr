#include "ast.h"

ASTNode *ast_new_literal_int(int i) {
  ASTNode *n = NULL;
  if (ALLOC(n))
    die(LOCATION);
  n->type = AST_Literal;
  n->as.literal.kind = Literal_Int;
  n->as.literal.as.integer = i;
  return n;
}

ASTNode *ast_new_literal_sym(const char *sym) {
  ASTNode *n = NULL;
  if (ALLOC(n))
    die(LOCATION);
  n->type = AST_Literal;
  n->as.literal.kind = Literal_Sym;
  n->as.literal.as.symbol = sym;
  return n;
}

ASTNode *ast_new_empty_expr_list(void) {
  ASTNode *n = NULL;
  if (ALLOC(n))
    die(LOCATION);
  n->type = AST_List;
  n->as.list.nodes = NULL;
  n->as.list.count++;
  return n;
}

ASTNode *ast_expr_list_append(ASTNode *list, ASTNode *item) {
  size_t count = list->as.list.count;
  ASTNode **new_nodes = list->as.list.nodes;
  if (REALLOC_N(new_nodes, count + 1))
    die(LOCATION);
  new_nodes[count] = item;
  list->as.list.nodes = new_nodes;
  list->as.list.count++;
  return list;
}

ASTNode *ast_new_call(const PrimOp *prim, ASTNode *args) {
  ASTNode *n = NULL;
  if (ALLOC(n))
    die(LOCATION);
  n->type = AST_Call;
  n->as.call.prim = prim;
  n->as.call.args = args;
  return n;
}

ASTNode *ast_new_closure(ASTNode *params, ASTNode *body) {
  ASTNode *n = NULL;
  if (ALLOC(n))
    die(LOCATION);
  n->type = AST_Closure;
  n->as.closure.params = params;
  n->as.closure.body = body;
  return n;
}
