/* if1normal.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:35  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void NormalizeNodes();
static void EliminateFanout();

static int latnm = 0;            /* COUNT OF LOOPA TEST NODES MOVED       */
static int fgnm  = 0;            /* COUNT OF FORALL GEN NODES MOVED       */
static int lbtnm = 0;            /* COUNT OF LOOPB TEST NODES MOVED       */
static int linm  = 0;            /* COUNT OF INITIAL NODES MOVED          */
static int redn  = 0;            /* COUNT OF REDUNDANT NOT NODE PAIRS     */
static int foldcnt = 0;          /* COUNT OF FOLDED NEGATION NODES        */
static int conv  = 0;            /* COUNT OF STRING CONVERSIONS           */
static int scnt  = 0;            /* COUNT OF REMOVED SET LOW NODES        */
static int redc  = 0;            /* COUNT OF CONVERTED ReduceCatenateS    */
static int sliml = 0;            /* COUNT OF REMOVED STREAM ALimL NODES   */
static int fan   = 0;            /* COUNT OF COMBINED EXPORTS             */
static int srcnt = 0;            /* COUNT OF CONVERTED AScatter NODES     */
static int fges  = 0;            /* COUNT OF ELIMINATED GENERATE EDGES    */
static int intc  = 0;            /* COUNT OF REMOVED Int NODES            */
static int rmf   = 0;            /* COUNT OF REMOVED FUNCTIONS            */
static int fill  = 0;            /* COUNT OF CONVERTED FILL NODES         */
static int eccnt = 0;            /* COUNT OF CONVERTED ERROR CONSTANTS    */
static int rgcnt = 0;            /* COUNT OF MOVED RangeGenerate NODES    */
static int nlog  = 0;            /* COUNT OF NORMALIZED LOGICAL NODES     */


/**************************************************************************/
/* LOCAL  **************    SimplifyGenerates      ************************/
/**************************************************************************/
/* PURPOSE: NORMALIZE THE GENERATE SUBGRAPHS OF THE Forall NODES IN GRAPH */
/*          g. THAT IS, REMOVE ALL BUT THE CONTROLLING RangeGenerate NODE */
/*          AND COMPUTE THE DOTS IN THE BODY AS A FUNCTION OF THE CONTROL */
/*          ROD. FOR EXAMPLE:                                             */
/*                                                                        */
/*           for i in 1,5 dot j in 3,7 ....   BECOMES                     */
/*                                                                        */
/*           let offset := -(1-3) in for i in 1,5  j := i+offset end let  */
/*                                                                        */
/*          THIS ROUTINE ALSO NORMALIZES LOGICAL OPERATIONS CONCERNING    */
/*          FORALL ITERATION BOUNDARIES.  THE FOLLOWING TRANSFORMATIONS   */
/*          ARE DONE GIVEN for i in 1,5 ...                               */
/*                                                                        */
/*           i > 1      ->  i ~= 1                                        */
/*           i < 5      ->  i ~= 5                                        */
/*           1 < i      ->  i ~= 1                                        */
/*           5 > 1      ->  i ~= 5                                        */
/*           # = i      ->  i  = #                                        */
/*                                                                        */
/**************************************************************************/

static void SimplifyGenerates( g )
PNODE g;
{
    register PNODE n;
    register PNODE sn;
    register PNODE sg;
    register PNODE plus;
    register PNODE neg;
    register PNODE minus;
    register PNODE rg1;
    register PNODE rg2;
    register PEDGE se;
    register PEDGE e;
    register PEDGE x;
    register PEDGE T;
    register int   t;

    register PEDGE c;
    register PEDGE hi;
    register PEDGE lo;
    register PNODE not;


    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	if ( IsCompound( n ) )
	    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
		SimplifyGenerates( sg );

	if ( !IsForall( n ) ) continue;

	/* THEY ALL MUST BE RangeGenerate NODES!!! */
	for ( rg1 = n->F_GEN->G_NODES; rg1 != NULL; rg1 = rg1->nsucc )
	    if ( rg1->type != IFRangeGenerate )
		break;

	if ( rg1 != NULL )
	  continue;

	rg1 = n->F_GEN->imp->src; /* CONTROL ROD */

	/* REMOVE ALL BUT THE CONTROL ROD */

	for ( rg2 = n->F_GEN->G_NODES; rg2 != NULL; rg2 = sn ) {
	    sn = rg2->nsucc;
	    if ( rg2 == rg1 ) continue; /* SKIP THE CONTROL ROD */

            minus = NodeAlloc( ++maxint, IFMinus );
	    CopyVitals( rg2, minus );
            neg   = NodeAlloc( ++maxint, IFNeg );
	    CopyVitals( rg2, neg );

            LinkNode( n->npred, minus );
            LinkNode( minus, neg );

            T = EdgeAlloc( neg, 1, n, t = (++maxint) );
            T->info = rg1->imp->info;
            LinkExport( neg, T );
            LinkImport( n, T );

            e = EdgeAlloc( minus, 1, neg, 1 );
            e->info = rg1->imp->info;
            LinkExport( minus, e );
            LinkImport( neg, e );

            if ( IsConst( rg1->imp ) ) {
	        e = CopyEdge( rg1->imp, NULL_NODE, minus );
                e->iport = 1;
            } else {
                e = CopyEdge((x=FindImport(n,rg1->imp->eport)),NULL_NODE,minus);
	        e->iport = 1;
        
	        if ( !IsConst( x ) ) {
	            e->src = x->src;
                    LinkExport( x->src, e );
	            }
	        }

            LinkImport( minus, e );
        
            if ( IsConst( rg2->imp ) ) {
	        e = CopyEdge( rg2->imp, NULL_NODE, minus );
                e->iport = 2;
            } else {
                e = CopyEdge((x=FindImport(n,rg2->imp->eport)),NULL_NODE,minus);
	        e->iport = 2;
        
	        if ( !IsConst( x ) ) {
	            e->src = x->src;
                    LinkExport( x->src, e );
	            }
	        }

            LinkImport( minus, e );

            plus = NodeAlloc( ++maxint, IFPlus );
	    CopyVitals( minus, plus );
	    LinkNode( n->F_BODY, plus );

            e = CopyEdge( rg1->exp, n->F_BODY, plus );
	    e->info  = rg1->imp->info;
	    e->eport = rg1->exp->iport;
	    e->iport = 1;
            LinkExport( n->F_BODY, e );
            LinkImport( plus, e );

            e = CopyEdge( T, n->F_BODY, plus );
	    e->iport = 2;
	    e->eport = t;
            LinkExport( n->F_BODY, e );
            LinkImport( plus, e );

            e = EdgeAlloc( plus, 1, n->F_BODY, t = (++maxint) );
            e->info = rg1->imp->info;
            LinkExport( plus, e );
            LinkImport( n->F_BODY, e );

	    for ( e = n->F_BODY->exp; e != NULL; e = se ) {
		se = e->esucc;
		if ( e->eport == rg2->exp->iport ) {
		    UnlinkExport( e );
		    e->eport = 1;
		    LinkExport( plus, e );
		    }
                }

	    for ( e = n->F_RET->exp; e != NULL; e = e->esucc )
		if ( e->eport == rg2->exp->iport )
		    e->eport = t;

	    UnlinkNode( rg2 );
	    UnlinkImport( rg2->exp );

	    if ( rg2->imp->isucc != NULL )
	        UnlinkExport( rg2->imp->isucc );

	    UnlinkExport( rg2->imp );
	    rgcnt++;
            }

       /* NORMALIZE CONTROL ROD LOGICALS IN BODY: MAY REMOVE A SUCCESSOR OF n */

       /* GET THE CONTROL ROD */
       c = n->F_GEN->imp;

       lo = c->src->imp;
       hi = lo->isucc;

       for ( e = n->F_BODY->exp; e != NULL; e = e->esucc ) {
	 /* IS e A REFERENCE TO THE CONTROL ROD */
	 if ( e->eport != c->iport )
	   continue;

	 switch ( e->dst->type ) {
	   case IFEqual:
	     if ( e->iport != 1 )
	       ImportSwap( e->dst );

	     break;

	   case IFLess:
	     /* CONTROL < HI */

	     /* IS THE CONTROL ROD THE FIRST IMPORT */
	     if ( e->iport != 1 )
	       goto MoveOn1;

	     /* IS THE SECOND IMPORT A REFERENCE TO HI */
	     if ( !AreValuesEqual( e->dst->imp->isucc, hi ) )
	       break;

	     e->dst->type = IFNotEqual;
	     nlog++;
	     break;

MoveOn1:
	     /* LO < CONTROL */

	     /* IS THE FIRST IMPORT A REFERENCE TO LO */
	     if ( !AreValuesEqual( e->dst->imp, lo ) )
	       break;

	     e->dst->type = IFNotEqual;
	     ImportSwap( e->dst );
	     nlog++;
	     break;

	   case IFLessEqual:
	     if ( e->dst->exp->esucc != NULL )
	       break;

	     not = e->dst->exp->dst;

	     if ( !IsNot( not ) )
	       break;

	     /* NOT( CONTROL <= LO ) FOR CONTROL > LO */

	     /* IS THE CONTROL ROD THE FIRST IMPORT */
	     if ( e->iport != 1 )
	       goto MoveOn2;

	     /* IS THE SECOND IMPORT A REFERENCE TO LO */
	     if ( !AreValuesEqual( e->isucc, lo ) )
	       break;

	     e->dst->type = IFNotEqual;

	     LinkExportLists( e->dst, not );

	     UnlinkExport( not->imp );
	     UnlinkNode( not );
	     /* free( not->imp ); */
	     /* free( not ); */
	     nlog++;
	     break;

MoveOn2:
	     /* NOT( HI <= CONTROL ) FOR HI > CONTORL */

	     /* IS THE SECOND IMPORT A REFERENCE TO HI */
	     if ( !AreValuesEqual( e->dst->imp, hi ) )
	       break;

	     e->dst->type = IFNotEqual;

	     ImportSwap( e->dst );

	     LinkExportLists( e->dst, not );

	     UnlinkExport( not->imp );
	     UnlinkNode( not );
	     /* free( not->imp ); */
	     /* free( not ); */
	     nlog++;
	     break;

	   default:
	     break;
	   }
	 }

       }
}


