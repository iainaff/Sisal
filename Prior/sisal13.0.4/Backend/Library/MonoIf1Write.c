#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoIf1Write      ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE MONOLITH TO output. THE TYPE INFORMATION IS FOUND   */
/*          TRAVERSING ihead; THE STAMPS ARE FOUND IN ARRAY stamps; AND   */
/*          THE FUNCTION GRAPHS ARE FOUND TRAVERSING nhead.               */
/**************************************************************************/

void MonoIf1Write()
{
  MonoWriteInfo();
  WriteStamps();
  WriteGlobals();
  MonoWriteNodes();
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:37  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:51  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
