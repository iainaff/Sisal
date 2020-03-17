bin_PROGRAMS = sisalc

sisalc_SOURCES = sisalc.c option.c charStarQueue.c \
	sisal.c if1ld.c if1opt.c if2mem.c if2up.c if2part.c if2gen.c \
	compile.c linker.c charStarQueue.h option.h sisalc.h

nodist_man_MANS = sisalc.1

CLEANFILES = sisalc.1 sisalc.html sisalc.txt

dist_noinst_SCRIPTS = generateOptions.py

nodist_doc_DATA = sisalc.html sisalc.txt

sisalc.1: sisalc$(EXEEXT)
	$(RM) $(builddir)/sisalc.1
	$(builddir)/sisalc$(EXEEXT) --man > $(builddir)/sisalc.1

sisalc.html: sisalc$(EXEEXT)
	$(RM) $(builddir)/sisalc.html
	$(builddir)/sisalc$(EXEEXT) --html > $(builddir)/sisalc.html

sisalc.txt: sisalc$(EXEEXT)
	$(RM) $(builddir)/sisalc.txt
	$(builddir)/sisalc$(EXEEXT) --help > $(builddir)/sisalc.txt

Newoptions:
	python $(srcdir)/generateOptions.py < $(srcdir)/sisalc.c > sisalc.new
	mv $(srcdir)/sisalc.c $(srcdir)/sisalc.old
	mv sisalc.new $(srcdir)/sisalc.c
