" Vim/Neovim syntax file
" Language: QuarkLang Assembly

" Copyright 2022-Present Siddharth Praveen Bharadwaj

" Permission is hereby granted, free of charge, to any person
" obtaining a copy of this software and associated documentation
" files (the "Software"), to deal in the Software without
" restriction, including without limitation the rights to use,
" copy, modify, merge, publish, distribute, sublicense, and/or
" sell copies of the Software, and to permit persons to whom
" the Software is furnished to do so, subject to the following
" conditions:

" The above copyright notice and this permission notice shall
" be included in all copies or substantial portions of the
" Software.

" THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
" KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
" WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
" AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
" HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
" WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
" FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
" OTHER DEALINGS IN THE SOFTWARE.

" Usage:
" Put this file in .vim/syntax/qas.vim (.config/nvim/syntax/qas.vim for Neovim)
" and add the following line to your .vimrc:
" autocmd BufRead,BufNewFile *.qas set filetype=qas

if exists("b:current_syntax")
	finish
endif

syntax keyword quarkVMTodos TODO XXX FIXME NOTE HACK BUG
syntax keyword quarkVMKeywords put kaput dup swap release jmp jif return invoke native stop
syntax keyword quarkVMOperators iplus iminus imul idiv imod fplus fminus fmul fdiv fmod
syntax keyword quarkVMComparisons ieq ineq ilt igt ile ige feq fneq flt fgt fle fge

syntax match quarkVMNumeric "[0-9]+\.?[0-9]+$"
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
