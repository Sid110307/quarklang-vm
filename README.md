# QuarkLang VM

> The QuarkLang Virtual Machine.

## Getting Started

### Building from source

- Clone the repository:

  - Git Clone via HTTPS:

    ```sh
    $ git clone https://github.com/Sid110307/quarklang-vm.git
    # Cloning into 'quarklang-vm'...
    ```

  - Git Clone via SSH:

    ```sh
    $ git clone git@github.com:Sid110307/quarklang-vm.git
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
$ make
# Building interpreter... Done.
# Building compiler... Done.
```

### Installation

- To install for the whole system:

```sh
$ make install
# Installing for system...
```

- To install for the current user:

```sh
$ make install-user
# Installing for user 'user'...
```

### Compiling and running the code

- To compile a QuarkLang Assembly file (`.qas`) into a QuarkLang Compiled Executable (`.qce`) file, run the following command:

```sh
$ quarki -f <source.qas>
# Or
$ quarki --file <source.qas>
```

- To run a QuarkLang Compiled Executable (`.qce`) file, run the following command:

```sh
$ quarkc -f <output.qce>
# Or
$ quarkc --file <output.qce>
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

### Fibonacci

```x86asm
put 0
put 1
dup 1
dup 1
plus
jmp 2
```
