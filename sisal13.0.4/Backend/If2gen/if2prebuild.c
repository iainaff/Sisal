/* if2prebuild.c,v
 * Revision 12.7  1992/11/04  22:05:02  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int IsBIPWellFormed();
static int IsMAllocInvariant();
static int AreAllReadOnly();
static void ModifyRedAT();
static int GetBIPType();
static char mem[2000];
static char sptr[2000];

#define MAX_MALLOCS 100           /* MAX NUMBER OF MemAlloc STACK ENTRIES */
#define MAX_DIMS    5            /* MAX NUMBER OF POINTER SWAP DIMENSIONS */

int max_dims = 0;        /* MAX NUMBER OF DESIRED POINTER SWAP DIMENSIONS */

/* WARNING: IF MAX_BRECORD CHANGES THEN CHANGE IT IN if2part AND if2up  */
#define MAX_BRECORD 4   /* MAX NUMBER OF FIELDS ALLOWED IN A BASIC RECORD */

#define PREBUILD1   1        /* PREBUILD AND POINTER SWAP EXECUTION MODES */
#define PREBUILD2   2
static  int pmode;

static  int dim   = 0;            /* DIMENSION OF POINTER SWAP CANDIDATE  */

#define BIP_ONE     0             /* BIP SUBGRAPH DESCRIPTIONS            */
#define BIP_MULT    1
#define BIP_REG     2

static int brcnt   = 0;        /* COUNT OF BASIC RECORD NODE CONVERSIONS  */

static int bipreg  = 0;                  /* COUNT OF REGULAR BIPs         */
static int bipmult = 0;                  /* COUNT OF MULTIPLE-FANOUT BIPs */
static int bipone  = 0;                  /* COUNT OF SINGULAR-FANOUT BIPs */
static int bipats  = 0;                  /* COUNT OF BIP AT-NODES         */
static int biprts  = 0;                  /* COUNT OF BIP AT-NODE ROOTS    */
static int bipmvc  = 0;                  /* COUNT OF BIP MOVEMENTS        */

static int mchcnt  = 0;        /* COUNT OF COMBINED AND HOISTED MemAllocs */
static int tmchcnt  = 0;        /* COUNT OF COMBINED AND HOISTED MemAllocs */

static int psccnt = 0;                /* COUNT OF POINTER SWAP CANDIDATES */

static int psdcnt[6] = {0,0,0,0,0,0};    /* COUNT POINTER SWAP DIMENSIONS */

static int case1  = 0;          /* COUNT OF SUCCESSFUL POINTER SWAP CASES */
static int case2  = 0;
static int case3  = 0;
static int case4a = 0;
static int case4b = 0;
static int case5  = 0;
static int case6  = 0;
static int case7  = 0;

static int rcnt1  = 0;                        /* RAGGED ALLOCATE COUNTERS */
static int rinv   = 0;

static int macnt1 = 0;  /* IFMemAllocDVI ENCOUNTER COUNTERS FOR DEBUGGING */
static int macnt2 = 0;

static PNODE mema = NULL;                   /* MemAlloc NODE PLACE HOLDER */

static PEDGE sz[MAX_DIMS];                 /* REGION DIMENSION SIZE STACK */
static int   sztop = -1;       /* REGION DIMENSION SIZE STACK TOP POINTER */

static int pstype  = 0; /* POINTER SWAP CONFIGURATION VARIABLE AND MACROS */
static PEDGE spare = NULL;

static int ss1cnt = 0;              /* SHARE POINTER SWAP STORAGE COUNTER */

#define PS_COND         0      /* ALLOC ONE, NO FREE-GLOBAL IF_PSBuffer   */
#define PS_ONE          1      /* ALLOC ONE, FREE ONE */
#define PS_TWO          2      /* ALLOC TWO, SWAP, FREE TWO */
#define PS_SPARE1       3      /* ALLOC ONE, FREE IF ZERO TRIP */
#define PS_SPARE2       4      /* ALLOC TWO, SWAP, FREE BOTH IF ZERO TRIP */
                               /* OR FREE SPARE */

static PNODE root = NULL;                     /* CURRENT BIP ROOT DV NODE */


/**************************************************************************/
/* LOCAL  **************    ArePSAllocsIdentical   ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF PSAlloc NODES a1 AND a2 ARE IDENTICAL.         */
/**************************************************************************/

