/* if2rwset.c,v
 * Revision 12.7  1992/11/04  22:05:12  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************    PropagateGlobalSets    ************************/
/**************************************************************************/
/* PURPOSE: IF EDGE e'S DESTINATION IS AN AElement OR RElements NODE THEN */
/*          ASSIGN ITS EXPORTS e'S GLOBAL READ AND WRITE SETS.  THIS IS   */
/*          CONTINUED UNTIL ALL DEREFERNCE PATHS BEGINNING WITH e ARE     */
/*          TRAVERSED.                                                    */
/**************************************************************************/

static void PropagateGlobalSets( e )
PEDGE e;
{
    register PEDGE ee;

    switch ( e->dst->type ) {
	case IFAElement:
	case IFRElements:
	    if ( (e->iport == 1) && (!IsStream( e->info )) )
		for ( ee = e->dst->exp; ee != NULL; ee = ee->esucc ) {
		    ee->grset = e->grset;
		    ee->gwset = e->gwset;

		    PropagateGlobalSets( ee );
		    }

            break;

        default:
	    break;
        }
}


/**************************************************************************/
/* LOCAL  **************      BuildGlobalSets      ************************/
/**************************************************************************/
/* PURPOSE: CREATE AND ASSIGN EMPTY GLOBAL READ/WRITE SETS TO THE EXPORTS */
/*          OF NODE n.  EACH REFERENCE TO A GIVEN EXPORT IS ASSIGNED THE  */
/*          SAME SET PAIR.  IF THE USER OF AN EXPORT IS A DEREFERENCE     */
/*          NODE, THE ASSIGNED SET PAIR IS PROPAGATED TO THE NODE'S       */
/*          EXPORTS, WHICH MAY THEMSELVES ADDRESS OTHER DEREFERENCE NODES.*/
/*          EACH ALLOCATED GLOBAL SET PAIR IS APPENDED TO THE GLOBAL SET  */
/*          LIST IN READ->WRITE ORDER. IF n IS NOT A GRAPH NODE, THE gen  */
/*          FIELD OF EACH ALLOCATED SET IS ASSIGNED THE ASSOCIATED EXPORT */
/*          TO IDENTIFY THE VALUES ABSOLUTE ORIGIN DURING LATER ANALYSIS. */
/**************************************************************************/

static void BuildGlobalSets( n, scope )
PNODE n;
PNODE scope;
{
    register PEDGE e;
    register PEDGE ee;
    register PSET  wset;
    register PSET  rset;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        if ( !IsAggregate( e->info ) )
            continue;

        if ( e->grset != NULL )
            continue;

        wset = SetAlloc( NULL_SET, scope );
        rset = SetAlloc( wset, scope );

	if ( !IsGraph( n ) )
	    wset->gen = rset->gen = e;

        if ( gshead == NULL )
            gshead = rset;
        else
            gstail->ssucc = rset;

        gstail = wset;


        for ( ee = e; ee != NULL; ee = ee->esucc ) {
            if ( e->eport == ee->eport ) {
                ee->grset = rset;
                ee->gwset = wset;

                PropagateGlobalSets( ee );
                }

            }
        }
}


/**************************************************************************/
/* LOCAL  **************      BuildLocalSets       ************************/
/**************************************************************************/
/* PURPOSE: CREATE, FILL, AND ASSIGN LOCAL READ/WRITE SETS TO THE EXPORTS */
/*          OF NODE n. EACH REFERENCE TO A GIVEN EXPORT IS ASSIGNED THE   */
/*          SAME SET PAIR.  IF AN EDGE IS CLASSIFIED AS A WRITE, IT IS    */
/*          ENTERED IN BOTH ITS ASSOCIATED LOCAL WRITE SET AND GLOBAL     */
/*          WRITE SET (CREATED By BuildGlobalSets).  A READ CLASSIFIED    */
/*          EDGE IS ENTERED IN IT'S ASSOCIATED LOCAL AND GLBOAL READ SETS.*/
/**************************************************************************/

static void BuildLocalSets( n, scope )
PNODE n;
PNODE scope;
{
    register PEDGE e;
    register PEDGE ee;
    register PSET  rset;
    register PSET  wset;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        if ( !IsAggregate( e->info ) )
            continue;

        if ( e->lrset != NULL )
            continue;

        rset = SetAlloc( NULL_SET, scope );
        wset = SetAlloc( NULL_SET, scope );

        for ( ee = e; ee != NULL; ee = ee->esucc )
            if ( e->eport == ee->eport ) {
                ee->lrset = rset;
                ee->lwset = wset;

                if ( ee->wmark ) {
                    EnterInSet( ee->lwset, ee );
                    EnterInSet( ee->gwset, ee );
                } else {
                    EnterInSet( ee->lrset, ee );
                    EnterInSet( ee->grset, ee );
                    }
                }
        }
}


/**************************************************************************/
/* LOCAL  **************     BuildReadWriteSets    ************************/
/**************************************************************************/
/* PURPOSE: BUILD LOCAL AND GLOBAL READ/WRITE SETS IN GRAPH g. A GLOBAL   */
/*          SET ORIGINATES FROM EITHER A COMPOUND NODE, CALL NODE, GRAPH  */
/*          NODE, OR AGGREGATE GENERATOR NODE.  LOCAL SETS ARE ALLOCATED  */
/*          FOR AGGREGATE EXPORTS OF ALL NODES.  IMMEDIATE REFERENCES TO  */
/*          THE SAME EXPORT SHARE THE SAME SETS, BUT ONLY GLOBAL SETS ARE */
/*          PROPAGATED THROUGH AElement AND RElements NODES; HENCE THEIR  */
/*          NAME "GLOBAL". THE GLOBAL SETS ARE APPENDED TO A GLOBAL SET   */
/*          LIST IN READ->WRITE PAIRS.                                    */
/**************************************************************************/

static void BuildReadWriteSets( g )
PNODE g;
{
    register PNODE n;
    register PNODE sg;

    for ( n = g; n != NULL; n = n->nsucc )       /* g AND ITS GRAPH NODES */
	switch ( n->type ) {
	    case IFForall:         case IFLoopA:         case IFLoopB:
	    case IFTagCase:        case IFSelect:        
		for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	            BuildReadWriteSets( sg );

	    case IFSGraph:         case IFXGraph:        case IFLGraph:

	    case IFCall:
	    
	    case IFAAddHAT:        case IFAAddLAT:       case IFACatenateAT:
	    case IFAReplace:       case IFASetL:         case IFARemL:
	    case IFARemH:          case IFAAdjust:       case IFABuildAT:
	    case IFAFillAT:        case IFRBuild:        case IFRReplace:
	    case IFABuild:         case IFAAddH:         case IFACatenate:

		BuildGlobalSets( n, g );
		BuildLocalSets(  n, g );
		break;

            case IFAElement:
	    case IFRElements:
                if ( !IsConst( n->imp ) )
		    if ( IsNoOp( n->imp->src ) )
			BuildGlobalSets( n, g );

		BuildLocalSets( n, g );
		break;

            default:
		break;
            }
}


/**************************************************************************/
/* GLOBAL **************      If2ReadWriteSets     ************************/
/**************************************************************************/
/* PURPOSE: CONSTRUCT THE READ/WRITE SETS IN ALL FUNCTION GRAPHS.         */
/**************************************************************************/

void If2ReadWriteSets()
{
    register PNODE f;

    for ( f = fhead; f != NULL; f = f->gsucc )
	BuildReadWriteSets( f );
}
