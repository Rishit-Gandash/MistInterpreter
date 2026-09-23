# MistInterpreter

MistInterpreter is a small interpreter written in C. It tokenizes and parses
integer/boolean expressions, loads a line-oriented scripting language, and
executes assignments, labels, jumps, conditional jumps, printing, and sleeps.

The repository also includes a Raylib example that drives a simple animated
scene from a Mist script.

## Project layout

| Path | Purpose |
| --- | --- |
| `interpreter/interpreter.c` | Lexer, Pratt parser, statement loader, hash maps, and evaluator/runtime. |
| `interpreter/interpreter.h` | Public data structures and declarations. |
| `script.txt` | Small command-line script example. |
| `ray.txt` | Script used by the Raylib animation example. |
| `main.c` | Raylib host that steps the interpreter once per frame. |
| `tests/lexer`, `tests/parser` | Lexer and parser test cases. |

## Build and test

The lexer and parser tests are driven by `test.sh` and require a C compiler and
Python 3:

```sh
sh test.sh
```

This is the intended test command: it builds executables under `build/` and
runs `test.py`. In the current checkout it stops at link time because
`interpreter/interpreter.c` also defines a standalone `main`, which conflicts
with the `main` functions in both test programs. Moving or conditionally
compiling that standalone entry point will restore the test path.

To build the Raylib demo, install Raylib and run:

```sh
gcc main.c -o main -lraylib
./main
```

The demo currently loads `script.txt`; change the filename in `main.c` to
`ray.txt` to use the animation-oriented script.

## Language reference

Scripts are line-oriented. Blank lines are ignored, and `;` begins a comment.
Labels begin with a period and are not executable statements.

```mist
; a comment
counter = 0
running = true

.loop
print counter
counter = counter + 1
if counter < 10 jump .loop
```

### Values and expressions

The runtime supports integers and booleans (`true`, `false`). Variable names
may contain letters, digits, and underscores, but must not start with a digit.

Supported expression features are:

| Category | Operators / syntax |
| --- | --- |
| Grouping | `(expression)` |
| Unary | `-value`, `!value` |
| Arithmetic | `+`, `-`, `*`, `/` |
| Comparison | `<`, `<=`, `>`, `>=` |
| Equality | `==`, `!=` |

Operator precedence, from highest to lowest, is unary operators, multiplication
and division, addition and subtraction, then comparisons/equality. Arithmetic
and ordered comparisons require integers; equality requires both operands to
have the same type.

### Statements

| Form | Meaning |
| --- | --- |
| `name = expression` | Assign or update a variable. |
| `.label` | Define a jump target. |
| `jump .label` | Transfer control to a label. |
| `if condition jump .label` | Jump when a boolean condition is true. |
| `print name` | Print a variable's integer or boolean value. If no variable exists, the name itself is printed. |
| `sleep expression` | Pause using an integer expression. |

For example:

```mist
x = 4
y = x * (10 - 2)
if y >= 32 jump .done
print y
.done
print true
```

## Embedding the interpreter

The implementation provides functions to initialize from a file, run a whole
script, step a script, and access integer variables:

```c
Interpreter *interpreter = new_interpreter_from_file("script.txt");
run_interpreter(interpreter);

/* Or integrate it into an event loop. */
step_interpreter(interpreter);
int value = get_var(interpreter, "counter");
set_var(interpreter, "counter", 42);
```

At present these runtime functions are implemented in `interpreter.c` but are
not all declared in `interpreter.h`; the Raylib host includes the implementation
file directly. Exporting their declarations and building the implementation as
a separate object would make this a conventional embeddable C API.

## Current limitations

This is a learning/prototype project and several rough edges are visible in the
current implementation:

- The runtime is limited to integers and booleans; it has no strings, functions,
  arrays, or compound boolean operators.
- The fixed-size, linear-probing variable/label maps have a capacity of 4096
  entries and do not support deletion.
- Error handling prints a message and terminates the process; source locations
  are not reported.
- The parser and interpreter allocate AST/runtime data without a complete
  teardown path, so it is best suited to short-lived processes at present.
- `sleep` converts a millisecond-like expression through `unistd.h`'s
  second-granularity `sleep`, so sub-second values are currently truncated.