/**************************************************************************/
/* LOCAL  **************      IsRecursive          ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF THERE IS A PATH FROM from TO to WITHOUT GOING */
/*          THROUGH A UNION.  UNIONS MAY BE ON RECURSIVE PATHS! THIS      */
/*          ROUTINE SHOULD BE IN THE FRONTEND, BUT IS NOT.                */
/**************************************************************************/

static int IsRecursive( from, to )
PINFO from;
PINFO to;
{
  register PINFO r;
  register PINFO ato;

  if ( from == to ) 
    return( TRUE );

  if ( from->touchflag )
    return( FALSE );

  from->touchflag = TRUE;

  /* to IS NULL ON INITIAL ENTRY TO IsRecursive TESTS */
  ato = (to == NULL)? from : to;

  switch ( from->type ) {
    case IF_RECORD:
      for ( r = from->R_FIRST; r != NULL; r = r->L_NEXT )
	if ( IsRecursive( r->L_SUB, ato ) )
	  return( TRUE );

      break;

    case IF_ARRAY:
    case IF_STREAM:
      return( IsRecursive( from->A_ELEM, ato ) );

    default:
      break;
    }

  return( FALSE );
}


/* HAVE is_error ALWAYS RETURN FALSE, ASSUMING error(..) GENERATES AN */
/* ERROR MESSAGE AND SHUTS DOWN EXECUTION.                            */
static void BindIsError( n )
PNODE n;
{
  register PEDGE e;
  register PEDGE se;

  for ( e = n->exp; e != NULL; e = se ) {
    se = e->esucc;

    UnlinkExport( e );
    e->src = NULL;
    e->CoNsT = "FALSE";
    e->eport = CONST_PORT;
    }

  n->exp = NULL;
  OptRemoveDeadNode( n );
}


static PNODE FindFunctionGraph( f, nm )
register PNODE  f;
char           *nm;
{
  register PNODE g;

  for ( g = glstop->gsucc; g != NULL; g = g->gsucc ) {
    if ( g == f ) continue;

    if ( strcmp( g->G_NAME, nm ) == 0 )
      return( g );
    }

  return( NULL );
}


static void MarkReferencedFunctions( f, g )
PNODE f;
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE ff;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	MarkReferencedFunctions( f, sg );

    if ( IsCall( n ) ) {
      if ( (ff = FindFunctionGraph( f, n->imp->CoNsT )) == NULL )
	continue;

      ff->print = TRUE;
      }
    }
}


