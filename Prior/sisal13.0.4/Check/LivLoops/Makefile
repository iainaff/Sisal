SHELL=/bin/sh

DIFF=diff

OSC=osc
SRFLAGS=
SFLAGS=

run: runloop1 runloop2 runloop3

runloop1: touch loop1 loop1.in loop1.ans
	loop1 $(SRFLAGS) loop1.in > loop1.test \
		-bformat "%c " \
		-cformat "'%c' " \
		-fformat "%.4f " \
		-dformat "%.4lf " \
		-iformat "%d "
	@-$(DIFF) loop1.test loop1.ans >> test.log
	$(DIFF) loop1.test loop1.ans
	@echo Loop1 Compared OK

runloop2: touch loop2 loop2.in loop2.ans
	loop2 $(SRFLAGS) loop2.in > loop2.test \
		-bformat "%c " \
		-cformat "'%c' " \
		-fformat "%.4f " \
		-dformat "%.4lf " \
		-iformat "%d "
	@-$(DIFF) loop2.test loop2.ans >> test.log
	$(DIFF) loop2.test loop2.ans
	@echo Loop2 Compared OK

runloop3: touch loop3 loop3.in loop3.ans
	loop3 $(SRFLAGS) loop3.in > loop3.test -dformat "%.6lf"
	@-$(DIFF) loop3.test loop3.ans >> test.log
	$(DIFF) loop3.test loop3.ans
	@echo Loop3 Compared OK

touch:

loop1: loop1.sis
	$(OSC) $(SFLAGS) loop1.sis -o loop1 >> test.log 2>&1
loop2: loop2.sis
	$(OSC) $(SFLAGS) loop2.sis -o loop2 >> test.log 2>&1
loop3: loop3.sis
	$(OSC) $(SFLAGS) loop3.sis -o loop3 >> test.log 2>&1

clean:
	@/bin/rm -f test.log s.info \
		loop1 loop1.o loop1.test \
		loop2 loop2.o loop2.test \
		loop3 loop3.o loop3.test
	@/bin/rm -f core *.if1 *.opt *.mono *.mem *.up *.part *.c *.o *.i
#
