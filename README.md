# clsr: Mini Stack-Based Interpreted Language with Closures

A simple stack-based interpreted language with support for closures, lexical environments, and basic function application.

## Atoms

- A *symbol* (i.e. a variable name to be resolved later via LOOKUP)
- A literal value (e.g. integers)

## Primitives (or Operators)

### `PUSH (arg0 arg1 ... argN)`
Pushes one or more atoms onto the stack.

This is a variadic producer instruction — it accepts zero or more arguments and *produces* values by pushing them onto the stack.

Example:

```lisp
PUSH (a b c)
; STACK:
; a
; b
; c
```

---

### `SET`
Assigns a value to a symbol in the current environment.

A pure consumer that establishes a new variable binding. This operation does not return a value — it only mutates the environment.

Example:

```lisp
PUSH (a 1)
; STACK:
; a
; 1
SET ; env={ a=1 }
```

---

### `LOOKUP`
A pure *producer* instruction.

Produces: the value bound to that symbol in the environment.

The original symbol is discarded, and the resolved value is pushed in its place.

Example:

```lisp
PUSH (a 1) 
; STACK:
; a
; 1
SET ; env={ a=1 }
PUSH (a)
; STACK:
; a
LOOKUP
; STACK:
; 1
```

---

### `CLOSURE (arg0 arg1 ... argN) (expression)`
Create a **closure** from a function `body` and its **captured environment `env`**, and push it onto the stack.

A pure producer that builds a closure object.

Consumes:
1. A parameter list (list of symbols)
1. A function body (an Obj *)

Produces:
1. A closure that contains:
  - The parameter list
  - The body Obj
  - The current environment, captured by reference (for lexical scope)

Example with cloure named `foo`:

```lisp
CLOSURE (a b c) (
  PUSH (bar)
  RETURN
)
PUSH (foo)
; foo
; (closure)
SET  ; env={ foo=(closure) }
```

---

### `APPLY`
Applies a closure to a sequence of argument values by evaluating its body in a new *stack frame*, with each argument bound to the corresponding parameter.

A consumer + producer instruction:

Consumes:
1. N arguments, already on the stack above the closure (right-to-left).
1. A closure object (on bottom of the stack).

Produces:
1. The result of evaluating the closure with its parameters bound to the arguments.

Side effect:
1. Pushes a new *stack frame*.

Example with anonymous clousre:

```lisp
PUSH (1 2 3)
; STACK:
;   3
;   2
;   1
CLOSURE (a b c) ( RETURN )
; STACK:
;   (closure)
;   3
;   2
;   1
APPLY
```

Example with closure named `foo`:

```lisp
CLOSURE (a b c) (
  PUSH (bar)
  RETURN
)
PUSH (foo)
; STACK:
;   foo
;   (closure)
SET
; env={ foo=closure }
PUSH (1 2 3)
PUSH (foo)
LOOKUP
; STACK:
;   (closure)
;   1
;   2
;   3
APPLY
```

---

### `RETURN`
Ends evaluation of the current function or closure and returns a value to the caller.

A consumer + producer instruction that:

Consumes:
1. A single value from the top of the current *stack frame* (the return value)

Produces:
1. That value, pushed onto the previous *stack frame*

Side effect:
1. Pops the current *stack frame*, restoring the caller’s environment
---
