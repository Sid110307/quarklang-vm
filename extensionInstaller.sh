#!/usr/bin/env bash

CMD=(dialog --title "QuarkLang VM Extension Installer" --menu "Choose the editor to install:" 0 0 0)

OPTIONS=(
	1 "Vim"
	2 "Emacs"
	3 "Visual Studio Code"
)

CHOICES=$("${CMD[@]}" "${OPTIONS[@]}" 2>&1 >/dev/tty)

for EDITOR in $CHOICES; do
	if [ $EDITOR == 1 ]; then
		sudo mkdir -p $HOME/.vim/syntax
		sudo cp plugins/vim-qas.vim $HOME/.vim/syntax/qas.vim

		if ! grep -q "autocmd BufRead,BufNewFile *.qas set filetype=qas" $HOME/.vimrc; then
			echo "autocmd BufRead,BufNewFile *.qas set filetype=qas" >>$HOME/.vimrc
		fi

		dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (Vim)." 0 0
	elif [ $EDITOR == 2 ]; then
		dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation for Emacs is not implemented yet :(" 0 0
	elif [ $EDITOR == 3 ]; then
		if ! command -v vsce &>/dev/null; then
			dialog --title "QuarkLang VM Extension Installer" --msgbox "vsce is not installed. Press OK to install it." 0 0
			sudo npm install -g vsce

			cd plugins/vscode-qas
			vsce package &>/dev/null
			code --install-extension *.vsix
			cd ../../

			dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (VS Code)." 0 0
		else
			cd plugins/vscode-qas
			vsce package &>/dev/null
			code --install-extension *.vsix
			cd ../../

			dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (VS Code)." 0 0
		fi
	else
		dialog --title "QuarkLang VM Extension Installer" --msgbox "No editor was selected.\n\nExiting." 0 0
	fi
done

clear
