/* if2owner.c,v
 * Revision 12.7  1992/11/04  22:05:11  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int Sharing = FALSE;


static void DoSharingAnalysis( g )
PNODE g;
{
    register PNODE n;
    register PNODE sg;
    register PEDGE i;
    register PEDGE e;

    for ( n = g; n != NULL; n = n->nsucc ) {
        if ( IsCompound( n ) )
	    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	        DoSharingAnalysis( sg );

        if ( n->type == IFAFill || n->type == IFAFillAT )
	    if ( !IsBasic( n->imp->isucc->isucc->info ) ) {
	        Sharing = TRUE;
	        return;
	        }

        for ( i = n->imp; i != NULL; i = i->isucc ) {
	    if ( i->pm >= 1 ) {
	        Sharing = TRUE;
	        return;
	        }

	    if ( i->sr > 1 ) {
	        Sharing = TRUE;
	        return;
	        }
            }

      for ( e = n->exp; e != NULL; e = e->esucc ) {
	  if ( e->pm >= 1 ) {
	      Sharing = TRUE;
	      return;
	      }

	  if ( e->sr > 1 ) {
	      Sharing = TRUE;
	      return;
	      }
	  }
    }
}


/**************************************************************************/
/* GLOBAL **************       If2Ownership        ************************/
/**************************************************************************/
/* PURPOSE: SET univso TO TRUE IF WE CAN GUARANTEE THAT ALL STREAMS IN A  */
/*          PROGRAM HAVE ONLY ONE CONSUMER.  SET univao TO TRUE IF WE CAN */
/*          GUARANTEE THAT ALL ARRAY DOPE VECTORS OWN THEIR PHYSICAL      */
/*          SPACE; THAT IS, THE PYSICAL SPACE IS NOT SHARED BETWEEN TWO   */
/*          OR MORE DOPE VECTORS.  THE GRAPH NoOp NODES ARE UPDATED TO    */
/*          REFLECT THE FINDINGS.  NOTE: SINCE A RECORDS DOPE VECTOR IS   */
/*          THE PYSICAL SPACE FOR THE RECORD, UNIVERSAL OWNERSHIP IS      */
/*          ALWAYS GUARANTEED.  AN ARRAY NoOp NODE WILL INVALIDATE        */
/*          UNIVERSAL ARRAY OWNERSHIP IF IT HAS A mk=P PRAGMA AND NOT A   */
/*          mk=R PRAGMA.  A STREAM NoOp NODE WILL INVALIDATE UNIVERSAL    */
/*          STREAM OWNERSHIP IF IT HAS A mk=P PRAGMA (THEY ALL DO!) AND   */
/*          NOT A mk=R PRAGMA.  EACH STREAM IMPORT OF THE MAIN FUNCTION   */
/*          GRAPH MUST HAVE A mk=R PRAGMA.                                */
/**************************************************************************/