static int ArePSAllocsIdentical( a1, a2 )
PNODE a1;
PNODE a2;
{
  register PEDGE i1;
  register PEDGE i2;

  if ( a1->usucc->exp->info->A_ELEM->A_ELEM->tname != 
       a2->usucc->exp->info->A_ELEM->A_ELEM->tname )
    return( FALSE );

  i2 = a2->imp;

  for ( i1 = a1->imp; i1 != NULL; i1 = i1->isucc ) {
    if ( i2 == NULL )
      break;

    if ( i1->src != i2->src )
      return( FALSE );

    if ( i1->eport != i2->eport )
      return( FALSE );

    if ( IsConst( i1 ) )
      if ( strcmp( i1->CoNsT, i2->CoNsT ) != 0 )
	return( FALSE );

    if ( i1->iport != i2->iport )
      return( FALSE );

    i2 = i2->isucc;
    }

  if ( i1 != NULL || i2 != NULL )
    return( FALSE );

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************        ShareStorage       ************************/
/**************************************************************************/
/* PURPOSE: MERGE ALL SHARABLE POINTER SWAP STORAGE FOR case1 AND case2   */
/*          STORAGE SWAPS.                                                */
/**************************************************************************/

static void ShareStorage( g )
PNODE g;
{
  register PNODE sg;
  register PNODE a1;
  register PNODE a2;
  register PNODE sa1;
  register PNODE sa2;
  register int   c;
  register PEDGE i;
  register PEDGE si;
  register PEDGE e;
  register PEDGE se;
  register PNODE f1;

  /* RENUMBER THE NODES FOR DATA FLOW ORDER CHECKS!!! */
  for ( c = 0, a1 = g->G_NODES; a1 != NULL; a1 = a1->nsucc ) {
    a1->label = ++c;

    if ( IsCompound( a1 ) )
      for ( sg = a1->C_SUBS; sg != NULL; sg = sg->gsucc )
	ShareStorage( sg );
    }

  for ( a1 = g->G_NODES; a1 != NULL; a1 = sa1 ) {
    sa1 = a1->nsucc;

    if ( a1->type != IFPSAllocOne )
      continue;

    sa1 = NULL;

    for ( a2 = a1->nsucc; a2 != NULL; a2 = sa2 ) {
      sa2 = a2->nsucc;

      if ( a1->type != a2->type ) 
	continue;

      if ( !ArePSAllocsIdentical( a1, a2 ) ) {
        /* SHORTCUT THE OUTER LOOP SO IT WILL RUN FASTER */
        if ( sa1 == NULL )
	  sa1 = a2;

	continue;
	}

      /* DO THEY HAVE DIFFERENT m DESTINATION NODES? */
      c = 0;
      for ( e = a1->exp; e != NULL; e = e->esucc ) {
	if ( e->dst->type == IFPSFreeOne ) {
	  c++;
	  continue;
	  }

	/* SANITY CHECK!!! */
	if ( !(IsForall( e->dst ) || IsLoop( e->dst )) )
	  break;

	for ( i = e->dst->imp; i != NULL; i = i->isucc )
	  if ( i->src == a2 )
	    goto MoveOn;
	}
MoveOn:
      if ( c > 1 )
	Error2( "ShareStorage", "MORE THAN ONE FREE NODE!!!" );

      if ( e != NULL ) {
	/* SHORTCUT THE OUTER LOOP SO IT WILL RUN FASTER */
	if ( sa1 == NULL )
	  sa1 = a2;

	continue;
	}

      /* WIRE THEM USING a1 AS THE ALLOCATION NODE; PICK THE RIGHT MOST */
      /* FREE NODE AND DISCARD THE OTHER. */

      for ( i = a2->imp; i != NULL; i = si ) {
	si = i->isucc;

	UnlinkExport( i );
	UnlinkImport( i );
	/* free( i ); */
	}

      for ( e = a2->exp; e != NULL; e = se ) {
	se = e->esucc;

	UnlinkExport( e );
	LinkExport( a1, e );
	}

      UnlinkNode( a2 );

      /* RUN a1's EXPORTS AND KEEP RIGHTMOST IFPSFreeOne NODE */
      for ( f1 = NULL, e = a1->exp; e != NULL; e = e->esucc ) {
	if ( e->dst->type != IFPSFreeOne )
	  continue;

	if ( f1 != NULL ) {
	  if ( f1->label > e->dst->label )
	    f1 = e->dst;
	  }
        else
	  f1 = e->dst;
	}

      if ( f1 == NULL )
	Error2( "ShareStorage", "FREE NODE SEARCH FAILED" );

      if ( f1->imp->isucc != NULL )
	Error2( "ShareStorage", "FREE NODE WITH TWO IMPORTS" );

      UnlinkExport( f1->imp );

      /* ADJUST sa2 */
      if ( f1 == sa2 )
	sa2 = f1->nsucc;

      UnlinkNode( f1 );
      ss1cnt++;
      }
    }
}



/**************************************************************************/
/* LOCAL  **************   FindSourceEdgeAndLink   ************************/
/**************************************************************************/
/* PURPOSE: FIND THE TRUE SOURCE EDGE FOR EDGE e THAT IS JUST OUTSIDE     */
/*          THE DOMAIN OF m. THEN COPY THE SOURCE EDGE AND LINK IT TO     */
/*          dst AT IMPORT PORT iport.                                     */
/**************************************************************************/

static void FindSourceEdgeAndLink( dst, iport, m, e )
PNODE dst;
int   iport;
PNODE m;
PEDGE e;
{
  register PNODE l;
  register PEDGE ee;

  /* FIND THE TRUE SOURCE EDGE OF e WHICH IS IMMEDIATELY OUTSIDE OF m */
  /* OR IS A CONSTANT. E WILL BE THAT SOURCE! */
  for ( ;; ) {
    if ( IsConst( e ) )
      break;

    if ( !IsSGraph( e->src ) ) {
      if ( IsGraph( e->src ) ) {
	if ( m == e->src )
	  break;

	Error2( "FindSourceEdgeAndLink", "ILLEGAL SOURCE EDGE SOURCE [A]" );
	}

      if ( FindGraphNode( e->src ) == m )
	break;

      Error2( "FindSourceEdgeAndLink", "ILLEGAL SOURCE EDGE SOURCE [B]" );
      }

    if ( e->src->xmark ) {
      if ( e->src == m )
	break;
      }

    l = e->src->G_DAD;

    if ( !(IsForall( l ) || IsLoop( l )) )
      Error2( "FindSourceEdgeAndLink", "DIDN'T FIND m IN SEARCH" );

    if ( (e = FindImport( l, e->eport )) == NULL )
      Error2( "FindSourceEdgeAndLink", "FindImport FAILED [A]" );

    if ( l == m )
      break;
    }

  if ( IsConst( e ) ) {
    ee = EdgeAlloc( NULL_NODE, CONST_PORT, dst, iport );
    ee->info  = e->info;
    ee->CoNsT = e->CoNsT;
    LinkImport( dst, ee );
  } else {
    ee = EdgeAlloc( e->src, e->eport, dst, iport );
    ee->info  = e->info;
    LinkExport( e->src, ee );
    LinkImport( dst, ee );
    }
}


/**************************************************************************/
/* LOCAL  **************    WireRegionalSwap       ************************/
/**************************************************************************/
/* PURPOSE: WIRE THE POINTER SWAP FOR MemAllocDVI b->src.                 */
/**************************************************************************/

static void WireRegionalSwap( m, b )
PNODE m;
PEDGE b;
{
  register PNODE ma;
  register PNODE psma;
  register PNODE psmd;
  register PNODE psmm;
  register int   idx;
  register PNODE l;
  register int   matype = -1;
  register int   mdtype = -1;
  register int   mmtype = -1;
  register int   cport;
  register int   dport;
  register PNODE f;
  register PNODE dst;
  register PNODE scat;
  register PEDGE i;
  register PEDGE e;
  register int   d;

  /* FIND THE MemAllocDVI NODE */
  if ( b == NULL )
    Error2( "WireRegionalSwap", "BUFFER NOT FOUND" );

  /* FIND THE BUFFER IF IT CROSSES AN INTERCEDING SELECT IN THE INNERMOST */
  /* DIMENSION.                                                           */
  while ( IsSGraph( b->src ) ) {
    if ( !IsSelect( b->src->G_DAD ) )
      Error2( "WireRegionalSwap", "Select BUFFER SEARCH FAILED!" );

    if ( (b = FindImport( b->src->G_DAD, b->eport )) == NULL )
      Error2( "WireRegionalSwap", "SELECT BUFFER NOT FOUND" );
    }

  ma = b->src;

  if ( ma->type != IFMemAllocDVI )
    Error2( "WireRegionalSwap", "IFMemAllocDVI NODE NOT FOUND!!!" );

  macnt2++;

/* fprintf( stderr, "DEBUG WIR: [%x], dst=[%s,%s,%d]\n", ma, ma->exp->dst->file, ma->exp->dst->funct, ma->exp->dst->line );  */

  /* CONFIGURE THE RESTRUCTION AS A FUNCTION OF pstype */

  switch ( pstype ) {
    case PS_ONE:
      matype = IFPSAllocOne;
      mdtype = IFPSFreeOne;
      mmtype = IFPSManager;
      break;

    case PS_TWO:
      matype = IFPSAllocTwo;
      mdtype = IFPSFreeTwo;
      mmtype = IFPSManagerSwap;
      break;

    case PS_SPARE1:
      matype = IFPSAllocSpare1;
      mdtype = IFPSFreeSpare1;
      mmtype = IFPSManager;
      break;

    case PS_SPARE2:
      matype = IFPSAllocSpare2;
      mdtype = IFPSFreeSpare2;
      mmtype = IFPSManagerSwap;
      break;

    case PS_COND:
      matype = IFPSAllocCond;
      mmtype = IFPSManager;
      break;

    default:
      Error2( "WireRegionalSwap", "ILLEGAL pstype" );
    }

  /* ALLOCATE AND LINK IN THE POINTER SWAP MEMORY MALLOC NODE AND ITS */
  /* CORRESPONDING DEALLOCATION AND MANAGEMENT NODES */

  psma = NodeAlloc( ++maxint, matype );
  psma->norm = ma->norm;
  psma->usucc = ma;

  psmm = NodeAlloc( ++maxint, mmtype );
  psmm->usucc = root;

  if ( mdtype != -1 ) {
    psmd = NodeAlloc( ++maxint, mdtype );
    psmd->usucc = root;
    }

  switch ( m->type ) {
    case IFLoopA:
    case IFLoopB:
      cport = ++maxint;

      LinkNode( m->npred, psma );
      LinkNode( m->L_BODY, psmm );

      e = EdgeAlloc( psma, 1, m, cport );
      e->info = ptr;
      LinkExport( psma, e );
      LinkImport( m, e );

      e = EdgeAlloc( m->L_BODY, cport, psmm, 1 );
      e->info = ptr;
      LinkExport( m->L_BODY, e );
      LinkImport( psmm, e );

      break;

    case IFForall:
      cport = ++maxint;

      LinkNode( m->npred, psma );
      LinkNode( m->F_BODY, psmm );

      e = EdgeAlloc( psma, 1, m, cport );
      e->info = ptr;
      LinkExport( psma, e );
      LinkImport( m, e );

      e = EdgeAlloc( m->F_BODY, cport, psmm, 1 );
      e->info = ptr;
      LinkExport( m->F_BODY, e );
      LinkImport( psmm, e );

      break;

    case IFXGraph:
    case IFSGraph:
    case IFLGraph:
      if ( mdtype != -1 )
	Error2( "WireRegionalSwap", "ILLEGAL psmd NODE" );

      if ( !m->xmark )
	Error2( "WireRegionalSwap", "ILLEGAL GRAPH NODE [no xmark]" );

      if ( sztop >= 0 ) {
	l = sz[0]->dst->exp->dst->G_DAD;
	LinkNode( l->npred, psma );
	}
      else
	LinkNode( ma->npred, psma );

      LinkNode( psma, psmm );

      e = EdgeAlloc( psma, 1, psmm, 1 );
      e->info = ptr;
      LinkExport( psma, e );
      LinkImport( psmm, e );
      break;

    default:
      Error2( "WireRegionalSwap", "ILLEGAL m NODE" );
    }

  if ( mdtype != -1 ) {
    LinkNode( m, psmd );

    e = EdgeAlloc( psma, 1, psmd, 1 );
    e->info = ptr;
    LinkExport( psma, e );
    LinkImport( psmd, e );

    if ( spare != NULL ) {
      if ( !(pstype == PS_SPARE1 || pstype == PS_SPARE2) )
	Error2( "WireRegionalSwap", "ILLEGAL spare!!!" );

      e = EdgeAlloc( spare->src, spare->eport, psmd, 2 );

      e->info = spare->info;
      e->sr   = spare->sr;
      e->pm   = spare->pm;

      LinkExport( spare->src, e );
      LinkImport( psmd, e );

      spare = NULL;
      }
    }

  /* WIRE THE POINTER SWAP SIZE INFORMATION TO psma [FIRST iport IS dim!!] */
  d = ((sztop < 0)? -1 : sztop) + 1;
  FindSourceEdgeAndLink( psma, d, m, ma->imp );                  /* SIZE */
  FindSourceEdgeAndLink( psma, d+1, m, ma->imp->isucc );  /* LOWER BOUND */

  for ( idx = 0; idx <= sztop; idx++ )
    FindSourceEdgeAndLink( psma, d+2+idx, m, sz[idx] );     /* DIM-SIZES */

  /* THREAD psmm TO ma */
  ma->type = IFPSMemAllocDVI;

  dst = ma;

  dport = 3;
  cport = ++maxint;

  for ( idx = sztop; idx >= 0; idx-- ) {
    f = sz[idx]->dst->exp->dst->G_DAD;

    scat = NodeAlloc( ++maxint, IFPSScatter );
    LinkNode( f->F_BODY, scat );

    e = EdgeAlloc( scat, 1, dst, dport );
    e->info = ptr;
    LinkExport( scat, e );
    LinkImport( dst, e );

    e = EdgeAlloc( f->F_BODY, cport, scat, 1 );
    e->info = ptr;
    LinkExport( f->F_BODY, e );
    LinkImport( scat, e );

    if ( (i = f->F_GEN->imp) == NULL )
      Error2( "WireRegionalSwap", "GENERATE IMPORT NOT FOUND" );

    e = EdgeAlloc( f->F_BODY, i->iport, scat, 3 );
    e->info = i->info;
    LinkExport( f->F_BODY, e );
    LinkImport( scat, e );

    if ( i->src->type != IFRangeGenerate )
      Error2( "WireRegionalSwap", "RangeGenerate NOT FOUND" );

    i = i->src->imp;

    if ( IsConst( i ) ) {
      e = EdgeAlloc( NULL_NODE, CONST_PORT, scat, 2 );
      e->info  = i->info;
      e->CoNsT = i->CoNsT;
      LinkImport( scat, e );
    } else {
      e = EdgeAlloc( f->F_BODY, i->eport, scat, 2 );
      e->info  = i->info;
      LinkExport( f->F_BODY, e );
      LinkImport( scat, e );
      }

    dport = cport;
    dst = f;
    }

  e = EdgeAlloc( psmm, 1, dst, dport );
  e->info = ptr;
  LinkExport( psmm, e );
  LinkImport( dst, e );
}


/**************************************************************************/
/* LOCAL  **************      CountTheArms         ************************/
/**************************************************************************/
/* PURPOSE: INCREMENT macnt2 ACCORDINGLY FOR Select SUBGRAPH sg AND       */
/*          EXPORT eport.  THIS ROUTINE IS USED TO DIAGNOSE PROBLEMS!!!   */
/**************************************************************************/


static void CountTheArms( sg, eport )
PNODE sg;
int   eport;
{
  register PEDGE e;

  if ( (e = FindImport( sg, eport )) == NULL )
    Error2( "CountTheArms", "FindImport FOR SUBGRAPH FAILED" );

  if ( IsSelect( e->src ) ) {
    CountTheArms( e->src->S_CONS, e->eport );
    CountTheArms( e->src->S_ALT,  e->eport );
    }
  else
    macnt2++;
}


/**************************************************************************/
/* LOCAL  **************     RestructureTheBIP     ************************/
/**************************************************************************/
/* PURPOSE: POINTER SWAP THE BIP ROOTED BY EDGE i.                        */
/**************************************************************************/


static void RestructureTheBIP( m, i )
PNODE m;
PEDGE i;
{
  register PEDGE e;
  register PNODE n;
  register PEDGE lii;
  register PEDGE ii;
  register PNODE l;
  register PEDGE b;

  if ( IsConst( i ) )
    return;

  if ( !IsArray( i->info ) )
    return;

  n = i->src;

  switch ( n->type ) {
    case IFSelect:
      /* ONLY ALLOWED TO INTERCEDE FOR THE INNER DIMENSION: IsBIPWellFormed */
      if ( (e = FindImport( n->S_CONS, i->eport ) ) == NULL )
        Error2( "RestructureTheBIP", "FindImport ON SELECT FAILED" );

      /* INCREMENT macnt2 ACCORDINGLY... FOR DEBUG!!! */
      CountTheArms( n->S_ALT, i->eport );

      RestructureTheBIP( m, e );
      break;

    case IFOptNoOp:
      RestructureTheBIP( m, n->imp );
      break;

    case IFAAddLAT:
    case IFAAddHAT:
      if ( !(n->nmark) ) 
	Error2( "RestructureTheBIP", "ILLEGAL AAdd[LH]AT BIP NODE" );
    case IFAAddLATDV:
    case IFAAddHATDV:
      if ( n->imp->pmark ) 
	RestructureTheBIP( m, n->imp );

      /* POINTER SWAP n's MemAllocDVI NODE */
      if ( n->type == IFAAddLATDV || n->type == IFAAddHATDV ) {
        b = n->imp->isucc->isucc;
        WireRegionalSwap( m, b );
	}

      RestructureTheBIP( m, n->imp->isucc );
      break;

    case IFACatenateAT:
      if ( !(n->nmark) ) 
	Error2( "RestructureTheBIP", "ILLEGAL ACatenateAT BIP NODE" );
    case IFACatenateATDV:
      if ( n->imp->pmark ) 
	RestructureTheBIP( m, n->imp );

      /* POINTER SWAP n's MemAllocDVI NODE */
      if ( n->type == IFACatenateATDV ) {
        b = n->imp->isucc->isucc;
        WireRegionalSwap( m, b );
	}

      if ( n->imp->isucc->pmark ) 
	RestructureTheBIP( m, n->imp->isucc );

      break;

    case IFABuildAT:
      if ( !(n->nmark) ) 
	Error2( "RestructureTheBIP", "ILLEGAL ABuildAT BIP NODE" );
    case IFABuildATDV:
      lii = FindLastImport( n );
      for ( ii = n->imp->isucc; ii != lii; ii = ii->isucc )
	RestructureTheBIP( m, ii );

      /* POINTER SWAP n's MemAllocDVI NODE */
      if ( n->type == IFABuildATDV ) {
        b = FindLastImport( n );
        WireRegionalSwap( m, b );
	}

      break;

    case IFLoopA:
    case IFLoopB:
      /* ONLY ALLOWED FOR THE INNER DIMENSION: SEE IsBIPWellFormed */
      if ( (ii = FindImport( n->L_RET, i->eport )) == NULL )
	Error2( "RestructureTheBIP", "FindImport FOR IFLoop[AB] FAILED!" );

      RestructureTheBIP( m, ii );
      break;

    case IFForall:
      if ( (ii = FindImport( n->F_RET, i->eport )) == NULL )
	Error2( "RestructureTheBIP", "FindImport FOR IFForall FAILED!" );

      RestructureTheBIP( m, ii );
      break;

    case IFAGatherAT:
      if ( !(n->nmark) ) 
	Error2( "RestructureTheBIP", "ILLEGAL AGatherAT BIP NODE" );
    case IFAGatherATDV:
      l = n->exp->dst->G_DAD;

      e = n->imp->isucc;

      if ( (e = FindImport( l->F_BODY, e->eport ) ) != NULL ) {
        /* PUSH THE SIZE ONTO THE REGION SIZE STACK */
        if ( (++sztop) == MAX_DIMS )
	  Error2( "RestructureTheBIP", "sz OVERFLOW" );
        sz[sztop] = n->imp->isucc->isucc->isucc;

        RestructureTheBIP( m, e );

        /* POP THE REGION SIZE STACK */
        sztop--;
	}

      /* POINTER SWAP n's MemAllocDVI NODE */
      if ( n->type == IFAGatherATDV ) {
        b = FindImport( l, n->imp->isucc->isucc->eport );
        WireRegionalSwap( m, b );
	}

      break;

    case IFReduceAT:
    case IFRedLeftAT:
    case IFRedRightAT:
    case IFRedTreeAT:
      /* ONLY ALLOWED FOR THE INNER DIMENSION: SEE IsBIPWellFormed */
      if ( !(n->nmark) ) 
	Error2( "RestructureTheBIP", "ILLEGAL ReduceAT BIP NODE" );

      break;

    case IFReduceATDV:
      /* ONLY ALLOWED FOR THE INNER DIMENSION: SEE IsBIPWellFormed */

      /* POINTER SWAP n's MemAllocDVI NODE */
      b = FindImport( n->exp->dst->G_DAD, n->imp->isucc->isucc->isucc->eport );
      WireRegionalSwap( m, b );
      break;

    default:
      break;
    }
}


static void IncDimCounter() /* FOR THE -info FILE!!! */
{
  /* DEBUG CHECKS!!! */
  if ( macnt1 != macnt2 ) {
    FPRINTF( stderr, "HELP: dim=%d macnt1=%d macnt2=%d\n", dim, macnt1, macnt2);
    Error2( "IncDimCounter", "ASYMMETRIC POINTER SWAP ENCOUNTERED, UGH!!!" );
    }

  if ( dim < 1 )
    Error2( "IncDimCounter", "dim < 1" );

  if ( dim > 5 ) {
    psdcnt[0]++;
    return;
    }

  psdcnt[dim]++;
}


/**************************************************************************/
/* GLOBAL **************     GenNormalizeNode      ************************/
/**************************************************************************/
/* PURPOSE: IF NODE n IMPORTS A CONSTANT AND n IS A Plus OR Times NODE,   */
/*          THEN MAKE THE CONSTANT THE SECOND IMPORT.                     */
/**************************************************************************/

void GenNormalizeNode( n )
PNODE n;
{
  switch ( n->type ) {
    case IFPlus:
    case IFTimes:
      break;

    default:
      return;
    }

  if ( IsConst( n->imp ) )
    if ( IsInteger( n->imp->info ) )
      ImportSwap( n );
}


/**************************************************************************/
/* LOCAL  **************    GenIsEdgeInvariant        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE e IS INVARIANT TO m, OR DEFINED IN m'S    */
/*          NODE LIST.                                                    */
/**************************************************************************/

static int GenIsEdgeInvariant( m, e )
PNODE m;
PEDGE e;
{
  register PNODE c;

  if ( IsConst( e ) )
    return( TRUE );

  while ( IsSGraph( e->src ) ) {
    c = e->src->G_DAD;

    if ( (e = FindImport( c, e->eport )) == NULL )
      return( FALSE );

    if ( IsConst( e ) )
      return( TRUE );

    if ( c == m )
      return( TRUE );
    }

  if ( e->src == m )
    return( TRUE );

  if ( FindGraphNode( e->src ) == m )
    return( TRUE );

  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************   IsNextDimWellFormed     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF DIMENSION d DEFINED BY ARRAY i IS WELL FORMED  */
/*          WITHIN m'S CONFINES.                                          */
/**************************************************************************/

static int IsNextDimWellFormed( d, m, i )
int   d;
PNODE m;
PEDGE i;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE b;
  register PEDGE ae;
  register PEDGE ce;
  register PNODE ma;
  register PNODE rn;

  n = i->src;

  if ( IsGraph( n ) )
    return( FALSE );

  if ( i->pm > 0 || i->cm == -1 || i->dmark || i->sr != 0 )
    return( FALSE );

  switch ( n->type ) {
    case IFSelect:
      /* ONLY ALLOW SELECTS TO INTERCEDE IF d IS THE INNERMOST DIMENSION */
      if ( d != dim )
        return( FALSE );

      if ( UsageCount( n, i->eport ) != 1 )
        return( FALSE );

      if ( (ce = FindImport( n->S_CONS, i->eport ) ) == NULL )
        return( FALSE );

      if ( (ae = FindImport( n->S_ALT, i->eport )  ) == NULL )
        return( FALSE );

      if ( !IsNextDimWellFormed( d, m, ce ) )
        return( FALSE );

      ma = mema;

      if ( !IsNextDimWellFormed( d, m, ae ) )
        return( FALSE );

      /* ONLY POSSIBLE IF BOTH MemAlloc NODES WERE HOISTED!!! */
      if ( ma != mema )
        return( FALSE );

      return( TRUE );

    case IFAAddHATDV:
    case IFAAddLATDV:
    case IFACatenateATDV:
      b = n->imp->isucc->isucc;

      if ( (e = n->exp) == NULL )
        Error2( "IsNextDimWellFormed", "NO EXPORTS FOUND (A)" );

      break;

    case IFABuildATDV:
      b = FindLastImport( n );

      if ( (e = n->exp) == NULL )
        Error2( "IsNextDimWellFormed", "NO EXPORTS FOUND (B)" );

      break;

    case IFLoopA:
    case IFLoopB:
      /* ONLY ALLOW LOOPS TO INTERCEDE IF d IS THE INNERMOST DIMENSION */
      if ( d != dim )
	return( FALSE );

      if ( (e = FindImport( n->L_RET, i->eport)) == NULL )
        Error2( "IsNextDimWellFormed", "FindImport FAILED (A)" );

      goto DoTheRest;
    case IFForall:
      if ( (e = FindImport( n->F_RET, i->eport)) == NULL )
        Error2( "IsNextDimWellFormed", "FindImport FAILED (B)" );
DoTheRest:

      rn = e->src;

      switch ( rn->type ) {
        case IFAGatherATDV:
          b = FindImport( n, rn->imp->isucc->isucc->eport );
          break;

        case IFReduceATDV:
          /* ONLY ALLOW REDUCTIONS IF d IS THE INNERMOST DIMENSION */
	  if ( d != dim )
            return( FALSE );

          b = FindImport( e->dst->G_DAD, rn->imp->isucc->isucc->isucc->eport );
	  break;

        default:
          return( FALSE );
        }

      break;

    default:
      return( FALSE );
    }

  if ( b == NULL )
    Error2( "IsNextDimWellFormed", "BUFFER NOT FOUND" );

  /* FIND THE BUFFER IF IT CROSSES AN INTERCEDING SELECT IN THE INNERMOST */
  /* DIMENSION.                                                           */
  while ( IsSGraph( b->src ) ) {
    if ( !IsSelect( b->src->G_DAD ) )
      return( FALSE );

    if ( (b = FindImport( b->src->G_DAD, b->eport )) == NULL )
      Error2( "IsNextDimWellFormed", "SELECT BUFFER NOT FOUND" );
    }

  mema = b->src;

/* fprintf( stderr, "DEBUG: IsNextDimWellFormed [d=%d,dim=%d] PROCESSING MEMA\n",d,dim ); */

  if ( mema->type != IFMemAllocDVI )
    return( FALSE );

  if ( b->sr != 1 )
    return( FALSE );

  if ( !IsMAllocInvariant( m, mema ) )
    return( FALSE );

  if ( !IsBIPWellFormed( d, m, e ) )
    return( FALSE );

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     IsBIPWellFormed       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF BUILD-IN-PLACE EDGE i IS PART OF A WELL FORMED */
/*          BIP CONSTRUCTION WITHIN m'S CONFINES. THE CURRENT DIMENSION   */
/*          BEING CONSIDERED IS d.                                        */
/**************************************************************************/

static int IsBIPWellFormed( d, m, i )
int   d;
PNODE m;
PEDGE i;
{
  register PEDGE e;
  register PNODE n;
  register PEDGE lii;
  register PEDGE ii;
  register PNODE l;

  
  /* ARE WE PROCESS THE INNER DIMENSION? IF SO, WE KNOW IT IS OK! */
  if ( d >= dim )
    return( TRUE ); 

  n = i->src;

  switch ( n->type ) {
    case IFOptNoOp:
      return( IsBIPWellFormed( d, m, n->imp ) );

    case IFAAddLAT:
    case IFAAddHAT:
      if ( !(n->nmark) ) Error2( "IsBIPWellFormed", "ILLEGAL BIP NODE" );
    case IFAAddLATDV:
    case IFAAddHATDV:
      if ( n->imp->pmark ) 
        if ( !IsBIPWellFormed( d, m, n->imp ) )
          return( FALSE );

      return( IsNextDimWellFormed( d+1, m, n->imp->isucc ) );

    case IFACatenateAT:
      if ( !(n->nmark) ) Error2( "IsBIPWellFormed", "ILLEGAL BIP NODE" );
    case IFACatenateATDV:
      if ( n->imp->pmark ) 
        if ( !IsBIPWellFormed( d, m, n->imp ) )
          return( FALSE );

      if ( n->imp->isucc->pmark ) 
        if ( !IsBIPWellFormed( d, m, n->imp->isucc ) )
          return( FALSE );

      break;

    case IFABuildAT:
      if ( !(n->nmark) ) Error2( "IsBIPWellFormed", "ILLEGAL BIP NODE" );
    case IFABuildATDV:
      lii = FindLastImport( n );

      for ( ii = n->imp->isucc; ii != lii; ii = ii->isucc )
        if ( !IsNextDimWellFormed( d+1, m, ii ) )
          return( FALSE );

      break;

    case IFLoopA:
    case IFLoopB:
      /* ONLY ALLOW LOOPS TO INTERCEDE IF d IS THE INNERMOST DIMENSION, WHICH */
      /* IS NOT THE CASE AT THIS TIME! */
      return( FALSE );

    case IFForall:
      /* SIMPLY IGNORE THE Forall BOUNDARY AND PROCESS THE ACTUAL AT NODE IN */
      /* THE RETURN SUBGRAPH */
      return( IsBIPWellFormed( d, m, FindImport( n->F_RET, i->eport ) ) );

    case IFAGatherAT:
      if ( !(n->nmark) ) Error2( "IsBIPWellFormed", "ILLEGAL BIP NODE" );
    case IFAGatherATDV:
      l = n->exp->dst->G_DAD;

      /* ONLY ALLOW LOOPS TO INTERCEDE IF d IS THE INNERMOST DIMENSION, WHICH */
      /* IS NOT THE CASE AT THIS TIME! */
      if ( IsLoop( l ) )
        return( FALSE );

      /* IS THE SIZE VARIANT TO m? */
      if ( !GenIsEdgeInvariant( m, n->imp->isucc->isucc->isucc ) )
        return( FALSE );

      e = n->imp->isucc;

      if ( e->pm > 0 || e->dmark || e->cm == -1 )
        return( FALSE );

      if ( UsageCount( l->F_RET, e->eport ) != 1 )
        return( FALSE );

      if ( (e = FindImport( l->F_BODY, e->eport ) ) == NULL )
        return( FALSE );

      return( IsNextDimWellFormed( d+1, m, e ) );

    case IFReduceAT:
    case IFRedLeftAT:
    case IFRedRightAT:
    case IFRedTreeAT:
      if ( !(n->nmark) ) Error2( "IsBIPWellFormed", "ILLEGAL BIP NODE" );
    case IFReduceATDV:
      /* ONLY ALLOW REDUCTIONS TO INTERCEDE IF d IS THE INNERMOST DIMENSION, */
      /* WHICH IS NOT THE CASE AT THIS TIME! */
      return( FALSE );

    default:
      return( FALSE );
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************       IsPSCandidate        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF INFO i IS A CANDIDATE FOR POINTER SWAP.        */
/**************************************************************************/

static int IsPSCandidate( i )
PINFO  i;
{
  if ( IsBasic( i ) )
    return( TRUE );

  if ( IsArray( i ) ) {
    dim++;
    return( IsPSCandidate( i->A_ELEM ) );
    }

  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************       IsGround            ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE e IS A GROUND EDGE.                       */
/**************************************************************************/

static int IsGround( e )
PEDGE e;
{
  if ( e->iport != 0 ) 
    return( FALSE );

  if ( !IsGraph( e->dst ) )
    return( FALSE );

  if ( !(e->cm == -1 || e->dmark) )
    return( FALSE );

  return( TRUE );
}


/**************************************************************************/
/* LOCAL **************      IsEdgeReadOnly        ************************/
/**************************************************************************/
/* PURPOSE: DETERMINE IF THE AGGREGATE CARRIED ON EDGE e IS READ-ONLY.    */
/**************************************************************************/

static int IsEdgeReadOnly( e )
PEDGE  e;
{
  register PNODE f;
  register PNODE n;

  if ( e->cm == -1 || e->pm > 0 || e->wmark || e->dmark )
    return( FALSE );

  n = e->dst;

  switch ( n->type ) {
    case IFALimL:
    case IFALimH:
    case IFPrefixSize:
    case IFASize:
      break;

    case IFCall:
      if ( (f = FindFunction( n->imp->CoNsT )) == NULL )
          Error2( "IsEdgeReadOnly", "FindFunction FOR CALL FAILED" );

      if ( IsIGraph( f ) )
	if ( f->mark != 's' )  /* NEW CANN 2/92 */
          break;

      return( FALSE );

    case IFSaveCallParam:
    case IFSaveSliceParam:
    case IFLoopPoolEnq:
    case IFOptLoopPoolEnq:
      /* DONE DURING IF2Prebuild1 BEFORE If2Yank, SO ABORT OUT */
      return( FALSE );

    case IFOptAElement:
      if ( e->info->type != IF_PTR )
        break;

      if ( !AreAllReadOnly( n, 1 ) )
        return( FALSE );

      break;

    case IFAElement:
      if ( IsBasic( e->info->A_ELEM ) )
        break;
    case IFGetArrayBase:
      if ( !AreAllReadOnly( n, 1 ) )
        return( FALSE );

      break;

    case IFSelect:
      if ( IsExport( n->S_TEST, e->iport ) != 0 )
        return( FALSE );

      if ( IsExport( n->S_CONS, e->iport ) != 0 )
        if ( !AreAllReadOnly( n->S_CONS, e->iport ) )
          return( FALSE );

      if ( IsExport( n->S_ALT, e->iport ) != 0 )
        if ( !AreAllReadOnly( n->S_ALT, e->iport ) )
          return( FALSE );

      break;

    case IFForall:
      if ( IsExport( n->F_GEN, e->iport ) != 0 )
        return( FALSE );
      if ( IsExport( n->F_RET, e->iport ) != 0 )
        return( FALSE );

      if ( !AreAllReadOnly( n->F_BODY, e->iport ) )
        return( FALSE );

      break;

    case IFLoopA:
    case IFLoopB:
      if ( IsExport( n->L_INIT, e->iport ) != 0 )
        return( FALSE );
      if ( IsExport( n->L_TEST, e->iport ) != 0 )
        return( FALSE );
      if ( IsExport( n->L_RET,  e->iport ) != 0 )
        return( FALSE );

      if ( !AreAllReadOnly( n->L_BODY, e->iport ) )
        return( FALSE );

      break;

    default:
      return( FALSE );
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL **************      AreAllReadOnly        ************************/
/**************************************************************************/
/* PURPOSE: DETERMINE IF ALL REFERENCED TO AGGREGATE eport OF NODE n ARE  */
/*          READ-ONLY.                                                    */
/**************************************************************************/

static int AreAllReadOnly( n, eport )
PNODE  n;
int    eport;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( !IsEdgeReadOnly( e ) )
      return( FALSE );
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     GetWriteEdge          ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF NODE n AT eport HAS A SINGLE WRITE ONLY REFERENCE.   */
/*          IF SO, RETURN IT.                                             */
/**************************************************************************/

static PEDGE GetWriteEdge( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;
  register PEDGE we;

  we = NULL;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( IsEdgeReadOnly( e ) )
      continue;

    /* TWO OR MORE WRITE REFERENCES */
    if ( we != NULL ) {
      return( NULL );
      }

    we = e;
    }

  if ( we == NULL ) {
    return( NULL );
    }

  /* OUTER LEVEL IS WRITE? */
  if ( we->wmark )
    return ( we );
      
  return( NULL );
}


/**************************************************************************/
/* LOCAL  **************     IsMAllocInvariant     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF MAlloc NODE ma IS INVARIANT WITH RESPECT TO m. */
/**************************************************************************/

static int IsMAllocInvariant( m, ma )
PNODE m;
PNODE ma;
{
  register PEDGE i;

  /* ABORT IF ma IS RAGGED!!! */
  if ( ma->wmark )
    return( FALSE );

  /* ABORT IF NOT A DVI NODE */
  if ( ma->type != IFMemAllocDVI )
    return( FALSE );

  for ( i = ma->imp; i != NULL; i = i->isucc )
    if ( !GenIsEdgeInvariant( m, i ) )
      return( FALSE );

  macnt1++;

/* fprintf( stderr, "DEBUG INV: [%x], dst=%s%s%d\n", ma, ma->exp->dst->file, ma->exp->dst->funct, ma->exp->dst->line );  */
  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************       PointerSwap         ************************/
/**************************************************************************/
/* PURPOSE: APPLY THE POINTER-SWAP OPTIMIZATION TO GRAPH g WITH OWNER m.  */
/*                                                                        */
/*  case1 := for i in 1,n
               x := for j in 1,n returns array of j + i end for;
               y := x[1];
             returns value of sum y
             end for; */
/*                                                                        */
/*  case2 := for initial
               i := 1; y := 0;
             while ( i <= n ) repeat
               i := old i + 1;
               x := for j in 1,n returns array of j + old i end for;
               y := x[1];
             returns value of sum y
             end for; */
/*                                                                        */
/*  case3 := for initial
               i := 1; y := 0;
               x := array_fill(1,n,1);
             while ( i <= n ) repeat
               i := old i + 1;
               x := for j in 1,n returns array of j + old x[j] end for;
               y := x[1];
             returns value of sum y
             end for; */
/*                                                                        */
/*  case5 := for initial
               i := 1; y := 0;
               x := array_fill(1,n,1);
             while ( i <= n ) repeat
               i := old i + 1;
               x := for j in 1,n returns array of j + old x[j] end for;
               y := x[1];
             returns value of x
             end for; */
/*                                                                        */
/*  case6 := for initial
               i := 1; y := 0;
               x := array_fill(1,n,1);
             while ( i <= n ) repeat
               i := old i + 1;
               x := for j in 1,n returns array of j + old i end for;
               y := x[1];
             returns value of x
             end for; */
/*                                                                        */
/*  case7 := for i in 1,n
               x := for j in 1,n returns array of j + i end for;
             returns value of x
             end for; */
/*                                                                        */
/*  %$entry=case4a
    define case4a
    type oned = array[integer];
    function case4a( returns integer )
      let
        x := for i in 1,1000
             returns array of i
             end for
      in
        x[43]
      end let
    end function */
/*                                                                        */
/*  %$entry=case4b
    define caseb
    type oned = array[integer];
    function case4b( returns oned )
      let
        x := for i in 1,1000
             returns array of i
             end for
      in
        x
      end let
    end function */
/*                                                                        */
/**************************************************************************/


void PointerSwap( m, g )
PNODE m;
PNODE g;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE we;
  register PEDGE wee;
  register PEDGE ret;
  register PEDGE ce;
  register PEDGE ci;
  register PEDGE b;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    switch ( n->type ) {
      case IFForall:
        if ( n->smark ) {
          if ( pmode == PREBUILD2 )
            PointerSwap( n, n->F_BODY );
          }
        else
          PointerSwap( n, n->F_BODY );

        PointerSwap( m, n->F_RET );
        break;

      case IFLoopA:
      case IFLoopB:
        PointerSwap( n, n->L_BODY );
        PointerSwap( m, n->L_RET );
        break;

      case IFSelect:
        PointerSwap( n->S_CONS, n->S_CONS );
        PointerSwap( n->S_ALT,  n->S_ALT );
        break;

      case IFTagCase:
        /* NOT IMPLEMENTED */
        break;

      /* THE BIP ROOT NODES FOLLOW!!! */

      case IFABuildATDV:
        if ( IsLoop( m ) || IsForall( m ) || m->xmark )
          psccnt++;
        else
          break;

        b = FindLastImport( n );
        if ( b->sr != 1 )
          break;

        if ( (e = n->exp) == NULL )
          Error2( "PointerSwap", "NO EXPORTS FOUND (A)" );

        goto DoTheRest;

      case IFAAddLATDV:
      case IFAAddHATDV:
      case IFACatenateATDV:
        if ( IsLoop( m ) || IsForall( m ) || m->xmark )
          psccnt++;
        else
          break;

        b = n->imp->isucc->isucc;

        if ( b->sr != 1 )
          break;

        if ( (e = n->exp) == NULL )
          Error2( "PointerSwap", "NO EXPORTS FOUND (B)" );

        goto DoTheRest;

      case IFAGatherATDV:
        if ( IsLoop( m ) || IsForall( m ) || m->xmark )
          psccnt++;
        else
          break;

        b = FindImport( n->exp->dst->G_DAD, n->imp->isucc->isucc->eport );
        if ( b == NULL )
          Error2( "PointerSwap", "CANNOT FIND BUFFER SOURCE (A)" );

        if ( b->sr != 1 )
          break;

        if ( (e = FindExport( n->exp->dst->G_DAD, n->exp->iport )) == NULL )
          Error2( "PointerSwap", "FindExport FAILED (A)" );

        goto DoTheRest;

      case IFReduceATDV:
        if ( IsLoop( m ) || IsForall( m ) || m->xmark )
          psccnt++;
        else
          break;

        b = FindImport( n->exp->dst->G_DAD, n->imp->isucc->isucc->isucc->eport);
        if ( b == NULL )
          Error2( "PointerSwap", "CANNOT FIND BUFFER SOURCE (B)" );

        if ( b->sr != 1 )
          break;

        if ( (e = FindExport( n->exp->dst->G_DAD, n->exp->iport )) == NULL )
          Error2( "PointerSwap", "FindExport FAILED (B)" );

DoTheRest:
        dim = 1;

	macnt1 = 0;
	macnt2 = 0;

        if ( !IsPSCandidate( e->info->A_ELEM ) )
          break;

        if ( dim > max_dims ) 
	  break;

        if ( e->sr != 0 || e->pm > 0 )
          break;

        if ( (we = GetWriteEdge(e->src,e->eport)) == NULL )
          break;

/* fprintf( stderr, "DEBUG: GO GO\n" ); */
/* fprintf( stderr, "DEBUG: OUTER PROCESSING MEMA [d=1,dim=%d]\n", dim ); */
        if ( !IsMAllocInvariant( m, b->src ) ) {
          break;
          }

        if ( !IsBIPWellFormed( 1, m, n->exp ) ) {
          break;
          }

	root  = n;
	spare = NULL;

        switch ( m->type ) {
          case IFForall:
            if ( IsGround( we ) ) {

	      pstype = PS_ONE;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* GET RID OF THE GROUND EDGE */
	      UnlinkImport( we );
	      UnlinkExport( we );
	      /* free( we ); */

              IncDimCounter();
              case1++;
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              break;
              }


            if ( (we->dst != m->F_BODY) || (we->cm == -1) || (we->pm > 0) || 
                 (we->dmark) || we->iport == 0 )
              break;

            ret = FindExport( m->F_RET, we->iport  );

            if ( ret != NULL ) {
              if ( ret->dst->type != IFFinalValue )
                break;
              if ( UsageCount( m->F_RET, ret->eport ) > 1 )
                break;
              if ( !(ret->dmark || ret->cm == -1) )
                break;
              if ( ret->pm > 0 )
                break;
              if ( ret->dst->lmark )
                break;

	      pstype = PS_SPARE1;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* we IS GUARANTEED OK! */

	      /* CLEAN THE RETURN SUBGRAPH REFERENCE */
	      ret->dmark = FALSE;
	      ret->cm = 0;

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case7++;
              break;
              }

            break;

          case IFLoopA:
          case IFLoopB:
            if ( IsGround( we ) ) {
	      pstype = PS_ONE;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* GET RID OF THE GROUND EDGE */
	      UnlinkExport( we );
	      UnlinkImport( we );
	      /* free( we ); */

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case2++;
              break;
              }

            if ( (we->dst != m->L_BODY) || (we->cm == -1) || (we->pm > 0) || 
                 (we->dmark) || we->iport == 0 )
              break;

            ret = FindExport( m->L_RET, we->iport  );
            ce  = FindExport( m->L_BODY, we->iport );

            /* NOT RETURNED BUT CARRIED??? */
            if ( ret == NULL && ce != NULL ) {
              if ( (wee = GetWriteEdge(m->L_BODY,ce->eport)) == NULL )
                break;

              if ( !IsGround( wee ) )
                break;

	      /* MAKE SURE THE INITIAL REFERENCE IS OK! */
	      if ( (ci = FindImport( m->L_INIT, wee->eport )) == NULL )
		break;
	      if ( ci->cm != -1 || ci->pm > 0 )
		break;

	      pstype = PS_TWO;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* GET RID OF THE GROUND EDGE */
	      UnlinkExport( wee );
	      UnlinkImport( wee );
	      /* free( wee ); */

	      /* NOTE: we IS GUARANTEED OK! */

	      /* CLEAN THE INITIAL SUBGRAPH */
	      if ( ci->cm == -1 || ci->dmark ) {
		ci->cm = 0;
		ci->dmark = FALSE;

		/* TRY AND FREE m'S INITIAL IMPORT */
	        if ( (ci = FindImport( m, ci->eport )) == NULL )
		  Error2( "PointerSwap", "m FindImport FAILED [A]\n" );

		ci->cm = -1;
		}

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case3++;
              break;
              }

            /* RETURNED AND CARRIED??? */
            if ( ret != NULL && ce != NULL ) {
              if ( (wee = GetWriteEdge(m->L_BODY,ce->eport)) == NULL )
                break;

              if ( !IsGround( wee ) )
                break;

              if ( ret->dst->type != IFFinalValue )
                break;
              if ( UsageCount( m->L_RET, ret->eport ) > 1 )
                break;
              if ( ret->dmark || ret->cm == -1 || ret->pm > 0 )
                break;
              if ( !(ret->dst->lmark) )
                break;

	      /* MAKE SURE THE INITIAL REFERENCE IS OK! AND SET THE SPARE */
	      if ( (ci = FindImport( m->L_INIT, wee->eport )) == NULL )
		break;
	      if ( ci->cm == -1 || ci->dmark || ci->pm > 0 )
		break;
              if ( (ci = FindImport( m, ci->eport )) == NULL )
		break;
              if ( ci->cm == -1 || ci->dmark )
		break;
	      if ( IsConst( ci ) )
		break;
              spare = ci;

	      pstype = PS_SPARE2;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* GET RID OF THE GROUND EDGE, REST IS OK! */
	      UnlinkExport( wee );
	      UnlinkImport( wee );
	      /* free( wee ); */

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case5++;
              break;
              }

            /* RETURNED BUT NOT CARRIED??? */
            if ( ret != NULL && ce == NULL ) {
              if ( ret->dst->type != IFFinalValue )
                break;
              if ( UsageCount( m->L_RET, ret->eport ) > 1 )
                break;
              if ( !(ret->dmark || ret->cm == -1) )
                break;
              if ( ret->pm > 0 )
                break;
              if ( ret->dst->lmark )
                break;

	      /* MAKE SURE THE INITIAL REFERENCE IS OK! AND SET THE SPARE */
	      if ( (ci = FindImport( m->L_INIT, ret->eport )) == NULL )
		break;
	      if ( ci->cm == -1 || ci->dmark || ci->pm > 0 )
		break;
              if ( (ci = FindImport( m, ci->eport )) == NULL )
		break;
              if ( ci->cm == -1 || ci->dmark )
		break;
	      if ( IsConst( ci ) )
		break;
              spare = ci;

	      pstype = PS_SPARE1;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* CLEAN UP THE FinalValue NODE */
	      ret->dmark = FALSE;
	      ret->cm = 0;

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case6++;
              break;
              }

            break;

          case IFLGraph:
          case IFSGraph:
          case IFXGraph:
            if ( !(m->xmark) )
              break;

            if ( IsGround( we ) ) {
	      pstype = PS_COND;
	      sztop = -1;
	      RestructureTheBIP( m, n->exp );

	      /* GET RID OF THE GROUND EDGE */
	      UnlinkExport( we );
	      UnlinkImport( we );
	      /* free( we ); */

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case4a++;
              break;
              }

	    /* WOW I'M SMART!!! */
            if ( !IsXGraph(m) )
              break;

	    /* CCCCC */
            /* A TRUE GRAPH EXPORT??? */
            if ( (we->dst == m) && (we->iport != 0) && 
                 (we->cm != -1) && (we->pm <= 0) ) {
	      pstype = PS_COND;
	      sztop = -1; 
	      RestructureTheBIP( m, n->exp );

	      /* MINOR CLEANUP REQUIRED TO PREVENT DEALLOCATION */
	      we->xmark = TRUE;

              IncDimCounter();
#ifdef MYI
              SPRINTF(sptr,
                  "%s Swapping pointer %4d at line %s, funct %s, file %s\n",
                  sptr, n->ID, n->line, n->funct, n->file);
              SPRINTF(sptr,
                  "%s with pointer %4d at line %s, funct %s, file %s\\nn",
                  sptr, m->ID, m->line, m->funct, m->file);
#endif
              case4b++;
              break;
              }

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
/* GLOBAL **************      MyIsPMarked          ************************/
/**************************************************************************/
/* PURPOSE: CALL IsPMakred RECORDING RECORDING THE ATTEMPTS AND RESULTS.  */
/**************************************************************************/

static int MyIsPMarked( n, eport )
PNODE n;
int   eport;
{
  register int st;

  bipats++;

  if ( !(st = IsPMarked( n, eport )) )
    biprts++;

  return( st );
}


/**************************************************************************/
/* LOCAL  **************        DriveXmark         ************************/
/**************************************************************************/
/* PURPOSE: DRIVE Xmark THROUGH GRAPH g.                                  */
/**************************************************************************/

static void DriveXmark( g )
PNODE g;
{
  register PNODE n;

  /* ALREADY DONE */
  if ( g->xmark )
    return;

  if ( IsIGraph( g ) )
    return;

  if ( IsLGraph( g ) ) {
    /* if ( (!(g->bmark)) && (!(g->Pmark)) && (!(g->emark)) ) */
    /* NEW CANN 2/92 */
    /* NOT RECURSIVE AND NOT CALLED IN PARALLEL */
    if ( !(g->bmark) && !(g->Pmark) )

      g->xmark = TRUE;
    else
      return;
    }
  else
    return;

  if ( !(g->xmark) ) return;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    switch ( n->type ) {
      case IFSelect:
	if ( g->xmark ) {
          n->S_CONS->xmark = TRUE;
          n->S_ALT->xmark  = TRUE;
          DriveXmark( n->S_CONS );
          DriveXmark( n->S_ALT );
	  }

        break;

      case IFCall:
        DriveXmark( FindFunction( n->imp->CoNsT ) );
        break;

      default:
        break;
      }
}


/**************************************************************************/
/* LOCAL  **************    FixBRecordOperators    ************************/
/**************************************************************************/
/* PURPOSE: FIX THE BASIC RECORD OPERATORS IN GRAPH g. ALSO ESTABLISH     */
/*          xmarkS USED LATER DURING POINTER SWAP ANALSYS.                */
/**************************************************************************/

static void FixBRecordOperators( g, ok )
PNODE g;
int   ok;
{
  register PNODE n;
  register PEDGE i;
  register PEDGE ee;
  register PEDGE se;
  register PEDGE si;
  register PNODE nop;
  register PNODE sg;

  for ( n = g; n != NULL; n = n->nsucc ) {
    for ( i = n->imp; i != NULL; i = si ) {
      si = i->isucc;

      if ( !IsBRecord( i->info ) )
        continue;

      if ( i->dmark )     /* CANN 10-3 */
        i->dmark = FALSE; /* CANN 10-3 */

      if ( i->cm == -1 ) {
        i->cm = 0;
        rmcmcnt++;
        }

      if ( i->pm > 0 ) {
        i->pm = 0;
        rmpmcnt++;
        }

      i->pl = 0;

      if ( i->sr > 0 )
        rmsrcnt++;

      i->sr = 0;

      /* REMOVE GROUND REFERENCES! */
      if ( !IsConst( i ) )
        if ( IsGraph( i->dst ) )
          if ( i->iport == 0 ) {
            UnlinkImport( i );
            UnlinkExport( i );
            continue;
            }
  
      if ( IsConst( i ) )
        continue;

      if ( !IsNoOp( i->src ) )
        continue;
  
      /* INSERT CODE TO REMOVE i->src (THE NOOP) and move its exports */
      nop = i->src;
  
      for ( ee = i->src->exp; ee != NULL; ee = se ) {
        se = ee->esucc;
        UnlinkExport( ee );
        ee->eport = nop->imp->eport;
        LinkExport( nop->imp->src, ee );
        }
  
      UnlinkExport( nop->imp );
      UnlinkNode( nop );
  
      if ( nop->imp->rmark1 == rMARK )
        rmcnoop++;
  
      if ( nop->imp->rmark1 == NOMARK )
        rmnoop++;
      }
  
    switch ( n->type ) {
      case IFRBuild:
        if ( !IsBRecord( n->exp->info ) )
          break;
  
        n->type = IFBRBuild;
        brcnt++;
        break;
  
      case IFRReplace:
        if ( !IsBRecord( n->imp->info ) )
          break;
  
        n->type = IFBRReplace;
        brcnt++;
        break;
  
      case IFRElements:
        if ( !IsBRecord( n->imp->info ) )
          break;
  
        n->type = IFBRElements;
        brcnt++;
        break;
  
      case IFSelect:
        if ( ok ) {
          n->S_CONS->xmark = TRUE;
          n->S_CONS->xmark = TRUE;
          }

        break;

      case IFCall:
        if ( ok )
          DriveXmark( FindFunction( n->imp->CoNsT ) );

        break;

      default:
        break;
      }
  
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
	/* NEW CANN 2/92 */
        FixBRecordOperators( sg, ok || 
				 ((IsForall(n)&&(!(n->smark))) || IsLoop(n)) ); 
	}
    }
}

/**************************************************************************/
/* GLOBAL **************     WritePrebuildInfo     ************************/
/**************************************************************************/
/* PURPOSE: WRITE PREBUILD OPTIMIZATION FEEDBACK TO stderr.               */
/**************************************************************************/

void WritePrebuildInfo()
{
/*  FPRINTF( infoptr2, "\n **** PREBUILD OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr, " Basic Record Node Conversions:        %d\n", brcnt  );
  FPRINTF( infoptr, " Ragged Array Allocates:               %d\n", rcnt1  );
  FPRINTF( infoptr, " Ragged Invariants:                    %d\n", rinv   );

  FPRINTF( infoptr, " External Pointer Swap Shares [case1,2] %d\n", ss1cnt);

  FPRINTF( infoptr, " BIP AT-Node count:                    %d\n", bipats );
  FPRINTF( infoptr, " BIP Root Nodes:                       %d\n", biprts );
  FPRINTF( infoptr, " BIP Node Movements:                   %d\n", bipmvc );
  FPRINTF( infoptr, " Prebuild Dope Optimizations SINGULAR: %d\n", bipone );
  FPRINTF( infoptr, " Prebuild Dope Optimizations MULTIPLE: %d\n", bipmult);

  FPRINTF( infoptr, " Prebuild Dope Optimizations REGULAR:  %d\n\n", bipreg ); */

  FPRINTF( infoptr2, "\n **** MEMALLOCDVI OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr2, "%s", mem );
  FPRINTF( infoptr2, " Combined And Hoisted MemAllocDVIs:    %d of %d\n\n", mchcnt, tmchcnt );

  FPRINTF( infoptr2, "\n **** POINTER SWAPS\n\n" );
  FPRINTF( infoptr2, "%s", sptr );
  FPRINTF( infoptr2, " Pointer Swap Candidates:              %d\n", psccnt );

  FPRINTF( infoptr2, " Pointer Swap Successes:               %d\n\n", 
           case1 + case2 + case3 + case4a + case4b + case5 + case6 + case7 );

  FPRINTF( infoptr2, " Pointer Swap Successes by Dim[%d,%d,%d,%d,%d,other=%d]\n",
           psdcnt[1],psdcnt[2],psdcnt[3],psdcnt[4],psdcnt[5],psdcnt[0]       );

  FPRINTF( infoptr2, " Pointer Swap Case 1:                  %d\n", case1  );
  FPRINTF( infoptr2, " Pointer Swap Case 2:                  %d\n", case2  );
  FPRINTF( infoptr2, " Pointer Swap Case 3:                  %d\n", case3  );
  FPRINTF( infoptr2, " Pointer Swap Case 4a:                 %d\n", case4a );
  FPRINTF( infoptr2, " Pointer Swap Case 4b:                 %d\n", case4b );
  FPRINTF( infoptr2, " Pointer Swap Case 5:                  %d\n", case5  );
  FPRINTF( infoptr2, " Pointer Swap Case 6:                  %d\n", case6  );
  FPRINTF( infoptr2, " Pointer Swap Case 7:                  %d\n", case7  );
}


/**************************************************************************/
/* LOCAL  **************      ZeroRCPragmas        ************************/
/**************************************************************************/
/* PURPOSE: ZERO-OUT THE REFERENCE COUNT PRAGMAS OF THE EXPORTS TO NODE n */
/*          WITH EXPORT PORT NUMBER eport.                                */
/**************************************************************************/

static void ZeroRCPragmas( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    e->pm = 0;
    e->cm = 0;
    e->sr = 0;
    e->dmark = FALSE;
    }
}


/**************************************************************************/
/* GLOBAL **************      ModifyBIP            ************************/
/**************************************************************************/
/* PURPOSE: MODIFY THE BIP ROOTED BY EDGE e. THE TRAVERSAL MUST BE THE    */
/*          SAME AS THAT DONE BY GetBIPType.                              */
/**************************************************************************/

static void ModifyBIP( e )
PEDGE e;
{
  register PNODE src;
  register PNODE rt;
  register PEDGE ee;

  if ( e == NULL )
    return;

  if ( !(e->pmark) ) {
    if ( e->src->type == IFNoOp )
      Error2( "ModifyBIP", "ILLEGAL NoOp ENCOUNTERED!!!" );

    return;
    }

  src = e->src;

  switch ( src->type ) {
    case IFSGraph:
      ModifyRedAT( e );
      break;

    case IFNoOp:
      src->type = IFOptNoOp;
      ZeroRCPragmas( src, 1 );
      ModifyBIP( src->imp );
      break;

    case IFAAddHAT:
      if ( src->exp->esucc != NULL )
        src->type = IFAAddHATDVI;
      else
        src->nmark = TRUE;

      ZeroRCPragmas( src, 1 );
      ModifyBIP( src->imp );
      break;

    case IFAAddLAT:
      if ( src->exp->esucc != NULL )
        src->type = IFAAddLATDVI;
      else
        src->nmark = TRUE;

      ZeroRCPragmas( src, 1 );
      ModifyBIP( src->imp );
      break;

    case IFACatenateAT:
      if ( src->exp->esucc != NULL )
        src->type = IFACatenateATDVI;
      else
        src->nmark = TRUE;

      ZeroRCPragmas( src, 1 );
      ModifyBIP( src->imp );
      ModifyBIP( src->imp->isucc );
      break;

    case IFABuildAT:
      ZeroRCPragmas( src, 1 );

      if ( src->exp->esucc != NULL )
        src->type = IFABuildATDVI;
      else
        src->nmark = TRUE;

      break;

    case IFLoopA:
    case IFLoopB:
      if ( (ee = FindImport( src->L_RET, e->eport )) == NULL )
        Error2( "ModifyBIP", "Loop[AB] FindImport FAILED!!!" );

      goto DoIt;

    case IFForall:
      if ( (ee = FindImport( src->F_RET, e->eport )) == NULL )
        Error2( "ModifyBIP", "Forall FindImport FAILED!!!" );

DoIt:
      rt = ee->src;

      ZeroRCPragmas( src, e->eport );
      ZeroRCPragmas( rt, 1 );

      switch ( rt->type ) {
        case IFAGatherAT:
          if ( UsageCount( src, e->eport ) > 1 )
            rt->type = IFAGatherATDVI;
          else
            rt->nmark = TRUE;

          break;

        default:
          ModifyRedAT( rt->imp->isucc->isucc );

          if ( UsageCount( src, e->eport ) > 1 )
            rt->type = IFReduceATDVI;
          else
            rt->nmark = TRUE;

          break;
        }

      break;

    default:
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************       ModifyRedAT         ************************/
/**************************************************************************/
/* PURPOSE: MODIFY THE BIP OF REDUCE CATENATE VALUE v.                    */
/**************************************************************************/

static void ModifyRedAT( v )
PEDGE v;
{
  register PEDGE ge;
  register PEDGE e;

  for ( ge = NULL, e = v->src->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != v->eport )
      continue;

    if ( e->iport == 0 )
      ge = e;
    }

  if ( ge == NULL )
    Error2( "ModifyRedAT", "GROUND NOT FOUND!!!" );

  ModifyBIP( FindImport( v->src->G_DAD->F_BODY, v->eport ) );
  v->cm = 0;
  v->dmark = FALSE;

  /* UNLINK THE GROUND EDGE!!! */
  UnlinkExport( ge );
  UnlinkImport( ge );
}


/**************************************************************************/
/* LOCAL  **************      GetRedATType         ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE BIP TYPE OF THE AT-SUBGRAPH ROOTED BY REDUCE       */
/*          CATENATE VALUE v.                                             */
/**************************************************************************/

static int GetRedATType( v )
PEDGE v;
{
  register PNODE sg;
  register PEDGE e2;
  register PEDGE i;
  register PEDGE e;

  sg = v->src;

  if ( IsLoop( sg->G_DAD ) )
    return( BIP_REG );

  if ( v->pm > 0 || (!(v->pmark)) )
    return( BIP_REG );

  if ( UsageCount( sg, v->eport ) != 2 )
    return( BIP_REG );

  for ( e2 = NULL, i = sg->imp; i != NULL; i = i->isucc ) {
    if ( i->eport != v->eport )
      continue;

    if ( i->iport != 0 )
      continue;

    e2 = i;
    }

  if ( e2 == NULL )
    return( BIP_REG );

  if ( (e = FindImport( sg->G_DAD->F_BODY, v->eport )) == NULL )
    return( BIP_REG );

  if ( (!(e->pmark)) || e->cm == -1 || e->pm > 0 || e->sr > 1 )
    return( BIP_REG );

  if ( e->dmark )
    return( BIP_REG );

  if ( UsageCount( e->src, e->eport ) > 1 )
    return( BIP_REG );

  switch ( GetBIPType( e ) ) {
    case BIP_ONE:
      return( BIP_ONE );

    case BIP_MULT:
    case BIP_REG:
      return( BIP_REG );

    default:
      Error2( "GetRedATType", "ILLEGAL GetBIPType return VALUE" );
    }

  return( BIP_REG );
}


/**************************************************************************/
/* GLOBAL **************       GetBIPType          ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE BIP TYPE OF AT-SUBGRAPH ROOTED BY EDGE e.          */
/**************************************************************************/

static int GetBIPType( e )
PEDGE e;
{
  register PNODE src;
  register int   t1;
  register PEDGE ee;

  if ( e == NULL ) {
    return( BIP_ONE );
    }

  if ( !(e->pmark) ) {
    if ( e->src->type == IFNoOp )
      Error2( "GetBIPType", "ILLEGAL NoOp ENCOUNTERED!!!" );

    return( BIP_ONE );
    }

  src = e->src;

  switch ( src->type ) {
    case IFSGraph:
      switch ( e->dst->type ) {
        case IFReduceAT:
        case IFRedTreeAT:
        case IFRedLeftAT:
        case IFRedRightAT:
          return( GetRedATType( e ) );

        default:
          return( BIP_REG );
        }

    case IFNoOp:
      if ( src->exp->esucc != NULL )
        break;

      if ( src->imp->rmark1 != RMARK )
        break;

      if ( src->imp->dmark )
        break;

      if ( src->imp->cm == -1 )
        break;

      if ( src->imp->pm > 0 || src->imp->sr > 1 )
        break;

      t1 = GetBIPType( src->imp );

      if ( t1 == BIP_REG )
        return( BIP_REG );

      if ( UsageCount( src->imp->src, src->imp->eport ) > 1 )
        return( BIP_MULT );

      return( t1 );

    case IFAAddHAT:
    case IFAAddLAT:
      return( GetBIPType( src->imp ) );

    case IFACatenateAT:
      /* TRY LEFT AND THEN RIGHT */
      if ( src->imp->pmark ) {
        if ( GetBIPType( src->imp->isucc ) != BIP_ONE )
          break;

        return( GetBIPType( src->imp ) );
        }

      if ( GetBIPType( src->imp ) != BIP_ONE )
        break;

      return( GetBIPType( src->imp->isucc ) );

    case IFABuildAT:
      return( BIP_ONE );

    case IFLoopA:
    case IFLoopB:
      if ( (ee = FindImport( src->L_RET, e->eport )) == NULL )
        Error2( "GetBIPType", "Loop[AB] FindImport FAILED!!!" );

      goto DoIt;

    case IFForall:
      if ( (ee = FindImport( src->F_RET, e->eport )) == NULL )
        Error2( "GetBIPType", "Forall FindImport FAILED!!!" );

DoIt:
      switch ( ee->src->type ) {
        case IFAGatherAT:
          if ( ee->sr != 1 )
            return( BIP_REG );

          return( BIP_ONE );

        case IFReduceAT:
        case IFRedLeftAT:
        case IFRedRightAT:
        case IFRedTreeAT:
          if ( ee->sr != 1 )
            return( BIP_REG );

          return( GetRedATType( ee->src->imp->isucc->isucc ) );

        default:
	  ;
        }
      return( BIP_REG );

    default:
      break;
    }

  return( BIP_REG );
}


/**************************************************************************/
/* LOCAL  **************       ProcessBIP          ************************/
/**************************************************************************/
/* PURPOSE: PROCESS BIP ROOT NODE n WITH INPUT AGGREGATE e1 AND e2 AND    */
/*          MEMORY ALLOCATION EXPORT EDGE m AND ROOT RESULT LOWER BOUND   */
/*          EDGE lb AND DVI NODE type DVItype and DV NODE TYPE DVtype.    */
/**************************************************************************/

static void ProcessBIP( n, e1, e2, m, lb, DVItype, DVtype )
PNODE n;
PEDGE e1;
PEDGE e2;
PEDGE m;
PEDGE lb;
int   DVItype;
int   DVtype;
{
  register int   ok;
  register int   t1;
  register int   t2;
  register PEDGE e;

  if ( n->cmark ) {
    bipreg++;
    return;
    }

  if ( m == NULL )
    Error2( "ProcessBIP", "NULL EDGE ENCOUNTERED (m)" );
  if ( lb == NULL )
    Error2( "ProcessBIP", "NULL EDGE ENCOUNTERED (lb)" );

  if ( m->src->type != IFMemAlloc )
    Error2( "ProcessBIP", "ILLEGAL BUFFER SOURCE" );

  t1 = GetBIPType( e1 );
  t2 = GetBIPType( e2 );

  if ( t1 == BIP_REG || t2 == BIP_REG )
    t1 = BIP_REG;
  else if ( t2 == BIP_MULT && t1 == BIP_MULT )
    t1 = BIP_REG;
  else if ( t2 == BIP_MULT )
    t1 = BIP_MULT;
  /* ELSE t1 GETS t1 */

  switch ( t1 ) {
    case BIP_ONE:
      ok = TRUE;

      if ( !IsConst( lb ) )
        if ( lb->src->label > m->src->label )
          ok = FALSE;

      if ( ok ) {
        n->type  = DVtype;
        m->src->type = IFMemAllocDVI;
        CopyEdgeAndLink( lb, m->src, 2 );

        for ( e = m->src->exp; e != NULL; e = e->esucc )
          e->sr = n->exp->sr;

        for ( e = n->exp; e != NULL; e = e->esucc )
          e->sr = 0;
      } else {
        n->type = DVItype;
        m->src->type = IFMemAllocDV;
        }

      ModifyBIP( e1 );
      ModifyBIP( e2 );
      bipone++;
      break;

    case BIP_MULT:
      n->type = DVItype;
      ModifyBIP( e1 );
      ModifyBIP( e2 );
      m->src->type = IFMemAllocDV;

      bipmult++;
      break;

    case BIP_REG:
      bipreg++;
      break;

    default:
      Error2( "ProcessBIP", "ILLEGAL GetBIPType RETURN VALUE" );
      break;
      }
}


/**************************************************************************/
/* LOCAL  **************    FindNearestDecendent   ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NEAREST DECENDENT OF EXPORT eport OF NODE n.       */
/**************************************************************************/

static PNODE FindNearestDecendent( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;
  register PEDGE ee;

  for ( ee = NULL, e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( e->dst->label <= 0 )
      continue;

    if ( ee == NULL ) {
      ee = e;
      continue;
      }

    if ( e->dst->label < ee->dst->label )
      ee = e;
    }

  if ( ee == NULL )
    return( NULL );

  return( ee->dst );
}


/**************************************************************************/
/* LOCAL  **************          IsRagged         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE i IS A FUNCTION OF A LOOP CARRIED VALUE.  */
/*          I's TYPE MUST BE INTEGER.                                     */
/**************************************************************************/

static int IsRagged( i )
PEDGE i;
{
  register PNODE src;
  register PEDGE ii;

  if ( IsConst( i ) )
    return( FALSE );

  if ( !IsInteger( i->info ) )
    return( FALSE );

  src = i->src;

  if ( IsSGraph( src ) ) {
    ii = FindImport( src->G_DAD, i->eport );

    switch ( src->G_DAD->type ) {
      case IFLoopA:
      case IFLoopB:
        if ( ii == NULL ) {
          return( TRUE );
	  }

      case IFForall:
        if ( ii == NULL )
          return( FALSE ); /* STOP HERE AND ASSUME NOT RAGGED */

      default:
        return( IsRagged( ii ) );
      }
    }

  if ( IsCompound( src ) )
    return( FALSE );  /* ERROR ON THE SAFE SIDE */

  if ( IsSimple( src ) )
    for ( ii = src->imp; ii != NULL; ii = ii->isucc ) {
      if ( IsRagged( ii ) ) {
        return( TRUE );
	}
      }

  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************   MoveRaggedInvariants    ************************/
/**************************************************************************/
/* PURPOSE: REMOVE RAGGED INVARIANTS FROM SUBGRAPH g.                     */
/**************************************************************************/

static void MoveRaggedInvariants( g )
PNODE g;
{
  register PNODE n;
  register PNODE sn;
  register PEDGE i;
  register PEDGE e;
  register PEDGE ee;
  register int   port;

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( n->type != IFRagged )
      continue;

    if ( IsConst( n->imp ) )
      continue;

    if ( !IsSGraph( n->imp->src ) )
      continue;

    if ( (i = FindImport( g->G_DAD, n->imp->eport )) == NULL )
      continue;

    if ( IsConst( i ) )
      continue;

    /* SHOULD NEVER BE THE CASE, AT LEAST NOT YET!!! */
    if ( n->exp->esucc != NULL )
      continue;

    /* OK, n IS INVARIANT, SO MOVE IT!!! */

    UnlinkNode( n );
    LinkNode( g->G_DAD->npred, n );

    port = ++maxint;
    e = n->exp;

    /* ATTACH n's OLD EXPORT TO g USING port */
    UnlinkExport( e );
    e->eport = port;
    LinkExport( g, e );

    /* LINK n TO g->G_DAD USING EXPORT PORT port */
    ee = EdgeAlloc( n, 1, g->G_DAD, port );
    ee->info = e->info;
    LinkExport( n, ee );
    LinkImport( g->G_DAD, ee );

    /* CHANGE n'S IMPORT TO REFERENCE i */
    UnlinkExport( n->imp );
    n->imp->eport = i->eport;
    LinkExport( i->src, n->imp );

    rinv++;
    }
}


/**************************************************************************/
/* LOCAL  **************  IdentifyRaggedMemAllocs  ************************/
/**************************************************************************/
/* PURPOSE: OPTIMIZE THE BUILD-IN-PLACE SUBGRAPHS IN GRAPH g.             */
/**************************************************************************/

static void IdentifyRaggedMemAllocs( g )
PNODE g;
{
  register PNODE n;
  register PNODE raggednode;
  register PNODE sg;
  register PEDGE e;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        IdentifyRaggedMemAllocs( sg );

      switch ( n->type ) {
	case IFLoopA:
	case IFLoopB:
	  MoveRaggedInvariants( n->L_BODY );
	  break;

	case IFForall:
	  MoveRaggedInvariants( n->F_BODY );
	  break;

	case IFSelect:
	  MoveRaggedInvariants( n->S_ALT );
	  MoveRaggedInvariants( n->S_CONS );
	  break;

	default:
	  break;
        }

      continue;
      }

    if ( !IsAlloc( n ) )
      continue;

    if ( IsRagged( n->imp ) ) {
      raggednode = NodeAlloc( ++maxint, IFRagged );
      LinkNode( n->npred, raggednode );

      e = EdgeAlloc( n->imp->src, n->imp->eport, raggednode, 1 );
      e->info = n->imp->info;
      LinkExport( n->imp->src, e );
      LinkImport( raggednode, e );

      e = EdgeAlloc( raggednode, 1, n, 3 );
      e->info = n->imp->info;
      LinkExport( raggednode, e );
      LinkImport( n, e );

      n->wmark = TRUE;
      rcnt1++;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************      OptimizeBIPs         ************************/
/**************************************************************************/
/* PURPOSE: OPTIMIZE THE BUILD-IN-PLACE SUBGRAPHS IN GRAPH g.             */
/**************************************************************************/

void OptimizeBIPs( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE b;
  register int   l;
  register PEDGE v;
  register PNODE pn;
  register PNODE nd;
  register PNODE ln;
  register PNODE nn;

  /* RELABEL THE NODES! */
  for ( ln = g, l = 0, n = g; n != NULL; n = n->nsucc ) {
    n->label = l++;

    if ( n->nsucc == NULL )
      ln = n;
    }

  if ( !bipmv )
    goto MoveOn;

  /* MOVE MemAlloc AND ShiftBuffer NODES TOWARD THEIR SOURCES TO BETTER THE */
  /* CHANCE FOR MemAllocDVI CREATIONS                                       */
  for ( n = ln; n != g; n = pn ) {
    pn = n->npred;

    switch ( n->type ) {
      case IFShiftBuffer:
      case IFMemAlloc:
        if ( (nd = FindNearestDecendent( n, 1 )) == NULL )
          break;

        bipmvc++;
        UnlinkNode( n );
        LinkNode( nd->npred, n );

        for ( l = pn->label + 1, nn = pn->nsucc; nn != nd; nn = nn->nsucc )
          nn->label = l++;

        break;

      default:
        break;
      }
    }

MoveOn:

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        OptimizeBIPs( sg );

      continue;
      }

    switch ( n->type ) {
      case IFAAddHAT:
        if ( !MyIsPMarked( n, 1 ) )
          ProcessBIP( n, n->imp, NULL_EDGE,
                      n->imp->isucc->isucc, 
                      n->imp->isucc->isucc->isucc->isucc, 
                      IFAAddHATDVI, IFAAddHATDV );

        break;

      case IFAAddLAT:
        if ( !MyIsPMarked( n, 1 ) )
          ProcessBIP( n, n->imp, NULL_EDGE,
                      n->imp->isucc->isucc, 
                      n->imp->isucc->isucc->isucc->isucc, 
                      IFAAddLATDVI, IFAAddLATDV );
        break;

      case IFACatenateAT:
        if ( !MyIsPMarked( n, 1 ) ) {
          ProcessBIP( n, n->imp, n->imp->isucc,
                      n->imp->isucc->isucc,
                      n->imp->isucc->isucc->isucc->isucc,
                      IFACatenateATDVI, IFACatenateATDV );
          }

        break;

      case IFABuildAT:
        if ( !MyIsPMarked( n, 1 ) )
          ProcessBIP( n, NULL_EDGE, NULL_EDGE,
                      FindLastImport( n ), n->imp,
                      IFABuildATDVI, IFABuildATDV );
        break;

      case IFReduceAT:
      case IFRedLeftAT:
      case IFRedRightAT:
      case IFRedTreeAT:
        sg = n->exp->dst;
        v  = n->imp->isucc->isucc;

        b = FindImport( sg->G_DAD, v->isucc->eport ); 

        if ( b == NULL )
          Error2( "OptimizeBIPs", "ReduceAT FindImport FOR BUFFER FAILED" );

        if ( !MyIsPMarked( sg->G_DAD, n->exp->iport ) ) {
          ProcessBIP( n, v, NULL_EDGE, b,
                      (IsConst(n->imp->isucc))? n->imp->isucc :
                         FindImport( sg->G_DAD, n->imp->isucc->eport ),
                      IFReduceATDVI, IFReduceATDV );
          }

        break;

      case IFAGatherAT:
        if ( !MyIsPMarked( n->exp->dst->G_DAD, n->exp->iport ) )
          ProcessBIP( n, NULL_EDGE, NULL_EDGE,
                      FindImport( n->exp->dst->G_DAD, 
                                  n->imp->isucc->isucc->eport ), 
                      (IsConst(n->imp))? n->imp :
                         FindImport( n->exp->dst->G_DAD, n->imp->eport ),
                      IFAGatherATDVI, IFAGatherATDV );
        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************  CombineAndHoistMemAllocs ************************/
/**************************************************************************/
/* PURPOSE: COMBINE AND HOIST COMMON IFMemAllocDVI NODES IN GRAPH g WHICH */
/*          RESIDE IN APPOSING SELECT ARMS AND CLEAN THE TRANSFORMATIONS. */
/**************************************************************************/

static void CombineAndHoistMemAllocs( g )
PNODE g;
{
  register PNODE n;
  register PNODE an;
  register PNODE cn;
  register PNODE cns;
  register PNODE sg;
  register PEDGE i1;
  register PEDGE i2;
  register PEDGE e;
  register int   port;
  register PEDGE se;
  register PINFO binfo;
  register int   sr;
  register PEDGE e1;
  register PEDGE e2;

  for ( n = g->nsucc; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        CombineAndHoistMemAllocs( sg );
    }

  for ( n = g->nsucc; n != NULL; n = n->nsucc ) {
      /* DO SOME GRAPH CLEANUP: IN SOME SPECIAL CASES WE MUST  */
      /* DISABLE CONCURRENTIZATION!!! AS Dope VECTORS REQUIRED */
      /* AND Phys REFERENCE COUNTS WILL BE MODIFIED, REQUIRING */
      /* MUTUAL EXCLUSIONS.  THE MACROS ARE NOT SET UP TO DO   */
      /* THIS IN A CRITICAL SECTION.                           */

      switch ( n->type ) {
        case IFReduceAT:
        case IFRedLeftAT:
        case IFRedRightAT:
        case IFRedTreeAT:
          sg = n->exp->dst;

          if ( !(n->nmark) ) {
            if ( sg->G_DAD->vmark ) {
              sg->G_DAD->vmark = FALSE;
              rmvmark++;
              }

            if ( sg->G_DAD->smark ) {
              sg->G_DAD->smark = FALSE;
              rmsmark++;
              }
            }

          break;

        case IFReduceATDVI:
          sg = n->exp->dst;

          if ( sg->G_DAD->vmark ) {
            sg->G_DAD->vmark = FALSE;
            rmvmark++;
            }

          if ( sg->G_DAD->smark ) {
            sg->G_DAD->smark = FALSE;
            rmsmark++;
            }

          break;

        case IFReduceATDV:
          break;

        default:
          break;
        }
      /* END OF PARALLELIZATION CLEANUP; IF !bip, SEE if2opt.c FOR SIMILAR */
      /* CLEANUP. */

      if ( !IsSelect( n ) )
        continue;

      for ( cn = n->S_CONS; cn != NULL; cn = cns ) {
        cns = cn->nsucc;

        if ( cn->type != IFMemAllocDVI )
          continue;

	++tmchcnt;

        /* 1-DIM ONLY */
        if ( !IsBasic( cn->exp->info->A_ELEM->A_ELEM ) )
          continue;

        /* INVARIANT? */
        if ( !IsConst( cn->imp ) )
          if ( !IsSGraph( cn->imp->src ) )
            continue;
        if ( !IsConst( cn->imp->isucc ) )
          if ( !IsSGraph( cn->imp->isucc->src ) )
            continue;

        if ( cn->imp->isucc->isucc != NULL )
          Error2( "OptimizeBIPs", "ILLEGAL IFMemAllocDVI NODE" );

        for ( an = n->S_ALT; an != NULL; an = an->nsucc ) {
          if ( an->type != IFMemAllocDVI )
            continue;

	++tmchcnt;

          if ( an->exp->sr != cn->exp->sr )
            continue;

          if ( IsConst( an->imp ) ^ IsConst( cn->imp ) )
            continue;

          if ( IsConst( an->imp->isucc ) ^ IsConst( cn->imp->isucc ) )
            continue;

          /* MAKE SURE cn AND an ARE EQUIVALENT */
          if ( IsConst( an->imp ) ) {
            if ( strcmp( an->imp->CoNsT, cn->imp->CoNsT ) != 0 )
              continue;
          } else {
            if ( !IsSGraph( an->imp->src ) )
              continue;

            if ( an->imp->eport != cn->imp->eport )
              continue;
            }

          if ( IsConst( an->imp->isucc ) ) {
            if ( strcmp(an->imp->isucc->CoNsT,cn->imp->isucc->CoNsT) != 0 )
              continue;
          } else {
            if ( !IsSGraph( an->imp->isucc->src ) )
              continue;

            if ( an->imp->isucc->eport != cn->imp->isucc->eport )
              continue;
            }

          /* THEY ARE EQUIVALENT SO COMBINE AND HOIST THEM ONE LEVEL */
#ifdef MYI
          SPRINTF(mem, 
              "%s Combining MemAllocDVI %d at line %d, funct %s, file %s\n",
              mem, n->ID, n->line, n->funct, n->file);	
          SPRINTF(mem, 
              "%s with MemAllocDVI %d at line %d, funct %s, file %s\n\n",
              mem, an->ID, an->line, an->funct, an->file);	
#endif
          port  = ++maxint;
          binfo = an->exp->info;
          sr    = an->exp->sr;

          /* LINK ALTERNATE EXPORTS TO ALT SUBGRAPH */
          for ( e = an->exp; e != NULL; e = se ) {
            se = e->esucc;

            UnlinkExport( e );
            e->eport = port;
            e->sr    = 0;
            LinkExport( n->S_ALT, e );
            }

          /* LINK CONSEQUENT EXPORTS TO CONS SUBGRAPH */
          for ( e = cn->exp; e != NULL; e = se ) {
            se = e->esucc;

            UnlinkExport( e );
            e->eport = port;
            e->sr    = 0;
            LinkExport( n->S_CONS, e );
            }

          /* THROW an AWAY */
          UnlinkNode( an );
          UnlinkExport( an->imp->isucc );
          UnlinkExport( an->imp );
          /* free( an ); */

          /* MOVE CN OUT */
          UnlinkNode( cn );
          LinkNode( n->npred, cn );

          e = EdgeAlloc( cn, 1, n, port );
          e->info = binfo;
          e->sr   = sr;
          LinkExport( cn, e );
          LinkImport( n, e );

          i1 = cn->imp;
          i2 = i1->isucc;

          /* RESET cn'S IMPORTS */
          e1 = NULL;
          e2 = NULL;

          if ( !IsConst( i1 ) ) {
            UnlinkExport( i1 );
            UnlinkImport( i1 );

            if ( (e = FindImport( n, i1->eport )) == NULL )
              Error2( "CombineAndHoistMemAllocs", "FindImport FAILED (A)" );

            e1 = e;
            CopyEdgeAndLink( e, cn, i1->iport );
            }

          if ( !IsConst( i2 ) ) {
            UnlinkExport( i2 );
            UnlinkImport( i2 );

            if ( (e = FindImport( n, i2->eport )) == NULL )
              Error2( "CombineAndHoistMemAllocs", "FindImport FAILED (B)" );

            e2 = e;
            CopyEdgeAndLink( e, cn, i2->iport );
            }

          /* CLEANUP */
          if ( e1 != NULL )
            if ( (!IsExport( n->S_TEST, e1->iport )) &&
                 (!IsExport( n->S_CONS, e1->iport )) &&
                 (!IsExport( n->S_ALT,  e1->iport ))  ) {
              if ( IsConst( e1 ) )
                UnlinkImport( e1 );
              else {
                if ( e1->src->exp->esucc != NULL ) {
                  UnlinkExport( e1 );
                  UnlinkImport( e1 );
                  }
                }
              }

          if ( e2 != NULL )
            if ( (!IsExport( n->S_TEST, e2->iport )) &&
                 (!IsExport( n->S_CONS, e2->iport )) &&
                 (!IsExport( n->S_ALT,  e2->iport ))  ) {
              if ( IsConst( e2 ) )
                UnlinkImport( e2 );
              else {
                if ( e2->src->exp->esucc != NULL ) {
                  UnlinkExport( e2 );
                  UnlinkImport( e2 );
                  }
                }
              }

          mchcnt++;
          break;
          }
        }
    }
}


/**************************************************************************/
/* GLOBAL **************     If2Prebuild0          ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ALL INFO ENTRIES IN THE SYMBOL TABLE THAT ARE RECORDS  */
/*          OF SCALARS TO TYPE BRECORD (BASIC RECORD).                    */
/**************************************************************************/

void If2Prebuild0()
{
  register PINFO ii;
  register PINFO i;
  register int   c;
  register PNODE f;
  register int   ok;

  /* if ( max_dims <= 0 ) goto SkipBRecordStuff; */

  for ( i = ihead; i != NULL; i = i->next ) {
    if ( nobrec )
      continue;

    if ( i->type != IF_RECORD )
      continue;

    for ( c = 0, ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT, c++ ) {
      if ( !IsBasic( ii->L_SUB ) )
        goto Abort;

      if ( IsBRecord( ii->L_SUB ) )
        goto Abort;
      }

    if ( c <= MAX_BRECORD )
      i->type = IF_BRECORD;

Abort:
    continue;
    }

/* SkipBRecordStuff: */

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    /* NOT RECURSIVE and NOT CALLED and ENTRY POINT and SEQUENTIAL */
    /* ok = (!(f->bmark)) && (!(f->Cmark)) && f->emark && (!(f->Pmark)); */
    /* CANN NEW 2/92 */
    ok = (!(f->bmark)) && (!(f->Cmark)) && IsEntry(f) && (!(f->Pmark));

    if ( IsXGraph( f ) ) {
      if ( !standalone && bindtosisal && ok )
        f->xmark = TRUE;
      else 
        ok = FALSE;
      }

    FixBRecordOperators( f, ok );
    }
}



/**************************************************************************/
/* GLOBAL **************       IF2Prebuild1        ************************/
/**************************************************************************/
/* PURPOSE: APPLY AGGREGATE PREBUILD OPTIMIZATIONS PRIOR TO NODE YANKING. */
/**************************************************************************/

void If2Prebuild1()
{
  register PNODE f;

  pmode = PREBUILD1;

  if ( max_dims > MAX_DIMS )
    max_dims = MAX_DIMS;
  else if ( max_dims < 0 )
    max_dims = 0;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    if ( bip ) {
      OptimizeBIPs( f );
      CombineAndHoistMemAllocs( f );
      }

    if ( rag )
      IdentifyRaggedMemAllocs( f );

    if ( max_dims > 0 )
      PointerSwap( f, f );
    }
}


/**************************************************************************/
/* GLOBAL **************       IF2Prebuild2        ************************/
/**************************************************************************/
/* PURPOSE: APPLY AGGREGATE PREBUILD OPTIMIZATIONS ON YANKED GRAPHS.      */
/**************************************************************************/

void If2Prebuild2()
{
  register PNODE f;

  pmode = PREBUILD2;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    switch ( f->type ) {
      case IFLPGraph:
	if ( max_dims > 0 )
          PointerSwap( f, f );

      default:
        if ( !IsIGraph( f ) && share && max_dims > 0 )
	  ShareStorage( f );

        break;
      }
    }
}