void EliminateDeadFunctions()
{
  register PNODE f;
  register PNODE sf;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( f->mark == 'e' || f->mark == 's' || 
	 f->mark == 'c' || f->mark == 'f' ||
         f->mark == 'd' )  /* ENTRY POINTS? */
      f->print = TRUE;

    MarkReferencedFunctions( f, f );
    }

  for ( f = glstop->gsucc; f != NULL; f = sf ) {
    sf = f->gsucc;

    if ( !(f->print) ) {
      UnlinkGraph( f );
      rmf++;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    ConvertFillToForall    ************************/
/**************************************************************************/
/* PURPOSE:  CONVERT AFill NODE n INTO FORALL NODE n.                     */
/**************************************************************************/

static void ConvertFillToForall( n )
PNODE n;
{
  register PNODE  g;
  register PNODE  b;
  register PNODE  r;
  register PNODE  gat;
  register PNODE  range;
  register PEDGE  e;
  register PALIST l;

  g = NodeAlloc( 0, IFSGraph );
  b = NodeAlloc( 0, IFSGraph );
  r = NodeAlloc( 0, IFSGraph );

  CopyVitals( n, g );
  CopyVitals( n, b );
  CopyVitals( n, r );

  LinkGraph( n, g );
  LinkGraph( g, b );
  LinkGraph( b, r );

  g->G_DAD = n;
  b->G_DAD = n;
  r->G_DAD = n;

  n->C_SCNT = 3;

  l = AssocListAlloc( 0 );
  l = LinkAssocLists( l, AssocListAlloc( 1 ) ) ;
  l = LinkAssocLists( l, AssocListAlloc( 2 ) ) ;

  n->C_ALST = l;

  n->type = IFForall;

  gat = NodeAlloc( 1, IFAGather );
  CopyVitals( n, gat );
  LinkNode( r, gat );

  e = EdgeAlloc( r, 1, gat, 1 ); /* LOWER BOUND */
  e->info = n->imp->info;
  LinkImport( gat, e );
  LinkExport( r, e );

  e = EdgeAlloc( r, 3, gat, 2 );        /* VALUE */
  e->info = n->imp->isucc->isucc->info; /* NOTE: MULTIPLE TYPE NOT ADDED! */
  LinkImport( gat, e );
  LinkExport( r, e );

  e = EdgeAlloc( gat, 1, r, 1 ); /* RESULT ARRAY */
  e->info = n->exp->info;
  LinkExport( gat, e );
  LinkImport( r, e );

  range = NodeAlloc( 1, IFRangeGenerate );
  CopyVitals( n, range );
  LinkNode( g, range );

  e = EdgeAlloc( g, 1, range, 1 ); /* LOWER BOUND */
  e->info = n->imp->info;
  LinkImport( range, e );
  LinkExport( g, e );

  e = EdgeAlloc( g, 2, range, 2 ); /* UPPER BOUND */
  e->info = n->imp->isucc->info;
  LinkImport( range, e );
  LinkExport( g, e );

  e = EdgeAlloc( range, 1, g, 4 ); /* MULTIPLE VALUE ON PORT 4!!! */
  e->info = n->imp->info;          /* NOTE: MULTIPLE TYPE NOT ADDED! */
  LinkExport( range, e );
  LinkImport( g, e );

  fill++;
}


/**************************************************************************/
/* LOCAL  **************  FixBooleanAndErrorConsts ************************/
/**************************************************************************/
/* PURPOSE:  CONVERT THE BOOLEAN CONSTANT IMPORTS TO NODE n TO UPPER      */
/*           CASE, AND CONVERT ERROR CONSTANTS INTO EDGES WITH IFError    */
/*           SOURCES (CHANGED BACK BY IF1Write!!!).                       */
/**************************************************************************/

static void FixBooleanAndErrorConsts( n )
PNODE n;
{
    register PEDGE i;
    register PNODE nn;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( !IsConst( i ) )
	  continue;

	if ( i->CoNsT == NULL ) { /* ERROR CONSTANT */
	  nn = NodeAlloc( ++maxint, IFError );
	  CopyVitals( n, nn );
	  i->eport = 1;
	  LinkExport( nn, i );
	  LinkNode( (IsGraph(n))? n : n->npred, nn );
	  eccnt++;
	  continue;
	  }

	if ( IsBoolean( i->info ) ) {
	  /* CANN: BUG FIX 2/1/90 is neg(y) compilation */
	  /* if ( i->CoNsT == NULL ) i->CoNsT = "FALSE"; */

	  if ( (i->CoNsT[0] == 'T') || (i->CoNsT[0] == 't') )
	    i->CoNsT = "TRUE";
          else
	    i->CoNsT = "FALSE";
          }
	}
}


/**************************************************************************/
/* LOCAL  **************       RemoveIntNode       ************************/
/**************************************************************************/
/* PURPOSE:  If native IS TRUE, ALLOW THE S PORT VALUE OF Select NODE s   */
/*           TO BE A BOOLEAN; THUS, REMOVE THE Int NODE ADDED BY LLNL     */
/*           SOFTWARE TO GURANTEE AN INTEGER SELECTOR.                    */
/**************************************************************************/

static void RemoveIntNode( s )
PNODE s;
{
    register PEDGE i;
    register PEDGE ii;

    if ( !native )
	return;

    if ( IsConst( s->S_TEST->imp ) )
	return;

    i = FindImport( s, s->S_TEST->imp->eport );

    if ( IsConst( i ) )
	return;

    if ( IsExport( s->S_ALT, i->iport ) || IsExport( s->S_CONS, i->iport ) )
	return;

    if ( UsageCount( i->src, i->eport ) > 1 )
	return;

    if ( !IsInt( i->src ) )
	return;

    ii = i->src->imp;
    ii->iport = i->iport;

    UnlinkImport( ii );
    UnlinkExport( i );
    UnlinkImport( i );
    UnlinkNode( i->src );
    LinkImport( s, ii );

    s->S_TEST->imp->info = ii->info; intc++;
}


/**************************************************************************/
/* LOCAL  ************** NormalizeGenerateSubgraph ************************/
/**************************************************************************/
/* PURPOSE: NORMALIZE THE GENERATE SUBGRAPH OF FORALL NODE f.  FIRST ALL  */
/*          NONGENERATE NODES ARE REMOVED. THEN THE ARRAY SCATTER NODES   */
/*          ARE CONVERTED INTO RangeGenerate NODES PLACING CORRESPONDING  */
/*          AElement NODES IN THE BODY. LASTLY, REMOVE THE HIGH RANGE ON  */ 
/*          NONCONTROL RangeGenerate NODES AND UNUSED INDEXES OUTPUT FROM */
/*          STREAM SCATTER NODES.                                         */
/**************************************************************************/

static void NormalizeGenerateSubgraph( f )
PNODE f;
{
    register PNODE n;
    register PNODE nd;
    register PEDGE e;
    register PEDGE se;
    register PEDGE a;
    register PEDGE v;
    register PNODE sn;

    NormalizeNodes( f->F_GEN  );                        /* WORK BOTTOM-UP */

    /* REMOVE NON-GENERATE NODES AND ELIMINATE RangeG AND Scat FANOUT     */

    for ( n = f->F_GEN->G_NODES; n != NULL; n = sn ) {
        sn = n->nsucc;

        switch ( n->type ) {
            case IFAScatter:
                if ( native ) {
                    EliminateFanout( n, f->F_BODY, f->F_RET, 1 );
                    EliminateFanout( n, f->F_BODY, f->F_RET, 2 );
                    }
    
                continue;

            case IFRangeGenerate:
                if ( native )
                    EliminateFanout( n, f->F_BODY, f->F_RET, 1 );

                continue;

            default:
                break;
            }

        RemoveNode( n, f->F_GEN );
        InsertNode( f, n );

        fgnm++;
        }

    if ( !native )
	return;

    /* CONVERT AScatter NODES (FOR ARRAYS) INTO RangeGenerate NODES       */

    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc ) {
	if ( !IsAScatter( n ) )
	    continue;

        if ( IsStream( n->imp->info ) )
	    continue;

	srcnt++;
	n->type = IFRangeGenerate;

	a = FindImport( f, n->imp->eport );

	/* INSERT AND WIRE THE ALimL NODE                                 */

	nd = NodeAlloc( ++maxint, IFALimL );
	CopyVitals( n, nd );
	LinkNode( f->npred, nd );

	e = EdgeAlloc( a->src, a->eport, nd, 1 ); /* e1 */
	e->info = a->info;
	LinkImport( nd, e );
	LinkExport( a->src, e );

	e = EdgeAlloc( nd, 1, f, ++maxint ); /* e2 */
	e->info = integer;
	LinkExport( nd, e );
	LinkImport( f,  e );

	n->imp->eport = maxint; /* e3 */
	n->imp->info  = integer;

	/* INSERT AND CORRESPONDINGLY WIRE THE ALimH NODE                 */

	nd = NodeAlloc( ++maxint, IFALimH );
	CopyVitals( n, nd );
	LinkNode( f->npred, nd );

	e = EdgeAlloc( a->src, a->eport, nd, 1 ); /* e1 */
	e->info = a->info;
	LinkImport( nd, e );
	LinkExport( a->src, e );

	e = EdgeAlloc( nd, 1, f, ++maxint ); /* e2 */
	e->info = integer;
	LinkExport( nd, e );
	LinkImport( f,  e );

	e = EdgeAlloc( f->F_GEN, maxint, n, 2 ); /* e3 */
	e->info = integer;
	LinkExport( f->F_GEN, e );
	LinkImport( n, e );

	/* REMOVE SCATTERED VALUE EDGE FROM NEW RangeGenerate NODE AND    */
	/* CHANGE EXPORT PORT NUMBER OF REMAINING EDGE TO ONE             */

	UnlinkExport( v = FindExport( n, 1 ) );
	UnlinkImport( v );

	n->exp->eport = 1;

	/* INSERT AElement IN BODY IF SCATTERED VALUE REFERENCE IN BODY   */
	/* OR RETURN SUBGRAPH                                             */
	   
        if ( IsExport( f->F_RET, v->iport ) || 
	     IsExport( f->F_BODY, v->iport ) ) {
            nd = NodeAlloc( ++maxint, IFAElement );
	    CopyVitals( n, nd );
	    LinkNode( f->F_BODY, nd );

	    e = EdgeAlloc( f->F_BODY, a->iport, nd, 1 ); /* A */
	    e->info = a->info;
	    LinkExport( f->F_BODY, e );
	    LinkImport( nd, e );

	    e = EdgeAlloc( f->F_BODY, n->exp->iport, nd, 2 ); /* INDEX */
	    e->info = integer;
	    LinkExport( f->F_BODY, e );
	    LinkImport( nd, e );

	    /* LINK v REFERENCES IN BODY SUBGRAPH TO THE AElement NODE    */

            for ( e = f->F_BODY->exp; e != NULL; e = se ) {
		se = e->esucc;

		if ( e->eport != v->iport )
		    continue;

                UnlinkExport( e );
		e->eport = 1;
		LinkExport( nd, e );
		}

	    /* WIRE AElement OUTPUT TO RETURN SUBGRAPH REFERENCES         */

	    if ( IsExport( f->F_RET, v->iport ) ) {
		e = EdgeAlloc( nd, 1, f->F_BODY, ++maxint );

		e->info = v->info->A_ELEM;       /* IT IS A MULTIPLE TYPE */

		LinkExport( nd, e );
		LinkImport( f->F_BODY, e );

		ChangeExportPorts( f->F_RET, v->iport, maxint );
		}

	    }

	/* free( v ); */
	}

    /* REMOVE THE SECOND INPUT FROM ALL NON-CONTROL RangeGenerate NODES   */
    /* AND THE INDEX OUTPUT FROM ALL AScatter NODES IF THEY ARE NOT USED  */

    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
	    case IFRangeGenerate:
		if ( n->exp != n->exp->dst->imp ) {
		    UnlinkExport( n->imp->isucc );
		    UnlinkImport( n->imp->isucc ); fges++;
		    }
 
		break;

	    default:
		e = FindExport( n, 2 );

		if ( IsExport( f->F_BODY, e->iport ) ||
		     IsExport( f->F_RET,  e->iport )  )
                    break;

                UnlinkExport( e );
		UnlinkImport( e ); fges++;
                
		break;
	    }
	}
}


