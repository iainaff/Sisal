/* if1gcse.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:34  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define MAX_SCOPE 100                          /* MAXIMUM SCOPE NESTING   */

static int ncnt  = 0;                          /* COUNT OF COMBINED NODES */
static int sacnt = 0;                       /* SELECT ANTI MOVEMENT COUNT */

static int top  = -1;                          /* SCOPE STACK TOP POINTER */

typedef struct scope SCOPE; 

struct scope {
    PNODE subg;                                /* SUBGRAPH DEFINING SCOPE */
    PNODE cmp;                 /* COMPOUND NODE OF INTEREST IN SCOPE subg */
    };

static SCOPE scopes[MAX_SCOPE];                            /* SCOPE STACK */


/**************************************************************************/
/* STATIC **************  CopyExportsForThreading  ************************/
/**************************************************************************/
/* PURPOSE: RETURN A COPY NODE n1'S EXPORT LIST MAKING THE FOLLOWING      */
/*          PORT NUMBER MODIFICATIONS TO EACH LIST (ASSUMING e1 IS AN     */
/*          EXPORT OF NODE n1 AND e2 IS e1'S COPY):                       */
/*                                                                        */
/*          1. ASSIGN e1'S EXPORT PORT TO THE EXPORT PORT OF e2 (DONE BY  */
/*             CopyImports).                                              */
/*                                                                        */
/*          2. ASSIGN A NEW (UNIQUE) PORT NUMBER TO e1'S EXPORT PORT AND  */
/*             ASSIGN THE SAME NUMBER TO e2'S IMPORT PORT.                */
/**************************************************************************/

static PEDGE CopyExportsForThreading( n1 )
PNODE n1;
{
    register PEDGE e1;
    register PEDGE e2;
    register int   eport;
             NODE  n2;

    n2.exp = NULL;

    CopyExports( n1, &n2 );

    for ( e1 = n1->exp; e1 != NULL; e1 = e1->esucc ) {
        if ( e1->eport < 0 )
            continue;

        eport = e1->eport;
        ++maxint;

        for ( e2 = e1; e2 != NULL; e2 = e2->esucc )
            if ( e2->eport == eport )
                e2->eport = -maxint;

        for ( e2 = n2.exp; e2 != NULL; e2 = e2->esucc )
            if ( e2->eport == eport )
                e2->iport = maxint;
        }

    for ( e1 = n1->exp; e1 != NULL; e1 = e1->esucc )
        if ( e1->eport <= 0 )
            e1->eport = -(e1->eport);

    return( n2.exp );
}


/**************************************************************************/
/* LOCAL  **************       CombineGNodes       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODE n1 IF A SIMILAR NODE IS DEFINED BELOW THE SCOPE   */
/*          DEFINING n1: g1. IF SUCH A NODE IS FOUND, n1 IS REMOVED FROM  */
/*          THE NODE LIST OF g1 AND THE APPROPRAITE REFERENCE EDGES ARE   */
/*          THREADED ACROSS ALL SUBGRAPH BOUNDARIES BETWEEN THE FOUND     */
/*          FOUND NODE AND THE NODES IMPORTING n1'S EXPORTS.  ALL         */
/*          REDUNDANT K PORT IMPORTS INTRODUCED BY THE THREADING PROCESS  */
/*          ARE REMOVED.                                                  */
/**************************************************************************/

static void CombineGNodes( g1, n1 )
PNODE g1;
PNODE n1;
{
    register PNODE n2;
    register PEDGE e;
    register int   t, f, l;
    register PEDGE nexp;

    for ( f = top; f >= 0; f-- ) {
        l = TRUE;

        for ( n2 = scopes[f].subg->G_NODES; n2 != NULL; n2 = n2->nsucc ) {
            if ( n2 == scopes[f].cmp )
                l = FALSE;

            /* SHORT CIRCUIT FOR FASTER EXECUTION */
            if ( n1->type != n2->type )
              continue;

            if ( !AreNodesEqual(n1,n2) )
                continue;

            goto DoThreading;
            }
        }

    return;

DoThreading:

    if ( !l ) {                            /* PRESERVE THE DATA FLOW ORDERING */
        UnlinkNode( n2 );
        LinkNode( scopes[f].cmp->npred, n2 );
        }

    RemoveNode( n1, g1 );

    for ( t = top; t >= f; t-- ) {
        if ( t != f )
            nexp = CopyExportsForThreading( n1 );

        for ( e = n1->exp; e != NULL; e = e->esucc )
            LinkImport( scopes[t].cmp, e );

        if ( t != f )
            LinkExportLists( scopes[t].subg, n1 );
        else
            LinkExportLists( n2, n1 );

        n1->exp = nexp;
        }

    ncnt++; dscnt++;

    for ( t = f; t <= top; t++ )
      CombineKports( scopes[t].cmp );
}


