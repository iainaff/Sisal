#include "world.h"


/**************************************************************************/
/* GLOBAL **************     FixConstantImports    ************************/
/**************************************************************************/
/* PURPOSE: CHECK FOR AND UNDO THE SIDE EFFECTS OF CONSTANT FOLDING:      */
/*          DOUBLE_REAL CONSTANTS IN REAL FORMAT AND SIGNED ARITHMETIC    */
/*          CONSTANTS (NOT ACCEPTED BY LLNL SOFTWARE). A SIGNED CONSTANT  */
/*          IS CONVERTED INTO A NEG NODE WHOSE IMPORT IS THE CONSTANT'S   */
/*          POSITIVE. THE NEW NODE IS GIVEN LABEL ++lab. THE  LAST        */
/*          ASSIGNED LABEL IS RETURNED.  A NULL CONSTANT DEFINES AN ERROR */
/*          VALUE.                                                        */
/**************************************************************************/

int FixConstantImports( n, lab )
PNODE n;
int   lab;
{
  register PEDGE  i;
  register PEDGE  ii;
  register char  *p;
  register PNODE  neg;

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    if ( !IsConst( i ) ) {
      if ( IsError( i->src ) ) {
	i->src = NULL;
	i->CoNsT = NULL;
	i->eport = CONST_PORT;
      }

      continue;
    }

    /* Watch out for literal buffers which have an elemental type */
    if ( IsDouble( i->info ) && i->CoNsT && i->CoNsT[0] != 'B' ) {
      for ( p = i->CoNsT; *p != '\0'; p++ ) {
	if ( (*p == 'E') || (*p == 'e') ) *p = 'd';
      }
    }

    if ( IsReal( i->info ) && i->CoNsT && i->CoNsT[0] != 'B' ) {
      for ( p = i->CoNsT; *p != '\0'; p++ ) {
	if ( (*p == 'D') || (*p == 'd') ) *p = 'e';
      }
    }

    if ( IsArithmetic( i->info ) && (!sgnok) )
      if ( i->CoNsT && i->CoNsT[0] == '-' ) {
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
 * Revision 1.2  1993/11/12  20:03:43  miller
 * Was bombing with error valued constants
 *
 * Revision 1.1  1993/04/16  19:00:15  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.2  1993/04/01  23:30:21  miller
 * FixConstantImports standardizes real and double lits.  Unfortunately,
 * that value is somethis the literal ``Buffer'' which is a text constant.
 * This was causing overwrite of the text segment for GCC.
 *
 * Revision 1.1  1993/01/21  23:28:39  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
