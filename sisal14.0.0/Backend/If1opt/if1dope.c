/* if1dope.c,v
 * Revision 12.7  1992/11/04  22:04:56  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:32  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define MAX_SCOPE 200

static int dcnt = 0;                   /* COUNT OF ALLOCATED DOPE VECTORS */
static int Tdcnt = 0;                   /* COUNT OF VECTORS */
static int liml  = 0;                  /* COUNT OF OPTIMIZED limlS        */
static int Tliml  = 0;                  /* COUNT OF limlS        */
static int limh  = 0;                  /* COUNT OF OPTIMIZED limhS        */
static int Tlimh  = 0;                  /* COUNT OF limhS        */
static int size  = 0;                  /* COUNT OF OPTIMIZED sizeS        */
static int Tsize  = 0;                  /* COUNT OF sizeS        */
static int aelmc = 0;                  /* COUNT OF AELEMENT INSERTIONS    */
static int pcyc  = 0;                  /* COUNT OF CARRIED PROPAGATIONS   */
static int Tpcyc  = 0;                  /* COUNT OF PROPAGATIONS   */

static PNODE scope[MAX_SCOPE];         /* SCOPE STACK FOR THREADING EDGES */


/**************************************************************************/
/* LOCAL  **************       AddAElement         ************************/
/**************************************************************************/
/* PURPOSE: ADD AN AELEMENT NODE TO FORALL BODY b THAT DEREFERENCES a     */
/*          (AN ARRAY IMPORT TO THE FORALL NODE) USING i+c AS THE INDEX.  */
/**************************************************************************/

static void AddAElement( i, c, b, a )
PEDGE i;
int   c;
PNODE b;
PEDGE a;
{
  register PNODE plus;
  register PNODE aelm;
  register PEDGE e;
  register PINFO iinfo;
	   char  buf[100];

  iinfo = (IsMultiple(i->info))? i->info->A_ELEM : i->info;

  aelmc++;

  if ( c < 0 ) {
    c = -c;
    plus = NodeAlloc( ++maxint, IFMinus );
    }
  else
    plus = NodeAlloc( ++maxint, IFPlus );

  CopyVitals( b, plus );

  LinkNode( b, plus );

  aelm = NodeAlloc( ++maxint, IFAElement );
  LinkNode( plus, aelm );
  CopyVitals( b, aelm );

  e = EdgeAlloc( plus, 1, aelm, 2 );
  e->info = iinfo;
  LinkExport( plus, e );
  LinkImport( aelm, e );

  e = EdgeAlloc( NULL_NODE, CONST_PORT, plus, 2 );
  e->info = iinfo;
  SPRINTF( buf, "%d", c );
  e->CoNsT = CopyString( buf );
  LinkImport( plus, e );

  e = EdgeAlloc( b, i->iport, plus, 1 );
  e->info = iinfo;
  LinkExport( b, e );
  LinkImport( plus, e );

  e = EdgeAlloc( b, a->iport, aelm, 1 );
  e->info = a->info;
  LinkExport( b, e );
  LinkImport( aelm, e );

  e = EdgeAlloc( aelm, 1, b, ++maxint );
  e->info = a->info->A_ELEM;
  LinkExport( aelm, e );
  LinkImport( b, e );
}


/**************************************************************************/
/* LOCAL  **************        AreDopesEqual      ************************/
/**************************************************************************/
/* PURPOSE: THIS ROUTINE ASSUMES e1 AND e2 ARE PART OF A LOOP CARRIED     */
/*          COMPUTATION, AND E1 IS THE INITIAL SUBGRAPH IMPORT AND e2 IS  */
/*          THE LOOP BODY IMPORT.  TWO IS RETURNED IF THE DOPES OF e1     */
/*          AND e2 ARE EQUAL.  ONE IS RETURNED IF THE LOWER BOUND PART    */
/*          OF E1 AND E2 ARE EQUAL. ELSE ZERO IS RETURNED.                */
/**************************************************************************/

