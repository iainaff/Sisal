/* if1move.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:35  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************    FindAndLinkToSource    ************************/
/**************************************************************************/
/* PURPOSE: FIND THE SOURCE OF THE VALUE REFERENCED BY d.  IF THE SOURCE  */
/*          IS A NODE, d IS LINKED TO ITS EXPORT LIST. IF THE SOURCE IS   */
/*          A CONSTANT, d IS CHANGED INTO IT.  IF THE SOURCE IS NOT FOUND */
/*          AND alt IS NOT NULL, THEN d IS LINKED TO alt'S EXPORT LIST.   */
/*          NOTHING IS DONE IF d IS A CONSTANT.  THE IMPORT LIST OF NODE  */
/*          n SERVES AS THE ORIGIN OF THE SEARCH.                         */
/**************************************************************************/

void FindAndLinkToSource( n, d, alt )
PNODE n;
PEDGE d;
PNODE alt;
{
    register PEDGE i;

    if ( IsConst( d ) )
        return;

    i = FindImport( n, d->eport );

    if ( i == NULL ) {
	if ( alt != NULL ) {
	    LinkExport( alt, d );
	    return;
	    }

	FPRINTF( stderr, "HELP: n->type = %d n->line %d d->eport %d\n",
		      n->type, n->if1line, d->eport );
        Error1( "FindAndLinkToSource: FindImport FAILED" );
	}

    if ( ! IsConst( i ) ) {
        d->eport = i->eport;
        LinkExport( i->src, d );
	}
    else
	ChangeToConst( d, i );
}


/**************************************************************************/
/* GLOBAL **************        RemoveNode         ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODE n FROM GRAPH g.  THE IMPORTS OF n MUST HAVE g AS  */
/*          THEIR SOURCE NODE OR BE CONSTANTS. THE FORMER ARE UNLINKED    */
/*          FROM g'S EXPORT LIST. A COPY OF n'S EXPORT LIST IS MADE, AND  */
/*          ATTACHED TO n, ALLOWING THE OLD EXPORTS TO BE ATTACHED TO g'S */
/*          EXPORT LIST. A NEW PORT NUMBER IS ASSINGED TO THE IMPORT PORT */
/*          OF EACH EDGE IN n'S NEW EXPORT LIST. THE SAME PORT NUMBERS    */
/*          ARE ASSIGNED TO THE CORRESPONDING EXPORT PORTS OF EACH EXPORT */
/*          IN THE OLD EXPORT LIST.                                       */
/**************************************************************************/

void RemoveNode( n, g )
PNODE n;
PNODE g;
{
    	     PEDGE export;
    register PEDGE e;
    register PEDGE se;
    register PEDGE ee;
    register PEDGE d;
    register PEDGE i;
    register int   eport;

    export      = n->exp;
    n->exp   = NULL;

    for ( e = export; e != NULL; e = e->esucc ) {
	if ( e->eport < 0 )                                 /* ALREADY COPIED */
	    continue;

        ee        = CopyEdge( e, n, NULL_NODE );
	ee->iport = ++maxint;

	eport = e->eport;

	for ( d = e; d != NULL; d = d->esucc )
	    if ( d->eport == eport )                 /* DONE WITH THIS EXPORT */
		d->eport = -maxint;

        LinkExport( n, ee );
	}

    for ( e = export; e != NULL; e = se ) {
	se = e->esucc;

	if ( e->eport <= 0 )
	    e->eport = -(e->eport);

	LinkExport( g, e );
	}

    for ( i = n->imp; i != NULL; i = i->isucc )
	UnlinkExport( i );

    UnlinkNode( n );
}


/**************************************************************************/
/* GLOBAL **************         InsertNode        ************************/
/**************************************************************************/
/* PURPOSE: INSERT NODE n2 INTO THE GRAPH CONTAINING NODE n1 SUCH THAT    */
/*          THE EXPORTS OF n2 ARE ATTACHED TO THE IMPORT LIST OF n1 AND   */
/*          THE IMPORTS OF n2 ARE LINKED TO THE APPROPRIATE EXPORT LISTS: */
/*          FOUND BY SEARCHING THE IMPORT LIST OF n1. THIS ROUTINE IS     */
/*          INTENDED TO BE USED IN TWO WAYS:                              */
/*                                                                        */
/*          1. ADD n2, ONCE DEFINED IN A SUBGRAPH OF COMPOUND NODE n1, TO */
/*             THE SCOPE DEFINING n1 (LOOP INVARIANT REMOVAL).            */
/*                                                                        */
/*          2. ADD n2, ONCE DEFINED IN SOME SUBGRAPH OF A COMPOUND NODE,  */
/*             TO ANOTHER SUBGRAPH (n1) OF THE SAME COMPOUND NODE (LOOP   */
/*             NORMALIZATION).                                            */
/**************************************************************************/

void InsertNode( n1, n2 )
PNODE n1;
PNODE n2;
{
    register PEDGE i;
    register PEDGE si;
    register PEDGE e;

    for ( i = n2->imp; i != NULL; i = si ) {
	si = i->isucc;
        FindAndLinkToSource( n1, i, n1 );
	}

    for ( e = n2->exp; e != NULL; e = e->esucc )
	LinkImport( n1, e );

    if ( IsSGraph( n1 ) )
	LinkNode( FindLastNode( n1 ), n2 );
    else
	LinkNode( n1->npred, n2 );
}
