/* $Log$
 * Revision 1.1  1993/01/14  22:29:10  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 * */

#include "world.h"

/**************************************************************************/
/* GLOBAL **************       IsStreamTask        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF LOOP n IMPORTS OR EXPORTS A STREAM, ELSE FALSE.*/
/**************************************************************************/

int IsStreamTask( n )
PNODE n;
{
    register PEDGE i;
    register PEDGE e;
    register PNODE init;
    register PNODE ret;
    register int   yes;

    if ( IsForall( n ) ) {
        init = n->F_GEN; 
        ret = n->F_RET;
    } else {
        init = n->L_INIT;
        ret = n->L_RET;
        }

    yes = FALSE;

    for ( i = n->imp; i != NULL; i = i->isucc ) 
        if ( IsStream( i->info ) )
            if ( IsExport( init, i->iport ) )
                yes = TRUE;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        if ( IsStream( e->info ) ) {
            if ( (i = FindImport( ret, e->eport )) == NULL )
              continue;

            switch ( i->src->type ) {
                case IFAGather:
                    yes = TRUE;
                    break;

                default:
                    break;
                }
            }
        else
            return( FALSE );
        }

    return( yes );
}
