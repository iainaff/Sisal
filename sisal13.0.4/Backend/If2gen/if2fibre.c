/* if2fibre.c,v
 * Revision 12.7  1992/11/04  22:05:01  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:00  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void PrintReadOp();
static void PrintWriteOp();

/**************************************************************************/
/* LOCAL  **************      PrintReadRoutine     ************************/
/**************************************************************************/
/* PURPOSE: PRINT READ ROUTINE FOR i TO output.                           */
/**************************************************************************/

static void PrintReadRoutine( i )
PINFO i;
{
  if ( i->LibNames ) {
    FPRINTF(output,"/* library %s %s */\n",i->tname, i->rname );
  } else {

    FPRINTF( output, "\nstatic %s %s()\n", i->tname, i->rname );
    FPRINTF( output, "{\n" );

    if ( IsBasic( i ) && !IsBRecord( i ) )
      FPRINTF( output, "  register %s val0;\n", i->tname ); 
    else
      FPRINTF( output, "  %s val0;\n", i->tname ); 

    PrintReadOp( 2, "val0", i );

    FPRINTF( output, "  return( val0 );\n" );
    FPRINTF( output, "}\n" );
  }
}


/**************************************************************************/
/* LOCAL  **************      PrintWriteRoutine    ************************/
/**************************************************************************/
/* PURPOSE: PRINT WRITE ROUTINE FOR i TO output.                          */
/**************************************************************************/

static void PrintWriteRoutine( i )
PINFO i;
{
  if ( i->LibNames ) {
    FPRINTF(output,"/* library void %s */\n", i->wname );
  } else {
    FPRINTF( output, "\nstatic void %s( val )\n", i->wname );
    FPRINTF( output, "%s val;\n", i->tname );
    FPRINTF( output, "{\n" );

    PrintWriteOp( 2, "val", i );

    FPRINTF( output, "}\n" );
  }
}


/**************************************************************************/
/* LOCAL  **************      PrintWriteUnion      ************************/
/**************************************************************************/
/* PURPOSE: PRINT UNION u'S WRITE ROUTINE TO output.                      */
/**************************************************************************/

static void PrintWriteUnion( indent, src, u )
int    indent;
char  *src;
PINFO  u;
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %s *un%d = (%s*) %s;\n", 
	   u->sname, indent, u->sname, src         );

  PrintIndentation( indent+2 );
  FPRINTF( output, "fprintf( FibreOutFd, \" %%d:\\n\", un%d->Tag);\n", indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "switch ( un%d->Tag ) {\n", indent );

  for ( c = 0, i = u->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "(un%d->Data.Fld%d)", indent, c );

    PrintIndentation( indent+4 );
    FPRINTF( output, "case %d:\n", c );

    /* WAS THE ROUTINE PRINTED? */
    if ( i->L_SUB->LibNames || i->L_SUB->touch1 || i->L_SUB->touch2 ) {
      PrintIndentation( indent+6 );
      FPRINTF( output, "%s( %s );\n", i->L_SUB->wname, buf );
      }
    else
      PrintWriteOp( indent+6, buf, i->L_SUB );

    PrintIndentation( indent+6 );
    FPRINTF( output, "break;\n" );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************     PrintWriteArray       ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY i'S WRITE ROUTINE TO output.                      */
/**************************************************************************/

