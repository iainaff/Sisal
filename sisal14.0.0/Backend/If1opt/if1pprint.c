/* if1pprint.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:36  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int nm    =  0;                             /* UNIQUE ID FOR NAMES */


/**************************************************************************/
/* GLOBAL **************    OptRemoveDeadNode      ************************/
/**************************************************************************/
void OptRemoveDeadNode( n )
PNODE n;
{
  /* Just a stub for the util.c module */
}

/**************************************************************************/
/* GLOBAL **************     OptNormalizeNode      ************************/
/**************************************************************************/
void OptNormalizeNode( n )
PNODE n;
{
  /* Just a stub for the util.c module */
}

/**************************************************************************/
/* STATIC **************      PrefixNameAlloc      ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE AND RETURN A UNIQUE NAME WITH PREFIX pre.            */
/**************************************************************************/

static char *PrefixNameAlloc( pre )
char *pre;
{
    char Namebuffer[100];

    SPRINTF( Namebuffer, "%s%d", pre, ++nm );

    return( CopyString( Namebuffer ) );
}


/**************************************************************************/
/* STATIC **************     AssignExportNames     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN A NAME TO EACH UNIQUE EXPORT OF n'S EXPORT LIST.       */
/**************************************************************************/

static void AssignExportNames( n, pre )
PNODE  n;
char  *pre;
{
    register PEDGE e1;
    register PEDGE e2;

    for ( e1 = n->exp; e1 != NULL; e1 = e1->esucc ) {
	if ( e1->dname != NULL )
	    continue;

        e1->dname = PrefixNameAlloc( pre );

	for ( e2 = e1->esucc; e2 != NULL; e2 = e2->esucc )
	    if ( (e2->eport == e1->eport) )
		e2->dname = e1->dname;
        }
}


/**************************************************************************/
/* STATIC **************      OptAssignNames       ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NAMES TO THE UNIQUE EXPORTS OF ALL NODES IN GRAPH g.   */
/**************************************************************************/

static void OptAssignNames( g )
register PNODE g;
{
    register PNODE n;

    if ( IsIGraph( g ) ) 
	return;

    if ( !IsSGraph( g ) )
	AssignExportNames( g, "I" );

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	AssignExportNames( n, "D" );

	if ( !IsSimple( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
	         OptAssignNames( g );
        }
}


/**************************************************************************/
/* STATIC **************   PPrintGraphParameters   ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE NAME OF EACH IMPORT AND EXPORT OF GRAPH g TO output.*/
/*          EXPORT NAME SYNTAX IS export port number : reference name AND */
/*          IMPORT NAME SYNTAX IS import port number : reference name or  */
/*          constant.  THE reference name CAN BE EITHER A SYMBOLIC NAME   */
/*          OR A PORT NUMBER.                                             */
/**************************************************************************/

static void PPrintGraphParameters( g )
PNODE g;
{
    register PEDGE e1;
    register PEDGE e2;
    register PEDGE i;

    FPRINTF( output, "(" );

    for ( e1 = g->exp; e1 != NULL; e1 = e1->esucc ) {
	if ( e1->eport < 0 )
	    continue;
	    
	FPRINTF( output, " %d:", e1->eport );

	if ( e1->dname == NULL )
            FPRINTF( output, "%d", e1->eport );
	else
            FPRINTF( output, "%s", e1->dname );

	for ( e2 = e1->esucc; e2 != NULL; e2 = e2->esucc )
	    if ( e2->eport == e1->eport )
		e2->eport = -(e2->eport);
        }

    for ( e1 = g->exp; e1 != NULL; e1 = e1->esucc )
        if ( e1->eport <= 0 )
	    e1->eport = -(e1->eport);

    FPRINTF( output, " RETURNS" );

    for ( i = g->imp; i != NULL; i = i->isucc ) {
	FPRINTF( output, " %d:", i->iport );

	if ( IsConst( i ) ) {
	    PPrintConst( i );
	    continue;
	    }

        if ( i->dname == NULL )
	    FPRINTF( output, "%d", i->eport );
        else
	    FPRINTF( output, "%s", i->dname );
        }

    FPRINTF( output, " )\n" );
}


/**************************************************************************/
/* STATIC **************    If1PPrintNodeParameters   ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE NAME OF EACH IMPORT AND EXPORT OF NODE n TO output  */
/*          EXPORT NAME SYNTAX IS export port number : symbolic name AND  */
/*          IMPORT NAME SYNTAX IS import port number : reference name or  */
/*          constant.  THE reference name CAN BE EITHER A SYMBOLIC NAME   */
/*          OR A PORT NUMBER.                                             */
/**************************************************************************/

static void If1PPrintNodeParameters( n )
PNODE n;
{
    register PEDGE e1;
    register PEDGE e2;
    register PEDGE i;

    FPRINTF( output, "(" );

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	FPRINTF( output, " %d:", i->iport );

	if ( IsConst( i ) ) {
	    PPrintConst( i );
	    continue;
	    }

        if ( i->dname == NULL )
	    FPRINTF( output, "%d", i->eport );
        else
	    FPRINTF( output, "%s", i->dname );
        }

    FPRINTF( output, " RETURNS" );

    for ( e1 = n->exp; e1 != NULL; e1 = e1->esucc ) {
	if ( e1->eport < 0 )
	    continue;
	    
        FPRINTF( output, " %d:%s", e1->eport, e1->dname );

	for ( e2 = e1->esucc; e2 != NULL; e2 = e2->esucc )
	    if ( e2->eport == e1->eport )
		e2->eport = -(e2->eport);
        }

    for ( e1 = n->exp; e1 != NULL; e1 = e1->esucc )
	if ( e1->eport <= 0 )
	    e1->eport = -(e1->eport);

    FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* LOCAL  **************         If1PPrintNode        ************************/
/**************************************************************************/
/* PURPOSE: PRINT GRAPH g IN A READABLE FORMAT; indent DEFINES THE        */
/*          NESTING LEVEL.  TYPE INFORMATION IS NOT PRINTED.              */
/**************************************************************************/

static void If1PPrintNode( g, indent )
PNODE g;
{
    register PNODE n;

    PPrintIndentation( indent, g->if1line );

    switch ( g->type ) {
	case IFIGraph:
	    FPRINTF( output, "IMPORT FUNCTION %s(...)\n", g->G_NAME );
	    return;

	case IFLGraph:
	    FPRINTF( output, "LOCAL FUNCTION %s", g->G_NAME );
	    break;

	case IFXGraph:
	    FPRINTF( output, "EXPORT FUNCTION %s", g->G_NAME );
	    break;

	case IFSGraph:
	    FPRINTF( output, "SGrph" );
	    break;
	}

    PPrintGraphParameters( g ); indent++;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	PPrintIndentation( indent, n->if1line );

	if ( IsOther( n ) )
	    FPRINTF( output, "Other[%d]", n->type );
	FPRINTF( output, "%s", GetNodeName(n));

        If1PPrintNodeParameters( n );

	if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		If1PPrintNode( g, indent + 1 );
	}
}


/**************************************************************************/
/* GLOBAL **************         if1PPrint         ************************/
/**************************************************************************/
/* PURPOSE: PRINT ALL FUNCTION GRAPHS TO output IN A READABLE FORM.       */
/**************************************************************************/

void If1PPrint()
{
    register PNODE f;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
	FPRINTF( output, "\n" );

	OptAssignNames( f );
	If1PPrintNode( f, 0 );
	}
}
