/* if2aimp.c,v
 * Revision 12.7  1992/11/04  22:05:00  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:59  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int rkcnt     = 0;         /* COUNT OF REMOVED K IMPORTS           */
static int nrkcnt    = 0;         /* COUNT OF UNREMOVED K IMPORTS         */
static int gabc      = 0;         /* COUNT OF INSERTED GetArrayBase Nodes */
static int arcnt     = 0;         /* COUNT OF OPTIMIZED AReplace NODES    */
static int sopt      = 0;         /* COUNT OF OPTIMIZED Select TESTS      */
static int scnt      = 0;         /* COUNT OF Select NODES                */
static int cgabp     = 0;         /* COUNT OF GAB PATH CSE                */
static int mgab      = 0;         /* COUNT OF MIGRATED GAB NODES          */
static int aicnt     = 0;    /* COUNT OF ARRAY INDEXING OPTIMIZATIONS     */
static int aifcnt    = 0;    /* COUNT OF FAILED ARRAY INDEX OPTIMIZATIONS */
static int nvl       = 0;             /* COUNT OF NORMALIZED VECTOR LOOPS */
static int spicnt    = 0;    /* COUNT OF SPECIAL GetArrayBase INVARIANTS  */ 
static int rmov      = 0;                /* COUNT OF ARRAY READ MOVEMENTS */
static int chains    = 0;                    /* COUNT OF CRAY X-MP CHAINS */
static int sgabs     = 0;  /* COUNT OF SPECIAL AReplace GABase INSERTIONS */


#define MAX_SCOPE 100                          /* MAXIMUM SCOPE NESTING   */

static int top   = -1;                         /* SCOPE STACK TOP POINTER */

typedef struct scope SCOPE; 

struct scope {
    PNODE subg;                                /* SUBGRAPH DEFINING SCOPE */
    PNODE cmp;                 /* COMPOUND NODE OF INTEREST IN SCOPE subg */
    };

static SCOPE scopes[MAX_SCOPE];                            /* SCOPE STACK */

void NormalizeVectorLoop( f )
PNODE f;
{
  register PEDGE e;
  register PEDGE ee;
  register PNODE nn;
  register PEDGE c;
  register int   v;
  register PNODE aelm;
  register PEDGE lo;
  register PEDGE hi;
  register int   max;
  register int   eport;
	   char  buf[100];

  max = 0;

  c = f->F_GEN->imp;

  for ( e = f->F_BODY->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != c->iport )
      continue;

    switch ( e->dst->type ) {
      case IFOptAElement:
	break;

      case IFAIndexPlus:
      case IFAIndexMinus:
	if ( e->iport != 1 )
	  return;

	if ( !IsConst( e->dst->imp->isucc ) )
	  return;

	v = atoi( e->dst->imp->isucc->CoNsT );

	if ( v < 1 ) 
	  return;

	if ( e->dst->type == IFAIndexMinus )
	  if ( v > max )
	    max = v;

	break;

      default:
	return;
      }
    }

  if ( max <= 0 )
    return;

  /* ADJUST LOOP CONTROL */

  lo = c->src->imp;
  hi = lo->isucc;


  if ( IsConst( lo ) ) {
    v = atoi(lo->CoNsT) - max;
    SPRINTF( buf, "%d", v );
    lo->CoNsT = CopyString( buf );
  } else {
    nn = NodeAlloc( ++maxint, IFMinus );
    LinkNode( f->npred, nn );

    eport = ++maxint;

    ee = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
    ee->info = lo->info;
    SPRINTF( buf, "%d", max );
    ee->CoNsT = CopyString( buf );
    LinkImport( nn,  ee );
    CopyEdgeAndLink( FindImport( f, lo->eport ), nn, 1 );

    ee = EdgeAlloc( nn, 1, f, eport );
    ee->info = lo->info;
    LinkImport( f,  ee );
    LinkExport( nn, ee );

    lo->eport = eport;
    }

  if ( IsConst(hi) ) {
    v = atoi(hi->CoNsT) - max;
    SPRINTF( buf, "%d", v );
    hi->CoNsT = CopyString( buf );
  } else {
    nn = NodeAlloc( ++maxint, IFMinus );
    LinkNode( f->npred, nn );

    eport = ++maxint;

    ee = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
    ee->info = hi->info;
    SPRINTF( buf, "%d", max );
    ee->CoNsT = CopyString( buf );
    LinkImport( nn,  ee );
    CopyEdgeAndLink( FindImport( f, hi->eport ), nn, 1 );

    ee = EdgeAlloc( nn, 1, f, eport );
    ee->info = hi->info;
    LinkImport( f,  ee );
    LinkExport( nn, ee );

    hi->eport = eport;
    }

  nvl++;

  /* ADJUST REFERENCES IN BODY */
  for ( e = f->F_BODY->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != c->iport )
      continue;

    switch ( e->dst->type ) {
      case IFAIndexPlus:
        v = atoi(e->dst->imp->isucc->CoNsT) + max;
        SPRINTF( buf, "%d", v );
        e->dst->imp->isucc->CoNsT = CopyString( buf );
	break;

      case IFAIndexMinus:
        v = (-(atoi(e->dst->imp->isucc->CoNsT))) + max;

	if ( v < 0 ) 
	  Error2( "NormalizeVectorLoops", "ADJUSTMENT ERROR" );

        SPRINTF( buf, "%d", v );
        e->dst->imp->isucc->CoNsT = CopyString( buf );
	e->dst->type = IFAIndexPlus;
	break;

      case IFOptAElement:
	aelm = e->dst;

        nn = NodeAlloc( ++maxint, IFAIndexPlus );
        LinkNode( aelm->npred, nn );

        UnlinkImport( e );
        e->iport = 1;
        LinkImport( nn, e );

        ee = EdgeAlloc( nn, 1, aelm, 2 );
        ee->info = e->info;
        LinkImport( aelm,  ee );
	LinkExport( nn, ee );

        ee = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
        ee->info = e->info;
        SPRINTF( buf, "%d", max );
        ee->CoNsT = CopyString( buf );
	LinkImport( nn, ee );
	break;

      default:
	Error2( "NormalizeVectorLoops", "ILLEGAL NORMALIZATION DESTINATION" );
      }
    }
}


/**************************************************************************/
/* LOCAL  **************          GetGABType       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE CORRECT GetArrayBase EXPORT TYPE FOR ARRAY i.      */
/**************************************************************************/

