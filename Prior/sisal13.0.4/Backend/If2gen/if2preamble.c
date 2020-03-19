/* if2preamble.c,v
 * Revision 12.7  1992/11/04  22:05:02  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:02  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


int sequential = FALSE;                 /* GENERATING CODE FOR THE MASTER */
int recursive  = FALSE;                 /* GENERATING RECURSIVE CODE      */


/**************************************************************************/
/* LOCAL  **************     PrintConstants        ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE CONSTANTS BEGINNING WITH frst AND ENDING BEFORE lst.*/
/**************************************************************************/

static void PrintConstants( frst, lst )
PEDGE frst;
PEDGE lst;
{
  register PEDGE i1;
  register PEDGE i2;
  register int   comma1;
  register int   comma2;
	   char  buf[100];

  for ( i1 = frst; i1 != lst; i1 = i1->isucc ) {
    comma1 = (i1->isucc == lst)? ' ' : ',';

    if ( IsBRecord( i1->info ) ) {
      for ( i2 = i1->src->imp; i2 != NULL; i2 = i2->isucc ) {
        comma2 = (i2->isucc == NULL)? comma1 : ',';

        if ( IsChar( i2->info ) )
          SPRINTF( buf, "\'%s\'", i2->CoNsT );
        else
          SPRINTF( buf, "%s", i2->CoNsT );

        FPRINTF( output, "  %s%c\n", buf, comma2 );
	}

      continue;
      }

    if ( IsConst( i1 ) ) {
      if ( IsChar( i1->info ) )
        SPRINTF( buf, "\'%s\'", i1->CoNsT );
      else
        SPRINTF( buf, "%s", i1->CoNsT );
      }
    else
      SPRINTF( buf, "(POINTER) &%sData", i1->src->G_NAME );

    FPRINTF( output, "  %s%c\n", buf, comma1 );
    }
}


/**************************************************************************/
/* LOCAL  **************   PrintCopyFunctions      ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY COPY FUNCTIONS TO output.                         */
/**************************************************************************/

static void PrintCopyFunctions()
{
  register char  *t;
  register PINFO  i;
  register int    c;
           int    printed[BASE_CODE_LAST+1];

  /* CLEAR touch1 FIELDS and NO REPEAT FIELDS */
  ClearTouchFlags();

  for ( c = 0; c <= BASE_CODE_LAST; c++ )
    printed[c] = FALSE;

  for ( i = ihead; i != NULL; i = i->next ) {
    if ( i->touch1 || !IsArray( i ) )
      continue;

    /* NO ARRAY OF ARRAY AND ARRAY OF SCALAR REPEATS! */
    if ( printed[i->A_ELEM->type] )
      continue;

    FPRINTF( output, "\nstatic void %s( dest, source, num )\n",
             GetCopyFunction( i )                            );

    FPRINTF( output, "POINTER  dest;\n" );
    FPRINTF( output, "POINTER  source;\n" );
    FPRINTF( output, "register int num;\n" );
    FPRINTF( output, "{\n" );
  
    if ( IsBasic( i->A_ELEM ) ) {

      t = i->A_ELEM->tname;
  
      FPRINTF( output, "  register int  i;\n" );
      FPRINTF( output, "  register %s  *src = (%s*) source;\n", t, t );
      FPRINTF( output, "  register %s  *dst = (%s*) dest;\n", t, t );

      if ( IsChar( i->A_ELEM ) )
	FPRINTF( output, "  MyBCopy( src, dst, num );\n" );
      else {
	PrintVECTOR();
	PrintASSOC();
	PrintSAFE( "dst" );
        FPRINTF( output, "  for ( i = 0; i < num; i++ )\n" );
        FPRINTF( output, "    dst[i] = src[i];\n" );
	}

      FPRINTF( output, "}\n" );

    } else {

      t = i->A_ELEM->sname;
  
      FPRINTF( output, "  register %s **src = (%s**) source;\n", t, t );
      FPRINTF( output, "  register %s **dst = (%s**) dest;\n", t, t );
      FPRINTF( output, "  while ( num-- > 0 ) {\n" );
      FPRINTF( output, "    *dst = *src++;\n" );
      FPRINTF( output, "    MY_LOCK( &(*dst)->Mutex );\n" );
      FPRINTF( output, "    (*dst)->RefCount++;\n" );
      FPRINTF( output, "    FLUSHLINE(&((*dst)->RefCount));\n" );
      FPRINTF( output, "    CACHESYNC;\n");
      FPRINTF( output, "    MY_UNLOCK( &(*dst)->Mutex );\n" );
      FPRINTF( output, "    FLUSHLINE( &(*dst)->Mutex );\n" );
      FPRINTF( output, "    dst++;\n" );
      FPRINTF( output, "    }\n" );
      FPRINTF( output, "}\n" );

      }
  
    i->touch1 = TRUE;

    if ( (IsBasic(i->A_ELEM) && !IsBRecord(i->A_ELEM)) || IsArray(i->A_ELEM) )
      printed[i->A_ELEM->type] = TRUE;
    }

  ClearTouchFlags();
}


