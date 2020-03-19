SHELL=/bin/sh

DIFF=diff

OSC=osc
SRFLAGS=-r
SFLAGS=

run: touch ricard ricard.in ricard.ans
	ricard $(SRFLAGS) -ds30000000 ricard.in > ricard.test \
		-bformat "%c " \
		-cformat "'%c' " \
		-fformat "%.1f " \
		-dformat "%.1lf " \
		-iformat "%d "
	@-$(DIFF) ricard.test ricard.ans >> test.log
	$(DIFF) ricard.test ricard.ans
	@echo Compared OK

touch:
	@touch ricard.in

ricard: ricard.sis
	$(OSC) $(SFLAGS) ricard.sis -o ricard >> test.log 2>&1

clean:
	@/bin/rm -f ricard ricard.test test.log s.info ricard.in
	@/bin/rm -f core *.if1 *.opt *.mono *.mem *.up *.part *.c *.o *.i
#