static PINFO GetGABType( i )
PINFO i;
{
  switch( i->A_ELEM->type ) {
    case IF_REAL:
      return( ptr_real );

    case IF_DOUBLE:
      return( ptr_double );

    case IF_INTEGER:
      return( ptr_integer );

    default:
      ;
    }

  return( ptr );
}


/**************************************************************************/
/* LOCAL  **************        DoTypeChange       ************************/
/**************************************************************************/
/* PURPOSE: CHANGE THE INFORMATION FIELD OF ALL REFERENCES TO n'S eport   */
/*          EXPORTS TO HAVE eport i.                                      */
/**************************************************************************/

static void DoTypeChange( n, eport, i )
PNODE n;
int   eport;
PINFO i;
{
  register PEDGE e;
  register PNODE sg;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    e->info = i;

    if ( IsCompound( e->dst ) )
      for ( sg = e->dst->C_SUBS; sg != NULL; sg = sg->gsucc )
	DoTypeChange( sg, e->iport, i );
    }
}


/**************************************************************************/
/* LOCAL  **************     FixGABExportTypes     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN POINTER TYPES TO ALL GetArrayBase REFERENCES           */
/**************************************************************************/

static void FixGABExportTypes( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PINFO i;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	FixGABExportTypes( sg );

    if ( n->type != IFGetArrayBase )
      continue;

    i = GetGABType( n->exp->info );
    DoTypeChange( n, n->exp->eport, i );
    }
}


static int IsInplace( i, e )
register PEDGE i;
register PEDGE e;
{
  register PNODE nop;

  if ( i == NULL ) 
    return( FALSE );

  if ( i->src == e->src ) {
    if ( i->eport == e->eport )
      return( TRUE );

    return( FALSE );
    }

  if ( !IsAReplace( i->src ) )
    return( FALSE );

  nop = i->src->imp->src;

  if ( !IsNoOp( nop ) )
    return( FALSE );

  if ( nop->imp->rmark1 != RMARK )
    return( FALSE );

  if ( !(nop->imp->omark1) )
    return( FALSE );

  return( IsInplace( nop->imp, e ) );
}


/**************************************************************************/
/* LOCAL  **************      OptSpecGABPaths      ************************/
/**************************************************************************/
/* PURPOSE: ALTHOUGH SOME ARRAYS ARE LOOP CARRIED, THEIR PHYSICAL STORAGE */
/*          NEVER CHANGES; HENCE THEIR GetArrayBase OPERATIONS ARE        */
/*          INVARIANT. THIS ROUTINE DETECTS AND OPTIMIZES THESE CASES.    */
/**************************************************************************/

static void OptSpecGABPaths( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE e;
  register PEDGE ii;
  register PEDGE se;
  register PEDGE ee;
  register PNODE gab;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	OptSpecGABPaths( sg );

    if ( !IsLoop( n ) ) 
      continue;

    for ( e = n->L_BODY->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( e->dst->type != IFGetArrayBase )
	continue;

      if ( !IsInplace( FindImport( n->L_BODY, e->eport ), e ) )
	continue;

      gab = e->dst;

      ii = FindImport( n->L_INIT, e->eport );
      if ( ii == NULL )
	continue;

      ii = FindImport( n, ii->eport );
      if ( ii == NULL )
	continue;

      maxint++;

      UnlinkNode( gab );

      for ( ee = gab->exp; ee != NULL; ee = ee->esucc )
	ee->eport = maxint;

      LinkExportLists( n->L_BODY, gab );

      UnlinkExport( e );
      e->eport = ii->eport;

      /* PRESERVE REFERENCE COUNT OPERATIONS */
      e->sr = ii->sr;
      e->pm = ii->pm;
      e->dmark = ii->dmark; /* CANN 10-3 */
      e->wmark = ii->wmark; /* CANN 10-3 */

      LinkExport( ii->src, e );
      LinkNode( n->npred, gab );

      e = EdgeAlloc( gab, 1, n, maxint );
      e->info = ii->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER!!! */
      LinkExport( gab, e );
      LinkImport( n, e );

      spicnt++;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************        OptGABPaths        ************************/
/**************************************************************************/
/* PURPOSE: MODIFY ARRAY REPLACE SEQUENCES SO THAT ALL THE NODES READ THE */
/*          OUTPUT OF THE SAME GetArrayBase NODE.  ALSO MIGRATE GetArrayB */
/*          NODES TOWARD THEIR FIRST USERS.                               */
/**************************************************************************/

static void OptGABPaths( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE nop;
  register int   l;
  register PEDGE gab1;
  register PEDGE gab2;
  register PEDGE e;
  register PEDGE ii;
  register PEDGE m;

  for ( n = FindLastNode( g ); n != g; n = n->npred ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	OptGABPaths( sg );

      continue;
      }

    if ( n->type != IFOptAReplace )
      continue;

    if ( (gab1 = FindImport( n, MAX_PORT_NUMBER )) == NULL ) 
      continue;

    if ( gab1->src->type != IFGetArrayBase ) 
      continue;

    nop = n->imp->src;
    if ( nop->type != IFNoOp ) continue;
    if ( nop->imp->rmark1 != RMARK ) continue; /* RO? */
    if ( !(nop->imp->omark1) ) continue;

    gab2 = NULL;
    nop  = NULL;
    for ( e = n->exp; e != NULL; e = e->esucc ) {
      if ( e->dst->type == IFGetArrayBase ) gab2 = e;
      if ( e->dst->type == IFNoOp ) nop = e->dst; 
      }

    if ( gab2 == NULL ) continue;
    if ( nop  == NULL ) continue;
    if ( nop->imp->rmark1 != RMARK ) continue; /* RO? */
    if ( !(nop->imp->omark1) ) continue;

    gab2 = gab2->dst->exp;

    if ( gab2 == NULL ) continue;

    UnlinkNode( gab2->src );
    UnlinkExport( gab2->src->imp );
    LinkExportLists( gab1->src, gab2->src );
    cgabp++;
    }

  /* MIGRATE GetArrayBase NODES Toward usages */

  for ( l = 1, n = g->G_NODES; n != NULL; n = n->nsucc, l++ )
    n->label = l;

  g->label = 0;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    switch ( n->type ) {
      case IFOptAReplace:
	if ( (ii = FindImport( n, MAX_PORT_NUMBER )) == NULL ) 
	  goto MoveOn;

	break;

      case IFOptAElement:
	ii = n->imp;
	break;

      default:
	goto MoveOn;
      }

    if ( ii->src->type != IFGetArrayBase )
      continue;

    m = ii->src->exp;
    for ( e = ii->src->exp; e != NULL; e = e->esucc ) {
      if ( e->dst->label == 0 ) continue;

      if ( m->dst->label > e->dst->label )
	m = e;
      }

    if ( m == NULL ) 
      continue;

    if ( m->dst->label == 0 ) 
      continue;

    if ( m->dst != n )
      continue;

    UnlinkNode( ii->src );
    LinkNode( n->npred, ii->src );
    mgab++;

MoveOn: 
    continue;
    }
}


