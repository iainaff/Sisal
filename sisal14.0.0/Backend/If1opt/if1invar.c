/* if1invar.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:34  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int   icnt = 0;                     /* COUNT OF INVARIANTS REMOVED */
static int eivcnt = 0;                     /* COUNT OF EXPOSED INVARIANTS */
static int  amcnt = 0;                     /* COUNT OF ANTI-MOVEMENTS     */


/**************************************************************************/
/* GLOBAL  **************    OptIsEdgeInvariant    ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n IS LOOP INVARIANT. RETURN AND GENERATE  */
/**************************************************************************/

int OptIsEdgeInvariant( i )
PEDGE i;
{
  register PNODE dad;

  if ( IsConst( i ) )
    return( TRUE );

  if ( !IsSGraph( i->src ) )
    return( FALSE );

  dad = i->src->G_DAD;

  if ( !IsImport( dad, i->eport ) )
    return( FALSE );

  if ( IsLoop( dad ) || IsForall( dad ) )
    return( TRUE );

  return( FALSE );
}


/**************************************************************************/
/* GLOBAL **************    ExposeInvariants       ************************/
/**************************************************************************/
/* PURPOSE: EXPOSE INVARIANTS ALONG THE ASSOCIATIVE AND COMMUTATIVE CHAIN */
/*          TERMINATED BY s, GIVEN t IS A PREDECESSOR TO s ON THE CHAIN.  */
/**************************************************************************/

void ExposeInvariants( s, t )
PNODE s;
PNODE t;
{
  register PEDGE e;
  register PEDGE ee;

  /* HAS THE ENTIRE CHAIN BEEN SEARCHED? */
  if ( s->type != t->type )
    return;

  /* LET CONSTANT FOLDING HAVE A SHOT AT IT! */
  if ( IsConst( t->imp ) && IsConst( t->imp->isucc ) )
    return;

  /* IS TARGET t A CANDIDATE? */
  if ( !OptIsEdgeInvariant( t->imp->isucc ) ) {
    if ( t != s )
      goto MoveUpTheChain;
    else
      return;
    }

  if ( OptIsEdgeInvariant( t->imp ) )
    return;

  /* ARE WE JUST STARTING? */
  if ( s == t ) 
    goto MoveUpTheChain;

  /* OK, DO THE MODIFICATION */
  e = s->imp;
  UnlinkExport( e );
  UnlinkImport( e );
  e->eport = 1;
  e->iport = 2;

  LinkExportLists( e->src, s );

  ee = t->imp->isucc;
  UnlinkImport( ee );
  ee->iport = 1;
  LinkImport( s, ee );

  LinkImport( t, e );
  LinkExport( s, e );

  UnlinkNode( s );
  LinkNode( t->npred, s );

  eivcnt++;
  return;

MoveUpTheChain:
  if ( IsConst( t->imp ) )
    return;

  /* FANOUT NOT ALLOWED IN THE CHAIN! */
  if ( t->imp->src->exp->esucc != NULL )
    return;

  ExposeInvariants( s, t->imp->src );
}


/**************************************************************************/
/* LOCAL  **************      RemoveInvariants     ************************/
/**************************************************************************/
/* PURPOSE: REMOVE ALL LOOP INVARIANTS FROM THE LOOP AND FORALL NODES     */
/*          DEFINED IN GRAPH g.  INVARIANTS OF INNERMOST LOOPS ARE        */
/*          REMOVED FIRST.  THE NODES ARE EXAMINED IN DATAFLOW ORDER;     */
/*          HENCE, AN INVARIANT NODE WHOSE IMPORTS COME FROM OTHER        */
/*          INVARIANT NODES IN THE SAME SCOPE ARE EASILY DETECTED.        */
/**************************************************************************/

static void RemoveInvariants( lvl, g )
int   lvl;
PNODE g;
{
    register PNODE n;
    register PNODE nd;
    register PNODE sn;
    register PNODE sg;
    register PEDGE e;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
      if ( IsCompound( n ) )
        for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
          RemoveInvariants(  lvl + 1, sg );

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
      if ( !(IsForall( n ) || IsLoop( n )) )
	continue;

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        for ( nd = sg->G_NODES; nd != NULL; nd = sn ) {
          sn = nd->nsucc;

          if ( IsLoop( nd ) || IsForall( nd ) )
            if ( (lvl == 1) && (!Oinvar) )
              continue;

          if ( OptIsInvariant( nd ) ) {
            switch ( nd->type ) {
              case IFAFill:
              case IFABuild:
              case IFAAddH:
              case IFAAddL:
              case IFACatenate:
                if ( !IsForall( n ) )
                  break;

                if ( !IsReadOnly( nd, 1 ) )
                  continue;

                break;

              case IFForall:
                if ( !IsForall( n ) )
                  break;

                for ( e = nd->exp; e != NULL; e = e->esucc )
                  if ( !IsReadOnly( nd, e->eport ) )
                    break;

                if ( e != NULL ) 
                  continue;

                break;

              default:
                break;
              }

            RemoveNode( nd, sg );
            InsertNode( n, nd ); icnt++;
            }
          }
      }
}


