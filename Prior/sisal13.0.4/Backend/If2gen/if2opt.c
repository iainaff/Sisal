/* if2opt.c,v
 * Revision 12.7  1992/11/04  22:05:02  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:01  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int gcnt = 0;                          /* COUNT OF REMOVED GROUNDS */

int rmcmcnt = 0;                           /* COUNT OF REMOVED CM PRAGMAS */
int trmcmcnt = 0;                           /* COUNT OF REMOVED CM PRAGMAS */
int rmpmcnt = 0;                           /* COUNT OF REMOVED PM PRAGMAS */
int trmpmcnt = 0;                           /* COUNT OF REMOVED PM PRAGMAS */
int rmsrcnt = 0;                           /* COUNT OF REMOVED SR PRAGMAS */
int trmsrcnt = 0;                           /* COUNT OF REMOVED SR PRAGMAS */
int rmcnoop = 0;               /* COUNT OF REMOVED CONDITIONAL COPY NoOpS */
int rmnoop  = 0;                           /* COUNT OF REMOVED COPY NoOpS */
int rmsmark = 0;                           /* COUNT OF REMOVED smarks     */
char rmark[2000];                           /* COUNT OF REMOVED smarks     */
int rmvmark = 0;                           /* COUNT OF REMOVED vmarks     */
char vmark[2000];                           /* COUNT OF REMOVED vmarks     */
int rmdab   = 0;                  /* COUNT OF REMOVED IFDefArrayBuf NODES */


/**************************************************************************/
/* LOCAL  **************     FixErrorConstants     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN ERROR CONSTANTS THEIR OWN SOURCE NODE.                 */
/**************************************************************************/

static void FixErrorConstants( n )
PNODE n;
{
  register PEDGE i;
  register PNODE nn;

  for ( i = n->imp; i != NULL; i = i->isucc )
    if ( IsConst( i ) && i->CoNsT == NULL ) {
      i->cm = 0;
      i->pm = 0;
      i->sr = 0;

      nn = NodeAlloc ( ++maxint, IFError );
      i->eport = 1;
      LinkExport( nn, i );

      if ( IsGraph( n ) )
	LinkNode( n, nn );
      else
	LinkNode( n->npred, nn );
      }
}


/**************************************************************************/
/* LOCAL  **************         FoldLogical       ************************/
/**************************************************************************/
/* PURPOSE: DO THE FOLLOWING LOGICAL TRANSFORMATIONS:  !(A<=B) -> A > B   */
/*          AND !(A<B) -> A >= B. NODE n DEFINES THE ROOT OPERATION <= OR */
/*          <.                                                            */
/**************************************************************************/

static void FoldLogical( n )
PNODE n;
{
  register PEDGE e;

  switch ( n->type ) {
    case IFLessEqual:
      /* IS IFNot THE ONLY USE */
      if ( n->exp->esucc != NULL )
        break;

      if ( n->exp->dst->type != IFNot )
        break;

      UnlinkNode( n->exp->dst );
      n->exp = n->exp->dst->exp;

      for ( e = n->exp; e != NULL; e = e->esucc )
        e->src = n;

      n->type = IFGreat;
      break;

    case IFLess:
      /* IS IFNot THE ONLY USE */
      if ( n->exp->esucc != NULL )
        break;

      if ( n->exp->dst->type != IFNot )
        break;

      UnlinkNode( n->exp->dst );
      n->exp = n->exp->dst->exp;

      for ( e = n->exp; e != NULL; e = e->esucc )
        e->src = n;

      n->type = IFGreatEqual;
      break;

    default:
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************     FoldReturnPragmas     ************************/
/**************************************************************************/
/* PURPOSE: FOLD THE REFERENCE COUNT PRAGMAS ON MULTIPLE NODE EXPORTS.    */
/*          THIS MAKES LIFE EASIER AND EXECUTION FASTER!                  */
/**************************************************************************/

static void FoldReturnPragmas( l, r )
PNODE l;
PNODE r;
{
  register PEDGE i;
  register PEDGE e;
  register PEDGE ee;

  for ( i = r->imp; i != NULL; i = i->isucc ) {
    if ( i->iport == 0 )
      continue;

    if ( IsBasic( i->info ) )
      continue;

    e = FindExport( l, i->iport );

    switch ( i->src->type ) {
      case IFFinalValue:
      case IFFinalValueAT:
        i->pm += i->cm + e->pm;
	break;

      default:
	i->sr += i->cm + e->pm;
	break;
      }

    if ( e->pm > 0 ) {
      rmpmcnt++;
      trmpmcnt++;
      }
    else
      trmpmcnt++;

    if ( i->cm == -1 ) {
      rmcmcnt++;
      trmcmcnt++;
      }
    else
      trmcmcnt++;

    i->cm = 0;

    for ( ee = l->exp; ee != NULL; ee = ee->esucc )
      if ( ee->eport == e->eport )
	ee->pm = 0;
    }
}


/**************************************************************************/
/* LOCAL  **************       FoldPragmas         ************************/
/**************************************************************************/
/* PURPOSE: ELIMINATE VARIOUS pm AND cm PRAGMAS.                          */
/**************************************************************************/

static void FoldPragmas( n )
PNODE n;
{
    switch( n->type ) {
      case IFForall:
        FoldReturnPragmas( n, n->F_RET );
	break;

      case IFLoopA:
      case IFLoopB:
        FoldReturnPragmas( n, n->L_RET );
	break;

      default:
	break;
      }
}


/**************************************************************************/
/* LOCAL  **************    RemoveMultipleTypes    ************************/
/**************************************************************************/
/* PURPOSE: REPLACE MULTIPLE TYPE REFERENCES ON THE IMPORTS OF NODE n     */
/*          WITH REFERENCES TO THE CONSTITUENT TYPES.                     */
/**************************************************************************/

static void RemoveMultipleTypes( n )
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc )
	if ( IsMultiple( i->info ) )
	    i->info = i->info->A_ELEM;
}