/**************************************************************************/
/* LOCAL  **************      EliminateFanout      ************************/
/**************************************************************************/
/* PURPOSE: COMBINE ALL EXPORTS OF NODE n WITH EXPORT PORT NUMBER eport   */
/*          TO ELIMINATE UNNECESSARY FANOUT. FOR THE MERGE TO BE LEGAL,   */
/*          n MUST BE A Scatter, RangeGenerate, OR MULTIPLE NODE.         */
/*          REFERENCES TO ELIMINATED EXPORTS ARE ADJUSTED ACCORDINGLY,    */
/*          UPDATING THE EXPORT PORT NUMBERS OF NODES n1 AND n2. IF n IS  */
/*          A MULTIPLE NODE THAN n1 MUST BE THE LOOP NODE AND n2 MUST BE  */
/*          NULL.  IF n IS A Scatter OR RangeGenerate NODE, THEN n1 AND   */
/*          n2 MUST BE THE Forall'S BODY AND RETURN GRAPH NODES.          */
/**************************************************************************/

static void EliminateFanout( n, n1, n2, eport )
PNODE n;
PNODE n1;
PNODE n2;
int   eport;
{
    register PEDGE e;
    register PEDGE se;
    register int   iport;

    for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->eport == eport )
	    break;

    if ( e == NULL )
	return;

    iport = e->iport;

    for ( e = e->esucc; e != NULL; e = se ) {
	se = e->esucc;

	if ( e->eport != eport )
	    continue;

        ChangeExportPorts( n1, e->iport, iport );

	if ( n2 != NULL )
	    ChangeExportPorts( n2, e->iport, iport );

	UnlinkImport( e );
	UnlinkExport( e );
	/* free( e ); */
	fan++;
	}
}


/**************************************************************************/
/* LOCAL  **************      RemoveSLimLNode      ************************/
/**************************************************************************/
/* PURPOSE: REMOVE STREAM IMPORTING ALimL NODE n. THE LOWER BOUND OF ALL  */
/*          STREAMS IS 1.  NODE n IS DESTROYED.                           */
/**************************************************************************/

static void RemoveSLimLNode( n )
PNODE n;
{
    register PEDGE e;
    register PEDGE se;

    for ( e = n->exp; e != NULL; e = se ) {
	se = e->esucc;

	UnlinkExport( e );

	e->CoNsT = "1";
	e->src   = NULL;
	e->esucc = NULL;
	e->epred = NULL;
	e->eport = CONST_PORT;
	}

    OptRemoveDeadNode( n );
    sliml++;
}


/**************************************************************************/
/* LOCAL  **************        FoldNegNode        ************************/
/**************************************************************************/
/* PURPOSE: FOLD CONSTANT NEGATION NODE n, CONVERTING ALL EXPORTS INTO    */
/*          THE NEGATED CONSTANT.  ERROR CONSTANTS ARE IGNORED.           */
/**************************************************************************/

static void FoldNegNode( n )
PNODE n;
{
    register PEDGE  i;
    register PEDGE  e;
    register PEDGE  se;
    register char  *r;
	     char   buf[100];

    if ( DeBuG ) return;

    i = n->imp;

    if ( IsInteger( i->info ) )
	SPRINTF( buf, "%d", -atoi( i->CoNsT ) );
    else
	SPRINTF( buf, "%.16e", -atof( DoubleToReal( i->CoNsT ) ) );

    r = CopyString( buf );

    /* ASSIGN THE NEGATED CONSTANT TO ALL EXPORTS OF NODE n AND DISCARD n */

    for ( e = n->exp; e != NULL; e = se ) {
	se = e->esucc;

	e->CoNsT = r;
	e->eport = CONST_PORT;
	e->esucc = NULL;
	e->epred = NULL;
	e->src   = NULL;
	}

    n->exp = NULL;
    OptRemoveDeadNode( n ); foldcnt++;

    return;
}


