/* if2migrate.c,v
 * Revision 12.7  1992/11/04  22:05:11  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define MAX_IMP 500

static PEDGE imp[MAX_IMP];
static int   limp[MAX_IMP];


static int MaxPathToSrc( i )
PEDGE i;
{
  register PEDGE ii;
  register int   ml;
  register int   l;

  if ( i == NULL ) return( 0 );

  if ( IsConst( i ) ) return( 0 );
  if ( IsGraph( i->src ) ) return( 0 );
  if ( IsCompound( i->src ) ) return( 0 );

  /* WARNING: THIS ROUTINE USES pl (assumed initialized to -2) */
  if ( i->src->pl >= 0 ) return( i->src->pl );

  for ( ml = 0, ii = i->src->imp; ii != NULL; ii = ii->isucc ) {
    l = MaxPathToSrc( ii );

    if ( l > ml ) ml = l;
    }

  return( (i->src->pl = ml + 1) );
}


/**************************************************************************/
/* LOCAL  **************       MigrageNodes        ************************/
/**************************************************************************/
/* PURPOSE: MIGRATE THE NODES OF GRAPH g TOWARD THEIR DEPENDENTS. THIS    */
/*          ROUTINE ASSUMES g'S NODE LIST IS DATA FLOW ORDERED.           */
/**************************************************************************/

static void MigrateNodes( g )
PNODE g;
{
    register int   l  = 0;
    register PNODE n  = g->G_NODES;
    register PEDGE i;
    register PEDGE e;
    register PADE  a;
    register PNODE sg;
    register PNODE prd;
    register int   j,k,s,t;

    /* RELABEL ALL NODES AND REMEMBER THE LAST NODE IN THE NODE LIST      */

    if ( n == NULL )
	return;

    for ( ;; ) {
        n->label = ++l;

	if ( n->nsucc == NULL )
	    break;

	n = n->nsucc;
	}

    g->label = 0;


    for ( /* NOTHING */; n != g; n = n->npred ) {
	prd = n->npred;

	/* NEW CANN 10/2 */
	if ( IsGraph( prd ) )
	    continue;
        /* END NEW SECTION */

	if ( IsCompound( n ) )
	    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
		MigrateNodes( sg );

	/* RUN IMPORTS RIGHT TO LEFT (FAVOR AElements): 10/9 CANN */
	for ( j = -1, i = n->imp; i != NULL; i = i->isucc ) {
	  if ( IsConst( i ) ) continue;
	  if ( IsGraph( i->src ) ) continue;

	  if ( (++j) >= MAX_IMP ) 
	    Error2( "MigrateNodes", "imp and limp OVERFLOW" );

	  limp[j] = MaxPathToSrc( i );
	  imp[j]  = i;
	  }

        do {
          s = 1;
          for ( k = 0; k <= j - 1; k++ )
            if ( limp[k] > limp[k+1] ) {
               t = limp[k];
               limp[k] = limp[k+1];
               limp[k+1] = t;
        
               e = imp[k];
               imp[k] = imp[k+1];
               imp[k+1] = e;
        
               s = 0;
               }
           }
        while (!s);

	for ( ; j >= 0; j-- ) {
	    i = imp[j];

	    /* IS i->src MORE THAN ONE HOP AWAY?                          */
	    if ( i->src->label >= prd->label )
		continue;

	    /* MAKE SURE MOVING i->src DOES NOT VIOLATE ANY DEPENDENCIES  */

	    for ( e = i->src->exp; e != NULL; e = e->esucc ) {
		if ( IsGraph( e->dst ) )
		    continue;

		if ( e->dst->label <= prd->label )
		    goto MoveOn;
		}

	    for ( a = i->src->aexp; a != NULL; a = a->esucc ) {
		if ( a->dst->label <= prd->label )
		    goto MoveOn;
		}

	    sg = i->src->npred;

	    UnlinkNode( i->src );
	    LinkNode( prd, i->src ); cmig++;

	    /* NEW 10/2 CANN */
	    prd = i->src;

	    for ( l = sg->label; sg != n; sg = sg->nsucc ) {
                sg->label = l++;
		}
	    /* END NEW SECTION */
MoveOn:
	    continue;
	    }
	}
}


static void DoCommute( f )
PNODE f;
{
    register PNODE n;
    register PNODE sg;
    register PEDGE i1, i2;

    for ( n = f->G_NODES; n != NULL; n = n->nsucc ) {
	if ( IsCompound( n ) )
	    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
		DoCommute( sg );

	switch ( n->type ) {
	    case IFTimes:
	    case IFPlus:
		if ( IsConst( n->imp ) )
		    break;

		if ( IsGraph( n->imp->src ) )
		    break;

		if ( n->imp->src->exp->esucc != NULL )
		    break;

                if ( n->imp->src->type == IFAElement ) {
		    i1 = n->imp->isucc;
		    i2 = n->imp;

		    i1->iport = 1;
		    i1->isucc = i2;
		    i2->iport = 2;
		    i2->isucc = NULL;
		    n->imp = i1;
		    }

                break;

	    default:
	        break;
            }
        }
}


/**************************************************************************/
/* GLOBAL **************         If2Migrate        ************************/
/**************************************************************************/
/* PURPOSE: MIGRATE THE NODES OF ALL FUNCTION GRAPHS TOWARD THEIR USES.   */
/**************************************************************************/

void If2Migrate()
{
    register PNODE f;

    for ( f = fhead; f != NULL; f = f->gsucc ) {
        MigrateNodes( f );

	if ( seqimp )
	    DoCommute( f );
        }
}
