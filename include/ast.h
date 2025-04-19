#ifndef AST_H
#define AST_H

#include "object.h"
#include "prim_op.h"

typedef enum {
    AST_LITERAL,     // An integer literal or interned symbol reference
    AST_LIST,        // A list of AST nodes (used for args, body)
    AST_CALL         // An operation and its arguments
    AST_PRIM,        // A primitive operator: PUSH, SET, etc.
} ASTNodeType;

// typedef enum {
//     VAL_INT,
//     VAL_SYMBOL
// } ValueType;

// typedef struct {
//     ValueType type;
//     union {
//         int i;
//         const char *symbol;
//     };
// } Value;

typedef struct ASTNode {
    ASTNodeType type;

    union {
        Object literal;              // for AST_LITERAL
        PrimOp *prim;

        struct {
            struct ASTNode *op;     // AST_PRIM node
            struct ASTNode *args;   // AST_LIST node
        } call;

        struct {
            struct ASTNode **items; // list of ASTNode*
            unsigned int count;
        } list;
    } as;
} ASTNode;

ASTNode *ast_new_literal_int(int i);
ASTNode *ast_new_literal_sym(const char *symbol);
ASTNode *ast_new_prim(const char *name);
ASTNode *ast_new_call(ASTNode *prim, ASTNode *args);
ASTNode *ast_new_list(ASTNode **items, unsigned int count);

void ast_free(ASTNode *node);

#endif