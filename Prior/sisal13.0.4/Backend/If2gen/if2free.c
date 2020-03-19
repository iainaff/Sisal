/* if2free.c,v
 * Revision 12.7  1992/11/04  22:05:01  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:00  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void PrintFreeOp();

/**************************************************************************/
/* LOCAL  **************      IsReadOnlyOuter      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE EXPORT AT PORT eport OF NODE n IS READ     */
/*          ONLY AT THE OUTERMOST LEVEL OF REFERENCE. THIS ROUTINE IS     */
/*          IS NOT GENERIC.                                               */
/**************************************************************************/

static int IsReadOnlyOuter( n, eport )
PNODE n;
int   eport;
{
  register PEDGE ee;
  register PEDGE e;
  register int   pm;
  register int   u;
  register int   w;

  u  = 0;
  pm = 0;
  w  = FALSE;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    pm = e->pm;

    /* BECAUSE OF ReduceArgAccessCost(..) */
    if ( e->dst->type == IFAssign ) {
      for ( ee = e->dst->exp; ee != NULL; ee = ee->esucc ) {
	if ( ee->wmark )
	  w = TRUE;

	u++;
	}

      continue;
      }

    if ( e->wmark )
      w = TRUE;

    u++;
    }

  /* THIS TEST IS NOT GENERIC, BUT IS STORAGE DEALLOCATION RELATED */
  if ( w && (pm < u) )
    return( FALSE );

  return( TRUE );
}


/**************************************************************************/
/* GLOBAL **************        GetFreeName        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE DEALLOCATION FUNCTION NAME FOR AGGREGATE i, WHICH  */
/*          IS MARKED FOR SEQUENTIAL DEALLOCATION (touch3) IF sequential  */
/*          IS TRUE, ELSE IT IS MARKED FOR PARALLEL DEALLOCATION (touch1).*/
/**************************************************************************/

char *GetFreeName( i )
PINFO i;
{
  register char *nm;

  switch ( i->type ) {
    case IF_UNION:
      i->touch1 = FALSE;
      i->touch3 = TRUE;  /* ALWAYS FOR PARALLEL USE */
      nm = i->fname2;
      break;

    case IF_RECORD:
    case IF_ARRAY:
      if ( sequential ) {
        i->touch1 = TRUE;
        nm = i->fname1;
      } else {
        i->touch3 = TRUE;
        nm = i->fname2;
        }
      
      break;

    default:
      Error2( "GetFreeName", "ILLEGAL TYPE" );
    }

  if ( !(i->touch2) )
    TouchTheUnions( i );

  i->touch2 = TRUE;

  return( nm );
}


/**************************************************************************/
/* LOCAL  **************     PrintFreeUnion        ************************/
/**************************************************************************/
/* PURPOSE: PRINT FREE UNION OPERATION u FOR UNION src TO output IN       */
/*          PARALLEL FORM.                                                */
/**************************************************************************/

