/* if2print.c,v
 * Revision 12.7  1992/11/04  22:05:02  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************   PrintDivByZeroCheck     ************************/
/**************************************************************************/
/* PURPOSE: PRINT indent BLANKS TO output.                                */
/**************************************************************************/

static void PrintDivByZeroCheck( n )
PNODE n;
{
  char buf[200];

  if ( sdbx ) {
    FPRINTF( output, "SdbxDivByZeroCheck( " );
    PrintTemp( n->imp->isucc );
    FPRINTF( output, " );\n" );
    return;
    }

  FPRINTF( output, "DivByZeroCheck( " );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", \"%s\", \"%s\", %s );\n", 
	   (n->imp->name == NULL)? "unknown" : n->imp->name,
	   (n->imp->isucc->name == NULL)? "unknown" : n->imp->isucc->name,
           GetSisalInfo( n, buf )                                      );
}


/**************************************************************************/
/* GLOBAL **************      PrintIndentation     ************************/
/**************************************************************************/
/* PURPOSE: PRINT indent BLANKS TO output.                                */
/**************************************************************************/

void PrintIndentation( indent )
int indent;
{
  register int i;

  if (indent < 40) {
    for ( i = 1; i <= indent; i++ ) (void)fputc( ' ', output );
  }
}


/**************************************************************************/
/* GLOBAL **************         PrintTemp         ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE TEMPORARY DEFINED BY EDGE e TO output. NOTE e MIGHT */
/*          DEFINE A CONSTANT.                                            */
/**************************************************************************/

void PrintTemp( e )
PEDGE e;
{
    register PTEMP t;

    if ( IsConst( e ) ) {
        if ( IsChar( e->info ) )
            FPRINTF( output, "\'%s\'", e->CoNsT );
        else
            FPRINTF( output, "(%s)", e->CoNsT );

	return;
	}

    if ( (t = e->temp) == NULL ) {
	if ( e->src->type == IFAIndexMinus ) {
	  PrintTemp( e->src->imp );

	  if ( IsConst( e->src->imp->isucc ) )
	    if ( atoi( e->src->imp->isucc->CoNsT ) == 0 )
	      return;

	  FPRINTF( output, "-" );
	  PrintTemp( e->src->imp->isucc );
	  return;
	  }

	if ( e->src->type == IFAIndexPlus ) {
	  PrintTemp( e->src->imp );

	  if ( IsConst( e->src->imp->isucc ) )
	    if ( atoi( e->src->imp->isucc->CoNsT ) == 0 )
	      return;

	  FPRINTF( output, "+" );
	  PrintTemp( e->src->imp->isucc );
	  return;
	  } 

	UNEXPECTED( "missing temporary" );
	ERRORINFO( "e->if1line = %d", e->if1line );
	ERRORINFO( "e->dst->type = %d ", e->dst->type );
	ERRORINFO( "e->eport = %d", e->eport );
        return;
	}

    if ( !IsFunction( t->info ) ) {
	FPRINTF( output, "%s", e->temp->name );
	return;
	}

    if ( strcmp( t->name, "args" ) == 0 )
	FPRINTF( output, "((%s*)%s)->In%d", t->info->sname, t->name, t->fld );
    else
	FPRINTF( output, "((%s*)%s)->Out%d", t->info->sname, t->name, t->fld );
}


/**************************************************************************/
/* GLOBAL **************        PrintFldRef        ************************/
/**************************************************************************/
/* PURPOSE: PRINT STRUCTURE REFERENCE TO output.                          */
/**************************************************************************/

void PrintFldRef( sname, name, e, f, fid )
char  *sname;
char  *name;
PEDGE  e;
char  *f;
int    fid;
{
    FPRINTF( output, "((%s*)", sname );

    if ( e != NULL )
	PrintTemp( e );
    else
	FPRINTF( output, "%s", name );

    FPRINTF( output, ")->%s%d", f, fid );
}


/**************************************************************************/
/* GLOBAL **************         PrintAssgn        ************************/
/**************************************************************************/
/* PURPOSE: PRINT AN ASSIGNMENT OPERATION TO output: e = i;               */
/**************************************************************************/

void PrintAssgn( indent, e, i )
int   indent;
PEDGE e;
PEDGE i;
{
    if ( e->temp == i->temp )
	return;

    PrintIndentation( indent );

    PrintTemp( e );
    FPRINTF( output, " = " );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintFldAssgn       ************************/
/**************************************************************************/
/* PURPOSE: PRINT STRUCTURE FIELD ASSIGNMENT TO output.                   */
/**************************************************************************/

void PrintFldAssgn( indent, sname, name, e, f, fid, i )
int    indent;
char  *sname;
char  *name;
PEDGE  e;
char  *f;
int    fid;
PEDGE  i;
{
    PrintIndentation( indent );

    PrintFldRef( sname, name, e, f, fid );
    FPRINTF( output, " = " );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
}


/**************************************************************************/
/* GLOBAL **************         PrintMacro        ************************/
/**************************************************************************/
/* PURPOSE: PRINT MACRO macro REPRESENTING NODE n TO output.  THE MACRO   */
/*          FORMAT IS "macro( n->exp, n->imps... );".  IF s IS NOT NULL   */
/*          EACH IMPORT TEMPORARY NAME IS OF THE FORM "name%s" USING s    */
/*          AS THE SUFFIX.                                                */
/**************************************************************************/

void PrintMacro( indent, macro, n, s )
int    indent;
char  *macro;
PNODE  n;
char  *s;
{
    register PEDGE i;

    PrintIndentation( indent );

    FPRINTF( output, "%s( ", macro );
    PrintTemp( n->exp );

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	FPRINTF( output, ", " );
	PrintTemp( i );

	if ( s != NULL )
	    FPRINTF( output, "%s", s );
	}
	
    FPRINTF( output, " );\n" );
}

