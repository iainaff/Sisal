#include "world.h"


/**************************************************************************/
/* GLOBAL **************        RemoveStamp        ************************/
/**************************************************************************/
/* PURPOSE: REMOVE STAMP stamp FROM THE STAMP TABLE.                      */
/**************************************************************************/

void RemoveStamp( stamp )
     char stamp;
{
  stamps[ (int)stamp ] = NULL;
}

/* $Log$
 * Revision 1.2  1994/04/15  15:51:54  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.1  1993/01/21  23:30:21  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