/**************************************************************************/
/* LOCAL  **************     RemoveSetLowNode      ************************/
/**************************************************************************/
/* PURPOSE: AS A SIDE EFFECT OF LLNL SOFTWARE AN ARRAY BUILT IN A FORALL  */
/*          LOOP BY A GATHER NODE IS ASSIGNED ITS CORRECT LOWER BOUND     */
/*          OUTSIDE THE LOOP. THIS SHOULD BE DONE BY THE GATHER NODE.     */
/*          THIS ROUTINE UNDOES THIS INEFFICIENCY. THIS IS NOT A GENERAL  */
/*          PURPOSE ROUTINE FOR REMOVING ALL UNNECESSARY ASetL NODES.     */
/*          HOWEVER, THE ROUTINE WAS MODIFIED TO ALSO REMOVE UNNECESSARY  */
/*          ASetL NODES WHOSE IMPORTS RESULT FROM GATHERING IN SEQUENTIAL */
/*          LOOP FORMS.                                                   */
/**************************************************************************/

static void RemoveSetLowNode( s )
PNODE s;
{
    register PEDGE i;
    register PEDGE e;
    register PEDGE a = s->imp;
    register PEDGE l = s->imp->isucc;
    register PNODE n;
    register PEDGE lb;

    if ( DeBuG ) return;

    /* IS THE ASetL NODE A CANDIDATE FOR REMOVAL? */

    if ( !IsArray( a->info ) || IsConst( a ) )           /* STREAM OR STRING */
	return;

    if ( UsageCount( a->src, a->eport ) > 1 )
	return;

    /* CONDITION ONE: x := array[b:...]; y := array_setl(x,lo); */
    if ( a->src->type == IFABuild ) {
      lb = a->src->imp;
      goto DoRemoval;
      }

    if ( !( IsForall( a->src ) || IsLoop( a->src ) ) )
	return;

    if ( IsForall( a->src ) )
        i = FindImport( a->src->F_RET, a->eport );
    else
        i = FindImport( a->src->L_RET, a->eport );

    switch ( i->src->type ) {
      case IFAGather:
	lb = i->src->imp;
	break;

      case IFReduce:
      case IFRedTree:
      case IFRedRight:
      case IFRedLeft:
        if ( i->src->imp->CoNsT[0] != REDUCE_CATENATE )
	  return;

	lb = i->src->imp->isucc;
	break;

      default:
	return;
      }

DoRemoval:

    /* NODE MOVEMENT NOT REQUIRED SINCE THE LOWER BOUND IS A CONSTANT */

    if ( IsConst( l ) ) {
	if ( IsConst( lb ) )
	    lb->CoNsT = l->CoNsT;
        else
	    ChangeEdgeToConst( lb, l );

        for ( e = s->exp; e != NULL; e = e->esucc )
	    e->eport = a->eport;

        LinkExportLists( a->src, s );
	OptRemoveDeadNode( s ); scnt++;

	return;
	}

    /* MAKE SURE THAT DATA FLOW ORDERING IS PRESERVED */

    for ( n = a->src; n != s; n = n->nsucc )
        if ( n == l->src )
	    return;

/* THREAD THE LOWER BOUND INTO THE FORALL NODE AND TO THE GATHER NODE. */
    
    UnlinkImport( l );
    l->iport = ++maxint;
    LinkImport( a->src, l );

    for ( e = s->exp; e != NULL; e = e->esucc )
	e->eport = a->eport;

    LinkExportLists( a->src, s );

    if ( IsConst( lb ) ) {
	lb->CoNsT = NULL;
	lb->eport = maxint;

	LinkExport( i->dst, lb );
	}
    else
	lb->eport = maxint;

    OptRemoveDeadNode( s );  scnt++;

    return;
}


/**************************************************************************/
/* LOCAL  **************   ConvertStringImports    ************************/
/**************************************************************************/
/* PURPOSE: CONVERT EACH STRING IMPORT OF NODE n TO ITS EQUIVALENT ABuild */
/*          NODE.  THE LOWER BOUND OF THE STRING IS ASSUMED TO BE ONE. A  */
/*          NULL STRING REPRESENTS AN ERROR STRING AND IS IGNORED.        */
/**************************************************************************/

static void ConvertStringImports( n )
PNODE n;
{
    register char  *s;
    register PEDGE  i;
    register PEDGE  c;
    register PNODE  bld;
    register int    idx;
    register int    ip;
             char   buf[100];

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( !(IsConst( i ) && IsArray( i->info )) )  /* STRING CONSTANT? */
	    continue;

        if ( (s = i->CoNsT) == NULL )          /* ERROR STRING CONSTANT?  */
	    continue;

        /* ALLOCATE ABuild NODE AND LINK TO THE SCOPE'S NODE LIST         */

        bld = NodeAlloc( ++maxint, IFABuild ); conv++;
	CopyVitals( n, bld );

        if ( IsGraph( n ) )
	    LinkNode( n, bld );
        else
	    LinkNode( n->npred, bld );

        /* ATTACH THE LOWER BOUND TO THE ABuild NODE                      */

        c = EdgeAlloc( NULL_NODE, CONST_PORT, bld, 1 );
        c->info  = integer;
	c->CoNsT = "1";

        LinkImport( bld, c );

        /* ATTACH CHARACTERS OF THE STRING TO THE ABuild NODE             */

        for ( idx = 0, ip = 2; *s != '\0'; idx = 0, ip++ ) {
	    if ( (buf[idx++] = *s++) == '\\' ) {
                if ( IsOctal( *s ) )
		    while ( IsOctal( *s ) ) 
		        buf[idx++] = *s++;
                else if ( *s == '\0' )
		    Error1( "StringToChars: ILLEGAL BACKSLASH SEQUENCE" );
	        else
                    buf[idx++] = *s++;
	        }

            buf[idx] = '\0';

	    c = EdgeAlloc( NULL_NODE, CONST_PORT, bld, ip );
	    c->info  = i->info->A_ELEM;
	    c->CoNsT = CopyString( buf );

	    LinkImport( bld, c );
	    }

        /* LINK i TO bld'S EXPORT LIST                                    */

        i->src   = bld;
        i->CoNsT = NULL;
        i->eport = 1;

        LinkExport( bld, i );
        }
}


/**************************************************************************/
/* LOCAL  **************     RemoveNotNodePair     ************************/
/**************************************************************************/
/* PURPOSE: IF NODE n IS A NOT NODE AND REDUNDANT REMOVE IT AND ITS TWIN. */
/*          FOR n TO BE REDUNDANT, ITS IMPORTS SOURCE MUST BE ANOTHER NOT */
/*          NODE WHOSE EXPORT IS ONLY USED BY n.                          */
/**************************************************************************/

static void RemoveNotNodePair( n )
PNODE n;
{
    register PEDGE e;
    register PEDGE se;
    register PNODE src;

    if ( DeBuG ) return;

    if ( IsNot( n ) ) {
	if ( IsConst( n->imp ) )
	    return;

	src = n->imp->src;

	if ( (src->exp->esucc == NULL) && (IsNot( src )) ) {
	    for ( e = n->exp; e != NULL; e = se ) {
		se = e->esucc;

		if ( IsConst( src->imp ) )
		    ChangeEdgeToConst( e, src->imp );
                else
		    e->eport = src->imp->eport;
                }

            if ( !IsConst( n->imp->src->imp ) )
		LinkExportLists( src->imp->src, n );

	    OptRemoveDeadNode( n ); redn++;

	    return;
	    }
        }

    return;
}


