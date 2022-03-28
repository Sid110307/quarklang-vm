# QuarkLang VM

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![Makefile CI: Build Status](
https://github.com/Sid110307/quarklang-vm/actions/workflows/makefile.yml/badge.svg?branch=master)

> The QuarkLang Virtual Machine.

## Getting Started

### Building from source

- Clone the repository:

  - Git Clone via HTTPS:

      ```sh
      $ git clone https://github.com/Sid110307/quarklang-vm.git
      # Cloning into 'quarklang-vm'...
      ```

  - GitHub CLI:

      ```sh
      $ gh repo clone Sid110307/quarklang-vm
      # Cloning into 'quarklang-vm'...
      ```

- Enter the directory:

```sh
$ cd quarklang-vm
# user@host:~/quarklang-vm$
```

- Run `make`:

```sh
$ make -s
# Building interpreter... Done.
# Building compiler... Done.
```

### Installation

- To install for the whole system:

```sh
$ make install -s
# Installing for system...
```

- To install for the current user:

```sh
$ make install-user -s
# Installing for user 'user'...
```

### Compiling and running the code

- To compile a QuarkLang Assembly file (`.qas`) into a QuarkLang Compiled Executable (`.qce`) file, run `quarki` with a file argument:

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

Check the [examples](https://github.com/Sid110307/quarklang-vm/tree/master/examples) folder for examples.

Run them with `make examples -s`.

## [License](https://github.com/Sid110307/quarklang-vm/tree/master/LICENSE)

Licensed under the [MIT](https://opensource.org/licenses/MIT) license.
