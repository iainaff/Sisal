#include "world.h"


/**************************************************************************/
/* GLOBAL **************       ReadAssocList       ************************/
/**************************************************************************/
/* PURPOSE: READ AND RETURN A COMPOUND NODE's ASSOCIATION LIST CONTAINING */
/*          cnt UNSINGED INTEGERS.                                        */
/**************************************************************************/

PALIST ReadAssocList( cnt )
register int cnt;
{
  register PALIST l = AssocListAlloc( ReadInteger() );

  while ( (--cnt) > 0 )
    l = LinkAssocLists( l, AssocListAlloc( ReadInteger() ) );

  return( l );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:08  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