/**************************************************************************/
/* LOCAL  **************   ConvertReduceCatenate   ************************/
/**************************************************************************/
/* PURPOSE: CONVERT ReduceCatneate NODE n TO VERSION 1.2 FROM 1.0, MAKING */
/*          THE SECOND IMPORT THE LOWER BOUND OF THE UNIT VALUE: 1.  THE  */
/*          ORIGINAL UNIT VALUE ABuild NODE IS REMOVED.  THIS ROUTINE     */
/*          ASSUMES THE UNIT VALUE ABuild NODE IS OUTSIDE THE LOOP. NOTE: */
/*          IF1 VERSION 1.2 IS IN-HOUSE AND HAS NOT BEEN APPROVED!  ITS   */
/*          USE ELIMINATES PROBABLE COPYING (UNIT VALUES SHOULD NOT BE    */
/*          SHARED).                                                      */
/**************************************************************************/

static void ConvertReduceCatenate( n )
PNODE n;
{
    register PNODE c;
    register PNODE g;
    register PEDGE u;
    register int   p;

    redc++;

    u = n->imp->isucc;
    p = u->eport;
    c = u->src->G_DAD;

    UnlinkExport( u );
    u->eport = CONST_PORT;
    u->CoNsT = "1";
    u->src   = NULL;
    u->info  = integer;

    /* LOCATE AND REMOVE THE ABuild UNIT VALUE                            */

    /* IS IT DEFINED IN THE BODY? (NormalizeForallRet,etc.) */
    if ( !IsImport( c, p ) )
      return;

    for ( ;; ) {
	/* IS THE UNIT VALUE REFERENCED WITHIN COMPOUND NODE c?           */

	for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
            if ( IsExport( g, p ) )
		return;

	u = FindImport( c, p );

	UnlinkExport( u );
	UnlinkImport( u );

	/* CONTINUE ONLY IF u->src IS A SUBGRAPH                          */

	if ( !IsSGraph( u->src ) ) {
	    OptRemoveDeadNode( u->src );
	    return;
	    }

	c = u->src->G_DAD;
	p = u->eport;
	}
}


static void NormalizeLoopRet( l )
PNODE l;
{
  register PNODE n;
  register PNODE sn;
  register PNODE nn;

  for ( n = l->L_RET->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    switch ( n->type ) {
    case IFReduce:
    case IFRedLeft:
    case IFRedTree:
    case IFRedRight:
      switch ( n->imp->CoNsT[0] ) {
      case REDUCE_CATENATE:
      case REDUCE_GREATEST:
      case REDUCE_LEAST:
      case REDUCE_PRODUCT:
      case REDUCE_SUM:
      case REDUCE_USER:
	break;

      default:
	fprintf(stderr,"Warning: Bad reduction \"%s\" on IF1 line %d\n",
		n->imp->CoNsT,n->imp->if1line);
      }
      /* Fall through... */

    case IFAGather:
    case IFRestValues:
    case IFFirstValue:
    case IFFinalValue:
    case IFUReduce:
      continue;

    default:
      ASSERT(!IsReturn(n), "Missing return type");
      RemoveNode( n, l->L_RET );

      CopyExports( n, nn = CopyNode( n ) );
      CopyImports( n, nn, FALSE );

      InsertNode( l->L_INIT, n );
      InsertNode( l->L_BODY, nn );

      break;
    }
  }
}

static void NormalizeForallRet( f )
PNODE f;
{
  register PNODE n;
  register PNODE sn;

  for ( n = f->F_RET->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    switch ( n->type ) {
    case IFReduce:
    case IFRedLeft:
    case IFRedTree:
    case IFRedRight:
      switch ( n->imp->CoNsT[0] ) {
      case REDUCE_CATENATE:
      case REDUCE_GREATEST:
      case REDUCE_LEAST:
      case REDUCE_PRODUCT:
      case REDUCE_SUM:
      case REDUCE_USER:
	break;

      default:
	fprintf(stderr,"Warning: Bad reduction \"%s\" on IF1 line %d\n",
		n->imp->CoNsT,n->imp->if1line);
      }
      /* Fall through... */
    case IFAGather:
    case IFRestValues:
    case IFFirstValue:
    case IFFinalValue:
    case IFUReduce:
      continue;

    default:
      ASSERT(!IsReturn(n), "Missing return type");
      RemoveNode( n, f->F_RET );
      InsertNode( f->F_BODY, n );
      break;
    }
  }
}


/**************************************************************************/
/* LOCAL  **************      IsRetNormalized      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE RETURN NODE OF LOOP l IS NORMALIZED:       */
/*          CONTAINS ONLY RETURN NODES AND DOESN'T CONTAIN STRING         */
/*          CONSTANTS.  CONSTRUCTS NOT SUPPORTED BY THE NATIVE COMPILER   */
/*          ARE REPORTED, CAUSING COMPILATION TO STOP, IF native IS TRUE. */
/*          FURTHER, native CAUSES CONVERTION OF 1.0 ReduceCatenate NODES */
/*          TO VERSION 1.2 WHERE THE SECOND IMPORT IS THE LOWER BOUND OF  */
/*          THE UNIT (ASSUMED TO BE 1).                                   */
/**************************************************************************/