/**************************************************************************/
/* GLOBAL **************     PrintSetRefCount      ************************/
/**************************************************************************/
/* PURPOSE: PRINT SET REFERENCE COUNT OPERATION FOR AGGREGATE e WITH sr=v */
/*          TO OUTPUT.  IF is1 AND v IS 1, THEN THE OPERATION IS NOT      */
/*          PRINTED.                                                      */
/**************************************************************************/

void PrintSetRefCount( indent, e, v, is1 )
int   indent;
PEDGE e;
int   v;
int   is1;
{
  if ( v == 0 ) {
    rmsrcnt++;
    return;
    }

  /* if ( is1 && v == 1 ) {
    rmsrcnt++;
    return;
    } */

  PrintIndentation( indent );

  FPRINTF( output, "%s( ", GetSetRefCountName( e->info ) );
  PrintTemp( e );
  FPRINTF( output, ", %s, %d );\n", e->info->sname, v );
}


/**************************************************************************/
/* GLOBAL **************      PrintFreeCall        ************************/
/**************************************************************************/
/* PURPOSE: PRINT FREE CALL FOR AGGREGATE i TO output.                    */
/**************************************************************************/

void PrintFreeCall( indent, i )
int   indent;
PEDGE i;
{
  PrintIndentation( indent );
  FPRINTF( output, "%s( ", GetFreeName(i->info) );
  PrintTemp( i );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************   PrintConsumerModifiers  ************************/
/**************************************************************************/
/* PURPOSE: PRINT CONSUMER MODIFY OPERATIONS FOR THE IMPORTS OF NODE n.   */
/**************************************************************************/

void PrintConsumerModifiers( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    if ( i->cm != -1 && (!(i->dmark)) )
      continue;

    PrintFreeCall( indent, i );
    }
}


/**************************************************************************/
/* GLOBAL ************** PrintProducerLastModifiers ***********************/
/**************************************************************************/
/* PURPOSE: PRINT PRODUCER LAST ITERATION MODIFY OPERATIONS FOR THE       */
/*          EXPORTS OF NODE n.                                            */
/**************************************************************************/

void PrintProducerLastModifiers( indent, n )
int   indent;
PNODE n;
{
    register PEDGE e;
    register PEDGE ee;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( e->eport < 0 ) {
	    e->eport = -(e->eport);
	    continue;
	    }

	if ( e->pl > 0 ) {
	    PrintIndentation( indent );

	    FPRINTF( output, "%s( ", GetIncRefCountName( e->info ) );
	    PrintTemp( e );
	    FPRINTF( output, ", %s, %d );\n", e->info->sname, e->pl );

	    for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
		if ( ee->eport == e->eport )
		    ee->eport = -(ee->eport);
	    }
	}
}


/**************************************************************************/
/* GLOBAL **************   PrintProducerModifiers  ************************/
/**************************************************************************/
/* PURPOSE: PRINT PRODUCER MODIFY OPERATIONS FOR THE EXPORTS OF NODE n.   */
/**************************************************************************/

void PrintProducerModifiers( indent, n )
int   indent;
PNODE n;
{
    register PEDGE e;
    register PEDGE ee;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( e->eport < 0 ) {
	    e->eport = -(e->eport);
	    continue;
	    }

	if ( e->pm > 0 ) {
	    PrintIndentation( indent );

	    FPRINTF( output, "%s( ", GetIncRefCountName( e->info ) );
	    PrintTemp( e );
	    FPRINTF( output, ", %s, %d );\n", e->info->sname, e->pm );

	    for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
		if ( ee->eport == e->eport )
		    ee->eport = -(ee->eport);
	    }
	}
}


/**************************************************************************/
/* LOCAL  **************        PrintMinMax        ************************/
/**************************************************************************/
/* PURPOSE: PRINT Min OR Max NODE n TO output.                            */
/**************************************************************************/

static void PrintMinMax( indent, n )
int    indent;
PNODE  n;
{
    char buf[100];

    if ( n->imp->temp == n->exp->temp )            /* if ( A op B ) A = B */
        SPRINTF( buf, "Opt%s", GetMacro( n ) );
    else if ( n->imp->isucc->temp == n->exp->temp )/* if ( B op A ) B = A */
        SPRINTF( buf, "COpt%s", GetMacro( n ) );
    else
	SPRINTF( buf, "%s", GetMacro( n ) );

    PrintMacro( indent, buf, n, (char*)NULL );
}


/**************************************************************************/
/* LOCAL  **************        PrintDyadic        ************************/
/**************************************************************************/
/* PURPOSE: PRINT DYADIC NODE n TO OUTPUT.                                */
/**************************************************************************/

