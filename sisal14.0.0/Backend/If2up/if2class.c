/* if2class.c,v
 * Revision 12.7  1992/11/04  22:05:10  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:01  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void  ClassifyGraphEdges();

/**************************************************************************/
/* LOCAL  **************         IsWritten         ************************/
/**************************************************************************/
/* PURPOSE: EXAMINE ALL EXPORTS OF NODE n WITH EXPORT PORT NUMBER eport,  */
/*          RETURING TRUE IF A WRITE CLASSIFIED EDGE IS ENCOUNTERED.      */
/**************************************************************************/

static int IsWritten( n, eport )
PNODE n;
int   eport;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->eport == eport )
	    if ( e->wmark )
		return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************     AssignWriteMarks      ************************/
/**************************************************************************/
/* PURPOSE: UNCONDITIONALLY CLASSIFY ALL AGGREGATE IMPORTS OF NODE n AS   */
/*          WRITES.                                                       */
/**************************************************************************/

static void AssignWriteMarks( n )
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
	    i->wmark = TRUE;
}


/**************************************************************************/
/* LOCAL  **************     ClassifyKImports      ************************/
/**************************************************************************/
/* PURPOSE: CLASSIFY THE AGGREGATE K IMPORTS TO COMPOUND NODE c.  FIRST,  */
/*          THE EDGES IN ALL SUBGRAPHS ARE CLASSIFIED.  THEN EACH K       */
/*          IMPORT REFERENCE IS EXAMINED; A K IMPORT IS ASSIGNED A WRITE  */
/*          CLASSIFICATION ONLY IF AN ASSOCIATED REFERENCE WITHIN ONE OF  */
/*          THE SUBGRAPHS' IS CLASSIFIED AS A WRITE. NOTE: IMPORTS TO     */
/*          STREAM PRODUCING LOOPS ARE AUTOMATICALLY CLASSIFIED AS WRITES */
/*          AND THE FIRST IMPORT TO A TageCase IS AUTOMATICALLY ASSIGNED  */
/*          A READ STATUS.                                                */
/**************************************************************************/

static void ClassifyKImports( c )
PNODE c;
{
    register PNODE g;
    register PEDGE i;
    register int   s  = FALSE;

    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
        ClassifyGraphEdges( g );

    if ( IsForall( c ) || IsLoop( c ) )
	if ( IsStream( c->exp->info ) )
	    s = TRUE;

    for ( i = c->imp; i != NULL; i = i->isucc ) {
	if ( !IsAggregate( i->info ) )
	    continue;

        if ( s ) {
	    i->wmark = TRUE;
	    continue;
	    }

	if ( IsTagCase( c ) && (i->iport == 1) )
	    continue;

	for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
	    if ( IsWritten( g, i->iport ) ) {
		i->wmark = TRUE;
		break;
		}
	}
}


/**************************************************************************/
/* LOCAL  **************     ClassifyGraphEdges    ************************/
/**************************************************************************/
/* PURPOSE: CLASSIFY THE EDGES IN GRAPH g. THE FOLLOWING CLASSIFICATIONS  */
/*          ARE ASSIGNED:                                                 */
/*                                                                        */
/*          GRAPHS            ----> WRITE (1..N)  GRAPH                   */
/*                                                                        */
/*          COUMPOUND NODES   REFER TO AssignKImports                     */
/*                                                                        */
/*          CALL              IF BROKEN TO ELIMINATE A CALL GRAPH CYCLE,  */
/*                            THEN ASSIGN WRITE TO ALL AGGREGATE IMPORTS. */
/*                            ELSE FIND THE CALLED FUNCTION AND ASSIGN    */
/*                            WRITE TO EACH ACTUAL ARGUMENT THAT HAS A    */
/*                            IMMEDIATE WRITE REFERENCE IN THE FUNCTION.  */
/*                                                                        */
/*          AAdd[HL]AT        THE SECOND IMPORT IS CLASSIFIED AS WRITE    */
/*                            AND SO IS THE FIRST IF IT HAS A P MARK.     */
/*                                                                        */
/*          ACatAT            THE FIRST IMPORT IS CLASSIFIED AS A WRITE   */
/*                            IF IT HAS A P MARK.  THE SECOND IMPORT IS   */
/*                            CLASSIFIED AS A WRITE IF IT HAS A P MARK    */
/*                            AND THE FIRST DOESN'T.                      */
/*                                                                        */
/*          AddH, ABuildAT, ABuild, AFillAT, RReplace, RBuild, AReplace,  */
/*          AGather, AGatherAT, FinalValueAT, FinalValue, NoOp, ASetL,    */
/*          ARemL, ARemH, AAdjust, ACatenate, AScatter(STREAM):           */
/*                                                                        */
/*                            ALL AGGREGATE IMPORTS ARE CLASSIFIED AS     */
/*                            WRITES.                                     */
/*                                                                        */
/*          NOTE: INITIALLY, ALL EDGES ARE CLASSIFIED AS READS: DONE      */
/*                DURING GRAPH CONSTRUCTION.                              */
/**************************************************************************/

