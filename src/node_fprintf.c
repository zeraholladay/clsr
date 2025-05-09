#include <stdio.h>

#include "core_def.h"

void node_fprintf(FILE *stream, const Node *node) {
  if (!node) {
    fprintf(stream, "NULL");
    return;
  }

  switch (node->kind) {
  case KIND_LITERAL:
    switch (node->as.literal.kind) {
    case LITERAL_INTEGER:
      fprintf(stream, "Int(%d)", node->as.literal.as.integer);
      break;
    case LITERAL_SYMBOL:
      fprintf(stream, "Sym(%s)", node->as.literal.as.symbol);
      break;
    // case LITERAL_KEYWORD:
    //   fprintf(stream, "Keywrd(%s)", node->as.literal.as.symbol);
    //   break;
    default:
      fprintf(stream, "UnknownLiteral");
      break;
    }
    break;

  case KIND_LIST: {
    fprintf(stream, "(");
    const Node *cur = node;
    while (cur && cur->kind == KIND_LIST) {
      if (cur->as.list.car == NULL && cur->as.list.cdr == NULL) {
        // fprintf(stream, "NIL");
      } else {
        node_fprintf(stream, cur->as.list.car);
        if (cur->as.list.cdr && cur->as.list.cdr->kind == KIND_LIST) {
          fprintf(stream, " ");
        }
      }
      cur = cur->as.list.cdr;
    }
    if (cur) {
      fprintf(stream, " . ");
      node_fprintf(stream, cur);
    }
    fprintf(stream, ")");
    break;
  }

  case KIND_FUNCTION:
    switch (node->as.function.kind) {
    case FN_PRIMITIVE:
      fprintf(stream, "<primitive %p>",
              (void *)node->as.function.as.primitive.prim_op);
      break;
    case FN_CLOSURE:
      fprintf(stream, "<closure params=");
      node_fprintf(stream, node->as.function.as.closure.params);
      fprintf(stream, " body=");
      node_fprintf(stream, node->as.function.as.closure.body);
      fprintf(stream, ">");
      break;
    default:
      fprintf(stream, "<unknown function>");
      break;
    }
    break;

  default:
    fprintf(stream, "<unknown node>");
    break;
  }
}
