/* if1count.c,v
 * Revision 12.7  1992/11/04  22:04:56  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:31  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* LOCAL  **************       WriteOptCountInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE COUNT VALUES TO stderr: TOTALS AND A SUMMARY BY LEVEL.  */
/**************************************************************************/

static void WriteOptCountInfo( msg )
char *msg;
{
    register struct level *l;
    register int           i;

    FPRINTF( stderr, "\n   * OCCURRENCE COUNTS %s\n\n", msg );

    FPRINTF( stderr, " Lits  %4d Edges %4d Smpls %4d Comps %4d Grphs %4d",
		       lits, edges, simples, comps, graphs              );

    for ( i = 0; i < IF1GraphNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", GraphName(i), gnodes[i] );
	}

    for ( i = 0; i < IF1CompoundNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", CompoundName(i), cnodes[i] );
	}

    for ( i = 0; i < IF1SimpleNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", SimpleName(i), snodes[i] );
	}

    FPRINTF( stderr, "\n" );

    FPRINTF( stderr, " RSum  %4d RProd %4d RLst  %4d RGrt  %4d RCat  %4d\n\n",
		       rsum, rprod, rleast, rgreat, rcat                    );

    FPRINTF( stderr, "   * SUMMARY BY LEVEL\n\n" );

    for ( i = 0; i <= maxl; i++ ) {
	l = &(levels[i]);

        FPRINTF( stderr, " Level %4d            Lits  %4d Edges %4d", i, 
			 l->lits, l->edges                           );

	FPRINTF( stderr, " Smpls %4d Comps %4d Grphs %4d\n",
		         l->simples, l->comps, l->graphs  );
        }

}


/**************************************************************************/
/* GLOBAL **************         If1Count          ************************/
/**************************************************************************/
/* PURPOSE: COUNT OCCURRENCES OF GRAPH NODES, SUBGRAPH NODES, SIMPLE      */
/*          NODES, COMPOUND NODES, LITERALS, AND EDGES IN ALL FUNCTION    */
/*          GRAPHS.  THE COUNTS ARE PRINTED TO stderr.                    */
/**************************************************************************/

void If1Count( msg )
char *msg;
{
    register struct level *l;
    register PNODE         f;
    register int           i;

    lits = edges  = simples = graphs = comps = 0;
    rsum = rgreat = rleast  = rcat   = rprod = 0;

    topl = maxl =  -1;

    for ( i = 0; i < MaxLevel; i++ ) {
	l = &(levels[i]);

	l->lits = l->edges = l->simples = l->comps = l->graphs = 0;
	}

    for ( i = 0; i < IF1GraphNodes; i++ )
	gnodes[i] = 0;

    for ( i = 0; i < IF1SimpleNodes; i++ )
	snodes[i] = 0;

    for ( i = 0; i < IF1CompoundNodes; i++ )
	cnodes[i] = 0;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
	Count( cfunct = f );

    WriteOptCountInfo( msg );
}
