#include "world.h"


/**************************************************************************/
/* GLOBAL **************         AddStamp          ************************/
/**************************************************************************/
/* PURPOSE: ADD STAMP stamp WITH COMMENTARY s TO THE STAMP TABLE.         */
/**************************************************************************/

void AddStamp( stamp, s )  
     char stamp;
     char *s;
{ 
  stamps[ (int)stamp ] = s;
}

/* $Log$
 * Revision 1.2  1994/04/15  15:51:38  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.1  1993/01/21  23:27:39  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
