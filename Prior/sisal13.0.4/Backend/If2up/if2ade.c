/* if2ade.c,v
 * Revision 12.7  1992/11/04  22:05:10  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:00  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************      FindLGenerator       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NODE REDEFINING A L PORT VALUE IN LOOP BODY GRAPH  */
/*          g THAT HAS COLOR color.  IF SUCH A NODE IS NOT FOUND, AN      */
/*          ERROR MESSAGE IS PRINTED.                                     */
/**************************************************************************/

static PNODE FindLGenerator( g, SearchColor )
PNODE g;
int   SearchColor;
{
    register PEDGE i;

    for ( i = g->imp; i != NULL; i = i->isucc ) {
	if ( IsConst( i ) )
	    continue;
 
	if ( IsSGraph( i->src ) )
	    continue;

        if ( i->src->color == SearchColor )
	    return( i->src );
        }

    Error1( "FindLGenerator: L GENERATOR NOT FOUND" );
    return NULL;
}


/**************************************************************************/
/* LOCAL  **************   OrderLPortGenerators    ************************/
/**************************************************************************/
/* PURPOSE: ORDER L VALUE GENERATORS IN ALL Loop[AB] BODY SUBGRAPHS TO    */
/*          REDUCE COPYING OF LOOP CARRIED SCALAR VALUES. AN L VALUE      */
/*          GENERATOR IS A NODE WHOSE EXPORT REDEFINES A LOOP CARRIED     */
/*          L PORT VALUE, SUCH AS * IN  l := (old l + 1) * 40.            */
/*          FIRST ALL NODES CONTRIBUTING TO THE REDEFINITION OF AN L PORT */
/*          VALUE ARE PAINTED THE SAME COLOR; A NODE IS ONLY PAINTED IF   */
/*          IT IS WHITE (THE INITIAL COLOR).  GIVEN NODE a IS COLORED     */
/*          c1 AND NODE b IS COLORED c2, WHERE c1 != c2, a AND b REDEFINE */
/*          LOOP CARRIED L PORT VALUES l1 AND l2 RESPECTIVELY, l2 IS      */ 
/*          REFERENCED BY NODE x (POSSIBLY a) WITH COLOR c1, AND l1 IS    */
/*          REFERENCED BY NODE y (POSSIBLY b) WITH COLOR c2, THEN AN ADE  */
/*          IS INSERTED WITH SOURCE NODE a AND DESTINATION NODE b. THE    */
/*          ADE IS GIVEN A HIGH REMOVAL PRIORITY.                         */
/**************************************************************************/

static void OrderLPortGenerators()
{
    register PNODE c;
    register PNODE n;
    register PEDGE i;
    register PEDGE e;
    register int   NodeColor;

    for ( c = chead; c != NULL; c = c->usucc ) {
	if ( !IsLoop( c ) )
	    continue;

	/*  PAINT ALL THE NODES IN THE LOOP BODY                          */

        NodeColor = WHITE + 1;

	for ( i = c->L_BODY->imp; i != NULL; i = i->isucc )
	    if ( !IsConst( i ) )
	        if ( !IsSGraph( i->src ) )
		    i->src->color = NodeColor++;
        
	for ( n = FindLastNode( c->L_BODY ); !IsSGraph( n ); n = n->npred ) {
	    if ( n->color == WHITE )
		n->color = NodeColor++;

            for ( i = n->imp; i != NULL; i = i->isucc )
		if ( !IsConst( i ) )
		    if ( !IsSGraph( i->src ) )
		        if ( i->src->color == WHITE )
			    i->src->color = n->color;
	    }

	/* DRIVEN BY THE NODE COLORING, ORDER THE NODES TO REDUCE COPYING */

        for ( i = c->L_BODY->imp; i != NULL; i = i->isucc ) {
	    if ( IsConst( i ) )
		continue;

            if ( IsSGraph( i->src ) )
		continue;

            for ( e = c->L_BODY->exp; e != NULL; e = e->esucc ) {
		if ( IsSGraph( e->dst ) )
		    continue;

                if ( e->eport == i->iport )
		    if ( e->dst->color != i->src->color )
                      /* BUG FIX FOR PEEK OPERATION: 12/9/91 CANN */
                      if ( e->dst->type != IFPeek ) {
			n = FindLGenerator( c->L_BODY, e->dst->color );

			if ( !IsAdePresent( n, i->src ) )
		            CreateAndInsertAde( n, i->src, HIGH_PRI );
			}
		}
	    }
	}
}

