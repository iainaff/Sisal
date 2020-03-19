SHELL=/bin/sh

DIFF=diff

OSC=osc
SRFLAGS=-r
SFLAGS=

run: touch simple simple.in simple.ans
	simple $(SRFLAGS) -ds30000000 simple.in > simple.test \
		-bformat "%c " \
		-cformat "'%c' " \
		-fformat "%.1f " \
		-dformat "%.1lf " \
		-iformat "%d "
	@-$(DIFF) simple.test simple.ans >> test.log
	$(DIFF) simple.test simple.ans
	@echo Compared OK

touch:
	@touch simple.in

simple: simple2a.sis
	$(OSC) $(SFLAGS) simple2a.sis -o simple >> test.log 2>&1

clean:
	@/bin/rm -f simple simple.test test.log s.info simple.in
	@/bin/rm -f core *.if1 *.opt *.mono *.mem *.up *.part *.c *.o *.i
#
