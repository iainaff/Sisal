testFiles=three.sis hello.sis 8queens.sis bubble.sis quicksort.sis crypto.sis fromC.sis

localFiles= \
	${testFiles:.sis=.exe} ${testFiles:.sis=.out} ${testFiles:.sis=.dif} ${testFiles:.sis=.out3} ${testFiles:.sis=.dif3} 

.PRECIOUS: ${localFiles}

include ../Include/makefile.inc

SISALC=${bindir}/sisalc

fromC.exe: fromC.o fromCdriver.c
	$(SISALC) -o $@ $^ -forC

fromC.o: fromC.sis
	$(SISALC)  $< -w -c -forC

%.exe: %.sis
	$(SISALC) -w $< -o $@

%.out: %.exe
	$(RM) $@
	$< < ${@:.out=.in} 2>&1 | tail -n +2 > $@

%.dif: %.out
	$(DIFF) $< ${<:.out=.ans}

%.out3: %.exe
	$(RM) $@
	$< -w3 < ${@:.out3=.in} 2>&1 | tail -n +2 > $@

%.dif3: %.out3
	$(DIFF) $< ${<:.out3=.ans}

clean:
	$(RM) ${localFiles} fromC.o

answers:
	for root in ${testFiles:.sis=.}; do $(RM) $$root'ans'; $(MAKE) $$root'out'; mv $$root'out' $$root'ans'; done
