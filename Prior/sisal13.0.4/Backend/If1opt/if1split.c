/* if1split.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:37  miller
 * Initial RCS Version by Cann
 * */

/* static PEDGE k; */

#include "world.h"


static PEDGE c;                              /* FORALL NODE'S CONTROL ROD */
static PEDGE lo;                             /* FORALL NODE'S LOWER BOUND */
static PEDGE hi;                             /* FORALL NODE'S UPPER BOUND */


static int   lcnt  = 0;                      /* COUNT OF SPLIT_LOW SPLITS       */
static int  nlcnt  = 0;                      /* COUNT OF SPLIT_NOT_LOW SPLITS   */
static int   hcnt  = 0;                      /* COUNT OF SPLIT_HIGH SPLITS      */
static int   Tcnt  = 0;                      /* COUNT OF loops      */
static int  nhcnt  = 0;                      /* COUNT OF SPLIT_NOT_HIGH SPLITS  */
static int  rfail  = 0;                      /* COUNT RETURN GRAPH ABORTS */
DYNDECLARE(printinfo,printbuf,printlen,printcount,char,2000);

#define SPLIT_BAD       0                    /* FORALL SPLIT TYPES */
#define SPLIT_LOW       1
#define SPLIT_HIGH      2
#define SPLIT_NOT_LOW   3
#define SPLIT_NOT_HIGH  4

/**************************************************************************/
/* LOCAL  **************       GetOperand          ************************/
/**************************************************************************/
/* PURPOSE: FIND THE SOURCE OF EDGE e IN FORALL NODE f. THIS ROUTINE IS   */
/*          SPECIFIC TO LOOP SPLITTING, AND SHOULD NOT BE USED OUTSIDE    */ 
/*          THIS CONTEXT.                                                 */
/**************************************************************************/

static PEDGE GetOperand( f, e )
PEDGE e;
PNODE f;
{
  register PEDGE i1;

  if ( IsConst( e ) )
    return( e );

  if ( !IsSGraph( e->src ) )
    return( NULL );

  i1 = FindImport( f, e->eport );

  /* IS e A REFERENCE TO f'S CONTROL ROD? */
  if ( i1 == NULL ) {
    i1 = FindImport( f->F_GEN, e->eport );

    if ( i1 == NULL )
      return( NULL );
    }
  
  return( i1 );
}


/**************************************************************************/
/* LOCAL  **************         SplitType         ************************/
/**************************************************************************/
/* PURPOSE: IDENTIFY THE SPLIT TYPE OF FORALL NODE f GIVEN NODE n.        */
/**************************************************************************/

static int SplitType( f, n )
PNODE f;
PNODE n;
{
   register PEDGE i1;
   register PEDGE i2;

   switch ( n->type ) {
      case IFNotEqual:
	/* GET THE LEFT OPERAND OF THE EQUAL NODE */
	if ( (i1 = GetOperand( f, n->imp )) == NULL ) 
	  return( SPLIT_BAD );

	/* GET THE RIGHT OPERAND OF THE EQUAL NODE */
	if ( (i2 = GetOperand( f, n->imp->isucc )) == NULL ) 
	  return( SPLIT_BAD );

	/* FOR i in L,H ... */

	if ( i1 == f->F_GEN->imp ) {
	  /* i ~= L */
	  if ( AreValuesEqual( i2, lo ) )
	    return( SPLIT_NOT_LOW );

	  /* i ~= H */
	  if ( AreValuesEqual( i2, hi ) )
	    return( SPLIT_NOT_HIGH );
	  }

	return( SPLIT_BAD );

      case IFEqual:
	/* GET THE LEFT OPERAND OF THE EQUAL NODE */
	if ( (i1 = GetOperand( f, n->imp )) == NULL ) 
	  return( SPLIT_BAD );

	/* GET THE RIGHT OPERAND OF THE EQUAL NODE */
	if ( (i2 = GetOperand( f, n->imp->isucc )) == NULL ) 
	  return( SPLIT_BAD );

	/* FOR i in L,H ... */

	if ( i1 == f->F_GEN->imp ) {
	  /* i = L */
	  if ( AreValuesEqual( i2, lo ) )
	    return( SPLIT_LOW );

	  /* i = H */
	  if ( AreValuesEqual( i2, hi ) )
	    return( SPLIT_HIGH );
	  }

	return( SPLIT_BAD );

      default:
	return( SPLIT_BAD );
      }
}


