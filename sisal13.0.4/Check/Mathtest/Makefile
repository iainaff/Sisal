SHELL=/bin/sh

DIFF=diff

OSC=osc
SRFLAGS=
SFLAGS=
ANS=

run: touch basic basic.in basic.ans
	basic $(SRFLAGS) basic.in > basic.test \
		-bformat "%c " \
		-cformat "'%c' " \
		-fformat "%.1f " \
		-dformat "%.1lf " \
		-iformat "%d "
	@-$(DIFF) basic.test basic$(ANS).ans >> test.log
	$(DIFF) basic.test basic$(ANS).ans
	@echo Compared OK

touch:

basic: basic.sis
	$(OSC) $(SFLAGS) -noinline basic.sis -o basic >> test.log 2>&1

clean:
	@/bin/rm -f basic basic.o basic.test test.log s.info
	@/bin/rm -f core *.if1 *.opt *.mono *.mem *.up *.part *.c *.o *.i
#