/**************************************************************************/
/* LOCAL  **************       PrintGlobals        ************************/
/**************************************************************************/
/* PURPOSE: PRINT GLOBAL CONSTANT RECORDS AND ARRAYS TO output.           */
/**************************************************************************/

static void PrintGlobals()
{
  register PNODE n;
  register PEDGE i;
  register PEDGE b;
  register int   gid = 0;
  register int   s;

  gdata = (chead == NULL)? FALSE : TRUE;

  for ( n = chead; n != NULL; n = n->usucc )
    switch ( n->type ) {
      case IFBRBuild:
        FPRINTF( output, "\nstatic shared %s %s = {\n",
                 n->exp->info->tname, n->G_NAME      );

	PrintConstants( n->imp, NULL_EDGE );
        FPRINTF( output, "  };\n" );
        break;

      case IFRBuild:
        FPRINTF( output, "\nstatic shared %s %sData = {\n",
                 n->exp->info->sname, n->G_NAME          );

	PrintConstants( n->imp, NULL_EDGE );
        FPRINTF( output, "  , %d\n", n->exp->sr + 9 );

        FPRINTF( output, "  };\n" );

	FPRINTF( output, "\nstatic shared %s %s = (%s) &%sData;\n",
	         n->exp->info->tname, n->G_NAME, n->exp->info->tname,
		 n->G_NAME                                         );
	break;

      case IFABuildAT:
        for ( s = 0, i = n->imp->isucc; i != NULL; i = i->isucc ) {
          if ( i->isucc != NULL )
            s++;
          else
	    b = i;
          }

	if ( s == 0 )
	  Error2( "PrintGlobals", "IFABuildAT WITHOUT ARGUMENTS" );

        FPRINTF( output, "\nstatic shared %s Storage%d[%d] = {\n", 
                 n->exp->info->A_ELEM->tname, ++gid, s          );

	PrintConstants( n->imp->isucc, b );
        FPRINTF( output, "  };\n" );

        FPRINTF( output, "\nstatic shared PHYS Phys%d = APhysStruct(\n", gid );
        FPRINTF( output, 
                 "  %d, (POINTER)Storage%d, 0, 1, 0, (POINTER)0, 1\n  );\n",
                 s, gid );

        FPRINTF( output, "\nstatic shared ARRAY %sData = AStruct(\n", 
                 n->G_NAME );
        FPRINTF( output, "  (POINTER) (Storage%d - (%s)), %s, %d",
                 gid, n->imp->CoNsT, n->imp->CoNsT, s           );
        FPRINTF( output, ", &Phys%d, 0, 0, %d\n  );\n", gid, n->exp->sr + 9 );

	FPRINTF( output, "\nstatic shared POINTER %s = (POINTER) &%sData;\n",
		 n->G_NAME, n->G_NAME                                      );
	break;

      default:
        break;
      }
}


/**************************************************************************/
/* LOCAL  **************      GenAssignNames       ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN STRUCTURE NAMES TO ALL RECORD, UNION, ARRAY, UNION,    */
/*          BUFFER, AND FUNCTION TYPES IN THE SYMBOL TABLE. ALSO ASSIGN   */
/*          WRITE, READ, COPY AND DEALLOCATION ROUTINE NAMES.             */
/**************************************************************************/

