/* if2mem.c,v
 * Revision 12.7  1992/11/04  22:05:06  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:22  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define AT_STACK_SIZE   4000
#define ResetAtNodeStack attop = -1

static PNODE atstack[AT_STACK_SIZE];           /* POTENTIAL AT-NODE STACK */
static int   attop;                        /* POTENTIAL AT-NODE STACK TOP */
static int tagg = 0;
int agg = 0;


/**************************************************************************/
/* GLOBAL **************         PushAtNode        ************************/
/**************************************************************************/
/* PURPOSE: PUSH POTENTIAL AT-NODE at ONTO THE AT-NODE STACK.  AN ERROR   */
/*          MESSAGE IS PRINTED IF OVERFLOW OCCURS.                        */
/**************************************************************************/

void PushAtNode( at )
PNODE at;
{
    if ( (++attop) >= AT_STACK_SIZE )
	Error2( "PushAtNode:", "AT-NODE STACK OVERFLOW" );

    atstack[attop] = at;
}


/**************************************************************************/
/* GLOBAL **************         PopAtNode         ************************/
/**************************************************************************/
/* PURPOSE: POP OFF THE TOP AT-NODE FROM THE AT-NODE STACK AND DECREMENT  */
/*          THE AT-NODE STACK TOP POINTER. IF THE AT STACK IS EMPTY, NULL */
/*          IS RETURNED.                                                  */
/**************************************************************************/

PNODE PopAtNode()
{
    if ( attop < 0 )
	return( NULL );

    return( atstack[attop--] );
}


/**************************************************************************/
/* GLOBAL **************      ReferencePoint       ************************/
/**************************************************************************/
/* PURPOSE: USING rpoint AS THE STARTING POINT AND min AS THE LEVEL OF    */
/*          THE STOPPING POINT, SEARCH FOR THE OPTIMAL LOCATION IN WHICH  */
/*          A REFERENCE TO A VALUE DEFINED AT LEVEL min SHOULD GO WITHOUT */
/*          INTRODUCING A LOOP INVARIANT OPERATION.  THIS ROUTINE ASSUMES */
/*          min IS < TO THE LEVEL OF RPOINT.                              */
/**************************************************************************/

PNODE ReferencePoint( rpoint, min )
PNODE rpoint;
int   min;
{
    register PNODE n;
    register PNODE nn;

    n = rpoint->lstack[ rpoint->level ];

    for ( ; ; ) {
	if ( !IsSGraph( n ) )            /* IF AT A FUNCTION GRAPH, STOP! */
	    return( n );

        switch ( n->G_DAD->type ) {
	    case IFLoopA:
	    case IFLoopB:
		if ( n->G_DAD->L_BODY != n )
		    return( n );

                break;

            case IFForall:
		if ( n->G_DAD->F_BODY != n )
		    return( n );

                break;

            default:
		return( n );
            }

        /* NOW WE KNOW SUBGRAPH n IS A LOOP BODY SUBGRAPH                 */

	nn = n->G_DAD->lstack[ n->G_DAD->level ];

	if ( nn->level == min )
	    return( n->G_DAD->npred );

        n = nn;
	}
}


/**************************************************************************/
/* GLOBAL **************       MaxNodeInDFO        ************************/
/**************************************************************************/
/* PURPOSE: DETERMINE WHERE A NODE IMPORTING AN EXPORT FROM BOTH NODE n1  */
/*          AND NODE n2 SHOULD BE PLACED SO NOT TO INTRODUCE AN INVARIANT */
/*          OPERATION (RETURNING ITS PREDECESSOR). NODE rpoint SERVES AS  */
/*          THE STARTING POINT IN THE SEARCH. THIS ROUTINE ASSUMES THE    */
/*          LEVEL OF BOTH n1 AND n2 ARE <= TO THAT OF rpoint.             */
/**************************************************************************/