/**************************************************************************/
/* LOCAL  **************        CanExecute         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE SOURCE NODES OF ALL IMPORTS, DATE AND ADE, */
/*          HAVE EXECUTED, ELSE RETURN FALSE.                             */
/**************************************************************************/

static int CanExecute( n )
PNODE n;
{
    register PADE  a;
    register PEDGE i;

    for ( a = n->aimp; a != NULL; a = a->isucc )
	if ( !(a->src->executed) )
	    return( FALSE );

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( IsConst( i ) )
	    continue;

	if ( IsGraph( i->src ) )
	    continue;

	if ( !(i->src->executed) )
	    return( FALSE );
        }

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************         SortNodes         ************************/
/**************************************************************************/
/* PURPOSE: DATA FLOW ORDER THE NODES IN GRAPH g. THE ALGORITHM SIMULATES */
/*          EXECUTION. IF THE SOURCE NODE OF EACH IMPORT, DATA AND ADE,   */
/*          TO A NODE n HAS EXECUTED (BEEN REMOVED FROM THE NODE LIST AND */
/*          PLACED IN THE SORTED LIST) THEN n CAN BE MOVED AND MARKED AS  */
/*          EXECUTED.                                                     */
/**************************************************************************/

static void SortNodes( g )
PNODE g;
{
    register int   m;
    register PNODE ehead = NULL;                /* HAS EXECUTED LIST HEAD */
    register PNODE etail = NULL;                /* HAS EXECUTED LIST TAIL */
    register PNODE n;
    register PNODE sn;
    register PNODE sg;

    while ( g->G_NODES != NULL ) {
	m = FALSE;

	for ( n = g->G_NODES; n != NULL; n = sn ) {
	    sn = n->nsucc;

	    if ( CanExecute( n ) ) {
                UnlinkNode( n ); m = TRUE;

                if ( ehead == NULL )
                    ehead = etail = n;
                else
                    etail = LinkNode( etail, n );

                n->executed = TRUE;

		if ( IsCompound( n ) )
		    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
			SortNodes( sg );
		}
            }

	if ( (!m) && (g->G_NODES != NULL) )
	    Error1( "SortNodes: CYCLES PRESENT" );
	}

    g->G_NODES = ehead;

    if ( ehead != NULL )
	ehead->npred = g;
}


/**************************************************************************/
/* GLOBAL **************          If2Ade           ************************/
/**************************************************************************/
/* PURPOSE: USING THE GLOBAL READ/WRITE SETS ADD ARTIFICIAL DEPENDENCE    */
/*          EDGES TO ALL FUNCTION GRAPHS: FROM ALL READ OPERATIONS TO ALL */
/*          WRITE OPERATIONS OF A GIVEN DATA VALUE.  CYCLES ARE REMOVED   */
/*          AFTER ALL ADES HAVE BEEN ADDED TO THE GRAPHS.  AN ATTEMPT IS  */
/*          MADE TO ORDER (USING ADES) NODES DIRECTLY PRODUCING LOOP      */
/*          CARRIED L PORT VALUES TO ELIMINATE SCALAR COPYING; SUCH ADES  */
/*          HAVE A HIGHER REMOVAL PRIORITY AS AGGREGATE COPY AVOIDANCE    */
/*          SHOULD ALWAYS TAKE PRECEDENCE WHEN BREAKING ADE CYCLES.       */
/**************************************************************************/

void If2Ade()
{
    register PSET  s;
    register PEDGE r;
    register PEDGE w;
    register PNODE f;
    register int   ri;
    register int   wi;

    /* BASED ON GLOBAL READ/WRITE SET INFORMATION, ORDER AGGREGATE NODES  */
    /* TO ELIMINATE UNNEEDED COPYING; CYCLE AVOIDANCE IS NOT PERFORMED.   */

    for ( s = gshead; s != NULL; s = s->ssucc->ssucc ) {
	for ( ri = 0; ri <= s->last; ri++ )
	    for ( wi = 0; wi <= s->ssucc->last; wi++ ) {
                r = s->set[ri];
		w = s->ssucc->set[wi];

		if ( IsGraph( w->dst ) )
		    continue;

		CreateAndInsertAde( r->dst, w->dst, LOW_PRI );
		}
	}

    /* TRY AND ELIMINATE SCALAR COPYING OF LOOP CARRIED VALUES            */
    OrderLPortGenerators();

    /* SORT NODES SO DEPENDENCIES POINT RIGHT                             */

    for ( f = fhead; f != NULL; f = f->gsucc )
        SortNodes( f );
}