void If2Ownership()
{
    register PNODE n;
    register PEDGE i;
    register PEDGE e;
    register int   suow = TRUE;
    register int   auow = TRUE;
    register PNODE f;

    /* EXAMINE NoOp NODES TO DETERMINE UNIVERSAL OWNERSHIP                */

    for ( n = nohead; n != NULL; n = n->usucc )
	switch ( n->imp->info->type ) {
	    case IF_STREAM:
		if ( n->imp->rmark1 != RMARK )
		    suow = FALSE;

		break;

	    case IF_ARRAY:
		if ( n->imp->pmark )
		    if ( n->imp->rmark1 != RMARK )
		        auow = FALSE;

		break;

            default:
		break;
            }

    /* STREAMS IMPORTS TO THE MAIN GRAPH MUST HAVE R MARKS                */

    if ( suow )
      for ( f = fhead; f != NULL; f = f->gsucc )
	if ( IsStamp(MONOLITH) && (f->mark == 'e') ) /* NEW CANN 2/92 */
	  for ( i = f->imp; i != NULL; i = i->isucc )
	    if ( IsStream( i->info ) && (i->rmark1 != RMARK) )
	      suow = FALSE;

    /* UPDATE ALL NoOp NODES TO REFLECT THE FINDINGS                      */

    if ( ststr )
        univso = suow;
    else 
	univso = FALSE;

    univao = auow;

    if ( !IsStamp(MONOLITH) ) { /* CANN NEW 2/92 */
      univao = auow = FALSE;        /* LOOKUP, IF TRUE ACROSS ALL MODULES */
      }

    for ( n = nohead; n != NULL; n = n->usucc )
        switch ( n->imp->info->type ) {
            case IF_STREAM:
                if ( suow )
                    n->imp->omark1 = TRUE;

                break;

            case IF_ARRAY:
                if ( auow )
		    n->imp->omark1 = TRUE;

		/* CatenateAT BUG FIX */
		for ( e = n->imp->src->exp; e != NULL; e = e->esucc )
	            if ( e->eport == n->imp->eport )
		        if ( e->pmark && !(e->wmark) ) {
			    n->imp->omark1 = FALSE;
		            break;
		            }

                break;

            default:
                n->imp->omark1 = TRUE;
                break;
            }

    if ( !IsStamp(MONOLITH) ) { /* NEW CANN 2/92 */
      return;                       /* LOOKUP: in all files is there sharing? */
      }

    for ( f = fhead; f != NULL; f = f->gsucc )
	DoSharingAnalysis( f );

    if ( Sharing )
	return;

    for ( n = nohead; n != NULL; n = n->usucc )
	n->imp->rmark1 = RMARK;
}


int swcnt = 0;

void OptimizeSwaps()
{
  register PNODE n1;
  register PNODE n2;
  register PNODE r1;
  register PNODE r2;
  register PNODE a1;
  register PNODE a2;
  register PEDGE e1;
  register PEDGE e2;
  register PEDGE c;
  register PEDGE r;
  register PEDGE e;

  /* EXAMINE NoOp NODES */

  for ( n1 = nohead; n1 != NULL; n1 = n1->usucc ) {
    e = n1->imp;

    if ( e->info->type != IF_ARRAY )
      continue;

    if ( UsageCount( n1, 1 ) != 2 )
      continue;
  
    e1 = n1->exp; 
    e2 = e1->esucc;

    r1 = NULL;
    a2 = NULL;

    if ( e1->dst->type == IFAReplace ) {
      r1 = e1->dst;
      a2 = e2->dst;
      }
    else if ( e2->dst->type == IFAReplace ) {
      r1 = e2->dst;
      a2 = e1->dst;
      }
    else
      continue;

    if ( a2 == NULL || r1 == NULL )
      continue;

    if ( a2->type != IFAElement )
      continue;

    if ( !(r1->nmark) ) 
      continue;

    r = a2->imp->isucc;

    if ( r1->exp->esucc != NULL ) 
      continue;

    if ( a2->exp->pm != 0 ) 
      continue;

    if ( !AreEdgesEqual( r, r1->imp->isucc ) )
      continue;

    n2 = r1->exp->dst;

    if ( n2->type != IFNoOp )
      continue;

    if ( n2->exp->esucc != NULL )
      continue;

    r2 = n2->exp->dst;

    if ( r2->type != IFAReplace )
      continue;

    if ( IsConst( r1->imp->isucc->isucc ) )
      continue;

    a1 = r1->imp->isucc->isucc->src;

    if ( a1->type != IFAElement )
      continue;

    c = r2->imp->isucc;

    if ( a1->exp->pm != 1 )
      continue;

    if ( !AreEdgesEqual( c, a1->imp->isucc ) )
      continue;

    if ( a1->imp->src != n1->imp->src )
      continue;

    if ( a1->imp->eport != n1->imp->eport )
      continue;

    swcnt++;

    r2->nmark = TRUE;

    for ( e = a1->exp; e != NULL; e = e->esucc )
      e->pm = 0;
    }
}