static int AreDopesEqual( e1, e2 )
PEDGE e1;
PEDGE e2;
{
  register PDOPE d1;
  register PDOPE d2;
  register PNODE LimLNode;
  register PNODE LimHNode;

  if ( e1->dope == NULL || e2->dope == NULL )
    return( 0 );

  d1 = e1->dope;
  d2 = e2->dope;

  /* PROCESS THE LOWER BOUND */

  if ( d1->lo == NULL || d2->lo == NULL )
    return( 0 );

  if ( !(IsConst( d1->lo ) ^ IsConst( d2->lo )) ) { /* XOR */
    /* TRY #1 BOTH CONSTANTS? */
    if ( IsConst( d1->lo ) && IsConst( d2->lo ) ) {
      if ( (atoi(d1->lo->CoNsT) - d1->dec) != (atoi(d2->lo->CoNsT) - d2->dec) )
        return( 0 );

      goto MoveOn;
      }

    /* TRY #2 */
    if ( AreValuesEqual( d1->lo, d2->lo ) && d1->dec == d2->dec )
      goto MoveOn;
    }

  /* TRY #3 */
  if ( IsConst( d2->lo ) )
    return( 0 );

  LimLNode = d2->lo->src;

  if ( !IsALimL( LimLNode ) )
    return( 0 );

  if ( IsConst( LimLNode->imp ) )
    return( 0 );

  if ( !IsSGraph( LimLNode->imp->src ) )
    return( 0 );

  if ( LimLNode->imp->src->G_DAD != e1->dst->G_DAD )
    return( 0 );

  if ( LimLNode->imp->src->G_DAD->L_BODY != LimLNode->imp->src )
    return( 0 );

  if ( LimLNode->imp->eport != e2->iport )
    return( 0 );

  if ( d2->dec != 0 )
    return( 0 );

MoveOn:
  /* PROCESS THE UPPER BOUND */

  if ( !(IsConst( d1->hi ) ^ IsConst( d2->hi )) ) { /* XOR */
    /* TRY #1 */
    if ( IsConst( d1->hi ) && IsConst( d2->hi ) ) {
      if ( (atoi(d1->hi->CoNsT) - d1->inc) != (atoi(d2->hi->CoNsT) - d2->inc) )
        return( 1 );

      return( 2 );
      }

    /* TRY #2 */
    if ( AreValuesEqual( d1->hi, d2->hi ) && d1->inc == d2->inc )
      return( 2 );
    }

  /* TRY #3 */
  if ( IsConst( d2->hi ) )
    return( 1 );

  LimHNode = d2->hi->src;

  if ( !IsALimH( LimHNode ) )
    return( 1 );

  if ( IsConst( LimHNode->imp ) )
    return( 1 );

  if ( !IsSGraph( LimHNode->imp->src ) )
    return( 1 );

  if ( LimHNode->imp->src->G_DAD != e1->dst->G_DAD )
    return( 1 );

  if ( LimHNode->imp->src->G_DAD->L_BODY != LimHNode->imp->src )
    return( 1 );

  if ( LimHNode->imp->eport != e2->iport )
    return( 1 );

  if ( d2->inc != 0 )
    return( 1 );

  return( 2 );
} 


/**************************************************************************/
/* LOCAL  **************         DopeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A DOPE VECTOR.               */
/**************************************************************************/

static PDOPE DopeAlloc()
{
  register PDOPE d;

  d = (PDOPE) MyAlloc( (int) sizeof(DOPE) );

  d->inc = 0;
  d->dec = 0;
  d->lo  = NULL;
  d->hi  = NULL;

  dcnt++;
  return( d );
}


/**************************************************************************/
/* LOCAL  **************      BindDopeInfo         ************************/
/**************************************************************************/
/* PURPOSE: BIND DOPE INFORMATION d TO ALL EXPORTS OF n WITH EXPORT PORT  */
/*          NUMBER eport.                                                 */
/**************************************************************************/

static void BindDopeInfo( n, eport, d )
PNODE n;
int   eport;
PDOPE d;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc )
    if ( e->eport == eport )
      e->dope = d;
}


