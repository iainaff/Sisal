/* if2vector.c,v
 * Revision 12.7  1992/11/04  22:05:04  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:05  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int  fmin  = 0;                     /* VECTORIZATION FORM COUNTERS */
static int  famin = 0;
static int  fmax  = 0;
static int  famax = 0;
static int  fsum  = 0;
static int  tri   = 0;


/**************************************************************************/
/* GLOBAL **************     WriteVectorInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE VECTORIZATION FEEDBACK TO stderr.                       */
/**************************************************************************/

void WriteVectorInfo()
{
  FPRINTF( infoptr, "\n **** VECTOR OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr, " First Minimum      conversions:  %d\n", fmin  );
  FPRINTF( infoptr, " First Abs Minimum  conversions:  %d\n", famin );
  FPRINTF( infoptr, " First Maximum      conversions:  %d\n", fmax  );
  FPRINTF( infoptr, " First Abs Maximum  conversions:  %d\n", famax );
  FPRINTF( infoptr, " First Sum          conversions:  %d\n", fsum  );
  FPRINTF( infoptr, " Tri-Diagonal       conversions:  %d\n", tri   );
}


/**************************************************************************/
/* LOCAL  **************    SPECIAL VECTOR LOOPS   ************************/
/**************************************************************************/
/** TRI-DIANGONAL ELIMINATION, BELOW DIAGONAL:
  for initial
    i := 2; X := XIn[1];
  while i <= n repeat
    i := old i + 1;
    X := Z[old i] * (Y[old i] - old X)
  returns array of X
  end for

** FIRST SUM:
  for initial
    i := 2; X := Yin[1];
  while ( i <= n ) repeat
    i := old i + 1;
    X := old X + Yin[old i];
  returns array of X
  end for

** INDEX OF FIRST MIN, MAX, ABSOLUTE MIN, ABSOLUTE MAX:
  for initial
    max24 := 1; k := 2;
  while ( k <= n ) repeat
    k := old k + 1;
    max24 := if ( X[old k] < X[old max24] ) then 
	     old k  else old max24 end if;
  returns value of max24
  end for
**/
/**************************************************************************/


static int IsFirstAbsMin( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PNODE t1;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;
  register PNODE abs1;
  register PNODE abs2;
  register PEDGE ii;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( !IsConst( c ) ) return( FALSE );
  if ( !IsConst( x ) ) return( FALSE );
  if ( atoi( c->CoNsT ) != (atoi( x->CoNsT ) + 1) ) return( FALSE );

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */

  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFFinalValue ) return( FALSE );
  if ( n->imp->isucc != NULL ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( n->imp->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  /* SELECT */
  op = xx->src;
  if ( op->type != IFSelect ) return( FALSE ); 
  if ( op->exp->esucc != NULL ) return( FALSE );
  i2 = op->S_TEST->imp; 
  if ( i2->isucc != NULL ) return( FALSE );
  if ( IsConst( i2 ) ) return( FALSE );
  if ( !IsSGraph( i2->src ) ) return( FALSE );

  /* Select Test Optimization: if2aimp.c */
  /* if ( (t1 = op->usucc) == NULL ) return( FALSE ); */
  /* NEW CANN 2/92 */
  if ( (t1 = op->usucc) == NULL ) {
    if ( (ii = FindImport( op, i1->eport )) == NULL )
      return( FALSE );

    if ( IsConst( ii ) )
      return( FALSE );

    t1 = ii->src;

    if ( UsageCount( t1, ii->eport ) != 1 )
      return( FALSE );
    }


  i1 = op->S_CONS->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != c->iport ) return( FALSE );
  i1 = op->S_ALT->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != x->iport ) return( FALSE );

  op = t1;
  if ( IsSGraph( op ) ) return( FALSE );
  if ( op->type != IFLess ) return( FALSE ); /* > */
  if ( IsConst( op->imp )  ) return( FALSE );
  if ( IsConst( op->imp->isucc )  ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( IsSGraph( op->imp->isucc->src ) ) return( FALSE );

  switch ( op->imp->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* Abs ONE */
  if ( (abs1 = op->imp->src)->type != IFAbs ) return( FALSE );
  if ( IsConst( abs1->imp ) ) return( FALSE );
  if ( IsSGraph( abs1->imp->src ) ) return( FALSE );

  /* Abs TWO */
  if ( (abs2 = op->imp->isucc->src)->type != IFAbs ) return( FALSE );
  if ( IsConst( abs2->imp ) ) return( FALSE );
  if ( IsSGraph( abs2->imp->src ) ) return( FALSE );

  /* OptAElement ONE */
  if ( (n = abs1->imp->src)->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  i1 = n->imp;
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  n = abs2->imp->src;
  if ( n->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( i1->eport != n->imp->eport ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  return( TRUE );
}


static int IsTri( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
    case IF_DOUBLE:
    case IF_REAL:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */

  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFAGatherAT ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  /* TIMES */
  op = xx->src;
            /* COULD BE A MINUS-NOT IMPLEMENTED */
  if ( op->type != IFTimes ) return( FALSE ); 
  if ( IsConst( op->imp ) ) return( FALSE );
  if ( IsConst( op->imp->isucc ) ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( IsSGraph( op->imp->isucc->src ) ) return( FALSE );

  /* OptAElement ONE */
  if ( (n = op->imp->src)->type != IFOptAElement )
    if ( assoc )
      ImportSwap( op );
  if ( (n = op->imp->src)->type != IFOptAElement ) {
    return( FALSE );
    }

  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( n->imp->isucc->eport != c->iport ) return( FALSE );

  /* MINUS */
  op = op->imp->isucc->src;
  if ( op->type != IFMinus ) return( FALSE );
  if ( IsConst( op->imp ) ) return( FALSE );
  if ( IsConst( op->imp->isucc ) ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( !IsSGraph( op->imp->isucc->src ) ) return( FALSE );
  if ( op->imp->isucc->eport != xx->iport ) return( FALSE );

  /* OptAElement Two */
  op = op->imp->src;
  if ( op->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  return( TRUE );
}


static int IsFirstAbsMax( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PNODE t1;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;
  register PNODE abs1;
  register PNODE abs2;
  register PEDGE ii;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( !IsConst( c ) ) return( FALSE );
  if ( !IsConst( x ) ) return( FALSE );
  if ( atoi( c->CoNsT ) != (atoi( x->CoNsT ) + 1) ) return( FALSE );

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */

  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFFinalValue ) return( FALSE );
  if ( n->imp->isucc != NULL ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( n->imp->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  /* SELECT */
  op = xx->src;
  if ( op->type != IFSelect ) return( FALSE ); 
  if ( op->exp->esucc != NULL ) return( FALSE );
  i2 = op->S_TEST->imp; 
  if ( i2->isucc != NULL ) return( FALSE );
  if ( IsConst( i2 ) ) return( FALSE );
  if ( !IsSGraph( i2->src ) ) return( FALSE );

  /* Select Test Optimization: if2aimp.c */
  /* if ( (t1 = op->usucc) == NULL ) return( FALSE ); */
  /* NEW CANN 2/92 */
  if ( (t1 = op->usucc) == NULL ) {
    if ( (ii = FindImport( op, i1->eport )) == NULL )
      return( FALSE );

    if ( IsConst( ii ) )
      return( FALSE );

    t1 = ii->src;

    if ( UsageCount( t1, ii->eport ) != 1 )
      return( FALSE );
    }


  i1 = op->S_CONS->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != c->iport ) return( FALSE );
  i1 = op->S_ALT->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != x->iport ) return( FALSE );

  op = t1;
  if ( IsSGraph( op ) ) return( FALSE );
  if ( op->type != IFGreat ) return( FALSE ); /* > */
  if ( IsConst( op->imp )  ) return( FALSE );
  if ( IsConst( op->imp->isucc )  ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( IsSGraph( op->imp->isucc->src ) ) return( FALSE );

  switch ( op->imp->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* Abs ONE */
  if ( (abs1 = op->imp->src)->type != IFAbs ) return( FALSE );
  if ( IsConst( abs1->imp ) ) return( FALSE );
  if ( IsSGraph( abs1->imp->src ) ) return( FALSE );

  /* Abs TWO */
  if ( (abs2 = op->imp->isucc->src)->type != IFAbs ) return( FALSE );
  if ( IsConst( abs2->imp ) ) return( FALSE );
  if ( IsSGraph( abs2->imp->src ) ) return( FALSE );

  /* OptAElement ONE */
  if ( (n = abs1->imp->src)->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  i1 = n->imp;
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  n = abs2->imp->src;
  if ( n->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( i1->eport != n->imp->eport ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  return( TRUE );
}


static int IsFirstMax( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PNODE t1;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;
  register PEDGE ii;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( !IsConst( c ) ) return( FALSE );
  if ( !IsConst( x ) ) return( FALSE );
  if ( atoi( c->CoNsT ) != (atoi( x->CoNsT ) + 1) ) return( FALSE );

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */

  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFFinalValue ) return( FALSE );
  if ( n->imp->isucc != NULL ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( n->imp->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  /* SELECT */
  op = xx->src;
  if ( op->type != IFSelect ) return( FALSE ); 
  if ( op->exp->esucc != NULL ) return( FALSE );
  i2 = op->S_TEST->imp; 
  if ( i2->isucc != NULL ) return( FALSE );
  if ( IsConst( i2 ) ) return( FALSE );
  if ( !IsSGraph( i2->src ) ) return( FALSE );

  /* Select Test Optimization: if2aimp.c */
  /* if ( (t1 = op->usucc) == NULL ) return( FALSE ); */
  /* NEW CANN 2/92 */
  if ( (t1 = op->usucc) == NULL ) {
    if ( (ii = FindImport( op, i1->eport )) == NULL )
      return( FALSE );

    if ( IsConst( ii ) )
      return( FALSE );

    t1 = ii->src;

    if ( UsageCount( t1, ii->eport ) != 1 )
      return( FALSE );
    }


  i1 = op->S_CONS->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != c->iport ) return( FALSE );
  i1 = op->S_ALT->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != x->iport ) return( FALSE );

  op = t1;
  if ( IsSGraph( op ) ) return( FALSE );
  if ( op->type != IFGreat ) return( FALSE ); /* > */
  if ( IsConst( op->imp )  ) return( FALSE );
  if ( IsConst( op->imp->isucc )  ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( IsSGraph( op->imp->isucc->src ) ) return( FALSE );

  switch ( op->imp->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* OptAElement ONE */
  if ( (n = op->imp->src)->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  i1 = n->imp;
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  n = op->imp->isucc->src;
  if ( n->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( i1->eport != n->imp->eport ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  return( TRUE );
}

static int IsFirstSum( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */

  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFAGatherAT ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  op = xx->src;
  /* COULD BE A MINUS-NOT IMPLEMENTED */
  if ( op->type != IFPlus ) return( FALSE ); 
  if ( IsConst( op->imp ) ) return( FALSE );
  if ( !IsSGraph( op->imp->src ) ) return( FALSE );
  if ( op->imp->eport != xx->iport ) return( FALSE );

  if ( IsConst( op->imp->isucc ) ) return( FALSE );
  if ( (n = op->imp->isucc->src)->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  return( TRUE );
}


static int IsFirstMin( l )
PNODE l;
{
  register PEDGE i1;
  register PEDGE i2;
  register PNODE t1;
  register PEDGE c;
  register PEDGE cc;
  register PEDGE x;
  register PEDGE xx;
  register PNODE n;
  register PNODE inc;
  register PNODE op;
  register PEDGE ii;

  /* INITIAL CHECK */
  if ( (i1 = l->L_INIT->imp) == NULL ) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  /* TEST CHECK */
  if ( (n = l->L_TEST->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFLessEqual ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );

  c = NULL;
  x = NULL;

  if ( n->imp->eport == i1->iport ) {
    c = i1;
    x = i2;
  } else  {
    c = i2;
    x = i1;
    }

  if ( !IsConst( c ) ) return( FALSE );
  if ( !IsConst( x ) ) return( FALSE );
  if ( atoi( c->CoNsT ) != (atoi( x->CoNsT ) + 1) ) return( FALSE );

  if ( n->imp->eport != c->iport ) return( FALSE );

  if ( !IsConst( n->imp->isucc ) ) {
    if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
    if ( !IsImport( l, n->imp->isucc->eport ) ) return( FALSE );
    }

  switch ( c->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  switch ( x->info->type ) {
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* RETURN CHECK */
  if ( (n = l->L_RET->G_NODES) == NULL ) return( FALSE );
  if ( n->nsucc != NULL ) return( FALSE );
  if ( n->type != IFFinalValue ) return( FALSE );
  if ( n->imp->isucc != NULL ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( n->imp->eport != x->iport ) return( FALSE );

  /* BODY CHECK */
  if ( (i1 = l->L_BODY->imp) == NULL) return( FALSE );
  if ( (i2 = i1->isucc) == NULL ) return( FALSE );
  if ( IsConst( i1) ) return( FALSE );
  if ( IsConst( i2) ) return( FALSE );
  if ( i2->isucc != NULL ) return( FALSE );

  cc = NULL; xx = NULL;
  if ( i1->iport == c->iport ) cc = i1;
  if ( i1->iport == x->iport ) xx = i1;
  if ( i2->iport == c->iport ) cc = i2;
  if ( i2->iport == x->iport ) xx = i2;

  if ( cc == xx ) return( FALSE );

  /* CONTROL INCREMENT CHECK */
  inc = cc->src;
  if ( inc->type != IFPlus ) return( FALSE );
  if ( !IsConst( inc->imp->isucc ) ) return( FALSE );
  if ( atoi( inc->imp->isucc->CoNsT ) != 1 ) return( FALSE );
  if ( IsConst( inc->imp ) ) return( FALSE );
  if ( !IsSGraph( inc->imp->src ) ) return( FALSE );
  if ( inc->imp->eport != c->iport ) return( FALSE );

  /* OPERATION CHECK */
  /* SELECT */
  op = xx->src;
  if ( op->type != IFSelect ) return( FALSE ); 
  if ( op->exp->esucc != NULL ) return( FALSE );
  i2 = op->S_TEST->imp; 
  if ( i2->isucc != NULL ) return( FALSE );
  if ( IsConst( i2 ) ) return( FALSE );
  if ( !IsSGraph( i2->src ) ) return( FALSE );

  /* Select Test Optimization: if2aimp.c */
  /* if ( (t1 = op->usucc) == NULL ) return( FALSE ); */
  /* NEW CANN 2/92 */
  if ( (t1 = op->usucc) == NULL ) {
    if ( (ii = FindImport( op, i1->eport )) == NULL )
      return( FALSE );

    if ( IsConst( ii ) )
      return( FALSE );

    t1 = ii->src;

    if ( UsageCount( t1, ii->eport ) != 1 )
      return( FALSE );
    }


  i1 = op->S_CONS->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != c->iport ) return( FALSE );
  i1 = op->S_ALT->imp;
  if ( i1->isucc != NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( (i1 = FindImport( op, i1->eport )) == NULL ) return( FALSE );
  if ( IsConst( i1 ) ) return( FALSE );
  if ( !IsSGraph( i1->src ) ) return( FALSE );
  if ( i1->eport != x->iport ) return( FALSE );

  op = t1;
  if ( IsSGraph( op ) ) return( FALSE );
  if ( op->type != IFLess ) return( FALSE ); /* > */
  if ( IsConst( op->imp )  ) return( FALSE );
  if ( IsConst( op->imp->isucc )  ) return( FALSE );
  if ( IsSGraph( op->imp->src ) ) return( FALSE );
  if ( IsSGraph( op->imp->isucc->src ) ) return( FALSE );

  switch ( op->imp->info->type ) {
    case IF_DOUBLE:
    case IF_REAL:
    case IF_INTEGER:
      break;

    default:
      return( FALSE );
    }

  /* OptAElement ONE */
  if ( (n = op->imp->src)->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  i1 = n->imp;
  if ( n->imp->isucc->eport != cc->iport ) return( FALSE );

  n = op->imp->isucc->src;
  if ( n->type != IFOptAElement ) return( FALSE );
  if ( IsConst( n->imp ) ) return( FALSE );
  if ( IsConst( n->imp->isucc ) ) return( FALSE );
  if ( !IsSGraph( n->imp->src ) ) return( FALSE );
  if ( !IsSGraph( n->imp->isucc->src ) ) return( FALSE );
  if ( !IsImport( l, n->imp->eport ) ) return( FALSE );
  if ( i1->eport != n->imp->eport ) return( FALSE );
  if ( n->imp->isucc->eport != x->iport ) return( FALSE );

  return( TRUE );
}


static void Vectorize( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	Vectorize( sg );

    if ( !IsLoopB( n ) )
      continue;

    if ( IsFirstMin( n ) ) {
      n->type = IFFirstMin;
      fmin++;
      continue;
      }
    else if ( IsFirstAbsMin( n ) ) {
      n->type = IFFirstAbsMin;
      famin++;
      continue;
      }
    else if ( IsFirstMax( n ) ) {
      n->type = IFFirstMax;
      fmax++;
      continue;
      }
    else if ( IsFirstAbsMax( n ) ) {
      n->type = IFFirstAbsMax;
      famax++;
      continue;
      }
    else if ( IsFirstSum( n ) ) {
      n->type = IFFirstSum;
      fsum++;
      continue;
      }
    else if ( IsTri( n ) ) {
      n->type = IFTri;
      tri++;
      continue;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************     If2Vectorize          ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO IDENTIFY THE NONSTANDARD VECTOR LOOPS SUCH AS      */
/*          First Min, First Max, First Absolute Min, First Absolute Max, */
/*          First Sum AND Tri-Diagonal Elimination.                        */
/**************************************************************************/

void If2Vectorize( useF )
int useF;
{
  register PNODE f;
  

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    if ( vec || useF )
      Vectorize( f );

/*  if ( RequestInfo(I_Info4, info) )
    WriteLoopMap( " **** LOOP MAP AFTER CONCURRENTIZATION AND VECTOR RECOMENDATION" ); */

}


/**************************************************************************/
/* GLOBAL **************  VECTORIZATION DIRECTIVES ************************/
/**************************************************************************/
/* PURPOSE: ROUTINES TO PRINT MACHINE SPECIFIC VECTORIZATION DIRECTIVES.  */
/*            PrintNOVECTOR: TO DISABLE VECTORIZATION OF NEXT LOOP.       */
/*            PrintVECTOR:   TO ENABLEE VECTORIZATION OF NEXT LOOP.       */
/*            PrintASSOC:    TO ENABLEE ASSOCIATIVE TRANSFORMATIONS IN    */
/*                           THE NEXT LOOP.                               */
/*            PrintSAFE:     IN THE NEXT LOOP, POINTER nm IS NOT PART OF  */
/*                           AN ALIAS.                                    */
/**************************************************************************/

void PrintNOVECTOR()
{
  if ( fvc ) {
    if ( nltss )
      FPRINTF( output, "#pragma hazard\n" );
    else
      FPRINTF( output, "#pragma _CRI novector\n" );
    }
  else if ( fva )
    FPRINTF( output, "#pragma novector\n" );
  else
    FPRINTF( output, "/* _NOVECTOR_ */\n" );
}

void PrintVECTOR()
{
  if ( fvc ) {
    if ( nltss )
      FPRINTF( output, "#pragma nohazard\n" );
    else
      FPRINTF( output, "#pragma _CRI ivdep\n" );
    }
  else if ( fva )
    FPRINTF( output, "#pragma vector\n" );
  else
    FPRINTF( output, "/* _VECTOR_ */\n" );
}

void PrintASSOC()
{
  if ( fvc )
    FPRINTF( output, "/* _ASSOC_ */\n" );
  else if ( fva )
    FPRINTF( output, "#pragma assoc\n" );
  else
    FPRINTF( output, "/* _ASSOC_ */\n" );
}

void PrintSAFE( nm )
char *nm;
{
  if ( fvc )
    FPRINTF( output, "/* _SAFE(%s) */\n", nm );
  else if ( fva )
    FPRINTF( output, "#pragma safe (%s)\n", nm );
  else
    FPRINTF( output, "/* _SAFE(%s) */\n", nm );
}
