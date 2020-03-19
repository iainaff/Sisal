/* if1invert.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:35  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int invcnt = 0;                         /* COUNT OF INVERTED LOOPS */
static int Tcnt = 0;                         /* COUNT OF LOOPS */
DYNDECLARE(printinfo,printbuf,printlen,printcount,char,2000);

/**************************************************************************/
/* GLOBAL **************     WriteInvertInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE LOOP INVERTION INFORMATION TO stderr.                   */
/**************************************************************************/

void WriteInvertInfo()
{
  FPRINTF( infoptr, "\n\n **** LOOP INVERSION\n\n%s\n", printinfo);
  FPRINTF( infoptr, " Inverted Loops: %d of %d\n", invcnt,Tcnt  );
}


/**************************************************************************/
/* LOCAL  **************    WireSelectSubgraph     ************************/
/**************************************************************************/
/* PURPOSE: WIRE sg TO f AND f TO sg USING s AS THE PARENT.               */
/**************************************************************************/

static void WireSelectSubgraph( s, sg, f )
PNODE s;
PNODE sg;
PNODE f;
{
  register PEDGE i;
  register PEDGE e;

  for ( i = s->imp; i != NULL; i = i->isucc ) {
    e = CopyEdge( i, sg, f );
    e->eport = e->iport;

    LinkExport( sg, e );
    LinkImport( f, e );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    e = CopyEdge( i, f, sg );
    e->eport = e->iport;

    LinkExport( f, e );
    LinkImport( sg, e );
    }
}


/**************************************************************************/
/* LOCAL  **************      FixForallBody        ************************/
/**************************************************************************/
/* PURPOSE: REPLACE s WITH THE NODE LIST OF sg AND WIRE THE NEW NODES     */
/*          INTO FORALL f.                                                */
/**************************************************************************/

static void FixForallBody( s, f, sg )
PNODE s;
PNODE f;
PNODE sg;
{
  register PEDGE e;
  register PEDGE i;
  register PEDGE se;
  register PEDGE si;
  register PNODE n;
  register PNODE sn;

  for ( e = s->exp; e != NULL; e = se ) {
    se = e->esucc;

    UnlinkExport( e );

    if ( (i = FindImport( sg, e->eport )) == NULL )
      Error2( "FixForallBody", "FindImport FAILURE ON ALT IMPORT SEARCH" );

    if ( IsConst( i ) )
      ChangeToConst( e, i );
    else {
      e->eport = i->eport;
      LinkExport( i->src, e );
      }
    }

  for ( i = sg->imp; i != NULL; i = si ) {
    si = i->isucc;

    UnlinkExport( i );
    UnlinkImport( i );
    /* free( i ); */
    }

  for ( e = sg->exp; e != NULL; e = se ) {
    se = e->esucc;

    UnlinkExport( e );

    if ( (i = FindImport( s, e->eport )) == NULL )
      Error2( "FixForallBody", "FindImport FAILURE ON Select IMPORT SEARCH" );

    if ( IsConst( i ) )
      ChangeToConst( e, i );
    else {
      e->eport = i->eport;
      LinkExport( i->src, e );
      }
    }

  for ( i = s->imp; i != NULL; i = si ) {
    si = i->isucc;

    UnlinkExport( i );
    UnlinkImport( i );
    /* free( i ); */
    }

  /* BUG FIX CANN: 7/7/91 */
  /* UnlinkNode( s ); */
  /* LinkNodeLists( f->F_BODY, sg ); */

  for ( n = sg->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    UnlinkNode( n );
    LinkNode( s->npred, n );
    }

  UnlinkNode( s );
}


/**************************************************************************/
/* LOCAL  **************      GetFirstNoOp         ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE FIRST NoOp NODE IN GRAPH g.                        */
/**************************************************************************/

static PNODE GetFirstNoOp( g )
PNODE g;
{
  register PNODE n;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( n->type == IFNoOp )
      return( n );

  Error2( "GetFirstNoOp", "NoOp NOT FOUND" );
  return NULL;
}


/**************************************************************************/
/* LOCAL  **************      DoTheInversion       ************************/
/**************************************************************************/
/* PURPOSE: DO THE LOOP INVERSION FOR f WITH SELECT NODE s.               */
/**************************************************************************/

