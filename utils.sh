#!/usr/bin/env bash

install_vim() {
  sudo mkdir -p "$HOME/.vim/pack/plugins/start/"
  sudo cp -r "plugins/vim-qas/" "$HOME/.vim/pack/plugins/start/"

  if ! grep -q "autocmd BufRead,BufNewFile *.qas set filetype=qas" "$HOME/.vimrc"; then
    echo "autocmd BufRead,BufNewFile *.qas set filetype=qas" >>"$HOME/.vimrc"
  fi

  dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (Vim)." 0 0
}

install_neovim() {
  sudo mkdir -p "$HOME/.local/share/nvim/site/pack/plugins/start/"
  sudo cp -r "plugins/vim-qas/" "$HOME/.local/share/nvim/site/pack/plugins/start/"

  if ! grep -q "autocmd BufRead,BufNewFile *.qas set filetype=qas" "$HOME/.config/nvim/init.vim"; then
    echo "autocmd BufRead,BufNewFile *.qas set filetype=qas" >>"$HOME/.config/nvim/init.vim"
  elif ! grep -q "vim.cmd(autocmd BufRead,BufNewFile *.qas set filetype=qas)" "$HOME/.config/nvim/init.lua"; then
    echo "-- QAS" >>"$HOME/.config/nvim/init.lua"
    echo "vim.cmd(autocmd BufRead,BufNewFile *.qas set filetype=qas)" >>"$HOME/.config/nvim/init.lua"
  fi

  dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (Neovim)." 0 0
}

install_emacs() {
  sudo mkdir -p "$HOME/.emacs.d/elisp"
  sudo cp -r "plugins/emacs-qas/qas-mode.el" "$HOME/.emacs.d/elisp/qas-mode.el"

  if ! grep -q "qas-mode" "$HOME/.emacs"; then
    echo "(add-to-list 'load-path \"$HOME/.emacs.d/elisp\")" >>"$HOME/.emacs"
    echo "(load \"qas-mode.el\")" >>"$HOME/.emacs"
  fi

  dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (Emacs)." 0 0
}

install_vscode() {
  if ! command -v vsce &>/dev/null; then
    dialog --title "QuarkLang VM Extension Installer" --msgbox "vsce is not installed. Press OK to install it." 0 0
    sudo npm install -g vsce
  fi

  cd ./plugins/vscode-qas || {
    dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation failed (Plugin not found)." 0 0
    return
  }

  vsce package -o "vscode-qas.vsix" &>/dev/null
  code --install-extension "vscode-qas.vsix"
  cd ../../ || return

  dialog --title "QuarkLang VM Extension Installer" --msgbox "Installation complete (VS Code)." 0 0
}

if [ "$1" == "extensions" ]; then
  CMD=(dialog --title "QuarkLang VM Extension Installer" --menu "Choose the editor to install:" 0 0 0)
  OPTIONS=(1 "Vim" 2 "Neovim" 3 "Emacs" 4 "Visual Studio Code")
  CHOICES=$("${CMD[@]}" "${OPTIONS[@]}" 2>&1 >/dev/tty)

  for EDITOR in $CHOICES; do
    case "$EDITOR" in
    1) install_vim ;;
    2) install_neovim ;;
    3) install_emacs ;;
    4) install_vscode ;;
    *) dialog --title "QuarkLang VM Extension Installer" --msgbox "No editor was selected.\n\nExit?" 0 0 ;;
    esac
  done

  clear
elif [ "$1" == "bash" ]; then
  if ! grep -q "export PATH=\"/usr/local/quark:$$PATH\"" "$HOME/.bashrc"; then
    echo "# add quarklang vm to PATH" | sudo tee -a "$HOME/.bashrc"
    echo "export PATH=\"/usr/local/quark:$$PATH\"" | sudo tee -a "$HOME/.bashrc"
  fi
fi