static void GenAssignNames()
{
  register PINFO i;

  for ( i = ihead; i != NULL; i = i->next ) {
    i->LibNames = FALSE;	/* Assume created names */
    nmid++;			/* Unique label for type */
    switch ( i->type ) {
    case IF_ARRAY:
      i->sname  = "ARRAY";
      i->LibNames = TRUE;
      switch ( i->A_ELEM->type ) {
        case IF_BOOL:
	  i->rname  = "ReadBoolVector";
	  i->wname  = "WriteBoolVector";
	  i->fname1 = "SFreeBoolVector";
	  i->fname2 = "PFreeBoolVector";
	  break;
	case IF_CHAR:
	  i->rname  = "ReadCharVector";
	  i->wname  = "WriteCharVector";
	  i->fname1 = "SFreeCharVector";
	  i->fname2 = "PFreeCharVector";
	  break;
	case IF_DOUBLE:
	  i->rname  = "ReadDoubleVector";
	  i->wname  = "WriteDoubleVector";
	  i->fname1 = "SFreeDoubleVector";
	  i->fname2 = "PFreeDoubleVector";
	  break;
	case IF_INTEGER:
	  i->rname  = "ReadIntegerVector";
	  i->wname  = "WriteIntegerVector";
	  i->fname1 = "SFreeIntegerVector";
	  i->fname2 = "PFreeIntegerVector";
	  break;
	case IF_NULL:
	  i->rname  = "ReadNullVector";
	  i->wname  = "WriteNullVector";
	  i->fname1 = "SFreeNullVector";
	  i->fname2 = "PFreeNullVector";
	  break;
	case IF_REAL:
	  i->rname  = "ReadRealVector";
	  i->wname  = "WriteRealVector";
	  i->fname1 = "SFreeRealVector";
	  i->fname2 = "PFreeRealVector";
	  break;
        default:
          i->LibNames = FALSE;	/* Assume created names */
	  i->rname  = MakeName( "ReadArr",   "",   nmid );
	  i->wname  = MakeName( "WriteArr",  "",   nmid );
	  i->fname1 = MakeName( "SFreeArr",  "",     nmid );
	  i->fname2 = MakeName( "PFreeArr",  "",     nmid );
      }
      break;

    case IF_BUFFER:
      i->sname = "BUFFER";
      break;

    case IF_UNION:
      i->sname  = MakeName( "struct Un",  "",   nmid );
      i->rname  = MakeName( "ReadUn",     "",   nmid );
      i->wname  = MakeName( "WriteUn",    "",   nmid );
      i->fname1 = MakeName( "SFreeUn",    "",   nmid );
      i->fname2 = MakeName( "PFreeUn",    "",   nmid );
      i->cname  = MakeName( "Un",         "",   nmid );
      break;

    case IF_RECORD:
      i->sname  = MakeName( "struct Rec", "",   nmid );
      i->fname1 = MakeName( "SFreeRec",   "",   nmid );
      i->fname2 = MakeName( "PFreeRec",   "",   nmid );
      i->cname  = MakeName( "Rec",        "",   nmid );
      i->rname  = MakeName( "ReadRec",    "",   nmid );
      i->wname  = MakeName( "WriteRec",   "",   nmid );
      break;

    case IF_BRECORD:
      i->sname = MakeName( "struct BRec", "",   nmid );
      i->tname = MakeName( "struct BRec", "",   nmid );
      i->cname = MakeName( "BRec",        "",   nmid );
      i->rname  = MakeName( "ReadRec",    "",   nmid );
      i->wname  = MakeName( "WriteRec",   "",   nmid );
      break;

    case IF_FUNCTION:
      i->sname = MakeName( "struct Args", "",   nmid );
      break;

    default:
      break;
    }
  }
}


/**************************************************************************/
/* LOCAL  **************       PrintStructs        ************************/
/**************************************************************************/
/* PURPOSE: PRINT STRUCTURES FOR ALL RECORDS, UNION, AND FUNCTIONS TO     */
/*          output.                                                       */
/**************************************************************************/

