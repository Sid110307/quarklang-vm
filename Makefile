CFLAGS=-std=c11 -pedantic
CWARNINGS=-Wall -Wextra -Wno-unused-value -Wno-unused-function
LIBS=-lm

EXAMPLES=$(patsubst %.qas,%.qce,$(wildcard ./examples/*.qas))

.PHONY: all examples
all: interpreter compiler disassembler

help:
	@echo "\033[1mUsage\033[0m: make <target> [-s | --silent]"
	@echo "\033[1mRun with -s or --silent to hide the output commands (\033[1;34mRECOMMENDED\033[0m)\033[0m\n"
	@echo "Available targets:"
	@echo "\033[1;36m  all\033[0m: Compile all targets."
	@echo "\033[1;36m  interpreter\033[0m: Build the interpreter."
	@echo "\033[1;36m  compiler\033[0m: Build the compiler."
	@echo "\033[1;36m  disassembler\033[0m: Build the disassembler."
	@echo "\033[1;36m  examples\033[0m: Run examples."
	@echo "\033[1;36m  clean\033[0m: Remove all compiled files (\033[1;31mWARNING\033[0m: This will also remove the interpreter and compiler binaries, if installed previously)."
	@echo "\033[1;36m  install\033[0m: Install the binaries to the system."
	@echo "\033[1;36m  install-user\033[0m: Install the binaries to the user's home directory."
	@echo "\033[1;36m  ext-install\033[0m: Install the extensions/plugins for an editor."
	@echo "\033[1;36m  help\033[0m: Show this help message and exit."

interpreter: src/quarki.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding interpreter... \033[0m"
	mkdir -p bin
	$(CC) $(CFLAGS) $(CWARNINGS) -o bin/quarki $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

compiler: src/quarkc.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding compiler... \033[0m"
	mkdir -p bin
	$(CC) $(CFLAGS) $(CWARNINGS) -o bin/quarkc $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

disassembler: src/unquark.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding disassembler... \033[0m"
	mkdir -p bin
	$(CC) $(CFLAGS) $(CWARNINGS) -o bin/unquark $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

examples: $(EXAMPLES)

examples/%.qce: interpreter compiler examples/%.qas
	@echo "\033[1;36mRunning example \033[0m$(word 3, $^)\033[1;36m...\033[0m "
	./bin/quarki -f $(word 3, $^) >/dev/null
	./bin/quarkc -f $@

install:
	@echo -n "\033[1;36mInstalling binaries... \033[0m"

	mkdir -p /usr/local/quark
	sudo cp bin/quarki /usr/local/quark/quarki
	sudo cp bin/quarkc /usr/local/quark/quarkc
	sudo cp bin/unquark /usr/local/quark/unquark

	./utils.sh bash
	@echo "\033[1;32mDone.\033[0m"
	@echo "\033[1;36mReload your terminal or run \"source ~/.bashrc\" to use the new binaries.\033[0m"

install-user:
	@echo -n "\033[1;36mInstalling binaries for user $(USER)... \033[0m"

	mkdir -p $(HOME)/.local/share/quark
	sudo cp bin/quarki $(HOME)/.local/share/quark/quarki
	sudo cp bin/quarkc $(HOME)/.local/share/quark/quarkc
	sudo cp bin/unquark $(HOME)/.local/share/quark/unquark

	./utils.sh bash
	@echo "\033[1;32mDone.\033[0m"
	@echo "\033[1;36mReload your terminal or run \"source ~/.bashrc\" to use the new binaries.\033[0m"

ext-install:
	./utils.sh extensions

clean:
	@echo -n "\033[1;36mCleaning... \033[0m"
	sudo rm -rf bin/* /usr/local/quark $(HOME)/.local/share/quark examples/*.qce
	@echo "\033[1;32mDone.\033[0m"