/**************************************************************************/
/* LOCAL  **************    PropagateDopeInfo      ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE DOPE VECTOR INFORMATION IMPORTED TO COMPOUND c  */
/*          TO ITS SUBGRAPH sg.                                           */
/**************************************************************************/

static void PropagateDopeInfo( c, sg )
PNODE c;
PNODE sg;
{
  register PEDGE i;
  register PEDGE e;
  register PDOPE d;

  for ( i = c->imp; i != NULL; i = i->isucc ) {
    if ( i->dope == NULL )
      continue;

    ++Tdcnt;

    if ( !IsExport( sg, i->iport ) )
      continue;

    d = DopeAlloc();
    *d = *(i->dope);

    for ( e = sg->exp; e != NULL; e = e->esucc ) {
      if ( e->eport != i->iport ) 
	continue;

      e->dope = d;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************        ObviateNode        ************************/
/**************************************************************************/
/* PURPOSE: REPLACE NODE n BY i + v.  IF i IS NOT A CONSTANT THEN v MUST  */
/*          BE ZERO. THE SCOPE STACK IS USED TO THREAD THE REFERENCE. N   */
/*          IS AT LEVEL lvl AND i IS AT LEVEL i->lvl.                     */
/**************************************************************************/

static int ObviateNode( n, lvl, i, v )
PNODE  n;
int    lvl;
PEDGE  i;
int    v;
{
  register PEDGE e;
  register PEDGE se;
	   char  buf[100];
  register int   eport;
  register PNODE src;
  register PEDGE ee;
  register PEDGE newee;
  register PNODE newsrc;
  register int   l;

  if ( i == NULL )
    Error2( "ObviateNode", "i IS NULL" );

  if ( (!IsConst( i )) && v != 0 )
    return( FALSE );

  if ( IsConst( i ) ) {
    for ( e = n->exp; e != NULL; e = se ) {
      se = e->esucc;

      ChangeEdgeToConst( e, i );

      if ( v != 0 ) {
        SPRINTF( buf, "%d", atoi(e->CoNsT) + v );
        e->CoNsT = CopyString( buf );
        }
      }

    OptRemoveDeadNode( n );
    return( TRUE );
    }

  /* THREAD REFERENCE TO i TO n->exp->dsts */

  eport = i->eport;
  src   = i->src;
  ee    = i;

  for ( l = i->lvl + 1; l <= lvl; l++ ) {
    if ( (newsrc = scope[l]) == NULL )
      Error2( "ObviateNode", "SCOPE STACK FAULT" );

    newee = EdgeAlloc( src, eport, newsrc->G_DAD, ++maxint );
    newee->info = ee->info;

    LinkExport( src, newee );
    LinkImport( newsrc->G_DAD, newee );

    eport = maxint;

    ee  = newee;
    src = newsrc;
    }

  for ( e = n->exp; e != NULL; e = e->esucc )
    e->eport = eport;

  LinkExportLists( src, n );

  OptRemoveDeadNode( n );
  return( TRUE );
}


/**************************************************************************/
/* GLOBAL **************     ProcessDopeInfo       ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN ARRAY DOPE VECTOR INFORMATION TO SELECTIVE EXPORTS AND */
/*          DO THE APPROPRIATE OPTIMIZATIONS BASED ON THE ASSIGNED INFO.  */
/*          Lvl DEFINES THE CURRENT SCOPE LEVEL FOR GRAPH g.              */
/**************************************************************************/

static void ProcessDopeInfo( g, lvl )
PNODE g;
int   lvl;
{
  register PNODE n;
  register PNODE sn;
  register PNODE sg;
  register PDOPE d;
  register PDOPE dd;
  register PEDGE i;
  register PEDGE ii;
  register PEDGE e;
  register int   c;
  register int   dlow;
  register int   dhigh;
  register PNODE l;
	   char  buf[100];

  if ( lvl >= MAX_SCOPE )
    Error2( "ProcessDopeInfo", "SCOPE STACK OVERFLOW" );

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) ) {
      switch ( n->type ) {
	case IFSelect:
	  PropagateDopeInfo( n, n->S_CONS );
	  PropagateDopeInfo( n, n->S_ALT );
	  break;

	case IFForall:
	  PropagateDopeInfo( n, n->F_BODY );
	  break;

	case IFLoopA:
	case IFLoopB:
	  PropagateDopeInfo( n, n->L_INIT );
	  PropagateDopeInfo( n, n->L_BODY );
	  break;

        case IFTagCase:
          break;

        case IFUReduce:		/* TBD: how is this used? */
	  PropagateDopeInfo( n, n->R_INIT );
	  PropagateDopeInfo( n, n->R_BODY );
          break;

	default:
          UNEXPECTED("Unknown compound");
        }

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
	scope[lvl+1] = sg;
	ProcessDopeInfo( sg, lvl+1 );
	}

      switch ( n->type ) {
	case IFLoopA:
	case IFLoopB:
	  for ( c = FALSE, i = n->L_INIT->imp; i != NULL; i = i->isucc ) {
	    if ( (ii = FindImport( n->L_BODY, i->iport )) == NULL )
	      continue;

            ++Tpcyc;

	    switch ( AreDopesEqual( i, ii ) ) {
	      case 0:
		break;

	      case 1:
		c = TRUE;
		pcyc++;

		/* JUST THE LOWER BOUND IS EQUAL */
	        d = DopeAlloc();
	        d->lo  = i->dope->lo;
		d->dec = i->dope->dec;
	        BindDopeInfo( n->L_BODY, i->iport, d );
		break;

	      case 2:
	        c = TRUE;
	        pcyc++;

		/* BOTH THE BOUNDS ARE EQUAL */
	        BindDopeInfo( n->L_BODY, i->iport, i->dope );
		break;

	      default:
		Error2( "ProcessDopeInfo", "AreDopesEqual RETURN VALUE ERROR" );
              }
	    }

          if ( c )
	    ProcessDopeInfo( n->L_BODY, lvl+1 );

	  break;

	default:
	  break;
        }
      }

    switch ( n->type ) {
      case IFABuild:
	if ( !IsArray( n->exp->info ) ) break;

	for ( c = 0, i = n->imp->isucc; i != NULL; i = i->isucc )
	  c++;

	d = DopeAlloc();
	d->lo = n->imp;

	n->imp->lvl = lvl;

	if ( IsConst(d->lo) ) {
	  d->hi = EdgeAlloc( NULL_NODE, CONST_PORT, NULL_NODE, CONST_PORT );
	  d->hi->info = d->lo->info;
	  SPRINTF( buf, "%d", atoi(d->lo->CoNsT) + c - 1 );
	  d->hi->CoNsT = CopyString( buf );
	  d->hi->lvl = lvl;
	  }

	BindDopeInfo( n, 1, d );
	break;

      case IFAGather:
	if ( !IsArray( n->exp->info ) ) break;

	/* IS THERE A FILTER? */
	if ( n->imp->isucc->isucc != NULL )
	  break;

	l = n->exp->dst->G_DAD;

	/* FOR FORALL NODES ONLY!!! */
	if ( !IsForall( l ) )
	  break;

	/* FROM A RangeGenerate NODE? */
	if ( (dd = l->F_GEN->imp->dope) == NULL )
	  break;
        
	if ( !AreValuesEqual( n->imp, dd->lo ) )
	  break;

	/* SKIP for i in 6,5 do */
	if ( IsConst(dd->lo) && IsConst(dd->hi) )
	  if ( atoi(dd->lo->CoNsT) > atoi(dd->hi->CoNsT) )
	    break;

	d = DopeAlloc();
	*d = *dd;

	BindDopeInfo( l, n->exp->iport, d );
	break;

      case IFRangeGenerate:
	/* IS n THE CONTROL? */
	if ( n->exp != n->exp->dst->imp )
	  break;

	/* IS n THE ONLY F_GEN NODE? */
	if ( n->nsucc != NULL )
	  break;

	l = n->exp->dst;

	d = DopeAlloc();

	d->lo = (IsConst(n->imp))? n->imp : 
		        FindImport( l->G_DAD, n->imp->eport );

	if ( d->lo == NULL )
	  Error2( "ProcessDopeInfo", "FindImport FAILURE" );

	d->hi = (IsConst(n->imp->isucc))? n->imp->isucc : 
		        FindImport( l->G_DAD, n->imp->isucc->eport );

	if ( d->hi == NULL )
	  Error2( "ProcessDopeInfo", "FindImport FAILURE" );

	d->lo->lvl = lvl-1; /* THE SCOPE CONTAINING l */
	d->hi->lvl = lvl-1;

	BindDopeInfo( n, 1, d );
	break;

      case IFAReplace:
	if ( (dd = n->imp->dope) == NULL )
	  break;

	d = DopeAlloc();
	*d = *dd;

	BindDopeInfo( n, 1, d );
	break;

      case IFAAddH:
	if ( (dd = n->imp->dope) == NULL )
	  break;

	if ( dd->hi == NULL )
	  break;

	d = DopeAlloc();
	*d = *dd;
	d->inc++;

	BindDopeInfo( n, 1, d );
	break;

      case IFAAddL:
	if ( (dd = n->imp->dope) == NULL )
	  break;

	if ( dd->lo == NULL )
	  break;

	d = DopeAlloc();
	*d = *dd;
	d->dec++;

	BindDopeInfo( n, 1, d );
	break;

      case IFALimL:

	++Tliml;

	if ( (d = n->imp->dope) == NULL )
	  break;

	if ( d->lo == NULL )
	  break;
    
	if ( ObviateNode( n, lvl, d->lo, -(d->dec) ) ) {
	  liml++;
	  }

	break;

      case IFALimH:

	++Tlimh;

	if ( (d = n->imp->dope) == NULL )
	  break;

	if ( d->lo == NULL || d->hi == NULL )
	  break;

	/* THEY BOTH MUST BE CONSTANTS BECAUSE OF 5,<4 GENERATE PROBLEM  */
	/* THE HIGH BOUND IS ACTUALLY 4!!! THIS SCREWS UP LOTS OF STUFF  */
	/* STUFF...UGH!!!  HOWEVER, IT IS OK IN THE FOLLOWING SITUATION: */
	/*                                                               */
	/*   for i in liml(x),limh(x) ... where both liml and limh have  */
	/*   the same dope descriptor!!!!                                */
	/*                                                               */
	/* THIS IS NOT CURRENTLY IMPLEMENTED!                            */

	if ( !(IsConst(d->lo) && IsConst(d->hi)) ) {
	  break;
	  }

	if ( ObviateNode( n, lvl, d->hi, d->inc ) ) {
	  limh++;
	  }

	break;

      case IFASize:
        ++Tsize;

	if ( (d = n->imp->dope) == NULL )
	  break;

	if ( d->lo == NULL || d->hi == NULL )
	  break;

	/* THEY BOTH MUST BE CONSTANTS!!! */
	if ( !(IsConst(d->lo) && IsConst(d->hi)) )
	  break;

	if ( ObviateNode( n, lvl, d->hi, 
			  d->inc + d->dec + 1 - atoi(d->lo->CoNsT) ) ) {
	  size++;
	  }

	break;

      case IFForall:
	if ( !tgcse )
	  break;

	/* OPEN UP OPPORTUNITIES FOR GCSE */
	i = n->F_GEN->imp;

	if ( (d = i->dope) == NULL )
	  break;
        if ( d->lo == NULL || d->hi == NULL )
	  break;

	for ( ii = n->imp; ii != NULL; ii = ii->isucc ) {
	  if ( (dd = ii->dope) == NULL )
	    continue;

	  if ( dd->lo == NULL || dd->hi == NULL )
	    continue;

	  if ( !IsExport( n->F_BODY, ii->iport ) )
	    continue;

	  /* BE CAREFUL NOT TO ADD UNNECESSARY OVERHEAD!!! */
	  for ( e = n->F_BODY->exp; e != NULL; e = e->esucc )
	    if ( e->eport == ii->iport )
	      if ( IsSelect( e->dst ) || IsTagCase( e->dst ) )
		break;

	  if ( e != NULL ) {
	    continue;
	    }

	  /* MAKE SURE THE OPTIMIZATION COULD RESULT IN INVARIANT */
	  /* REMOVAL!                                             */
	  for ( e = n->F_BODY->exp; e != NULL; e = e->esucc ) {
	    if ( e->eport != ii->iport )
	      continue;

	    if ( IsForall( e->dst ) ) {
	      if ( IsExport( e->dst->F_BODY, e->iport ) )
	        break;
	      }

	    if ( IsLoop( e->dst ) ) {
	      if ( IsExport( e->dst->L_BODY, e->iport ) )
	        break;
	      }
	    }

	  if ( e == NULL ) {
	    continue;
	    }

	  if ( AreValuesEqual( d->lo, dd->lo ) && 
	       AreValuesEqual( d->hi, dd->hi ) &&
	       d->dec == dd->dec && d->inc == dd->inc  ) {
	    dlow  = 0;
	    dhigh = 0;
            }
	  else if ( IsConst( d->lo ) && IsConst( dd->lo ) &&
		    IsConst( d->hi ) && IsConst( dd->hi ) ) {
	    /* d IS THE CONTROL!!! */
	    dlow = (atoi(dd->lo->CoNsT) - dd->dec) -
                   (atoi(d->lo->CoNsT)  - d->dec);

	    if ( dlow > 0 )
	      continue;

	    if ( dlow < -5 ) 
	      dlow = -5;

	    /* d IS THE CONTROL!!! */
	    dhigh = (atoi(dd->hi->CoNsT) + dd->inc) -
                    (atoi(d->hi->CoNsT)  + d->inc);

	    if ( dhigh < 0 )
	      continue;

	    if ( dhigh > 5 )
	      dhigh = 5;
	    }
	  else
	    continue;

	  for ( c = 0; c >= dlow; c-- )
	    AddAElement( i, c, n->F_BODY, ii );

	  for ( c = 1; c <= dhigh; c++ )
	    AddAElement( i, c, n->F_BODY, ii );
	  }

	break;

      default:
	break;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************           If1Dope         ************************/
/**************************************************************************/
/* PURPOSE: DO ALL THE DOPE VECTOR OPTIMIZATIONS.  NOTE THAT EDGE FIELD   */
/*          dope IS ONLY VALID WITHIN THE CONFINES OF If1Dope. FURTHER    */
/*          If1Dope CANULD ONLY BE CALLED ONCE DURING if1opt EXECUTION.   */
/**************************************************************************/

void If1Dope()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    ProcessDopeInfo( f, 0 );
    }
}


/**************************************************************************/
/* GLOBAL **************      WriteDopeInfo        ************************/
/**************************************************************************/
/* PURPOSE: WRITE DOPE VECTOR OPTIMIZATION INFORMATION TO stderr.         */
/**************************************************************************/

void WriteDopeInfo()
{
    FPRINTF( infoptr, "\n **** DOPE VECTOR OPTIMIZATIONS\n\n" );

    FPRINTF( infoptr, " Allocated Dope Vectors:  %d of %d\n", dcnt,Tdcnt );
    FPRINTF( infoptr, " Optimized ALimL Nodes:   %d of %d\n", liml,Tliml );
    FPRINTF( infoptr, " Optimized ALimH Nodes:   %d of %d\n", limh,Tlimh );
    FPRINTF( infoptr, " Optimized ASize Nodes:   %d of %d\n", size,Tsize );
    FPRINTF( infoptr, " Inserted AElement Nodes: %d\n", aelmc);
    FPRINTF( infoptr, " Carried Propagations:    %d of %d\n", pcyc,Tpcyc );
}