static void PrintWriteArray( indent, src, i )
int    indent;
char  *src;
PINFO  i;
{
  char buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  register POINTER Base%d;\n", indent );

  PrintIndentation( indent );
  FPRINTF( output, "  register int     HiBound;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  register int     Lo%d;\n", indent );

  PrintIndentation( indent );
  FPRINTF( output, "  register ARRAYP arr = (ARRAYP) %s;\n", src );

  PrintIndentation( indent );
  FPRINTF( output, "  PrintIndent;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "Lo%d = arr->LoBound;\n", indent );

  PrintIndentation( indent );
  FPRINTF( output, 
     "  fprintf( FibreOutFd, \"[ %%d,%%d:\", Lo%d, Lo%d+(arr->Size)-1 );\n", 
     indent, indent );

/* PRINT REFERENCE COUNTS FOR DEBUG */
 PrintIndentation( indent );
 FPRINTF( output, "  fprintf( FibreOutFd, \" # DRC=%%d PRC=%%d\\n\", arr->RefCount, arr->Phys->RefCount );\n" );


  PrintIndentation( indent );
  FPRINTF( output, "  Indent++;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  Base%d = arr->Base;\n", indent );

  PrintIndentation( indent );
  FPRINTF( output, "  HiBound = Lo%d + arr->Size - 1;\n", indent );
  
  SPRINTF( buf, "(((%s*)Base%d)[Lo%d])", i->A_ELEM->tname, indent, indent );

  PrintIndentation( indent );
  FPRINTF( output, "  for ( /* NOTHING */; Lo%d <= HiBound; Lo%d++ ) {\n", 
	   indent, indent                                               );

  /* WAS THE ROUTINE PRINTED? */
  if ( i->A_ELEM->LibNames || i->A_ELEM->touch1 || i->A_ELEM->touch2 ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "%s( %s );\n", i->A_ELEM->wname, buf );
    }
  else
    PrintWriteOp( indent+4, buf, i->A_ELEM );

  PrintIndentation( indent );
  FPRINTF( output, "    }\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  Indent--;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  PrintIndent;\n" );

  PrintIndentation( indent );

  FPRINTF( output, "  fprintf( FibreOutFd, \"]\\n\" );\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************     PrintWriteRecord      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD r's WRITE ROUTINE TO output. NOTE, r MAY BE A    */
/*          BRECORD.                                                      */
/**************************************************************************/

static void PrintWriteRecord( indent, src, r )
int    indent;
char  *src;
PINFO  r;
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  if ( IsBRecord( r ) )
    FPRINTF( output, "register %s *rec%d = (%s*) &(%s);\n", 
	     r->sname, indent, r->sname, src             );
  else
    FPRINTF( output, "register %s *rec%d = (%s*) %s;\n", 
	     r->sname, indent, r->sname, src          );

  for ( c = 1, i = r->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "(rec%d->Fld%d)", indent, c );

    /* WAS THE ROUTINE PRINTED? */
    if ( i->L_SUB->LibNames || i->L_SUB->touch1 || i->L_SUB->touch2 ) {
      PrintIndentation( indent+2 );
      FPRINTF( output, "%s( %s );\n", i->L_SUB->wname, buf );
      }
    else
      PrintWriteOp( indent+2, buf, i->L_SUB );
    }

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************       PrintReadUnion      ************************/
/**************************************************************************/
/* PURPOSE: PRINT UNION u's READ ROUTINE TO output.                       */
/**************************************************************************/

static void PrintReadUnion( indent, dst, u )
int    indent;
char  *dst;
PINFO  u;
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %s un%d;\n", u->tname, indent );

  PrintIndentation( indent+2 );
  FPRINTF( output, "UBld( un%d, %s, 1, FibreInt );\n", indent, u->sname );

  PrintIndentation( indent+2 );
  FPRINTF( output, "switch ( FibreInt ) {\n" );

  for ( c = 0, i = u->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "case %d:\n", c );
    SPRINTF( buf, "((%s*)un%d)->Data.Fld%d", u->sname, indent, c );

    /* WAS THE ROUTINE PRINTED? */
    if ( i->L_SUB->LibNames || i->L_SUB->touch1 || i->L_SUB->touch2 ) {
      PrintIndentation( indent+6 );
      FPRINTF( output, "%s = %s();\n", buf, i->L_SUB->rname );
      }
    else
      PrintReadOp( indent+6, buf, i->L_SUB );

    PrintIndentation( indent+6 );
    FPRINTF( output, "break;\n" );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "default:\n" );

  PrintIndentation( indent+6 );
  FPRINTF( output, "FibreError( \"ILLEGAL UNION TAG\" );\n" );

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "%s = un%d;\n", dst, indent );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************      PrintReadRecord      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD r's READ ROUTINE TO output. NOTE, r MAY BE A     */
/*          BASIC RECORD.                                                 */
/**************************************************************************/