PNODE MaxNodeInDFO( rpoint, n1, n2 )
PNODE rpoint;
PNODE n1;
PNODE n2;
{
    register PNODE n;

    if ( n1->level < rpoint->level ) {
	if ( n2->level < rpoint->level )
	    return( ReferencePoint( rpoint, Max( n1->level, n2->level ) ) );

        return( n2 );
	}

    if ( n2->level < rpoint->level )
	return( n1 );

    /* WHICH ONE IS THE RIGHTMOST NODE?                                   */

    for ( n = n1->lstack[n1->level]; n != NULL; n = n->nsucc ) {
        if ( n == n1 )
            return( n2 );

        if ( n == n2 )
            return( n1 );
        }

    Error2( "MaxNodeInDFO:", "FAILED TO LOCATE NODES IN NODE LIST" );
    return NULL;
}


/**************************************************************************/
/* GLOBAL **************       MaxSourceInDFO      ************************/
/**************************************************************************/
/* PURPOSE: DETERMINE WHERE A NODE IMPORTING EDGE e1 AND EDGE e2 SHOULD   */
/*          BE PLACED SO NOT TO INTRODUCE AN INVARIANT OPERATION; ITS     */
/*          PREDECESSOR IS RETURNED. THE EDGES MAY DEFINE CONSTANTS.      */
/*          NODE rpoint SERVES AS THE STARTING POINT IN THE SEARCH.       */
/**************************************************************************/

PNODE MaxSourceInDFO( rpoint, e1, e2 )
PNODE rpoint;
PEDGE e1;
PEDGE e2;
{
    if ( IsConst( e1 ) ) {
        if ( IsConst( e2 ) )
            return( ReferencePoint( rpoint, -1 ) );
        else if ( e2->src->level < rpoint->level )
            return( ReferencePoint( rpoint, e2->src->level ) );
        else
            return( e2->src );
        }

    if ( IsConst( e2 ) ) {
	if ( e1->src->level < rpoint->level )
            return( ReferencePoint( rpoint, e1->src->level ) );
        else
            return( e1->src );
         }

    /* BOTH SOURCE NODES EXIST, BUT MAY BE AT DIFFERENT LEVELS            */

    return( MaxNodeInDFO( rpoint, e1->src, e2->src ) );
}


/**************************************************************************/
/* LOCAL  **************      MoveNodesLeft        ************************/
/**************************************************************************/
/* PURPOSE: MOVE NON-COMPOUND NODES LEFT.  THIS WILL HELP PASS2 TO BE     */
/*          MORE SUCCESSFUL.                                              */
/**************************************************************************/

static void MoveNodesLeft( g )
PNODE g;
{
    register PNODE n;
    register PNODE sn;
    register PNODE pr;
    register PEDGE i;
    register PNODE sg;
    register int   c;

    for ( c = 0, n = g; n != NULL; n = n->nsucc ) 
	n->label = c++;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
	sn = n->nsucc;

	if ( IsCompound( n ) ) {
	    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
		MoveNodesLeft( sg );

            continue;
	    }

	switch ( n->type ) {
	    case IFABuild:
	    case IFAFill:
	    case IFACatenate:
	    case IFAAddH:
	    case IFAAddL:
	        continue;

	    default:
	        break;
            }

	pr = g;

	for ( i = n->imp; i != NULL; i = i->isucc ) {
	    if ( IsConst( i ) )
		continue;

	    if ( pr->label < i->src->label )
		pr = i->src;
	    }

	UnlinkNode( n );
	LinkNode( pr, n );

        for ( c = pr->label, n = pr; n != sn; n = n->nsucc ) 
	    n->label = c++;
        }
}

void WriteAggregateInfo ()
{
	FPRINTF (infoptr, "\n **** AGGREGATE INFORMATION\n\n");
	FPRINTF (infoptr, " Total Number of Aggregates:                    %d\n", tagg);
	FPRINTF (infoptr, " Total Number of Preallocated Aggregates:       %d\n", agg);
}


/**************************************************************************/
/* GLOBAL **************           If2Mem          ************************/
/**************************************************************************/
/* PURPOSE: TRY TO PREALLOCATE STORAGE FOR ARRAYS IN ALL FUNCTION GRAPHS. */
/**************************************************************************/

void If2Mem()
{
    register PNODE f;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
	cfunct = f;

        ResetAtNodeStack;

	AssignLevelNumbers( f );

	AssignSizes( f );
	MoveNodesLeft( f );
	AllocIf2Nodes( f );
	}
}
