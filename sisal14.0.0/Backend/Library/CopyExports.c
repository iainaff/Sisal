#include "world.h"


/**************************************************************************/
/* GLOBAL **************        CopyExports        ************************/
/**************************************************************************/
/* PURPOSE: COPY THE EXPORT LIST OF NODE n1 AND ATTACH IT TO NODE n2. A   */
/*          COPIED EXPORT IS NOT LINKED TO ITS COPY'S  DESTINATION NODE.  */
/**************************************************************************/

void CopyExports( n1, n2 )
PNODE n1;
PNODE n2;
{
    register PEDGE e;

    for ( e = n1->exp; e != NULL; e = e->esucc )
	LinkExport( n2, CopyEdge( e, n2, NULL_NODE ) );
}


