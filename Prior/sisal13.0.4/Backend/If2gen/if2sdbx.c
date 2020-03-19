/* if2sdbx.c,v
 * Revision 12.7  1992/11/04  22:05:03  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

#define MAX_ENTRIES  2000

static int ssize = -1;
static char *scope[MAX_ENTRIES];

static char *cfl  = NULL;
static char *cfn = NULL;


/**************************************************************************/
/* GLOBAL **************   PrintSdbxFunctionList   ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE SDBX FUNCTION LIST TO output.                       */
/**************************************************************************/

void PrintSdbxFunctionList()
{
  register PNODE  f;
  register int    c;
  register char  *nm;

  for ( c = 0, f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    c++;
    }

  FPRINTF( output, "\nstatic char *MyFunctionList[%d] = {\n", c+1 );

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    nm = LowerCase( f->funct, FALSE, FALSE );

    FPRINTF( output, "  \"%s\",\n", nm );
    free( nm );
    }

  FPRINTF( output, "  \"\" };\n\n" );
}


/**************************************************************************/
/* STATIC **************    AssignSdbxOffset       ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN AN SDBX SCOPE OFFSET TO THE NAME ASSOCIATED WITH EDGE  */
/*          e.                                                            */
/**************************************************************************/

static void AssignSdbxOffset( e )
PEDGE e;
{
  register int   i;
  register char *nm;

  nm = LowerCase( e->name, FALSE, FALSE );

  for ( i = 0; i <= ssize; i++ )
    if ( strcmp( nm, scope[i] ) == 0 ) {
      e->sdbx = i;
      break;
      }

  if ( i > ssize ) {
    if ( (ssize+1) >= MAX_ENTRIES )
      return;

    scope[++ssize] = nm;
    e->sdbx = ssize;
    }
}


/**************************************************************************/
/* GLOBAL **************    AssignScopePositions   ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN SCOPE POSITIONS TO THE NAMES DEFINED IN GRAPH g.       */
/**************************************************************************/

static void AssignScopePositions( g )
PNODE g;
{
  register PNODE  n;
  register PNODE  sg;
  register PEDGE  e;
  register PEDGE  ee;
  register PEDGE  ii;
  register int    c;
  register int    l;
	   char   buf[100];

  l = -9999;

  for ( n = g; n != NULL; n = n->nsucc ) {
    /* CLEANUP THE LINE NUMBERS!!! LINE 0 0 0 0 NEXT-LINE... */
    if ( n != g ) {
      if ( l == -9999 && n->line > 0 )
	l = n->line;
      else if ( n->line > 0 && n->line == l )
	n->line = 0;
      else if ( n->line > 0 )
	l = n->line;
      }

    /* CLEAR OUT THE # NAMES INTRODUCED BY THE FRONTEND */
    /* ALSO INTRODUCE # ARRAY NAMES */
    for ( e = n->exp; e != NULL; e = e->esucc ) {
      if ( e->name != NULL )
	if ( e->name[0] == '#' )
	  e->name = NULL;

      if ( e->dst->type == IFAElement && e->iport == 1 && e->name == NULL )
	e->name = CopyString( "#1" );

      if ( e->dst->type == IFDiv && e->iport == 2 && e->name == NULL )
	e->name = CopyString( "%1" );
      }

    switch ( n->type ) {
      case IFLGraph:
      case IFXGraph:
      case IFSGraph:
	/* ASSIGN $ NAMES TO SCOPE AND FUNCTION IMPORTS */
	for ( c = 1, ii = n->imp; ii != NULL; ii = ii->isucc, c++ )
	  if ( ii->name == NULL ) {
	    SPRINTF( buf, "$%d", c );
	    ii->name = CopyString( buf );
	    }

	break;

      case IFForall:
        if ( (ii = n->F_GEN->imp) != NULL )
	  if ( ii->name != NULL )
	    if ( !IsExport( n->F_BODY, ii->iport ) )
	      AssignSdbxOffset( ii );

	AssignScopePositions( n->F_BODY );
	break;

      case IFLoopB:
      case IFLoopA:
	AssignScopePositions( n->L_BODY );
	break;

      case IFSelect:
	AssignScopePositions( n->S_ALT );
	AssignScopePositions( n->S_CONS );
	break;

      case IFTagCase:
	for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	  AssignScopePositions( sg );

	break;

      default:
	break;
      }

    for ( e = n->exp; e != NULL; e = e->esucc ) {
      if ( e->eport < 0 ) {
	e->eport = -(e->eport);
	continue;
	}

      if ( e->name == NULL )
	continue;

      AssignSdbxOffset( e );

      for ( ee = e->esucc; ee != NULL; ee = ee->esucc ) {
	if ( ee->eport != e->eport )
	  continue;
	
	ee->eport = -(ee->eport);
	}
      }
    }
}