/**************************************************************************/
/* LOCAL  **************        SelectTestOpt      ************************/
/**************************************************************************/
/* PURPOSE: ARTIFICALLY PULL THE ROOT NODE GENERATING Select NODE s'S     */
/*          BOOLEAN CONTROL INTO THE TEST SUBGRAPH TO ALLOW GENERATION OF */
/*          BETTER CODE FROM THE RESULTING C:                             */
/*                                                                        */
/*           tmp1 = tmp2 < tmp3; if ( tmp1 ) then... CONVERTED TO         */
/*           if ( tmp2 < tmp3 ) then....                                  */
/**************************************************************************/

static void SelectTestOpt( s )
PNODE s;
{
    register PEDGE i;
    register PEDGE e;

    e = s->S_TEST->imp; scnt++;

    if ( IsConst( e ) )
	return;

    if ( (i = FindImport( s, e->eport )) == NULL )
      return;

    if ( IsConst( i ) )
	return;

    if ( IsExport( s->S_ALT, i->iport ) || IsExport( s->S_CONS, i->iport ) )
	return;

    if ( i->src->exp->esucc != NULL )
	return;

    switch ( i->src->type ) {
	case IFPlus:
	case IFMinus:
	    if ( !IsBoolean( i->info ) )
		return;

	case IFLess: 
	case IFLessEqual:
	case IFEqual:
	case IFNotEqual:
	case IFNot:
	case IFGreat:
	case IFGreatEqual:
	    break;

	default:
	    return;
        }

    s->usucc = i->src; sopt++;
    UnlinkNode( i->src );
    LinkNode( s->npred, i->src );
    UnlinkImport( i );
    UnlinkExport( i ); i->src->exp = NULL; /* <--- MAKE SURE!! */
}

/**************************************************************************/
/* LOCAL  **************      GenFindSource        ************************/
/**************************************************************************/
/* PURPOSE: FIND THE COMPOUND NODE IMPORT ASSOCIATED WITH SUBGRAPH EXPORT */
/*          e. IF e IS ASSOCIATED WITH IMPORT ONE OF A TAGCASE NODE, THEN */
/*          NULL IS RETURNED.                                             */
/**************************************************************************/

static PEDGE GenFindSource( e )
PEDGE e;
{
    if ( IsTagCase( e->src->G_DAD ) && (e->eport == 1) )
        return( NULL );

    return( FindImport( e->src->G_DAD, e->eport ) );
}


/**************************************************************************/
/* LOCAL  **************     GenAreNodesEqual      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODES n1 AND n2 ARE EQUAL; n1 IS ASSUMED TO BE */
/*          A GetArrayBase NODE AND NESTED DEEPER THAN n2.                */
/**************************************************************************/

static int GenAreNodesEqual( n1, n2 )
PNODE n1;
PNODE n2;
{
    register PEDGE i1;
    register PEDGE i2;

    if ( n1->type != n2->type )
	return( FALSE );

    i1 = n1->imp;
    i2 = n2->imp; 
    
    for ( ;; ) {
	if ( i1->eport == i2->eport )
	    if ( i1->src == i2->src )
	        return( TRUE );

	if ( !IsSGraph( i1->src ) )
	    return( FALSE );

	if ( (i1 = GenFindSource( i1 )) == NULL )
	    return( FALSE );
        }
}


/**************************************************************************/
/* STATIC **************    GenAssignNewKports     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW K PORT NUMBERS, STARTING WITH 1, TO THE IMPORTS OF */
/*          COMPOUND NODE c AND ADJUST ALL REFERENCES. THE NEXT LEGAL     */
/*          PORT NUMBER IS RETURNED.                                      */
/**************************************************************************/

static int GenAssignNewKports( c )
PNODE c;
{
    register PNODE g;
    register PEDGE i;
    register int   p = 1;

    for ( i = c->imp; i != NULL; i = i->isucc, p++ ) {
	if ( (i->iport != p) && (i->iport != 0) ) {
            for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
                ChangeExportPorts( g, i->iport, -p );

            i->iport = -p;
	    }

	if ( i->iport == 0 )
	    p--;
        }

    return( p );
}


/**************************************************************************/
/* STATIC **************    GenAssignNewLports     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW L PORT NUMBERS, STARTING WITH p, TO THE IMPORTS    */
/*          OF LOOP l'S INITIAL SUBGRAPH AND ADJUST ALL REFERENCES. THE   */
/*          NEXT LEGAL PORT NUMBER IS RETURNED.                           */
/**************************************************************************/

static int GenAssignNewLports( p, l )
int   p;
PNODE l;
{
    register PEDGE i;

    for ( i = l->L_INIT->imp; i != NULL; i = i->isucc, p++ ) {
	if ( (i->iport != p) && (i->iport != 0) ) {
            ChangeExportPorts( l->L_TEST, i->iport, -p );
            ChangeExportPorts( l->L_BODY, i->iport, -p );
            ChangeExportPorts( l->L_RET,  i->iport, -p );
	    ChangeImportPorts( l->L_BODY, i->iport, -p );

            i->iport = -p;
	    }

	if ( i->iport == 0 )
	    p--;
        }

    return( p );
}


/**************************************************************************/
/* STATIC **************    GenAssignNewMports     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW M PORT NUMBERS, STARTING WITH p, TO THE IMPORTS OF */
/*          FORALL f'S GENERATE SUBGRAPH AND ADJUST ALL REFERENCES.  THE  */
/*          NEXT LEGAL PORT NUMBER IS RETURNED.                           */
/**************************************************************************/

static int GenAssignNewMports( p, f )
int   p;
PNODE f;
{
    register PEDGE i;

    for ( i = f->F_GEN->imp; i != NULL; i = i->isucc, p++ ) {
	if ( (i->iport != p) && (i->iport != 0) ) {
            ChangeExportPorts( f->F_BODY, i->iport, -p );
            ChangeExportPorts( f->F_RET,  i->iport, -p );

            i->iport = -p;
	    }

	if ( i->iport == 0 )
	    p--;
        }

    return( p );
}


/**************************************************************************/
/* STATIC **************    GenAssignNewTports     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW T PORT NUMBERS, STARTING WITH p, TO THE IMPORTS OF */
/*          FORALL f'S BODY SUBGRAPH AND ADJUST ALL REFERENCES. 	  */
/**************************************************************************/

