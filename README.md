# Mini Stack-Based VM Instructions

This virtual machine uses a simple stack-based architecture with support for closures, lexical environments, and basic function application.

## Instructions

### `PUSH x`
Push a variable `x` onto the stack.

- `x` may refer to:
  - A variable name (resolved later via `LOOKUP`)
  - A literal value (if literals are supported in your extension)

---

### `LOOKUP i`
Look up a variable at **depth `i`** in the current environment chain and push its value onto the stack.

- Environment depth is zero-indexed:
  - `0` = current frame
  - `1` = immediate parent
  - `2` = grandparent, etc.

---

### `CLOSURE body, env`
Create a **closure** from a function `body` and its **captured environment `env`**, and push it onto the stack.

- The closure captures the lexical environment for future execution.

---

### `APPLY`
Pop a **function** and an **argument** from the stack and invoke the function with the argument.

- Creates a new stack frame for the function call.
- Can be used to apply either a user-defined closure or a built-in.

---

### `RETURN`
Return from the current function call.

- Pops the current stack frame.
- Pushes the result value back onto the previous frame’s stack.

---

## Example Execution (Conceptual)

Given an expression like:

```
PUSH 42
CLOSURE <body>, <env>
APPLY
RETURN
```