static void PrintStructs()
{
  register PINFO i;
  register PINFO ii;
  register int   c;
  register int   cc;
           char  buf[100];

  for ( i = ihead; i != NULL; i = i->next ) {
    switch ( i->type ) {
      case IF_UNION:
        FPRINTF( output, "\n%s {\n  ", i->sname );
        FPRINTF( output, "union {\n    " );

        for ( c = 0, ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT ) {
          SPRINTF( buf, "%-7s Fld%d; ", ii->L_SUB->tname, c++ );
          FPRINTF( output, "%-16s", buf ); 

          if ( (c % 4) == 0 )
            FPRINTF( output, "\n    " );
          }

        if ( (c % 4) != 0 )
          FPRINTF( output, "\n    " );

        FPRINTF( output, "  } Data;\n" );
        FPRINTF( output, "%-16s  %-16s  %-16s\n  ", 
                 "int  RefCount; ", "int  Tag; ", "LOCK_TYPE Mutex; " );
        FPRINTF( output, "  };\n" );
        break;

      case IF_RECORD:
        FPRINTF( output, "\n%s {\n  ", i->sname );
	
        for ( c = 0, ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT ) {
          SPRINTF( buf, "%-7s Fld%d; ", ii->L_SUB->tname, ++c );
          FPRINTF( output, "%-16s", buf ); 

          if ( (c % 4) == 0 )
            FPRINTF( output, "\n  " );
          }

        if ( (c % 4) != 0 )
          FPRINTF( output, "\n  " );

        FPRINTF( output, "%-16s %-16s\n  ",
		 "int  RefCount;", "LOCK_TYPE Mutex; " );
        FPRINTF( output, "};\n" );
        break;

      case IF_BRECORD:
        FPRINTF( output, "\n%s {\n  ", i->sname );
	
        for ( c = 0, ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT ) {
          SPRINTF( buf, "%-7s Fld%d; ", ii->L_SUB->tname, ++c );
          FPRINTF( output, "%-16s", buf ); 

          if ( (c % 4) == 0 )
            FPRINTF( output, "\n  " );
          }

        if ( (c % 4) != 0 )
          FPRINTF( output, "\n  " );

        FPRINTF( output, "};\n" );
        break;

      case IF_FUNCTION:
        FPRINTF( output,"\n%s {   \n", i->sname );
	FPRINTF( output, "struct ActRec *FirstAR; int Count;   \n" );

        for ( c = 0, ii = i->F_IN; ii != NULL; ii = ii->L_NEXT ) {
          if ( ii->L_SUB->type == IF_BUFFER ) 
            SPRINTF( buf, "%-7s In%d; ", "POINTER", ++c );
          else
            SPRINTF( buf, "%-7s In%d; ", ii->L_SUB->tname, ++c );

          FPRINTF( output, "%-16s", buf );

          if ( (c % 4) == 0 )
            FPRINTF( output, "\n  " );
          }

        cc = c;

#if defined(NON_COHERENT_CACHE)
          FPRINTF(output,"char padIn[CACHE_LINE]; ",c);
          cc++;
#endif

        for ( c = 0, ii = i->F_OUT; ii != NULL; ii = ii->L_NEXT ) {
          if ( ii->L_SUB->type == IF_BUFFER )
            Error2( "PrintStructs", "FUNCTION RETURNING BUFFER" );

          SPRINTF( buf, "%-7s Out%d; ", ii->L_SUB->tname, ++c );
          FPRINTF( output, "%-16s", buf ); 
#if defined(NON_COHERENT_CACHE)
          FPRINTF(output,"char pad%d[CACHE_LINE]; ",c);
          cc++;
#endif

	  cc++;

          if ( (cc % 4) == 0 )
            FPRINTF( output, "\n  " );
          }

#if defined(NON_COHERENT_CACHE)
        FPRINTF(output,"char pad[CACHE_LINE];");
        cc++;
#endif

        if ( (cc % 4) != 0 )
          FPRINTF( output, "\n  " );

        FPRINTF( output, "};\n" );
        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    PrintExternFunction    ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE EXTERN DECLARATION FOR THE FUNCTION NAMED f WHICH   */
/*          RETURNS TYPE t.  THE CALLED FUNCTION GRAPH IS ff, UNLESS IT   */
/*          IS NULL.                                                      */
/**************************************************************************/

static void PrintExternFunction( t, f, ff )
char  *t;
char  *f;
PNODE  ff;
{
  /* ------------------------------------------------------------ */
  /* If this name happens to be a predefined macro, bad things	  */
  /* can happen.  We're best off undefining it			  */
  /* ------------------------------------------------------------ */
  FPRINTF( output, "#undef %s\n",f);
  
  /* ------------------------------------------------------------ */
  /* Mark internal forwards as static.  Otherwise, mark extern	  */
  /* ------------------------------------------------------------ */
  if ( ff && (ff->mark == 's' || 
	      ff->mark == 'f' ||
	      ff->mark == 'c' ||
	      ff->mark == 'i') ) {
    FPRINTF( output, "extern %-12s %s();", t, f );
  } else {
    FPRINTF( output, "static %-12s %s();", t, f );
  }

  if ( ff != NULL ) {
    FPRINTF( output, "\t/* [Call=%c,Rec=%c,Par=%c,Xmk=%c,Mk=%c] */",
	    (ff->Cmark)? 'T' : 'F', 
	    (ff->bmark)? 'T' : 'F', 
	    (ff->Pmark)? 'T' : 'F',
	    (ff->xmark)? 'T' : 'F',
	    (ff->mark) ? ff->mark : ' ');
  }

  FPRINTF( output, "\n" );
}


/**************************************************************************/
/* LOCAL  **************        PrintForwards      ************************/
/**************************************************************************/
/* PURPOSE: PRINT FORWARD DECLARATIONS FOR SISAL, LOOP TASK, FREE, AND    */
/*          FIBRE READ AND WRITE OPERATIONS TO output.                    */
/**************************************************************************/

static void PrintForwards()
{
  register PNODE f;
  register PINFO i;
	   char  buf[100];

  /* PRINT FORWARD DECLARATIONS */
  FPRINTF( output, "static void InitGlobalData();\n\n" );

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) ) {
      if ( GenIsIntrinsic( f ) )
	continue;

      if ( f->mark != 's' ) /* NEW CANN 2/92 */
        PrintExternFunction( f->info->F_OUT->L_SUB->tname, f->G_NAME, f );
      else
        PrintExternFunction( "void", f->G_NAME, f );
      }
    else
      PrintExternFunction( "void", f->G_NAME, f );
    }

  /* PRINT DEALLOCATION AND FIBRE READ AND WRITE FORWARD DECLARATIONS */
  /* AND INTERFACE ROUTINES */
  for ( i = ihead; i != NULL; i = i->next ) {
    switch ( i->type ) {
    case IF_UNION:
    case IF_RECORD:
    case IF_ARRAY:
      if ( !i->LibNames ) {
	PrintExternFunction( i->tname, i->rname,   NULL_NODE );
	PrintExternFunction( "void",   i->wname,   NULL_NODE );
      }

      /* INTERFACE ROUTINES */
      SPRINTF( buf, "I%s", i->rname );
      PrintExternFunction( i->tname, buf, NULL_NODE );
      SPRINTF( buf, "I%s", i->wname );
      PrintExternFunction( "void", buf, NULL_NODE );

      PrintExternFunction( "void",   i->fname1,  NULL_NODE );
      PrintExternFunction( "void",   i->fname2,  NULL_NODE );
      break;

    case IF_BRECORD:
      if ( !i->LibNames ) {
	PrintExternFunction( i->tname, i->rname,   NULL_NODE );
	PrintExternFunction( "void",   i->wname,   NULL_NODE );
      }
      break;

    default:
      break;
    }
  }
}


