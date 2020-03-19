#include "world.h"


/**************************************************************************/
/* GLOBAL **************          MakeAde          ************************/
/**************************************************************************/
/* PURPOSE: MAKE AN ARTIFICAIL DEPENDENCE EDGE AND INITIALIZE IT.  THEN   */
/*          LINK THE EDGE INTO ITS DESTINATION NODE'S ADE IMPORT LIST AND */
/*          SOURCE NODE'S ADE EXPORT LIST.                                */
/*									  */
/*	    IF FULLY ORDERED, DO NOTHING.  			          */
/**************************************************************************/

void MakeAde( snode, dnode )
int snode;
int dnode;
{
  if ( !FullyOrdered ) {
    register PADE  a;
    register PNODE src;
    register PNODE dst;

    dst = FindNode( dnode, IFUndefined );
    src = FindNode( snode, IFUndefined );

    a          = AdeAlloc( src, dst, LOW_PRI );
    a->if1line = line;

    LinkAdeImport( dst, a );
    LinkAdeExport( src, a );
  }
}

/* $Log$
 * Revision 1.2  1994/07/13  18:08:37  denton
 * Removed BAD from IFX.h and fix MakeAde to use LOW_PRI (it incorrectly used LOW)
 *
 * Revision 1.1  1993/01/21  23:29:32  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