static void PrintReadRecord( indent, dst, r )
int    indent;
char  *dst;
PINFO  r;
{
  register PINFO i;
  register int   c;
	   char  buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "%s rec%d;\n", r->tname, indent );

  if ( !IsBRecord( r ) ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "RBld( rec%d, %s, 1 );\n", indent, r->sname );
    }

  for ( c = 1, i = r->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
    if ( IsBRecord( r ) )
      SPRINTF( buf, "rec%d.Fld%d", indent, c );
    else
      SPRINTF( buf, "((%s*)rec%d)->Fld%d", r->sname, indent, c );

    /* WAS THE ROUTINE PRINTED? */
    if ( i->L_SUB->LibNames || i->L_SUB->touch1 || i->L_SUB->touch2 ) {
      PrintIndentation( indent+2 );
      FPRINTF( output, "%s = %s();\n", buf, i->L_SUB->rname );
      }
    else
      PrintReadOp( indent+2, buf, i->L_SUB );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "%s = rec%d;\n", dst, indent );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


static void PrintGetArray( indent, dst, del, i )
int    indent;
char  *dst;
char  *del;
PINFO  i;
{
  char buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent );
  if ( IsBasic( i->A_ELEM ) && !IsBRecord( i->A_ELEM ) )
    FPRINTF( output, "  register %s val%d;\n", i->A_ELEM->tname, indent );
  else
    FPRINTF( output, "  %s val%d;\n", i->A_ELEM->tname, indent );

  PrintIndentation( indent );
  FPRINTF( output, "  FibreParse( INT_ );\n" );

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );
  PrintIndentation( indent );
  FPRINTF( output, "register int lob;\n" );
  PrintIndentation( indent );
  FPRINTF( output, "lob = FibreInt;\n" );
  PrintIndentation( indent );
  FPRINTF( output, "GET_LOOKAHEAD;\n" );
  PrintIndentation( indent );
  FPRINTF( output, "if ( LookAheadToken == COMMA_ ) {\n" );
  PrintIndentation( indent );
  FPRINTF( output, "  FibreParse( COMMA_ );\n" );
  PrintIndentation( indent );
  FPRINTF( output, "  FibreParse( INT_ );\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  OptABld( %s, lob, 1, FibreInt, %s );\n", dst, 
	   i->A_ELEM->tname );

  /* 12/12/91 CANN OptABld SETS Phys->Size TO THE ALLOCATED SIZE */
  /*               SO CLEAR IT, ALLOWING AGather TO INCREMENT IT */
  PrintIndentation( indent );
  FPRINTF( output,   "  ((ARRAYP)%s)->Phys->Size = 0;\n", dst ); 

  PrintIndentation( indent );
  FPRINTF( output, "} else {\n" );
  PrintIndentation( indent );
  FPRINTF( output, "  ABld( %s, lob, 1 );\n", dst );
  PrintIndentation( indent );
  FPRINTF( output, " }\n" );
  PrintIndentation( indent );
  FPRINTF( output, "FibreParse( COLON_ );\n" );
  PrintIndentation( indent );
  FPRINTF( output, "}\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  GET_LOOKAHEAD;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  while ( LookAheadToken != %s ) {\n", del );

  PrintIndentation( indent );
  FPRINTF( output, "    if ( LookAheadToken == SEMI_COLON_ )\n" );

  PrintIndentation( indent );
  FPRINTF( output, 
    "      FibreError( \"REPETITION FACILITY NOT IMPLEMENTED\" );\n" );

  SPRINTF( buf, "val%d", indent );

  /* WAS THE ROUTINE PRINTED? */
  if ( i->A_ELEM->LibNames || i->A_ELEM->touch1 || i->A_ELEM->touch2 ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "%s = %s();\n", buf, i->A_ELEM->rname );
    }
  else
    PrintReadOp( indent+4, buf, i->A_ELEM );

  PrintIndentation( indent );
  FPRINTF( output,  "    AGather( %s, %s, %s );\n",
	   dst, buf, i->A_ELEM->tname                );
 
  PrintIndentation( indent );
  FPRINTF( output, "    GET_LOOKAHEAD;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "    }\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


