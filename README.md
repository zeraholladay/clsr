# clsr: Mini Lisp-like Interpreted Language with Closures

A Lisp-dialect.

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

## Types

- NIL
- Symbols (i.e. a variable name)
- Integers
- Closure or lambda
- Primitive functions (C functions)
- List (i.e. a Node)

## Primitives (or Operators)

### `T, NIL, symbols, and integers`
Examples:

```lisp
T    ; primitive symbol true
NIL  ; primitive NIL false
'foo ; symbol foo
'bar ; symbol bar
42   ; number 42
```

---

### `QUOTE x`
Returns the unevaluated expressioin.

Examples:

```lisp
'foo            ; foo
(quote foo)     ; foo
'(foo)          ; (foo)
(quote '(foo))  ; (QUOTE foo)
```

---

### `SET x y`
Sets a symbol to a value in the current context.

Example:

```lisp
(set 'foo 42) ; 42
foo           ; 42
```

---

### `CONS x y`
Constructs a list from two arguments.

Example:

```lisp
(cons 'foo '(bar))  ; (foo bar)
(cons 'foo 'bar)    ; (foo.bar)
```

---

### `LIST arg1 arg2 ... argN`
Creates a proper list (one ending in NIL cell).

Example:

```lisp
(list 'foo '(bar) 42) ; (foo (bar) 42)
```

---

### `FIRST x & REST x`
Returns the first or the rest of a list.

Example:

```lisp
(first '(foo bar))  ; foo
(rest '(foo bar))   ; (bar)
```

---

### `CLOSURE '(arg0 arg1 ... argN) '(body)`
Create and returns a **closure** from a function `body` and its **captured environment `env`**.

```lisp
(closure '(a b) '(cons a (cons b '()))) ; creates an anonymous closure

(set 'foofn 
      (closure '(a b c) 
        '(cons a (cons b (cons c '())))
        )
)
(foofn 1 2 '3)  ; call a closure named foofn

((closure '(a b c) 
  '(cons a (cons b (cons c '()))))
  1 2 3) ; call an anonymous closure with args 1 2 3
```

---

### `APPLY fn arglist`
Applies arguments to a primitive function or closure.

```lisp
(apply set '(a 42))
(apply first '((a 42))) ; a
(apply rest '((a 42)))  ; (42)
```

---

### `FUNCALL fn arg1 arg2 ... argN`
Calls arguments to a function.

```lisp
(funcall set 'a 42)     ; a is 42
(funcall first '(a 42)) ; a
; 
```

---

### `LEN x`
Length of a list.

```lisp
(len '(1 2 3 4 5))
; 5
```
---

### `PAIR x y`
Pairs two lists:

```lisp
(pair '(1 2 3 4 5) 
      '(a b c d e))
```

---

### `EVAL x`
Evaluates an expression:

```lisp
(set 'a 42)
(eval ''a)                  ; 42
(eval '(cons 'foo '(bar)))  ; (foo bar)
```

---

### `EQ`
Returns `T` or `NIL` if arguments are equal:

Example:

```lisp
(eq T T)            ; T/true
(eq T NIL)          ; NIL/false
(eq '() '())        ; T
(eq '(dog) '(dog))  ; NIL
(eq (len '(a b c)) 
    (len '(1 2 3))) ; T
```

---

### `PRINT x`
Prints an argument:

Example:

```lisp
(print 'foo) ; foo and returns T
```

---

## TODO

In no particular order:

1. ~~funcall~~
1. ~~Fix eval and tests.~~
1. Math
1. ~~Conditionals~~
1. ~~Fix T/NIL~~
1. ~~Equality~~
1. Exceptions
1. ~~Error status and messages~~
1. Memory management (tracking)
1. I/O subsystem
1. ~~Refactor Primitives to Enum~~
1. Docs/README.md
1. Test list
1. Test heaplist
1. ~~Repr/str is a mess~~
1. Define/def
1. ~~List function~~
1. Strings (started kind of)
1. Pool to arena (will need fix to yyparse)
1. Parse errors (exceptions first)
1. GC
1. repl I/O from files
1. ~~Bug squashing~~
1. Max symbol size
1. Rename `set` to `let`
1. `map-car`
