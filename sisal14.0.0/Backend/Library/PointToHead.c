#include "world.h"


/**************************************************************************/
/* GLOBAL **************        PointToHead        ************************/
/**************************************************************************/
/* PURPOSE: MODIFY THE LABEL OF EACH EQUIVALENCE CLASS MEMBER, EXCEPT THE */
/*          FIRST, TO THAT OF THE FIRST (THE ONLY ONE TO BE PRINTED).     */
/*          ALSO SET THE fmem OF EACH MEMBER TO ADDRESS THE FIRST.        */
/**************************************************************************/

void PointToHead()
{
  register PINFO m;
  register PINFO r;
  register int   c;

  for ( c = 0; c <= lclass; c++ )
    if ( htable[c] != NULL ) {
      r = htable[c];

      for ( m = r->mnext; m != NULL; m = m->mnext ) {
	m->label = r->label;
	m->print = FALSE;
	m->fmem  = r;
      }
    }
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:58  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