/**************************************************************************/
/* GLOBAL **************   BuildAndPrintSdbxScope  ************************/
/**************************************************************************/
/* PURPOSE: BUILD AND PRINT THE SDBX SCOPE DATA BASE FOR GRAPH g.         */
/**************************************************************************/

void BuildAndPrintSdbxScope( f )
PNODE f;
{
  register int i;

  if ( !sdbx )
    return;

  cfl = NULL;
  cfn = NULL;

  ssize = -1;

  AssignScopePositions( f );

  f->sdbx = ssize+1; /* SAVE THE ACTUAL COUNT! */

  if ( ssize < 0 )
    return;

  FPRINTF( output, 
	   "\nstatic struct SdbxValue SdbxScope%s[%d] = {\n", 
	   f->G_NAME, ssize+2 );

  for ( i = 0; i <= ssize; i++ ) {
    FPRINTF( output, "  {\"%s\",0,0,0,0},\n", scope[i] );
    }

  FPRINTF( output, "  {0,0,0,0,0} };\n" );
}


/**************************************************************************/
/* GLOBAL **************    UpdateSdbxScopeNames   ************************/
/**************************************************************************/
/* PURPOSE: UPDATE THE SCOPE NAME INFORMATION FOR THE EXPORTS OF NODE n.  */
/**************************************************************************/


void UpdateSdbxScopeNames( n )
PNODE n;
{
  register PEDGE  e;
  register PEDGE  ii;
  register PEDGE  i;
  register char  *ks;
  register char  *p;
  register int    arr;

  if ( !sdbx ) /* GUT CHECK!!! */
    return;

  if ( n == NULL )
    return;

  if ( IsSGraph( n ) ) {
    if ( IsForall( n->G_DAD ) )
      if ( n->G_DAD->F_BODY == n ) {
	ii = n->G_DAD->F_GEN->imp;
        UpdateSdbxScopeNames( ii->src );
	}
    }

  if ( IsGraph( n ) ) {
    for ( i = n->imp; i != NULL; i = i->isucc ) {
      if ( !IsConst( i ) )
	continue;

      if ( !IsBasic( i->info ) )
	continue;

      switch ( i->info->type ) {
        case IF_BOOL:
        case IF_CHAR:
        case IF_NULL:
        case IF_INTEGER:
	  ks = "SDBX_INT";
	  break;

        case IF_DOUBLE:
        case IF_REAL:
	  ks = "SDBX_DBL";
	  break;

	default:
	  continue;
	}

      FPRINTF( output, "SdbxCurrentScope[%d].Active = TRUE;\n", i->sdbx );
      FPRINTF( output, "SdbxCurrentScope[%d].Kind = %s;\n", i->sdbx, ks );

      FPRINTF( output, "SdbxCurrentScope[%d].ArrayType = FALSE;\n", i->sdbx );

      switch ( i->info->type ) {
        case IF_BOOL:
        case IF_CHAR:
        case IF_NULL:
        case IF_INTEGER:
	  FPRINTF( output, "SdbxCurrentScope[%d].Value.InT = ", i->sdbx );
	  PrintTemp( i );
	  FPRINTF( output, ";\n" );
	  break;

        case IF_DOUBLE:
        case IF_REAL:
	  FPRINTF( output, "SdbxCurrentScope[%d].Value.DbL = ", i->sdbx );
	  PrintTemp( i );
	  FPRINTF( output, ";\n" );
	  break;

        default:
	  break;
	}
      }
    }

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    arr = FALSE;

    if ( e->sdbx < 0 )
      continue;

    switch ( e->info->type ) {
      case IF_NONTYPE:
      case IF_BRECORD:
      case IF_PTR_DOUBLE:
      case IF_PTR_INTEGER:
      case IF_PTR_REAL:
      case IF_PTR:
        continue;

      case IF_BOOL:
      case IF_CHAR:
      case IF_NULL:
      case IF_INTEGER:
	ks = "SDBX_INT";
	p  = NULL;
	break;

      case IF_DOUBLE:
      case IF_REAL:
	ks = "SDBX_DBL";
	p  = NULL;
	break;

      case IF_ARRAY:
	arr = TRUE;
      default:
	ks = "SDBX_PTR";
	p  = e->info->wname;
	e->info->touch6 = TRUE;
      }

    FPRINTF( output, "SdbxCurrentScope[%d].Active = TRUE;\n", e->sdbx );
    FPRINTF( output, "SdbxCurrentScope[%d].Kind = %s;\n", e->sdbx, ks );

    if ( arr )
      FPRINTF( output, "SdbxCurrentScope[%d].ArrayType = TRUE;\n", e->sdbx );
    else
      FPRINTF( output, "SdbxCurrentScope[%d].ArrayType = FALSE;\n", e->sdbx );

    if ( p != NULL )
      FPRINTF( output, "SdbxCurrentScope[%d].PrintRoutine = %s;\n", e->sdbx, p);

    switch ( e->info->type ) {
      case IF_BOOL:
      case IF_CHAR:
      case IF_NULL:
      case IF_INTEGER:
	FPRINTF( output, "SdbxCurrentScope[%d].Value.InT = ", e->sdbx );
	PrintTemp( e );
	FPRINTF( output, ";\n" );
	break;

      case IF_DOUBLE:
      case IF_REAL:
	FPRINTF( output, "SdbxCurrentScope[%d].Value.DbL = ", e->sdbx );
	PrintTemp( e );
	FPRINTF( output, ";\n" );
	break;

      default:
	FPRINTF( output, "SdbxCurrentScope[%d].Value.PtR = ", e->sdbx );
	PrintTemp( e );
	FPRINTF( output, ";\n" );
      }
    }
}