/**************************************************************************/
/* LOCAL  **************        FixBody            ************************/
/**************************************************************************/
/* PURPOSE: ADJUST THE t CONDITIONALS IN FORALL f TO YIELD v.             */
/**************************************************************************/

static void FixBody( t, f, v )
int    t;
PNODE  f;
char  *v;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE se;
  register int   tt;
  register PNODE sn;

  for ( n = f->F_BODY->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    /* SHORT CIRCUIT FOR FASTER EXECUTION */
    if ( !(n->type == IFEqual || n->type == IFNotEqual) )
      continue;

    if ( (tt = SplitType( f, n )) == SPLIT_BAD )
      continue;

    if ( t != tt )
      continue;

    for ( e = n->exp; e != NULL; e = se ) {
      se = e->esucc;

      UnlinkExport( e );
      e->src   = NULL;
      e->eport = CONST_PORT;
      e->CoNsT = CopyString( v );
      }

    n->exp = NULL;
    OptRemoveDeadNode( n );
    }
}


/* kk = hi or lo, ee = bound edge, rg = range generate, v = value */

/**************************************************************************/
/* LOCAL  **************         FixControl2       ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ee (AN IMPORT TO RangeGenerate NODE rg) TO REFERENCE   */
/*          THE RESULT OF ADDING kk (AN IMPORT VALUE TO rg) TO v.         */
/**************************************************************************/

static void FixControl2( kk, ee, rg, v )
PEDGE   kk;
PEDGE   ee;
PNODE   rg;
char   *v;
{
  register PNODE f;
  register PEDGE e;
  register int   iport;
  register PNODE op;

  f = rg->exp->dst->G_DAD;

  op = NodeAlloc( ++maxint, IFPlus );
  CopyVitals( f, op );
  LinkNode( f->npred, op );

  e = EdgeAlloc( op, 1, f, ++maxint );
  iport = maxint;
  e->info = integer;
  LinkImport( f, e );
  LinkExport( op, e );

  e = CopyEdge( kk, kk->src, op );
  e->iport = 1;
  LinkImport( op, e );
  if ( !IsConst( kk ) )
    LinkExport( kk->src, e ); 

  e = EdgeAlloc( NULL_NODE, CONST_PORT, op, 2 );
  e->info = integer;
  e->CoNsT = CopyString( v );
  LinkImport( op, e );

  if ( IsConst( ee ) ) {
    ee->eport = iport;
    ee->CoNsT = NULL;
  } else {
    UnlinkExport( ee );
    ee->eport = iport;
    }
  
  LinkExport( rg->exp->dst, ee );
}


/* ee = RangeGenerate import, rg = RangeGenerate, mt = type */

/**************************************************************************/
/* LOCAL  **************         FixControl1       ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ee (AN IMPORT TO RangeGenerate NODE rg) TO REFERENCE   */
/*          THE RESULT OF APPLYING mt (MIN OR MAX) TO THE IMPORT VALUES   */
/*          OF rg (GLOBALS lo AND hi).                                    */
/**************************************************************************/

static void FixControl1( ee, rg, mt )
PEDGE   ee;
PNODE   rg;
int     mt;
{
  register PNODE f;
  register PEDGE e;
  register int   iport;
  register PNODE min;

  f = rg->exp->dst->G_DAD;

  min = NodeAlloc( ++maxint, mt );
  CopyVitals( f, min );
  LinkNode( f->npred, min );

  e = EdgeAlloc( min, 1, f, ++maxint );
  iport = maxint;
  e->info = integer;
  LinkImport( f, e );
  LinkExport( min, e );

  e = CopyEdge( lo, lo->src, min );
  e->iport = 1;
  LinkImport( min, e );
  if ( !IsConst( lo ) )
    LinkExport( lo->src, e ); 

  e = CopyEdge( hi, hi->src, min );
  e->iport = 2;
  LinkImport( min, e );
  if ( !IsConst( hi ) )
    LinkExport( hi->src, e ); 

  if ( IsConst( ee ) ) {
    ee->eport = iport;
    ee->CoNsT = NULL;
  } else {
    UnlinkExport( ee );
    ee->eport = iport;
    }

  LinkExport( rg->exp->dst, ee );
}