/**************************************************************************/
/* GLOBAL **************     PrintFilePrologue     ************************/
/**************************************************************************/
/* PURPOSE: PRINT INCLUDES, FORWARDS, TYPE TABLE, ENTRY POINT, GLOBALS,   */
/*          AND ARUGMENT AND RECORD STRUCTURES TO output.                 */
/**************************************************************************/

void PrintFilePrologue()
{
  register PNODE f;

  FPRINTF( output, "#ifdef CInfo\n" );
  FPRINTF( output, "#define GatherCopyInfo 1\n" );
  FPRINTF( output, "#endif\n\n" );

  FPRINTF( output, "#ifdef FInfo\n" );
  FPRINTF( output, "#define GatherFlopInfo 1\n" );
  FPRINTF( output, "#endif\n\n" );

  if ( !regok ) FPRINTF( output, "#define register\n\n" );

  FPRINTF( output, "#define _INTRINSICS_ 1\n\n" );

  FPRINTF( output, "#include \"sisal.h\"\n\n" );


  /* ------------------------------------------------------------ */
  /* START: DUMP MODULE COORDINATION VARIABLES			  */
  /* ------------------------------------------------------------ */
  if ( IsStamp( PDBASE ) ) {
    FPRINTF( output,"int AllCompilesMustUseTheModuleDataBase;\n\n" );
  } else if ( IsStamp( MDBASE ) ) {
    FPRINTF( output,"extern int AllCompilesMustUseTheModuleDataBase;\n" );
    FPRINTF( output,
	    "static int *ModuleValue = &AllCompilesMustUseTheModuleDataBase;\n\n");
  } else if ( IsStamp( PNODBASE ) ) {
    FPRINTF( output,"int ProvideModuleDataBaseOnAllCompiles;\n\n" );
  } else if ( IsStamp( MNODBASE ) ) {
    FPRINTF( output,"extern int ProvideModuleDataBaseOnAllCompiles;\n" );
    FPRINTF( output,
	    "static int *ModuleValue = &ProvideModuleDataBaseOnAllCompiles;\n\n");
  }

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    int Pmark = f->Pmark;
    Pmark = TRUE;		/* Not doing what I expect, PJM! */
    if ( f->mark == 's' && IsIGraph( f ) && Pmark ) {
      FPRINTF( output, 
	      "extern int RecompileTheModuleDefining%s;\n",
	      f->G_NAME );
      FPRINTF( output, 
	      "static int *%sValue = &RecompileTheModuleDefining%s;\n\n",
	      f->G_NAME, f->G_NAME );
    } else if (Pmark && IsXGraph( f ) && f->mark == 's' ) {
      FPRINTF( output, "int RecompileTheModuleDefining%s;\n\n", f->G_NAME );
    }
  }
  /* ------------------------------------------------------------ */
  /* END: DUMP MODULE COORDINATION VARIABLES			  */
  /* ------------------------------------------------------------ */


  if ( sdbx ) PrintSdbxFunctionList();
    
  GenAssignNames();

  PrintForwards();
  PrintStructs();
  PrintCopyFunctions();
  PrintGlobals();

  /* FOR DEALLOCATION ROUTINE INFORMATION */
  ClearTouchFlags();
}


/**************************************************************************/
/* LOCAL  **************    DriveRecursiveMarks    ************************/
/**************************************************************************/
/* PURPOSE: MARK ALL THE FUNCTIONS CALLED WITHIN GRAPH g AS RECURSIVE.    */
/**************************************************************************/

static int DriveRecursiveMarks( g, bmark )
PNODE g;
int   bmark;
{
  register PNODE n;
  register PNODE sg;
  register PNODE f;
  register int   change;

  for ( change = FALSE, n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	change = change || DriveRecursiveMarks( sg, bmark );

    if ( !IsCall( n ) )
      continue;

    f = FindFunction( n->imp->CoNsT );

    /* MARK AS CALLED! */
    if ( !(f->Cmark) ) {
      f->Cmark = TRUE;
      change = TRUE;
      }

    if ( bmark )
      if ( !(f->bmark) ) {
        f->bmark = TRUE;
        change = TRUE;
        }
    }

  return( change );
}