static void PrintFreeUnion( indent, u, par, src )
int    indent;
PINFO  u;
char  *src;
int    par; /* TRUE ASSUMED!!! */
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %s *un%d = (%s*) %s;\n", u->sname, indent,
	   u->sname, src                                             );

  PrintIndentation( indent+2 );
  FPRINTF( output, "MY_LOCK( &un%d->Mutex );\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "RefCount = --(un%d->RefCount);\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "FLUSHLINE(&(un%d->RefCount));\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "CACHESYNC;\n");

  PrintIndentation( indent+2 );
  FPRINTF( output, "MY_UNLOCK( &un%d->Mutex );\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "FLUSHLINE( &un%d->Mutex );\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "CACHESYNC;\n");

  PrintIndentation( indent+2 );
  FPRINTF( output, "if ( RefCount == 0 ) {\n" );

  for ( i = u->R_FIRST; i != NULL; i = i->L_NEXT )
    if ( !IsBasic( i->L_SUB ) )
      break;

  if ( i != NULL ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "switch ( un%d->Tag ) {\n", indent );

    for ( c = 0, i = u->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
      if ( IsBasic( i->L_SUB ) )
        continue;

      SPRINTF( buf, "(un%d->Data.Fld%d)", indent, c );

      PrintIndentation( indent+6 );
      FPRINTF( output, "case %d:\n", c );

      if ( IsUnion( i->L_SUB ) ) {
        PrintIndentation( indent+8 );
        FPRINTF( output, "%s( %s );\n", i->L_SUB->fname2, buf );
        }
      else
        PrintFreeOp( indent+8, i->L_SUB, TRUE, buf );

      PrintIndentation( indent+8 );
      FPRINTF( output, "break;\n" );
      }

    PrintIndentation( indent+6 );
    FPRINTF( output, "default:\n" );
  
    PrintIndentation( indent+8 );
    FPRINTF( output, "break;\n" );

    PrintIndentation( indent+6 );
    FPRINTF( output, "}\n" );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "DeAlloc( (POINTER) un%d );\n", indent );

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************     PrintFreeRecord       ************************/
/**************************************************************************/
/* PURPOSE: PRINT FREE RECORD OPERATION r FOR src TO OUTPUT.              */
/**************************************************************************/

static void PrintFreeRecord( indent, r, par, src )
int    indent;
PINFO  r;
int    par;
char  *src;
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %s *rec%d = (%s*) %s;\n", r->sname, indent,
	   r->sname, src                                              );

  if ( par ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "MY_LOCK( &rec%d->Mutex );\n", indent );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "RefCount = --(rec%d->RefCount);\n", indent );

  if ( par ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "FLUSHLINE(&(rec%d->RefCount));\n", indent );

    PrintIndentation( indent+2 );
    FPRINTF( output, "CACHESYNC;\n");

    PrintIndentation( indent+2 );
    FPRINTF( output, "MY_UNLOCK( &rec%d->Mutex );\n", indent );

    PrintIndentation( indent+2 );
    FPRINTF( output, "FLUSHLINE( &rec%d->Mutex );\n", indent );

    PrintIndentation( indent+2 );
    FPRINTF( output, "CACHESYNC;\n");
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "if ( RefCount == 0 ) {\n" );

  for ( c = 1, i = r->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
    if ( IsBasic( i->L_SUB ) )
      continue;

    SPRINTF( buf, "(rec%d->Fld%d)", indent, c );

    if ( IsUnion( i->L_SUB ) ) {
      PrintIndentation( indent+4 );
      FPRINTF( output, "%s( %s );\n", i->L_SUB->fname2, buf );
      }
    else
      PrintFreeOp( indent+4, i->L_SUB, par, buf );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "DeAlloc( (POINTER) rec%d );\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************      PrintFreeArray       ************************/
/**************************************************************************/
/* PURPOSE: PRINT FREE OPERATION i FOR ARRAY src TO output IN PARALLEL    */
/*          FORM IF par IS TRUE.                                          */
/**************************************************************************/

static void PrintFreeArray( indent, i, par, src )
int    indent;
PINFO  i;
int    par;
char  *src;
{
  char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  if ( !IsBasic( i->A_ELEM ) ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "register %s *First%d;\n", i->A_ELEM->tname, indent );

    PrintIndentation( indent+2 );
    FPRINTF( output, "register %s *Last%d;\n", i->A_ELEM->tname, indent );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "register PHYSP   phys;\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register ARRAYP arr = (ARRAYP) %s;\n", src );

  if ( par ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "MY_LOCK( &arr->Mutex );\n" );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "RefCount = --(arr->RefCount);\n" );

  if ( par ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "FLUSHLINE(&(arr->RefCount));\n" );

    PrintIndentation( indent+2 );
    FPRINTF( output, "CACHESYNC;\n");

    PrintIndentation( indent+2 );
    FPRINTF( output, "MY_UNLOCK( &arr->Mutex );\n" );

    PrintIndentation( indent+2 );
    FPRINTF( output, "FLUSHLINE( &arr->Mutex );\n" );

    PrintIndentation( indent+2 );
    FPRINTF( output, "CACHESYNC;\n");
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "if ( RefCount == 0 ) {\n" );

    PrintIndentation( indent+4 );
    FPRINTF( output, "phys = arr->Phys;\n" );

    if ( par ) {
      PrintIndentation( indent+4 );
      FPRINTF( output, "MY_LOCK( &phys->Mutex );\n" );
      }

    PrintIndentation( indent+4 );
    FPRINTF( output, "RefCount = --(phys->RefCount);\n" );

    if ( par ) {
      PrintIndentation( indent+4 );
      FPRINTF( output, "FLUSHLINE(&(phys->RefCount));\n" );

      PrintIndentation( indent+4 );
      FPRINTF( output, "CACHESYNC;\n");

      PrintIndentation( indent+4 );
      FPRINTF( output, "MY_UNLOCK( &phys->Mutex );\n" );

      PrintIndentation( indent+4 );
      FPRINTF( output, "FLUSHLINE( &phys->Mutex );\n" );

      PrintIndentation( indent+4 );
      FPRINTF( output, "CACHESYNC;\n");
      }

    PrintIndentation( indent+4 );
    FPRINTF( output, "if ( RefCount == 0 ) {\n" );

      if ( !IsBasic( i->A_ELEM ) ) {
        PrintIndentation( indent+6 );
        FPRINTF( output, "First%d = (%s*) (arr->Phys->Base);\n", 
		 indent, i->A_ELEM->tname                     );

        PrintIndentation( indent+6 );
        FPRINTF( output, "Last%d = First%d + arr->Phys->Size;\n", 
		 indent, indent                                );

        SPRINTF( buf, "(*First%d)", indent );

        PrintIndentation( indent+6 );
        FPRINTF( output, "for ( ;First%d < Last%d; First%d++ ) {\n", 
	         indent, indent, indent                           );

        if ( IsUnion( i->A_ELEM ) ) {
          PrintIndentation( indent+8 );
          FPRINTF( output, "%s( %s );\n", i->A_ELEM->fname2, buf );
          }
        else
          PrintFreeOp( indent+8, i->A_ELEM, par, buf );

        PrintIndentation( indent+8 );
        FPRINTF( output, "}\n" );
        }

      PrintIndentation( indent+6 );
      FPRINTF( output, "DeAlloc( (POINTER) phys );\n" );

      PrintIndentation( indent+6 );
      FPRINTF( output, "}\n" );

  PrintIndentation( indent+4 );
  FPRINTF( output, "DeAlloc( (POINTER) arr );\n" );

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************       PrintFreeOp         ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE FREE OPERATION i FOR src TO output.                 */
/**************************************************************************/

