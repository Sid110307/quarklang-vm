CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=

.PHONY: all

all: interpreter compiler

help:
	@echo "\033[1mUsage\033[0m: make <target> [-s | --silent]\n"
	@echo "Available targets:"
	@echo "\033[1;36m  interpreter\033[0m: Compile the interpreter."
	@echo "\033[1;36m  compiler\033[0m: Compile the compiler."
	@echo "\033[1;36m  all\033[0m: Compile all targets."
	@echo "\033[1;36m  clean\033[0m: Remove all compiled files (\033[1;31mWARNING\033[0m: This will also remove the interpreter and compiler binaries)."
	@echo "\033[1;36m  install\033[0m: Install the binaries to the system."
	@echo "\033[1;36m  install-user\033[0m: Install the binaries to the user's home directory."
	@echo "\033[1;36m  help\033[0m: Show this help message and exit."

interpreter: src/quarki.c src/compiler.c
	@echo -n "\033[1;36mBuilding interpreter... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/quarki $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

compiler: src/quarkc.c src/compiler.c
	@echo -n "\033[1;36mBuilding the compiler... \033[0m"
	sudo mkdir -p bin
	sudo $(CC) $(CFLAGS) -o bin/quarkc $< $(LIBS)
	@echo "\033[1;32mDone.\033[0m"

install:
	@echo -n "\033[1;36mInstalling binaries... \033[0m"

	sudo mkdir -p /usr/local/quark
	sudo cp bin/quarki /usr/local/quark/quarki
	sudo cp bin/quarkc /usr/local/quark/quarkc

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

	if ! grep -q 'export PATH="$(HOME)/.local/share/quark:$$PATH"' $(HOME)/.bashrc; then\
		sudo echo '# add quarklang vm to PATH' >> $(HOME)/.bashrc;\
		sudo echo 'export PATH="$$HOME/.local/share/quark:$$PATH"' >> $(HOME)/.bashrc;\
	fi

	@echo "\033[1;32mDone.\033[0m"
	@echo "\033[1;36mReload your terminal or run 'source ~/.bashrc' to use the new binaries.\033[0m"

clean:
	@echo -n "\033[1;36mCleaning up... \033[0m"
	sudo rm -rf bin/*
	sudo rm -rf /usr/local/quark;
	@echo "\033[1;32mDone.\033[0m"
