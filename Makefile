CFLAGS=-std=c11 -pedantic -Wall -Wextra -Wswitch-enum -Wmissing-prototypes
LIBS=-lm

EXAMPLES=$(shell find examples -name "*.qas" | sed 's/\.qas/.qce/g' | tr '\n' ' ')

.PHONY: all
all: clean interpreter compiler disassembler nanboxer

help:
	@echo "\033[1mUsage\033[0m: make <target> [-s | --silent]"
	@echo "\033[1mRun with -s or --silent to hide the output commands (\033[0;34mRECOMMENDED\033[0m)\033[0m\n"
	@echo "Available targets:"
	@echo "\033[1;36m  all\033[0m: Compile all targets."
	@echo "\033[1;36m  interpreter\033[0m: Build the interpreter."
	@echo "\033[1;36m  compiler\033[0m: Build the compiler."
	@echo "\033[1;36m  disassembler\033[0m: Build the disassembler."
	@echo "\033[1;36m  nanboxer\033[0m: Build the NaN boxer."
	@echo "\033[1;36m  examples\033[0m: Compile examples."
	@echo "\033[1;36m  clean\033[0m: Remove all compiled files (\033[1;31mWARNING\033[0m: This will also remove the interpreter and compiler binaries, if installed previously)."
	@echo "\033[1;36m  install\033[0m: Install the binaries to the system."
	@echo "\033[1;36m  install-user\033[0m: Install the binaries to the user's home directory."
	@echo "\033[1;36m  ext-install\033[0m: Install the extensions/plugins for an editor."
	@echo "\033[1;36m  help\033[0m: Show this help message and exit."

interpreter: src/quarki.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding interpreter... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/quarki $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

compiler: src/quarkc.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding compiler... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/quarkc $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

disassembler: src/unquark.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding disassembler... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/unquark $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

nanboxer: src/nanbox.c src/include/compiler.h
	@echo -n "\033[1;36mBuilding NaN boxer... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/nanbox $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

.PHONY: examples
examples: $(EXAMPLES)

examples/%.qce: interpreter examples/%.qas
	@echo -n "\033[1;36mBuilding example \033[0m$(word 2, $^)\033[1;36m...\033[0m\n"
	./bin/quarki -f $(word 2, $^)
	./bin/quarkc -f $@

install:
	@echo -n "\033[1;36mInstalling binaries... \033[0m"

	sudo mkdir -p /usr/local/quark
	sudo cp bin/quarki /usr/local/quark/quarki
	sudo cp bin/quarkc /usr/local/quark/quarkc
	sudo cp bin/unquark /usr/local/quark/unquark
	sudo cp bin/nanbox /usr/local/quark/nanbox

	if ! grep -q 'export PATH="/usr/local/quark:$$PATH"' $(HOME)/.bashrc; then\
		sudo echo '# add quarklang vm to PATH' >> $(HOME)/.bashrc;\
		sudo echo 'export PATH="/usr/local/quark:$$PATH"' >> $(HOME)/.bashrc;\
	fi

	@echo "\033[1;32mDone.\033[0m"
	@echo "\033[1;36mReload your terminal or run 'source ~/.bashrc' to use the new binaries.\033[0m"

install-user:
	@echo -n "\033[1;36mInstalling binaries for user $(USER)... \033[0m"

	sudo mkdir -p $(HOME)/.local/share/quark
	sudo cp bin/quarki $(HOME)/.local/share/quark/quarki
	sudo cp bin/quarkc $(HOME)/.local/share/quark/quarkc
	sudo cp bin/unquark $(HOME)/.local/share/quark/unquark
	sudo cp bin/nanbox $(HOME)/.local/share/quark/nanbox

	if ! grep -q 'export PATH="$(HOME)/.local/share/quark:$$PATH"' $(HOME)/.bashrc; then\
		sudo echo '# add quarklang vm to PATH' >> $(HOME)/.bashrc;\
		sudo echo 'export PATH="$$HOME/.local/share/quark:$$PATH"' >> $(HOME)/.bashrc;\
	fi

	@echo "\033[1;32mDone.\033[0m"
	@echo "\033[1;36mReload your terminal or run 'source ~/.bashrc' to use the new binaries.\033[0m"

ext-install:
	./extensionInstaller.sh

clean:
	@echo -n "\033[1;36mCleaning up... \033[0m"
	sudo rm -rf bin/*
	sudo rm -rf /usr/local/quark
	sudo rm -rf $(HOME)/.local/share/quark
	@echo "\033[1;32mDone.\033[0m"
