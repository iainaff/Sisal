/* if2refcnt.c,v
 * Revision 12.7  1992/11/04  22:05:11  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:03  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************      AssignPMPragmas      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN PRAGMA pm=FANOUT+v TO THE AGGREGATE EXPORTS OF NODE n. */
/**************************************************************************/

static void AssignPMPragmas( n, v )
PNODE n;
int   v;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( !IsAggregate( e->info ) )
	    continue;

	e->pm = UsageCount( n, e->eport ) + v;
        }
}


/**************************************************************************/
/* GLOBAL **************      AssignCMPragmas      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN PRAGMA cm=v TO ALL AGGREGATE IMPORTS OF NODE n.        */
/**************************************************************************/

void AssignCMPragmas( n, v )
PNODE n;
int   v;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( !IsAggregate( i->info ) )
	    continue;

        i->cm = v;
	}
}


/**************************************************************************/
/* LOCAL  **************      AssignSRPragmas      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN PRAGMA sr=FANOUT TO ALL AGGREGATE EXPORTS OF NODE n.   */
/**************************************************************************/

static void AssignSRPragmas( n )
PNODE n;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( !IsAggregate( e->info ) )
	    continue;

        e->sr = UsageCount( n, e->eport );
        }
}


/**************************************************************************/
/* LOCAL  **************   AssignReferenceCounts   ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN DEFAULT REFERENCE COUNT PRAGMAS TO ALL AGGREGATE NODES */
/*          IN GRAPH g.  THE FOLLOWING ASSIGNMENTS ARE MADE:              */
/*                                                                        */
/*          NoOp       ---->    cm=-1    (1)NODE(1)     sr=FANOUT   ----> */
/*                                                                        */
/*          RBuild                                                        */
/*          ABuild(STR)                                                   */
/*          ASetL                                                         */
/*          ARemL                                                         */
/*          ARemH                                                         */
/*          AAdjust                                                       */
/*          ABuildAT                                                      */
/*          AFillAT                                                       */
/*          AReplace                                                      */
/*          RReplace   ---->    cm=0  (1..N)NODE(1)     sr=FANOUT   ----> */
/*                                                                        */
/*          ACatAT     ---->    cm=0    P(1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=-1    (2)                              */
/*          ACatAT     ---->    cm=-1    (1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=0    P(2)                              */
/*          ACatAT     ---->    cm=-1    (1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=-1    (2)                              */
/*                                                                        */
/*          AAdd[LH]AT ---->    cm=-1  ~P(1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=0     (2)                              */
/*          AAdd[LH]AT ---->    cm=0     (1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=0     (2)                              */
/*                                                                        */
/*          ACat(STR)  ---->    cm=-1    (1)NODE(1)     sr=FANOUT   ----> */
/*                              cm=-1    (2)                              */
/*                                                                        */
/*          FinalValue(AT) -->  cm=-1    (1)NODE(1)     pm=FANOUT-1 ----> */
/*          AGather(AT)    -->  cm=0     (1)NODE(1)     sr=FANOUT   ----> */
/*          RedCat(ALL)    -->  cm=-1    (3)NODE(1)     sr=FANOUT   ----> */
/*                                                                        */
/*          AIsEmpty(STR)                                                 */
/*          ASize                                                         */
/*          ALimL                                                         */
/*          ALimH      ---->    cm=-1    (1)NODE(1)     NOTHING     ----> */
/*                                                                        */
/*          AScatter                                                      */
/*          RElements                                                     */
/*          AElement   ---->    cm=-1    (1)NODE(1..N)  pm=FANOUT   ----> */
/*                                                                        */
/*          Call       ---->    cm=-1 (1..N)NODE(1..M)  pm=FANOUT-1 ----> */
/*                                                                        */
/*          FG         ---->    cm=0  (1..N) FG (1..M)  pm=FANOUT   ----> */
/*                                                                        */
/*          Select                                                        */
/*          TagCase    ---->    cm=-1 (1..N)NODE(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..R) SG (1..M)  pm=FANOUT   ----> */
/*                                                                        */
/*          Forall     ---->    cm=-1 (1..N)NODE(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..N)GENE(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..N)BODY(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..R)RETU(1..M)  pm=FANOUT   ----> */
/*                                                                        */
/*          LoopA/B    ---->    cm=-1 (1..N)NODE(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..N)INIT(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..N)BODY(1..M)  pm=FANOUT   ----> */
/*                     ---->    cm=-1 (1..R)RETU(1..M)  pm=FANOUT   ----> */
/**************************************************************************/

