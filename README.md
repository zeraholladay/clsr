# clsr: Mini Lisp-like Interpreted Language with Closures

A simple stack-based interpreted language with support for closures, lexical environments, and basic function application.

## Build and Test

Build:

```bash
make clean all
```

Test:

```bash
make clean test
```

Debug:

```
env DEBUG=1 make clean test all
```

## Atoms

- A *symbol* (i.e. a variable name to be resolved later via lookup)
- A literal value (e.g. integers or `long longs`)

## Primitives (or Operators)

### `symbols and integers`
Disables evaluation

Example:

```lisp
'foo ; symbol foo
'bar ; symbol bar
42   ; number 42
```

---

### `QUOTE`
Disables evaluation

Example:

```lisp
'(foo)
```

---

### `SET`
Binds a symbol in the current context.

Example:

```lisp
(set 'foo 42)
```

---

### `CONS`
Constructs a list.

Example:

```lisp
(cons 'foo '(bar))
```

---

### `FIRST & REST`
Returns the first element on the list or the rest of the emelments on the list.

Example:

```lisp
(first '(foo bar)) ; foo
(first '(foo bar)) ; bar
```

---

### `CLOSURE '(arg0 arg1 ... argN) '(body)`
Create and returns a **closure** from a function `body` and its **captured environment `env`**.

```lisp
(closure '(a b) '(cons a (cons b '())))

(set 'foofn 
      (closure '(a b c) 
        '(cons a (cons b (cons c '())))
      )
)
(foofn 1 2 '3)

```

---

### `APPLY`
Applies arguments to a function.

```lisp
(apply foofn '(a b c))
```

Example with closure named `foo`:

```lisp
(apply rest '((a b)))
```

---

### `LEN`
Lenth of a list.

```lisp
(len '(1 2 3 4 5))
```
---

### `PAIR`
Pairs two lists:

```lisp
(pair '(1 2 3 4 5) '(a b c d e))
```

## TODO

1. Math
1. Conditionals
1. Equality
1. Error status and messages
1. Memory management (tracking)
1. I/O subsystem
1. Refactor PrimOps to Enum
1. Docs/README.md
1. Testing/tests
1. Repr/str is a mess
1. Define/def
1. List operator
1. Strings
1. Pool to arena (will need fix to yyparse)
1. Parse errors
1. GC
1. repl I/O from files
1. Bug squashing
