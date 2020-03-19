/* if2smash.c,v
 * Revision 12.7  1992/11/04  22:05:03  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************   AdjustInfoReferences    ************************/
/**************************************************************************/
/* PURPOSE: ADJUST ALL INFO REFERENCES IN GRAPH g SO THEY ADDRESS THE     */
/*          FIRST MEMBER OF EACH EQUIVALENCE CLASS.                       */
/**************************************************************************/

static void AdjustInfoReferences( g )
PNODE g;
{
  register PNODE sg;
  register PNODE n;
  register PEDGE i;

  for ( n = g; n != NULL; n = n->nsucc ) {
    if ( n->info != NULL )
      if ( n->info->fmem != NULL )
        n->info = n->info->fmem;

    for ( i = n->imp; i != NULL; i = i->isucc )
      if ( i->info != NULL )
	if ( i->info->fmem != NULL )
	  i->info = i->info->fmem;

    /* BUG FIX cann 7/15/91 */
    if ( IsCompound( n ) || (n->type >= IFFirstSum && n->type <= IFFirstAbsMax))
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	AdjustInfoReferences( sg );
    }
}


/**************************************************************************/
/* GLOBAL **************       GenSmashTypes       ************************/
/**************************************************************************/
/* PURPOSE: GIVE STRUCTURALLY EQUIVALENT TYPES THE SAME LABEL. THEN       */
/*          RELABEL ALL SYMBOL TABLE ENTRIES TO APPEAR IN THE TYPE TABLE. */
/*          NOTE: ORIGINALLY WRITTEN IN PASCAL BY sks AT LLNL:  1/10/83.  */
/*          ONLY THE FIRST MEMBER OF EACH EQUIVALENCE CLASS IS KEEPED IN  */
/*          THE SYMBOL TABLE.  THE IF2 GRAPH INFO POINTERS ARE ADJUSTED   */
/*          ACCORDINGLY.                                                  */
/**************************************************************************/

void GenSmashTypes()
{
    register int   chgd = TRUE;
    register int   c;
    register PINFO p;
    register PINFO m;
    register PINFO r;
    register PINFO ppred;
    register PINFO ps;
    register PNODE f;

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

    /* RELABEL ALL NODES TO APPEAR IN THE TYPE TABLE                      */

    for ( c = 0, p = ihead; p != NULL; p = p->next )
	switch ( p->type ) {
	    case IF_TUPLE:
	    case IF_FUNCTION:
	    case IF_UNKNOWN:
	    case IF_NONTYPE:
	    case IF_MULTIPLE:
	    case IF_BUFFER:
		continue;

	    default:
		/* ONLY ADJUST LABEL OF FIRST EQUIVALENCE CLASS MEMBER    */
		if ( p->fmem == NULL )
		    p->label = ++c;

                break;
            }

    /* IN EACH CLASS, ADJUST THE LABEL OF EACH MEMBER TO THAT OF THE 1ST  */
    for ( p = ihead; p != NULL; p = p->next )
	if ( p->fmem != NULL )
	    p->label = p->fmem->label;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
      AdjustInfoReferences( f );

    /* FIX integer AND VARIOUS OTHER SPECIAL TYPES */
    if ( integer != NULL )
      if ( integer->fmem != NULL )
	integer = integer->fmem;

    if ( ptr_integer != NULL )
      if ( ptr_integer->fmem != NULL )
	ptr_integer = ptr_integer->fmem;

    if ( ptr_real != NULL )
      if ( ptr_real->fmem != NULL )
	ptr_real = ptr_real->fmem;

    if ( ptr_double != NULL )
      if ( ptr_double->fmem != NULL )
	ptr_double = ptr_double->fmem;

    if ( ptr != NULL )
      if ( ptr->fmem != NULL )
	ptr = ptr->fmem;

    /* ONLY KEEP THE FIRST MEMBER OF EACH EQUIVALENCE CLASS IN THE SYMBOL */
    /* TABLE.                                                             */
    for ( ppred = NULL, p = ihead; p != NULL; p = ps ) {
      if ( p->info1 != NULL )
	if ( p->info1->fmem != NULL )
	  p->info1 = p->info1->fmem;

      if ( p->info2 != NULL )
	if ( p->info2->fmem != NULL )
	  p->info2 = p->info2->fmem;

      ps = p->next;

      if ( p->fmem == NULL ) {
	p->mnext = NULL;
	ppred    = p;
	continue;
	}

      if ( ppred == NULL )
	ihead = ps;
      else
	ppred->next = p->next;

      p->type = IF_UNKNOWN; /* HOPEFULLY, THIS WILL HELP UNCOVER BUGS! */
      }

    itail = ppred;
}
