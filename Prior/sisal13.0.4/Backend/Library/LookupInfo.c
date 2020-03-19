#include "world.h"


/**************************************************************************/
/* GLOBAL **************       LookupInfo          ************************/
/**************************************************************************/
/* PURPOSE: UTILITY USED BY FindInfo TO LOCATE AN INFO NODE WITH LABEL    */
/*          label in THE INFO LIST HEADED BY ihead. IF IT IS NOT IN THE   */
/*          LIST, THEN A NEW INFO NODE IS ALLOCATED, APPENDED TO THE END, */
/*          AND RETURNED.                                                 */
/**************************************************************************/

PINFO LookupInfo( label, type )
register int label;
         int type;
{
  register PINFO i;

  for ( i = ihead; i != NULL; i = i->next )
    if ( label == i->label ) {
      if ( i->type == IF_NONTYPE )
	i->type = type;

      return( i );
    }

  i = InfoAlloc( label, type );

  itail->next = i;
  itail       = i;

  return( i );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:30  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