static int IsRetNormalized( l )
PNODE l;
{
    register PNODE n;
    register PEDGE i;

    if ( IsForall( l ) )
	n = l->F_RET->G_NODES;
    else
	n = l->L_RET->G_NODES;

    for ( /* EMPTY */; n != NULL; n = n->nsucc ) {
	if ( !IsReturn( n ) )
	    return( FALSE );

	if ( native ) {
	    EliminateFanout( n, l, NULL_NODE, 1 );

	    switch ( n->type ) {
		case IFFirstValue:
		    Error1( "IFFirstValue NOT IMPLEMENTED" );
		    break;

		case IFRestValues:
		    Error1( "returns old NOT IMPLEMENTED" );
		    break;

		case IFReduce:
		case IFRedLeft:
		case IFRedRight:
		case IFRedTree:
		    if ( n->imp->CoNsT[0] == REDUCE_CATENATE )
			ConvertReduceCatenate( n );

		    break;

	        case IFFinalValue:
		    break;

		default:
		    break;
		}
            }

        for ( i = n->imp; i != NULL; i = i->isucc )
	    if ( IsConst( i ) && IsArray( i->info ) )
		return( FALSE );
        }

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     MarkLegalTestNodes    ************************/
/**************************************************************************/
/* PURPOSE: STARTING WITH NODE n, RECURSIVELY MARK ALL BOOLEAN OPERATION  */
/*          NODES THAT DIRECTLY CONTRIBUTE TO THE CONSTRUCTION OF A TEST  */
/*          SUBGRAPH'S B PORT IMPORT. A NODE IS CONSIDERED MARKED IF ITS  */
/*          label FIELD IS NEGATIVE.                                      */
/**************************************************************************/

static void MarkLegalTestNodes( n )
PNODE n;
{
    register PEDGE i;

    if ( n == NULL )
	return;

    switch ( n->type ) {
	case IFNot:
	case IFLess:
	case IFLessEqual:
	case IFEqual:
	case IFNotEqual:
            break;

	case IFSGraph:
            return;

	case IFPlus:
	case IFTimes:
	    if ( IsBoolean( n->exp->info ) )
		break;

        default:
	    return;
	}

    n->label = -(n->label);

    for ( i = n->imp; i != NULL; i = i->isucc )
	MarkLegalTestNodes( i->src );
}


/**************************************************************************/
/* LOCAL  **************     NormalizeLoopTest     ************************/
/**************************************************************************/
/* PURPOSE: NORMALIZE THE TEST SUBGRAPH OF LOOP l.  IF l IS EITHER A      */
/*          LOOPA OR LOOPB NODE, ALL NON-BOOLEAN OPERATION NODES THAT DO  */
/*          NOT DIRECTLY CONTRIBUTE TO THE CONSTRUCTION OF THE LOOP's B   */
/*          PORT VALUE ARE MOVED TO THE BODY SUBGRAPH.  IF l IS A LOOPB   */
/*          NODE, A COPY OF EACH REMOVED NODE IS ALSO MOVED TO THE        */
/*          INITIAL SUBGRAPH.                                             */
/**************************************************************************/

static void NormalizeLoopTest( l )
PNODE l;
{
    register PNODE n;
    register PNODE nn;
    register PNODE sn;

    MarkLegalTestNodes( l->L_TEST->imp->src );

    for ( n = l->L_TEST->G_NODES; n != NULL; n = sn ) {
        sn = n->nsucc;

        if ( n->label > 0 ) {
	    if ( l->type == IFLoopA ) {
                RemoveNode( n, l->L_TEST );
                InsertNode( l->L_BODY, n );

                latnm++;
	    } else {
		RemoveNode( n, l->L_TEST );

		CopyExports( n, nn = CopyNode( n ) );
		CopyImports( n, nn, FALSE );

		InsertNode( l->L_INIT, n );
		InsertNode( l->L_BODY, nn );

		lbtnm++;
		}
	    }
        else
            n->label = -(n->label);
	}
}


/**************************************************************************/
/* LOCAL  **************       NormalizeNodes      ************************/
/**************************************************************************/
/* PURPOSE: NORMALIZE ALL NODES IN GRAPH g'S NODE LIST.  THE FOLLOWING    */
/*          ACTION IS TAKEN FOR EACH INDIVIDUAL COMPOUND NODE:            */
/*                                                                        */
/*          SELECT:  MAKE SURE THAT THE TEST SUBGRAPH IS ALREADY EMPTY.   */
/*                   THEN NORMALIZE THE NODES IN THE OTHER SUBGRAPHS.     */
/*                                                                        */
/*          TAGCASE: NORMALIZE THE NODES IN ALL THE SUBGRAPHS             */
/*                                                                        */
/*          FORALL:  MAKE SURE THAT THE RETURN SUBGRAPHS IS ALREADY       */
/*                   NORMALIZED.  THEN NORMALIZE THE NODES IN THE BODY    */
/*                   AND GENERATE SUBGRAPHS. FINALLY NORMALIZE THE BODY   */
/*                   AND GENERATE SUBGRAPHS.                              */
/*                                                                        */
/*                                                                        */
/*          LOOPA:                                                        */
/*          LOOPB:   MAKE SURE THAT THE RETURN SUBGRAPH IS ALREADY        */
/*                   NORMALIZED, THEN NORMALIZE THE TEST SUBGRAPH. NEXT   */
/*                   NORMALIZED ALL THE NODES IN THE INITIAL AND BODY     */
/*                   SUBGRAPHS.  FINALLY, NORMALIZE THE INITIAL SUBGRAPH, */
/*                   REMOVING ALL NODES.                                  */
/*                                                                        */
/*          FURTHER, CONSTANT NEGATION NODES ARE FOLDED, REDUNDANT NOT    */
/*          NODE PAIRS ARE REMOVED, STRING CONSTANTS ARE CONVERTED INTO   */
/*          ABuild NODES, AND UNNECESSARY ARRAY SET LOW NODES ARE REMOVED */
/*          (GIVEN asetl IS TRUE). IF native IS TRUE, CONSTURCTS NOT      */
/*          SUPPORTED BY THE NATIVE COMPILER ARE REPORTED---CAUSING       */
/*          COMPILATION TO STOP.                                          */
/**************************************************************************/

static void NormalizeNodes( g )
PNODE g;
{
    register PNODE n;
    register PNODE nd;
    register PNODE sn;
    register PNODE nn;
    register PEDGE e;

    ConvertStringImports( g );
    FixBooleanAndErrorConsts( g );

    for ( n = g->G_NODES; n != NULL; n = nn ) {
	nn = n->nsucc;

	ConvertStringImports( n );
        FixBooleanAndErrorConsts( n );

	if ( IsCall( n ) )
	  n->imp->CoNsT = LowerCase( n->imp->CoNsT, FALSE, FALSE );

	if ( n->exp == NULL ) {
	  OptRemoveDeadNode( n );
	  continue;
	  }

	ConvertStringImports( n );
        FixBooleanAndErrorConsts( n );

        switch ( n->type ) {
	    case IFExp:
	      /* ELIMINATE FRONTEND INTRODUCED INEFFICIENCY  */
	      if ( !IsInteger( n->imp->info ) )
		break;

	      if ( !IsInteger( n->imp->isucc->info ) )
		break;

	      if ( IsInteger( n->exp->info ) )
		break;

	      if ( n->exp->esucc != NULL )
		break;

	      if ( !IsInt( n->exp->dst ) )
		break;

	      e = n->exp;
	      UnlinkExport( e );
	      LinkExportLists( n, e->dst );
	      UnlinkNode( e->dst );
	      /* free( e->dst ); */
	      /* free( e ); */

	      nn = n->nsucc;
	      break;

	    case IFLoopA:
	    case IFLoopB:
		NormalizeLoopRet( n );

                if ( !IsRetNormalized( n ) )
		    Error1( "LOOP RETURN SUBGRAPHS NOT NORMALIZED" );

	        NormalizeLoopTest( n );

		for ( nd = n->L_TEST->G_NODES; nd != NULL; nd = sn ) {
		    sn = nd->nsucc;

		    if ( IsNot( nd ) )
			RemoveNotNodePair( nd );    /* MAY REMOVE NODE nd */
                    }

	        NormalizeNodes( n->L_INIT );
	        NormalizeNodes( n->L_BODY );

	        for ( nd = n->L_INIT->G_NODES; nd != NULL; nd = sn ) {
		    sn = nd->nsucc;

		    RemoveNode( nd, n->L_INIT );
		    InsertNode( n, nd );

		    linm++;
		    }

	        break;

	    case IFForall:
StartForall:
		NormalizeForallRet( n );

                if ( !IsRetNormalized( n ) )
		    Error1( "FORALL RETURN SUBGRAPHS NOT NORMALIZED" );

                NormalizeGenerateSubgraph( n );
                NormalizeNodes( n->F_BODY );

	        break;

	    case IFSelect:
	        if ( !IsNodeListEmpty( n->S_TEST ) )
		    Error1( "SELECT TEST SUBGRAPHS NOT NORMALIZED" );

		RemoveIntNode( n );

	    case IFTagCase:
	        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		    NormalizeNodes( g );

	        break;

            case IFNot:
	        RemoveNotNodePair( n );
		break;

	    case IFASetL:
		if ( !asetl )
		    break;

		RemoveSetLowNode( n );
                break;

	    case IFNeg:
		if ( !sgnok )
		    break;

		if ( IsConst( n->imp ) )
		    FoldNegNode( n );

                break;
		
	    case IFALimL:
		if ( !IsStream( n->imp->info ) )
		    break;

		RemoveSLimLNode( n );
		break;

	    case IFIsError:
		if ( native )
		  BindIsError( n );

                break;

	    case IFNoOp:
		if ( native )
		    Error1( "IFNoOp NOT IMPLEMENTED" );

                break;

	    case IFBindArguments:
		if ( native )
		    Error1( "IFBindArguments NOT IMPLEMENTED" );

                break;

	    case IFAFill:
		if ( native ) {
		  if ( IsStream( n->exp->info ) )
		    Error1( "STREAM IFAFill NOT IMPLEMENTED" );

		  ConvertFillToForall( n );
		  goto StartForall;
                  }

                break;

	    case IFACatenate:
		if ( native )
                    if ( n->imp->isucc->isucc != NULL )
		        Error1( "IFACatenate: > 2 IMPORTS NOT IMPLEMENTED" );

                break;

	    case IFALimH:
		if ( native )
		    if ( IsStream( n->imp->info ) )
		        Error1( "STREAM IFALimH NOT IMPLEMENTED" );

                break;

	    case IFASize:
                break;

	    case IFAIsEmpty:
		if ( native )
		    if ( IsArray( n->imp->info ) )
		        Error1( "ARRAY IFAIsEmpty NOT IMPLEMENTED" );

                break;

	    default:
	        break;
            }
        }
}


static void ConvertStreamsToArrays( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE e;
  register PNODE nn;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	ConvertStreamsToArrays( sg );

    switch ( n->type ) {
      case IFPrefixSize:
      case IFASize:
	if ( IsStream( n->imp->info ) )
	  Error1( "STREAM_SIZE AND STREAM_PREFIXSIZE NOT IMPLEMENTED" );

	break;

      case IFAIsEmpty:         /* STREAM_EMPTY      */
	n->type = IFASize;

	nn = NodeAlloc( ++maxint, IFLess );
	CopyVitals( n, nn );
	LinkNode( n, nn );
	LinkExportLists( nn, n );

        e = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
        e->info  = integer;
	e->CoNsT = "1";
	LinkImport( nn, e );

	e = EdgeAlloc( n, 1, nn, 1 );
	e->info = integer;
	LinkExport( n, e );
	LinkImport( nn, e );
	break;

      case IFARemL:
	if ( !IsStream( n->imp->info ) )
	  break;

	/* STREAM_REST */
	nn = NodeAlloc( ++maxint, IFASetL );
	CopyVitals( n, nn );
	LinkNode( n, nn );
	LinkExportLists( nn, n );

        e = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
        e->info  = integer;
	e->CoNsT = "1";
	LinkImport( nn, e );

	e = EdgeAlloc( n, 1, nn, 1 );
	e->info = n->imp->info;
	LinkExport( n, e );
	LinkImport( nn, e );
	break;
	
      default:
	break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************     WriteNormalizeInfo    ************************/
/**************************************************************************/
/* PURPOSE: WRITE INFORMATION GATHERED DURING NORMALIZATION TO stderr.    */
/**************************************************************************/

static void WriteNormalizeInfo()
{
    FPRINTF( infoptr, "\n Initial Subgraph Nodes Moved:      %d\n", linm   );
    FPRINTF( infoptr,   " LoopA Test Subgraph Nodes Moved:   %d\n", latnm  );
    FPRINTF( infoptr,   " LoopB Test Subgraph Nodes Moved:   %d\n", lbtnm  );
    FPRINTF( infoptr,   " Forall Gen Subgraph Nodes Moved:   %d\n", fgnm   );
    FPRINTF( infoptr,   " Removed Not Node Pairs:            %d\n", redn   );
    FPRINTF( infoptr,   " Removed Set Low Nodes:             %d\n", scnt   );
    FPRINTF( infoptr,   " Converted String Constants:        %d\n", conv   );
    FPRINTF( infoptr,   " Folded Neg Nodes:                  %d\n", foldcnt );
    FPRINTF( infoptr,   " Converted Reduce Catenate Nodes:   %d\n", redc   );
    FPRINTF( infoptr,   " Removed Stream Limit Low Nodes:    %d\n", sliml  );
    FPRINTF( infoptr,   " Eliminated Fanout Edges:           %d\n", fan    );
    FPRINTF( infoptr,   " Eliminated Forall Generate Edges:  %d\n", fges   );
    FPRINTF( infoptr,   " Converted AScatter Nodes:          %d\n", srcnt  );
    FPRINTF( infoptr,   " Removed Int Nodes:                 %d\n", intc   );
    FPRINTF( infoptr,   " Removed Dead Functions:            %d\n", rmf    );
    FPRINTF( infoptr,   " Converted AFill Nodes:             %d\n", fill   );
    FPRINTF( infoptr,   " Converted Error Constants:         %d\n", eccnt  );
    FPRINTF( infoptr,   " RangeGenerate Nodes Removed:       %d\n", rgcnt  );
    FPRINTF( infoptr,   " Normalized Logicals:               %d\n", nlog   );
}


/**************************************************************************/
/* GLOBAL **************       If1Normalize        ************************/
/**************************************************************************/
/* PURPOSE: NORMALIZE ALL FUNCTION GRAPHS. UNNECESSARY SET LOW NODES ARE  */
/*          REMOVED IF asetl IS TRUE. IF native IS TRUE, NODES NOT        */
/*          IMPLEMENTED BY THE C CODE GENERATOR ARE REPORTED AND          */
/*          ReduceCatenate NODES ARE CHANGED SO THE UNIT VALUE DEFINES    */
/*          THE LOWER BOUND OF THE RESULTING ARRAY. ASetL NODES WITH      */
/*          STREAM IMPORTS ARE REPLACED BY THE CONSTANT 1.                */
/**************************************************************************/

void If1Normalize()
{
  register PNODE f;
  register PINFO i;

  /* FOR NATIVE COMPILATION, REMOVE ALL STREAM DATA TYPES */
  if ( native ) {
    /* CHECK FOR RECURSIVE RECORDS, STREAMS, AND ARRAYS: SHOULD HAVE BEEN */
    /* DONE BY THE FRONTEND */
    for ( i = ihead; i != NULL; i = i->next )
      i->touchflag = FALSE;

    for ( i = ihead; i != NULL; i = i->next )
      switch ( i->type ) {
	case IF_STREAM:
	  if ( IsRecursive( i, NULL_INFO ) )
	    Error1( "ILLEGAL RECURSIVE STREAM ENCOUNTERED" );

          break;

	case IF_ARRAY:
	  if ( IsRecursive( i, NULL_INFO ) )
	    Error1( "ILLEGAL RECURSIVE ARRAY ENCOUNTERED" );

          break;

	case IF_RECORD:
	  if ( IsRecursive( i, NULL_INFO ) )
	    Error1( "ILLEGAL RECURSIVE RECORD ENCOUNTERED" );

	  break;

        default:
	  break;
        }

    for ( i = ihead; i != NULL; i = i->next )
      i->touchflag = FALSE;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
      ConvertStreamsToArrays( cfunct = f );

    for ( i = ihead; i != NULL; i = i->next )
        if ( IsStream(i) ) {
	    i->type = IF_ARRAY;

	    /* RECORD THAT THIS CONVERTED TYPE WAS ONCE A STREAM!!! */
	    /* THIS IS USED TO GET PROPER FIBRE PROCESSING OF INPUT */
	    /* AND OUTPUT STREAMS.                                  */
	    i->mark = 'S';
	    }
    }


  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
      f->G_NAME = LowerCase( f->G_NAME, FALSE, FALSE );

      NormalizeNodes( cfunct = f );
      SimplifyGenerates( f );
      }

  EliminateDeadFunctions();

/*  if ( RequestInfo(I_Info1,info) )
    WriteNormalizeInfo(); */
}