static void PrintDyadic( indent, n )
int    indent;
PNODE  n;
{
    char buf[100];
    char *op;

    if ( n->type == IFDiv )
      if ( bounds )
	PrintDivByZeroCheck( n );

    if ( IsDivByZero( n ) )
      op = "DivZero";
    else
      op = GetMacro( n );

    if ( n->imp->temp == n->exp->temp )                     /* A op= B    */
	SPRINTF( buf, "Opt%s", op );
    else                                                    /* C = A op B */
	SPRINTF( buf, "%s", op );

    PrintMacro( indent, buf, n, (char*)NULL );
}


/**************************************************************************/
/* LOCAL  **************       PrintDyadicCom      ************************/
/**************************************************************************/
/* PURPOSE: PRINT COMMUTATIVE DYADIC NODE n TO OUTPUT.                    */
/**************************************************************************/

static void PrintDyadicCom( indent, n, override )
int    indent;
PNODE  n;
char  *override;
{
    char buf[100];

    if ( n->imp->temp == n->exp->temp )                     /* A op= B    */
	SPRINTF( buf, "Opt%s", (override != NULL)? override : GetMacro( n ) );
    else if ( n->imp->isucc->temp == n->exp->temp )         /* B op= A    */
	SPRINTF( buf, "COpt%s", (override != NULL)? override : GetMacro( n ) );
    else                                                    /* C = A op B */
	SPRINTF( buf, "%s", (override != NULL)? override : GetMacro( n ) );

    PrintMacro( indent, buf, n, (char*)NULL );
}

/**************************************************************************/
/* LOCAL  **************    ExpandDollarFormula    ************************/
/**************************************************************************/
/* PURPOSE:  Expand the $vars found in the LoopSlice (and other?)	  */
/*	     formulas to the defined variables.				  */
/**************************************************************************/
static void ExpandDollarFormula(c,n,errorbuf)
     char	*c;
     PNODE	n;
     char	*errorbuf;
{
  errorbuf[0] = '\0';		/* Assume no errors */
     
  for(; *c; c++) {
    if ( *c == '$' ) {
      c++;
      switch ( *c ) {		/* Decode the $vars */
	/* ------------------------------------------------------------ */
       case 'p':		/* PROCESSORS */
       case 'P':
	FPRINTF( output, "(MAX_PROCS)" );
	break;

       case 'l':		/* Low of range */
       case 'L':
	PrintRangeLow(n->usucc);
	break;

       case 'h':		/* High of range */
       case 'H':
	PrintRangeHigh(n->usucc);
	break;

       case '\000':
	strcpy(errorbuf,"Premature end of formula");
	break;

       default:
	SPRINTF(errorbuf,"Bad $var ($%c) in formula", *c);
      }
    } else {
      (void)fputc(*c,output);
    }
  }
}

/**************************************************************************/
/* LOCAL  **************        IsComplex          ************************/
/**************************************************************************/
/* PURPOSE:								  */
/**************************************************************************/
static int IsComplex( g )
PNODE g;
{
  register PNODE n;

  for ( n = g->nsucc; n != NULL; n = n->nsucc ) {
    if ( IsLoop( n ) || IsForall( n ) )
      return( TRUE );

    if ( IsSelect( n ) ) {
      if ( IsComplex( n->S_ALT ) )
	return( TRUE );
      if ( IsComplex( n->S_CONS ) )
	return( TRUE );
      }
    }

  return( FALSE );
}

/**************************************************************************/
/* LOCAL  **************    PrintBuildSlices	   ************************/
/**************************************************************************/
/* PURPOSE: Prints out the code to invoke the loop slice scheduler	  */
/**************************************************************************/
static void PrintBuildSlices( indent, n )
int   indent;
PNODE n;
{
  register PNODE f;
  char	   buf[100];
  char	   ebuf[100];
  char	   *Style;

  int	   size;

  f = n->usucc->usucc->imp->src;

  size = LCMSize(n->usucc->usucc);

  /* ------------------------------------------------------------ */
  /* Select the appropriate style of slicer.  For Runtime	  */
  /* decision, we choose a generic slicer that takes the default  */
  /* action							  */
  /* ------------------------------------------------------------ */
  switch ( f->Style ) {
   case 'G':
    if ( IsComplex(f->F_BODY) )  { /* Can only slice complex for GSS */
      Style = "GSS";
      break;
    }
    PrintIndentation( indent );
    FPRINTF( output, "/* GSS Converted to BLOCK */\n");
    /* else fall through to BLOCK */
   case 'B':
    Style = "Block";
    break;
   case 'R':
    Style = "";
    break;
   case 'S':
    Style = "Strided";
    break;
#if defined(NON_COHERENT_CACHE)
   case 'C':
    Style = "Cached";
    break;
#endif
   default:
    SPRINTF(buf,"Bad Slice option for loop %d",f->ID);
    Error1(buf);
  }

  PrintIndentation( indent );
  FPRINTF( output, "%sBSlices( %s, %s, %s, %s",
	  Style,
	  (IsComplex( f->F_BODY ))? "SPAWN_COMPLEX" : "SPAWN_SIMPLE",
	  n->usucc->temp->info->sname, 
	  n->usucc->usucc->G_NAME,
	  n->usucc->temp->name
	  );
	   
  PrintIndentation( (int)(indent+strlen(Style)+9) );
  PrintRanges( n->usucc );

  /* ------------------------------------------------------------ */
  /* MinSlice parameter						  */
  /* ------------------------------------------------------------ */
  if ( MinSliceThrottle && f->MinSlice ) {
    FPRINTF( output, ",");
    ExpandDollarFormula(f->MinSlice,n,ebuf);
    if ( ebuf[0] ) {
      SPRINTF(buf,"%s -- Loop %d\n",ebuf,f->ID);
      Error1(buf);
    }
    FPRINTF( output, ",");
  } else {
    FPRINTF( output, ", 1," );
  }

  /* ------------------------------------------------------------ */
  /* Loop Slice formula						  */
  /* ------------------------------------------------------------ */
  if ( f->LoopSlice ) {
    ExpandDollarFormula(f->LoopSlice,n,ebuf);
    if ( ebuf[0] ) {
      SPRINTF(buf,"%s -- Loop %d\n",ebuf,f->ID);
      Error1(buf);
    }
  } else {
    FPRINTF( output, "LoopSlices" );
  }

  FPRINTF( output, ", %d",size );
  FPRINTF( output,", %d",f->norm);

  /* ------------------------------------------------------------ */
  FPRINTF( output, " );\n" );
}