/**************************************************************************/
/* GLOBAL **************   MarkRecursiveFunctions  ************************/
/**************************************************************************/
/* PURPOSE: MARK ALL THE RECURSIVE FUNCTIONS IN THE PROGRAM.              */
/**************************************************************************/

void MarkRecursiveFunctions()
{
  register PNODE f;
  register int   change = TRUE;
  register int   rsmodule = FALSE;

  /* COULD THIS MODULE BE PART OF A RECURSIVE CYCLE THAT CROSSES MODULES? */
  /* LOOKUP WOULD HELP! */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    if ( IsIGraph( f ) )
      if ( f->mark == 's' )
	 rsmodule = TRUE;

  while ( change ) {
    change = FALSE;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
      switch( f->type ) {
	case IFIGraph:
	  break;

	case IFXGraph:
	  /* NEW CANN 2/92 ASSUME A SISAL MODULE THAT CALLS ANOTHER SISAL */
	  /* MODULE IS RECURSIVE; LOOKUP WOULD HELP! */
	  /* NOTE: PROGRAM MODULE ENTRY POINTS (e,c,f) CANNOT BE RECURSIVE!!! */
	  if ( f->mark == 's' && rsmodule )
	     f->bmark = TRUE;

	case IFLGraph:
	  change = DriveRecursiveMarks( f, (int)f->bmark );
	  break;

	default:
	  Error2( "MarkRecursiveFunctions", "ILLEGAL GRAPH NODE" );
	}
    }
}


/**************************************************************************/
/* LOCAL  **************    CheckParallelMarks     ************************/
/**************************************************************************/
/* PURPOSE: FUNCTION g IS CALLED IN PARALLEL, SO MARK IT'S CALLEES        */
/*          SIMILARLY.                                                    */
/**************************************************************************/

static void CheckParallelMarks( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE f;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	CheckParallelMarks( sg );

    if ( !IsCall( n ) )
      continue;

    f = FindFunction( n->imp->CoNsT );

    if ( !(f->Pmark) )
      Error2( "CheckParallelMarks", "Pmark IS NOT SET ON FUNCTION CALL" );
    }
}


/**************************************************************************/
/* GLOBAL **************   CheckParallelFunctions  ************************/
/**************************************************************************/
/* PURPOSE: VERIFY THE EXECUTION MODES FOR ALL THE FUNCTIONS.             */
/**************************************************************************/

void CheckParallelFunctions()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    switch( f->type ) {
      case IFLPGraph:
	if ( !(f->Pmark) )
	  Error2( "CheckParallelFunctions", "Pmark NOT SET ON LPGraph" );

	CheckParallelMarks( f );
	break;

      case IFIGraph:
	break;

      case IFLGraph:
      case IFXGraph:
	if ( f->Pmark )
	  CheckParallelMarks( f );

	break;

      default:
	Error2( "CheckParallelFunctions", "ILLEGAL GRAPH NODE" );
      }
}


/**************************************************************************/
/* GLOBAL **************     PrintFunctPrologue    ************************/
/**************************************************************************/
/* PURPOSE: PRINT FUNCTION f'S PROLOGUE TO output.                        */
/**************************************************************************/

