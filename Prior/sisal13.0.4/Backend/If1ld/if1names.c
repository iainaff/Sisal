/* if1names.c,v
 * Revision 12.7  1992/11/04  22:04:54  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:07:15  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


PNAME xnames = NULL;                        /* EXPORT AND LOCAL NAME LIST */
PNAME inames = NULL;                                  /* IMPORT NAME LIST */


/**************************************************************************/
/* LOCAL  **************     IsInterfaceType       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF TYPE i IS APPROPRIATE FOR AN INTERFACE CALL.   */
/*          SEE IsInterfaceCandidate BELOW.                               */
/**************************************************************************/

static int IsInterfaceType( i )
register PINFO i;
{
  register PINFO ii;

  switch ( i->type ) {
    case IF_INTEGER:
    case IF_REAL:
    case IF_DOUBLE:
    case IF_CHAR:
      return( TRUE );

    case IF_ARRAY:
      break;

    default:
      return( FALSE );
    }

  for ( ii = i->A_ELEM; ii != NULL; ii = ii->A_ELEM )
    switch ( ii->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
      case IF_CHAR:
        return( TRUE );

      case IF_ARRAY:
        continue;

      default:
        return( FALSE );
      }

  Error2( "IsInterfaceType", "FOR LOOP FAILURE" );
  return 0;
}


/**************************************************************************/
/* LOCAL  **************    IsInterfaceCandidate   ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NAME name OF TYPE t DEFINES AN INTERFACE;      */
/*          THAT IS, A FUNCTION THAT RETURNS EITHER AN INTEGER, REAL,     */
/*          DOUBLE_REAL, CHARACTER,  OR ONE OR N-DIMENSIONAL ARRAYS OF    */
/*          TYPE INTEGER,  REAL, DOUBLE_REAL OR CHARACTER AND TAKES ZERO  */
/*          OR MORE INPUTS OF OF TYPE INTEGER, REAL, DOUBLE_REAL,         */
/*          CHARACTER, OR N-DIMENSIONAL ARRAY OF INTEGER, REAL, CHARACTER */
/*          OR DOUBLE_REAL.                                               */
/*                                                                        */
/*          RECENTLY MOFIFIED TO ALLOW MULTIPLE-ARITY!!!                  */
/**************************************************************************/

static int IsInterfaceCandidate( name, t )
char  *name;
PINFO  t;
{
  register PINFO i;
  register int   ac;
  register int   c;

/* CANN NEW 3/92 */
  for ( ac = 0, i = t->F_OUT; i != NULL; i = i->L_NEXT ) {
    if ( !IsInterfaceType( i->L_SUB ) )
      return( FALSE );

    if ( !IsArray( i->L_SUB ) )
      continue;

    ac++;
    }

  if ( ac > 0 ) {
    for ( c = 0, i = t->F_IN; i != NULL; i = i->L_NEXT ) {
      if ( IsArray( i->L_SUB ) )
	if ( IsInteger( i->L_SUB->A_ELEM ) ) {
	  c++;
	  continue;
	  }

      c = 0;
      }

    if ( c != ac )
      return( FALSE );
    }
/* END CANN NEW 3/92 */

  for ( i = t->F_IN; i != NULL; i = i->L_NEXT ) {
    if ( !IsInterfaceType( i->L_SUB ) )
      return( FALSE );
    }
    
  return( TRUE );
}


/**************************************************************************/
/* GLOBAL **************      AddToImportList      ************************/
/**************************************************************************/
/* PURPOSE: ADD NAME nm TO THE IMPORT NAME LIST. NOTE, DUPLICATE ENTRIES  */
/*          ARE ALLOWED AT THIS POINT.  N IS nm'S IGraph NODE.            */ 
/**************************************************************************/

void AddToImportList( n, nm, t )
PNODE  n;
char  *nm;
PINFO  t;
{
  register PNAME i;
  register PNAME ii;

  ii = NameAlloc( n, nm, t );

  for ( i = inames; i != NULL; i = i->next )
    if ( strcmp( i->name, nm ) == 0 ) {
      ii->usucc = i->usucc;
      i->usucc  = ii;
      return;
      }

  ii->next = inames;
  inames  = ii;
}


/**************************************************************************/
/* GLOBAL **************        AddToNameList      ************************/
/**************************************************************************/
/* PURPOSE: ADD THE FUNCTION DEFINED BY GRAPH NODE n TO THE NAME LIST.    */
/*          DUPLICATE ENTRIES RESULT IN AN ERROR MESSAGE. NODE n CAN BE   */
/*          EITHER A XGraph OR LGraph NODE.                               */
/**************************************************************************/