static void PrintSaveSliceParam( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;
  register PNODE lpe;

  i   = n->imp;
  lpe = n->usucc;

  if ( i->info->type == IF_BUFFER ) {
    PrintIndentation( indent );

    if ( i->isucc->isucc == NULL )  /* ONLY THE LOWER BOUND */
      FPRINTF( output, "VecGathATInit( " );
    else
      FPRINTF( output, "VecRedATInit( " );

    PrintTemp( n->imp->isucc );
    FPRINTF( output, ", %s, ", n->imp->info->A_ELEM->A_ELEM->tname );
    PrintFldRef( lpe->temp->info->sname, lpe->temp->name, NULL_EDGE, 
                 "In", i->iport );
    FPRINTF( output, ", ((BUFFERP)" );
    PrintTemp( i );
    FPRINTF( output, ")" );

    if ( i->isucc->isucc != NULL ) { /* REDUCE CATENATE */
      FPRINTF( output, ", " );
      PrintTemp( i->isucc->isucc );
      }

    FPRINTF( output, " );\n" );
    }
  else
    PrintFldAssgn( indent, lpe->temp->info->sname, lpe->temp->name, NULL_EDGE, 
                   "In", i->iport, i                                   );
}


/**************************************************************************/
/* LOCAL  **************     PrintLoopPoolEnq     *************************/
/**************************************************************************/
/* PURPOSE: PRINT LoopPoolEnQ NODE n TO output.                           */
/**************************************************************************/

static void PrintLoopPoolEnq( indent, n )
int   indent;
PNODE n;
{
  register PNODE f;
  char	   *Style;
  char	   buf[100];

  f = n->usucc->imp->src;

  PrintSliceTaskInit( indent, n );

  PrintIndentation( indent );

  if ( n->type == IFOptLoopPoolEnq ) {
    FPRINTF( output, "OptSpawn%s( %s,", (n->Fmark ? "Fast":""),
				n->temp->info->sname );
  } else {
    /* ------------------------------------------------------------ */
    /* Select the appropriate style of slicer.  For Runtime	  */
    /* decision, we choose a generic slicer that takes the default  */
    /* action							  */
    /* ------------------------------------------------------------ */
    switch ( f->Style ) {
     case 'G':
      if ( IsComplex(f->F_BODY) )  { /* Can only slice complex for GSS */
	Style = "GSS";
	break;
      }
      FPRINTF( output, "	/* GSS Converted to BLOCK */\n");
      PrintIndentation( indent );
      /* else fall through to BLOCK */
     case 'B':
      Style = "Block";
      break;
     case 'R':
      Style = "";
      break;
     case 'S':
      Style = "Strided";
      break;
     default:
      SPRINTF(buf,"Bad Slice option for loop %d",f->ID);
      Error1(buf);
    }

    FPRINTF( output, "%sSpawn(",Style );
  }

  FPRINTF( output, " %s, %s, %s, %d, %d",
	   (IsComplex( f->F_BODY ))? "SPAWN_COMPLEX" : "SPAWN_SIMPLE",
	   n->usucc->G_NAME, n->temp->name, LCMSize(f->F_RET), f->norm      );
	   
  PrintRanges( n );
  FPRINTF( output, " );\n" );

  PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* LOCAL  **************      GenIsIntrinsic       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n CALLS ONE OF THE FOUR LOGICAL FUNCTIONS */
/**************************************************************************/

int GenIsIntrinsic( f )
PNODE f;
{
  return( (f->mark == 'i')? TRUE : FALSE );
}


/**************************************************************************/
/* LOCAL  **************   PrintIntrinsicFunction  ************************/
/**************************************************************************/
/* PURPOSE: PRINT LOGICAL FUNCTION n TO output.                           */
/**************************************************************************/

static void PrintIntrinsicFunction( indent, n, f )
int   indent;
PNODE n;
PNODE f;
{
  PrintIndentation( indent );

  /* NEW CANN 2/92 */
  FPRINTF( output, "_S%s%s( ", (IsReal(n->exp->info))? "f" : "", n->imp->CoNsT );

  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );

  if ( n->imp->isucc->isucc != NULL ) {
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc->isucc );
    }

  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* LOCAL  **************         PrintCall         ************************/