/**************************************************************************/
/* LOCAL  **************     IsGCSECandidate       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF n IS A CANDIDATE FOR GCSE.  NODE n MUST NOT    */
/*          BE A DEREFERENCE NODE THAT IMPORTS AN ARRAY OR RECORD THAT    */
/*          IS IMPORTED TO A REPLACE NODE.  SUCH MOVEMENT MIGHT HINDER    */
/*          SUCCESS OF UPDATE-IN-PLACE.                                   */
/**************************************************************************/

static int IsGCSECandidate( n )
PNODE n;
{
    register PEDGE e;

    if ( IsAElement( n ) ) {
        if ( IsBasic( n->exp->info ) )
            return( TRUE );

        for ( e = n->imp->src->exp; e != NULL; e = e->esucc )
            if ( e->eport == n->imp->eport )
                switch ( e->dst->type ) {
                    case IFAReplace:
                    case IFRReplace:
                        return( FALSE );

                    default:
                        break;
                    }
        }

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************        RemoveGCses        ************************/
/**************************************************************************/
/* PURPOSE: COMBINE SIMPLE NODES DEFINED IN THE NODE LIST OF GRAPH g WITH */
/*          IDENTICAL NODES FOUND OUTSIDE g. ONLY INVARIANT NODES ARE     */
/*          CONSIDERED.                                                   */
/**************************************************************************/

static void RemoveGCses( g )
PNODE g;
{
    register PNODE n;
    register PNODE sn;
    register PNODE sg;

    if ( IsIGraph( g ) )
        return;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
        sn = n->nsucc;

        if ( IsCompound( n ) ) {
            if ( (++top) >= MAX_SCOPE )          /* PUSH SCOPE ON SCOPE STACK */
                Error1( "RemoveGCses: SCOPE STACK OVERFLOW" );

            scopes[top].subg = g; scopes[top].cmp  = n;

            for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
                RemoveGCses( sg );

            top--;                               /* POP SCOPE OFF SCOPE STACK */

            sn = n->nsucc;
            continue;
            }

        if ( IsSGraph( g ) )
          if ( IsSelect( g->G_DAD ) )
            if ( OptIsInvariant( n ) )
              if ( IsGCSECandidate( n ) )
                CombineGNodes( g, n );
        }
}


/**************************************************************************/
/* GLOBAL **************        WriteGCseInfo      ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING GCSE TO stderr.             */
/**************************************************************************/

void WriteGCseInfo()
{
    FPRINTF( infoptr, "\n **** GLOBAL COMMON NODE ELIMINATION\n\n" );
    FPRINTF( infoptr, " Select Subgraph Movements:  %d\n", ccnt   );
    FPRINTF( infoptr, " Combined Nodes:             %d\n", ncnt   );
    FPRINTF( infoptr, " Select Clean Operations:    %d\n", sccnt  );
    FPRINTF( infoptr, " Select ICSE Movements:      %d\n", sacnt  );
}


/**************************************************************************/
/* LOCAL  **************        DoAntiGCse         ************************/
/**************************************************************************/
/* PURPOSE: MOVE COMMON SUBEXPRESSIONS BETWEEN SELECT SUBGRAPHS, BUT WORK */
/*          FROM THE SUBGRAPH IMPORT LIST AND MOVE CSEs TOWARD THEIR      */
/*          DECENDENTS. THIS OPTIMIZATION WILL IMPROVE VECTOR PERFORMANCE */
/*          ON VECTOR MACHINES.  I DOUBT IT WILL IMPROVE PERFORMANCE ON   */
/*          NON-VECTOR MACHINES.                                          */
/**************************************************************************/