static void ClassifyGraphEdges( g )
PNODE g;
{
    register PNODE n;
    register PEDGE i;
    register PNODE f;

    AssignWriteMarks( g );

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	switch ( n->type ) {
	    case IFForall:
	    case IFLoopA:
	    case IFLoopB:
	    case IFTagCase:
	    case IFSelect:
		ClassifyKImports( n );
		break;

	    case IFCall:
                if ( n->bmark ) {
		    AssignWriteMarks( n );
		    break;
		    }

		f = FindFunction( n->imp->CoNsT );

		if ( IsIGraph( f ) )
		    break;

                for ( i = n->imp->isucc; i != NULL; i = i->isucc )
		    if ( IsAggregate( i->info ) )
			if ( IsWritten( f, i->iport - 1 ) ) {
			    i->wmark = TRUE;
			    }

		break;

	    case IFAAddHAT:
	    case IFAAddLAT:
		if ( n->imp->pmark )
		    n->imp->wmark = TRUE;

		n->imp->isucc->wmark = TRUE;
		break;

	    case IFACatenateAT:
		if ( n->imp->pmark )
		    n->imp->wmark = TRUE;
		/* else if ( n->imp->isucc->pmark )
                    n->imp->isucc->wmark = TRUE; */

		if ( n->imp->isucc->pmark ) /* CANN 10-4 */
                    n->imp->isucc->wmark = TRUE;

		break;


	    case IFAScatter:
		if ( IsArray( n->imp->info ) )
		    break;

	    case IFAGather:
	    case IFAGatherAT:
	    case IFFinalValue:
	    case IFFinalValueAT:
	    case IFRReplace:
	    case IFRBuild:
	    case IFAFillAT:
	    case IFABuildAT:
	    case IFNoOp:
	    case IFASetL:
	    case IFARemL:                    /* IMPORTS A STREAM OR ARRAY */
	    case IFARemH:
	    case IFAAdjust:
            case IFAReplace:
	    case IFABuild:                            /* EXPORTS A STREAM */
		AssignWriteMarks( n );
		break;

	    case IFACatenate:                         /* IMPORTS STREAMS  */
		AssignWriteMarks( n );
		break;

	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		if ( n->imp->CoNsT[0] == REDUCE_CATENATE )
		    if ( IsStream( n->exp->info ) ) {
		      n->imp->isucc->isucc->wmark = TRUE;
		      ststr = FALSE;
		      }

		break;

	    case IFAAddH:                             /* IMPORTS A STREAM */
	        if ( IsAggregate( n->imp->isucc->info ) )
		    n->imp->isucc->wmark = TRUE;

		/* if ( IsArray( n->imp->info ) ) */
		n->imp->wmark = TRUE;
		break;

	    default:
		break;
	    }
}


/**************************************************************************/
/* GLOBAL **************         If2Classify       ************************/
/**************************************************************************/
/* PURPOSE: CLASSIFY THE AGGREGATE EDGES IN ALL FUNCTION GRAPHS AS READ   */
/*          OR WRITE DEPENDING ON THEIR DESTINATION NODE. THE CALL GRAPH  */
/*          IS PROCESSED FROM THE LEAVES UP.                              */
/**************************************************************************/

void If2Classify()
{
    register PNODE f;

    for ( f = fhead; f != NULL; f = f->gsucc )
	ClassifyGraphEdges( f );
}