static void GenAssignNewTports( p, f )
int   p;
PNODE f;
{
    register PEDGE i;

    for ( i = f->F_BODY->imp; i != NULL; i = i->isucc, p++ ) {
	if ( (i->iport != p) && (i->iport != 0) ) {
            ChangeExportPorts( f->F_RET, i->iport, -p );
            i->iport = -p;
	    }

	if ( i->iport == 0 )
	    p--;
        }
}


/**************************************************************************/
/* STATIC **************  GenAssignNewLoopTports   ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW T PORT NUMBERS, STARTING WITH p, TO THE IMPORTS OF */
/*          LOOP l'S BODY SUBGRAPH AND ADJUST ALL REFERENCES.		  */
/**************************************************************************/

static void GenAssignNewLoopTports( p, l )
int   p;
PNODE l;
{
    register PEDGE i;

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc )
        if ( !IsImport( l->L_INIT, i->iport ) ) {
	    if ( (i->iport != p) && (i->iport != 0) ) {
                ChangeExportPorts( l->L_TEST, i->iport, -p );
                i->iport = -p;
		}

	    if ( i->iport != 0 )
                p++;
            }
}


/**************************************************************************/
/* STATIC **************      GenAssignNewRports      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW R PORT NUMBERS, STARTING WITH 1, TO THE IMPORTS OF */
/*          SUBGRAPH g AND ADJUST ALL REFERENCES IN THE EXPORT LIST OF    */
/*          THE COMPOUND NODE TO WHICH g BELONGS.                         */
/**************************************************************************/

static void GenAssignNewRports( g )
PNODE g;
{
    register PEDGE i;
    register int   p = 1;

    for ( i = g->imp; i != NULL; i = i->isucc, p++ ) {
	if ( (i->iport != p) && (i->iport != 0) ) {
            ChangeExportPorts( g->G_DAD, i->iport, -p );
            i->iport = -p;
	    }

	if ( i->iport == 0 )
	    p--;
        }
}


/**************************************************************************/
/* GLOBAL **************      AssignNewPortNums    ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW PORT NUMBERS TO K, M, T, L, AND R PORTS OF ALL     */
/*          COMPOUND NODES SUCH THAT PORT NUMBERS OBEY THE FOLLOWING:     */
/*                                                                        */
/*          FORALL:  K < M < T  AND K AND R PORTS  START AT 1 BY 1        */
/*          LOOPA :  K < L < T  AND K AND R PORTS  START AT 1 BY 1        */
/*          LOOPB :  K < L      AND K AND R PORTS  START AT 1 BY 1        */
/**************************************************************************/

void  AssignNewPortNums( g, STestOpt )
register PNODE g;
int            STestOpt;
{
    register PNODE n;
    register int   p;
    register PEDGE e;
    register PEDGE i;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
        if ( !IsCompound( n ) )
	    continue;

	PropagateConst( n );

        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
            AssignNewPortNums( g, STestOpt );

        p = GenAssignNewKports( n );

	switch ( n->type ) {
	    case IFTagCase:
		for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		    GenAssignNewRports( g );

		break;

	    case IFSelect:
		n->S_TEST->imp->iport = 1;                   /* B PORT NUMBER */

		GenAssignNewRports( n->S_ALT );
		GenAssignNewRports( n->S_CONS );

		if ( STestOpt )
		  SelectTestOpt( n );

		break;

	    case IFForall:
		GenAssignNewTports( GenAssignNewMports( p, n ), n );
		GenAssignNewRports( n->F_RET );
                break;

	    case IFLoopA:
		n->L_TEST->imp->iport = 1;                   /* B PORT NUMBER */

                GenAssignNewLoopTports( GenAssignNewLports( p, n ), n );
		GenAssignNewRports( n->L_RET );
		break;

	    case IFLoopB:
		n->L_TEST->imp->iport = 1;                   /* B PORT NUMBER */

                GenAssignNewLports( p, n );
		GenAssignNewRports( n->L_RET );
		break;

            case IFUReduce:
		UNIMPLEMENTED( "IFUReduce" );
		break;

            default:
		UNEXPECTED( "Unknown compound" );
	    }

	for ( i = n->imp; i != NULL; i = i->isucc )
	    if ( i->iport < 0 )
		i->iport = -(i->iport);

	for ( e = n->exp; e != NULL; e = e->esucc )
	    if ( e->eport < 0 )
		e->eport = -(e->eport);

	for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) {
	    for ( i = g->imp; i != NULL; i = i->isucc )
		if ( i->iport < 0 )
		    i->iport = -(i->iport);

	    for ( e = g->exp; e != NULL; e = e->esucc )
		if ( e->eport < 0 )
		    e->eport = -(e->eport);
	    }
        }
}


/**************************************************************************/
/* LOCAL  **************         AReplaceOpt       ************************/
/**************************************************************************/
/* PURPOSE: LOCATE A GetArrayBase NODE FOR NODE n AND THREAD A REFERENCE  */
/*          TO n.  NULL IS RETURNED IF AN ASSOCIATED GetArrayBase NODE IS */
/*          NOT FOUND.                                                    */
/**************************************************************************/

static PEDGE AReplaceOpt( n, i, iport )
PNODE n;
PEDGE i;
int   iport;
{
  register PEDGE e;
  register PEDGE ee;
  register PEDGE ii;

  if ( i == NULL )               /* PREVIOUS FindImport MAY HAVE FAILED */
    return( NULL );

  for ( e = i->src->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != i->eport )
      continue;

    if ( IsGetArrayBase( e->dst ) ) {
      UnlinkNode( e->dst );       /* TO PRESERVE DATA FLOW ORDERING */
      LinkNode( i->src, e->dst );

      ee = EdgeAlloc( e->dst, 1, n, iport );
      ee->info = i->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER!!! */

      LinkExport( e->dst, ee );
      LinkImport( n, ee ); 
      arcnt++;

      return( ee );
      }
    }

  switch ( i->src->type ) {
    case IFSGraph:
      /* CHECK IF OPTIMIZATION ADDs OVERHEAD TO ARGUMENT FRAME  */
      /* CONSTRUCTION.                                          */
      if ( IsForall( i->src->G_DAD ) && i->src->G_DAD->smark )
        return( NULL );

      ii = FindImport( i->src->G_DAD, i->eport );

      if ( (ii = AReplaceOpt( i->src->G_DAD, ii, ++maxint )) == NULL )
        return( NULL );

      ee = EdgeAlloc( i->src, ii->iport, n, iport );
      ee->info = i->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER!!! */

      LinkExport( i->src, ee );
      LinkImport( n, ee );

      return( ee );

    case IFNoOp:
      if ( !(i->src->imp->omark1) || (i->src->imp->rmark1 != RMARK) ) /* RO? */
	return( NULL );

      if ( i->src->imp->src->type == IFOptAReplace ) {
	ii = FindImport( i->src->imp->src, MAX_PORT_NUMBER );
        ee = EdgeAlloc( ii->src, ii->eport, n, iport );
        ee->info = i->info;

        LinkExport( ii->src, ee );
        LinkImport( n, ee );

        return( ee );
	}

      return( AReplaceOpt( n, i->src->imp, iport ) );

    default:
      return( NULL );
    }
}


