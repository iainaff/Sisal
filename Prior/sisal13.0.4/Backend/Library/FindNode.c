#include "world.h"


/**************************************************************************/
/* GLOBAL **************         FindNode          ************************/
/**************************************************************************/
/* PURPOSE: FIND AND RETURN THE NODE WITH LABEL label.  IF label IS 0,    */
/*          THE GRAPH NODE OF THE GRAPH BEING CONSTRUCTED IS BEING        */
/*          REFERENCED (LAST ONE IN THE GRAPH LIST ADDRESSED BY glstop),  */
/*          ELSE SEARCH FOR IT IN THE NODE LIST ADDRESSED BY nlstop. IF   */
/*          NOT FOUND, IT IS CREATED, PLACED IN THE LIST IN label ORDER,  */
/*          AND RETURNED.                                                 */
/**************************************************************************/

PNODE FindNode( label, type )
int label;
int type;
{
  register PNODE n;

  if ( label == 0 )
    return( glstop->gpred );

  /* FIRST CHECK THE FAST NODE LOOKUP HASH TABLE */
  if ( (n = nhash[label%MAX_NHASH]) != NULL )
    if ( n->label == label ) {
      if ( n->type == IFUndefined )
	n->type = type;

      return( n );
    }

  if ( nprd != NULL )
    if ( nprd->label < label )
      goto DoThePlacement;

  for ( n = nlstop->nsucc; n != NULL; n = n->nsucc )
    if ( n->label == label ) {
      if ( n->type == IFUndefined )
	n->type = type;

      /* ENTER n INTO THE FAST LOOKUP HASH TABLE */
      nhash[label%MAX_NHASH] = n;

      return( n );
    }

 DoThePlacement:
  PlaceNode( n = NodeAlloc( label, type ) );

  /* ENTER n INTO THE FAST LOOKUP HASH TABLE */
  nhash[label%MAX_NHASH] = n;

  return( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:36  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
