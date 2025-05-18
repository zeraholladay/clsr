#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "heap_list.h"
#include "safe_str.h"
#include "types.h"

#define CINTEGER_TYPE_FMT "%lld"

#define LOG10_2 0.30103

#define INTEGER_TYPE_STR_MAX_SIZE                                             \
  ((size_t)(sizeof (Integer) * CHAR_BIT * LOG10_2 + 3))

// Type eq
static inline int
type_eq (Node *self, Node *other)
{
  return type (self) == type (other);
}

// NIL type
static int
nil_eq (Node *self, Node *other)
{
  (void)self;
  (void)other;
  return self == other;
}

static char *
nil_tostr (Node *self)
{
  (void)self;
  return STR_LITERAL_DUP ("NIL");
}

// Integer type
static int
integer_eq (Node *self, Node *other)
{
  return type_eq (self, other) && GET_INTEGER (self) == GET_INTEGER (other);
}

static char *
integer_tostr (Node *self)
{
  char str[INTEGER_TYPE_STR_MAX_SIZE];
  size_t n = sizeof (str);

  int result = snprintf (str, n, CINTEGER_TYPE_FMT, GET_INTEGER (self));

  if (result < 0 || (size_t)result >= n)
    return NULL;

  return safe_strndup (str, n);
}

// Symbol type
static int
symbol_eq (Node *self, Node *other)
{
  return type_eq (self, other) && GET_SYMBOL (self) == GET_SYMBOL (other);
}

static char *
symbol_tostr (Node *self)
{
  const char *str = GET_SYMBOL (self);
  return safe_strndup (str, strlen (str));
}

// List type
static int
list_eq (Node *self, Node *other)
{
  return type_eq (self, other)
         && ((IS_NIL (self) && IS_NIL (other))
             || GET_LIST (self) == GET_LIST (other));
}

static char *
list_tostr (Node *self)
{
  HeapList *hl = NULL;
  size_t total = 0;
  Node *cur;

  hl = hl_alloc ();

  if (!hl)
    return NULL;

  total += hl_append_strdup (hl, "(");

  for (cur = self; IS_LIST (cur); cur = REST (cur))
    {
      Node *car = FIRST (cur), *cdr = REST (cur);

      if (car)
        {
          total += hl_append_strdup (hl, type (car)->str_fn (car));

          if (FIRST (cdr))
            total += hl_append_strdup (hl, " ");
        }
    }

  if (!IS_NIL (cur))
    {
      total += hl_append_strdup (hl, ".");
      total += hl_append_strdup (hl, type (cur)->str_fn (cur));
    }

  total += hl_append_strdup (hl, ")");

  // merge down into a single str

  char *str = calloc (total + 1, sizeof *(str));
  char *dst = str;

  if (!str)
    return NULL;

  for (size_t i = 0; i < hl->count; ++i)
    {
      for (char *src = hl->items[i]; (*dst = *src); ++src, ++dst)
        ;
      free (hl->items[i]);
    }

  hl_free (hl);

  return str;
}

// Primitive type
static int
primitive_eq (Node *self, Node *other)
{
  return type_eq (self, other)
         && GET_PRIMITIVE (self) == GET_PRIMITIVE (other);
}

static char *
primitive_tostr (Node *self)
{
  const Primitive *prim = GET_PRIMITIVE (self);
  return safe_strndup (prim->name, strlen (prim->name));
}

// Lambda type
static int
lambda_eq (Node *self, Node *other)
{
  return type_eq (self, other) && GET_LAMBDA (self) == GET_LAMBDA (other);
}

static char *
lambda_tostr (Node *self)
{
  const char *fmt = "(#LAMBDA %s %s)";

  char *params_str
      = type (GET_LAMBDA_PARAMS (self))->str_fn (GET_LAMBDA_PARAMS (self));
  char *body_str = type (GET_LAMBDA_BODY (self))
                       ->str_fn (GET_LAMBDA_BODY (self)); // FIXME

  size_t total = strlen (fmt) + NULLABLE_STRLEN (params_str)
                 + NULLABLE_STRLEN (body_str);

  char *str = calloc (total, sizeof *str);

  if (!str)
    return NULL;

  int result = snprintf (str, total, fmt, params_str, body_str);
  if (result < 0 || (size_t)result >= total)
    {
      free (str);
      return NULL;
    }

  free (params_str);
  free (body_str);

  return str;
}

// String type
static int
string_eq (Node *self, Node *other)
{
  return type_eq (self, other)
         && (!strcmp (GET_STRING (self),
                      GET_STRING (other))); // FIX ME: strings should have len
}

static char *
string_tostr (Node *self)
{
  return GET_STRING (self);
}

static Type type_tab[] = {
  // Special constant
  [TYPE_NIL] = { .type_name = "NIL", .str_fn = nil_tostr, .eq_fn = nil_eq },

  // Literal values
  [TYPE_INTEGER]
  = { .type_name = "INTEGER", .str_fn = integer_tostr, .eq_fn = integer_eq },
  [TYPE_STRING]
  = { .type_name = "STRING", .str_fn = string_tostr, .eq_fn = string_eq },
  [TYPE_SYMBOL]
  = { .type_name = "SYMBOL", .str_fn = symbol_tostr, .eq_fn = symbol_eq },

  // Composite structures
  [TYPE_LIST]
  = { .type_name = "List", .str_fn = list_tostr, .eq_fn = list_eq },

  // Function-like values
  [TYPE_PRIMITIVE] = { .type_name = "PRIMITIVE",
                       .str_fn = primitive_tostr,
                       .eq_fn = primitive_eq },
  [TYPE_LAMBDA]
  = { .type_name = "LAMBDA", .str_fn = lambda_tostr, .eq_fn = lambda_eq },
};

// type()
const Type *
type (Node *self)
{
  if (!self || IS_NIL (self))
    return &type_tab[TYPE_NIL];

  return &type_tab[self->type];
}

Node *
cons_prim (Pool **p, const Keyword *keyword)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_PRIMITIVE;
  node->as.primitive = keyword;
  return node;
}

Node *
cons_lambda (Pool **p, Node *params, Node *body, Env *env)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_LAMBDA;
  node->as.lambda.params = params;
  node->as.lambda.body = body;
  node->as.lambda.env = env;
  return node;
}

Node *
cons_integer (Pool **p, Integer i)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_INTEGER;
  node->as.integer = i;
  return node;
}

Node *
cons_list (Pool **p, Node *car, Node *cdr)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_LIST;
  node->as.list.first = car;
  node->as.list.rest = cdr;
  return node;
}

Node *
cons_string (Pool **p, char *str)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_STRING;
  node->as.string = str;
  return node;
}

Node *
cons_symbol (Pool **p, const char *sym)
{
  Node *node = pool_alloc_hier (p);
  node->type = TYPE_SYMBOL;
  node->as.symbol = sym;
  return node;
}