/**************************************************************************/
/* LOCAL  ************** TryAndInsertSpecGABNode   ************************/
/**************************************************************************/
/* PURPOSE: IF THE NoOp NODE ASSOCIATED WITH n IS A TRUE NoOp AND ITS     */
/*          INPUT IS A LOOP CARRIED VALUE, THEN INSERT A GetArrayBase     */
/*          NODE AND WIRE IT TO n.                                        */
/**************************************************************************/

static void TryAndInsertSpecGABNode( n )
PNODE n;
{
  register PNODE nop;
  register PNODE nn;
  register PEDGE e;

  nop = n->imp->src;

  if ( !(nop->imp->rmark1 == RMARK && nop->imp->omark1) )
    return;

  if ( IsConst( nop->imp ) )
    return;

  if ( !IsSGraph( nop->imp->src ) )
    return;

  if ( !IsLoop( nop->imp->src->G_DAD ) )
    return;

  nn = NodeAlloc( ++maxint, IFGetArrayBase );
  CopyVitals(n,nn);
  nn->funct = n->funct; nn->file = n->file; nn->line = n->line;
  LinkNode( n->npred, nn );

  e = EdgeAlloc( nn, 1, n, MAX_PORT_NUMBER );
  e->info = nop->imp->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER! */
  LinkExport( nn, e );
  LinkImport( n,  e );

  e = EdgeAlloc( nop->imp->src, nop->imp->eport, nn, 1 );
  e->info = nop->imp->info;
  e->pm   = nop->imp->pm;
  LinkExport( nop->imp->src, e );
  LinkImport( nn, e );

  sgabs++;
}


/**************************************************************************/
/* LOCAL  **************   DecoupleAReplaceNodes   ************************/
/**************************************************************************/
/* PURPOSE: DECOUPLE ARRAY BASE REFERENCE FROM INDEX OPERATION OF ALL     */
/*          CANDIDATE AReplace NODES IN GRAPH g.                          */
/**************************************************************************/

static void DecoupleAReplaceNodes( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        DecoupleAReplaceNodes( sg );

      continue;
      }

    if ( !IsAReplace( n ) )
      continue;

    if ( AReplaceOpt( n, n->imp, MAX_PORT_NUMBER ) != NULL )
      n->type = IFOptAReplace;
    else
      TryAndInsertSpecGABNode( n );
    }
}


/**************************************************************************/
/* LOCAL  **************       InsertGABNodes      ************************/
/**************************************************************************/
/* PURPOSE: DECOUPLE ARRAY BASE REFERENCE FROM THE INDEX OPERATION OF ALL */
/*          CANDIDATE AElement NODES IN GRAPH g.  AN AElement NODE IS     */
/*          A CANDIDATE FOR OPTIMIZATION IF ITS IMPORT DOESN't REQUIRE    */
/*          REFERENCE COUNTING.                                           */
/**************************************************************************/

static void InsertGABNodes( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE nn;
  register PEDGE i;
  register PEDGE e;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	InsertGABNodes( sg );
        continue;
	}

    if ( !IsAElement( n ) )
      continue;

    if ( !IsArray( n->imp->info ) )
      continue;

    if ( n->imp->cm == -1 )
      continue;

    nn = NodeAlloc( ++maxint, IFGetArrayBase );
    CopyVitals(n,nn);
    nn->wmark = n->wmark; n->wmark = FALSE;
    nn->funct = n->funct; nn->file = n->file; nn->line = n->line;
    LinkNode( n->npred, nn );

    UnlinkImport( i = n->imp );
    LinkImport( nn, i );

    e = EdgeAlloc( nn, 1, n, 1 );
    e->info = i->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER! */

    LinkExport( nn, e );
    LinkImport( n,  e );

    n->type = IFOptAElement; 
    gabc++;
    }
}


/**************************************************************************/
/* LOCAL  **************         GenIsUsed         ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF IMPORT i OF COMPOUND NODE c IS REFERENCED     */
/*          WITHIN c.  THE FIRST IMPORT OF A TAGCASE NODE IS ALWAYS USED. */
/**************************************************************************/