/**************************************************************************/
/* GLOBAL **************      WriteSplitInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE LOOP SPLITTING INFORMATION TO stderr.                   */
/**************************************************************************/

void WriteSplitInfo()
{
  FPRINTF( infoptr, "\n\n **** LOOP SPLITTING\n\n%s\n", printinfo);

  FPRINTF( infoptr, " Loops Split:              %d of %d\n", 
		lcnt + hcnt + nlcnt + nhcnt, Tcnt );
/*  FPRINTF( infoptr, " Low Splits:               %d\n", lcnt );
  FPRINTF( infoptr, " High Splits:              %d\n", hcnt  );
  FPRINTF( infoptr, " Not Low Splits:           %d\n", nlcnt );
  FPRINTF( infoptr, " Not High Splits:          %d\n", nhcnt );
  FPRINTF( infoptr, " Return Subgraph Failures: %d\n", rfail ); */
}


/**************************************************************************/
/* LOCAL  **************     IsSplitCandidate      ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF FORALL NODE f IS A LOOP SPLIT CANDIDATE. IF SO, THEN */
/*          RETURN ITS SPLIT TYPE (SPLIT_LOW, SPLIT_NOT_LOW, SPLIT_HIGH, SPLIT_NOT_HIGH); ELSE    */
/*          RETURN SPLIT_BAD.                                                   */
/**************************************************************************/

static int IsSplitCandidate( f, ReasonP )
PNODE f;
char **ReasonP;
{
  register PNODE r;
  register PNODE n;
  register int   s;
  register int   t;

  r = f->F_GEN->G_NODES;

  if ( r->nsucc != NULL ) {
    *ReasonP = "successor is not null";
    return( SPLIT_BAD );
  }

  if ( r->type != IFRangeGenerate ) {
    *ReasonP = "type is not range generate";
    return( SPLIT_BAD );
  }

  /* ONLY SPLIT INNERMOST LOOPS!!! */
  if ( !IsInnerLoop( f->F_BODY ) ) {
    *ReasonP = "not the innermost loop";
    return( SPLIT_BAD );
    }

  /* GET THE CONTROL ROD */
  c = r->exp;

  /* GET THE SPLIT_LOW AND SPLIT_HIGH BOUND OF THE LOOP */
  if ( IsConst( r->imp ) )
    lo = r->imp;
  else
    lo = FindImport( f, r->imp->eport );

  if ( lo == NULL ) {
    *ReasonP = "loop has no low bound";
    return( SPLIT_BAD );
  }

  if ( IsConst( r->imp->isucc ) )
    hi = r->imp->isucc;
  else
    hi = FindImport( f, r->imp->isucc->eport );

  if ( hi == NULL ) {
    *ReasonP = "loop has no high bound";
    return( SPLIT_BAD );
  }

  if ( AreValuesEqual( lo, hi ) ) {
    *ReasonP = "high and low bounds are equal";
    return( SPLIT_BAD );
  }

  /* IDENTIFY THE SPLIT TYPE, IF SUCH A TYPE EVEN EXISTS */
  s = SPLIT_BAD;

  for ( n = f->F_BODY; n != NULL; n = n->nsucc ) {
    /* WITH SHORT CIRCUIT FOR FASTER EXECUTION */
    if ( n->type == IFEqual || n->type == IFNotEqual )
      if ( (t = SplitType( f, n )) != SPLIT_BAD )
        s = t;
    }

  if ( s == SPLIT_BAD ) {
    *ReasonP = "loop is of the wrong split type";
    return( SPLIT_BAD );
  }

  /* CURRENTLY, SPLIT LOOPS MUST BUILD UNFILTERED ARRAYS */
  for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc )
    switch ( n->type ) {
      case IFAGather:
	/* FILTER? */
	if ( n->imp->isucc->isucc != NULL ) {
    	  *ReasonP = "loop builds a filtered array";
	  return( SPLIT_BAD );
        }

	break;

      default:
    	  *ReasonP = "loop builds wrong type of array";
	rfail++;
	return( SPLIT_BAD );
      }

  return( s );
}


