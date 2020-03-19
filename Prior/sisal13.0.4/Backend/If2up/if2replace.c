/* if2replace.c,v
 * Revision 12.7  1992/11/04  22:05:12  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************      ChangeSourceNode     ************************/
/**************************************************************************/
/* PURPOSE: UNLINK EXPORT e OF NODE osrc AND RELINK IT TO THE EXPORT LIST */
/*          OF rpl's NoOp NODE, MAKING THE APPROPRIATE PRAGMA CHANGES.    */
/**************************************************************************/

static void ChangeSourceNode( osrc, e, rpl )
PNODE osrc;
PEDGE e;
PNODE rpl;
{
    register PEDGE ee;

    for ( ee = osrc->exp; ee != NULL; ee = ee->esucc ) {
	if ( ee->eport != e->eport )
            continue;

        if ( ee->pm != -2 )
            ee->pm--;
        else
            ee->sr--;
        }

    UnlinkExport( e );

    e->eport = 1;
    e->pm    = -2;
    e->pl    = -2;
    e->sr    = rpl->imp->sr;
    e->cm    = 0;

    LinkExport( rpl->imp->src, e );
}


/**************************************************************************/
/* LOCAL  **************     FlushCompoundNodes    ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ALL COMPOUND NODES c WHOSE REFERENCES TO i ARE READS   */
/*          SO TO REFERENCE rpl->imp INSTEAD OF i. THE APPROPRIATE PRAGMA */
/*          ADJUSTMENTS ARE MADE. A BOUND ADE IS INSERTED BETWEEN ALL     */
/*          ADJUSTED c AND rpl. THIS ROUTINE IS USED TO KLUDGE AROUND     */
/*          GLOBAL COMMON SUBEXPRESSION OPTIMIZATIONS PREVENTING          */
/*          UPDATE-IN-PLACE.                                              */
/*                                                                        */
/*          x := if ( b ) then a[1,2] else a[3,4] end if;                 */
/*          y := a[1,2:1000];                                             */
/*                                                                        */
/*          ONLY DONE IF i->dst HAS AN EXPORT TO THE COMPOUND NODE        */
/*          BEING MOVED (Ex Psa.sis code).  IF THERE IS A LATER USE OF    */
/*          THE AGGREGATE, THEN ONE MIGHT WANT TO NOT MOVE THE NODE, BUT  */
/*          UNDO THE CSE (NOT IMPLEMENTED YET).                           */
/**************************************************************************/

static void FlushCompoundNodes( rpl, i )
PNODE rpl;
PEDGE i;
{
    register PEDGE e;
    register PEDGE ii;
    register PEDGE si;
    register int   m;
    register int   ok;
    register int   eport;
    register PNODE src;

    m = TRUE;

    while ( m ) {
	m = FALSE;

	for ( e = i->src->exp; e != NULL; e = e->esucc ) {
	    if ( e->eport != i->eport )
		continue;

            if ( !IsCompound( e->dst ) )
		continue;


	    /* PSA ENHANCEMENT 4/25/90 5 LINES */
            ok = FALSE;
	    for ( ii = i->dst->exp; ii != NULL; ii = ii->isucc )
	      if ( ii->dst == e->dst )
		ok = TRUE;
	    if ( !ok ) continue;

            ok = TRUE;

	    /* ARE ALL e IMPORTS TO e->dst READS? */

	    for ( ii = e->dst->imp; ii != NULL; ii = ii->isucc ) {
		if ( ii->eport != e->eport )
		    continue;

                if ( ii->src != e->src )
		    continue;

		if ( ii->wmark ) {
	            ok = FALSE;
		    break;
	            }
		}

	    if ( !ok )
		continue;

	    /* LETS NOT INTRODUCE A BOUNDED CYCLE! */
	    if ( AreNodesDependent( rpl, e->dst ) )
		continue;

	    m = TRUE;
	    break;
	    }

	if ( !m )
	    continue;

	src   = e->src;
	eport = e->eport;

	for ( ii = e->dst->imp; ii != NULL; ii = si ) {
	    si = ii->isucc;

	    if ( ii->eport != eport )
                continue;

            if ( ii->src != src )
                continue;

	    ChangeSourceNode( src, ii, rpl );
	    }

	CreateAndInsertAde( e->dst, rpl, BOUND );
	}
}


