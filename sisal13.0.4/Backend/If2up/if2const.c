/* if2const.c,v
 * Revision 12.7  1992/11/04  22:05:10  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:01  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************     If2ConstGenerators    ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODES IN THE CONSTANT AGGREGATE GENERATOR LIST THAT    */
/*          DON'T CONSTRUCT CONSTANT AGGREGATES.  AN ABuildAT, AFillAT,   */
/*          OR RBuild NODE IS REMOVED IF ITS IMPORTS ARE NOT LITERALS OR  */
/*          REFERENCES TO OTHER CONSTANT AGGREGATE GENERATOR OUTPUTS.     */
/*          A DefArrayBuf NODE IS REMOVED IF ITS SIZE IMPORT IS NOT A     */
/*          LITERAL OR ITS EXPORT IS NOT IMPORT TO A MemAlloc NODE STILL  */
/*          IN THE CONSTANT AGGREGATE GENERATOR LIST.  A MemAlloc NODE    */
/*          IS REMOVED IF ITS IMPORT IS THE EXPORT OF A REMOVED           */
/*          DefArrayBuf NODE, ITS EXPORT HAS MORE THAN ONE USE, OR IS     */
/*          IMPORTED TO A NODE NOT IN THE LIST.  THE LIST IS REPEATIVELY  */
/*          EXAMINED UNTIL A PASS FAILS TO REMOVE A NODE.  INITIALLY ALL  */
/*          NODES IN THE LIST HAVE cmark SET TO TRUE.  TO REMOVE A NODE,  */
/*          ITS cmark IS SET TO FALSE. THE sr PRAGMAS ON OUTPUTS OF       */
/*          CONSTANT GENERATORS ARE INCREMENTED TO GUARANTEE IT IS NOT    */
/*          DEALLOCATED.                                                  */
/**************************************************************************/

void If2ConstGenerators()
{
    register PNODE n;
    register PEDGE i;
    register int   ch;
    register PEDGE e;

    ch = TRUE;

    while ( ch ) {
	ch = FALSE;

        for ( n = cohead; n != NULL; n = n->usucc ) {
	    /* HAS THE NODE ALREADY BEEN REMOVED?                         */
	    if ( !(n->cmark) )
		continue;

	    /* ARE n'S NONCONSTANT IMPORTS EXPORTS OF LIST NODES?         */

            for ( i = n->imp; i != NULL; i = i->isucc ) {
	        if ( IsConst( i ) ) {
	            if ( IsNonErrorConst( i ) )
		        continue;

		    break;
	            }

	        if ( !(i->src->cmark) )
		    break;

		/* NO BRecord IMPORTS ALLOWED!!! */
		if ( IsABRecord( i->info ) )
		  break;
	        }

	    if ( i != NULL ) {
	        n->cmark = FALSE; ch = TRUE;
	        continue;
	        }
	  
	    if ( IsDefArrayBuf( n ) ) {
                if ( !(n->exp->dst->cmark) ) {
	            n->cmark = FALSE; ch = TRUE;
		    }

                continue;
		}

	    if ( IsMemAlloc( n ) ) {
		if ( (n->exp->esucc != NULL) || !(n->exp->dst->cmark) ) {
		    n->cmark = FALSE; ch = TRUE;
		    }

		continue;
		}
	    }
        }

    for ( n = cohead; n != NULL; n = n->usucc ) {
	if ( !(n->cmark) )
	    continue;

        switch ( n->type ) {
	    case IFABuildAT:
	    case IFAFillAT:
	    case IFRBuild:
		for ( e = n->exp; e != NULL; e = e->esucc )
		    e->sr++;

		break;

	    default:
		break;
	    }
	}
}
