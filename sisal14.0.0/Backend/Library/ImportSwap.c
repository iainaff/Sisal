#include "world.h"


/**************************************************************************/
/* GLOBAL **************        ImportSwap         ************************/
/**************************************************************************/
/* PURPOSE: SWAP THE IMPORTS OF DYADIC NODE n.                            */
/**************************************************************************/

void ImportSwap( n )
PNODE n;
{
  register PEDGE one;
  register PEDGE two;

  one = n->imp;
  two = one->isucc;

  UnlinkImport( one );
  UnlinkImport( two );

  one->iport = 2;
  two->iport = 1;

  LinkImport( n, two );
  LinkImport( n, one );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:54  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
