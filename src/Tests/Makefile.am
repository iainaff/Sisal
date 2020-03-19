SISALC=${bindir}/sisalc

LDADD = $(pkglibdir)/libsisal.a $(pkglibdir)/srt0.o

LDFLAGS += $(parallel_link)

AM_DEFAULT_SOURCE_EXT = .sis

.sis.o:
	$(SISALC) -w -c $< -o $@

TESTSUITE = three hello 8queens bubble quicksort crypto fromC

check_PROGRAMS = $(TESTSUITE)

fromC_SOURCES = fromC.sis fromCdriver.c

fromC_LDADD = $(pkglibdir)/libsisal.a

fromC.$(OBJEXT): fromC.sis
	$(SISALC) -forC -w -c $(srcdir)/fromC.sis -o $(builddir)/fromC.$(OBJEXT)

dist_check_SCRIPTS = run-test

TESTS_ENVIRONMENT = $(SHELL) $(srcdir)/run-test "$(DIFF)" "$(RM)" "$(srcdir)" "$(builddir)" 

TESTS = $(check_PROGRAMS)

dist_noinst_DATA = ${TESTSUITE:%=%.in} ${TESTSUITE:%=%.ans}

CLEANFILES = ${TESTSUITE:%=%.out} ${TESTSUITE:%=%.out3} ${TESTSUITE:%=%.dif} ${TESTSUITE:%=%.dif3}

answers:
	for tst in $(TESTS); do $(TESTS_ENVIRONMENT) --answer $(builddir)/$${tst} ; done

