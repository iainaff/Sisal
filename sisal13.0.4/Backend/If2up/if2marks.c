/* if2marks.c,v
 * Revision 12.7  1992/11/04  22:05:11  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void PropagateMarks();

static int lnstable  = 0;   /* COUNT OF RETRIES DURING LOOP PROCESSING    */
static int fnstable  = 0;   /* COUNT OF FUNCTION RETRIES BECAUSE UNSTABLE */
static int fnvisited = 0;   /* COUNT OF UNVISITED FUNCTIONS               */


/**************************************************************************/
/* LOCAL  **************        CatAtBugFix        ************************/
/**************************************************************************/
/* PURPOSE: MAKE SURE THE FOLLOWING EXECUTES CORRECTLY; THAT IS THAT THE  */
/*          REPLACE DOES NOT WORK IN PLACE!                               */
/*                                                                        */
/*          let x := for ...; y := for ...; in x || y, y[1:2000] end let  */
/*                                                                        */
/**************************************************************************/

static int CatAtBugFix( e, omark )
PEDGE e;
int   omark;
{
    register PEDGE p;

    for ( p = e->src->exp; p != NULL; p = p->esucc )
	if ( p->eport == e->eport )
	    if ( p->pmark && (!p->wmark) )
		break;

    if ( p == NULL )
	return( omark );

    p->omark1 = FALSE;

    if ( !omark )
	return( FALSE );

    /* MIGHT READ WRONG VALUE! */
    if ( UsageCount( e->src, e->eport ) != 2 )
	return( FALSE );

    if ( SetSize( e->lwset ) != 1 )
	return( FALSE );

    /* ONLY WRITE NOT A GROUND: MIGHT CORRUPT LIVE DATA */
    if ( e->lwset->set[0]->iport != 0 )
	return( FALSE );

    /* p IS READ ONCE THEN DEALLOCATED: NOT SURE IF LAST REFERENCE? */
    p->omark1 = TRUE;
    return( FALSE );
}


/**************************************************************************/
/* GLOBAL **************     If2RefineGrounds      ************************/
/**************************************************************************/
/* PURPOSE: FOR ALL DATA GROUNDS, IMPORTS TO GRAPHS AT PORT 0 OR L PORT   */
/*          VALUES, HAVING THE mk=R PRAGMA, ADD THE mk=D PRAGMA AND       */
/*          CHANGE THE cm=-1 PRAGMA TO CM=0;                              */
/**************************************************************************/

void If2RefineGrounds()
{
    register PEDGE e;
    register PEDGE ee;
    register PEDGE ii;

    for ( e = dghead; e != NULL; e = e->usucc ) {
	if ( e->rmark1 == RMARK ) {
	    e->dmark = TRUE;
	    e->cm    = 0;
	    }

	if ( e->rmark1 == rMARK )
	  if ( e->sr == 0 && e->pm == 0 )
	    if ( IsForall( e->src ) ) {
              for ( ee = e->src->exp; ee != NULL; ee = ee->esucc )
	        if ( ee->eport == e->eport )
		  if ( ee->wmark )
		    goto MoveOn;


	      ii = FindImport( e->src->F_RET, e->eport );

	      if ( ii->esucc != NULL ) 
		goto MoveOn; 

	      if ( ii->src->type != IFAGatherAT )
		goto MoveOn;

	      if ( ii->sr != 1 )
		goto MoveOn;

	      ii = FindImport( ii->src, 4 ); /* BUFFER */

	      if ( IsImport( e->src->F_GEN, ii->eport ) )
		goto MoveOn;

	      if ( IsImport( e->src->F_BODY, ii->eport ) )
		goto MoveOn;

	      ii = FindImport( e->src, ii->eport );

	      if ( ii->esucc != NULL )
		goto MoveOn;

	      if ( ii->src->type != IFMemAlloc )
		goto MoveOn;

	      e->rmark1 = RMARK;
	      e->omark1 = TRUE;
	      e->dmark  = TRUE;
	      e->cm     = 0;
	      }

MoveOn:
        /* PART OF CatenateAT BUG FIX */
        for ( ee = e->src->exp; ee != NULL; ee = ee->esucc )
	    if ( ee->eport == e->eport )
		if ( ee->pmark && !(ee->wmark) ) {
	            e->omark1 = FALSE;
		    break;
		    }
        }
}


/**************************************************************************/
/* LOCAL  **************         BindRMark         ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN rmark TO rmark1 OF ALL EDGES IN SET s.                 */
/**************************************************************************/

