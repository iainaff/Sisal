/* if2call.c,v
 * Revision 12.7  1992/11/04  22:05:10  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:00  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int color = WHITE;            /* CURRENT COLOR IN CYCLE DETECTION    */

/**************************************************************************/
/* LOCAL  **************        UpFindCallee       ************************/
/**************************************************************************/
/* PURPOSE: FIND AND RETURN THE CALL GRAPH TRUNK NODE ASSOCIATED WITH     */
/*          FUNCTION nm. IF NOT FOUND, NULL IS RETURNED.                  */
/**************************************************************************/

static PCALL UpFindCallee( nm )
char *nm;
{
    register PCALL c;

    for ( c = cghead; c != NULL; c = c->callee )
	if ( strcmp( nm, c->graph->G_NAME ) == 0 )
	    return( c );

    return( NULL );
}


/**************************************************************************/
/* LOCAL  **************   UpAddCalleeReferences   ************************/
/**************************************************************************/
/* PURPOSE: RECURSIVELY TRAVERSE THE IF1 GRAPH g, ADDING A CALL GRAPH     */
/*          NODE TO THE CALLEE REFERENCE LIST HEADED BY TRUNK NODE c FOR  */
/*          EACH ENCOUNTERED IF1 CALL NODE. THE REFERENCE COUNT OF THE    */
/*          CALLEE'S CALL GRAPH TRUNK NODE IS INCREMENTED.                */
/**************************************************************************/

static void UpAddCalleeReferences( c, g )
PCALL c;
PNODE g;
{
    register PNODE n;
    register PCALL r;
    register PCALL cee; 

    if ( IsIGraph( g ) )
	return;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	if ( IsCall( n ) ) {
	    if ( (cee = UpFindCallee( n->imp->CoNsT )) == NULL )
		continue;

	    r = CallAlloc( n );

	    r->caller = c->caller;
	    c->caller = r;

	    r->callee = cee;

	    continue;
	    }

        if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		UpAddCalleeReferences( c, g );
        }
}


/**************************************************************************/
/* LOCAL  **************       UpMakeCallGraph       ************************/
/**************************************************************************/
/* PURPOSE: CONSTRUCT A CALL GRAPH.  FIRST THE TRUNK OF THE CALL GRAPH IS */
/*          BUILT.  THE TRUNK CONTAINS AN ENTRY FOR EACH FUNCTION IN THE  */ 
/*          INPUT. AFTER THE TRUNK IS BUILT, THE CALLEE REFERENCE LISTS   */
/*          ARE CONSTRUCTED. Gmain IS SET TO ADDRESS THE MONOLITH ENTRY   */
/*          POINT.                                                        */
/**************************************************************************/

static void UpMakeCallGraph()
{
    register PNODE f;
    register PCALL c;
    /* register PNODE gmain = NULL; */ /* CANN NEW 2/92 */

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
	/* if ( f->emark ) */ /* CANN NEW 2/92 */
	    /* gmain = f; */

	c = CallAlloc( cfunct = f );

	if ( cghead != NULL )
	    cgtail->callee = c;
        else
	    cghead = c;

        cgtail = c;

	/* NEW CANN: FORCE TO LOOK LIKE A RECURSIVE ENTRY POINT 2/92 */
	if ( f->mark == 's' )
	  f->bmark = TRUE;
	}

    for ( c = cghead; c != NULL; c = c->callee )
	UpAddCalleeReferences( c, c->graph );

    /* if ( gmain == NULL ) */ /* CANN NEW 2/92 */
	/* Error1( "UpMakeCallGraph: MONOLITH ENTRY POINT NOT FOUND" ); */
}


/**************************************************************************/
/* LOCAL  **************        UpBreakCycles        ************************/
/**************************************************************************/
/* PURPOSE: IDENTIFY AND BREAK ANY RECURSIVE CYCLES ORIGINATING FROM CALL */
/*          NODE caller->graph (REFERENCING callee->graph).  IF callee    */
/*          HAS THE SAME COLOR AS color, IT IS PART OF A CYCLE THAT WILL  */
/*          REENTER THE FUNCTION CONTAINING caller->graph; SUCH A CALL    */
/*          NODE AND ITS ASSOCIATED CALLEE FUNCTION GRAPH ARE MARKED AS   */
/*          BROKEN.                                                       */
/**************************************************************************/

static void UpBreakCycles( caller, callee )
PCALL caller;
PCALL callee;
{
    register PCALL r;

    if ( callee->checked ) {
	if ( caller != NULL ) 
	    if ( callee->graph->bmark )
		caller->graph->bmark = TRUE;

	return;
	}

    if ( callee->color == color ) {
	if ( caller != NULL )
	    caller->graph->bmark = TRUE;

	callee->graph->bmark = TRUE;

	return;
	}

    callee->color = color;

    for ( r = callee->caller; r != NULL; r = r->caller )
	UpBreakCycles( r, r->callee );

    callee->checked = TRUE;
}


/**************************************************************************/
/* LOCAL  **************    SortFunctionGraphs     ************************/
/**************************************************************************/
/* PURPOSE: SORT THE FUNCTION GRAPH LIST SO THAT ALL REFERENCES TO ANY    */
/*          FUNCTION GRAPH f ARE SUCCESSORS OF f IN THE SORTED LIST.      */
/*          WHEN DONE, fhead WILL ADDRESS THE HEAD OF THE SORTED LIST AND */
/*          ftail WILL ADDRESS THE TAIL.  AS CYCLES HAVE ALREADY BEEN     */
/*          BROKEN, THE ROUTINE WILL TERMINATE.                           */
/**************************************************************************/

static void SortFunctionGraphs()
{
    register PNODE f;
    register PNODE sf;
    register PCALL r;
    register int   m;

    while ( glstop->gsucc != NULL )
	for ( f = glstop->gsucc; f != NULL; f = sf ) {
	    sf = f->gsucc;

	    m  = TRUE;
	    r = UpFindCallee( f->G_NAME );

	    if ( !IsIGraph( f ) )
	        for ( r = r->caller; r != NULL; r = r->caller ) {
		    if ( r->graph->bmark )
		        continue;

                    if ( r->callee->graph->sorted )
		        continue;

                    m = FALSE;
		    break;
		    }

	    if ( !m )
		continue;

	    UnlinkGraph( f );

            if ( fhead == NULL )
		fhead = ftail = f;
            else
		ftail = LinkGraph( ftail, f );

            f->sorted = TRUE;
	    }
}


/**************************************************************************/
/* GLOBAL **************        If2CallGraph       ************************/
/**************************************************************************/
/* PURPOSE: BUILD THE CALL GRAPH, BREAK ANY CYCLES, AND CHRONOLOGICALLY   */
/*          SORT THE FUNCTION GRAPHS (HEADED BY fhead).                   */
/**************************************************************************/

void If2CallGraph()
{
    register PCALL c;

    UpMakeCallGraph();

    for ( c = cghead; c != NULL; c = c->callee ) {
	color++;
	UpBreakCycles( NULL_CALL, c );
	}

    SortFunctionGraphs();
}