void PrintFunctPrologue( f )
PNODE f;
{
  register PINFO ii;
  register int   eport;

  switch ( f->type ) {
   case IFIGraph:
    return;

   case IFLPGraph:
    /* ------------------------------------------------------------ */
    /* Depending on the parallelism style, the frame looks a little */
    /* different.  The runtime decision and strided styles require  */
    /* a step.  The block style does not.			    */
    /* ------------------------------------------------------------ */
    switch ( f->Style ) {
      /* ------------------------------------------------------------ */
     case 'G':/* Guided Self Scheduling */
     case 'B':/* Block */
     case 'C':/* Cached */
      if ( f->Style == 'C' ) FPRINTF( output, "/* CACHED STYLE */\n");
      if ( f->Style == 'G' ) FPRINTF( output, "/* GSS STYLE */\n");
      if ( f->Style == 'B' ) FPRINTF( output, "/* BLOCK STYLE */\n");
      FPRINTF( output, "\nstatic void %s( args, lo, hi )\n", f->G_NAME );
      FPRINTF( output, "%s args;\n", f->info->tname );
      FPRINTF( output, "int lo, hi;\n{\n" );
      break;

      /* ------------------------------------------------------------ */
     case 'S':/* Strided */
      FPRINTF( output, "/* STRIDED STYLE */\n");
      FPRINTF( output, "\nstatic void %s( args, lo, hi, step )\n", f->G_NAME );
      FPRINTF( output, "%s args;\n", f->info->tname );
      FPRINTF( output, "int lo, hi,step;\n{\n" );
      break;

      /* ------------------------------------------------------------ */
     case 'R':/* Runtime decision */
     case '\000':
      FPRINTF( output, "/* RUNTIME DECISION FOR STYLE */\n");
      FPRINTF( output, "\nstatic void %s( args, lo, hi, step )\n", f->G_NAME );
      FPRINTF( output, "%s args;\n", f->info->tname );
      FPRINTF( output, "int lo, hi, step;\n{\n" );
      break;
    }
    break;

   default:
    /* ------------------------------------------------------------ */
    /* Check to make sure all arguments are used		    */
    /* ------------------------------------------------------------ */
    eport = 1;
    for ( ii = f->info->F_IN; ii != NULL; ii = ii->L_NEXT, eport++ ) {
      if ( IsExport( f, eport ) ) continue;

      if ( Warnings ) {
	FPRINTF( stderr,
		"%s: W - ARGUMENT %d OF %s ON LINE %d IN %s IS NEVER USED\n",
		program, eport, 
		(f->funct == NULL)? "FUNCT?()" : f->funct, f->line, 
		(f->file == NULL)? "FILE?.sis" : f->file 
		);
      }
    }

    /* ------------------------------------------------------------ */
    /* A standard function has only its frame as an argument	    */
    /* ------------------------------------------------------------ */
    FPRINTF( output, "\n%svoid %s( args )\n",
	    /* (f->emark)? "" : "static ", f->G_NAME ); */
	    (f->mark == 's')? "" : "static ", f->G_NAME); /* NEW CANN 2/92 */
    FPRINTF( output, "%s args;\n{\n", f->info->tname );
    break;
  }

  PrintLocals();

  if ( f->flp ) {
    FPRINTF( output, "register double FCa;\n" );
    FPRINTF( output, "register double FCl;\n" );
    FPRINTF( output, "register double FCi;\n" );
  }

  if ( sdbx )
    SaveSdbxState( f );

  if ( sequential ) {
    FPRINTF( output, "#undef  MY_LOCK\n"   );
    FPRINTF( output, "#undef  MY_UNLOCK\n" );
    FPRINTF( output, "#define MY_LOCK(x)\n"   );
    FPRINTF( output, "#define MY_UNLOCK(x)\n\n" );
  }

  if ( f->flp ) {
    FPRINTF( output, "FCa = MyInfo->FlopCountA;\n" );
    FPRINTF( output, "FCl = MyInfo->FlopCountL;\n" );
    FPRINTF( output, "FCi = MyInfo->FlopCountI;\n" );
  }

  if ( f->time ) {
    FPRINTF( output, "  StartFunctionTimer(\"%s\");\n\n", 
	    (f->funct == NULL)? f->G_NAME : f->funct );
  }

  if ( f->trace ) {
    FPRINTF( output, "  Trace(Write_%s_FibreInputs,\"%s\");\n\n", 
	    (f->funct == NULL)? f->G_NAME : f->funct,
	    (f->funct == NULL)? f->G_NAME : f->funct
	    );
  }

}


/**************************************************************************/
/* GLOBAL **************     PrintFunctEpilogue    ************************/
/**************************************************************************/
/* PURPOSE: PRINT FUNCTION f'S EPILOGUE TO output. GRAPH TYPE CONTROLS    */
/*          THE TERMINATION MECHANISM. WHEN DONE, THE LOCAL NAME STORAGE  */
/*          IS RELEASED.                                                  */
/**************************************************************************/