static void BindRMark( s, rmark )
PSET s;
int  rmark;
{
    register PEDGE e;
    register int   m;

    for ( m = 0; m <= s->last; m++ ) {
	e = s->set[m];
	e->rmark1 = rmark;
	}
}


/**************************************************************************/
/* LOCAL  **************  ConditionallyBindOMark   ************************/
/**************************************************************************/
/* PURPOSE: IF ALL MEMBERS OF SET s HAVE NoOp NODE DESTINATIONS AND DON'T */
/*          HAVE mk=P PRAGMAS, THEN ASSIGN omark TO omark1 OF EACH MEMBER.*/
/**************************************************************************/

static void ConditionallyBindOMark( s, omark )
PSET s;
int  omark;
{
    register PEDGE e;
    register int   m;

    for ( m = 0; m <= s->last; m++ ) {
	e = s->set[m];

	if ( !IsNoOp( e->dst ) )
	    return;

	if ( e->pmark )
	    return;
        }

    for ( m = 0; m <= s->last; m++ ) {
	e = s->set[m];
	e->omark1 = omark;
	}
}


/**************************************************************************/
/* LOCAL  **************        AssignMarks        ************************/
/**************************************************************************/
/* PURPOSE: BASED ON THE PRODUCER REFERENCE COUNT VALUE OF THE AGGREGATE  */
/*          REPRESENTED BY EDGE e, ASSIGN APPROPRIATE R AND O MARKS TO    */
/*          ITS WRITE REFERENCES. IF e'S SOURCE NODE IS A PURE-GENERATOR, */
/*          GOVERED BY AN sr MODIFIER, AND THE MODIFIERS VALUE IS ONE,    */
/*          THEN THE ONLY WRITE OPERATION CAN UPDATE THE AGGREGATE'S DOPE */
/*          VECTOR IN-SITU AND POSSIBLY ITS PHYSICAL SPACE (GOVERNED BY   */
/*          omark). IF THE VALUE IS NOT 1, BUT IS EQUAL TO THE NUMBER OF  */
/*          WRITE REFERENCES, THEN THERE IS A POSIBILITY THAT AT RUNTIME  */
/*          ONE OF THE WRITE OPERATIONS CAN OPERATE ON THE DOPE VECTOR    */
/*          IN-SITU AND POSSIBLY THE PHYSICAL SPACE (GOVERNED BY omark,   */
/*          WHICH CAN ONLY BE ASSIGNED IF ALL WRITE OPERATIONS ARE FULL   */
/*          COPY NoOp NODES).                                             */
/*                                                                        */
/*          IF THE SOURCE NODE IS NOT A PURE-GENERATOR, GOVERED NOT BY A  */
/*          sr MODIFIER, BUT A pm MODIFIER, AND THE MODIFIER'S VALUE IS   */
/*          ZERO THEN THE ONLY WRITE OPERATION CAN WORK IN-SITU IF rmark, */
/*          PROPAGATED TO THE SOURCE NODE, IS RMARK, ELSE THE DECISION    */
/*          CANNOT BE MADE UNTIL RUNTIME.  HERE omark IS ASSINGED TO THE  */
/*          THE WRITE REFERENCE.  IF THE VALUE IS NOT ZERO, BUT IS EQUAL  */
/*          TO THE NUMBER OF WRITE OPERATIONS MINUS ONE, THEN ALL READ    */
/*          OPERATIONS WILL EXECUTE BEFORE THE WRITES AND ONE OF THE      */
/*          WRITES MIGHT BE ABLE TO EXECUTE IN-SITU---AGAIN NOT KNOWN     */
/*          UNTIL RUNTIME; HERE IF ALL WRITES ARE FULL COPY NoOp NODES    */
/*          THEN omark CAN BE ASSIGNED.                                   */
/**************************************************************************/

static void AssignMarks( e, rmark, omark )
PEDGE e;
int   rmark;
int   omark;
{
    register PEDGE w;
    register int   wc;

    if ( e == NULL )
	return;

    omark = CatAtBugFix( e, omark ); /* CANN */

    wc = SetSize( e->lwset );

    if ( e->sr != -2 ) {
	if ( e->sr == -1 )
            Error1( "AssignMarks (HELP!): e->sr == -1" );
	else if ( e->sr == 1 ) {
	    if ( wc != 1 )  /* x := record rec [a:1]; DoIt(x) returns x.a */
		return;

	    w = e->lwset->set[0];
	    w->rmark1 = RMARK;
	    w->omark1 = omark;
	    }
	else if ( wc == e->sr ) {
	    BindRMark( e->lwset, rMARK );
	    ConditionallyBindOMark( e->lwset, omark );
	    }
	}
    else if ( e->pm <= -1 )
        Error1( "AssignMarks (HELP!): e->pm <= -1" );
    else if ( e->pm == 0 ) {
	if ( wc != 1 )                               /* AElement -> READS */
	    return;

	w = e->lwset->set[0];

	if ( rmark == RMARK )
	    w->rmark1 = RMARK;
        else
	    w->rmark1 = rMARK;

        w->omark1 = omark;
	}
    else if ( wc == e->pm + 1 ) {
	BindRMark( e->lwset, rMARK );
	ConditionallyBindOMark( e->lwset, omark );
	}
}


