#include "ast.h"

ASTNode *ast_new_literal_int(int i);
ASTNode *ast_new_literal_sym(const char *sym);

ASTNode *ast_new_expr_list(ASTNode **items, size_t count);
ASTNode *ast_expr_list_append(ASTNode *list, ASTNode *item);

ASTNode *ast_new_call(const char *op_name, ASTNode *args);

ASTNode *ast_new_closure(ASTNode *params, ASTNode *body);
