#include <assert.h>

#include "core_def.h"
#include "list.h"
#include "node_alloc.h"

// static Node _true = {.kind = Node_Literal,
//                         .as.literal = {
//                             .kind = Literal_Keywrd,
//                             .symbol = "True",
//                         }};
// static Node _false = {.kind = Node_Literal,
//                          .as.literal = {
//                              .kind = Literal_Keywrd,
//                              .symbol = "False",
//                          }};

// Node *const Node_true = &_true;
// Node *const Node_false = &_false;

// Node *apply(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node = CTX_POP(ctx);

//   if (!Node_IS_CLOSURE(Node)) {
//     assert(0 && "Node is not a closure.");
//     return Node_false; // TODO: error message
//   }

//   Closure closure = Node_AS(Node, closure);

//   jList *params = Node_AS(closure.params, list);

//   Env *closure_env = env_new(closure.env); // TODO: error handling

//   for (unsigned int i = 0; i < params->count; ++i) {
//     Node *o = CTX_POP(ctx);
//     const char *symbol = Node_AS(params->nodes[i], literal).symbol;
//     env_set(closure_env, symbol, o);
//   }

//   ENTER_FRAME(CTX_STACK(ctx));

//   Context new_ctx = *ctx;
//   CTX_ENV(&new_ctx) = closure_env;

//   return eval(closure.body,
//               &new_ctx); // does not EXIT_FRAME (ie force RETURN)
// }

// Node *closure(Node *Node, Context *ctx) {
//   if (!Node_IS_CLOSURE(Node)) {
//     assert(0 && "Node is not a closure.");
//     return Node_false; // TODO: error handling
//   }

//   Closure *closure = Node_AS_PTR(Node, closure);
//   closure->env = CTX_ENV(ctx);
//   CTX_PUSH(ctx, Node);
//   return Node_true;
// }

// Node *eq(Node *Node, Context *ctx) {
//   Node *Node1 = CTX_POP(ctx);
//   Node *Node2 = CTX_POP(ctx);

//   // Literal ints are special
//   if (Node_IS_LITERAL_INT(Node1) && Node_IS_LITERAL_INT(Node2)) {
//     Node *result = Node_AS(Node1, literal).integer == Node_AS(Node2, literal).integer
//                       ? Node_true
//                       : Node_false;
//     CTX_PUSH(ctx, result);
//     return Node_true;
//   }

//   // Symbols, keyword literals, lists, calls, closures, ifs, etc.
//   // if we get lists, a comparison would be cool.
//   CTX_PUSH(ctx, Node2);
//   CTX_PUSH(ctx, Node1);
//   return is(Node, ctx);
// }

// Node *if_(Node *Node, Context *ctx) {
//   if (!Node_IS_IF(Node)) {
//     assert(0 && "Node is not an if.");
//     return Node_false; // TODO: error handling
//   }

//   Node *cond = CTX_POP(ctx);

//   if (!cond) {
//     return Node_false; // TODO: error handling
//   }

//   NodeIf Node_if = Node_AS(Node, if_);

//   Node *branch = (cond == Node_true) ? Node_if.then : Node_if.else_;

//   return eval(branch, ctx);
// }

// Node *is(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node1 = CTX_POP(ctx);
//   Node *Node2 = CTX_POP(ctx);

//   Node *result = NULL;

//   // symbols are globally unique
//   if (Node_IS_LITERAL_SYM(Node1) && Node_IS_LITERAL_SYM(Node1)) {
//     const char *sym1 = Node_AS(Node1, literal).symbol;
//     const char *sym2 = Node_AS(Node2, literal).symbol;
//     result = (uintptr_t)sym1 == (uintptr_t)sym2 ? Node_true : Node_false;
//   } else
//     result = (uintptr_t)Node1 == (uintptr_t)Node2 ? Node_true : Node_false;

//   CTX_PUSH(ctx, result);

//   return Node_true;
// }

// Node *lookup(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *key = CTX_POP(ctx);

//   if (!Node_IS_LITERAL_SYM(key)) {
//     assert(0 && "Node is not a literal symbol.");
//     return Node_false; // TODO: error handling
//   }

//   void *rval;

//   if (env_lookup(CTX_ENV(ctx), Node_AS(key, literal).symbol, &rval))
//     return Node_false;
//   else
//     CTX_PUSH(ctx, rval);

//   return Node_true;
// }

// Node *push(Node *Node, Context *ctx) {
//   NodeCall Node_call = Node_AS(Node, call);
//   jList *list = Node_AS(Node_call.args, list);

//   for (unsigned int i = list->count; i > 0; --i) {
//     assert(Node_ISKIND(list->nodes[i - 1], Node_Literal));
//     CTX_PUSH(ctx, list->nodes[i - 1]);
//   }

//   return Node_true;
// }

// Node *quote(Node *Node, Context *void_ctx) {
//   (void)void_ctx;
//   jList *list = Node_AS(Node, list);
//   assert(list->count == 2);
//   return list->nodes[1];
// }

// Node *return_(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *Node_rval = CTX_POP(ctx);
//   EXIT_FRAME(CTX_STACK(ctx));
//   CTX_PUSH(ctx, Node_rval);

//   return Node_true;
// }

// Node *set(Node *void_Node, Context *ctx) {
//   (void)void_Node;

//   Node *key = CTX_POP(ctx);
//   Node *val = CTX_POP(ctx);

//   if (!Node_IS_LITERAL_SYM(key)) {
//     assert(0 && "Node is not a literal symbol.");
//     return Node_false; // TODO: error handling
//   }

//   env_set(CTX_ENV(ctx), Node_AS(key, literal).symbol,
//           val); // TODO: error handling

//   return Node_true;
// }

Node *eval(Node *expr, Context *ctx) {
  if (is_literal(expr)) {
    return expr;
  }

  if (is_symbol(expr)) {
    return lookup(expr, ctx);
  }

  if (is_call(expr)) {
  }

  if (is_list(expr)) {
    if (is_empty_list(expr)) {
      return expr; // NIL or '()
    }

    Node *op = first(expr);
    Node *args = rest(expr);

    Node *fn = eval(op, ctx);
    Node *evaluated_args = eval_list(args, ctx);

    return apply(fn, evaluated_args, ctx); // XXX
  }

  error("Cannot evaluate expression");
  return NULL;
}

Node *eval_list(Node *list, Context *ctx) {
  if (is_empty_list(list))
    return empty_list();

  Node *first_result = eval(first(list), ctx);
  Node *rest_results = eval_list(rest(list), ctx);

  return cons(first_result, rest_results);
}

Node *eval_program(Node *program, Context *ctx) {
  List *expressions = program->as.list;
  Node *result = false;

  for (unsigned int i = 0; i < expressions->count; ++i) {
    result = eval(expressions->items[i], ctx);
  }

  return result;
}
