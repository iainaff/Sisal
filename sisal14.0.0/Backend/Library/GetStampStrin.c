#include "world.h"


/**************************************************************************/
/* GLOBAL **************      GetStampString       ************************/
/**************************************************************************/
/* PURPOSE: RETURN A POINTER TO THE COMMENTARY FOR STAMP stamp.           */
/**************************************************************************/

char *GetStampString( stamp )
     char stamp;
{
  return( stamps[ (int)stamp ] );
}

/* $Log$
 * Revision 1.2  1994/04/15  15:51:45  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.1  1993/04/16  19:00:20  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:46  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