static void AssignReferenceCounts( g )
PNODE g;
{
    register PNODE n;
    register PEDGE i;

    AssignPMPragmas( g, 0 );
      
    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	switch ( n->type ) {
	    case IFCall:
		AssignCMPragmas( n, -1 );
		AssignPMPragmas( n, -1 );
		break;

	    case IFRElements:
	    case IFAElement:                /* IMPORTS AN ARRAY OR STREAM */
		n->imp->cm = -1;
		AssignPMPragmas( n, 0 );
		break;

	    case IFAAddH:                             /* IMPORTS A STREAM */
	    case IFNoOp:
		n->imp->cm = -1;
		AssignSRPragmas( n );
		break;

	    case IFABuild:                            /* EXPORTS A STREAM */
	    case IFRBuild:
	    case IFABuildAT:
	    case IFAFillAT:
	    case IFASetL:
	    case IFARemL:                   /* IMPORTS AN ARRAY OR STREAM */
	    case IFARemH:
	    case IFAAdjust:
	    case IFRReplace:
	    case IFAReplace:
		AssignSRPragmas( n );
		break;

	    case IFACatenateAT:
		if ( n->imp->pmark )
		    n->imp->isucc->cm = -1; 
                else if ( n->imp->isucc->pmark )
		    n->imp->cm = -1; 
                else
		    n->imp->cm = n->imp->isucc->cm = -1;

		AssignSRPragmas( n );
		break;

	    case IFAAddLAT:
            case IFAAddHAT:
		if ( !(n->imp->pmark) )
		    n->imp->cm = -1;

                AssignSRPragmas( n );
		break;

	    case IFACatenate:                          /* IMPORTS STREAMS */
                n->imp->cm = n->imp->isucc->cm = -1;
		AssignSRPragmas( n );
		break;

	    case IFRedTree:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFReduce:
		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

	    case IFRedTreeAT:             /* Reduce?ATs RETURN AGGREGATES */
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFReduceAT:
                n->imp->isucc->isucc->cm = -1;
		AssignSRPragmas( n );
		break;

	    case IFAScatter:                /* IMPORTS AN ARRAY OR STREAM */
		n->imp->cm = -1;
                AssignPMPragmas( n, 0 );
		break;

	    case IFFinalValue:
		if ( !IsAggregate( n->exp->info ) )
		    break;

	    case IFFinalValueAT:
		n->imp->cm = -1;
		AssignPMPragmas( n, -1 );
		break;

	    case IFAGather:                 /* IMPORTS AN ARRAY OR STREAM */
	    case IFAGatherAT:
		AssignSRPragmas( n );
		break;

	    case IFPeek:
		for ( i = n->imp; i != NULL; i = i->isucc ) {
		  if ( !IsAggregate( i->info ) )
		    continue;

		  i->cm = -1;
		  }
                
		break;

	    case IFAIsEmpty:                          /* IMPORTS A STREAM */
	    case IFASize:
	    case IFALimL:
	    case IFALimH:
		n->imp->cm = -1;
		break;

	    case IFTagCase:
	    case IFSelect:
	    case IFForall:
	    case IFLoopA:
	    case IFLoopB:
		AssignCMPragmas( n, -1 );
		AssignPMPragmas( n,  0 );

		for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) {
		    AssignCMPragmas( g, -1 );
		    AssignReferenceCounts( g );
		    }

                break;

	    default:
		break;
	    }
}


/**************************************************************************/
/* GLOBAL **************      If2ReferenceCount    ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN DEFAULT REFERENCE COUNT PRAGMAS TO ALL FUNCTION GRAPHS.*/
/**************************************************************************/

void If2ReferenceCount()
{
    register PNODE f;

    for ( f = fhead; f != NULL; f = f->gsucc )
	AssignReferenceCounts( f );
}
