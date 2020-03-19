;; Sisal code editing commands for Emacs
;;
;; want automatic sisal mode for files ending with .sis
;;
;;
;; This document was prepared as an account of work sponsored by an 
;; agency of the United States Government.  Neither the United States
;; Government nor the University of California nor any of their employees,
;; make any warranty, express or implied, or assumes any legal liability
;; or responsibility for the accuracy, completeness or usefulness of any
;; information, apparatus, product or process disclosed, or represents that
;; its use would not infringe privately owned right.  Reference herein to
;; any specific commercial products, process, or service by trade name,
;; trademark, manufacturer, or otherwise, does not necessarily constitute
;; or imply its endorsement, recommendation, or favoring by the United
;; States Government or the University of California.  The views and 
;; opinions of authors expressed herein do not necessarily state or 
;; reflect those of the United States Government, and shall not be used
;; for advertising or product endorsement purposes.
;; 
;; Work performed under the auspices of the U.S. Department of Energy by
;; Lawrence Livermore National Laboratory under contract number 
;; W-7405-Eng-48.
;; 
;; Copyright 1990 the Regents of the University of California.  All
;; rights reserved.  Permission to use, copy, modify, and distribute this
;; software and its documentation for any purpose and without fee is
;; hereby granted, provided that the above copyright notice appear in all
;; copies.
;;

