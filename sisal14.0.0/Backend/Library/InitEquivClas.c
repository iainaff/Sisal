#include "world.h"


/**************************************************************************/
/* GLOBAL **************     InitEquivClasses      ************************/
/**************************************************************************/
/* PURPOSE: PLACE ALL SYMBOL TABLE TYPES INTO ONE THEIR APPROPRIATE       */
/*          EQUIVALENT CLASSES. WHEN DONE, lclass + 1 ADDRESSES THE NEXT  */
/*          AVAILABLE SLOT IN THE EQUIVALENCE CLASS TABLE.                */
/**************************************************************************/

void InitEquivClasses()
{
  register PINFO i;
  register int   c;

  for ( c = 0; c < MaxClass; c++ ) {
    htable[ c ] = NULL;
    ttable[ c ] = NULL;
  }

  for ( i = ihead; i != NULL; i = i->next ) {
    i->mnext = NULL;
    i->eid   = i->type;

    if ( htable[ i->eid ] == NULL ) {
      htable[ i->eid ] = ttable[ i->eid ] = i;
      continue;
    }

    ttable[ i->eid ]->mnext = i;
    ttable[ i->eid ] = i;
  }

  lclass = BASE_CODE_LAST;		/* LAST LEGAL IF2 TYPE */
}

/* $Log$
 * Revision 1.2  1994/02/15  23:20:32  miller
 * Allow new IF1 types (Typeset, complex, etc...)
 *
 * Revision 1.1  1993/04/16  19:00:25  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:57  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
