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

## License

[MIT](https://opensource.org/licenses/MIT)
