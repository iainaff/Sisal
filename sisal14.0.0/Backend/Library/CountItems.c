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



/* $Log$
 * Revision 1.1  1993/01/21  23:28:15  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
