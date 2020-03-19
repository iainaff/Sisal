/**************************************************************************/
/* FILE   **************        CountItems.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


struct level levels[MaxLevel];     /* OCCURRENCE COUNT LEVEL STACK */
int          maxl;                 /* MAXIMUM ENCOUNTERED LEVEL    */
int          topl;                 /* TOP OF LEVEL STACK           */ 

int lits;                          /* TOTAL OCCURRENCE COUNTERS    */
int edges;
int simples;
int ats;
int graphs;
int comps;
int rsum;
int rprod;
int rcat;
int rleast;
int rgreat;

int gnodes[IF1GraphNodes];
int snodes[IF1SimpleNodes];
int atnodes[IF2AtNodes];
int cnodes[IF1CompoundNodes];

int convatns  = 0;
int inatns    = 0;
int patns     = 0;
int syncatns  = 0;
int fsyncatns = 0;
int incratns  = 0;
int fincratns = 0;

void bogus_call_to_force_link_countitems(void) {
}


/*
 * $Log$
 * Revision 1.3  2003/12/11 22:28:18  patmiller
 * Updates to handle bogus linker error with GCC on MacOSX
 * wherein the linker fails to load from .o that have no
 * functions (e.g. data segment only).
 *
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:14  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.1  1993/01/21  23:28:15  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