/**************************************************************************/
/* LOCAL  **************        ExlusiveUse        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE EXCLUSIVE SUBGRAPH OF SELECT NODE s REFERENCING    */
/*          IMPORT iport,  ELSE RETURN NULL.                              */
/**************************************************************************/

static PNODE ExclusiveUse( s, iport )
PNODE s;
int   iport;
{
  register PEDGE e1;
  register PEDGE e2;

  if ( IsExport( s->S_TEST, iport ) )
    return( NULL );

  e1 = FindExport( s->S_CONS, iport );
  e2 = FindExport( s->S_ALT,  iport );

  if ( e1 != NULL && e2 != NULL )
    return( NULL );

  if ( e1 == NULL && e2 == NULL )
    return( NULL );

  if ( e1 == NULL )
    return( s->S_ALT );

  return( s->S_CONS );
}


/**************************************************************************/
/* STATIC **************       AntiMovement        ************************/
/**************************************************************************/
/* PURPOSE: APPLY ANTI-NODE MOVEMENT TO GRAPH g.  THIS IS AN ATTEMPT TO   */
/*          MOVE COMPUTATIONS BACK ACROSS SUBGRAPH BOARDERS TOWARD THEIR  */
/*          CONSUMERS.                                                    */
/**************************************************************************/

void AntiMovement( g )
PNODE g;
{
  register PNODE n;
  register PEDGE e;
  register PNODE sg;
  register PNODE s;
  register PNODE np;
  register PEDGE ee;
  register PEDGE se;
  register PEDGE see;
  register PEDGE i;
  register PEDGE si;
  register int   port;

  if ( DeBuG ) return;

  for ( n = FindLastNode( g ); n != g; n = np ) {
    np = n->npred;

    /* for -glue and PEEK */
    if ( n->exp == NULL )
      continue;

    if ( IsCompound( n ) )
      continue;


    /* 12/10/91 CANN TO PREVENT BUILD-IN-PLACE FAILURES LIKE THE FOLLOWING:  */
    /* a := array_addh(for...end for,v); in if (x) error[work] else a end if */
    for ( i = n->imp; i != NULL; i = i->isucc ) {
      if ( !IsBasic( i->info ) )
	break;
      }
    if ( i != NULL )
      continue;


    s  = NULL;
    sg = NULL;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
      if ( !IsSelect( e->dst ) )
	break;

      if ( s == NULL ) {
	s = e->dst;

	if ( (sg = ExclusiveUse( s, e->iport )) == NULL )
	  break;
	}
      else if ( s != e->dst )
	break;

      if ( sg != ExclusiveUse( s, e->iport ) )
	break;
      }

    if ( e != NULL )
      continue;

    /* OK, DO THE MOVEMENT */
    UnlinkNode( n );
    LinkNode( sg, n );

    e = n->exp;
    n->exp = NULL;

    for ( /* NOTHING */; e != NULL; e = se ) {
      se = e->esucc;

      UnlinkImport( e );

      for ( ee = sg->exp; ee != NULL; ee = see ) {
	see = ee->esucc;

	if ( ee->eport != e->iport )
	  continue;

	UnlinkExport( ee );
	ee->eport = e->eport;
	LinkExport( n, ee );
	}
      }

    i = n->imp;
    n->imp = NULL;

    for ( /* NOTHING */; i != NULL; i = si ) {
      si = i->isucc;

      if ( IsConst( i ) ) {
	LinkImport( n, i );
	continue;
	}


      port = ++maxint;

      ee = EdgeAlloc( sg, port, n, i->iport );
      ee->info = i->info;

      i->iport = port;

      LinkImport( s, i );
      LinkExport( sg, ee );
      LinkImport( n, ee );
      }

    amcnt++;
    }

  /* OK, NOW STEP INTO THE NEST */
  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( IsCompound( n )  )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	AntiMovement( sg );
}


/**************************************************************************/
/* GLOBAL **************       WriteInvarInfo      ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING INVARIANT REMOVAL TO stderr.*/
/**************************************************************************/

void WriteInvarInfo()
{
  FPRINTF( infoptr, "\n **** LOOP INVARIANT NODE REMOVAL\n\n" );
  FPRINTF( infoptr, " Loop Invariants Removed:   %d\n", icnt   );
  FPRINTF( infoptr, " Exposed Invariants:        %d\n", eivcnt );
  FPRINTF( infoptr, " Movements into Selects:    %d\n", amcnt  );
}


/**************************************************************************/
/* GLOBAL **************         If1Invar          ************************/
/**************************************************************************/
/* PURPOSE: PERFORM LOOP INVARIANT REMOVAL ON ALL FUNCTION GRAPHS. FIRST, */
/*          SPECIAL SELECT INVARIANTS ARE MOVED.                          */
/**************************************************************************/

static int first = TRUE;
static int adone = FALSE;

void If1Invar()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    /* EXPOSURE CAN ALTER CSE SO GIVE CSE A CHANCE BEFORE EXPOSURE */
    if ( !first )
      OptRemoveSCses( f ); 

    RemoveInvariants( 1, f );

    if ( amove && first && !adone )
      AntiMovement( f );
    }

  first = FALSE;
  adone = TRUE;
}