/**************************************************************************/
/* PURPOSE: PRINT CALL NODE n TO output. NOTE, n MIGHT CALL AN INTERFACE  */
/*          FUNCTION.                                                     */
/**************************************************************************/

static void PrintCall( indent, n )
int   indent;
PNODE n;
{
  register PNODE f;
  register int   eport;
  register PINFO ii;

  f = FindFunction( n->imp->CoNsT );

  if ( IsIGraph( f ) ) {
    if ( GenIsIntrinsic( f ) )
      PrintIntrinsicFunction( indent, n, f );
    else if ( f->mark == 'c' || f->mark == 'f' ) /* NEW CANN 2/92 */
      PrintInterfaceCall( indent, n, f );
    else { /* NEW CANN 2/92 */
      if ( sdbx )
        FPRINTF( output, "SdbxStateBackup = SdbxState;\n" );

      PrintIndentation( indent );
      FPRINTF( output, "Call( %s, %s );\n", n->imp->CoNsT, n->temp->name );

      if ( sdbx )
        FPRINTF( output, "SdbxState = SdbxStateBackup;\n" );
      }
    }
  else {
    if ( sdbx )
      FPRINTF( output, "SdbxStateBackup = SdbxState;\n" );

    PrintIndentation( indent );
    FPRINTF( output, "Call( %s, %s );\n", n->imp->CoNsT, n->temp->name );

    if ( sdbx )
      FPRINTF( output, "SdbxState = SdbxStateBackup;\n" );
    }

  /* CANN 2/92: TO FREE UNUSED COMPOUND CALL RESULTS */
  if ( f->mark == 'i' || f->mark == 'f' || f->mark == 'c' )
    goto SkipCleanUp;

  for ( eport = 1, ii = f->info->F_OUT; ii != NULL; ii = ii->L_NEXT, eport++ ) {
    if ( IsExport( n, eport ) )
      continue;

    if ( IsBasic( ii->L_SUB ) )
      continue;

    FPRINTF( output, "/* UNUSED */ %s( ", GetFreeName(ii->L_SUB) );
    FPRINTF( output, "((%s*)%s)->Out%d", 
	     n->temp->info->sname, n->temp->name, eport );
    FPRINTF( output, " );\n" );
    }

SkipCleanUp:
  PrintProducerModifiers( indent, n );
  PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************         PrintGraph        ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE NODES OF GRAPH g TO output.                         */
/**************************************************************************/

void PrintGraph( indent, g )
int   indent;
PNODE g;
{
    register PNODE n;
    register PEDGE i;
    register int   lk;
    register int   LastLine;
    register char  *LastFile;
    char	   *SourceLine;
	     char  buf[100];
    PINFO    info;

    if ( g->mark == 's' ) {
      FPRINTF( output, "#ifdef CInfo\n" );
      FPRINTF( output, "  SaveCopyInfo;\n" );
      FPRINTF( output, "#endif\n" );

      FPRINTF( output, "#ifdef FInfo\n" );
      FPRINTF( output, "  SaveFlopInfo;\n" );
      FPRINTF( output, "#endif\n" );

      if ( gdata )
        FPRINTF( output, "  InitGlobalData();\n" );
      }

    if ( !IsIGraph ( g ) )
      if ( g->flps != NULL ) {
	if ( g->flps[ARITHMETICS] > 0 )
	  FPRINTF( output, "IncFlopCountA(%d);\n", g->flps[ARITHMETICS] );
	if ( g->flps[LOGICALS] > 0 )
	  FPRINTF( output, "IncFlopCountL(%d);\n", g->flps[LOGICALS] );
	if ( g->flps[INTRINSICS] > 0 )
	  FPRINTF( output, "IncFlopCountI(%d);\n", g->flps[INTRINSICS] );
        }

    switch ( g->type ) {
	case IFXGraph:
	case IFLGraph:
	case IFLPGraph:
	    if ( sdbx && g->type == IFLPGraph )
	      Error2( "PrintGraph", "-sdbx AND IFLPGraph ENCOUNTERED!!!" );

	    PrintProducerModifiers( indent, g );
	    break;

	default:
	    break;
	}

    if ( sdbx && IsSGraph( g ) )
      SaveSdbxState( g );

    LastLine = 0;
    LastFile = "";
    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	/* ------------------------------------------------------------ */
	/* If the line has changed, output a source line comment	*/
	/* ------------------------------------------------------------ */
	if ( CodeComments
	    && n->line
	    && n->file
	    && ( n->line != LastLine || (strcmp(n->file,LastFile) != 0))
	    ) {
	  LastLine = n->line;
	  LastFile = n->file;

	  SourceLine = GetSourceLine(n);
	  if (SourceLine) FPRINTF( output,
				  "\n/* %s.%d: %s */\n",
				  n->file,
				  LastLine,
				  SourceLine);
	}


	/* ------------------------------------------------------------ */
	/* Output the debugger stuff					*/
	/* ------------------------------------------------------------ */
	if ( sdbx )
	  SaveSdbxState( n );

	/* ------------------------------------------------------------ */
	/* Output the code for the operation				*/
	/* ------------------------------------------------------------ */
        switch( n->type ) {
	    case IFTri:
	      PrintTri( indent, n );
	      break;

	    case IFFirstSum:
	      PrintFirstSum( indent, n );
	      break;

	    case IFFirstAbsMax:
	      PrintVMinMax( indent, n, "ifamax" );
	      break;

	    case IFFirstAbsMin:
	      PrintVMinMax( indent, n, "ifamin" );
	      break;

	    case IFFirstMin:
	      PrintVMinMax( indent, n, "ifmin" );
	      break;

	    case IFFirstMax:
	      PrintVMinMax( indent, n, "ifmax" );
	      break;

	    case IFLoopA:
	    case IFLoopB:
		PrintLoop( indent, n );
		break;

	    case IFForall:
		PrintForall( indent, n );
		break;

	    case IFSelect:
		PrintSelect( indent, n );
		break;

	    case IFTagCase:
		PrintTagCase( indent, n );
		break;

            case IFLeast:
                PrintYankedRed( indent, n, "Min" );
                break;

            case IFGreatest:
                PrintYankedRed( indent, n, "Max" );
                break;

            case IFProduct:
                if ( IsBoolean( n->gsucc->exp->info ) )
                    PrintYankedRed( indent, n, "And" );
                else
                    PrintYankedRed( indent, n, "Times" );

                break;

	    case IFBuildSlices:
		PrintBuildSlices( indent, n );
		break;

	    case IFSaveSliceParam:
		PrintSaveSliceParam( indent, n );
		break;

            case IFSum:
                if ( IsBoolean( n->gsucc->exp->info ) )
                    PrintYankedRed( indent, n, "Or" );
                else {
		    if ( IsConst( n->imp ) ) {
		      if ( strcmp( n->imp->CoNsT, "0" ) == 0 )
			break;

		      if ( strcmp( n->imp->CoNsT, "0.0" ) == 0 )
			break;

		      if ( strcmp( n->imp->CoNsT, "0.0D0" ) == 0 )
			break;

		      if ( strcmp( n->imp->CoNsT, "0.0d0" ) == 0 )
			break;

		      if ( strcmp( n->imp->CoNsT, "0.0E0" ) == 0 )
			break;

		      if ( strcmp( n->imp->CoNsT, "0.0e0" ) == 0 )
			break;
		      }

                    PrintYankedRed( indent, n, "Plus" );
		    }

                break;

	    case IFBRBuild:
		if ( n->exp->esucc == NULL )
		  if ( n->exp->dst->type == IFAStore ) {
		    /* ELIMINATE SUPERFLUOUS FIELD MOVES */
		    PrintBRAStore( indent, n, n->exp->dst->gsucc );
		    n->exp->dst->nmark = TRUE; /* (see IFAStore below) */
		    break;
		    }

		PrintBRBuild( indent, n );
		break;

            case IFAStore:
		if ( n->nmark ) /* ABRStore OPT (see IFBRBuild above) */
		  break;

		PrintAStore( indent, n, n->gsucc );
                break;

	    case IFDiv:
		if ( IsConst( n->imp->isucc ) )
		    if ( strcmp( n->imp->isucc->CoNsT, "2" ) == 0 )
			n->type = IFDiv2;

		PrintDyadic( indent, n );
		break;

	    case IFMod:
	    case IFMinus:
		PrintDyadic( indent, n );
		break;

	    case IFSaveCallParam:
                PrintFldAssgn( indent, n->usucc->imp->info->sname, 
			       n->usucc->temp->name, NULL_EDGE, "In", 
			       n->imp->iport, n->imp            );
	        break;

	    case IFError:
		PrintIndentation( indent );
		PrintTemp( n->exp );
	        FPRINTF( output, " = (%s) ", n->exp->info->tname );
		FPRINTF( output, 
		   "SisalError( %s, \"EXPLICIT ERROR VALUE GENERATED!\" );\n",
		   GetSisalInfoOnEdge( n->exp, buf ) );

		break;

	    case IFPlus:
		if ( n->exp == NULL )
		    break;

		if ( IsBoolean( n->exp->info ) )
		    PrintDyadicCom( indent, n, "Or" );
		else
		    PrintDyadicCom( indent, n, (char*)NULL );

		break;

	    case IFTimes:
		if ( n->exp == NULL )
		    break;

		if ( IsBoolean( n->exp->info ) )
		    PrintDyadicCom( indent, n, "And" );
		else
		    PrintDyadicCom( indent, n, (char*)NULL );

		break;

	    case IFMax:
	    case IFMin:
		PrintMinMax( indent, n );
		break;

	    case IFAbs:
		if ( n->exp->temp == n->imp->temp ) {
		    if ( ( strcmp( n->exp->temp->name, "double1" ) == 0 ) ||
		         ( strcmp( n->exp->temp->name, "double2" ) == 0 ) ||
		         ( strcmp( n->exp->temp->name, "double3" ) == 0 ) ) {
			PrintIndentation( indent );
			FPRINTF( output, "asm(\"absl _%s,_%s\");\n", 
				 n->exp->temp->name, n->exp->temp->name );

			break;
			}

		    if ( n->exp->info->type == IF_DOUBLE ||
			 n->exp->info->type == IF_REAL )
		      SPRINTF( buf, "OptF%s", GetMacro( n ) );
                    else
		      SPRINTF( buf, "Opt%s", GetMacro( n ) );
		} else {
		    if ( n->exp->info->type == IF_DOUBLE ||
			 n->exp->info->type == IF_REAL )
		      SPRINTF( buf, "F%s", GetMacro( n ) );
                    else
		      SPRINTF( buf, "%s", GetMacro( n ) );
                    }

		PrintMacro( indent, buf, n, (char*)NULL );
		break;

            case IFGreatEqual:
		if ( n->exp == NULL )
		    break;

		PrintMacro( indent, "GreatEqual", n, (char*)NULL );
		break;

            case IFGreat:
		if ( n->exp == NULL )
		    break;

		PrintMacro( indent, "Great", n, (char*)NULL );
		break;

	    case IFNotEqual:
	    case IFLessEqual:
	    case IFLess:
	    case IFEqual:
            case IFNot:
		if ( n->exp == NULL )
		    break;

	    case IFNeg:
	    case IFBool:
	    case IFChar:
	    case IFDouble:
	    case IFTrunc:
	    case IFSingle:
	    case IFFloor:
		PrintMacro( indent, GetMacro( n ), n, (char*)NULL );
		break;

	    case IFOptNoOp:
	        break;

	    case IFAssign:
		PrintAssgn( indent, n->exp, n->imp );
		break;

	    case IFExp:
		PrintMacro( indent, "Pow", n, (char*)NULL );
		break;

	    case IFInt:
		if ( IsDouble( n->imp->info ) || IsReal( n->imp->info ) )
		    PrintMacro( indent, "Floor", n, " + 0.5" );
		else
		    PrintMacro( indent, GetMacro( n ), n, (char*)NULL );

		break;

	    case IFLoopPoolEnq:
	    case IFOptLoopPoolEnq:
	        PrintLoopPoolEnq( indent, n );
	        break;

	    case IFCall:
		PrintCall( indent, n );
		break;

	    case IFUTagTest:
		PrintUTagTest( indent, n );
		break;

	    case IFUGetTag:
		PrintUGetTag( indent, n );
		break;

	    case IFUElement:
		PrintUElement( indent, n );
		break;

	    case IFBRReplace:
		PrintBRReplace( indent, n );
	        break;

	    case IFUBuild:
		PrintUBuild( indent, n );
		break;

	    case IFRBuild:
		PrintRBuild( indent, n );
                break;

	    case IFRElements:
		PrintRElements( indent, n );
		break;

	    case IFRReplace:
		PrintRReplace( indent, n );
		break;

	    case IFPeek:
		PrintPeek( indent, n );
		break;

	    case IFPrefixSize:
	      /* Just pretend that PrefixSize is the same as ASize */
	      n->type = IFASize;
	      /* Fall through... */
	    case IFASize:
            case IFALimL:
	    case IFALimH:
		PrintMacro( indent, GetMacro( n ), n, (char*)NULL );
                PrintConsumerModifiers( indent, n );
		break;

	    case IFAElement:
		if ( bounds ) 
		   PrintBoundsCheck( indent, n, n->imp, n->imp->isucc );

            case IFOptAElement:
		/* NOTE -B (-bounds) IN OSC RESULTS in aimp (-a) == FALSE */
		if ( n->exp->esucc == NULL && 
		     n->exp->dst->type == IFBRElements ) {
		    n->exp->dst->nmark = TRUE;
		    break;
		    }

		PrintOptAElement( indent, n );
		break;

	    case IFBRElements:
		/* ALREADY DONE BY IFOptAElement ABOVE??? */
		if ( n->nmark ) {
		  PrintBROptAElement( indent, n );
		  break;
		  }

		PrintBRElements( indent, n );
		break;


	    case IFGetArrayBase:
		PrintGABase( indent, n );
		break;

	    case IFARemL:
	    case IFARemH:
		if ( bounds )
		   PrintBoundsCheck( indent, n, n->imp, NULL_EDGE );

                if ( n->exp->temp != n->imp->temp )
	            PrintAssgn( indent, n->exp, n->imp );

		if ( n->imp->omark1 )
                    PrintArrayMacro( indent, "Opt", GetMacro( n ), n );
		else
                    PrintArrayMacro( indent, "", GetMacro( n ), n );

                PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE );
		break;

	    case IFAAdjust:
		if ( bounds ) {
		   PrintBoundsCheck( indent, n, n->imp, n->imp->isucc );
		   PrintBoundsCheck( indent, n, n->imp, n->imp->isucc->isucc );
		   }

	    case IFASetL:
                if ( n->exp->temp != n->imp->temp )
	            PrintAssgn( indent, n->exp, n->imp );

                PrintArrayMacro( indent, "", GetMacro( n ), n );
                PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE );
		break;

	    case IFOptAReplace:
	    case IFAReplace:
		if ( bounds ) 
		   PrintBoundsCheck( indent, n, n->imp, n->imp->isucc );

		PrintAReplace( indent, n );
		break;

	    case IFABuild:
		PrintABuild( indent, n );
		break;

	    case IFABuildAT:
	    case IFABuildATDV:
	    case IFABuildATDVI:
		PrintABuildAT( indent, n );
		break;

	    case IFAAddLAT:
	    case IFAAddHAT:
	    case IFAAddHATDV:
	    case IFAAddHATDVI:
	    case IFAAddLATDV:
	    case IFAAddLATDVI:
		PrintAAddHLAT( indent, n );
		break;

	    case IFACatenateAT:
	    case IFACatenateATDV:
	    case IFACatenateATDVI:
		PrintACatenateAT( indent, n );
		break;

	    case IFShiftBuffer:
                if ( n->imp->temp == n->exp->temp )
	            PrintArrayMacro( indent, "Opt", GetMacro( n ), n );
                else
	            PrintArrayMacro( indent, "", GetMacro( n ), n );

		break;

            case IFPSAllocCond:
		PrintPSAlloc( indent, n, "Cond" );
		break;
            case IFPSAllocOne:
		PrintPSAlloc( indent, n, "One" );
		break;
            case IFPSAllocTwo:
		PrintPSAlloc( indent, n, "Two" );
		break;
            case IFPSAllocSpare1:
		PrintPSAlloc( indent, n, "Spare1" );
		break;
            case IFPSAllocSpare2:
		PrintPSAlloc( indent, n, "Spare2" );
		break;

            case IFPSFreeOne:
		PrintPSFree( indent, n, "One", g );
		break;
            case IFPSFreeTwo:
		PrintPSFree( indent, n, "Two", g );
		break;
            case IFPSFreeSpare1:
		PrintPSFree( indent, n, "Spare1", g );
		break;
            case IFPSFreeSpare2:
		PrintPSFree( indent, n, "Spare2", g );
		break;

            case IFPSManager:
		PrintPSManager( indent, n, "" );
		break;
            case IFPSManagerSwap:
		PrintPSManager( indent, n, "Swap" );
		break;

            case IFPSScatter:
		PrintPSScatter( indent, n );
		break;

	    case IFRagged:
	       if ( n->exp->dst->cmark )
		 break;

	       PrintRagged( indent, n );
	       break;

	    case IFPSMemAllocDVI:
	       PrintPSMemAllocDVI( indent, n );
	       break;

	    case IFMemAllocDVI:
	    case IFMemAllocDV:
	    case IFMemAlloc:
		if ( n->cmark )
		    break;

		PrintMemAlloc( indent, n );
		break;

	    case IFAAddH:
		PrintAAddH( indent, n );
		break;

	    case IFNoOp:
		if ( IsRecord( n->imp->info ) )
		    PrintRecordNoOp( indent, n );
		else if ( IsArray( n->imp->info ) )
	            PrintArrayNoOp( indent, n );

		break;

	    case IFDefArrayBuf:
		Error2( "PrintGraph", "IFDefArrayBuf NODE ENCOUNTERED" );
		break;

	    case IFAIndexPlus:
	    case IFAIndexMinus:
	      break;

	    default:
		UNEXPECTED( "Unknown node" );
	    }

	if ( sdbx )
	  UpdateSdbxScopeNames( n );
        }

    if ( sdbx && IsSGraph( g ) )
      FPRINTF( output, "SdbxMonitor( SDBX_SEXIT );\n" );

    /* if ( freeall || !(g->emark && standalone) ) */
    /* NEW CANN 2/92 */
    if ( freeall || (g->mark != 'e') || (g->mark == 'e' && g->bmark) )

      for ( n = g->G_NODES; n != NULL; n = n->nsucc )
        if ( n->type == IFBuildSlices ) {
	  PrintIndentation( indent );
	  FPRINTF( output, "DeAlloc( ((%s *)%s)->FirstAR );\n", 
		   n->usucc->temp->info->sname, n->usucc->temp->name );
	  }

    switch ( g->type ) {
	case IFLPGraph:
	case IFXGraph:
	case IFLGraph:
            PrintConsumerModifiers( indent, g );

            for ( lk = FALSE, i = g->imp; i != NULL; i = i->isucc )
		if ( !IsArray( i->info ) )
	            lk = TRUE;

	    if ( IsLPGraph( g ) && lk ) {
		PrintIndentation( indent );
		FPRINTF( output, "LockParent;\n" );
		}

            info = NULL;
	    for ( i = g->imp; i != NULL; i = i->isucc ) {
		if ( i->iport == 0 )
		    continue;

		if ( IsLPGraph( g ) ) {
		    if ( !IsArray( i->info ) )
		    {
                        PrintSumOfTerms( indent, i );
			info = i->dst->info;
		    }

		    continue;
                    }

		PrintFldAssgn( indent, g->info->sname, "args", 
			       NULL_EDGE, "Out", i->iport, i     );
                }

            if ( IsLPGraph( g ) && lk && (info != NULL) ) {
		FPRINTF(output, "CACHESYNC;\n");
                }

	    if ( IsLPGraph( g ) && lk ) {
		PrintIndentation( indent );
		FPRINTF( output, "UnlockParent;\n" );
		}

	    break;

	default:
	    break;
	}
}
