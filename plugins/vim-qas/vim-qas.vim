" Vim syntax file
" Language: QuarkLang Assembly

" Usage:
" Put this file in .vim/syntax/qas.vim
" and add the following line to your .vimrc:
" autocmd BufRead,BufNewFile *.qas set filetype=qas

if exists("b:current_syntax")
	finish
endif

syntax keyword quarkVMTodos TODO XXX FIXME NOTE HACK BUG
syntax keyword quarkVMKeywords put kaput dup jmp jif stop print
syntax keyword quarkVMOperators plus minus mul div mod eq lt gt le ge

syntax match quarkVMNumeric "[0-9]\+$"
syntax match quarkVMFunction "\v[a-zA-Z0-9_]+\:$"

syntax region quarkVMCommentLine start="--" end="$" contains=quarkVMTodos

syntax region quarkVMString start=/\v"/ skip=/\v\\./ end=/\v"/
syntax region quarkVMString start=/\v'/ skip=/\v\\./ end=/\v'/

highlight default link quarkVMTodos Todo
highlight default link quarkVMKeywords Keyword
highlight default link quarkVMOperators Operator
highlight default link quarkVMNumeric Number
highlight default link quarkVMFunction Function
highlight default link quarkVMCommentLine Comment
highlight default link quarkVMString String

let b:current_syntax = "qas"