#include "world.h"


/**************************************************************************/
/* GLOBAL **************     PrepareForNextFile    ************************/
/**************************************************************************/
/* PURPOSE: PREPARE TO READ THE NEXT IF1 FILE AND BUILD ITS INTERNAL FORM.*/
/*          SO THAT ALL TYPE LABELS DEFINED IN THE MONOLITH ARE UNIQUE    */
/*          AND CONSECUTIVE, tbase IS SET TO THE LAST LABEL ENCOUNTERED   */
/*          IN THE PREVIOUS FILE.                                         */
/**************************************************************************/

void PrepareForNextFile()
{
  tbase  = LargestTypeLabelSoFar;
}

/* $Log$
 * Revision 1.2  1994/05/25  23:35:02  solomon
 * Changed variable tmax to LargestTypeLabelSoFar.
 *
 * Revision 1.1  1993/04/16  19:00:52  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:01  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
