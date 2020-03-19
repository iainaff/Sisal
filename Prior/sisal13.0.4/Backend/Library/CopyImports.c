#include "world.h"


/**************************************************************************/
/* GLOBAL **************        CopyImports        ************************/
/**************************************************************************/
/* PURPOSE: COPY THE IMPORT LIST OF NODE n1 AND ATTACH IT TO NODE n2.  IF */
/*          atts IS TRUE, THEN EACH NEW IMPORT IS ATTACHED TO ITS COPY'S  */
/*          SOURCE NODE COPY'S EXPORT LIST.                               */
/**************************************************************************/

void CopyImports( n1, n2, atts )
PNODE n1;
PNODE n2;
int   atts;
{
    register PEDGE i;
    register PEDGE ii;

    for ( i = n1->imp; i != NULL; i = i->isucc ) {
        ii = CopyEdge( i, NULL_NODE, n2 );

	LinkImport( n2, ii );

	if ( atts && (!IsConst(i)) )
	    LinkExport( i->src->copy, ii );
	}
}