/**************************************************************************/
/* GLOBAL **************       SaveSdbxState       ************************/
/**************************************************************************/
/* PURPOSE: SAVE THE SDBX STATE INFORMATION FOR NODE n, WHICH MAY BE A    */
/*          GRAPH NODE.                                                   */
/**************************************************************************/


void SaveSdbxState( n )
PNODE n;
{
  if ( !sdbx ) /* GUT CHECK!!! */
    return;

  if ( IsGraph( n ) && !IsSGraph( n ) )
    FPRINTF( output, "struct SdbxInfo SdbxStateBackup;\n" );

  if ( IsGraph( n ) && n->mark == 's' && !IsSGraph(n) ) {
    FPRINTF( output, "register int ClearSdbx;\n" );
    FPRINTF( output, "SdbxCurrentFunctionList = MyFunctionList;\n" );
    FPRINTF( output, "if ( !UsingSdbx ) {\n" );
    FPRINTF( output, "  ClearSdbx = TRUE;\n" );

    if ( n->Pmark ) {
      FPRINTF( output, "  if ( NumWorkers > 1 )\n" );
      FPRINTF( output, 
	       "    Error( \"SDBX\", \"-sdbx MODULE CALLED IN PARALLEL\" );\n");
      }

    FPRINTF( output, "  SdbxMonitor( SDBX_ESTART );\n" );
    FPRINTF( output, "  }\n" );
    FPRINTF( output, "else\n" );
    FPRINTF( output, "  ClearSdbx = FALSE;\n" );
    }
  
  if ( !IsGraph( n ) ) {
    if ( n->line <= 0 )
      return;

    /* FIRST NODE OF GRAPH? */
    if ( !IsGraph( n->npred ) )
      FPRINTF( output, "SdbxHandler( TRUE );\n" );
    }

  if ( !IsSGraph( n ) ) {
    if ( n->funct != NULL ) {
      if ( cfn == NULL ) {
        FPRINTF( output, "SdbxState.Function = \"%s\";\n", n->funct );
        cfn = n->funct;
        }
      else if ( strcmp( n->funct, cfn ) != 0 )
        FPRINTF( output, "SdbxState.Function = \"%s\";\n", n->funct );
      }

    if ( n->file != NULL ) {
      if ( cfl == NULL ) {
        FPRINTF( output, "SdbxState.File = \"%s\";\n", n->file ); 
        cfl = n->file;
	}
      else if ( strcmp( n->file, cfl ) != 0 )
        FPRINTF( output, "SdbxState.File = \"%s\";\n", n->file ); 
      }

    FPRINTF( output, "SdbxState.Line = %d;\n",  n->line );
    }
  
  if ( IsGraph(n) && !IsSGraph(n) ) {
    FPRINTF( output, "SdbxScopeSize = %d;\n", n->sdbx );

    if ( n->sdbx > 0 )
      FPRINTF( output, "SdbxScope = SdbxScope%s;\n", n->G_NAME );

    FPRINTF( output, "SdbxMonitor( SDBX_PUSH );\n" );
    UpdateSdbxScopeNames( n );
    FPRINTF( output, "SdbxHandler( FALSE );\n" );
    }

  if ( IsSGraph( n ) ) {
    FPRINTF( output, "SdbxMonitor( SDBX_SENTER );\n" );
    UpdateSdbxScopeNames( n );
    }
}
