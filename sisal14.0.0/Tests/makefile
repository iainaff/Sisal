testFiles=three.sis 8queens.sis bubble.sis quicksort.sis

localFiles=${testFiles:.sis=.exe} ${testFiles:.sis=.out} ${testFiles:.sis=.dif} 
.PRECIOUS: ${localFiles}

include ../Include/makefile.inc

SISALC=${bindir}/sisalc

%.exe: %.sis
	$(SISALC) -o $@ -w $<

%.out: %.exe
	$(RM) $@
	$< < ${<:.exe=.in} 2>&1 | tail -n +2 > $@

%.dif: %.out
	$(DIFF) $< ${<:.out=.ans}

clean:
	$(RM) ${localFiles}

answers:
	for root in ${testFiles:.sis=.}; do $(RM) $$root'ans'; $(MAKE) $$root'out'; mv $$root'out' $$root'ans'; done
