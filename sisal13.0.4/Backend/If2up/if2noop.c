/* if2noop.c,v
 * Revision 12.7  1992/11/04  22:05:11  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************        InsertNoOp         ************************/
/**************************************************************************/
/* PURPOSE: FORCE THE AGGREGATE REPRESENTED BY EDGE e TO PASS THROUGH A   */
/*          NoOp NODE BEFORE REACHING ITS IMMEDIATE DESTINATION.  THE     */
/*          NoOp EXPORT IS MARKED TO REFLECT THAT THE DESTINATION CAN     */
/*          OPERATE ON THE AGGREGATE IN-SITU: ASSIGNING RMARK and omark.  */
/*          THE ALLOCATED NODE IS APPENDED TO THE GLOBAL NoOp NODE LIST.  */
/**************************************************************************/

static void InsertNoOp( e, omark )
PEDGE e;
int omark;
{
    register PNODE n;
    register PEDGE ee;

    n  = NodeAlloc( ++maxint, IFNoOp );
    ee = EdgeAlloc( n, 1, e->dst, e->iport );

    n->line  = e->dst->line;       /* USED BY UpIf2Count TO SUMMARIZE NoOps */
    n->file  = e->dst->file;
    n->funct = e->dst->funct;

    LinkNode( e->dst->npred, n );

    CopyPragmas( e, ee );

    ee->rmark1 = RMARK;
    ee->omark1 = omark;
    ee->info   = e->info;

    LinkExport( n, ee );
    UnlinkImport( e );
    LinkImport( e->dst, ee );

    e->iport = 1;
    LinkImport( n, e );

    AppendToUtilityList( nohead, notail, n );
}

/**************************************************************************/
/* LOCAL  **************      AddNoOpsToGraph      ************************/
/**************************************************************************/
/* PURPOSE: ADD NoOp NODES TO GRAPH g. IMPLICITY ALL AGGREGATE MODIFIERS  */
/*          WORK IN-SITU GIVEN THE SIDE EFFECT WILL NOT ALTER PROGRAM     */
/*          CORRECTNESS.  NoOp NODES ARE INSERTED TO GUARANTEE AT COMPILE */
/*          TIME THAT SUCH MODIFIERS CAN WORK IN-SITU---THE QUESTION TO   */
/*          COPY IS NOW ANSWERED BY THE NoOp NODE AND NOT THE MODIFIER.   */
/*          POTENTIAL CONSTANT AGGREGATE GENERATORS ARE PLACED ON THE     */
/*          THE CONSTANT AGGREGATE GENERATOR LIST.                        */
/**************************************************************************/

static void AddNoOpsToGraph( g )
PNODE g;
{
    register PNODE n;
    register PEDGE e;

    AppendToUtilityList( chead, ctail, g );

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	switch ( n->type ) {
	    case IFCall:
                AppendToUtilityList( chead, ctail, n );
		break;

	    case IFRReplace:
		InsertNoOp( n->imp, TRUE );
		break;

	    case IFACatenateAT:
		if ( n->imp->pmark ) 
		    InsertNoOp( n->imp, FALSE );
                /* else if ( n->imp->isucc->pmark )
		    InsertNoOp( n->imp->isucc, FALSE ); */

                if ( n->imp->isucc->pmark ) /* CANN 10-4 */
		    InsertNoOp( n->imp->isucc, FALSE );

		break;

	    case IFAAddLAT:
            case IFAAddHAT:
		if ( n->imp->pmark )
		    InsertNoOp( n->imp, FALSE );

		break;

	    case IFACatenate:                /* MUST BE A STREAM CATENATE */
                n->imp->pmark = n->imp->isucc->pmark = TRUE;
		InsertNoOp( n->imp, FALSE );
		InsertNoOp( n->imp->isucc , FALSE );
		break;

	    case IFAScatter:                  /* ONLY FOR STREAM SCATTERS */
		if ( !IsStream( n->imp->info ) )
		    break;

	    case IFAAddH:                     /* MUST BE A STREAM_APPEND  */
		/* AAddH MODIFICATION 4/25/90 */
		if ( IsArray( n->imp->info ) )
		  break;

                n->imp->pmark = TRUE;
		InsertNoOp( n->imp, FALSE );
                break;

	    case IFASetL:
	    case IFARemL:                     /* ARRAYS AND STREAMS       */
	    case IFARemH:
	    case IFAAdjust:
		n->imp->pmark = TRUE;
		InsertNoOp( n->imp, FALSE );
		break;

            case IFAReplace:
		InsertNoOp( n->imp, TRUE );
		break;

	    case IFSelect:
	    case IFTagCase:
	    case IFForall:
	    case IFLoopA:
	    case IFLoopB:
		AppendToUtilityList( chead, ctail, n );

		for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		    AddNoOpsToGraph( g );

		break;

	    case IFRBuild:
		if ( IsUnion( n->exp->info ) )
		    break;

	    case IFABuildAT:
	    case IFAFillAT:
		if ( n->type == IFABuildAT || n->type == IFAFillAT ) {
		  for ( e = n->exp; e != NULL; e = e->esucc )
		    if ( IsLoop( e->dst ) )
		      if ( IsExport( e->dst->L_INIT, e->iport ) ) 
		        break;

		  if ( e != NULL ) break;
		  }

	    case IFMemAlloc:
	    case IFDefArrayBuf:
		if ( !cagg )
		    break;

		/* NO EMPTY ARRAY CONSTANTS ALLOWED! */
                if ( n->type == IFABuildAT )
		  if ( n->imp->isucc->isucc == NULL )
		    break;

		n->cmark = TRUE;
                AppendToUtilityList( cohead, cotail, n );
		break;

	    default:
		break;
	    }
}


/**************************************************************************/
/* GLOBAL **************          If2NoOp          ************************/
/**************************************************************************/
/* PURPOSE: INSERT NoOp NODES IN ALL FUNCTION GRAPHS.  THE ALLOCATED NoOp */
/*          NODES ARE APPENDED TO THE GLOBAL NoOp NODE LIST AND COMPOUND  */
/*          NODES ARE LINKED TO THE GLOBAL COMPOUND NODE LIST; BOTH USED  */
/*          TO ENHANCE PERFORMANCE OF LATER OPTIMIZATION SUBPHASES.       */
/**************************************************************************/

void If2NoOp()
{
    register PNODE f;

    for ( f = fhead; f != NULL; f = f->gsucc )
	AddNoOpsToGraph( f );

}