/**************************************************************************/
/* LOCAL  **************    PropagateCallMarks     ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE IMPORT MARKS OF CALL NODE c THROUGH THE CALLEE  */
/*          FUNCTION GRAPH AND ASSIGN THE RESULTING MARKS TO c'S EXPORTS. */
/*          IF THE CALLEE IS IMPORTED IT MUST BE AN INTRINSIC TAKING AND  */
/*          RETURNING NONAGGREGATES.  IF THE CALLEE IS BROKEN, UNKNOWN    */
/*          MARKS ARE ASSIGNED TO c'S EXPORTS.  IF THE PROPAGATION CAUSES */
/*          AN INSTABILITY THE OFFENDING FORMAL ARGUMENTS ARE FLAGED FOR  */ 
/*          LATER PROCESSING.                                             */
/**************************************************************************/

static void PropagateCallMarks( c )
PNODE c;
{
    register PNODE f;
    register PEDGE e;
    register PEDGE i;
    register int   s;
    register int   ss;

    f = FindFunction( c->imp->CoNsT );

    if ( IsIGraph( f ) )
	return;

    if ( f->bmark ) {
	for ( e = c->exp; e != NULL; e = e->esucc )
	    if ( IsAggregate( e->info ) )
                AssignMarks( e, NOMARK, FALSE );

	return;
	}

    /* IF THE CALLEE HAS YET TO BE SEEN DURING PROPAGATION, INITIALIZE IT */
    /* AND PROPAGATE THE MARKS.                                           */

    if ( !(f->visited) ) {
	for ( i = c->imp->isucc; i != NULL; i = i->isucc )
	    if ( IsAggregate( i->info ) )
	        for ( e = f->exp; e != NULL; e = e->esucc )
		    if ( (i->iport - 1) == e->eport ) {
			e->rmark2 = i->rmark1;
			e->omark2 = i->omark1;

			AssignMarks( e, (int)i->rmark1, (int)i->omark1 );
			}

	f->visited = TRUE;
	PropagateMarks( f );
    } else {
        /* NOTE IF THE PROPAGATION PROCESS WILL CAUSE AN INSTABILITY.  IF */
        /* IT DOESN'T, THEN THE CALLEE'S EXPORT MARKS ARE VALID FOR THIS  */
	/* CALL SITE AND THE RESULTS CAN BE IMMEDIATELY PROPAGATED TO THE */
	/* CALL NODE'S EXPORTS, ELSE NOT WHO CAUSED THE INSTABILITY AND   */
	/* PERFORM THE PROPAGATION PROCESS.                               */

	s = TRUE;

	for ( i = c->imp->isucc; i != NULL; i = i->isucc )
	    if ( IsAggregate( i->info ) )
		for ( e = f->exp; e != NULL; e = e->esucc ) {
		    ss = TRUE;

                    if ( (i->iport - 1) == e->eport ) {
			if ( i->rmark1 != e->rmark2 ) {
			    e->rstable = FALSE;
			    e->rmark2  = i->rmark1;
			    ss = FALSE;
			    }

                        if ( i->omark1 != e->omark2 ) {
			    e->ostable = FALSE;
			    e->omark2  = i->omark1;
			    ss = FALSE;
			    }

			if ( !ss )
			    AssignMarks( e, (int)i->rmark1, (int)i->omark1 );
			}

		    s &= ss;
		    }

	if ( !s )
	    PropagateMarks( f );
	}

    /* MOVE THE RESULT MARKS TO THE CALL NODE'S EXPORTS                   */

    for ( i = f->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
	    AssignMarks( FindExport( c, i->iport ), (int)i->rmark1, (int)i->omark1 );
}


/**************************************************************************/
/* LOCAL  **************   PropagateSelectMarks    ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE IMPORT MARKS OF SELECT NODE s THROUGH ALL       */
/*          SUBGRAPHS AND ASSIGN THE APPROPRIATE MARKS TO s'S EXPORTS.    */
/*          IF THE MARKS ON EACH SUBGRAPH'S IMPORT EDGE ASSOCIATED WITH   */
/*          THE SAME SELECT NODE EXPORT ARE THE SAME,  THEY MAY BE        */
/*          PROPAGATED TO THE EXPORT, ELSE UNKNOWNS ARE ASSIGNED.  THIS   */
/*          ROUTINE IS BUILT TO DEAL WITH BOTH Select AND TagCase NODES.  */ 
/*          THE FIRST IMPORT (IMPORT PORT NUMBER ONE) OF A TagCase NODE   */
/*          IS ALWAYS ASSUMED TO HAVE UNKNOWN MARKS (ALWAYS READ ONLY).   */
/**************************************************************************/

static void PropagateSelectMarks( s )
PNODE s;
{
    register PNODE g;
    register PEDGE i;
    register PEDGE e;
    register int   sr;
    register int   so;
    register PEDGE ii;
    register PNODE sg;

    /* ASSIGN s IMPORT MARKS TO ALL CORRESPONDING SUBGRAPH EXPORTS AND */
    /* AND PROPAGATE THEM THROUGH THE SUBGRAPHS.                       */

    for ( g = s->C_SUBS; g != NULL; g = g->gsucc ) {
	for ( i = s->imp; i != NULL; i = i->isucc )
	    if ( IsAggregate( i->info ) ) {
	        e = FindExport( g, i->iport );

	        if ( IsTagCase( s ) && (i->iport == 1) )
		    continue;

                AssignMarks( e, (int)i->rmark1, (int)i->omark1 );
		}

	PropagateMarks( g );
	}

    /* USING THE FIRST SUBGRAPH AS A REFERENCE POINT, SUMMARIZE THE       */
    /* RESULTING MARKS AND ASSIGN THE APPROPRAITE MARKS TO ALL s EXPORTS. */

    if ( IsSelect( s ) )
	g = s->C_SUBS->gsucc;
    else
	g = s->C_SUBS;

    for ( i = g->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) ) {
	    /* GROUND EDGE?                                               */
	    if ( i->iport == 0 )
		continue;

	    sr = so = TRUE;

	    for ( sg = g->gsucc; sg != NULL; sg = sg->gsucc ) {
		ii = FindImport( sg, i->iport );

		if ( i->rmark1 != ii->rmark1 )
		    sr = FALSE;

                if ( i->omark1 != ii->omark1 )
		    so = FALSE;
		}

	    e = FindExport( s, i->iport );
            AssignMarks( e, (int)((sr)? i->rmark1 : NOMARK), 
			    (int)((so)? i->omark1 : FALSE ) );
	    }
}