void PrintFunctEpilogue( f )
PNODE f;
{
  PrintFrameDeallocs();

  if ( f->time )
    FPRINTF( output, "\n  StopFunctionTimer(\"%s\");\n", 
	     (f->funct == NULL)? f->G_NAME : f->funct );

  if ( sdbx )
    FPRINTF( output, "SdbxMonitor( SDBX_POP );\n" );

  if ( f->mark == 's' && sdbx ) {
    FPRINTF( output, "if ( ClearSdbx ) {\n" );
    FPRINTF( output, " SdbxMonitor( SDBX_ESTOP );\n" );
    FPRINTF( output, " }\n" );
    }

  if ( f->flp ) {
    FPRINTF( output, "fprintf( stderr, \"%%18.0fA %%18.0fL %%18.0fI [%s]\\n\",(MyInfo->FlopCountA)-FCa,(MyInfo->FlopCountL)-FCl, (MyInfo->FlopCountI)-FCi );\n", 
      (f->funct == NULL)? f->G_NAME : f->funct );
    }

  if ( sequential ) {
    FPRINTF( output, "\n#undef  MY_LOCK\n" );
    FPRINTF( output, "#undef  MY_UNLOCK\n" );
    FPRINTF( output, "#define MY_LOCK(x)    MY_LOCK_BACKUP(x)\n"   );
    FPRINTF( output, "#define MY_UNLOCK(x)  MY_UNLOCK_BACKUP(x)\n" );
    }

  if(LCMSize(f) != 0)
        FPRINTF(output, "FLUSHALL;\n");
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************   PrintInitGlobalData     ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE GLOBAL CONSTANT INITIALIZATION FUNCTION.            */
/**************************************************************************/

static void PrintInitGlobalData()
{
  register PNODE n;

  FPRINTF( output, "\nstatic int *GInit = NULL;\n" ); 

  FPRINTF( output, "\nstatic void InitGlobalData()\n" );
  FPRINTF( output, "{\n" );
  FPRINTF( output, "  SLockParent;\n\n" );
  FPRINTF( output, "  if ( GInit == NULL )\n" );
  FPRINTF( output, "    GInit = (int *) Alloc( sizeof(int) );\n" );
  FPRINTF( output, "  else if ( *GInit ) {\n" );
  FPRINTF( output, "    SUnlockParent;\n" );
  FPRINTF( output, "    return;\n" );
  FPRINTF( output, "    }\n\n" );

  FPRINTF( output, "  *GInit = TRUE;\n\n" );

  for ( n = chead; n != NULL; n = n->usucc )
    switch ( n->type ) {
      case IFRBuild:
        FPRINTF( output, "  MY_INIT_LOCK( &(%sData.Mutex) );\n", n->G_NAME );
        break;

     case IFABuildAT:
       FPRINTF( output, "  MY_INIT_LOCK( &(%sData.Phys->Mutex) );\n",n->G_NAME);
       FPRINTF( output, "  MY_INIT_LOCK( &(%sData.Mutex) );\n", n->G_NAME );
       break;

     default:
       break;
     }

  FPRINTF( output, "\n  SUnlockParent;\n" );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  ************** PrintStandAloneEntryPoint ************************/
/**************************************************************************/
/* PURPOSE: PRINT STAND ALONE SISAL ENTRY POINT FOR FUNCTION f TO output. */
/**************************************************************************/

static void PrintStandAloneEntryPoint( f )
PNODE f;
{
  FPRINTF( output, "\nvoid SisalMain( args )\n" );
  FPRINTF( output, "POINTER args;\n" );
  FPRINTF( output, "{\n" );

  FPRINTF( output, "#ifdef CInfo\n" );
  FPRINTF( output, "  SaveCopyInfo;\n" );
  FPRINTF( output, "#endif\n" );

  FPRINTF( output, "#ifdef FInfo\n" );
  FPRINTF( output, "  SaveFlopInfo;\n" );
  FPRINTF( output, "#endif\n" );

  if ( gdata )
    FPRINTF( output, "  InitGlobalData();\n" );

  if ( sdbx ) {
    FPRINTF( output, "SdbxCurrentFunctionList = MyFunctionList;\n" );
    FPRINTF( output, "SdbxMonitor( SDBX_ESTART );\n" );
    }

  FPRINTF( output, "  %s( args );\n", f->G_NAME );

  if ( sdbx ) {
    FPRINTF( output, "SdbxMonitor( SDBX_ESTOP );\n" );
    }

  if ( freeall ) {
    PrintInputDeallocs( (char*)NULL, 2, f );

    FPRINTF( output, "  if ( NoFibreOutput ) {\n" );

    PrintOutputDeallocs( 4, f );
    FPRINTF( output, "    DeAlloc( args );\n" );

    FPRINTF( output, "    }\n" );
    }

  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* GLOBAL **************     PrintFileEpilogue     ************************/
/**************************************************************************/
/* PURPOSE: PRINT ALL REMAINING STUFF TO output TO COMPLETE THE C CODE.   */
/**************************************************************************/

void PrintFileEpilogue()
{
  register PNODE f;

  /* FORCE SEQUENTIAL MODE OF EXECUTION */
  sequential = TRUE;

  PrintInitGlobalData();

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {

    /* NEW CANN 2/92 */
    if ( IsIGraph( f ) )
      continue;
    /* if ( !f->emark ) */
    if ( f->mark == '\0' || f->mark == 's' || f->mark == 'd' )
      continue;

    if ( standalone ) {
      PrintStandAloneEntryPoint( f );
      break; 
      }

    PrintInterface( f );
    }

  PrintInterfaceUtilities();
  PrintFreeUtilities();

  if ( standalone ) {
    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
      /* if ( !f->emark ) */
      if ( f->mark != 'e' )	/* NEW CANN 2/92 */
        continue;

      PrintReadFibreInputs( f );

      PrintTypeWriters(f);
      PrintWriteFibreOutputs( f );
      break;
    }
  } else {
    PrintTypeWriters(NULL_NODE);
  }

  /* Handle the trace output routines */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( f->trace ) {
      PrintWriteFibreInputs(f);
    }
  }

  /* CLEAR MODE OF EXECUTION */
  sequential = FALSE;
}