static void DoAntiGCse( g )
PNODE g;
{
  register PNODE sg;
  register PNODE n;
  register PNODE sn;
  register PNODE sg1;
  register PNODE sg2;
  register PEDGE e;
  register PEDGE ee;
  register PEDGE se;
  register PEDGE i1;
  register PEDGE i2;
  register int   c;
  register int   p1;
  register int   p2;
  register PNODE n1;
  register PNODE n2;
  register PEDGE see;
  register PEDGE ii1;
  register PEDGE ii2;

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	DoAntiGCse( sg );

    if ( !IsSelect( n ) )
      continue;

    sg1 = n->S_CONS;
    sg2 = n->S_ALT;

    if ( agcse ) {
      for ( e = n->exp; e != NULL; e = se ) {
	se = e->esucc;

        if ( (i1 = FindImport( sg1, e->eport )) == NULL )
	  Error2( "DoAntiGCse", "FindImport FAILED FOR agcse i1" );
        if ( (i2 = FindImport( sg2, e->eport )) == NULL )
	  Error2( "DoAntiGCse", "FindImport FAILED FOR agcse i2" );

	n1 = i1->src;
	n2 = i2->src;

        if ( IsConst(i1) || IsConst(i2) )
	  continue;
	if ( IsSGraph(n1) || IsSGraph(n2) )
	  continue;
        if ( i1->eport != 1 || i2->eport != 1 )
	  continue;

	/* ARE i1->src AND i2->src CANDIDATES? */
	if ( !IsBasic( i1->info ) )
	  continue;
	if ( !IsSimple( n1 ) )
	  continue;
	/* MUST BE MONADIC OR DYADIC */
	if ( n1->imp == NULL )
	  continue;
	if ( n1->imp->isucc != NULL )
	  if ( n1->imp->isucc->isucc != NULL )
	    continue;
	if ( n1->exp->esucc != NULL )
	  continue;

	if ( i2->esucc != NULL )
	  continue;
	if ( !IsBasic( i2->info ) )
	  continue;
	/* MUST BE MONADIC OR DYADIC */
	if ( n2->imp == NULL )
	  continue;
	if ( n2->imp->isucc != NULL )
	  if ( n2->imp->isucc->isucc != NULL )
	    continue;
	if ( n2->exp->esucc != NULL )
	  continue;

	if ( n1->type != n2->type )
	  continue;

	/* CALL??? */
	c = FALSE;
	if ( IsCall(n1) ) {
	  if ( strcmp( n1->imp->CoNsT, n2->imp->CoNsT ) != 0 )
	    continue;
	  c = TRUE;
	  }

        /* DO THE ANTI-MOVEMENT !!!! */
	sacnt++;
	p1 = -1;
	p2 = -1;

	ii1 = NULL;
	ii2 = NULL;

	/* PREPARE FOR e REMOVAL! */
	se = e->epred;

	UnlinkNode( n1 );
	UnlinkNode( n2 );
	UnlinkImport( i1 );
	UnlinkImport( i2 );

	/* PROCESS FIRST ARGUMENT TO n1 AND n2 */
	if ( c ) {
	  ii1 = n1->imp;
	  UnlinkImport( ii1 );
	  UnlinkImport( n2->imp );
        } else {
	  UnlinkImport( ii1 = n1->imp );
	  p1 = ++maxint;
	  ii1->iport = p1;
	  LinkImport( sg1, ii1 );

	  UnlinkImport( ee = n2->imp );
	  ee->iport = p1;
	  LinkImport( sg2, ee );
	  }

	/* PROCESS SECOND ARGUMENT TO n1 AND n2? */
        if ( n1->imp != NULL ) {
	  UnlinkImport( ii2 = n1->imp );
	  p2 = ++maxint;
	  ii2->iport = p2;
	  LinkImport( sg1, ii2 );

	  UnlinkImport( ee = n2->imp );
	  ee->iport = p2;
	  LinkImport( sg2, ee );
	  }

	/* MOVE n1 OUT, FORGET ABOUT n2 */
	UnlinkExport( n1->exp );
	LinkNode( n, n1 );

	/* LINK UP THE IMPORTS TO n1 */
	if ( c )
	  LinkImport( n, ii1 );
        else {
	  ee = EdgeAlloc( n, p1, n1, 1 );
	  ee->info = ii1->info;
	  LinkExportToEnd( n, ee );
	  LinkImport( n1, ee );
	  }

	if ( ii2 != NULL ) {
	  ee = EdgeAlloc( n, p2, n1, 2 );
	  ee->info = ii2->info;
	  LinkExportToEnd( n, ee );
	  LinkImport( n1, ee );
	  }

        /* MOVE e REFERENCES TO n1's EXPORT LIST */
	p1 = e->eport;

        for ( /* NOTHING */; e != NULL; e = see ) {
	  see = e->esucc;

	  if ( e->eport != p1 )
	    continue;

	  UnlinkExport( e );
	  e->eport = 1;     /* EXPORT NUMBER VERIFIED ABOVE!!! */
          LinkExport( n1, e );
          }

	/* ADJUST se SO WE CAN CONTINUE */
	if ( se == NULL )
	  se = n->exp;
        else
	  se = se->esucc;
        }
      }


    /* CLEAN SELECT NODE */
    for ( e = n->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( (i1 = FindImport( sg1, e->eport )) == NULL )
	Error2( "DoAntiGCse", "FindImport FAILED FOR i1" );
      if ( (i2 = FindImport( sg2, e->eport )) == NULL )
	Error2( "DoAntiGCse", "FindImport FAILED FOR i2" );

      /* CASE OF TWO IDENTICAL CONSTANT EXPORTS IS COVERED IN if1fold.c */
      if ( IsConst(i1) || IsConst(i2) )
	continue;

      if ( IsSGraph(i1->src) && IsSGraph(i2->src) ) {
        if ( i1->eport == i2->eport ) {
	  UnlinkExport( e );

	  if ( (ee = FindImport( n, i1->eport )) == NULL )
	    Error2( "DoAntiGCse", "FindImport FAILED FOR ee" );

	  if ( IsConst( ee ) ) {
	    e->eport = CONST_PORT;
	    e->CoNsT = ee->CoNsT;
	  } else {
	    e->eport = ee->eport;
	    LinkExport( ee->src, e );
	    }

	  sccnt++;
	  }

	continue;
	}
      }
    }
}