/**************************************************************************/
/* GLOBAL **************        If2Replace         ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO ORDER REPLACEMENT AND DEREFERENCE NODES TO GIVE    */
/*          OWNERSHIP OF REPLACED AGGREGATES TO NODES SELCTING THEM;      */
/*          HENCE DECREASING THE POSIBILITY OF COPYING. ONLY ARRAYS AND   */
/*          RECORDS ARE CONSIDERED. FOR EXAMPLE, A[1,2: 2010 ] COMPILES   */
/*          INTO:                                                         */
/*                                                                        */
/*                  T1 := AElem(A,1);                                     */
/*                  T2 := ARepl(T1,2,2010);                               */
/*                  T3 := ARepl(A,1,T2);                                  */
/*                                                                        */
/*          IF THE REPLACE NODE GENERATING T3 WAS GUARANTEED TO EXECUTE   */
/*          AFTER THE AElem NODE GENERATING T1, THEN OWNERSHIP OF A[1]    */
/*          COULD BE GIVEN TO THE DEREFERENCE NODE SO THAT IT CAN BE      */
/*          UPDATED IN PLACE BY THE REPLACE NODE GENERATING T2. CHANGE    */
/*          IN OWNERSHIP IMPLIES THAT ARepl(A,1,T2) WILL NOT ALTER A[1]'S */
/*          REFERENCE COUNT.  SUCH A REPLACE NODE IS GIVEN MARK n AND     */
/*          EXECUTION ORDER IS BOUND USING AN ADE RUNNING FROM THE SELECT */
/*          NODE TO THE REPLACE.  THE FIRST IMPORT OF BOTH NODES MUST BE  */
/*          THE SAME (ALSO THE SECOND IMPORT FOR ARRAY NODES).  ALL       */
/*          FUNCTIONS ARE CONSIDERED. ONLY DEREFERENCE NODES HAVING AN    */
/*          EXPORT WITH WRITE CLASSIFICATION ARE CONSIDERED.              */
/**************************************************************************/

void If2Replace()
{
    register PNODE n;
    register PNODE rpl;
    register PEDGE e;
    register PEDGE ee;
    register PEDGE i;
    register PNODE elm;
    register int   agg;
    register int   ok;

    for ( n = nohead; n != NULL; n = n->usucc ) {
	elm = NULL;
	rpl = n->exp->dst;

	switch ( rpl->type ) {
	    case IFAReplace:
		/* IS ONLY ONE MEMBER OF THE AGGREGATE REPLACED?          */
		if ( rpl->imp->isucc->isucc->isucc != NULL )
		    break;

                for ( e = n->imp->src->exp; e != NULL; e = e->esucc ) {
		    if ( e->eport != n->imp->eport )
			continue;

                    if ( !IsAElement( e->dst ) )
			continue;

		    /* IS THE DEREFERENCE RESULT AN AGGREGATE?            */
                    if ( !IsAggregate( e->dst->exp->info ) )
			continue;

		    /* ARE THE AREPLACE AND AELEMENT INDEXES THE SAME     */
		    if ( !AreEdgesEqual( e->dst->imp->isucc, rpl->imp->isucc ) )
			continue;

		    if ( HasWriteExport( e->dst ) ) {
			elm = e->dst;
			break;
			}
		    }

		break;

	    case IFRReplace:
                for ( e = n->imp->src->exp; e != NULL; e = e->esucc ) {
		    if ( e->eport != n->imp->eport )
			continue;

                    if ( !IsRElements( e->dst ) )
			continue;

                    /* DOES THE RElement NODE EXPORT AN AGGREGATE AND IS  */
		    /* IT REPLACED BY THE RReplace NODE?                  */

		    agg = FALSE;
                    ok  = TRUE; 

		    for ( ee = e->dst->exp; ee != NULL; ee = ee->esucc )
			if ( IsAggregate( ee->info ) ) {
			    agg = TRUE;

			    if ( !IsImport( rpl, ee->eport + 1 ) ) {
				ok = FALSE;
				break;
				}
                            }

		    if ( (!ok) || (!agg) )
			continue;

                    /* ARE ALL REPLACED AGGREGATES DEREFERENCED BY THE    */
		    /* RElements NODE.                                    */

		    ok = TRUE;

		    for ( i = rpl->imp->isucc; i != NULL; i = i->isucc ) {
			if ( !IsAggregate( i->info ) )
			    continue;

			if ( !IsExport( e->dst, i->iport - 1 ) ) {
			    ok = FALSE;
			    break;
			    }
                        }

		    if ( !ok )
			continue;

                    if ( HasWriteExport( e->dst ) ) {
			elm = e->dst;
			break;
			}
                    }

		break;

	    default:
		break;
	    }

	if ( elm == NULL )
	    continue;

	FlushCompoundNodes( rpl, elm->imp );
	ChangeSourceNode( elm->imp->src, elm->imp, rpl );

	/* ADJUST THE DEREFERENCE NODE'S EXPORTS PRODUCER MARKS AS THE    */
	/* NODE IS NOW CONSIDERED THE GENERATOR.                          */

	for ( e = elm->exp; e != NULL; e = e->esucc )
	    if ( IsAggregate( e->info ) )
		e->pm--;

	CreateAndInsertAde( elm, rpl, BOUND );
	rpl->nmark = TRUE;
	}
}