static void PrintGetString( indent, dst, i )
int    indent;
char  *dst;
PINFO  i;
{
  PrintIndentation( indent );
  FPRINTF( output, "ABld( %s, 1, 1 );\n", dst );

  PrintIndentation( indent );
  FPRINTF( output, "while ( FibreParse( ANY_ ) != STRING_TERM_ ) {\n" );

  PrintIndentation( indent );
  FPRINTF( output, "  AGather( %s, FibreChar, char );\n", dst );

  PrintIndentation( indent );
  FPRINTF( output, "  }\n" );
}


/**************************************************************************/
/* LOCAL  **************       PrintWriteOp        ************************/
/**************************************************************************/
/* PURPOSE: PRINT WRITE OPERATION FOR i TO output.                        */
/**************************************************************************/

static void PrintWriteOp( indent, src, i )
int    indent;
char  *src;
PINFO  i;
{
  switch ( i->type ) {
    case IF_UNION:
      PrintIndentation( indent );
      FPRINTF( output, "PrintIndent;\n" );

      PrintIndentation( indent );
      FPRINTF( output, "fprintf( FibreOutFd, \"(\" );\n" );

      PrintIndentation( indent );
      FPRINTF( output, "Indent++;\n" );

      PrintWriteUnion( indent, src, i );

      PrintIndentation( indent );
      FPRINTF( output, "Indent--;\n" );

      PrintIndentation( indent );
      FPRINTF( output, "PrintIndent;\n" );
      PrintIndentation( indent );

      FPRINTF( output, "fprintf( FibreOutFd, \")\\n\" );\n" );
      break;

    case IF_BRECORD:
    case IF_RECORD:
      PrintIndentation( indent );
      FPRINTF( output, "PrintIndent;\n" );

      PrintIndentation( indent );

      if ( IsBRecord( i ) )
        FPRINTF( output, "fprintf( FibreOutFd, \"<\\n\" );\n" );
      else
        FPRINTF( output, "fprintf( FibreOutFd, \"< # PRC=%%d\\n\", ((%s *)%s)->RefCount );\n", i->sname, src );

      PrintIndentation( indent );
      FPRINTF( output, "Indent++;\n" );

      PrintWriteRecord( indent, src, i );

      PrintIndentation( indent );
      FPRINTF( output, "Indent--;\n" );

      PrintIndentation( indent );
      FPRINTF( output, "PrintIndent;\n" );
      PrintIndentation( indent );

      FPRINTF( output, "fprintf( FibreOutFd, \">\\n\" );\n" );
      break;

    case IF_ARRAY:
      PrintWriteArray( indent, src, i );
      break;

    default:
      PrintIndentation( indent );
      FPRINTF( output, "%s( %s );\n", GetWriteFunction( i->type ), src );
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************       PrintReadOp         ************************/
/**************************************************************************/
/* PURPOSE: PRINT READ OPERATION FOR i TO output.                         */
/**************************************************************************/

static void PrintReadOp( indent, dst, i )
int    indent;
char  *dst;
PINFO  i;
{
  switch ( i->type ) {
    case IF_UNION:
      PrintIndentation( indent );
      FPRINTF( output, "FibreParse( UNIONB_ ); FibreParse( INT_ );\n" );
      PrintIndentation( indent );
      FPRINTF( output, "FibreParse( COLON_ );\n" );

      PrintReadUnion( indent, dst, i );

      PrintIndentation( indent );
      FPRINTF( output, "FibreParse( UNIONE_ );\n" );
      break;

    case IF_BRECORD:
    case IF_RECORD:
      PrintIndentation( indent );
      FPRINTF( output, "FibreParse( RECORDB_ );\n" );

      PrintReadRecord( indent, dst, i );

      PrintIndentation( indent );
      FPRINTF( output, "FibreParse( RECORDE_ );\n" );
      break;

    case IF_ARRAY:
      PrintIndentation( indent );
      FPRINTF( output, "switch ( FibreParse( ANY_ ) ) {\n" );

      /* ------------------------------------------------------------ */
      /* Normal array format					      */
      /* ------------------------------------------------------------ */
      PrintIndentation( indent );
      FPRINTF( output, "  case ARRAYB_:\n" );
      PrintGetArray( indent+4, dst, "ARRAYE_", i );
      PrintIndentation( indent );
      FPRINTF( output, "    break;\n" );

      /* ------------------------------------------------------------ */
      /* Stream array format					      */
      /* ------------------------------------------------------------ */
      PrintIndentation( indent );
      FPRINTF( output, "  case STREAMB_:\n" );
      PrintGetArray( indent+4, dst, "STREAME_", i );
      PrintIndentation( indent );
      FPRINTF( output, "    break;\n" );

      /* ------------------------------------------------------------ */
      /* String array format (only for array[character]		      */
      /* ------------------------------------------------------------ */
      PrintIndentation( indent );
      FPRINTF( output, "  case STRING_START_:\n" );
      if ( i->A_ELEM->type == IF_CHAR )
        PrintGetString( indent+4, dst, i );
      else {
        PrintIndentation( indent );
        FPRINTF( output,
	 "    FibreError( \"STRING DELIMITER WAS NOT EXPECTED\" );\n" );
        }
      PrintIndentation( indent );
      FPRINTF( output, "    break;\n" );

      /* ------------------------------------------------------------ */
      /* Anything else is an error				      */
      /* ------------------------------------------------------------ */
      PrintIndentation( indent );
      FPRINTF( output, "  default:\n" );
      PrintIndentation( indent );
      FPRINTF( output, 
	       "    FibreError( \"ARRAY DELIMITER EXPECTED\" );\n" );

      PrintIndentation( indent );
      FPRINTF( output, "  }\n" );
      break;

    default:
      PrintIndentation( indent );
      FPRINTF( output, "%s( %s );\n", GetReadFunction( i->type ), dst );
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************   PrintReadFibreInputs    ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE FIBRE INPUT ROUTINES FOR FUNCTION f TO output. MOST */
/*          OF THE ROUTINES ARE INLINED.  ALL UNIONS ARE ASSUMED TO BE    */
/*          RECURSIVE.                                                    */
/**************************************************************************/

void PrintReadFibreInputs( f )
PNODE f;
{
  register PINFO i;
  register PINFO mi;
  register int   c;
	   char  buf[100];

  FPRINTF( output, "\n#undef GatherCopyInfo\n" );

  mi = f->info;

  ClearTouchFlags();

  /* DUMP OUTERMOST FIBRE AGGREAGATE READ ROUTINES */
  for ( i = mi->F_IN; i != NULL; i = i->L_NEXT ) {
    if ( i->L_SUB->LibNames || i->L_SUB->rname == NULL || i->L_SUB->touch1 )
      continue;

    TouchTheUnions( i->L_SUB );   /* UNIONS CAN BE RECURSIVE */
    PrintReadRoutine( i->L_SUB );

    i->L_SUB->touch1 = TRUE;
    }

  /* DUMP THE TOUCHED UNION READ ROUTINES: UNIONS CAN BE RECURSIVE!!! */
  for ( i = ihead; i != NULL; i = i->next ) {
    if ( !(i->touch2) )
      continue;

    if ( i->touch1 )
      continue;

    PrintReadRoutine( i );
    }

  /* DUMP THE INPUT DRIVER */
  FPRINTF( output, "\n%s ReadFibreInputs()\n", mi->tname );
  FPRINTF( output, "{\n" );
  FPRINTF( output, "  register %s *args = ", mi->sname );
  FPRINTF( output, "(%s*) Alloc( sizeof( %s ) );\n", mi->sname, mi->sname );

  for ( c = 1, i = mi->F_IN; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "args->In%d", c );

    if ( i->L_SUB->LibNames || i->L_SUB->touch1 )
      FPRINTF( output, "  %s = %s();\n", buf, i->L_SUB->rname );
    else
      PrintReadOp( 2, buf, i->L_SUB );
    }

  FPRINTF( output, "  return( (%s) args );\n}\n", mi->tname );

  ClearTouchFlags();

  FPRINTF( output, "\n#ifdef CInfo" );
  FPRINTF( output, "\n#define GatherCopyInfo 1" );
  FPRINTF( output, "\n#endif\n" );
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* PrintTypeWriters						*/
/* Purpose: Print out any output routines needed by the main	*/
/*	    routine, any peeks, and any trace routines		*/
/* ------------------------------------------------------------ */
void PrintTypeWriters(f)
     PNODE	f;
{
  register PINFO i;
  register PINFO mi;

  ClearTouchFlags(); /* 1-3 */

  /* ------------------------------------------------------------ */
  /* Touch types for the entry function (if any)		  */
  /* ------------------------------------------------------------ */
  if ( f != NULL ) {

    mi = f->info;

    /* DUMP OUTERMOST FIBRE WRITE ROUTINES */
    for ( i = mi->F_OUT; i != NULL; i = i->L_NEXT ) {
      if ( i->L_SUB->LibNames || i->L_SUB->rname == NULL || i->L_SUB->touch1 )
	continue;

      TouchTheUnions( i->L_SUB ); /* UNIONS CAN BE RECURSIVE */
      PrintWriteRoutine( i->L_SUB );

      i->L_SUB->touch1 = TRUE;
    }
  }

  /* ------------------------------------------------------------ */
  /* Touch types for the traced functions (if any)		  */
  /* ------------------------------------------------------------ */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( !f->trace ) continue;

    mi = f->info;
    for ( i = mi->F_IN; i != NULL; i = i->L_NEXT ) {
      if ( i->L_SUB->LibNames || i->L_SUB->rname == NULL || i->L_SUB->touch1 )
	continue;

      TouchTheUnions( i->L_SUB ); /* UNIONS CAN BE RECURSIVE */
      PrintWriteRoutine( i->L_SUB );

      i->L_SUB->touch1 = TRUE;
    }
  }

  /* ------------------------------------------------------------ */
  /* PROCESS PEEK ROUTINES!!!					  */
  /* ------------------------------------------------------------ */
  for ( i = ihead; i != NULL; i = i->next ) {
    if ( i->LibNames || i->rname == NULL || i->touch1 )
      continue;

    /* RECURSIVE UNION: ALREADY TOUCHED */
    if ( i->touch2 )
      continue;

    if ( !(i->touch6) )
      continue;

    TouchTheUnions( i );	/* UNIONS CAN BE RECURSIVE */
    PrintWriteRoutine( i );

    i->touch1 = TRUE;
  }

  /* ------------------------------------------------------------ */
  /* DUMP THE TOUCHED UNION WRITE ROUTINES: UNIONS CAN BE	  */
  /* RECURSIVE!!!						  */
  /* ------------------------------------------------------------ */
  for ( i = ihead; i != NULL; i = i->next ) {
    if ( !(i->touch2) )
      continue;

    if ( i->LibNames || i->touch1 )
      continue;

    PrintWriteRoutine( i );
  }

  /* ClearTouchFlags(); */

}


/**************************************************************************/
/* LOCAL  **************  PrintWriteFibreOutputs   ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE FIBRE OUTPUT ROUTINES FOR FUNCTION f TO output.     */
/*          MOST OF THE ROUTINES ARE INLINED. ALL UNIONS ARE ASSUMED TO   */
/*          BE RECURSIVE.                                                 */
/**************************************************************************/

void PrintWriteFibreOutputs( f )
     PNODE 	f;
{
  register PINFO i;
  register PINFO mi;
  register int   c;
  char	   buf[100];


  /* DUMP THE OUTPUT DRIVER */
  mi = f->info;

  FPRINTF( output, "\nvoid WriteFibreOutputs( args )\n");
  FPRINTF( output, "%s args;\n{\n", mi->tname );
  FPRINTF( output, "  register %s *p = (%s*) args;\n", mi->sname, mi->sname );

  for ( c = 1, i = mi->F_OUT; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "(p->Out%d)", c );

    if ( i->L_SUB->LibNames || i->L_SUB->touch1 ) {
      FPRINTF( output, "  %s( %s );\n", i->L_SUB->wname, buf );
    } else {
      PrintWriteOp( 2, buf, i->L_SUB );
    }
  }

  FPRINTF( output, "}\n" );
}

/**************************************************************************/
/* LOCAL  **************  PrintWriteFibreInputs   ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE FIBRE OUTPUT ROUTINES FOR FUNCTION f TO output.     */
/*          MOST OF THE ROUTINES ARE INLINED. ALL UNIONS ARE ASSUMED TO   */
/*          BE RECURSIVE.						  */
/*  New by miller */
/**************************************************************************/

void PrintWriteFibreInputs( f )
     PNODE 	f;
{
  register PINFO i;
  register PINFO mi;
  register int   c;
	   char  buf[100];

  /* DUMP THE DRIVER */
  mi = f->info;
  FPRINTF( output, "\nvoid Write_%s_FibreInputs( args )\n",
	  (f->funct == NULL)? f->G_NAME : f->funct);
  FPRINTF( output, "%s args;\n{\n", mi->tname );
  FPRINTF( output, "  register %s *p = (%s*) args;\n", mi->sname, mi->sname );

  for ( c = 1, i = mi->F_IN; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "(p->In%d)", c );

    if ( i->L_SUB->LibNames || i->L_SUB->touch1 )
      FPRINTF( output, "  %s( %s );\n", i->L_SUB->wname, buf );
    else
      PrintWriteOp( 2, buf, i->L_SUB );
    }

  FPRINTF( output, "}\n" );

}


/**************************************************************************/
/* GLOBAL **************          PrintPeek        ************************/
/**************************************************************************/
/* PURPOSE: PRINT PEEK NODE n TO OUTPUT.                                  */
/**************************************************************************/

void PrintPeek( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  FPRINTF( output, "{\n" );

  FPRINTF( output, "  FILE *tmpfd;\n" );
  FPRINTF( output, "  SLockParent;\n" );
  FPRINTF( output, "  tmpfd = FibreOutFd;\n" );
  FPRINTF( output, "  FibreOutFd = stderr;\n" );

  /* ------------------------------------------------------------ */
  /* Lay down a peek request for each input to the peek node	  */
  /* ------------------------------------------------------------ */
  FPRINTF( output, 
	   "  fprintf( FibreOutFd, \"\\n# ****** BEGIN PEEK ******\\n\" );\n");
  for ( i = n->imp; i != NULL; i = i->isucc ) {
    /* ------------------------------------------------------------ */
    /* Object name (if known)					    */
    /* ------------------------------------------------------------ */
    FPRINTF( output, "  fprintf( FibreOutFd, \"# *** %s\\n\" );\n", 
	    (i->name == NULL)? "unknown" : i->name              );

    /* ------------------------------------------------------------ */
    /* If there's no reader/writer defined, try the default writer  */
    /* ------------------------------------------------------------ */
    if ( i->info->wname == NULL ) {
      FPRINTF( output, "  %s( ", GetWriteFunction( i->info->type ) );
      PrintTemp( i );
      FPRINTF( output, " );\n" );
      continue;
    }

    /* ------------------------------------------------------------ */
    /* Use the declared writer (set touch6 to emit this routine)    */
    /* ------------------------------------------------------------ */
    FPRINTF( output, "  %s( ", i->info->wname );
    PrintTemp( i );
    FPRINTF( output, " );\n" );

    i->info->touch6 = TRUE;
  }

  if ( n->exp != NULL ) {
    FPRINTF( output, "  " );
    PrintTemp( n->exp );
    FPRINTF( output, " = 1; " );
    }

  FPRINTF( output, 
	   "  fprintf( FibreOutFd, \"# ******  END PEEK  ******\\n\" );\n");

  FPRINTF( output, "  FibreOutFd = tmpfd;\n" );
  FPRINTF( output, "  SUnlockParent;\n" );

  FPRINTF( output, "}\n" );

  PrintConsumerModifiers( indent, n );
}