/**************************************************************************/
/* LOCAL  **************   PropagateForallMarks    ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE IMPORT MARKS OF FORALL NODE f THROUGH ALL       */
/*          SUBGRAPHS AND ASSIGN THE APPROPRIATE MARKS TO f'S EXPORTS.    */
/*          IMPORT MARKS CAN ONLY BE PROPAGATED TO GENERATE SUBGRAPHS AND */
/*          THIS CAN ONLY BE DONE IF THE K PORT VALUE IS NOT REFERENCED   */
/*          IN THE BODY OR RETURN SUBGRAPH.  ALL BODY ARGUMENTS ARE       */ 
/*          ASSUMED TO HAVE UNKNOWN MARKS.  THE FINAL T PORT MARKS ARE    */
/*          PROPAGATED TO THE RETURN SUBGRAPH AND DRIVEN THROUGH ALL      */
/*          NODES BEFORE MARKS ARE ACCORDINGLY ASSIGNED TO f EXPORTS.     */ 
/**************************************************************************/

static void PropagateForallMarks( f )
PNODE f;
{
    register PEDGE i;
    register PEDGE e;

    /* IF AN f IMPORT IS ONLY REFERENCED IN ITS GENERATE SUBGRAPH THEN    */
    /* PROPAGATE THE IMPORT'S MARKS THROUGH THE SUBGRAPH.                 */

    for ( i = f->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) ) {
	    if ( IsExport( f->F_BODY, i->iport ) || 
		 IsExport( f->F_RET,  i->iport )  )
		continue;

            e = FindExport( f->F_GEN, i->iport );
            AssignMarks( e, (int)i->rmark1, (int)i->omark1 );
	    }

    PropagateMarks( f->F_GEN  );

    /* PROPAGATE MARKS WITHIN THE BODY SUBGRAPH                           */

    PropagateMarks( f->F_BODY );

    /* ASSIGN AND PROPAGATE T PORT MARKS THROUGH THE RETURN SUBGRAPH      */

    for ( i = f->F_BODY->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
	    if ( (e = FindExport( f->F_RET, i->iport )) != NULL )
	        AssignMarks( e, (int)i->rmark1, (int)i->omark1 );

    PropagateMarks( f->F_RET );

    /* MOVE MARKS ASSIGNED TO THE RESULT EDGES OF THE RETURN SUBGRAPH TO  */
    /* THE APPROPRIATE EXPORTS OF f.                                      */

    for ( i = f->F_RET->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
            AssignMarks( FindExport( f, i->iport ), (int)i->rmark1, (int)i->omark1 );
}


/**************************************************************************/
/* LOCAL  **************     PropagateLoopMarks    ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE IMPORT MARKS OF LOOP NODE l THROUGH ALL         */
/*          SUBGRAPHS AND ASSIGN THE APPROPRIATE MARKS TO l'S EXPORTS.    */
/*          IMPORT MARKS CAN ONLY BE PROPAGATED TO THE INIT SUBGRAPHS AND */
/*          THIS CAN ONLY BE DONE IF THE K PORT VALUE IS NOT REFERENCED   */
/*          IN THE BODY OR RETURN SUBGRAPH, IS ONLY REFERENCED ONCE IN    */
/*          THE INIT SUBGRAPH, AND IS THE INITIAL VALUE OF AN L VALUE     */
/*          THAT IF REFERENCED IN THE RETURN SUBGRAPH, IS ONLY READ OR    */
/*          IMPORTED TO A FinalValue OR FinalValueAT NODE. THE INITIAL L  */ 
/*          VALUE MARKS ARE PROPAGATED THROUGH THE BODY SUBGRAPH; IF THE  */
/*          MARKS OF THE REDEFINED L VALUE DO NOT MATCH THE INITIAL L     */
/*          VALUE MARKS THEN THE INITIAL MARKS ARE ADJUSTED AND THE       */
/*          PROPAGATION IS REPEATED.  ONCE DONE, THE FINAL L VALUE MARKS  */
/*          ARE DRIVEN THROUGH THE RETURN SUBGRAPH AND ASSIGNED TO THE    */
/*          LOOP'S EXPORTS. MARK AJUSTMENT PROCEEDS AS FOLLOWS: IF THE    */
/*          CURRENT INITIAL MARK IS mk=R THEN IT CAN BECOME mk=r; IF THE  */
/*          CURRENT INITIAL MARK IS mk=r THEN IT IS GROUNDED (mk=NOMARK). */
/*          AN O MARK IS SET TO NOMARK DURING ADJUSTMENT.                 */
/**************************************************************************/

static void PropagateLoopMarks( l )
PNODE l;
{
    register PEDGE e;
    register PEDGE i;
    register PEDGE ee;
    register PEDGE ii;
    register int   ocnt;
    register int   d;
    register int   dd;

    /* CLEAN UP MARKS FROM PREVIOUS LOOP EXAMINATION                      */

    for ( e = l->L_INIT->exp; e != NULL; e = e->esucc )
	if ( IsAggregate( e->info ) ) {
	    e->rmark1 = NOMARK;
	    e->omark1 = FALSE;
	    }

    /* ASSIGN LOOP IMPORT MARKS TO APPROPRIATE INIT SUBGRAPH REFERENCES   */

    for ( e = l->L_INIT->exp; e != NULL; e = e->esucc ) {
	if ( !IsAggregate( e->info ) )
	    continue;

	if ( IsExport( l->L_BODY, e->eport ) || 
	     IsExport( l->L_RET,  e->eport ) || 
	     (UsageCount( l->L_INIT, e->eport ) != 1) )
	    continue;

	for ( ocnt = 0, ee = l->L_RET->exp; ee != NULL; ee = ee->esucc )
	    if ( ee->eport == e->iport )
		switch ( ee->dst->type ) {
		    case IFFinalValueAT:
		    case IFFinalValue:
			if ( !(ee->dst->lmark) )
			    ocnt++;

			break;

		    default:
			if ( ee->wmark )
			    ocnt++;

			break;
		    }

	if ( ocnt != 0 )
	    continue;

	i = FindImport( l, e->eport );

	e->rmark1 = i->rmark1;
	e->omark1 = i->omark1;
	}

    /* ASSIGN THE INITIAL MARKS TO ALL ASSOCIATED REFERENCES              */

    for ( i = l->L_INIT->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
            AssignMarks( FindExport(l->L_BODY, i->iport), (int)i->rmark1, (int)i->omark1);

    /* DRIVE THE MARKS THROUGH THE BODY SUBGRAPH. IF THE INITIAL L VALUE  */
    /* MARKS DON'T MATCH THE ASSOCIATED REDEFINITION MARKS THEN ADJUST    */
    /* THE INITIAL MARKS AND REPEAT.                                      */

    do  {
	d = TRUE;
	PropagateMarks( l->L_BODY );

	for ( i = l->L_INIT->imp; i != NULL; i = i->isucc ) {
	    ii = FindImport( l->L_BODY, i->iport );
            dd  = TRUE;

	    if ( i->rmark1 != ii->rmark1 ) {
		if ( IsExport( l->L_BODY, i->iport ) ) {
		    if ( i->rmark1 != rMARK ) {
			dd = FALSE;
			i->rmark1 = rMARK;
			}
		    }
                else
	            i->rmark1 = NOMARK;
		}

	    if ( i->omark1 != ii->omark1 )
                if ( i->omark1 ) {
		    if ( IsExport( l->L_BODY, i->iport ) )
			dd = FALSE;

                    i->omark1 = FALSE;
		    }

	    if ( !dd )
	        AssignMarks( FindExport( l->L_BODY, i->iport ), 
			     (int)i->rmark1, (int)i->omark1 );

	    d &= dd;
	    }

	if ( !d )
	    lnstable++;
	}
    while ( !d );

    /* ASSIGN THE FINAL INITIAL MARKS TO THE ASSOCIATED REFERENCES IN THE */
    /* RETURN SUBGRAPH AND DRIVE THEM THROUGH THE SUBGRAPH.               */

    for ( i = l->L_INIT->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
	    AssignMarks( FindExport(l->L_RET, i->iport), (int)i->rmark1, (int)i->omark1 );

    PropagateMarks( l->L_RET );

    /* ASSIGN THE RESULT MARKS IN THE RETURN SUBGRAPH TO THE APPROPRIATE  */
    /* LOOP EXPORTS                                                       */

    for ( i = l->L_RET->imp; i != NULL; i = i->isucc )
	if ( IsAggregate( i->info ) )
	    AssignMarks( FindExport( l, i->iport ), (int)i->rmark1, (int)i->omark1 );
}


/**************************************************************************/
/* LOCAL  **************      PropagateMarks       ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE MARKS PREVIOUSLY ASSIGNED TO GRAPH g's EXPORT   */
/*          LIST TO ALL RELEVENT NODES IN WITHIN THE GRAPH.  BECAUSE THE  */
/*          NODES ARE VISITED IN DATA FLOW ORDER, ALL NODE IMPORT MARKS   */
/*          ARE READY FOR PROPAGATION.                                    */
/**************************************************************************/

static void PropagateMarks( g )
PNODE g;
{
    register PNODE n;
    register PEDGE e;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	switch( n->type ) {
	    case IFCall:
		PropagateCallMarks( n );
		break;

            case IFSelect:
	    case IFTagCase:
		PropagateSelectMarks( n );
		break;

            case IFForall:
		PropagateForallMarks( n );
		break;

	    case IFLoopA:
	    case IFLoopB:
		PropagateLoopMarks( n );
		break;

	    case IFABuildAT:
	    case IFRBuild:
	    case IFRReplace:
	    case IFAFillAT:
	    case IFAReplace:
            case IFABuild:
		AssignMarks( n->exp, RMARK, TRUE );
		break;

	    case IFAScatter:
		if ( IsArray( n->imp->info ) )
		    break;

		n->imp->dmark = TRUE;
		n->imp->cm    = 0;
		break;

	    case IFACatenate:
		AssignMarks( n->exp, RMARK, TRUE );

		n->imp->dmark = n->imp->isucc->dmark = TRUE;
		n->imp->cm    = n->imp->isucc->cm    = 0;
		break;

	    case IFAAddH:
		AssignMarks( n->exp, RMARK, TRUE );

		/* AAddH MODIFICATION 4/25/90 */
		/* THE IF ADDED---CONTENTS IS NOT NEW!*/
		if ( !IsArray( n->imp->info ) ) {
		    n->imp->dmark = TRUE;
		    n->imp->cm    = 0;
		    }

		break;

	    /* ATTEMPTED PSA BUG FIX: 4/20/90 */
	    /* case IFAElement:
		for ( e = n->exp; e != NULL; e = e->esucc )
		    if ( e->info->type == IF_ARRAY )
		        AssignMarks( e, NOMARK, FALSE );

                break; */

	    case IFAElement:
            case IFRElements:
		for ( e = n->exp; e != NULL; e = e->esucc )
		    if ( !IsBasic( e->info ) )
		        AssignMarks( e, NOMARK, FALSE );

                break;


            case IFASetL:
	    case IFARemL:
	    case IFARemH:
	    case IFAAdjust:
		AssignMarks( n->exp, RMARK, (int)n->imp->omark1 );
		break;

            case IFAAddHAT:
	    case IFAAddLAT:
		if ( n->imp->pmark )
		    AssignMarks( n->exp, RMARK, (int)n->imp->omark1 );
		else
		    AssignMarks( n->exp, RMARK, TRUE );

		break;

	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

	    case IFAGatherAT:
            case IFAGather:
		n->exp->rmark1 = RMARK;
		n->exp->omark1 = TRUE;
		break;

	    case IFFinalValueAT:
            case IFFinalValue:
		n->exp->rmark1 = n->imp->rmark1;
		n->exp->omark1 = n->imp->omark1;
		break;

            case IFReduceAT:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		n->exp->rmark1 = RMARK;

		if ( n->imp->isucc->isucc->pmark )
		    n->exp->omark1 = n->imp->isucc->isucc->omark1;
		else
		    n->exp->omark1 = TRUE;

		break;

            case IFNoOp:
		if ( n->imp->pmark )
		    for ( e = n->exp; e != NULL; e = e->esucc ) {
			if ( n->imp->rmark1 == RMARK )
			    e->omark1 = n->imp->omark1;
                        else
			    e->omark1 = FALSE;
                        }

		break;

            case IFACatenateAT:
		if ( n->imp->pmark ) {
		    if ( n->imp->isucc->pmark ) {
		        if ( n->imp->omark1 && n->imp->isucc->omark1 )
			    AssignMarks( n->exp, RMARK, TRUE  );
                        else
			    AssignMarks( n->exp, RMARK, FALSE );
                        }
                    else
		        AssignMarks( n->exp, RMARK, (int)n->imp->omark1 );
                    }
                else if ( n->imp->isucc->pmark )
		    AssignMarks( n->exp, RMARK, (int)n->imp->isucc->omark1 );
                else
		    AssignMarks( n->exp, RMARK, TRUE );

		break;

	    default:
		break;
            }
}


/**************************************************************************/
/* GLOBAL **************     If2PropagateMarks     ************************/
/**************************************************************************/
/* PURPOSE: BEGINNING WITH FUNCTION MAIN DRIVE R, r, NOMARK, AND O marks  */
/*          THROUGH EACH FUNCTION GRAPH.  INITIALLY IT IS ASSUMED THAT    */
/*          AGGREGATE ARGUMENTS TO THE MAIN FUNCTION FROM THE OUTSIDE     */
/*          WORLD HAVE mk=RO marks---GUARANTEED BY THE FIBRE PROCESSOR.   */
/*          WHEN A NONBROKEN CALL NODE IS ENCOUNTERED, THE ACTUAL         */
/*          ARGUMENT'S MARKS ARE PROPAGATED THROUGH THE CALLEE AND THE    */
/*          RESULTING MARKS ARE USED TO COMPLETE LOCAL PROCESSING.  IF    */
/*          ANOTHER CALL SITE HAS PROPAGATED DIFFERENT MARKS TO THE       */
/*          CALLEE, THEN THE AGGREGATE FORMALS OF INTEREST ARE FLAGGED AS */
/*          UNSTABLE---IN FINAL PROCESSING OF EACH FUNCTION GRAPH NOMARKS */
/*          ARE ASSUMED FOR EACH UNSTABLE ARGUMENT. FUNCTIONS NOT VISITED */
/*          DURING INITIAL TRAVERSAL OF THE CALL GRAPH WHERE BROKEN TO    */
/*          ELIMINATE CYCLES AND ARE TRAVERSED DURING THE STABALIZING     */
/*          PASS OVER THE CALL GRAPH.  THE CALL GRAPH IS TRAVERED FROM    */
/*          THE ROOT TO THE LEAVES; HENCE ALL PROPAGATIONS ACROSS         */
/*          FUNCTION BOUNDARIES MOVE TOWARD THE LEAVES.                   */
/**************************************************************************/

void If2PropagateMarks()
{
    register PEDGE e;
    register PNODE f;
    register int   s;
    register int   ss;
    register int   trmark;
    register int   tomark;

    /* INITIALIZE THE FIBRE ARGUMENTS TO THE MAIN FUNCTION                */

    for ( f = fhead; f != NULL; f = f->gsucc ) {

      if ( IsIGraph( f ) ) /* NEW CANN 2/92 */
	continue;

      /* if ( f->emark && (!f->bmark) && (!f->visited) ) { */

      /* NEW CANN: IF NOT A SISAL MODULE && NOT CALLED INTERNALLY AND NOT    */
      /* ALREADY VISITED THEN....                                            */
      if ((f->mark != 's' && f->mark != ' ') && (!f->bmark) && (!f->visited)) {

        for ( e = f->exp; e != NULL; e = e->esucc )
	    if ( IsAggregate( e->info ) ) {
	        e->rmark2 = RMARK;
	        e->omark2 = TRUE;

	        AssignMarks( e, RMARK, TRUE );
	        }

	/* MARK THE MAIN FUNCTION AS VISITED AND PROPAGATE FIBRE INPUT    */
	/* MARKS THROUGH THE GRAPH AND ALL ITS CALLEES NOT CYCLE ENTRY    */
	/* POINTS.                                                        */

        f->visited = TRUE;
        PropagateMarks( f );

	}

      /* NEW CANN: THERE MIGHT BE A CHANCE FOR MODULE ENTRY POINTS */
      /* LOOKUP WOULD HELP!!! */
      else if ( f->mark == 's' && (!f->visited) && (!f->bmark) ) {
        for ( e = f->exp; e != NULL; e = e->esucc )
	    if ( IsAggregate( e->info ) ) {
	        e->rmark2 = rMARK;
	        e->omark2 = FALSE;

	        AssignMarks( e, rMARK, FALSE );
	        }

	/* MARK THE MAIN FUNCTION AS VISITED AND PROPAGATE SISAL MODULE   */
	/* INPUT MARKS THROUGH THE GRAPH AND ALL ITS CALLEES NOT CYCLE    */
	/* ENTRY POINTS.                                                  */

        f->visited = TRUE;
        PropagateMarks( f );
	}

      }


    /* STABALIZE ALL FUNCTION GRAPHS AS SOME FUNCTIONS MAY HAVE INHERITED */
    /* DIFFERENT ARGUMENT MARKS FROM DIFFERENT CALL SITES.  IF SO, IT IS  */
    /* ASSUMED THAT NOTHING IS KNOWN ABOUT THE ARGUMENTS.                 */

    for ( f = ftail; f != NULL; f = f->gpred ) {
	if ( IsIGraph( f ) )
	    continue;

        /* A NON VISITED FUNCTION WAS BROKEN TO REMOVE A RECURSIVE CYCLE. */
	/* EACH ARGUMENT'S MARKS ARE ASSUMED UNKOWN AND DRIVEN THROUGH    */
	/* THE CALL GRAPH.                                                */

	if ( !(f->visited) ) {
	    for ( e = f->exp; e != NULL; e = e->esucc )
		if ( IsAggregate( e->info ) ) {
		    e->rmark2 = NOMARK;
		    e->omark2 = FALSE;

		    AssignMarks( e, NOMARK, FALSE );
		    }

            f->visited = TRUE;
	    fnvisited++;
	    PropagateMarks( f );

	    continue;
	    }

        /* IF AN ARUMENT TO THE FUNCTION IS NOT STABLE, REPEAT THE MARK   */
	/* PROPAGATION SUBSTITUTING UNKNOWN FOR THE OFFENDING MARK.       */

	s = TRUE;

	for ( e = f->exp; e != NULL; e = e->esucc ) {
	    if ( !IsAggregate( e->info ) )
		continue;

	    ss = TRUE;

	    if ( !(e->rstable) ) {
		trmark = NOMARK;
		ss     = FALSE;
		}
            else
		trmark = e->rmark2;

            if ( !(e->ostable) ) {
		tomark = FALSE;
		ss     = FALSE;
		}
	    else
		tomark = e->omark2;

	    if ( !ss )
		AssignMarks( e, trmark, tomark );

	    s &= ss;
	    }

        if ( !s ) {
	    fnstable++;
	    PropagateMarks( f );
	    }
	}

/*    if ( RequestInfo(I_Info3,info)  ) {
      FPRINTF( infoptr, "\n **** MARK PROPAGATION BEHAVIOR\n\n" );
      FPRINTF( infoptr,   " Number of Unstable  Loops:     %d\n", lnstable  );
      FPRINTF( infoptr,   " Number of Unstable  Functions: %d\n", fnstable  );
      FPRINTF( infoptr,   " Number of Unvisited Functions: %d\n", fnvisited );
    } */
}
