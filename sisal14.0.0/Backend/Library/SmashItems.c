#include "world.h"

int   lclass;			/* LAST EQUIVALENCE CLASS IN htable  */
PINFO htable[MaxClass];		/* EQUIVALENCE CLASS HEAD POINTERS   */
PINFO ttable[MaxClass];		/* EQUIVALENCE CLASS TAIL POINTERS   */

/* $Log$
 * Revision 1.1  1993/01/21  23:30:24  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