;; Add this to your .emacs file if sisal-mode.elc is in your load-path
;;(autoload 'sisal-mode "sisal-mode" "Major mode for editing Sisal source." t)
;;
;; Add this to your .emacs files if sisal-mode.elc is NOT in your load-path
;; (e.g. in /whereever/bin)
;;(load-file "/whereever/bin/sisal-mode.elc")

(setq auto-mode-alist
  (append auto-mode-alist (quote (("\\.sis$" . sisal-mode)))))

(defvar sisal-mode-abbrev-table nil
  "Abbrev table in use in sisal-mode buffers.")
(define-abbrev-table 'sisal-mode-abbrev-table ())

(defvar sisal-mode-syntax-table nil
  "syntax table used in sisal mode.")

(defvar sisal-mode-map nil
  "Keymap used in sisal mode.")

(if sisal-mode-map
    ()
  (setq sisal-mode-map (make-sparse-keymap))
  (define-key sisal-mode-map "\177" 'backward-delete-char-untabify)
  (define-key sisal-mode-map "\C-m" 'sisal-newline-indent-command)
  (define-key sisal-mode-map "\t" 'sisal-indent-command))

(if sisal-mode-syntax-table
    ()
  (setq sisal-mode-syntax-table (make-syntax-table))
  (modify-syntax-entry ?\\ "\\" sisal-mode-syntax-table)
  (modify-syntax-entry ?+ "." sisal-mode-syntax-table)
  (modify-syntax-entry ?- "." sisal-mode-syntax-table)
  (modify-syntax-entry ?= "." sisal-mode-syntax-table)
  (modify-syntax-entry ?% "." sisal-mode-syntax-table)
  (modify-syntax-entry ?< "." sisal-mode-syntax-table)
  (modify-syntax-entry ?> "." sisal-mode-syntax-table)
  (modify-syntax-entry ?& "." sisal-mode-syntax-table)
  (modify-syntax-entry ?| "." sisal-mode-syntax-table)
  (modify-syntax-entry ?\' "\"" sisal-mode-syntax-table))

(defconst sisal-left-margin 0
  "Left margin of sisal code.  Usually set to 0")

(defconst sisal-left-item-keyword-list '("define" "global")
  "list of keywords which must be at the left margin (use only lowercase).")

(defconst sisal-block-item-keyword-list 
  '("function" "let" "for" "if" "tagcase")
  "list of keywords which define a block. (use only lowercase)")

(defconst sisal-middle-item-keyword-list
  '("returns" "else" "in" "elseif" "then" "while" "until" "repeat" "tag")
  "list of keywords which might be indented the same as the beginning
of a block but do not begin the block  (e.g. else) (use only lowercase)")

(defconst sisal-end-item-keyword-list 
  '("end")
  "list of keywords which end blocks (use only lowercase)")

;;; Is this used?
(defconst sisal-indent-levels '(5 10 15 20)
  "amount of indenting per level")

(defconst sisal-next-indent-amount 2
  "amount of indenting to the next level")

(defconst sisal-comment-begin "%"
  "character or characters that begin a comment.")

(defconst sisal-comment-end "\n"
  "character or characters that end a comment.")

(defconst sisal-debug nil
  "debugging mode.  Usually set to nil")

(defun sisal-move-to-first-character-on-line ()
  "Moves to the first non-white space character on the line and returns
the position.  If line contains no non-white space, then moves to beginning
of line."
  (interactive)
  (let ((line-end 0)
	(line-begin 0))
    (save-excursion 
      (end-of-line)
      (setq line-end (point)))
    (beginning-of-line)
    (setq line-begin (point))
    (if (re-search-forward "\\S-" line-end line-end)
	(if (> (point) line-begin) 
	    (backward-char 1))
      (beginning-of-line))
    (point)))

(defun sisal-move-cursor (position)
  "Moves the point to the POSITION relative to the first non-white space
character.  Will not move past end-of-line or before beginning of line."
  (interactive "nEnter position to move to:")
  (let ((line-end (progn (end-of-line)
			 (point)))
	(line-begin (progn (beginning-of-line)
			   (point))))
    (goto-char line-begin)
    (sisal-move-to-first-character-on-line)
    (if (> (- line-end (point)) position)
	(forward-char position)
      (goto-char line-end))
    (if (< (point) line-begin)
	(goto-char line-begin))))

(defun sisal-relative-cursor-position ()
  "Returns the cursor position relative to the first non-white space
character."
  (interactive)
  (save-excursion
    (let ((position (current-column)))
      (beginning-of-line)
      (sisal-move-to-first-character-on-line)
      (- position (current-column)))))

(defun sisal-move-line-right (column)
  "Moves the current line to the COLUMN position. Keeps the cursor position
relative to the first non-white space.  Will move the cursor to the first
non-white space if it's before the first non-white space."
  (interactive "nEnter column to move line right:")
  (let ((position (sisal-relative-cursor-position)))
    (sisal-move-line-left)
    (beginning-of-line)
    (if (> column 0)
	(indent-to-column column))
    (sisal-move-to-first-character-on-line)
    ; if there are no characters, then sisal-m-t-f-c-on-line leaves point
    ; at beginning of line
    (if (looking-at "\\s ")
	(end-of-line))
    (if (> position 0)
	(sisal-move-cursor position))))

(defun sisal-move-line-left ()
  "Moves the current line to the sisal-left-limit."
  (interactive)
  (let* ((position (sisal-relative-cursor-position)))
    (beginning-of-line)
    (delete-horizontal-space)
    (indent-to-column sisal-left-margin)
    ; don't place cursor to the left of the left-margin 
    (if (> position 0)
	(sisal-move-cursor position))))

(defun sisal-item-keyword (word word-list)
  (let ((n 0)
	(found nil))
    (setq word (downcase word))
    (while (< n (length word-list))
      (if (string= word (nth n word-list))
	  (setq found word))
      (setq n (+ n 1)))
    found))

(defun sisal-left-item ()
  "Returns non-nil if line begins with a sisal-left-item-keyword."
  (interactive)
  (save-excursion
    (let* ((line-begin (sisal-move-to-first-character-on-line))
	   (next-word (buffer-substring line-begin 
					(progn (forward-word 1)
					       (point)))))
      (if (sisal-item-keyword next-word sisal-left-item-keyword-list)
	  next-word
	nil))))

(defun sisal-end-item ()
  "Returns non-nil if line begins with a sisal-end-item-keyword."
  (interactive)
  (save-excursion
    (let* ((line-begin (sisal-move-to-first-character-on-line))
	   (next-word (buffer-substring line-begin 
					(progn (forward-word 1)
					       (point)))))
      (if (sisal-item-keyword next-word sisal-end-item-keyword-list)
	  next-word
	nil))))

(defun sisal-middle-item ()
  "Returns non-nil if line begins with a sisal-middle-item-keyword."
  (interactive)
  (save-excursion
    (let* ((line-begin (sisal-move-to-first-character-on-line))
	   (next-word (buffer-substring line-begin 
					(progn (forward-word 1)
					       (point)))))
      (if (sisal-item-keyword next-word sisal-middle-item-keyword-list)
	  next-word
	nil))))

(defun sisal-block-or-middle-item ()
  "Returns non-nil if line begins with a sisal-block-item-keyword or
a sisal-middle-item-keyword."
  (interactive)
  (save-excursion
    (let* ((line-begin (sisal-move-to-first-character-on-line))
	   (next-word (buffer-substring line-begin (progn (forward-word 1)
							  (point)))))
      (if (sisal-item-keyword next-word 
			      (append sisal-block-item-keyword-list
				      sisal-middle-item-keyword-list))
	  next-word
	nil))))

(defun sisal-contains-item (item-list)
  "returns non-nil if line contains a word in the item-list."
  (save-excursion
    (beginning-of-line)
    ; the keyword must be a word so we look for [^a-z0-9A-Z_]word[^a-z0-9A-Z_]
    ; which might occur at the beginning of end of a line too.
    (if (re-search-forward (concat "\\(^\\|[^a-z0-9A-Z_]\\)\\("
				   (mapconcat 'identity 
					      item-list
					      "\\|")
				   "\\)\\($\\|[^a-z0-9A-Z_]\\)")
			   (save-excursion (end-of-line)
					   (point))
		       "move-to-limit-of-search")
	(let ((found (point)))
	  (if (> found (save-excursion
			 (sisal-move-to-first-character-on-line)
			 (re-search-forward sisal-comment-begin 
					    (save-excursion (end-of-line)
							    (point))
					    "move-to-limit-of-search")
			 (point)))
	      nil
	    t)))))


(defun sisal-contains-block-item ()
  "Returns non-nil if line contains a sisal-block-item-keyword.  If the
keyword is found after a sisal-comment-begin, then return nil.  This should
be changed for more general style comments, but for now assume that the
comment-end is the end of line."
  (interactive)
  (sisal-contains-item sisal-block-item-keyword-list))

(defun sisal-contains-block-or-middle-item ()
  "Returns non-nil if line contains a sisal-block-item-keyword or a
sisal-middle-item-keyword.  If the keyword is found after a
sisal-comment-begin, then return nil.  This should be changed for more
general style comments, but for now assume that the comment-end is the
end of line."
  (interactive)
  (sisal-contains-item (append sisal-block-item-keyword-list
			       sisal-middle-item-keyword-list)))

(defun sisal-contains-end-item ()
  "Returns non-nil if line contains a sisal-end-item-keyword.  If the
keyword is found after a sisal-comment-begin, then return nil.  This should
be changed for more general style comments, but for now assume that the
comment-end is the end of line."
  (interactive)
  (sisal-contains-item sisal-end-item-keyword-list))

(defun sisal-next-indent-level (current-indent-level)
  "Calculates the next indent level based on the current-indent-level. For 
now, this simply returns sisal-next-indent-amount + current-indent-level."
  (+ sisal-next-indent-amount current-indent-level))

(defun sisal-backward-line ()
  "Move backward until found a non-blank line.  Puts point at 
beginning of line.  Returns point."
  (interactive)
  (let ((found nil))
    (while (and (not found) (not (bobp)))
      (forward-line -1)
      (sisal-move-to-first-character-on-line)
      ; look for non-whitespace and non-blank lines
      (if (looking-at "\\S-")
	  (setq found t))
      (beginning-of-line))
    (point)))

(defun sisal-goto-line-further-left ()
  "Moves to the nearest line that is further left than the current line."
  (interactive)
  (let* ((current-indent (sisal-get-indent-level))
	 (new-indent current-indent))
    (while (and (>= new-indent current-indent)
		(not (bobp))
		(> new-indent sisal-left-margin))
      (sisal-backward-line)
      (setq new-indent (sisal-get-indent-level)))))

(defun sisal-goto-line-further-left-block ()
  "Moves to the nearest line that is further left than the current line and
contains a block item or is a middle item."
  (interactive)
  (sisal-goto-line-further-left)
  (while (and (not (sisal-contains-block-item))
	      (not (sisal-middle-item))
	      (not (bobp))
	      (> (sisal-get-indent-level) sisal-left-margin))
    (sisal-goto-line-further-left)))

(defun sisal-goto-nearest-outer-scope ()
  "Moves to the nearest outer scope for the current line.  Assumes
that the current line might not be indented properly, But that the previous
line is indented properly.
Puts the point at the beginning of the line and returns the indent level 
of the line."
  (interactive)
  (sisal-backward-line)
  (cond ((and (not (sisal-contains-end-item))
	      (or (sisal-contains-block-or-middle-item)))
	 t) ; found outer scope at previous line
	; find matching begin - end block.
	((sisal-end-item)
	 (let* ((current-indent (sisal-get-indent-level))
		(new-indent current-indent)
		(found nil))
	   (while (and (not (bobp))
		       (> new-indent sisal-left-margin)
		       (not found))
	     (sisal-backward-line)
	     (setq new-indent (sisal-get-indent-level))
	     (if (and (>= current-indent new-indent)
		      (sisal-contains-block-item))
		 (setq found t)))
	   (sisal-goto-line-further-left-block)))
	(t (sisal-goto-line-further-left-block)))
  (sisal-get-indent-level)
  (beginning-of-line))

(defun sisal-get-indent-level ()
  "Returns the number of spaces that have been indented."
  (interactive)
  (save-excursion
    (sisal-move-to-first-character-on-line)
    (current-column)))

(defun sisal-indent-command ()
  "Indent current line as sisal code."
  (interactive)
  ; (message "called sisal-indent-command")
  ; Note: the previous line is the previous non-blank line.
  (sisal-move-line-right
   (cond        ; 1. if line is left-item  or previous line is current line
		; then move line to left margin
    ((sisal-left-item)
     (if sisal-debug (message "left-item"))
     sisal-left-margin)
		; 2. if previous line is left-item and not block-item
		; then move line to left margin
    ((save-excursion (sisal-backward-line)
		     (and (sisal-left-item)
			  (not (sisal-block-or-middle-item))))
     (if sisal-debug 
	 (message "previous line is left-item and not block or middle"))
     sisal-left-margin)
		; 2. if line is normal or begin and previous line is normal 
                ; then indent like previous line.
    ((and (not (sisal-end-item)) 
	  (not (sisal-middle-item))
	  (save-excursion (sisal-backward-line)
			  (and (not (sisal-end-item))
			       (not (sisal-contains-block-or-middle-item)))))
     (if sisal-debug 
	 (message "line is normal or begin and previous is normal"))
     (save-excursion (sisal-backward-line)
		     (sisal-get-indent-level)))
		; 2. normal line or begin-line so indent according to
		; nearest outerscope.
    ((and (not (sisal-end-item)) (not (sisal-middle-item)))
     (save-excursion
       (sisal-goto-nearest-outer-scope)
       (cond ((sisal-end-item)
	      (if sisal-debug
		  (message
		   "line is normal or begin and outer scope is end"))
	      (sisal-get-indent-level))
	     ((sisal-block-or-middle-item)
	      (progn 
		(if sisal-debug 
		    (message 
		     "line is normal or begin and scope is begin or middle"))
		(sisal-next-indent-level (sisal-get-indent-level))))
	     ((sisal-contains-block-item)
	      (if sisal-debug
		  (message 
		   "line is normal or begin and outer scope contains begin"))
	      (sisal-next-indent-level 
	       (sisal-next-indent-level (sisal-get-indent-level))))
	     (t 
	      (if sisal-debug
		  (message
		   "line is normal or begin and scope is normal"))
	      (sisal-get-indent-level)))))
		; 3. line is an end or middle item
    ((or (sisal-end-item) (sisal-middle-item))
     (save-excursion
       (sisal-goto-nearest-outer-scope)
       (cond ((sisal-end-item)
	      (if sisal-debug
		  (message
		   "line is end or middle and outer scope is end"))
	      (message "Warning, cannot find enclosing begin.")
	      (sisal-get-indent-level))
	     ((sisal-block-or-middle-item)
	      (progn 
		(if sisal-debug
		    (message 
		     "line is end or middle and outer scope is begin"))
		(sisal-get-indent-level)))
	     (t 
	      (if sisal-debug
		  (message 
		   "line is end or middle and outer scope is not a begin"))
	      (sisal-next-indent-level (sisal-get-indent-level))))))
		; 4. default is like previous line
    (t (save-excursion
	 (if sisal-debug (message "line is nothing special."))
	 (sisal-backward-line)
	   (sisal-get-indent-level))))))

(defun sisal-newline-indent-command ()
  (interactive)
  "Indent current line as sisal code."
  (if (or (sisal-end-item)
	  (sisal-middle-item))
      (sisal-indent-command))
  (insert "\n")
  (sisal-indent-command))


(defun sisal-mode ()
  "Major mode for editing sisal code.
Expression and list commands understand all Sisal brackets.
Tab indents for sisal code.
Delete converts tabs to spaces as it moves back.
\\{sisal-mode-map}
Variables controlling indentation style:

Turning on sisal mode calls the value of the variable sisal-mode-hook 
with no args, if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map sisal-mode-map)
  (setq major-mode 'sisal-mode)
  (setq mode-name "Sisal")
  (setq local-abbrev-table sisal-mode-abbrev-table)
  (set-syntax-table sisal-mode-syntax-table)
  (make-local-variable 'comment-start)
  (setq comment-start "% ")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "%+[ \t]*")
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'sisal-indent-command)
  (make-local-variable 'comment-column)
  (setq comment-column 32)
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'sisal-comment-indent)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (run-hooks 'sisal-mode-hook))

;; This is used by indent-for-comment
;; to decide how much to indent a comment in C code
;; based on its context.
(defun sisal-comment-indent ()
  (if (looking-at "^%")
      0				;Existing comment at bol stays there.
    (save-excursion
      (skip-chars-backward " \t")
      (max (1+ (current-column))	;Else indent at comment column
	   comment-column))))	; except leave at least one space.

