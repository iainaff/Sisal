#include "world.h"
/**************************************************************************/
/* GLOBAL **************  FixSignedConstantImports ************************/
/**************************************************************************/
/* PURPOSE: IF sgnok IS TRUE, REMOVE THE SIGN OF ALL  NEGATIVE ARITHMETIC */
/*          IMPORTS TO NODE n AND INSERT THE APPROPRIATE NEG NODES; LLNL  */
/*          SOFTWARE DOES NOT ACCEPT SIGNED CONSTANTS. A NULL CONSTANT    */
/*          DEFINES AN ERROR VALUE.                                       */
/**************************************************************************/

int FixSignedConstantImports( n, lab )
PNODE n;
int   lab;
{
  register PEDGE  i;
  register PEDGE  ii;
  register PNODE  neg;

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    if ( !IsConst( i ) || (i->CoNsT == NULL) )
      continue;

    if ( IsArithmetic( i->info ) && (!sgnok) )
      if ( i->CoNsT[0] == '-' ) {
	neg = NodeAlloc( ++lab, IFNeg );
	ii  = EdgeAlloc( (PNODE)NULL, CONST_PORT, neg, 1 );

	ii->info = i->info;
	ii->CoNsT = &(i->CoNsT[1]);

	LinkImport( neg, ii );

	i->CoNsT = NULL;
	i->eport = 1;

	LinkExport( neg, i );
	LinkNode( (IsGraph(n))? n : n->npred, neg );
      }
  }

  return( lab );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:17  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:40  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