static void DoTheInversion( f, s )
PNODE f;
PNODE s;
{
  register PNODE fa;
  register PNODE fc;
  register PNODE a;
  register PNODE c;
  register PEDGE i;

  /* CHANGE THE SELECT TYPE TO AVOID COPYING s WHEN COPYING f */
  s->type = IFNoOp;

  /* MAKE COPIES OF f */
  fa = CopyNode( f ); NewCompoundID(fa);
  fc = CopyNode( f ); NewCompoundID(fc);

  FixForallBody( GetFirstNoOp( fa->F_BODY ), fa, s->S_ALT );
  FixForallBody( GetFirstNoOp( fc->F_BODY ), fc, s->S_CONS );

  /* CONVERT f INTO A Select NODE; THE ASSUMED SUBGRAPH ORDER IS S_TEST, */
  /* S_ALT, AND S_CONS. */
  f->type = IFSelect;
  CopyPragmas( s, f );
  f->C_ALST = s->C_ALST;

  f->gsucc  = NULL;
  f->gpred  = NULL;

  if ( (i = FindImport( s, s->S_TEST->imp->eport )) == NULL )
    Error2( "DoTheInversion", "FindImport FAILURE" );

  if ( IsConst( i ) )
    Error2( "DoTheInversion", "FindImport FOUND CONSTANT" );

  LinkGraph( f, s->S_TEST );
  f->S_TEST->G_DAD = f;
  f->S_TEST->imp->eport = i->eport;

  a = NodeAlloc( 0, IFSGraph );
  LinkGraph( f->S_TEST, a );
  a->G_DAD = f;
  LinkNode( a, fa );

  c = NodeAlloc( 0, IFSGraph );
  LinkGraph( f->S_ALT, c );
  c->G_DAD = f;
  LinkNode( c, fc );

  WireSelectSubgraph( f, a, fa );
  WireSelectSubgraph( f, c, fc );
}


/**************************************************************************/
/* LOCAL  **************       InvertLoops         ************************/
/**************************************************************************/
/* PURPOSE: INVERT CANDIDATE LOOPS IN GRAPH g.                            */
/*                                                                        */ 
/*    for i in 1,10                                                       */
/*    returns array of if ( G ) then a + 1 else b[i] end if end for       */
/*                                                                        */ 
/*    BECOMES  if ( G ) then for i in 1,10 returns array of a+1 end for   */
/*             else for i in 1,10 returns array of b[i] end for end if    */
/**************************************************************************/

static void InvertLoops( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE nn;
  register PEDGE i;
  register PEDGE e;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	InvertLoops( sg );
    
    if ( !IsForall( n ) )
      continue;

    ++Tcnt;

    /* IS nn A RangeGenerate NODE AND THE ONLY NODE IN THE GENERATOR */
    nn = n->F_GEN->G_NODES;
    if ( nn == NULL || nn->nsucc != NULL ||
       nn->type != IFRangeGenerate ) 
      continue;

    /* FIND THE FIRST SELECT HAVING AN INVARIANT TEST */
    for ( nn = n->F_BODY->G_NODES; nn != NULL; nn = nn->nsucc ) {
      if ( !IsSelect( nn ) ) 
        continue;

      /* IS THE SELECT TEST INVARIANT TO n */
      i = nn->S_TEST->imp;
      if ( IsConst( i ) || !IsSGraph( i->src ) ||
	(i = FindImport( nn, i->eport )) == NULL || IsConst( i ) ||
        !IsSGraph( i->src ) || !IsImport( n, i->eport ) ) 
        continue;

      break;
      }

    if ( nn == NULL )
      continue;

    /* RETURN FANOUT NOT ALLOWED! */
    for ( i = n->F_RET->imp; i != NULL; i = i->isucc )
      if ( i->esucc != NULL )
	break;

    if ( i != NULL ) 
      continue;

    /* MAKE SURE INVERSION WILL NOT BLOCK BUILD-IN-PLACE ANALYSIS AND   */
    /* INTRODUCE COPYING: f := for ...; c := addh(f,v); d := addl(f,v); */
    /*                    e := f || ...;                                */

    for ( i = n->F_RET->imp; i != NULL; i = i->isucc ) {
      switch ( i->src->type ) {
	case IFAGather:
	  goto MoveOn;

	case IFReduce:
	case IFRedLeft:
	case IFRedRight:
	case IFRedTree:
	  if ( i->src->imp->CoNsT[0] == REDUCE_CATENATE )
	    goto MoveOn;

	  break;

	default:
	  break;
	}

      continue;

MoveOn:

      for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( e->eport != i->iport )
	  continue;

	if ( e->dst->type == IFAAddH )
	  if ( e->iport == 1 )
	    break;

	if ( e->dst->type == IFAAddL )
	  if ( e->iport == 1 )
	    break;

	if ( e->dst->type == IFACatenate )
	  break;
	}

      if ( e != NULL )
	break;
      }

    if ( i != NULL ) 
      continue;

    /* IS n AN INNER LOOP? */
    if ( !IsInnerLoop( n->F_BODY ) ) 
      continue;

    /* OK, DO IT! */
    if (RequestInfo(I_Info1, info)) {
    DYNEXPAND(printinfo,printbuf,printlen,printcount,char,printlen+300);
    printlen += (SPRINTF(printinfo + printlen,
      " Inverting loop %d at line %d, funct %s, file %s\n",
       n->ID, n->line, n->funct, n->file), strlen(printinfo + printlen));
    printlen += (SPRINTF(printinfo + printlen,
      " with loop %d at line %d, funct %s, file %s\n\n",
       nn->ID, nn->line, nn->funct, nn->file), strlen(printinfo + printlen));
  }
    DoTheInversion( n, nn );
    invcnt++;
    Tcnt++;
    }
}


/**************************************************************************/
/* GLOBAL **************         If1Invert         ************************/
/**************************************************************************/
/* PURPOSE: INVERT CANDIDATE LOOPS IN ALL FUNCTION GRAPHS.                */
/**************************************************************************/

void If1Invert()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    InvertLoops( f );
}