static int GenIsUsed( c, i )
PNODE c;
PEDGE i;
{
    register PNODE g;

    if ( IsTagCase(c) && i->iport == 1 )
	return( TRUE );

    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
        if ( IsExport( g, i->iport ) )
	    return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* LOCAL   **************       GenIsInvariant        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF ArrayBaseNode NODE n IS LOOP INVARIANT: IF ITS */
/*          SINGLE IMPORT IS A K PORT VALUE.                              */
/**************************************************************************/

static int GenIsInvariant( n )
PNODE n;
{
  register PEDGE i;

  if ( !IsSGraph( n->imp->src ) ) {
    return( FALSE );
    }

  i = FindImport( n->imp->src->G_DAD, n->imp->eport );

  if ( i == NULL ) {
    return( FALSE );
    }

  /* if ( i->cm == -1 || i->pm > 0 ) */
  if ( i->cm == -1 ) {
    return( FALSE );
    }

  if ( n->imp->cm == -1 || n->imp->pm > 0 ) {
    return( FALSE );
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************    CommonGABRemoval       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE COMMON GetArrayBase NODES IN GRAPH g.                 */
/**************************************************************************/

static void CommonGABRemoval( g )
PNODE g;
{
  register PNODE n1;
  register PNODE n2;
  register PNODE sn;

  for ( n1 = g->G_NODES; n1 != NULL; n1 = n1->nsucc ) {
    if ( n1->type == IFGetArrayBase ) {
      for ( n2 = n1->nsucc; n2 != NULL; n2 = sn ) {
        sn = n2->nsucc;

        if ( n1->type != n2->type )
	  continue;

        if ( n1->imp->eport != n2->imp->eport )
	  continue;
    
        if ( n1->imp->src != n2->imp->src )
	  continue;

	LinkExportLists( n1, n2 );
	UnlinkExport( n2->imp );
	UnlinkNode( n2 );  
	ccnt++;
	}

      continue;
      }

    if ( IsCompound( n1 ) )
      for ( g = n1->C_SUBS; g != NULL; g = g->gsucc )
        CommonGABRemoval( g );
    }
}


/**************************************************************************/
/* LOCAL  **************     InvarGABRemoval       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE LOOP INVARIANT GetArrayBase NODES FROM LOOP AND FORALL */
/*          NODES IN GRAPH g.  INVARIANTS OF INNERMOST LOOPS ARE REMOVED  */
/*          FIRST.  THE NODES ARE EXAMINED IN DATAFLOW ORDER.             */
/**************************************************************************/

static void InvarGABRemoval( g )
PNODE g;
{
  register PNODE n;
  register PNODE nd;
  register PEDGE i;
  register PEDGE e;
  register PNODE sn;
  register PNODE nn;
  register PEDGE ii;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( IsCompound( n ) ) {
      for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
        InvarGABRemoval( g );

      if ( !(IsForall( n ) || IsLoop( n ) || IsSelect( n )) )
        continue;

      for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
        for ( nd = g->G_NODES; nd != NULL; nd = sn ) {
          sn = nd->nsucc;

          if ( !IsGetArrayBase( nd ) )
            continue;

          if ( !GenIsInvariant( nd ) )  {
            continue;
	    }

          /* PLACE A COPY OF NODE nd BEFORE THE LOOP NODE */
  
          nn = NodeAlloc( ++maxint, nd->type );
          nn->wmark = n->wmark;
          LinkNode( n->npred, nn );
                      
          e = nd->exp; /* GUARANTEED TO BE ONLY ONE EXPORT!     */
          i = nd->imp;

          UnlinkExport( e );
          e->eport = ++maxint;
          LinkExport( i->src, e );
          UnlinkExport( i );

          /* ATTACH nn'S IMPORTS */
	  UnlinkImport( i );
	  LinkImport( nn, i );
  
          ii = FindImport( n, i->eport );
  
	  /* PRESERVE REFERENCE COUNTS!!! */
          i->sr = ii->sr;
          i->pm = ii->pm;
	  i->dmark = ii->dmark; /* CANN 10-3 */
          i->wmark = ii->wmark; /* CANN 10-3 */

          i->eport = ii->eport;

          LinkExport( ii->src, i );

          if ( !GenIsUsed( n, ii ) && (ii->cm != -1) ) {
            UnlinkImport( ii );
            UnlinkExport( ii ); 
	    rkcnt++;
            }
          else
            nrkcnt++;

          /* THREAD A REFERENCE TO nn'S EXPORT TO ITS USE IN n  */
          e = EdgeAlloc( nn, 1, n, maxint );
          e->info = i->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER!!! */
          LinkExport( nn, e );
          LinkImport( n,  e );
  
          UnlinkNode( nd ); 
	  vcnt++;
          }
      }
}


/**************************************************************************/
/* LOCAL  **************     GenCombineKports      ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS TO COMPOUND NODE c AND ADJUST ALL   */
/*          REFERENCES. NOTE, THE FIRST IMPORT TO TAGCASE NODES IS NEVER  */
/*          COMBINED WITH OTHER IMPORTS.                                  */
/**************************************************************************/

static void GenCombineKports( c )
PNODE c;
{
    register PEDGE i1;
    register PEDGE i2;
    register PNODE g;
    register PEDGE si;
    register PEDGE sii;

    i1 = (IsTagCase( c ))? c->imp->isucc : c->imp;

    for ( ; i1 != NULL; i1 = sii ) {
	sii = i1->isucc;

	if ( IsConst( i1 ) )
	    continue;

	if ( (i1->cm != -1) && !GenIsUsed( c, i1 ) ) {
	    UnlinkImport( i1 );
	    UnlinkExport( i1 ); 
	    rkcnt++;
	    continue;
	    }

	for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
	    si = i2->isucc;

	    if ( IsConst( i2 ) )
		continue;

	    if ( i1->eport != i2->eport )
		continue;

            if ( i1->src != i2->src )
		continue;

	    if ( (i1->cm == -1) || (i2->cm == -1) )
		continue;

	    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
	        ChangeExportPorts( g, i2->iport, i1->iport );
                
	    UnlinkImport( i2 );
	    UnlinkExport( i2 ); 
	    ckcnt++;
	    }
        }
}


/**************************************************************************/
/* LOCAL  **************     CombineGGABNode       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODE n1 IF A SIMILAR NODE IS DEFINED BELOW THE SCOPE   */
/*          DEFINING n1: g1. IF SUCH A NODE IS FOUND, n1 IS REMOVED FROM  */
/*          THE NODE LIST OF g1 AND THE APPROPRAITE REFERENCE EDGES ARE   */
/*          THREADED ACROSS ALL SUBGRAPH BOUNDARIES BETWEEN THE FOUND     */
/*          NODE AND THE NODES IMPORTING n1'S EXPORTS.  ALL REDUNDANT K   */
/*          PORT IMPORTS INTRODUCED BY THE THREADING PROCESS ARE REMOVED. */
/*          N1 IS ASSUMED TO PRODUCE ONE VALUE AND HAVE ONE IMPORT; THAT  */
/*          IS, BE A GetArrayBase NODE.                                   */
/**************************************************************************/

static void CombineGGABNode( g1, n1 )
PNODE g1;
PNODE n1;
{
  register PNODE n2;
  register PEDGE e;
  register int   t, f, l;
  register PEDGE se;

  for ( f = top; f >= 0; f-- ) {
    l = TRUE; /* ON THE LEFT OR RIGHT OF THE COMPOUND NODE? */

    for ( n2 = scopes[f].subg->G_NODES; n2 != NULL; n2 = n2->nsucc ) {
      if ( n2 == scopes[f].cmp )
        l = FALSE;

      if ( !GenAreNodesEqual( n1, n2 ) )
        continue;

      goto DoThreading;
      }
    }

  return;

DoThreading:

  if ( !l ) {                        /* PRESERVE THE DATA FLOW ORDERING */
    UnlinkNode( n2 );
    LinkNode( scopes[f].cmp->npred, n2 );
    }

  e = EdgeAlloc( n2, 1, scopes[f].cmp, ++maxint );
  e->info = n2->imp->info;

  LinkExport( n2, e );
  LinkImport( scopes[f].cmp, e );

  for ( t = f + 1; t <= top; t++ ) {
    e = EdgeAlloc( scopes[t].subg, maxint, scopes[t].cmp, maxint );
    e->info = n2->imp->info; /* NOT THE CORRECT TYPE, BUT FIXED LATER!!! */

    LinkExport( scopes[t].subg, e );
    LinkImport( scopes[t].cmp,  e );
    }

  for ( e = n1->exp; e != NULL; e = se ) {
    se = e->esucc;

    UnlinkExport( e );
    e->eport = maxint;
    LinkExport( g1, e );
    }

  UnlinkExport( n1->imp );
  UnlinkNode( n1 ); 
  gccnt++;

  for ( t = f; t <= top; t++ )
    GenCombineKports( scopes[t].cmp );
}


/**************************************************************************/
/* LOCAL  **************    GCommonGABRemoval      ************************/
/**************************************************************************/
/* PURPOSE: COMBINE GetArrayBase NODES DEFINED IN THE NODE LIST OF GRAPH  */
/*          g WITH IDENTICAL NODES FOUND OUTSIDE g. ONLY INVARIANTS ARE   */
/*          CONSIDERED.                                                   */
/**************************************************************************/

static void GCommonGABRemoval( g )
PNODE g;
{
  register PNODE n;
  register PNODE sn;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) ) {
      if ( (++top) >= MAX_SCOPE ) /* PUSH SCOPE ON SCOPE STACK */
        Error1( "GCommonGABRemoval: SCOPE STACK OVERFLOW" );

      scopes[top].subg = g;
      scopes[top].cmp  = n;

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        GCommonGABRemoval( sg );

      top--; /* POP SCOPE OFF SCOPE STACK */

      sn = n->nsucc;
      continue;
      }

    if ( IsSGraph( g ) )
      if ( IsGetArrayBase( n ) )
	if ( GenIsInvariant( n ) )
	  CombineGGABNode( g, n );
    }
}


