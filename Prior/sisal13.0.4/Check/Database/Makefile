SHELL=/bin/sh

DIFF=diff

OSC=osc
SRFLAGS=
SFLAGS=-info=1-4

run: touch driver
	@-/bin/rm -f driver.test
	driver $(SRFLAGS) > driver.test
	@-$(DIFF) driver.test driver.ans >> test.log
	$(DIFF) driver.test driver.ans
	@echo Compared OK

touch:

cdf.o: cdf.sis
	$(OSC) $(SFLAGS) -module -noload -db database cdf.sis >>test.log 2>&1

driver: driver.sis cdf.o
	$(OSC) $(SFLAGS) driver.sis cdf.o -db database -o driver >>test.log 2>&1

clean:
	@/bin/rm -f driver driver.o cdf.o database driver.test test.log s.info
	@/bin/rm -f core *.if1 *.opt *.mono *.mem *.up *.part *.c *.o *.i
#
