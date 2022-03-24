;;; qas-mode.el --- Major mode for editing QuarkLang Assembly source files -*- lexical-binding:t -*-

;; Copyright 2021-Present Siddharth Praveen Bharadwaj

;; Permission is hereby granted, free of charge, to any person
;; obtaining a copy of this software and associated documentation
;; files (the "Software"), to deal in the Software without
;; restriction, including without limitation the rights to use,
;; copy, modify, merge, publish, distribute, sublicense, and/or
;; sell copies of the Software, and to permit persons to whom
;; the Software is furnished to do so, subject to the following
;; conditions:

;; The above copyright notice and this permission notice shall
;; be included in all copies or substantial portions of the
;; Software.

;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
;; KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;; WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
;; AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;; WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
;; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
;; OTHER DEALINGS IN THE SOFTWARE.

(defconst qas-mode-syntax-table
	(with-syntax-table (copy-syntax-table)
		(modify-syntax-entry ?\; "<")
		(modify-syntax-entry ?\n ">")
		(modify-syntax-entry ?\" "\"")
		(modify-syntax-entry ?\' "\"")
		(syntax-table))
	"Syntax table for `qas-mode'.")

(eval-and-compile
	(defconst qas-instructions
		'("put" "kaput" "dup" "jmp" "jif" "stop"
		"print")))

(eval-and-compile
	(defconst qas-operators
		'("plus" "minus" "mul" "div" "mod" "eq"
		"lt" "gt" "le" "ge")))

(eval-and-compile
	(defconst qas-keywords
		'()))

(defconst qas-highlights
	`(("%[[:word:]_]+" . font-lock-preprocessor-face)
		("[[:word:]_]+\\:" . font-lock-constant-face)
		(,(regexp-opt qas-instructions 'symbols) . font-lock-keyword-face)
		(,(regexp-opt qas-operators 'symbols) . font-lock-builtin-face)
		(,(regexp-opt qas-keywords 'symbols) . font-lock-preprocessor-face)))

;;;###autoload
(define-derived-mode qas-mode prog-mode "QAS"
  "Major mode for editing QuarkLang Assembly source files."
  (setq font-lock-defaults '(qas-highlights))
  (set-syntax-table qas-mode-syntax-table))

;;;###autoload
(add-to-list 'auto-mode-alist '("\\.qas\\'" . qas-mode))

(provide 'qas-mode)