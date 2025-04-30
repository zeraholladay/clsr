# clsr: Mini Stack-Based Interpreted Language with Closures

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
- A literal value (e.g. integers)

## Primitives (or Operators)

### `push (arg0 arg1 ... argN)`
Pushes one or more atoms onto the stack.

This is a variadic producer instruction — it accepts zero or more arguments and *produces* values by pushing them onto the stack.

Example:

```lisp
push (a b c)
; STACK:
; a
; b
; c
```

---

### `set`
Assigns a value to a symbol in the current environment.

A pure consumer that establishes a new variable binding. This operation does not return a value — it only mutates the environment.

Example:

```lisp
push (a 1)
; STACK:
; a
; 1
set ; env={ a=1 }
```

---

### `lookup`
A pure *producer* instruction.

Produces: the value bound to that symbol in the environment.

The original symbol is discarded, and the resolved value is pushed in its place.

Example:

```lisp
push (a 1) set ; env={ a=1 }
push (a)
; STACK:
; a
lookup
; STACK:
; 1
```

---

### `closure (arg0 arg1 ... argN) (expression)`
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
closure (a b c) (
  push (bar)
  return
) push (foo)
; foo
; (closure)
set  ; env={ foo=(closure) }
```

---

### `apply`
Applies a closure to a sequence of argument values by evaluating its body in a new *stack frame*, with each argument bound to the corresponding parameter.

A consumer + producer instruction:

Consumes:
1. N arguments, already on the stack above the closure (right-to-left).
1. A closure object (on bottom of the stack).

Produces:
1. The result of evaluating the closure with its parameters bound to the arguments.

Side effect:
1. pushes a new *stack frame*.

Example with anonymous clousre:

```lisp
push (1 2 3) closure (a b c) ( return )
; STACK:
;   (closure)
;   3
;   2
;   1
apply
```

Example with closure named `foo`:

```lisp
closure (a b c) (
  push (bar)
  return
) push (foo) set
push (foo 1 2 3)
lookup ; lookup foo
; STACK:
;   (closure)
;   1
;   2
;   3
apply
```

---

### `return`
Ends evaluation of the current function or closure and returns a value to the caller.

A consumer + producer instruction that:

Consumes:
1. A single value from the top of the current *stack frame* (the return value)

Produces:
1. That value, pushed onto the previous *stack frame*

Side effect:
1. Pops the current *stack frame*, restoring the caller’s environment

---

### `if`
A conditional instruction that pops the top value from the stack and evaluates **either** the `(then)` expression **or** the `(else)` expression, depending on whether the value is true.

Consumes:
1. A single `true` or `false` from the top of the *stack*.

Produces:
1. Dependent on the side effect.

Side effect:
1. Evaluates either the `(then)` expression otherwise the `(else)` expression

```lisp
push (true) if (push(42)) (push(-1)) ; 42

push (false) if (push(-1)) (push(42)) ; 42
```

---