/**************************************************************************/
/* GLOBAL **************        OptRemoveSCses        ************************/
/**************************************************************************/
/* PURPOSE: MOVE COMMON SUBEXPRESSIONS BETWEEN SELECT SUBGRAPHS TO        */
/*          IMPROVE CHANCES FOR INVARIANT REMOVAL AND CSE.                */
/**************************************************************************/

void OptRemoveSCses( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE aa;
  register PNODE cc;
  register PNODE sa;
  register PEDGE e;
  register PNODE sn;
  register PEDGE ee;
  register PEDGE se;

  if ( DeBuG ) return;

  /* DEPTH FIRST */
  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    /* TRY AND EXPOSE LOOP INVARIANTS IN ASSOCIATIVE AND COMMUTATIVE CHAINS */
    /* TO HELP BOOST INVARIANT REMOVAL                                      */
    switch ( n->type ) {
      case IFPlus:
      case IFTimes:
	if ( IsBoolean( n->imp->info ) )
	  break;

	OptNormalizeNode( n );
	ExposeInvariants( n, n );
	break;

      default:
	break;
      }

    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
        OptRemoveSCses( sg );
	}
      }
    }

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( !IsSelect( n ) ) continue;

    for ( aa = n->S_ALT->G_NODES; aa != NULL; aa = sa ) {
      sa = aa->nsucc;

      for ( cc = n->S_CONS->G_NODES; cc != NULL; cc = cc->nsucc ) {
	/* A SHORT CIRCUIT FOR FASTER EXECUTION */
	if ( aa->type != cc->type )
	  continue;

        if ( !OptIsInvariant( aa ) )
	  continue;
	if ( !OptIsInvariant( cc ) )
	  continue;
        if ( !AreNodesEqual(aa,cc) )
          continue;

	/* BE CAREFUL NOT TO HURT UPDATE-IN-PLACE ANALYSIS */
        if ( IsAElement( aa ) ) {
          for ( e = aa->imp->src->exp; e != NULL; e = e->esucc )
            if ( e->dst->type == IFAReplace )
              goto MoveOn;

          for ( e = cc->imp->src->exp; e != NULL; e = e->esucc )
            if ( e->dst->type == IFAReplace )
              goto MoveOn;
	  }

        RemoveNode( cc, n->S_CONS );
        InsertNode( n, cc );

        /* RemoveNode( aa, n->S_ALT ); */
        /* InsertNode( n, aa ); */
	for ( e = cc->exp; e != NULL; e = e->esucc )
	  for ( ee = aa->exp; ee != NULL; ee = se ) {
	    se = ee->esucc;

	    if ( ee->eport != e->eport )
	      continue;

	    UnlinkExport( ee );
	    ee->eport = e->iport;
	    LinkExport( n->S_ALT, ee );
	    }

	OptRemoveDeadNode( aa );
        ccnt++;

	/* SUCCESS!!! SO BREAK OUT AND MOVE ON */
	break;
        }

      MoveOn: continue;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************          If1GCse          ************************/
/**************************************************************************/
/* PURPOSE: CROSS ALL SUBGRAPH BOUNDARIES IN AN ATTEMPT TO ELIMINATE      */
/*          COMMON SUBEXPRESSIONS IN ALL FUNCTION GRAPHS.                 */
/**************************************************************************/

void If1GCse()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    top = -1;

    RemoveGCses( f );
    DoAntiGCse( f );
    }
}