void AddToNameList( n )
PNODE n;
{
  register PNAME x;

  for ( x = xnames; x != NULL; x = x->next )
    if ( strcmp( x->name, n->CoNsT ) == 0 )
      Error2( (n->funct)? n->funct : n->CoNsT, "IS MULTIPLY DEFINED" );

  x = NameAlloc( n, n->CoNsT, n->info );

  x->next = xnames;
  xnames  = x;
}


/**************************************************************************/
/* LOCAL  **************        NameLookup         ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NODE OF NAME LIST l WHOSE name FIELD IS EQUAL TO n.*/
/*          NULL IS RETURNED IF n IS NOT FOUND.                           */
/**************************************************************************/

static PNAME NameLookup( l, n )
PNAME  l;
char  *n;
{
    register PNAME p;
    
    for ( p = l; p != NULL; p = p->next )
	if ( strcmp( p->name, n ) == 0 )
	    return( p );

    return( NULL );
}


/**************************************************************************/
/* GLOBAL **************  CheckForUnresolvedNames  ************************/
/**************************************************************************/
/* PURPOSE: CHECK FOR UNRESOLVED FUNCTION REFERENCES AND PARAMETER TYPE   */
/*          CONFLICTS AND DECORATE THE GRAPH ACCORDINGLY.                 */
/**************************************************************************/

void CheckForUnresolvedNames()
{
  register PNAME i;
  register PNAME ii;
  register PNAME x;
  register PNODE n;
  register int   idx;

  /* MARK THE ENTRY POINTS */
  for ( x = xnames; x != NULL; x = x->next ) {
    n = x->node;

    if ( (idx = IsEntryPoint( n->CoNsT )) != -1 ) {
      entrys[idx] = TRUE;

      if ( n->type != IFXGraph )
        Error2( (n->funct)? n->funct : n->CoNsT,
                "WAS SELECTED AS AN ENTRY POINT BUT NOT INCLUDED IN A define");

      if ( smodule )
        n->mark = 's';
      else if ( forC )
        n->mark = 'c';
      else if ( forF )
        n->mark = 'f';
      else
        n->mark = 'e';

      continue;
      }

    if ( IsReductionInterface( x->name ) )
      n->mark = 'd';

    /* NOT AN ENTRY POINT SO MAKE IT A LOCAL FUNCTION IN THE MODULE! */
    if ( n->type == IFXGraph )
      n->type = IFLGraph;
    }

  /* COMPLETE PROCESSING OF THE IMPORTS */

  /* ARE DUPLICATE ENTRIES IDENTICAL? */
  for ( i = inames; i != NULL; i = i->next ) {
    for ( ii = i->usucc; ii != NULL; ii = ii->usucc )
      if ( ii->info->label != i->info->label )
        Error2( "ARGUMENT TYPE CONFLICT:", 
		(i->node->funct)? i->node->funct : i->name );
    }

  for ( i = inames; i != NULL; i = i->next ) {
    if ( IsCInterface( i->name ) ) {
      i->mark = 'c';

      if ( !IsInterfaceCandidate( i->name, i->info ) )
        Error2( "ILLEGAL INTERFACE-FUNCTION:",
		(i->node->funct)? i->node->funct : i->name       );
      }
    else if ( IsFortranInterface( i->name ) ) {
      i->mark = 'f';

      if ( !IsInterfaceCandidate( i->name, i->info ) )
        Error2( "ILLEGAL INTERFACE-FUNCTION:",
		(i->node->funct)? i->node->funct : i->name       );
      }
    else if ( IsIntrinsic( i->name, i->info ) )
      i->mark = 'i';
    else
      i->mark = 's';
    }

  for ( i = inames; i != NULL; i = i->next ) {
    if ( (x = NameLookup( xnames, i->name )) != NULL ) {
      if ( i->mark != 's' )
	Error2( (i->node->funct)? i->node->funct : i->name,
	        "MULTIPLY DEFINED"                    );

      if ( i->info->label != x->info->label )
        Error2( "PARAMETER LIST TYPE MISMATCH FOR:",
		(i->node->funct)? i->node->funct : i->name );

      i->mark = '\0'; /* GET i OUT OF THE IMPORT LIST */
      continue;
      }
    }

  monolith = FALSE;

  if ( pmodule ) {
    monolith = TRUE;

    for ( i = inames; i != NULL; i = i->next )
      if ( i->mark == 's' )
	monolith = FALSE;
    }


  if ( !(forF || forC || smodule) )
    if ( etop > 0 )
      Error1( "MULTIPLE ENTRY POINTS NOT ALLOWED IN A STAND-ALONE PROGRAM" );

  for ( idx = 0; idx <= etop; idx++ )
    if ( !(entrys[idx]) )
      Error2( "ENTRY POINT NOT ENCOUNTERED:", entryt[idx] );
}