static void CombineKs( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( IsCompound( n ) ) {
      GenCombineKports( n );

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	CombineKs( sg );
      }
}


/**************************************************************************/
/* LOCAL  **************      ImproveIndexing      ************************/
/**************************************************************************/
/* PURPOSE: ISOLATE FORALL LOOP INDEXING COMPUTATIONS AND ASSIGN SPECIAL  */
/*          INDEX NODES. THAT IS CONVERT tmpX = index + C; ...tmpY[tmpX]  */
/*          INTO ...tmpY[index + C].                                      */
/**************************************************************************/

static void ImproveIndexing( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE e;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( alliantfx )
      if ( IsForall( n ) && n->vmark )
        continue;

    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	ImproveIndexing( sg );

    switch ( n->type ) {
      case IFPlus:
      case IFMinus:
	if ( IsConst( n->imp ) ) {
	  if ( IsConst( n->imp->isucc ) ) 
	    break;

	  GenNormalizeNode( n );
          }

	/* AVOID POSSIBLE STRUCT REFERENCE IN INDEXING EXPRESSION */
	if ( IsConst( n->imp ) || (!(IsConst( n->imp->isucc ))) )
	  break;

	switch ( n->imp->src->type ) {
	  case IFSGraph:
	    /* NOT FOR ALLIANT VECTOR LOOPS */
	    if ( IsForall( n->imp->src->G_DAD ) && n->imp->src->G_DAD->vmark )
	      if ( !cRay )
	        goto MoveOn;

	    break;

          case IFCall:
	  case IFForall:
	  case IFLoopA:
	  case IFTagCase:
	  case IFSelect:
	  case IFXGraph:
	  case IFLGraph:
	    goto MoveOn;

	  default:
	    break;
	  }

	if ( !IsConst( n->imp->isucc ) )
	  goto MoveOn;

	for ( e = n->exp; e != NULL; e = e->esucc )
	  switch ( e->dst->type ) {
	    case IFOptAReplace:
	    case IFOptAElement:
	      if ( !(e->iport == 2) )
		goto MoveOn;

	      break;

	    default:
	      aifcnt++;
	      goto MoveOn;
	    }

        if ( e != NULL ) 
	  break;

	n->type = (n->type == IFPlus)? IFAIndexPlus : IFAIndexMinus;
        aicnt++;
MoveOn:
	break;

      default:
	break;
      }

    if ( IsForall( n ) && n->vmark && cRay )
      NormalizeVectorLoop( n );
    }
}


/**************************************************************************/
/* LOCAL  **************     NearestSuccessor      ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NEAREST SUCCESSOR NODE OF NODE n.                  */
/**************************************************************************/

static PNODE NearestSuccessor( n )
PNODE n;
{
  register PNODE ns;
  register PEDGE e;

  for ( ns = NULL, e = n->exp; e != NULL; e = e->esucc ) {
    if ( ns == NULL )
      ns = e->dst;

    if ( IsGraph( e->dst ) )
      continue;

    if ( IsGraph( ns ) )
      ns = e->dst;
    else if ( ns->label > e->dst->label )
      ns = e->dst;
    }

  if ( ns == NULL )
    Error2( "NearestSuccessor", "ns IS NULL" );

  return( ns );
}


/**************************************************************************/
/* LOCAL  **************        IsChained          ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n IS ALREADY PART OF A CRAY X-MP CHAIN.   */
/**************************************************************************/