/**************************************************************************/
/* LOCAL  **************         DoHighSplit       ************************/
/**************************************************************************/
/* PURPOSE: THE SPLIT TYPE OF FORALL f1 IS ONE OF THE SPLIT_HIGH TYPES (DEFINED */
/*          BY kind).  THIS ROUTINE SPLITS f1 INTO TWO LOOPS, THE SECOND  */
/*          JUST HANDLES THE SPLIT_HIGH TEST.                                   */
/**************************************************************************/

static void DoHighSplit( f1, kind )
PNODE f1;
int   kind;
{
  register PNODE f2;
  register PNODE cat;
  register PEDGE e;
  register PEDGE i;
  register PEDGE se;
  register PEDGE e1;
  register PEDGE e2;
  register PNODE ag;

  f2 = CopyNode( f1 );
  LinkNode( f1, f2 );

  for ( i = f1->imp; i != NULL; i = i->isucc ) {
    e = CopyEdge( i, i->src, f2 );

    LinkImport( f2, e );

    if ( !IsConst( i ) )
      LinkExport( i->src, e );
    }

  for ( ag = f1->F_RET->G_NODES; ag != NULL; ag = ag->nsucc ) {
    cat = NodeAlloc( ++maxint, IFACatenate );
    CopyVitals( f2, cat );
    LinkNode( f2, cat );

    for ( e = f1->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( e->eport == ag->exp->iport ) {
        UnlinkExport( e );
        e->eport = 1;
        LinkExport( cat, e );
        }
      }

    e1 = EdgeAlloc( f1, ag->exp->iport, cat, 1 );
    e1->info = ag->exp->info;
    LinkImport( cat, e1 );
    LinkExport( f1, e1 );

    e2 = EdgeAlloc( f2, ag->exp->iport, cat, 2 );
    e2->info = ag->exp->info;
    LinkImport( cat, e2 );
    LinkExport( f2, e2 );
    }

  if ( kind == SPLIT_HIGH ) {
    FixBody( SPLIT_HIGH, f1, "FALSE" );
    FixBody( SPLIT_HIGH, f2, "TRUE" );
    hcnt++;
  } else {
    FixBody( SPLIT_NOT_HIGH, f1, "TRUE" );
    FixBody( SPLIT_NOT_HIGH, f2, "FALSE" );
    nhcnt++;
    }

  FixControl1( f2->F_GEN->imp->src->imp, f2->F_GEN->imp->src, IFMax );
  FixControl2( hi, c->src->imp->isucc, c->src, "-1" );

  NewCompoundID(f2);		/* Give it a unique ID */
  f2->ThinCopy = TRUE;		/* TO DISABLE CONCURRENTIZATION OF THIN COPY */
}


/**************************************************************************/
/* LOCAL  **************          DoLowSplit       ************************/
/**************************************************************************/
/* PURPOSE: THE SPLIT TYPE OF FORALL f1 IS ONE OF THE SPLIT_LOW TYPES (DEFINED  */
/*          BY kind).  THIS ROUTINE SPLITS f1 INTO TWO LOOPS, THE FIRST   */
/*          JUST HANDLES THE SPLIT_LOW TEST.                                    */
/**************************************************************************/

