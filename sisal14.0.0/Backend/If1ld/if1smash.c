/* if1smash.c,v
 * Revision 12.7  1992/11/04  22:04:54  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:07:16  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

/**************************************************************************/
/* GLOBAL **************       LoadSmashTypes      ************************/
/**************************************************************************/
/* PURPOSE: GIVE STRUCTURALLY EQUIVALENT TYPES THE SAME LABEL. ONLY ONE   */
/*          TYPE FROM EACH EQUIVALENT CLASS IS MARKED FOR PRINTING.       */
/*                                                                        */
/* NOTE:    ORIGINALLY WRITTEN IN PASCAL BY sks AT LLNL:  1/10/83.        */
/**************************************************************************/

void LoadSmashTypes()
{
    register int   chgd = TRUE;
    register int   c;
    register PINFO p;
    register PINFO m;
    register PINFO r;

    InitEquivClasses();

    while ( chgd ) {
        chgd = FALSE;

        for ( c = 0; c <= lclass; c++ ) {
	    r = htable[c];
	    p = r;

	    if ( p == NULL )
	        m = NULL;
	    else
	        m = p->mnext;

	    while ( m != NULL )
	        if ( SameEquivClass( r, m ) ) {
		    p = m;
		    m = p->mnext;
	        } else {
		    GatherOthers( p, m );
		    chgd = TRUE; m = NULL;
		    }
	    }
        }    

    PointToHead();
}