/**************************************************************************/
/* LOCAL  **************    ApplyVariousIf2Opts    ************************/
/**************************************************************************/
/* PURPOSE: APPLY THE VARIOUS MISCELLANEOUS GENERAL PURPOSE OPTIMIZATIONS */
/*          TO GRAPH g.                                                   */
/**************************************************************************/

static void ApplyVariousIf2Opts( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE s;
  register PEDGE m;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        ApplyVariousIf2Opts( sg );

    switch ( n->type ) {
      case IFDefArrayBuf:
	s = n->imp;
	m = n->exp;

	switch ( m->dst->type ) {
	  case IFMemAlloc:
	    break;

	  default:
	    Error2( "ApplyVariousIf2Opts", "DESTINATION NOT A IFMemAlloc NODE");
          }

	UnlinkImport( s );
	s->iport = m->iport;

	UnlinkImport( m );
	UnlinkExport( m );

	LinkImport( m->dst, s );
	n = n->npred;
	UnlinkNode( n->nsucc );
	rmdab++;
	break;

      case IFReduceAT:
      case IFRedLeftAT:
      case IFRedTreeAT:
      case IFRedRightAT:
	if ( !bip ) {
	  if ( n->exp->dst->G_DAD->vmark ) {
	    rmvmark++;
#ifdef MYI
            SPRINTF(vmark,
                "%s Sequentializing loop %d at line %d, funct %s, file %s\n\n", 
                vmark, n->ID, n->line, n->funct, n->file);
#endif
	    n->exp->dst->G_DAD->vmark = FALSE;
	    }

	  if ( n->exp->dst->G_DAD->smark ) {
	    rmsmark++;
#ifdef MYI
            SPRINTF(rmark,
                "%s Sequentializing loop %d at line %d, funct %s, file %s\n\n", 
		rmark, n->ID, n->line, n->funct, n->file);
#endif
	    n->exp->dst->G_DAD->smark = FALSE;
	    }
          }

	break;

      default:
	break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************      RemoveGrounds        ************************/
/**************************************************************************/
/* PURPOSE: SHOULD STORAGE BE RECYCLED BEFORE COMPLETION OF A NON-        */
/*          RECURSIVE PROGRAM ENTRY POINT? YES, IF standalone IS TRUE.    */
/**************************************************************************/

static void RemoveGrounds( f )
PNODE f;
{
  register PEDGE i;
  register PEDGE si;

  /* if ( !(f->emark && standalone) ) */ 
  if (  f->mark != 'e' )  /* NEW CANN 2/92 */
    return;

  if ( f->Cmark )
    return;

  for ( i = f->imp; i != NULL; i = si ) {
    si = i->isucc;

    /* GROUND EDGE? */
    if ( i->iport == 0 ) {
      if ( i->cm == -1 ) {
        rmcmcnt++;
        trmcmcnt++;
	}
      else
        trmcmcnt++;

      UnlinkImport( i );
      UnlinkExport( i ); 
      gcnt++;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************     AddToFlopCounts       ************************/
/**************************************************************************/
/* PURPOSE: IF APPROPRIATE, ADD n TO GRAPH g's FLOP COUNTERS.             */
/**************************************************************************/

static void AddToFlopCounts( g, n )
PNODE g;
PNODE n;
{
  register PNODE f;

  switch ( n->type ) {
    case IFPlus:
    case IFTimes:
    case IFDiv:
    case IFMinus:
    case IFNeg:
      if ( IsReal( n->imp->info ) || IsDouble( n->imp->info ) )
	g->flps[ARITHMETICS]++;

      break;

    case IFForall:
      n->F_GEN->flps[LOGICALS]    = 0;
      n->F_GEN->flps[ARITHMETICS] = 0;
      n->F_GEN->flps[INTRINSICS]  = 0;

      n->F_BODY->flps[LOGICALS]    += n->F_RET->flps[LOGICALS];
      n->F_BODY->flps[ARITHMETICS] += n->F_RET->flps[ARITHMETICS];
      n->F_BODY->flps[INTRINSICS]  += n->F_RET->flps[INTRINSICS];

      n->F_RET->flps[LOGICALS]     = 0;
      n->F_RET->flps[ARITHMETICS]  = 0;
      n->F_RET->flps[INTRINSICS]   = 0;
      break;

    case IFLoopA:
    case IFLoopB:
      n->L_INIT->flps[LOGICALS]    = 0;
      n->L_INIT->flps[ARITHMETICS] = 0;
      n->L_INIT->flps[INTRINSICS]  = 0;

      /* ADD IMPLICIT FIRST RETURN AND TEST COUNTS TO g'S COUNTS */
      g->flps[LOGICALS]    += n->L_RET->flps[LOGICALS] +
			      n->L_TEST->flps[LOGICALS];
      g->flps[ARITHMETICS] += n->L_RET->flps[ARITHMETICS] + 
			      n->L_TEST->flps[ARITHMETICS];
      g->flps[INTRINSICS]  += n->L_RET->flps[INTRINSICS] + 
			      n->L_TEST->flps[INTRINSICS];

      /* EVERY BODY EXECUTION IS FOLLOWED BY RETURN AND TEST EXECUTION */
      n->L_BODY->flps[LOGICALS]    += n->L_RET->flps[LOGICALS] +
			              n->L_TEST->flps[LOGICALS];
      n->L_BODY->flps[ARITHMETICS] += n->L_RET->flps[ARITHMETICS] +
			              n->L_TEST->flps[ARITHMETICS];
      n->L_BODY->flps[INTRINSICS]  += n->L_RET->flps[INTRINSICS] +
			              n->L_TEST->flps[INTRINSICS];

      n->L_TEST->flps[LOGICALS]    = 0;
      n->L_TEST->flps[ARITHMETICS] = 0;
      n->L_TEST->flps[INTRINSICS]  = 0;

      n->L_RET->flps[LOGICALS]     = 0;
      n->L_RET->flps[ARITHMETICS]  = 0;
      n->L_RET->flps[INTRINSICS]   = 0;
      break;

    case IFSelect:
      n->S_CONS->flps[LOGICALS]    += n->S_TEST->flps[LOGICALS];
      n->S_CONS->flps[ARITHMETICS] += n->S_TEST->flps[ARITHMETICS];
      n->S_CONS->flps[INTRINSICS]  += n->S_TEST->flps[INTRINSICS];

      n->S_ALT->flps[LOGICALS]    += n->S_TEST->flps[LOGICALS];
      n->S_ALT->flps[ARITHMETICS] += n->S_TEST->flps[ARITHMETICS];
      n->S_ALT->flps[INTRINSICS]  += n->S_TEST->flps[INTRINSICS];

      n->S_TEST->flps[LOGICALS]    = 0;
      n->S_TEST->flps[ARITHMETICS] = 0;
      n->S_TEST->flps[INTRINSICS]  = 0;
      break;

    case IFTagCase:
      break;

    case IFEqual:
    case IFNotEqual:
    case IFLess:
    case IFLessEqual:
    case IFGreat:
    case IFGreatEqual:
      if ( IsReal( n->imp->info ) || IsDouble( n->imp->info ) )
	g->flps[LOGICALS]++;

      break;

    case IFReduce:
    case IFRedLeft:
    case IFRedRight:
    case IFRedTree:
      if ( n->imp->isucc->isucc->isucc != NULL )
	break;

      switch( n->imp->CoNsT[0] ) {
       case REDUCE_PRODUCT:
       case REDUCE_SUM:
	  if ( IsReal( n->exp->info ) || IsDouble( n->exp->info ) )
	    g->flps[ARITHMETICS]++;

	  break;

       case REDUCE_GREATEST:
       case REDUCE_LEAST:
	  if ( IsReal( n->exp->info ) || IsDouble( n->exp->info ) )
	    g->flps[LOGICALS]++;

	  break;

       case REDUCE_USER:
       case REDUCE_CATENATE:
          break;
 
        default:
          UNEXPECTED("Unknown reduction");
          ERRORINFO("%s", n->imp->CoNsT);
          ERRORINFO("%c", n->imp->CoNsT[0]);
	}

      break;

    case IFCall:
      f = FindFunction( n->imp->CoNsT );

      if ( IsIGraph( f ) ) {
	if ( !GenIsIntrinsic( f ) )
	  break;

	if ( strcmp( n->imp->CoNsT, "and" ) == 0 ) break;
	if ( strcmp( n->imp->CoNsT, "or" ) == 0 ) break;
	if ( strcmp( n->imp->CoNsT, "xor" ) == 0 ) break;
	if ( strcmp( n->imp->CoNsT, "shiftl" ) == 0 ) break;
	if ( strcmp( n->imp->CoNsT, "shiftr" ) == 0 ) break;

	g->flps[INTRINSICS]++;
        }
      
      break;

    case IFExp:
      if ( IsReal( n->exp->info ) || IsDouble( n->exp->info ) )
	g->flps[INTRINSICS]++;

      break;

    default:
      break;
    }
}


/**************************************************************************/
/* GLOBAL **************       PrepareGraph        ************************/
/**************************************************************************/
/* PURPOSE: APPLY MISCELLANEOUS CLEANUPS TO GRAPH g IN PREPARATION FOR    */
/*          THE START OF if2gen EXECUTION.                                */
/**************************************************************************/

void PrepareGraph( g )
register PNODE g;
{
  register PNODE sg;
  register PNODE n;

  RemoveMultipleTypes( g );
  FixErrorConstants( g );
  FoldPragmas( g );

  g->flps = (int *) MyAlloc((int)(sizeof(int)*3));
  g->flps[ARITHMETICS] = 0;
  g->flps[LOGICALS]    = 0;
  g->flps[INTRINSICS]  = 0;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    RemoveMultipleTypes( n );
    FoldPragmas( n );
    FixErrorConstants( n );
    FoldLogical( n );

    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        PrepareGraph( sg );

    AddToFlopCounts( g, n );
    }
}


/**************************************************************************/
/* GLOBAL **************          If2Opt           ************************/
/**************************************************************************/
/* PURPOSE: APPLY MISCELLANEOUS OPTIMIZATIONS TO THE IF2 GRAPHS.          */
/**************************************************************************/

void If2Opt()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    ApplyVariousIf2Opts( f );

    if ( !freeall )
      RemoveGrounds( f );
    }
}


/**************************************************************************/
/* GLOBAL **************     WriteIf2OptInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE if2opt INFORMATION TO stderr.                           */
/**************************************************************************/
void WriteIf2OptInfo2()
{
  FPRINTF( infoptr4, "\n **** SEQUENTIALIZED LOOPS\n\n" );
  FPRINTF( infoptr4, "%s\n Sequentialized Concurrent Loops: %d\n", rmark,rmsmark );
  FPRINTF( infoptr4, "%s\n Sequentialized Vector Loops:     %d\n", vmark,rmvmark );
}

void WriteIf2OptInfo()
{
  FPRINTF( infoptr3, "\n **** ADDITIONAL COPY ELIMINATIONS\n\n" );
/*  FPRINTF( infoptr3, " Removed Ground Imports:           %d\n", gcnt  ); */
  FPRINTF( infoptr3, " Additionally Eliminated Increment Reference Count Pragmas: %d of %d\n", rmpmcnt, trmpmcnt );
  FPRINTF( infoptr3, " Additionally Eliminated Decrement Reference Count Pragmas: %d of %d\n", rmcmcnt, trmcmcnt );
  FPRINTF( infoptr3, " Additionally Eliminated Set Reference Count Pragmas:       %d of %d\n", rmsrcnt, trmsrcnt );
  FPRINTF( infoptr3, " Additionally Eliminated Copy Nodes:                        %d\n", rmnoop  );
  FPRINTF( infoptr3, " Additionally Eliminated Conditional Copy Nodes:            %d\n", rmcnoop );
  /*FPRINTF( infoptr3, " Removed IFDefArrayBuf Nodes:     %d\n", rmdab   ); */
}
