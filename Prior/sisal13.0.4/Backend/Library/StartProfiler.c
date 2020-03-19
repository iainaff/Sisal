#include "world.h"


/**************************************************************************/
/* GLOBAL **************       StartProfiler       ************************/
/**************************************************************************/
/* PURPOSE: RECORD THE CURRENT TSECND VALUE IN StartTime                  */
/**************************************************************************/

void StartProfiler()
{
  if ( prof )
    StartTime = TSECND();
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:25  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
