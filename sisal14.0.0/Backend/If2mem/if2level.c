/* if2level.c,v
 * Revision 12.7  1992/11/04  22:05:06  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:22  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define SIZE_LEVEL_STACK 400

static int   level = -1;                           /* TOP OF LEVEL STACK */
static PNODE lstack[SIZE_LEVEL_STACK];             /* LEVEL STACK        */


/**************************************************************************/
/* LOCAL  **************       CopyLevelStack      ************************/
/**************************************************************************/
/* PURPOSE: RETURN A COPY OF THE CURRENT LEVEL STACK.                     */
/**************************************************************************/

static PPNODE CopyLevelStack()
{
    register PPNODE ls;
    register int    i;

    ls = (PPNODE) MyAlloc( (int) ((level+1) * sizeof(PNODE)) );

    for ( i = 0; i <= level; i++ )
	ls[i] = lstack[i];

    return( ls );
}


/**************************************************************************/
/* GLOBAL **************    AssignLevelNumbers     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN LEVEL NUMBERS AND LEVEL STACKS TO EACH NODE IN GRAPH g.*/
/*          FIRST THE LEVEL IS INCREMENTED SINCE WE ARE ENTERING A NEW    */
/*          SCOPE.  GRAPH g IS THEN PUSHED ONTO THE LEVEL STACK.  AFTER   */
/*          LEVEL INFORMATION IS ASSIGNED TO ALL NODES AT THIS LEVEL AND  */
/*          ALL DEEPER LEVELS ACCESSABLE FROM IT, THE LEVEL IS POPPED     */
/*          FROM THE LEVEL STACK.                                         */
/**************************************************************************/

void AssignLevelNumbers( g )
PNODE g;
{
    register PNODE  n;
    register PPNODE ls;

	             /* PUSH A NEW LEVEL */

    if ( (++level) >= SIZE_LEVEL_STACK )
	Error2( "AssignLevelNumbers",  "LEVEL STACK OVERFLOW" );

    lstack[level] = g;

    ls = CopyLevelStack();

    g->lstack = ls;
    g->level  = level;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	n->lstack = ls;
	n->level  = level;

	if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		AssignLevelNumbers( g );
	}

	             /* POP THE NEW LEVEL */

    level--;
}
