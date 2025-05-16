#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stddef.h>

#define KEYWORD(name) keyword_lookup (#name, sizeof (#name) - 1)
#define KEYWORD_LIT(str) keyword_lookup (str, sizeof (str) - 1)

// Forward declarations
struct Node;
struct Context;

typedef struct Keyword
{
  const char *name;
  struct Node *(*fn) (struct Node *, struct Context *);
  int arity, token;
} Keyword;

/* KEYWORD.gperf */
const struct Keyword *keyword_lookup (register const char *str,
                                      register size_t len);
const char *is_keyword_strncmp (const char *text, int state);

#endif