static void DoLowSplit( f1, kind )
PNODE f1;
int   kind;
{
  register PNODE f2;
  register PNODE cat;
  register PEDGE e;
  register PEDGE i;
  register PEDGE se;
  register PEDGE e1;
  register PEDGE e2;
  register PNODE ag;

  f2 = CopyNode( f1 );
  LinkNode( f1, f2 );

  for ( i = f1->imp; i != NULL; i = i->isucc ) {
    e = CopyEdge( i, i->src, f2 );

    LinkImport( f2, e );

    if ( !IsConst( i ) )
      LinkExport( i->src, e );
    }

  for ( ag = f1->F_RET->G_NODES; ag != NULL; ag = ag->nsucc ) {
    cat = NodeAlloc( ++maxint, IFACatenate );
    CopyVitals( f2, cat );
    LinkNode( f2, cat );

    for ( e = f1->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( e->eport == ag->exp->iport ) {
        UnlinkExport( e );
        e->eport = 1;
        LinkExport( cat, e );
        }
      }

    e1 = EdgeAlloc( f1, ag->exp->iport, cat, 1 );
    e1->info = ag->exp->info;
    LinkImport( cat, e1 );
    LinkExport( f1, e1 );

    e2 = EdgeAlloc( f2, ag->exp->iport, cat, 2 );
    e2->info = ag->exp->info;
    LinkImport( cat, e2 );
    LinkExport( f2, e2 );
    }

  if ( kind == SPLIT_LOW ) {
    FixBody( SPLIT_LOW, f1, "TRUE" );
    FixBody( SPLIT_LOW, f2, "FALSE" );
    lcnt++;
  } else {
    FixBody( SPLIT_NOT_LOW, f1, "FALSE" );
    FixBody( SPLIT_NOT_LOW, f2, "TRUE" );
    nlcnt++;
    }

  FixControl2( lo, f2->F_GEN->imp->src->imp, f2->F_GEN->imp->src, "1" );
  FixControl1( c->src->imp->isucc, c->src, IFMin );

  NewCompoundID(f1);		/* Give it a unique ID */
  f1->ThinCopy = TRUE;		/* TO DISABLE CONCURRENTIZATION OF THIN COPY */
}


/**************************************************************************/
/* LOCAL  **************       SplitForalls        ************************/
/**************************************************************************/
/* PURPOSE: SPLIT THE FORALL LOOPS IN GRAPH g.                            */
/**************************************************************************/

static void SplitForalls( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register int   kind;
  char *Reason;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      PropagateConst( n );

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	SplitForalls( sg );
      }

    if ( !IsForall( n ) )
      continue;

    ++Tcnt;
    
    if ( (kind = IsSplitCandidate( n , &Reason)) == SPLIT_BAD ) 
      continue;

    switch( kind ) {
      case SPLIT_NOT_LOW:
      case SPLIT_LOW:
        DoLowSplit( n, kind );
	break;

      case SPLIT_HIGH:
      case SPLIT_NOT_HIGH:
        DoHighSplit( n, kind );
	break;

      default:
	Error2( "SplitForalls", "ILLEGAL IsSplitCandidate KIND" );
      }
    if (RequestInfo(I_Info1,info)) {
    DYNEXPAND(printinfo,printbuf,printlen,printcount,char,printlen+200);
    printlen += (SPRINTF(printinfo + printlen, 
      " Splitting loop %d at line %d, funct %s, file %s\n\n",
       n->ID, n->line, n->funct, n->file), strlen(printinfo + printlen));
  }
    }
}


/**************************************************************************/
/* GLOBAL **************         If1Split          ************************/
/**************************************************************************/
/* PURPOSE: DETECT AND ELIMINATE BOUNDARY CONDITION CONDITIONALS FROM     */
/*          THE FORALL NODES IN THE PROGRAM. FOR EXAMPLE:                 */
/*                                                                        */
/*    for i in 1,n x := if ( i == 1 ) then 1 else 0 end if;               */
/*    returns array of x end for   BECOMES                                */
/*                                                                        */
/*    array [1:1] || for i in 2,n x:= 0; returns array of x end for       */
/*                                                                        */
/**************************************************************************/

void If1Split()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    SplitForalls( f );
}
