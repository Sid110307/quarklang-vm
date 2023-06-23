# QuarkLang VM

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Makefile CI: Build Status](
https://github.com/Sid110307/quarklang-vm/actions/workflows/makefile.yml/badge.svg?branch=master)

> The QuarkLang Virtual Machine.
>
> QuarkLang is a simple, stack-based, assembly-like language.

## Getting Started

### Prerequisites

- [Git](https://git-scm.com/)
- A C compiler ([GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/), etc.)
- [GNU Make](https://www.gnu.org/software/make/)

### Building from source

- Clone the repository:

    - Via HTTPS:

        ```sh
        $ git clone https://github.com/Sid110307/quarklang-vm.git
        ```

    - GitHub CLI:

        ```sh
        $ gh repo clone Sid110307/quarklang-vm
        ```

- Enter the directory:

```sh
$ cd quarklang-vm
```

- Run `make`:

```sh
$ make -s
```

### Installation

- To install for the whole system:

```sh
$ make install -s
```

- To install for the current user:

```sh
$ make install-user -s
```

### Compiling and running programs

- To compile a QuarkLang Assembly file (`.qas`) into a QuarkLang Compiled Executable (`.qce`) file, run `quarki` with a
  file argument:

```sh
$ quarki -f <source.qas>
# Or
$ quarki --file <source.qas>
```

- To run a QuarkLang Compiled Executable (`.qce`) file, run `quarkc` with a file argument:

```sh
$ quarkc -f <output.qce>
# Or
$ quarkc --file <output.qce>
```

### Decompiler/Disassembler

- To view the assembly code of a `.qce` file, run `unquark` with a file argument:

```sh
$ unquark -f <source.qce>
# Or
$ unquark --file <source.qce>
```

## Debugging

- There is a built-in debugger that can be used to debug QuarkLang programs.
- Use the `-d` or `--debug` flag in the compiler to enable the debugger.

```sh
$ quarkc -d -f <source.qas>
# Or
$ quarkc --debug -f <source.qas>
```

## Examples

Check the [examples](examples) folder for examples.

Run them with `make examples -s`.

## Docs

### Comments

- Single-line comments start with `--`.
- Example:

```lua
-- This is a comment
```

### Instructions

| Instruction | Description                                                                                    | Arguments |
|-------------|------------------------------------------------------------------------------------------------|-----------|
| `kaput`     | Does nothing                                                                                   | 0         |
| `put`       | Pushes a value onto the stack                                                                  | 1         |
| `dup`       | Duplicates a value on the stack to the top                                                     | 1         |
| `swap`      | Swaps a value on the stack with the value on top                                               | 1         |
| `release`   | Pops the value on top of the stack                                                             | 0         |
|             |                                                                                                |           |
| `iplus`     | Adds the top two values on the stack and pushes the result                                     | 0         |
| `iminus`    | Subtracts the top two integers on the stack and pushes the result                              | 0         |
| `imul`      | Multiplies the top two integers on the stack and pushes the result                             | 0         |
| `idiv`      | Divides the top two integers on the stack and pushes the result                                | 0         |
| `imod`      | Calculates the modulus of the top two integers on the stack and pushes the result              | 0         |
|             |                                                                                                |           |
| `fplus`     | Adds the top two floats on the stack and pushes the result                                     | 0         |
| `fminus`    | Subtracts the top two floats on the stack and pushes the result                                | 0         |
| `fmul`      | Multiplies the top two floats on the stack and pushes the result                               | 0         |
| `fdiv`      | Divides the top two floats on the stack and pushes the result                                  | 0         |
| `fmod`      | Calculates the modulus of the top two floats on the stack and pushes the result                | 0         |
|             |                                                                                                |           |
| `jump`      | Jumps to the specified function label                                                          | 1         |
| `jif`       | Jumps to the specified function label if the top value on the stack is true                    | 1         |
| `return`    | Returns from the current function                                                              | 0         |
| `invoke`    | Calls a function                                                                               | 1         |
| `native`    | Calls a [native](#native-functions) function                                                   | 1         |
|             |                                                                                                |           |
| `ieq`       | Checks if the top two integers on the stack are equal and pushes the result                    | 0         |
| `ineq`      | Checks if the top two integers on the stack are not equal and pushes the result                | 0         |
| `igt`       | Checks if the top two integers on the stack are greater than and pushes the result             | 0         |
| `ilt`       | Checks if the top two integers on the stack are less than and pushes the result                | 0         |
| `ige`       | Checks if the top two integers on the stack are greater than or equal to and pushes the result | 0         |
| `ile`       | Checks if the top two integers on the stack are less than or equal to and pushes the result    | 0         |
|             |                                                                                                |           |
| `feq`       | Checks if the top two floats on the stack are equal and pushes the result                      | 0         |
| `fneq`      | Checks if the top two floats on the stack are not equal and pushes the result                  | 0         |
| `fgt`       | Checks if the top two floats on the stack are greater than and pushes the result               | 0         | 
| `flt`       | Checks if the top two floats on the stack are less than and pushes the result                  | 0         |
| `fge`       | Checks if the top two floats on the stack are greater than or equal to and pushes the result   | 0         |
| `fle`       | Checks if the top two floats on the stack are less than or equal to and pushes the result      | 0         |
|             |                                                                                                |           |
| `halt`      | Halts the program                                                                              | 0         |

### Native Functions

| Function | Description                                                                         |
|----------|-------------------------------------------------------------------------------------|
| `0`      | Allocates a block of memory of the specified size (located in the top of the stack) | 
| `1`      | Frees a block of memory (located in the top of the stack)                           |
| `2`      | Prints a value as a float (located in the top of the stack) to stdout               |
| `3`      | Prints a value as an integer (located in the top of the stack) to stdout            |
| `4`      | Prints a value as a pointer (located in the top of the stack) to stdout             |

### Exceptions

| Exception                     | Description                |
|-------------------------------|----------------------------|
| EX_OK                         | All good                   |                         
| EX_STACK_OVERFLOW             | Stack overflow             |
| EX_STACK_UNDERFLOW            | Stack underflow            |
| EX_INVALID_INSTRUCTION        | Invalid instruction        |
| EX_ILLEGAL_INSTRUCTION_ACCESS | Illegal instruction access |
| EX_ILLEGAL_OPERATION          | Illegal operation          |
| EX_DIVIDE_BY_ZERO             | Dividing by zero           |

## License

[MIT](https://opensource.org/licenses/MIT)
