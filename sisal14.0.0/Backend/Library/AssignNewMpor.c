#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AssignNewMports      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW M PORT NUMBERS, STARTING WITH p, TO THE IMPORTS OF */
/*          FORALL f'S GENERATE SUBGRAPH AND ADJUST ALL REFERENCES.  THE  */
/*          NEXT LEGAL PORT NUMBER IS RETURNED.                           */
/**************************************************************************/

int AssignNewMports( p, f )
int   p;
PNODE f;
{
  register PEDGE i;

  for ( i = f->F_GEN->imp; i != NULL; i = i->isucc, p++ )
    if ( i->iport != p ) {
      ChangeExportPorts( f->F_BODY, i->iport, -p );
      ChangeExportPorts( f->F_RET,  i->iport, -p );

      i->iport = -p;
    }

  return( p );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:49  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:53  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