static void PrintFreeOp( indent, i, par, src )
int    indent;
PINFO  i;
int    par;
char  *src;
{
  switch( i->type ) {
    case IF_UNION:
      PrintFreeUnion( indent, i, TRUE, src );  /* ALWAYS IN PARALLEL FORM! */
      break;

    case IF_RECORD:
      PrintFreeRecord( indent, i, par, src );
      break;

    case IF_ARRAY:
      PrintFreeArray( indent, i, par, src );
      break;

    default:
      Error2( "PrintFreeOp", "ILLEGAL TYPE" );
    }
}


/**************************************************************************/
/* LOCAL  **************     PrintFreeRoutine      ************************/
/**************************************************************************/
/* PURPOSE: PRINT FREE ROUTINE FOR i TO output IN PARALLEL FORM IF pat IS */
/*          TRUE.                                                         */
/**************************************************************************/

static void PrintFreeRoutine( i, par )
PINFO i;
int   par;
{
  FPRINTF( output, "\nstatic void %s( ptr )\n", (par)? i->fname2 : i->fname1 );
  FPRINTF( output, "%s ptr;\n", i->tname );
  FPRINTF( output, "{\n" );
  FPRINTF( output, "  register int RefCount;\n" );

  if ( !par )
    FPRINTF( output, "  Sequential = TRUE;\n" );

  PrintFreeOp( 2, i, par, "ptr" );

  if ( !par )
    FPRINTF( output, "  Sequential = FALSE;\n" );

  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* GLOBAL **************     PrintFreeUtilities    ************************/
/**************************************************************************/
/* PURPOSE: PRINT AGGREGATE DEALLOCATION ROUTINES TO output. ONLY THOSE   */
/*          SYMBOL TABLE ENTRIES WITH touch2 EQUAL TO TRUE ARE PRINTED.   */
/**************************************************************************/

void PrintFreeUtilities()
{
  register PINFO i;

  for ( i = ihead; i != NULL; i = i->next ) {
    if ( !i->touch2 )
      continue;

    if ( IsBasic( i ) )
      continue;

    if ( IsUnion( i ) ) { /* ALWAYS FOR PARALLEL */
      PrintFreeRoutine( i, TRUE );
      continue;
      }

    if ( i->touch1 )
      PrintFreeRoutine( i, FALSE ); /* SEQUENTIAL */

    if ( i->touch3 )
      PrintFreeRoutine( i, TRUE );  /* PARALLEL */
    }
}


/**************************************************************************/
/* GLOBAL **************     PrintInputDeallocs    ************************/
/**************************************************************************/
/* PURPOSE: DEALLOCATE CANDIDATE AGGREGATES IMPORTED TO FUNCTION f        */
/*          (NAMED nm) THEIR OUTER DEMENSION OR SCOPE IS READ ONLY.       */
/**************************************************************************/

void PrintInputDeallocs( nm, indent, f )
char  *nm;
int    indent;
PNODE  f;
{
  register PINFO i;
  register int   eport;
  register int   ronly;
	   char  buf[100];

  SPRINTF( buf, "((%s*)args)->In", f->info->sname );

  for ( eport = 1, i = f->info->F_IN; i != NULL; i = i->L_NEXT, eport++ ) {
    if ( IsBasic( i->L_SUB ) )
      continue;

    if ( !IsReadOnlyOuter( f, eport ) )
      continue;

    /* IsReadOnlyOuter MEETS ONE CRITERIA OF IsReadOnly */
    ronly = FALSE;

    if ( bindtosisal )
      if ( GenIsReadOnly( f, eport ) )
	ronly = TRUE;

    if ( ronly ) {
      PrintIndentation( indent );
      FPRINTF( output, "if ( !(((ARRAYP)(args->In%d))->Mutable) ) {\n", eport);
      PrintIndentation( 2 );
      }

    PrintIndentation( indent );
    FPRINTF( output, "%s( %s%d );\n", GetFreeName(i->L_SUB), buf, eport );

    if ( ronly ) {
      PrintIndentation( indent+2 );
      FPRINTF( output, "args->In%d = (POINTER) NULL;\n", eport );
      PrintIndentation( indent+2 );
      FPRINTF( output, "}\n" );
      }
    }
}


/**************************************************************************/
/* GLOBAL **************     PrintOutputDeallocs   ************************/
/**************************************************************************/
/* PURPOSE: DEALLOCATE CANDIDATE AGGREGATES EXPORTED FROM FUNCTION f.     */
/**************************************************************************/

void PrintOutputDeallocs( indent, f )
int   indent;
PNODE f;
{
  register PINFO i;
  register PEDGE ii;
  register int   c;
	   char  buf[100];

  SPRINTF( buf, "((%s*)args)->Out", f->info->sname );

  for ( c = 1, i = f->info->F_OUT; i != NULL; i = i->L_NEXT, c++ ) {
    if ( IsBasic( i->L_SUB ) )
      continue;

    if ( IsXGraph( f ) && f->xmark ) {
      if ( (ii = FindImport( f, c )) == NULL )
	Error2( "PrintOutputDeallocs", "xmark FindImport FAILED!!!" );

      /* DON'T TRY AND FREE PREBUILD OUTPUT VALUES FROM XGraphs */
      if ( ii->xmark )
	continue;
      }

    PrintIndentation( indent );
    FPRINTF( output, "%s( %s%d );\n", GetFreeName(i->L_SUB), buf, c );
    }
}