static int IsChained( n )
PNODE n;
{
  if ( n->nsucc != NULL )
    if ( n->label == n->nsucc->label )
      return( TRUE );

  if ( n->npred != NULL )
    if ( n->label == n->npred->label )
      return( TRUE );

  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************     FormCrayXmpChains     ************************/
/**************************************************************************/
/* PURPOSE: CHAIN + AND - NODES WITH * AND / NODES TO IMPROVE CHAINING ON */
/*          THE CRAY X-MP.                                                */
/**************************************************************************/

static void FormCrayXmpChains( g )
PNODE g;
{
  register PNODE n;
  register PNODE nn;
  register PNODE sn;
  register int   l;
  register PNODE sg;

  for ( l = 0, n = g; n != NULL; n = n->nsucc )
    n->label = l++;

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	FormCrayXmpChains( sg );
      
      continue;
      }

    if ( IsChained( n ) )
      continue;

    switch ( n->type ) {
      case IFPlus:
      case IFMinus:
	if ( !(IsReal(n->exp->info) || IsDouble(n->exp->info) ||
	      IsInteger(n->exp->info)) )
	  break;

	nn = NearestSuccessor( n );

	switch ( nn->type ) {
	  case IFDiv:
	  case IFTimes:
	    if ( IsChained( nn ) )
	      break;

	    if ( n->nsucc != nn ) /* ALREADY NEXT TO EACH OTHER?? */
	      chains++;

	    UnlinkNode( n );
	    LinkNode( nn->npred, n );
	    n->label = nn->label;
	    break;

	  default:
	    break;
	  }

	break;

      case IFDiv:
      case IFTimes:
	if ( !(IsReal(n->exp->info) || IsDouble(n->exp->info) ||
	      IsInteger(n->exp->info)) )
	  break;

	nn = NearestSuccessor( n );

	switch ( nn->type ) {
	  case IFPlus:
	  case IFMinus:
	    if ( IsChained( nn ) )
	      break;

	    if ( n->nsucc != nn ) /* ALREADY NEXT TO EACH OTHER?? */
	      chains++;

	    UnlinkNode( n );
	    LinkNode( nn->npred, n );
	    n->label = nn->label;
	    break;

	  default:
	    break;
	  }
	break;

      default:
	break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    MigrateReadsUpward     ************************/
/**************************************************************************/
/* PURPOSE: MIGRATE ARRAY READ OPERATIONS IN LOOP BODIES TOWARD THE BODY  */
/*          ENTRY POINT TO HELP FILL MEMORY REFERENCE CHAINS AND TO       */
/*          IMPROVE ARITHMETIC INSTRUCTION CHAINING. THIS OPTIMIZATION    */
/*          IMPROVE THE PERFORMANCE OF SEQUENTIAL CODE ON THE CRAY X-MP.  */
/**************************************************************************/

static void MigrateReadsUpward( g, vmode )
PNODE g;
int   vmode;
{
  register PNODE sg;
  register PNODE n;
  register int   l;
  register PNODE nn;
  register PNODE nnn;
  register PNODE sn;
  register PEDGE i;

  /* DO NOT MOVE READS UPWARD INSIDE A VECTOR LOOP! */
  /* BUT FORM CRAY-XMP CHAINS                       */
  if ( vmode ) {
    if ( xmpchains )
      FormCrayXmpChains( g );

    return;
    }

  if ( !vmode )
    for ( l = 0, n = g; n != NULL; n = n->nsucc )
      n->label = l++;

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	MigrateReadsUpward( sg, (IsForall(n) && n->vmark)? TRUE : FALSE );
      
      continue;
      }

    if ( !movereads )
      continue;

    switch ( n->type ) {
      case IFGetArrayBase:
      case IFAIndexPlus:
      case IFAIndexMinus:
      case IFOptAElement:
	for ( nn = NULL, i = n->imp; i != NULL; i = i->isucc ) {
	  if ( IsConst( i ) )
	    continue;

	  if ( nn == NULL )
	    nn = i->src;
	  else if ( nn->label < i->src->label )
	    nn = i->src;
	  }

	if ( nn == NULL )
	  break;

	nnn = n->nsucc;

	UnlinkNode( n );
	LinkNode( nn, n );

	for ( l = nn->label+1; n != nnn; n = n->nsucc )
	  n->label = l++;

	rmov++;
	break;

      default:
	break;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************         If2AImp           ************************/
/**************************************************************************/
/* PURPOSE: OPTIMIZE ARRAY REFERENCE NODES IN ALL FUNCTION GRAPHS. ARRAY  */
/*          BASE DEREFERENCES ARE DECOUPLED FROM AElement AND AReplace    */
/*          NODES, COMBINED WITH OTHER COMMON DEREFERENCES, AND MOVED OUT */
/*          OF LOOP BODIES IF FOUND INVARIANT.  THIS OPTIMIZATION IS ONLY */
/*          DONE IF aimp IS TRUE. A SCATTERING OF OTHER OPTIMIZATIONS ARE */
/*          DONE (see the specific routines).                             */
/**************************************************************************/

void If2AImp()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( aimp ) {
      InsertGABNodes( f );
      InvarGABRemoval( f );

      CommonGABRemoval( f );

      GCommonGABRemoval( f );
      CombineKs( f );
      DecoupleAReplaceNodes( f );
      OptGABPaths( f );
      OptSpecGABPaths( f );

      FixGABExportTypes( f );

      ImproveIndexing( f );

      if ( movereads || xmpchains )
	MigrateReadsUpward( f, FALSE );
      }

    AssignNewPortNums( f, FALSE );
    }
}


/**************************************************************************/
/* GLOBAL **************     WriteIf2AImpInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE ARRAY IMPROVEMENT OPTIMIZATION FEEDBACK TO stderr.      */
/**************************************************************************/


void WriteIf2AImpInfo()
{
  FPRINTF( infoptr, "\n **** ARRAY IMPROVMENTS\n\n" );
  FPRINTF( infoptr, " GetArrayBase Nodes Inserted:          %d\n", gabc  );
  FPRINTF( infoptr, " Special GetArrayBase Nodes Inserted:  %d\n", sgabs );
  FPRINTF( infoptr, " GetArrayBase Invariants Removed:      %d\n", vcnt  );
  FPRINTF( infoptr, " GetArrayBase Nodes Combined:          %d\n", ccnt  );
  FPRINTF( infoptr, " Removed K Imports:                    %d\n", rkcnt );
  FPRINTF( infoptr, " Unremoved Array Imports:              %d\n", nrkcnt);
  FPRINTF( infoptr, " Combined K Imports:                   %d\n", ckcnt );
  FPRINTF( infoptr, " Globally Combined GetArrayBase Nodes: %d\n", gccnt );
  FPRINTF( infoptr, " Optimized AReplace Nodes:             %d\n", arcnt );
  FPRINTF( infoptr, " Optimized GetArrayBase Path Nodes:    %d\n", cgabp );
  FPRINTF( infoptr, " Migrated GetArrayBase Nodes:          %d\n", mgab  );
  FPRINTF( infoptr, " Special GetArrayBase Invariants:      %d\n", spicnt);
  FPRINTF( infoptr, " Array Indexing Optimizations:         %d\n", aicnt );
  FPRINTF( infoptr, " Failed Array Indexing Optimizations:  %d\n", aifcnt);
  FPRINTF( infoptr, " Normalized Vector Loops:              %d\n", nvl   );
  FPRINTF( infoptr, " Moved Array Read Operations:          %d\n", rmov   );
  FPRINTF( infoptr, " Formed Cray X-MP Chains:              %d\n", chains );
}

void WriteIf2AImpInfo2()
{
  FPRINTF( infoptr1, "\n **** SELECT TEST IMPROVEMENTS\n\n" );
  FPRINTF( infoptr1, " Optimized Select Tests:               %d of %d\n", sopt,scnt  );
}
