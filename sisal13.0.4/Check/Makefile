###############################################################################
# Makefile for SISAL OSC tests
#       SFLAGS = compile time flags for OSC (-info, -v, -noopt, etc.)
#	SFLAGS_ALL = SFLAGS used when checking all tests
###############################################################################

SHELL=/bin/sh

DIFF         = diff

CC           = cc
FF           = f77
BINSTALL     = cp
SINSTALL     = cp
OSC	     = osc 
RANLIB       = touch
AR           = ar r
PAR          =

PROCS        = 1
SRFLAGS      = 
SFLAGS       = -info=1-4
SFLAGS_ALL   = -Keep -v
SRFLAGS_PAR  = -r -w$(PROCS)
ANS          =

MAKE_CHECK   = $(MAKE) OSC=$(OSC) SRFLAGS="$(SRFLAGS)" SFLAGS="$(SFLAGS)"
MAKE_ALL     = $(MAKE) OSC=$(OSC) SRFLAGS="$(SRFLAGS)" SFLAGS="$(SFLAGS_ALL)"
MAKE_PAR     = $(MAKE) OSC=$(OSC) SRFLAGS="$(SRFLAGS_PAR)" SFLAGS="$(SFLAGS)"

local: check
optional:
all: local optional

install:
	echo "Nothing to install"
lint:
	echo "Nothing to lint"

check:     mathtest       livloops       database
check_all: mathtest_all   livloops_all   database_all \
	   simple_all     ricard_all
check_par: mathtest_par   livloops_par   database_par \
	   simple_par     ricard_par
clean:	   clean_mathtest clean_livloops clean_database \
	   clean_simple   clean_ricard

mathtest:
	cd Mathtest; $(MAKE_CHECK) DIFF=$(DIFF) ANS=$(ANS)
mathtest_all:
	-cd Mathtest; $(MAKE_ALL) DIFF=$(DIFF) ANS=$(ANS)
mathtest_par:
	-cd Mathtest; $(MAKE_PAR) DIFF=$(DIFF) ANS=$(ANS)
clean_mathtest:
	cd Mathtest; make clean

livloops:
	cd LivLoops; $(MAKE_CHECK) DIFF=$(DIFF) ANS=$(ANS)
livloops_all:
	-cd LivLoops; $(MAKE_ALL) DIFF=$(DIFF) ANS=$(ANS)
livloops_par:
	-cd LivLoops; $(MAKE_PAR) DIFF=$(DIFF) ANS=$(ANS)
clean_livloops:
	cd LivLoops; make clean

database:
	cd Database; $(MAKE_CHECK) DIFF=$(DIFF) ANS=$(ANS)
database_all:
	-cd Database; $(MAKE_ALL) DIFF=$(DIFF) ANS=$(ANS)
database_par:
	-cd Database; $(MAKE_PAR) DIFF=$(DIFF) ANS=$(ANS)
clean_database:
	cd Database; make clean

simple:
	cd Simple; $(MAKE_CHECK) DIFF=$(DIFF) ANS=$(ANS)
simple_all:
	-cd Simple; $(MAKE_ALL) DIFF=$(DIFF) ANS=$(ANS)
simple_par:
	-cd Simple; $(MAKE_PAR) DIFF=$(DIFF) ANS=$(ANS)
clean_simple:
	cd Simple; make clean

ricard:
	cd Ricard; $(MAKE_CHECK) DIFF=$(DIFF) ANS=$(ANS)
ricard_all:
	-cd Ricard; $(MAKE_ALL) DIFF=$(DIFF) ANS=$(ANS)
ricard_par:
	-cd Ricard; $(MAKE_PAR) DIFF=$(DIFF) ANS=$(ANS)
clean_ricard:
	cd Ricard; make clean
#
