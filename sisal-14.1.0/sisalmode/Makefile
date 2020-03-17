SHELL=/bin/sh
BIN_PATH = /usr/local/lib/emacs/lisp
MMASK = 644
SINSTALL = cp

all: sisal-mode.elc

sisal-mode.elc: sisal-mode.el
	cp /dev/null ./ecmd
	echo '(byte-compile-file "./sisal-mode.el")' >> ./ecmd
	echo '(kill-emacs)' >> ./ecmd
	DISPLAY="" emacs -l ecmd


install: sisal-mode.elc
	$(SINSTALL) sisal-mode.elc $(BIN_PATH)/sisal-mode.elc
	chmod $(MMASK) $(BIN_PATH)/sisal-mode.elc

clean:
	/bin/rm -f ./ecmd ./sisal-mode.elc

#