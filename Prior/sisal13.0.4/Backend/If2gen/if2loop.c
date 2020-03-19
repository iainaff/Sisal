/* if2loop.c,v
 * Revision 12.7  1992/11/04  22:05:01  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:01  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static void PrintVecNode();

/* scc UNDER Unicos APPEARS TO HAVE TROUBLE CORRECTLY VECTORIZING LOOPS */
/* NOT ALREADY SUBJECTED TO cse AND gcse OPTIMIZATIONS                  */
static int WasCseANDGcseDone()
{
  register char *s;
  register int   gcse;
  register int   cse;
  
  if ( (s = stamps[ IF1OPTIMIZED ]) == NULL )
    return( FALSE );

  gcse = cse = FALSE;

  while ( *s != '\0' ) {
    if ( *(s) == ' ' )
      if ( *(s+1) == 'g' )
        if ( *(s+2) == 'c' )
          if ( *(s+3) == 's' )
            if ( *(s+4) == 'e' )
              gcse = TRUE;

    if ( *(s) == ' ' )
      if ( *(s+1) == 'c' )
        if ( *(s+2) == 's' )
          if ( *(s+3) == 'e' )
            cse = TRUE;

    s++;
    }

  return( gcse && cse );
}


static int AreImportReferenceCountOps( n )
PNODE n;
{
  register PEDGE i;

  for ( i = n->imp; i != NULL; i = i->isucc )
    if ( i->cm != 0 || i->pm > 0 || i->sr > 0 || i->pl > 0 )
      return( TRUE );

  return( FALSE );
}


static int AreComplexImports( n )
PNODE n;
{
  register PEDGE i;

  i = (IsCall( n ))? n->imp->isucc : n->imp;
  for ( /* NOTHING */; i != NULL; i = i->isucc )
    if ( !IsBasic( i->info ) )
      /* BUFFERS ALLOWED IN SPECIAL ReduceAT CONSTRUCTIONS */
      if ( !IsBuffer( i->info ) )
        return( TRUE );

  return( FALSE );
}


static int IsIvdepOk( g,ReasonP )
     PNODE g;
     char	**ReasonP;
{
  register PNODE n;
  register PEDGE e;

  if ( !WasCseANDGcseDone() ) {
    *ReasonP = "Didn't do CSE and GCSE";
    return( FALSE );
  }

  if ( AreImportReferenceCountOps( g ) ) {
    *ReasonP = "There is imported reference counts to the graph";
    return( FALSE );
  }

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( AreImportReferenceCountOps( n ) ) {
      *ReasonP = "There is imported reference counts to an interior node";
      return( FALSE );
    }

    if ( IsSelect( n ) ) {
      if ( IsConst( n->S_TEST->imp ) ) {
	*ReasonP = "If-then-else with constant test";
	return( FALSE );
      }

      if ( !IsIvdepOk( n->S_CONS,ReasonP ) ) return( FALSE );

      if ( !IsIvdepOk( n->S_ALT,ReasonP ) ) return( FALSE );

      continue;
    }

    if ( IsCompound( n ) ) {
      *ReasonP = "Loop contains interior compound (other than if)";
      return( FALSE );
    }

    if ( n->type == IFOptAElement ) {
      /* basic record dereferences within selects seem to confuse scc */
      if ( IsBRecord( n->exp->info ) ) {
	if ( !IsForall( g->G_DAD ) ) {
	  *ReasonP = "Selecting from array of records";
	  return( FALSE );
	}
      }

      continue;
    }

    /* select imported indirection seems to confuse scc */
    if ( n->type == IFGetArrayBase ) {
      for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( IsCompound( e->dst ) ) {
#if 0
	  {
	    PNODE Z;
	    PEDGE Q;
	    fprintf(stderr,"Feeding %s at line %d\n",GetNodeName(e->dst),e->dst->line);
	    fprintf(stderr,"outedge\n");
	    if ( e->name ) fprintf(stderr,">> %s\n",e->name);
	    fprintf(stderr,"inedge\n");
	    Q = n->imp;
	    if ( Q && Q->name ) fprintf(stderr,">> %s\n",Q->name);
	    if ( Q ) Q = Q->isucc;
	    if ( Q && Q->name ) fprintf(stderr,">> %s\n",Q->name);
	    if ( Q ) Q = Q->isucc;
	    if ( Q && Q->name ) fprintf(stderr,">> %s\n",Q->name);
	    if ( Q ) Q = Q->isucc;
	    if ( Q && Q->name ) fprintf(stderr,">> %s\n",Q->name);

	    for(Z=n; Z; Z=Z->npred) {
	      fprintf(stderr,"%s at line %d\n",GetNodeName(Z),Z->line);
	      if ( IsCompound(Z) ) break;
	    }
	    fprintf(stderr,"---\n");
	    for(Z=n; Z; Z=Z->nsucc) {
	      fprintf(stderr,"%s at line %d\n",GetNodeName(Z),Z->line);
	    }
	  }
#endif
	  *ReasonP = "Cray C compiler gets confused over this indirection";
	  return( FALSE );
	}
      }

      continue;
    }


    if ( AreComplexImports( n ) ) {
      *ReasonP = "Loop uses non-scalar interior types";
      return( FALSE );
    }
  }

  return( TRUE );
}


static char *PrintVectorTemp( fd, i )
FILE  *fd;
PEDGE  i;
{
  register int  type;
	   char buf[100];

  if ( i->vtemp <= 0 )
    Error2( "PrintVectorTemp", "VECTOR TEMP NOT ASSIGNED" );

  if ( IsBuffer( i->info ) )
    type = i->info->A_ELEM->A_ELEM->type;
  else if ( IsArray( i->info ) )
    type = i->info->A_ELEM->type;
  else
    Error2( "PrintVectorTemp", "ILLEGAL VECTOR TYPE" );

  switch ( type ) {
    case IF_DOUBLE:
      SPRINTF( buf, "VD%d", i->vtemp );
      break;

    case IF_REAL:
      SPRINTF( buf, "VF%d", i->vtemp );
      break;

    case IF_INTEGER:
      SPRINTF( buf, "VI%d", i->vtemp );
      break;

    default:
      Error2( "PrintVectorTemp", "ILLEGAL BUFFER COMPONENT TYPE" );
      break;
    }

  if ( fd != NULL ) {
    FPRINTF( fd, "%s", buf );
    return( NULL );
    }
  return( CopyString( buf ) );
}


/**************************************************************************/
/* GLOBAL **************        PrintAStore        ************************/
/**************************************************************************/
/* PURPOSE: PRINT AStore NODE n OF AGatherAT NODE gat TO output.          */
/**************************************************************************/

void PrintAStore( indent, n, gat )
int   indent;
PNODE n;
PNODE gat;
{
  register PNODE l;

  l = gat->exp->dst->G_DAD;

  PrintIndentation( indent );

  if ( l->vmark || l->smark ) {
    FPRINTF( output, "VecGathATUpd( " );
    PrintTemp( l->F_GEN->imp );
    FPRINTF( output, ", %s, ", gat->exp->info->A_ELEM->tname );
    }
  else
    FPRINTF( output, "GathATUpd( %s, ", gat->exp->info->A_ELEM->tname ); 

  PrintTemp( gat->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* LOCAL  **************      PrintYankedRed       ************************/
/**************************************************************************/
/* PURPOSE: PRINT YANKED BASIC REDUCTION UPDATE NODE n TO OUTPUT.         */
/**************************************************************************/

void PrintYankedRed( indent, n, macro )
int    indent;
PNODE  n;
char  *macro;
{
    register PEDGE i = n->gsucc->exp;

    FPRINTF( output, "/* YankedRed */\n" );
    PrintIndentation( indent );
    FPRINTF( output, "Opt%s( ", macro );

    PrintTemp( i );
    FPRINTF( output, ", " );
    PrintTemp( i );
    FPRINTF( output, ", " );
    PrintTemp( n->imp );
    FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************      AreAllUnitFanout     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE FANOUT OF EACH EXPORT OF EACH NODE IN THE  */
/*          NODE LIST OF LOOP TEST SUBGRAPH g IS 1.                       */
/**************************************************************************/

int AreAllUnitFanout( g )
PNODE g;
{
    register PNODE n;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	if ( n->exp->esucc != NULL )
	    return( FALSE );

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************       PrintLoopTest       ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE LOOP TEST EXPRESSION TREE ROOTED BY EDGE i TO       */
/*          output.                                                       */
/**************************************************************************/

static void PrintLoopTest( i )
PEDGE i;
{
    if ( IsConst( i ) ) {
	PrintTemp( i );
	return;
	}

    switch ( i->src->type ) {
        case IFPlus:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " || " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFTimes:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " && " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFGreat:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " > " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFGreatEqual:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " >= " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFLess:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " < " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFLessEqual:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " <= " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFNotEqual:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " != " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFEqual:
	    FPRINTF( output, "( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " == " );
	    PrintLoopTest( i->src->imp->isucc );
	    FPRINTF( output, " )" );
	    break;

	case IFNot:
	    FPRINTF( output, "( !( " );
	    PrintLoopTest( i->src->imp );
	    FPRINTF( output, " ) )" );
	    break;

	case IFSGraph:
	    PrintTemp( i );
	    break;

	default:
	    Error2( "PrintLoopTest", "ILLEGAL LOOP TEST NODE" );
	}
}


/**************************************************************************/
/* GLOBAL **************       PrintSumOfTerms     ************************/
/**************************************************************************/
/* PURPOSE: PRINT SUM OF TERMS MACRO FOR FORALL NODE e->src AND THE       */
/*          ASSOCIATED MUTLIPLE NODE TO output.                           */
/**************************************************************************/

void PrintSumOfTerms( indent, e )
int   indent;
PEDGE e;
{
    register PEDGE i = FindImport( e->src->F_RET, e->eport );

    PrintIndentation( indent );

    switch ( i->src->imp->CoNsT[0] ) {
        case REDUCE_LEAST: 
            FPRINTF( output, "OptMin( " );
            break;

        case REDUCE_GREATEST:
            FPRINTF( output, "OptMax( " );
            break;

        case REDUCE_PRODUCT:
            if ( IsBoolean( i->info ) )
                FPRINTF( output, "OptAnd( " );
            else
                FPRINTF( output, "OptTimes( " );

            break;

        case REDUCE_SUM:
            if ( IsBoolean( i->info ) )
                FPRINTF( output, "OptOr( " );
            else
                FPRINTF( output, "OptPlus( " );

            break;

        case REDUCE_USER:
            PrintFldRef( e->dst->info->sname, "args", NULL_EDGE, "Out",
                e->iport );
            FPRINTF( output, " = " );
            PrintTemp( e );
            FPRINTF( output, "; /* REDUCE_USER Mult.\n" );
            FPRINTF( output, "FLUSHLINE(&(");
            PrintFldRef( e->dst->info->sname, "args", NULL_EDGE, "Out", 
                e->iport );
            FPRINTF(output ,"));\n");
            return;

        default:
/*            Error2( "PrintSumOfTerms", "ILLEGAL REDUCTION\n" ); */
            break;
        };

    PrintFldRef( e->dst->info->sname, "args", NULL_EDGE, "Out", e->iport );
    FPRINTF( output, ", " );
    PrintFldRef( e->dst->info->sname, "args", NULL_EDGE, "Out", e->iport );
    FPRINTF( output, ", " );
    PrintTemp( e );
    FPRINTF( output, " );\n" );
    FPRINTF( output, "FLUSHLINE(&(");
    PrintFldRef( e->dst->info->sname, "args", NULL_EDGE, "Out", e->iport );
    FPRINTF(output ,"));\n");
}


/**************************************************************************/
/* LOCAL  **************       PrintBasicRed       ************************/
/**************************************************************************/
/* PURPOSE: PRINT BASIC REDUCTION UPDATE MACRO macro TO OUTPUT.           */
/**************************************************************************/

static void PrintBasicRed( indent, i, macro )
int    indent;
PEDGE  i;
char  *macro;
{
    register PEDGE v = i->src->imp->isucc->isucc;

    PrintIndentation( indent );

    FPRINTF( output, "%sOpt%s( ", (v->isucc != NULL)? "B" : "", macro );

    PrintTemp( i );
    FPRINTF( output, ", " );
    PrintTemp( i );
    FPRINTF( output, ", " );
    PrintTemp( v );

    if ( v->isucc != NULL ) {
	FPRINTF( output, ", " );
	PrintTemp( v->isucc );
	}

    FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* LOCAL  **************       PrintUserRed        ************************/
/**************************************************************************/
/* PURPOSE: PRINT USER REDUCTION UPDATE MACRO macro TO OUTPUT.            */
/**************************************************************************/

static void PrintUserRed( indent , i )          /* TBD */
int    indent;
PEDGE  i;
{
    PNODE       graph;
    PEDGE       edge;

    edge = FindImport( i->src, 1 );             /* reduction call name */
    graph = FindFunction( UpperCase( edge->CoNsT, TRUE, FALSE ) );

    /*** TBD: Set in the arguments. ***/

    PrintIndentation( indent );
    FPRINTF( output, "((%s*)%s)->In%d = ", graph->info->sname,
        i->src->temp->name, 1 );
    FPRINTF( output, "%s;\n", i->temp->name );
    PrintIndentation( indent );
    if ( i->src->imp->CoNsT[3]=='b' ) {
        FPRINTF( output, "((%s*)%s)->In%d = ", graph->info->sname,
            i->src->temp->name, 2 );
        FPRINTF( output, "%s;\n", i->src->imp->isucc->isucc->temp->name );
    }

    /*** Print the call. ***/

    PrintIndentation( indent );
    FPRINTF( output, "LockParent;\n" );
    PrintIndentation( indent );
    FPRINTF( output, "Call( %s, ", UpperCase( edge->CoNsT, TRUE, FALSE ) );
    FPRINTF( output, "%s );", i->src->temp->name );
    FPRINTF( output, " /* REDUCE_USER Update */\n" );
    PrintIndentation( indent );
    FPRINTF( output, "UnlockParent;\n" );

    /*** Extract the results. ***/

    PrintIndentation( indent );
    if ( i->src->imp->CoNsT[3]=='b' ) {
        if ( !IsBRecord(i->src->imp->isucc->isucc->info) ) {
            FPRINTF( output, "DeAlloc(%s);\n", 
                i->src->imp->isucc->isucc->temp->name );
        }
    }
    PrintIndentation( indent );
    FPRINTF( output, "%s = ", i->temp->name );
    FPRINTF( output, "((%s*)%s)->Out%d;\n", graph->info->sname,
        i->src->temp->name, 1 );                /* TBD: multiple outputs */

}


/**************************************************************************/
/* LOCAL  **************       PrintRCatUpd        ************************/
/**************************************************************************/
/* PURPOSE: PRINT A Reduce Catenate UPDATE NODE TO output. THE REDUCE     */
/*          NODE IS ASSUMED TO BE i->src.                                 */
/**************************************************************************/

static void PrintRCatUpd( indent, i, v, bool, atnode )
int    indent;
PEDGE  i;
PEDGE  v;
PEDGE  bool;
int    atnode;
{
  register char  *macro;

  macro = (atnode)? "RCatAT" : "RCat";

  PrintIndentation( indent );

  FPRINTF( output, "%s%s%s( %s, ", (bool != NULL)? "B" : "", macro, 
	   IsBasic( i->info->A_ELEM )? "" : "X", i->info->A_ELEM->tname  );

  PrintTemp( i );

  FPRINTF( output, ", " );
  PrintTemp( v );

  if ( bool != NULL ) {
    FPRINTF( output, ", " );
    PrintTemp( bool );
    }

  if ( !atnode ) {
    if ( IsBasic( i->info->A_ELEM ) )
      FPRINTF( output, ", %s ", i->info->A_ELEM->tname );
    else
      FPRINTF( output, ", %s ", i->info->A_ELEM->sname );
    }

  if ( atnode ) {
    FPRINTF( output, ", %s, ", GetCopyFunction( i->info ) );
    /* THERE ISN'T A BOOLEAN IMPORT */
    PrintTemp( i->src->imp->isucc->isucc->isucc->isucc );
    }

  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* LOCAL  **************         PrintBldAT        ************************/
/**************************************************************************/
/* PURPOSE: PRINT AN ARRAY BuildAT AT MACRO TO output.                    */
/**************************************************************************/

static void PrintBldAT( indent, i, e, low, sz, b, n )
int   indent;
PEDGE i;
PEDGE e;
PEDGE low;
PEDGE sz;
PEDGE b;
PNODE n;
{
    register char *s;

    PrintIndentation( indent );

    switch ( n->type ) {
      case IFAGatherATDVI:
      case IFReduceATDVI:
	s = "DVI";
	break;

      case IFAGatherATDV:
      case IFReduceATDV:
	s = "DV";
	break;

      default:
	s = "";
	break;
      }

    FPRINTF( output, "%sBldAT%s( %s, ", 
	     (n->nmark)? "Opt" : "", s, i->info->A_ELEM->tname );

    PrintTemp( e );
    FPRINTF( output, ", " );
    PrintTemp( low );
    FPRINTF( output, ", " );
    PrintTemp( sz );
    FPRINTF( output, ", " );
    PrintTemp( b );
    FPRINTF( output, " );\n" );

    if ( i->src->usucc != NULL )
      if ( i->src->usucc->usucc == i->src )
	 return;

    if ( !(n->nmark) )
      PrintSetRefCount( indent, e, i->sr, FALSE );
}


/**************************************************************************/
/* LOCAL  **************      PrintReduceInit      ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE INITIALIZATION OF e->temp WITH THE INITIAL VALUE    */
/*          ASSOCIATED WITH THE REDUCTION FUNCTION DEFINED BY EDGE f      */
/*          TO output.                                                    */
/**************************************************************************/

static void PrintReduceInit( indent, f, e )
int   indent;
PEDGE f;
PEDGE e;
{
    PrintIndentation( indent );

    PrintTemp( e );
    FPRINTF( output, " = " );

    switch ( f->CoNsT[0] ) {
        case REDUCE_LEAST: 
            switch ( e->info->type ) {
                case IF_REAL:
                    FPRINTF( output, "MaxFloat;\n" );
                    break;

                case IF_DOUBLE:
                    FPRINTF( output, "MaxDouble;\n" );
                    break;

                case IF_INTEGER:
                    FPRINTF( output, "MaxInt;\n" );
                    break;

                case IF_BOOL:
                    FPRINTF( output, "True;\n" );
                    break;
                }

            break;

        case REDUCE_GREATEST:
            switch ( e->info->type ) {
                case IF_REAL:
                    FPRINTF( output, "MinFloat;\n" );
                    break;

                case IF_DOUBLE:
                    FPRINTF( output, "MinDouble;\n" );
                    break;

                case IF_INTEGER:
                    FPRINTF( output, "MinInt;\n" );
                    break;

                case IF_BOOL:
                    FPRINTF( output, "False;\n" );
                    break;
                }

            break;

        case REDUCE_SUM:
            if ( !IsBoolean( e->info ) ) {
	        PrintTemp( f->isucc );
	        FPRINTF( output, ";\n" );
	        }
            else
	        FPRINTF( output, "False;\n" );

	    break;

        case REDUCE_PRODUCT:
            if ( !IsBoolean( e->info ) ) {
	        PrintTemp( f->isucc );
	        FPRINTF( output, ";\n" );
	        }
            else
	        FPRINTF( output, "True;\n" );

	    break;

        case REDUCE_USER:
            if ( !IsBoolean( e->info ) ) {
                PrintTemp( f->isucc );
                FPRINTF( output, ";" );
                }
            else
                FPRINTF( output, "False;" );

            FPRINTF( output, " /* REDUCE_USER Return */\n" );

            break;

	default:
/*            Error2( "PrintReduceInit", "ILLEGAL REDUCTION\n" ); */
	    break;
	}
}


/**************************************************************************/
/* LOCAL  **************     GetSliceParam     ************************/
/**************************************************************************/
/* PURPOSE: IF EDGE i DEFINES A CONSTANT THEN RETURN IT, ELSE FIND AND    */
/*          RETURN THE CORRESPONDING IMPORT TO ENQUE NODE n.              */
/**************************************************************************/

PEDGE GetSliceParam( i, n )
PEDGE i;
PNODE n;
{
    register PEDGE ii;

    if ( IsConst( i ) )
	return( i );

    /* MAKE SOME COMPILER CORRECTNESS CHECKS ON PORT NUMBERS, ETC. */
    if ( !IsSGraph( i->src ) )
      Error2( "GetSliceParam", "i->src NOT AN IFSGraph" );

    if ( (ii = FindImport( i->src->G_DAD, i->eport )) == NULL )
      Error2( "GetSliceParam", "FindImport FOR LOOP FAILED" );

    if ( !IsForall( i->src->G_DAD ) )
      Error2( "GetSliceParam", "i->src->G_DAD NOT IFForall" );

DoItAgain:
    if ( IsConst( ii ) )
      Error2( "GetSliceParam", "FindImport FOUND A CONSTANT" );

    if ( ii->src->type != IFLPGraph ) {
      if ( ii->src->type != IFAssign )
        Error2( "GetSliceParam", "FindImport FOUND A NON-IFLPGraph SOURCE" );

      ii = ii->src->imp;
      goto DoItAgain;
      }

    if ( ii->eport != i->eport )
      Error2( "GetSliceParam", "FindImport FOUND A PORT NUMBER ERROR" );

    if ( (ii = FindImport( n, i->eport )) == NULL )
      Error2( "GetSliceParam", "FindImport FOR ENQUE FAILED" );

    return( ii );
}


/**************************************************************************/
/* GLOBAL **************        PrintRanges        ************************/
/**************************************************************************/
/* PURPOSE: PRINT LoopPoolEnq NODE n'S RANGE ARGUMENTS TO output.  THE    */
/*          SLICED LOOPS CONTROLLING RANGES ARE PRINTED.                  */
/**************************************************************************/

void PrintRanges( n )
PNODE n;
{
    FPRINTF( output, ", " );                              /* CONTROL LOW */
    PrintRangeLow(n);

    FPRINTF( output, ", " );                             /* CONTROL HIGH */
    PrintRangeHigh(n);
}
void PrintRangeLow( n )
PNODE n;
{
    register PEDGE i;

    /* GUARANTEED NOT TO BE ScatterBufPartitions                         */
    i = n->usucc->imp->src->F_GEN->imp;

    PrintTemp( GetSliceParam( i->src->imp->src->imp, n ) );
}
void PrintRangeHigh( n )
PNODE n;
{
    register PEDGE i;

    /* GUARANTEED NOT TO BE ScatterBufPartitions                         */
    i = n->usucc->imp->src->F_GEN->imp;

    PrintTemp( GetSliceParam( i->src->imp->isucc->src->imp, n ) );
}


/**************************************************************************/
/* GLOBAL **************    PrintSliceTaskInit     ************************/
/**************************************************************************/
/* PURPOSE: PRINT SLICE TASK INTIALIZATION MACROS TO output. EACH Out     */
/*          FIELD IS INTIALIZED ACCORDING TO THE MULTIPLE NODE BUILDING   */
/*          ITS VALUE.                                                    */
/**************************************************************************/

void PrintSliceTaskInit( indent, n )
int   indent;
PNODE n;
{
    register PEDGE i;

    for ( i = n->usucc->imp->src->F_RET->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

	switch ( i->src->type ) {
	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
                if ( i->src->imp->CoNsT[0] == REDUCE_USER ) {
                    PEDGE e =  FindExport( n, i->iport);
                    PEDGE edge;

                    PrintIndentation( indent );
                    PrintTemp( e );
                    FPRINTF( output, " = " );
                    for ( edge = e->src->imp; edge->isucc!=NULL;
                        edge = edge->isucc);    /* TBD: it is always last */
                    PrintTemp( edge );
                    FPRINTF( output, ";\n" );

                    PrintIndentation( indent );
                    FPRINTF( output, "((%s*)%s)->In%d", e->temp->info->sname,
                         e->temp->name, i->src->imp->isucc->temp->fld );
                    FPRINTF( output, " = " );
                    PrintTemp( edge );
                    FPRINTF( output, "; /* REDUCE_USER Init. */\n" );

                    break;
                    }

		PrintReduceInit( indent, i->src->imp, FindExport( n, i->iport));
		break;

	    case IFReduceAT:
	    case IFReduceATDVI:
	    case IFReduceATDV:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		PrintBldAT( indent, i, FindExport( n, i->iport ), 
			    GetSliceParam( i->src->imp->isucc, n ),
			    GetSliceParam( FindImport( i->src, 6 ), n ),
			    GetSliceParam( FindImport( i->src, 5 ), n ),
			    i->src );
		break;

	    case IFAGatherATDVI:
	    case IFAGatherATDV:
	    case IFAGatherAT:
		PrintBldAT( indent, i, FindExport( n, i->iport ), 
			    GetSliceParam( i->src->imp, n ),
			    GetSliceParam( FindImport( i->src, 5 ), n ),
			    GetSliceParam( FindImport( i->src, 4 ), n ),
			    i->src );
		break;

            case IFUReduce:
                break;

            default:
                UNEXPECTED("Missing return type");
	    }
	}
}


/**************************************************************************/
/* GLOBAL **************     PrintReturnRapUp      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RETURN SUBGRAPH r'S RAP-UP MACROS TO output.  THIS IS   */
/*          DONE AFTER THE LOOP HAS COMPLETED TO FINALIZE THE RESULTS.    */
/**************************************************************************/

void PrintReturnRapUp( indent, r )
int   indent;
PNODE r;
{
    register PEDGE i;
    PNODE n;

    if ( r->G_DAD->smark )                               /* SLICED LOOP? */
	return;

    for ( i = r->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

	switch ( i->src->type ) {
	    case IFFinalValue:
	    case IFFinalValueAT:
		if ( i->src->lmark )
		    PrintAssgn( indent, i, i->src->imp );

		PrintProducerModifiers( indent, i->src );
		break;

            case IFReduceAT:
	    case IFReduceATDVI:
	    case IFReduceATDV:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		PrintBldAT( indent, i, i, i->src->imp->isucc,  
			    FindImport( i->src, 6 ), FindImport( i->src, 5 ),
			    i->src );

		if ( i->src->imp->isucc->isucc->pmark )
		    if ( !(i->src->nmark)  ) {
		      PrintIndentation( indent );

		      FPRINTF( output, "PRedATRapUp( " );
		      PrintTemp( FindImport( i->src, 5 ) );
		      FPRINTF( output, " );\n" );
		      }

		break;

	    case IFAGatherATDVI:
	    case IFAGatherATDV:
	    case IFAGatherAT:
                PrintBldAT( indent, i, i, i->src->imp, 
			    FindImport( i->src, 5 ), FindImport( i->src, 4 ),
			    i->src );
		break;

	    case IFAGather:
		break;

            case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		break;

            case IFUReduce:
                break;

	    default:
		UNEXPECTED("Missing return type");
	    }
	}

        /* PRINT USER-REDUCTION WRAP UP PER NODE.                         */

        for ( n = r->nsucc; n != NULL; n = n->nsucc ) {
            if ( n->type == IFUReduce )
                PrintUReduceRapUp( indent, n );
        }
}


/**************************************************************************/
/* LOCAL  **************       PrintReturnUpd      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RETURN SUBGRAPH r'S UPDATE MACROS TO output.            */
/**************************************************************************/

static void PrintReturnUpd( indent, r )
int   indent;
PNODE r;
{
    register PEDGE i;
    register PEDGE b;
    PNODE n;

    for ( i = r->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

	switch ( i->src->type ) {
	    case IFAGather:
		b = i->src->imp->isucc->isucc;

		PrintIndentation( indent );

		FPRINTF( output, "%sGathUpd%s( %s, ", (b != NULL)? "B" : "",
			 (b != NULL & !IsBasic( i->info->A_ELEM ))? "X" : "",
			 i->info->A_ELEM->tname                           );

		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( i->src->imp->isucc );

		if ( b != NULL ) {
		    FPRINTF( output, ", " );
		    PrintTemp( b );

		    if ( !IsBasic( i->info->A_ELEM ) )
	              FPRINTF( output, ", %s ", GetFreeName( i->info->A_ELEM ));
                    }

		FPRINTF( output, " );\n" );
		break;

	    case IFReduceAT:
	    case IFReduceATDVI:
	    case IFReduceATDV:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
              if ( !(i->src->imp->isucc->isucc->pmark) )
		PrintRCatUpd( indent, i,i->src->imp->isucc->isucc, NULL_EDGE, TRUE );

	      PrintConsumerModifiers( indent, i->src );
              break;

	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		if ( i->src->gsucc != NULL )
		    break;

		if ( i->src->imp->CoNsT[0] == REDUCE_CATENATE ) {
		    PrintRCatUpd( indent, i, i->src->imp->isucc->isucc,
			          i->src->imp->isucc->isucc->isucc, FALSE );

		    PrintConsumerModifiers( indent, i->src );
		    break;
		    }

		switch ( i->src->imp->CoNsT[0] ) {
                    case REDUCE_LEAST: 
			PrintBasicRed( indent, i, "Min" );
			break;

                    case REDUCE_GREATEST:
                        PrintBasicRed( indent, i, "Max" );
			break;

                    case REDUCE_PRODUCT:
			if ( IsBoolean( i->info ) )
                            PrintBasicRed( indent, i, "And" );
			else
                            PrintBasicRed( indent, i, "Times" );

			break;

                    case REDUCE_SUM:
			if ( IsBoolean( i->info ) )
                            PrintBasicRed( indent, i, "Or" );
			else
                            PrintBasicRed( indent, i, "Plus" );

			break;

                    case REDUCE_USER:
                        PrintUserRed( indent, i );

                        break;

                    default:
/*                      Error2( "PrintReturnUpd", "ILLEGAL REDUCTION\n" ); */
                        break;
		    }

		break;
		
	    case IFAGatherATDVI:
	    case IFAGatherATDV:
	    case IFAGatherAT:
		/* NEW CANN 2/92 SKIP IMPLICIT ITERATION */
		if ( i->src->umark ) {
		  if ( !IsBasic( i->src->imp->isucc->info ) )
		    if ( i->src->imp->isucc->cm != -1 )
		      PrintFreeCall( indent, i->src->imp->isucc );

		  i->src->umark = FALSE;
		  break;
		  }

		if ( i->src->gsucc != NULL )
		    break;

		PrintIndentation( indent );

		if ( r->G_DAD->vmark || r->G_DAD->smark ) {
		    FPRINTF( output, "VecGathATUpd( " );
		    PrintTemp( r->G_DAD->F_GEN->imp );
		    FPRINTF( output, ", %s, ", i->info->A_ELEM->tname );
		    }
		else
		    FPRINTF( output, "GathATUpd( %s, ", i->info->A_ELEM->tname);

		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( i->src->imp->isucc );
		FPRINTF( output, " );\n" );
		break;

	    case IFFinalValueAT:
	    case IFFinalValue:
		if ( i->src->lmark )
		    break;

		PrintIndentation( indent );

		b = i->src->imp->isucc;

		FPRINTF( output, "%sFvUpd", (b != NULL)? "B" : "" );

		if ( IsBasic( i->info ) )
		  FPRINTF( output, "( " );
		else { 
		  if ( i->src->imp->cm == -1 || i->src->imp->dmark )
		    FPRINTF( output, "X( " );
		  else 
		    FPRINTF( output, "( " );
		  /* else Error2( "DEBUG", "FvUpdX ERROR" ); */
		  }

		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( i->src->imp );

		if ( b != NULL ) {
		    FPRINTF( output, ", " );
		    PrintTemp( b );
		    }

		if ( !IsBasic( i->info ) )
		  if ( i->src->imp->cm == -1 || i->src->imp->dmark )
	            FPRINTF( output, ", %s ", GetFreeName( i->info ) );

		FPRINTF( output, " );\n" );
		break;

            case IFUReduce:
                break;

            default:
                UNEXPECTED("Missing return type");
	    }
	}

        /* PRINT USER-REDUCTION UPDATES PER NODE.                         */

        for ( n = r->nsucc; n != NULL; n = n->nsucc ) {
            if ( n->type == IFUReduce )
                PrintUReduceUpd( indent, n );
        }
}


/**************************************************************************/
/* LOCAL  **************      PrintReturnInit      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RETURN SUBGRAPH r'S INITIALIZATION MACROS TO output.    */
/**************************************************************************/

static void PrintReturnInit( indent, r )
int   indent;
PNODE r;
{
    register PEDGE  i;
    register char  *s;
    PNODE n;

    for ( i = r->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

	switch ( i->src->type ) {
	    case IFAGather:
		PrintIndentation( indent );

		FPRINTF( output, "DoABld( " );
		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( i->src->imp );
		FPRINTF( output, ", %d );\n", i->sr );
		break;

	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		if ( i->src->imp->CoNsT[0] == REDUCE_CATENATE ) {
		    PrintIndentation( indent );

		    FPRINTF( output, "DoABld( " );
		    PrintTemp( i );
		    FPRINTF( output, ", " );
		    PrintTemp( i->src->imp->isucc );
		    FPRINTF( output, ", %d );\n", i->sr );
		    break;
		    }

		PrintReduceInit( indent, i->src->imp, i );
		break;
		
	    case IFFinalValue:
	    case IFFinalValueAT:
		if ( i->src->lmark || IsBasic( i->info ) )
		    break;

		if ( i->src->imp->cm == -1 || i->src->imp->dmark ) {
		  PrintIndentation( indent );
		  FPRINTF( output, "InitFvUpdX( " );
		  PrintTemp( i );
		  FPRINTF( output, " );\n" );
		  }
		/* else Error2( "DEBUG", "InitFvUpdX ERROR" ); */

		break;

	    case IFReduceAT:
	    case IFReduceATDVI:
	    case IFReduceATDV:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		if ( r->G_DAD->smark )
		    break;

		s = NULL;

		if ( i->src->imp->isucc->isucc->pmark ) {
		  if ( !(i->src->nmark) )
		    s = "PRedATInit";
		  }
                else
		    s = "RedATInit";

		if ( s == NULL )
		  break;

		PrintIndentation( indent );
		FPRINTF( output, "%s( ", s );
		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( FindImport( i->src, 5 ) );
		FPRINTF( output, " );\n" );
		break;

	    case IFAGatherATDVI:
	    case IFAGatherATDV:
	    case IFAGatherAT:
		PrintIndentation( indent );

		if ( r->G_DAD->smark ) {
		    FPRINTF( output, "VecSliceGathATInit( " );
		    PrintTemp( r->G_DAD->F_GEN->imp );
		    FPRINTF( output, "," );
		    }
		else if ( r->G_DAD->vmark ) {
		    FPRINTF( output, "VecGathATInit( " );
		    PrintTemp( r->G_DAD->F_GEN->imp );
		    FPRINTF( output, "," );
		    }
                else
		    FPRINTF( output, "GathATInit(" );

		FPRINTF( output, " %s, ", i->info->A_ELEM->tname );
		PrintTemp( i );
		FPRINTF( output, ", " );
		PrintTemp( FindImport( i->src, 4 ) );
		FPRINTF( output, " );\n" );
		break;

            case IFUReduce:
                break;

            default:
                UNEXPECTED("Missing return type");
	    }
	}

        /* PRINT USER-REDUCTION RETURN INITIALIZATIONS PER NODE.          */

        for ( n = r->nsucc; n != NULL; n = n->nsucc ) {
            if ( n->type == IFUReduce )
                PrintUReduceInit( indent, n );
        }
}


/**************************************************************************/
/* LOCAL  **************       PrintGenInit        ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE INTIALIZATION MACROS FOR Forall NODE f'S GENERATE   */
/*          SUBGRAPH TO output. ONLY NoOp AND Assgn NODES ARE CONSIDERED. */
/**************************************************************************/

static void PrintGenInit( indent, f )
int   indent;
PNODE f;
{
    register PNODE n;
    register PEDGE i;
	     char  buf[100];

    if ( f->smark ) {
	i = f->F_GEN->imp;

        PrintIndentation( indent );
        PrintTemp( i->src->imp );
        FPRINTF( output, " = lo;\n" );

        PrintIndentation( indent );
        PrintTemp( i->src->imp->isucc );
        FPRINTF( output, " = hi;\n" );
	}

    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc )
	switch ( n->type ) {
	    case IFRangeGenerate:
		break;

	    case IFScatterBufPartitions:
		if ( f->smark )
                    SPRINTF( buf, "%sSliceBufCpy( %s, ",
			     (f->vmark)? "Vec" : "",
			     n->imp->info->A_ELEM->A_ELEM->tname );
                else if ( n->imp->temp != n->exp->temp )
                    SPRINTF( buf, "%sBufCpy( ", (f->vmark)? "Vec" : "" );
                else
		    break;

		PrintIndentation( indent );

		FPRINTF( output, "%s", buf );
		PrintTemp( n->exp );
		FPRINTF( output, ", " );
		PrintTemp( n->imp );
		FPRINTF( output, ", " );
		PrintTemp( n->imp->isucc );

		if ( f->smark ) {
		  FPRINTF( output, ", " );
		  PrintTemp( f->F_GEN->imp );
		  }

		FPRINTF( output, " );\n" );

		break;

	    case IFAssign:
		if ( IsScatterBufPartitions( n->exp->dst ) ) {
		  if ( !(f->vmark) )
		    PrintAssgn( indent, n->exp, n->imp );

		  break;
                  }

		if ( !(f->smark) )
		  PrintAssgn( indent, n->exp, n->imp );

		break;

	    default:
		break;
	    }
}


/**************************************************************************/
/* LOCAL  **************        PrintGenUpd        ************************/
/**************************************************************************/
/* PURPOSE: PRINT Forall NODE f'S GENERATE UPDATE MACROS TO output.       */
/**************************************************************************/

static void PrintGenUpd( indent, f )
int   indent;
PNODE f;
{
    register PNODE n;

    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc )
        switch ( n->type ) {
	    case IFScatterBufPartitions:
		PrintIndentation( indent );

		FPRINTF( output, "%sOptShiftB( %s, ", 
			 (f->vmark)? "Vec" : "",
			 n->exp->info->A_ELEM->A_ELEM->tname );
		PrintTemp( n->exp );
		FPRINTF( output, ", " );
		PrintTemp( n->exp );
		FPRINTF( output, ", " );
		PrintTemp( n->imp->isucc );
		FPRINTF( output, " );\n" );
		break;

	    case IFRangeGenerate:
		if ( (alliantfx && (!f->vmark)) || 
		     (!(n == f->F_GEN->imp->src)) ) {
		  PrintIndentation( indent );
		  PrintTemp( n->exp );
		  FPRINTF( output, "++;\n" );
		  }

		break;

	    default:
		break;
	    }
}


/**************************************************************************/
/* LOCAL  **************       PrintGenRapUp       ************************/
/**************************************************************************/
/* PURPOSE: PRINT Forall NODE f'S GENERATE RAP-UP MACROS TO output.       */
/**************************************************************************/

static void PrintGenRapUp( indent, f )
int   indent;
PNODE f;
{
}


/**************************************************************************/
/* LOCAL  **************      PrintGenControl      ************************/
/**************************************************************************/
/* PURPOSE: PRINT Forall NODE f's LOOP CONTROL MACROS TO output.          */
/**************************************************************************/

static void PrintGenControl( indent, f )
int   indent;
PNODE f;
{
    register PNODE  n;
    register PNODE  nn;
    register PEDGE  i;
    register PEDGE  e;
    register PNODE  g;
	     char   buf[200];
	     char   *Reason;

    n = f->F_GEN->imp->src;
    g = f->F_BODY;

    if ( IsRangeGenerate( n ) ) {

      if ( g->flps != NULL ) {
        if ( g->flps[ARITHMETICS] > 0 ) {
          FPRINTF( output, "IncFlopCountA(" );
          FPRINTF( output, "(((double)%d)*((double)", g->flps[ARITHMETICS] );
          FPRINTF( output, "(((" );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1) < 0)? 0 : " );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1))));\n" );
          }
        if ( g->flps[LOGICALS] > 0 ) {
          FPRINTF( output, "IncFlopCountL(" );
          FPRINTF( output, "(((double)%d)*((double)", g->flps[LOGICALS] );
          FPRINTF( output, "(((" );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1) < 0)? 0 : " );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1))));\n" );
          }
        if ( g->flps[INTRINSICS] > 0 ) {
          FPRINTF( output, "IncFlopCountI(" );
          FPRINTF( output, "(((double)%d)*((double)", g->flps[INTRINSICS] );
          FPRINTF( output, "(((" );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1) < 0)? 0 : " );
          PrintTemp( n->imp->isucc );
          FPRINTF( output, "-" );
          PrintTemp( n->imp );
          FPRINTF( output, "+1))));\n" );
          }

        g->flps = NULL;
        }

	if ( f->vmark ) {
	  if ( alliantfx ) {
	    for ( i = f->imp; i != NULL; i = i->isucc )
	      if ( IsBuffer( i->info ) ) {
	        PrintVectorTemp( output, i );
	        e = FindExport( f->F_RET, i->iport );
	        FPRINTF( output, " = (%s *) ", i->info->A_ELEM->A_ELEM->tname );
	        PrintTemp( e->dst->exp );
	        FPRINTF( output, ";\n" );
	        }

	    PrintVECTOR();
	    }

	  if ( cRay ) {
	    if ( IsIvdepOk( f->F_BODY, &Reason ) ) {
	      PrintVECTOR();
            } else {
              FPRINTF( stderr, "OSC INTERNAL WARNING: IVDEP FAILURE %s\n", 
		      GetSisalInfo( f, buf )                           );
	      FPRINTF( stderr, "-- %s\n",Reason );
	    }
	  }

	  PrintASSOC();

	  if ( cRay && fvc )
	    FPRINTF( output, "#line %d \"%s\"\n", 
		     (f->line <= 0)? 1 : f->line,
		     (f->file == NULL)? "UNKNOWN_FILE.SIS" : f->file );

	  if ( alliantfx )
	    for ( nn = f->F_RET->G_NODES; nn != NULL; nn = nn->nsucc )
	      if ( nn->type == IFAGatherAT || 
		   nn->type == IFAGatherATDV ||
		   nn->type == IFAGatherATDVI ) {
	        i = FindImport( nn, 4 );
		PrintSAFE(PrintVectorTemp( (FILE*)NULL, FindImport(f,i->eport) ));
	        }
          }
        /* NOVECTOR sometimes breaks fxc */
        else if ( f->ThinCopy && (!alliantfx) )
	  PrintNOVECTOR();

	if ( (!alliantfx) || f->vmark ) {
	  PrintIndentation( indent );
	  FPRINTF( output, "for ( " );

	  if ( alliantfx ) {
	    PrintTemp( n->imp );
	    FPRINTF( output, " = " );
	    PrintTemp( n->imp );
	  }

	  FPRINTF( output, "; " );

	  /* ------------------------------------------------------------ */
	  /* Choose a loop style for this generator if parallel, else	  */
	  /* just use the standard step style.				  */
	  /* ------------------------------------------------------------ */
	  if ( !f->smark ) {
	    /* Normal, inline loop */
	    PrintTemp( n->imp );
	    FPRINTF( output, " <= " );
	    PrintTemp( n->imp->isucc );
	    FPRINTF( output, "; " );

	    PrintTemp( n->imp );
	    FPRINTF( output, "++ ) {  /* Normal Loop */\n" );

	  } else {
	    switch ( f->Style ) {
	      /* -------------------------------------------------------- */
	      /* STRIDED */
	     case 'S':
	      PrintTemp( n->imp );
	      FPRINTF( output, " <= " );
	      PrintTemp( n->imp->isucc );
	      FPRINTF( output, "; " );

	      PrintTemp( n->imp );
	      FPRINTF( output, " += step ) {" );
	      break;

	      /* -------------------------------------------------------- */
	      /* GSS or BLOCKED */
	     case 'G':
	     case 'B':
	     case 'C':
	      PrintTemp( n->imp );
	      FPRINTF( output, " <= " );
	      PrintTemp( n->imp->isucc );
	      FPRINTF( output, "; " );

	      PrintTemp( n->imp );
	      FPRINTF( output, "++ ) {" );
	      break;

	      /* -------------------------------------------------------- */
	      /* RUNTIME DECISION */
	     case 'R':
	      PrintTemp( n->imp );
	      FPRINTF( output, " <= " );
	      PrintTemp( n->imp->isucc );
	      FPRINTF( output, "; " );

	      PrintTemp( n->imp );
	      FPRINTF( output, " += step ) {" );
	      break;

	     default:
	      SPRINTF(buf,"Invalid Loop Style '%c' -- loop %d",
		      f->Style,f->ID);
	      Error1(buf);
	    }
	    FPRINTF(output,"	/* Style=%c */\n",f->Style);
	  }
        } else {
	  PrintIndentation( indent );
	  FPRINTF( output, "while ( " );
	  PrintTemp( n->imp );
	  FPRINTF( output, " <= " );
	  PrintTemp( n->imp->isucc );
	  FPRINTF( output, ") {\n" );
	}
    } else {
	PrintIndentation( indent );
	FPRINTF( output, "for ( ;; ) {\n" );
	}
}


static void PrintVecNodeImport( indent, i )
int   indent;
PEDGE i;
{

  if ( IsConst( i ) ) {
    FPRINTF( output, "%s", i->CoNsT );
    return;
    }

  if ( i->temp != NULL ) {
    PrintTemp( i );
    return;
    }

  PrintVecNode( indent, i->src );
}

static int ncnt = 0;

static void PrintVecNode( indent, n )
int   indent;
PNODE n;
{
  register char op;

  switch ( n->type ) {
    case IFPlus:
      op = '+'; goto PrintIt;
    case IFMinus:
      op = '-'; goto PrintIt;
    case IFTimes:
      op = '*'; goto PrintIt;
    case IFDiv:
      op = '/';

PrintIt:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      FPRINTF( output, "(" );
      PrintVecNodeImport( indent,  n->imp );
      FPRINTF( output, " %c ", op );

      if ( IsDivByZero( n ) )
	FPRINTF( output, "DivByZero()" );
      else
        PrintVecNodeImport( indent, n->imp->isucc );
      FPRINTF( output, ")" );
      break;

    case IFNeg:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      FPRINTF( output, "(-(" );
      PrintVecNodeImport( indent, n->imp );
      FPRINTF( output, "))" );
      break;

    case IFDouble:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      FPRINTF( output, "((double)" );
      PrintVecNodeImport( indent, n->imp );
      FPRINTF( output, ")" );
      break;

    case IFTrunc:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      FPRINTF( output, "((int)" );
      PrintVecNodeImport( indent, n->imp );
      FPRINTF( output, ")" );
      break;

    case IFSingle:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      FPRINTF( output, "((float)" );
      PrintVecNodeImport( indent, n->imp );
      FPRINTF( output, ")" );
      break;

    case IFAbs:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      if (n->exp->info->type == IF_INTEGER)
        FPRINTF( output, "abs(" );
      else
        FPRINTF( output, "fabs(" );

      PrintVecNodeImport( indent, n->imp );
      FPRINTF( output, ")" );
      break;

    case IFOptAElement:
      if ( ncnt++ > 6 ) {
	 FPRINTF( output, "\n" );
         PrintIndentation( indent + 2 );
	 ncnt = 0;
	 }

      PrintTemp( FindImport( n->imp->src->G_DAD, n->imp->eport ) );
      FPRINTF( output, "[" );
      PrintVecNodeImport( indent, n->imp->isucc );
      FPRINTF( output, "]" );
      break;

    default:
      Error2( "PrintVecNode", "ILLEGAL VECTOR NODE\n" );
      break;
    }
}


static void PrintVecUpd( indent, i, f )
int   indent;
PEDGE i;
PNODE f;
{
  register PEDGE e;

  if ( IsConst( i ) )
    FPRINTF( output, "%s", i->CoNsT );
  else if ( (e=FindImport( f->F_BODY, i->eport)) == NULL )
    PrintTemp( i );
  else
    PrintVecNodeImport( indent, e );
}


static void PrintVecBody( indent, f )
int   indent;
PNODE f;
{
  register PNODE n;
  register PEDGE i;
  register char  op;

  for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc )
    if ( n->exp->temp != NULL ) {
      ncnt = 0;
      PrintIndentation( indent );
      PrintTemp( n->exp );
      FPRINTF( output, " = " );
      PrintVecNode( indent, n );
      FPRINTF( output, ";\n" );
      }

  for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc )
    switch ( n->type ) {
      case IFReduce:
      case IFRedLeft:
      case IFRedRight:
      case IFRedTree:
        switch ( n->imp->CoNsT[0] ) {
          case REDUCE_LEAST: 
	    op = '>'; goto PrintIt1;
          case REDUCE_GREATEST:
	    op = '<';
PrintIt1:
	    if ( !IsConst( n->imp->isucc->isucc ) )
	      if ( IsImport( n->exp->dst->G_DAD->F_BODY, 
			     n->imp->isucc->isucc->eport ) ) {
	      PrintIndentation( indent );
	      PrintTemp( n->imp->isucc->isucc );
	      FPRINTF( output, " = " );
	      PrintVecUpd( indent, n->imp->isucc->isucc, f );
	      FPRINTF( output, ";\n" );
	      }

            PrintIndentation( indent );
            FPRINTF( output, "if ( " );
            PrintTemp( n->exp );
            FPRINTF( output, " %c ", op );
            PrintTemp( n->imp->isucc->isucc );
            FPRINTF( output, " ) " );
            PrintTemp( n->exp );
            FPRINTF( output, " = " );
            PrintTemp( n->imp->isucc->isucc );
            FPRINTF( output, ";\n" );
            break;
      
          case REDUCE_PRODUCT:
	    op = '*'; goto PrintIt2;
          case REDUCE_SUM:
            op = '+';
PrintIt2:
	    ncnt =0;
            PrintIndentation( indent );
            PrintTemp( n->exp );
            FPRINTF( output, " %c= ", op );

	    PrintVecUpd( indent, n->imp->isucc->isucc, f );

            FPRINTF( output, ";\n" );
            break;
      
          default:
            Error2( "PrintVecBody", "ILLEGAL VECTOR RETURN NODE\n" );
            break;
          }
      
        break;

      case IFAGatherATDVI:
      case IFAGatherATDV:
      case IFAGatherAT:
	ncnt =0;
        PrintIndentation( indent );
	i = FindImport( n, 4 );
        PrintVectorTemp( output, FindImport( f, i->eport ) );
        FPRINTF( output, "[" );
        PrintTemp( f->F_GEN->imp );
        FPRINTF( output, " ] = " );

	PrintVecUpd( indent, n->imp->isucc, f );

        FPRINTF( output, ";\n" );
        break;

      default:
        Error2( "PrintVecBody", "ILLEGAL VECTOR RETURN NODE\n" );
        break;
      }
}


#define GENF_NODE_THRESHOLD 4
#define GENF_IMP_THRESHOLD  10

static int hcnt = 0;

static void PrintHTemp( e )
PEDGE e;
{
    if ( IsConst( e ) ) {
        FPRINTF( hyfd, "(%s)", e->CoNsT );
	return;
	}

    if ( e->temp == NULL ) {
	if ( e->src->type == IFAIndexMinus ) {
	  PrintHTemp( e->src->imp );

	  if ( IsConst( e->src->imp->isucc ) )
	    if ( atoi( e->src->imp->isucc->CoNsT ) == 0 )
	      return;

	  FPRINTF( hyfd, "-" );
	  PrintHTemp( e->src->imp->isucc );
	  return;
	  }

	if ( e->src->type == IFAIndexPlus ) {
	  PrintHTemp( e->src->imp );

	  if ( IsConst( e->src->imp->isucc ) )
	    if ( atoi( e->src->imp->isucc->CoNsT ) == 0 )
	      return;

	  FPRINTF( hyfd, "+" );
	  PrintHTemp( e->src->imp->isucc );
	  return;
	  } 

	Error1( "PrintHTemp: temp == NULL" );
	}

    switch ( e->info->type ) {
      case IF_REAL:
	FPRINTF( hyfd, "R" );
	break;

      case IF_DOUBLE:
	FPRINTF( hyfd, "D" );
	break;

      case IF_INTEGER:
	FPRINTF( hyfd, "I" );
	break;

      default:
	break;
      }

    FPRINTF( hyfd, "%s", e->temp->name );
}


static char *GetHybridTypeProlog( i )
PEDGE i;
{
  switch( i->info->type ) {
    case IF_ARRAY:
      switch( i->info->A_ELEM->type ) {
	case IF_INTEGER:
	  return( "integer" );

	case IF_REAL:
	  return( "real*4" );

	case IF_DOUBLE:
	  return( "real*8" );

	default:
	  Error2( "GetHybridTypeProlog", "ILLEGAL ARRAY COMPONENT TYPE!!!" );
	}

      break;

    case IF_PTR_INTEGER:
    case IF_INTEGER:
      return( "integer" );

    case IF_PTR_REAL:
    case IF_REAL:
      return( "real*4" );

    case IF_PTR_DOUBLE:
    case IF_DOUBLE:
      return( "real*8" );

    default:
      Error2( "GetHybridTypeProlog", "ILLEGAL TYPE ENCOUNTERED!!!" );
    }

  return NULL;
}


static void PrintHybridGraph( f, g, crod )
PNODE f;
PNODE g;
PEDGE crod;
{
  register PNODE  n;
  register char  *op;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    switch ( n->type ) {
      case IFAssign:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, "\n" );
	break;

      case IFGreat:
      case IFGreatEqual:
      case IFLess:
      case IFLessEqual:
      case IFEqual:
      case IFNotEqual:
        break;

      case IFSelect:
        FPRINTF( hyfd, "       if ( " );
	PrintHTemp( n->usucc->imp );

        switch ( n->usucc->type ) {
          case IFGreat:
	    FPRINTF( hyfd, " .gt. " );
	    break;

          case IFGreatEqual:
	    FPRINTF( hyfd, " .ge. " );
	    break;

          case IFLess:
	    FPRINTF( hyfd, " .lt. " );
	    break;

          case IFLessEqual:
	    FPRINTF( hyfd, " .le. " );
	    break;

          case IFEqual:
	    FPRINTF( hyfd, " .eq. " );
	    break;

          case IFNotEqual:
	    FPRINTF( hyfd, " .ne. " );
	    break;

          default:
	    break;
          }

	PrintHTemp( n->usucc->imp->isucc );
	FPRINTF( hyfd, " ) then\n" );

        PrintHybridGraph( f, n->S_CONS, crod );

        FPRINTF( hyfd, "       else\n" );

        PrintHybridGraph( f, n->S_ALT, crod );

        FPRINTF( hyfd, "       endif\n" );
        break;

      case IFLeast:
	op = "min";
	goto DoReduction1;
      case IFGreatest:
	op = "max";
DoReduction1:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->gsucc->exp );
	FPRINTF( hyfd, " = %s( ", op );
	PrintHTemp( n->gsucc->exp );
	FPRINTF( hyfd, ", " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, " )\n" );
	break;

      case IFProduct:
	op = " * ";
	goto DoReduction2;
      case IFSum:
        op = " + ";
DoReduction2:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->gsucc->exp );
	FPRINTF( hyfd, " = " );
	PrintHTemp( n->gsucc->exp );
	FPRINTF( hyfd, "%s", op );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, "\n" );
	break;

      case IFAStore:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->gsucc->exp );
	FPRINTF( hyfd, "(" );

	if ( f->vmark || f->smark )
	  PrintHTemp( crod );
	else
	  FPRINTF( hyfd, "iii" );

	FPRINTF( hyfd, ") = " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, "\n" );
	break;

      case IFCall:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = %s(", n->imp->CoNsT );
	PrintHTemp( n->imp->isucc );
	FPRINTF( hyfd, ")\n" );
	break;

      case IFSingle:
	op = "real";
	goto DoMop;
      case IFDouble:
        op = "dble";
	goto DoMop;
      case IFTrunc:
        op = "int";
	goto DoMop;
      case IFNeg:
	op = "-";
DoMop:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = %s(", op );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, ")\n" );
	break;

      case IFPlus:
	op = " + ";
	goto DoDyadic;
      case IFMinus:
	op = " - ";
	goto DoDyadic;
      case IFTimes:
	op = " * ";
	goto DoDyadic;
      case IFDiv:
	op = " / ";
DoDyadic:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, "%s", op );
	PrintHTemp( n->imp->isucc );
	FPRINTF( hyfd, "\n" );
	break;
 

      case IFMod:
	op = "mod";
	goto DoPrefix;
      case IFMin:
        op = "min";
	goto DoPrefix;
      case IFMax:
        op = "max";
DoPrefix:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = %s(", op );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, ", " );
	PrintHTemp( n->imp->isucc );
	FPRINTF( hyfd, " )\n" );
	break;

      case IFAbs:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = " );
	FPRINTF( hyfd, "abs( " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, " ) \n"  );
	break;


      case IFAIndexPlus:
      case IFAIndexMinus:
	break;

      case IFOptAElement:
        FPRINTF( hyfd, "       " );
	PrintHTemp( n->exp );
	FPRINTF( hyfd, " = " );
	PrintHTemp( n->imp );
	FPRINTF( hyfd, "(" );
	PrintHTemp( n->imp->isucc );
	FPRINTF( hyfd, ")\n" );
	break;

      default:
	Error2( "PrintHybridGraph", "ILLEGAL BODY NODE!!!" );
      }
    }
}


static void PrintHybrid( f )
PNODE f;
{
  register PEDGE  i;
  register char  *nm;
  register PEDGE  crod;
           char   buf[100];

  SPRINTF( buf, "h%d%3.3s", ++hcnt, hybrid );
  nm = BindInterfaceName( buf, FOR_FORTRAN, ' ' );

  crod = f->F_GEN->imp;


  /* PRINT THE HYBRID CALL */

  FPRINTF( output, "{\n" );
  FPRINTF( output, "  int lo1,lo2; \n" );

  for ( i = f->imp; i != NULL; i = i->isucc ) {
    if ( !IsExport( f->F_BODY, i->iport ) )
      continue;
    if ( !IsArithmetic( i->info ) )
      continue;

    FPRINTF( output, "  %s fin%d;\n", i->info->tname, i->iport );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( !IsArithmetic( i->info ) )
      continue;

    FPRINTF( output, "  %s fout%d;\n", i->info->tname, i->iport );
    }

  i = crod->src->imp;

  FPRINTF( output, "  lo1 = " );
  PrintTemp( i );
  FPRINTF( output, ";\n" );

  FPRINTF( output, "  lo2 = " );
  PrintTemp( i->isucc );
  FPRINTF( output, ";\n" );

  for ( i = f->imp; i != NULL; i = i->isucc ) {
    if ( !IsExport( f->F_BODY, i->iport ) )
      continue;
    if ( !IsArithmetic( i->info ) )
      continue;

    FPRINTF( output, "  fin%d = ", i->iport );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( !IsArithmetic( i->info ) )
      continue;

    FPRINTF( output, "  fout%d = ", i->iport );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
    }

  FPRINTF( output, "  %7.7s( &lo1, &lo2", nm );

  for ( i = f->imp; i != NULL; i = i->isucc ) {
    if ( !IsExport( f->F_BODY, i->iport ) )
      continue;

    if ( IsArithmeticPtr( i->info ) ) {
      FPRINTF( output, ", " );
      PrintTemp( i );
      FPRINTF( output, "+1" );
      continue;
      }

    FPRINTF( output, ", &fin%d", i->iport );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( IsArithmetic( i->info ) ) {
      FPRINTF( output, ", &fout%d", i->iport );
      continue;
      }

    FPRINTF( output, ", ((%s*)", i->info->A_ELEM->tname );
    PrintTemp( i );
    FPRINTF( output, ")+1" );
    }

  FPRINTF( output, " );\n" );

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( !IsArithmetic( i->info ) )
      continue;

    FPRINTF( output, "  " );
    PrintTemp( i );
    FPRINTF( output, " = fout%d;\n", i->iport );
    }

  FPRINTF( output, "}\n" );

  /* PRINT THE HYBRID SUBROUTINE */

  FPRINTF( hyfd, "\n       subroutine %7.7s( lo,hi\n", buf );

  for ( i = f->imp; i != NULL; i = i->isucc ) {
    if ( !IsExport( f->F_BODY, i->iport ) )
      continue;

    FPRINTF( hyfd, "     *," );
    PrintHTemp( i );
    FPRINTF( hyfd, "\n" );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    FPRINTF( hyfd, "     *," );
    PrintHTemp( i );
    FPRINTF( hyfd, "\n" );
    }

  FPRINTF( hyfd, "     *)\n" );

  FPRINTF( hyfd, "      implicit integer (I)\n" );
  FPRINTF( hyfd, "      implicit real*4  (R)\n" );
  FPRINTF( hyfd, "      implicit real*8  (D)\n" );

  FPRINTF( hyfd, "      integer lo,hi,iii\n" );

  for ( i = f->imp; i != NULL; i = i->isucc ) {
    if ( !IsExport( f->F_BODY, i->iport ) )
      continue;

    if ( IsArithmetic( i->info ) )
      continue;

    FPRINTF( hyfd, "      %s ", GetHybridTypeProlog( i ) );
    PrintHTemp( i );
    FPRINTF( hyfd, "(*)\n" );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( IsArithmetic( i->info ) )
      continue;

    FPRINTF( hyfd, "      %s ", GetHybridTypeProlog( i ) );
    PrintHTemp( i );
    FPRINTF( hyfd, "(*)\n" );
    }

  if ( !(f->vmark || f->smark) )
    FPRINTF( hyfd, "\n      iii = 0\n" );

  if ( f->vmark && cRay )
    FPRINTF( hyfd, "CDIR$ IVDEP" );

  FPRINTF( hyfd, "\n      do 100 " );
  PrintHTemp( crod );
  FPRINTF( hyfd, " = lo,hi\n" );

  PrintHybridGraph( f, f->F_BODY, crod );

  if ( !(f->vmark || f->smark) )
    FPRINTF( hyfd, "       iii = iii + 1\n" );

  FPRINTF( hyfd, " 100  continue\n\n" );


  FPRINTF( hyfd, "      return\n" );
  FPRINTF( hyfd, "      end\n" );
}

static int IsHybridGraph( g, outer )
PNODE g;
int   outer;
{
  register PNODE n;
  register PEDGE i;
  register PNODE f;
  register int   c;

  for ( c = 0, n = g->G_NODES; n != NULL; n = n->nsucc ) {
    switch ( n->type ) {
      case IFAssign:
        if ( !IsArithmetic( n->imp->info ) )
	  return( FALSE );

        break;

      case IFGreat:
      case IFGreatEqual:
      case IFLess:
      case IFLessEqual:
      case IFEqual:
      case IFNotEqual:
        if ( !IsArithmetic( n->imp->info ) )
	  return( FALSE );

	if ( n->exp != NULL )
	  return( FALSE );

        c++;
        break;
  
      case IFSelect:
        if ( n->usucc == NULL )
          return( FALSE );
  
        switch ( n->usucc->type ) {
          case IFGreat:
          case IFGreatEqual:
          case IFLess:
          case IFLessEqual:
          case IFEqual:
          case IFNotEqual:
	    break;
  
          default:
	    return( FALSE );
          }
  
	for ( i = n->S_ALT->imp; i != NULL; i = i->isucc )
          if ( !IsArithmetic( i->info ) )
            if ( !IsArithmeticPtr( i->info ) )
	      return( FALSE );

	for ( i = n->S_CONS->imp; i != NULL; i = i->isucc )
          if ( !IsArithmetic( i->info ) )
            if ( !IsArithmeticPtr( i->info ) )
	      return( FALSE );

	for ( i = n->imp; i != NULL; i = i->isucc )
          if ( !IsArithmetic( i->info ) )
            if ( !IsArithmeticPtr( i->info ) )
	      return( FALSE );

        if ( IsHybridGraph( n->S_ALT, FALSE ) )
	  if ( IsHybridGraph( n->S_CONS, FALSE ) ) {
	    c++;
	    break;
	    }
  
        return( FALSE );

      case IFCall:
	if ( n->exp == NULL )
	  return( FALSE );

	if ( (f = FindFunction( n->imp->CoNsT )) == NULL )
	  Error2( "IsHybridGraph", "FindFunction FAILED!!!" );

	if ( !GenIsIntrinsic( f ) )
	  return( FALSE );

	if ( strcmp( n->imp->CoNsT, "sin" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "cos" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "tan" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "asin" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "acos" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "atan" ) == 0 )
	  goto DoIntrinsic;
	if ( strcmp( n->imp->CoNsT, "sqrt" ) == 0 )
	  goto DoIntrinsic;

        if ( strcmp( n->imp->CoNsT, "atan2" ) == 0 )
          if (  (IsReal( n->imp->isucc->info ) ||
               IsDouble( n->imp->isucc->info )) &&
                (IsReal( n->imp->isucc->isucc->info ) ||
               IsDouble( n->imp->isucc->isucc->info )) )
            break;
          
        return( FALSE );
DoIntrinsic:
        if ( !(IsReal( n->imp->isucc->info ) || 
	       IsDouble( n->imp->isucc->info )) )
	  return( FALSE );

	break;

      case IFSum:
      case IFSingle:
      case IFDouble:
      case IFTrunc:
      case IFNeg:
      case IFAStore:
      case IFLeast:
      case IFGreatest:
      case IFProduct:
        if ( !IsArithmetic( n->imp->info ) )
	  return( FALSE );

	break;

      case IFPlus:
      case IFMinus:
      case IFTimes:
	if ( n->imp->info->type != IF_INTEGER )
	  c++;

	if ( !IsArithmetic( n->imp->info ) )
	  return( FALSE );

	break;

      case IFMod:
      case IFMin:
      case IFMax:
      case IFAbs:
      case IFDiv:
	if ( !IsArithmetic( n->imp->info ) )
	  return( FALSE );

	c++;
	break;

      case IFAIndexPlus:
      case IFAIndexMinus:
	break;

      case IFOptAElement:
	if ( !IsArithmeticPtr( n->imp->info ) )
	  return( FALSE );

	break;

      default:
	return( FALSE );
      }
    }

  if ( outer )
    if ( c >= GENF_NODE_THRESHOLD )
      return( TRUE );

  return( FALSE );
}


static int IsHybridCandidate( f )
PNODE f;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE i;
  register int   c;
  register PTEMP t;

  if ( f->F_GEN->imp->isucc != NULL )
    return( FALSE );
  if ( !IsRangeGenerate( f->F_GEN->imp->src ) )
    return( FALSE );

  for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) {
    if ( n->gsucc == NULL )
      return( FALSE );

    switch ( n->gsucc->type ) {
      case IFAStore:
      case IFLeast:
      case IFGreatest:
      case IFProduct:
      case IFSum:
	break;

      default:
        return( FALSE );
      }
    }

  for ( e = f->F_BODY->exp; e != NULL; e = e->esucc ) 
    if ( !IsArithmetic( e->info ) )
      if ( !IsArithmeticPtr( e->info ) )
	return( FALSE );

  for ( c = 0, i = f->imp; i != NULL; i = i->isucc ) {

    if ( IsConst( i ) )
      return( FALSE );

    /* IS IT A FRAME REFERENCE??? IF SO, ABORT!!! */
    if ( IsExport( f->F_BODY, i->iport ) ) {
      c++;

      if ( (t = i->temp) == NULL )
        return( FALSE );

      if ( IsFunction( t->info ) )
        return( FALSE );
      }
    }

  if ( c > GENF_IMP_THRESHOLD )
    return( FALSE );

  for ( i = f->F_BODY->imp; i != NULL; i = i->isucc ) 
    if ( !IsArithmetic( i->info ) )
      if ( !IsArithmeticPtr( i->info ) )
	return( FALSE );

  if ( !IsHybridGraph( f->F_BODY, TRUE ) )
    return( FALSE );

  return( TRUE );
}

/**************************************************************************/
/* GLOBAL **************        PrintForall        ************************/
/**************************************************************************/
/* PURPOSE: PRINT Forall NODE f TO output. THIS ROUTINE HANDLES BOTH      */
/*          SLICED AND NONSLICED FORMS.                                   */
/**************************************************************************/

void PrintForall( indent, f )
int   indent;
PNODE f;
{
    PrintProducerModifiers( indent, f->F_GEN );
    PrintGenInit( indent, f );

    PrintReturnInit( indent, f->F_RET ); /* ONCE WAS THE FIRST CALL */

    if ( hybrid != NULL )
      if ( IsHybridCandidate( f ) ) {
        PrintHybrid( f );

	if ( info )
          FPRINTF( stderr, "Hybrid Loop: [%s,%s,%d]\n", 
		   f->funct, f->file, f->line );

	goto MoveOn;
	}

    PrintGenControl( indent, f );
    PrintProducerModifiers( indent + 2, f->F_BODY );

    if ( f->vmark && alliantfx )
      PrintVecBody( indent + 2, f );
    else
      PrintGraph( indent + 2, f->F_BODY );

    if ( !(f->vmark && alliantfx) ) {
      PrintProducerModifiers( indent + 2, f->F_RET );
      PrintReturnUpd( indent + 2, f->F_RET );
      PrintConsumerModifiers( indent + 2, f->F_RET );
      PrintConsumerModifiers( indent + 2, f->F_BODY );
      PrintConsumerModifiers( indent + 2, f->F_GEN  );
      PrintGenUpd( indent + 2, f );
      }

    PrintIndentation( indent + 2 );
    FPRINTF( output, "}\n" );

MoveOn:
    PrintReturnRapUp( indent, f->F_RET );
    PrintGenRapUp( indent, f );
    PrintConsumerModifiers( indent, f );
}


/**************************************************************************/
/* GLOBAL **************         PrintLoop         ************************/
/**************************************************************************/
/* PURPOSE: PRINT LOOP NODE l (LoopA OR LoopB, TASK OR NON-TASK) TO       */
/*          output.                                                       */
/**************************************************************************/

void PrintLoop( indent, l )
int   indent;
PNODE l;
{
    register PEDGE i;
    register PEDGE ii;

    PrintProducerModifiers( indent, l->L_INIT );
    PrintReturnInit( indent, l->L_RET );
    PrintGraph( indent, l->L_INIT );

    /* PRINT RETURN UPDATE FOR IMPLICT FIRST ITERATION */
    PrintProducerModifiers( indent + 2, l->L_RET );
    PrintReturnUpd( indent + 2, l->L_RET );
    PrintConsumerModifiers( indent + 2, l->L_RET );

    /* PRINT LOOP CONTROL HEADER */
    PrintIndentation( indent );

    if ( IsLoopB( l ) ) {
	if ( AreAllUnitFanout( l->L_TEST ) ) {
          FPRINTF( output, "for ( ; " );
	  PrintLoopTest( l->L_TEST->imp );
          FPRINTF( output, "; ) {\n" );
	} else {
            FPRINTF( output, "for ( ;; ) {\n" );
	    PrintGraph( indent + 2, l->L_TEST );
	    PrintIndentation( indent + 2 );
	    FPRINTF( output, "if ( !" );
	    PrintTemp( l->L_TEST->imp );
	    FPRINTF( output, " ) break;\n" );
	    }
	}
    else
	FPRINTF( output, "do {\n" );

    /* PRINT LOOP BODY */
    PrintProducerModifiers( indent + 2, l->L_BODY );
    PrintGraph( indent + 2, l->L_BODY );

    /* UPDATE LOOP CARRIED TEMPORARIES: FIRST DEALLOCATING OLD AGGREGATES */

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc ) {
        if ( IsBasic( i->info ) )
	    continue;

	if ( i->cm != -1 && (!(i->dmark)) )
	    continue;

	if ( i->iport == 0 )
	    PrintFreeCall( indent+2, i );
	else
	    PrintFreeCall( indent+2, FindImport( l->L_INIT, i->iport ) );
	}

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

        if ( IsConst( i ) )
	    continue;

        if ( (ii = FindImport( l->L_INIT, i->iport )) == NULL )     /* T */
	    continue;

	if ( ii->temp == i->temp )
	    continue;

	if ( !IsSGraph( i->src ) )
	    continue;

	PrintAssgn( indent + 2, ii, i );
	}

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

        if ( IsConst( i ) )
	    continue;

	if ( IsSGraph( i->src ) )
	    continue;

        if ( (ii = FindImport( l->L_INIT, i->iport )) == NULL )     /* T */
	    continue;

	if ( ii->temp == i->temp )
	    continue;

	PrintAssgn( indent+2, ii, i );
	}

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc )
	if ( IsConst( i ) )
	    PrintAssgn( indent+2, FindImport( l->L_INIT, i->iport ), i );

    /* PRINT RETURN UPDATE FOR BODY EXECUTION */
    PrintProducerModifiers( indent+2, l->L_RET );
    PrintReturnUpd( indent+2, l->L_RET );
    PrintConsumerModifiers( indent+2, l->L_RET );

    /* PRINT LOOP CONTINUATION TEST FOR LoopA NODES */
    if ( IsLoopA( l ) ) {
	if ( AreAllUnitFanout( l->L_TEST ) ) {
	    PrintIndentation( indent );
	    FPRINTF( output, "} while ( " );
	    PrintLoopTest( l->L_TEST->imp );
	    FPRINTF( output, " );\n" );
	} else {
	    PrintGraph( indent+2, l->L_TEST );
	    PrintIndentation( indent );
	    FPRINTF( output, "} while ( " );
	    PrintTemp( l->L_TEST->imp );
	    FPRINTF( output, " );\n" );
	    }
    } else {
      PrintIndentation( indent+2 );
      FPRINTF( output, "}\n" );
      }

    PrintProducerLastModifiers( indent, l->L_RET );
    PrintReturnRapUp( indent, l->L_RET );

    if ( freeall )
      goto DoTheFrees;

    /* SKIP DEALLOCATIONS IF THEY ARE THE LAST THINGS DONE IN THE PROGRAM */
    if ( sequential && standalone ) {
      register PNODE nn;

      /* IF COMPLEX LIES AHEAD THEN FREE THE STORAGE */
      for ( nn = l->nsucc; nn != NULL; nn = nn->nsucc )
	if ( IsCompound( nn ) || IsCall( nn ) )
	  break;

      if ( nn == NULL ) {
	register PEDGE ee;
	register PEDGE iii;

	for ( ee = l->exp; ee != NULL; ee = ee->esucc )
	  if ( ee->dst != NULL )
	    if ( IsXGraph( ee->dst ) ) {
	      for ( iii = l->L_INIT->imp; iii != NULL; iii = iii->isucc )
		if ( iii->cm == -1 )
		  rmcmcnt++;

	      goto LastThing;
	      }
	}
      }
      
DoTheFrees:
    PrintConsumerModifiers( indent, l->L_INIT );
    PrintConsumerModifiers( indent, l );

LastThing:
    return;
}


/**************************************************************************/
/* LOCAL  **************       GetTypePrefix       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE PREFIX CHARACTER ASSOCIATED WITH TYPE i.           */
/**************************************************************************/

static char GetTypePrefix( i )
PINFO i;
{
  switch( i->type ) {
    case IF_DOUBLE:
      return( 'd' );

    case IF_REAL:
      return( 'r' );

    case IF_INTEGER:
      return( 'i' );

    default:
      FPRINTF( stderr, "HELP type = %d\n", i->type );
      Error2( "GetTypePrefix", "ILLEGAL TYPE" );
    }

  return '\0';
}


void PrintFirstSum( indent, l )
int   indent;
PNODE l;
{
             char   fn[100];
    register PEDGE  x;
    register PEDGE  y;
    register PEDGE  lo;
    register PEDGE  hi;
    register PEDGE  iv;

    PrintReturnInit( indent, l->L_RET );

    iv = FindImport( l->L_INIT, l->L_RET->imp->src->imp->isucc->eport );

    if ( (lo = iv->isucc) == NULL )
      lo = l->L_INIT->imp;
   
    if ( !IsConst( iv ) )
      iv = FindImport( l, iv->eport );

    if ( !IsConst( lo ) )
      lo = FindImport( l, lo->eport );

    hi = l->L_TEST->imp->src->imp->isucc;

    if ( !IsConst( hi ) )
      hi = FindImport( l, hi->eport );

    for ( y = l->L_BODY->exp; y != NULL; y = y->esucc )
      if ( IsArrayBase( y->info ) )
	break;

    y = FindImport( l, y->eport );

    x = l->L_RET->imp;

    SPRINTF( fn, "%cfsum", GetTypePrefix( x->src->imp->isucc->info ) );

    PrintIndentation( indent );
    FPRINTF( output, "FirstSum( " );
    PrintTemp( x ); 
    FPRINTF( output, ", " );
    PrintTemp( y ); 
    FPRINTF( output, ", " );
    PrintTemp( lo ); 
    FPRINTF( output, ", " );
    PrintTemp( hi ); 
    FPRINTF( output, ", " );
    PrintTemp( iv ); 
    FPRINTF( output, ", %s, %s );\n",
             l->L_RET->imp->src->imp->isucc->info->tname, 
	     BindInterfaceName( fn, FOR_FORTRAN, 's' ) );

    PrintReturnRapUp( indent, l->L_RET );
    PrintConsumerModifiers( indent, l );
}


void PrintTri( indent, l )
int   indent;
PNODE l;
{
             char  fn[100];
    register PEDGE x;
    register PEDGE y;
    register PEDGE z;
    register PEDGE e;
    register PEDGE lo;
    register PEDGE hi;
    register PEDGE iv;

    PrintReturnInit( indent, l->L_RET );

    iv = FindImport( l->L_INIT, l->L_RET->imp->src->imp->isucc->eport );

    if ( (lo = iv->isucc) == NULL )
      lo = l->L_INIT->imp;
   
    if ( !IsConst( iv ) )
      iv = FindImport( l, iv->eport );

    if ( !IsConst( lo ) )
      lo = FindImport( l, lo->eport );

    hi = l->L_TEST->imp->src->imp->isucc;

    if ( !IsConst( hi ) )
      hi = FindImport( l, hi->eport );

    z = y = NULL;
    for ( e = l->L_BODY->exp; e != NULL; e = e->esucc )
      if ( IsArrayBase( e->info ) ) {
	if ( e->dst->exp->dst->type == IFTimes ) {
	  z = e;
	  continue;
	  }

	y = e;
	continue;
	}

    if ( z == NULL ) z = y;
    if ( y == NULL ) y = z;

    y = FindImport( l, y->eport );
    z = FindImport( l, z->eport );

    x = l->L_RET->imp;

    SPRINTF( fn, "%ctri", GetTypePrefix( x->src->imp->isucc->info ) );

    PrintIndentation( indent );
    FPRINTF( output, "TrI( " );
    PrintTemp( x ); 
    FPRINTF( output, ", " );
    PrintTemp( y ); 
    FPRINTF( output, ", " );
    PrintTemp( z );
    FPRINTF( output, ", " );
    PrintTemp( lo ); 
    FPRINTF( output, ", " );
    PrintTemp( hi ); 
    FPRINTF( output, ", " );
    PrintTemp( iv ); 
    FPRINTF( output, ", %s, %s );\n", 
	     l->L_RET->imp->src->imp->isucc->info->tname, 
	     BindInterfaceName( fn, FOR_FORTRAN, 's' ) );

    PrintReturnRapUp( indent, l->L_RET );
    PrintConsumerModifiers( indent, l );
}


void PrintVMinMax( indent, l, root )
int    indent;
PNODE  l;
char  *root;
{
             char  fn[100];
    register PEDGE x;
    register PEDGE y;
    register PEDGE lo;
    register PEDGE hi;
    register PEDGE iv;

    PrintReturnInit( indent, l->L_RET );

    iv = FindImport( l->L_INIT, l->L_RET->imp->src->imp->eport );

    if ( (lo = iv->isucc) == NULL )
      lo = l->L_INIT->imp;
   
    if ( !IsConst( iv ) )
      iv = FindImport( l, iv->eport );

    if ( !IsConst( lo ) )
      lo = FindImport( l, lo->eport );

    hi = l->L_TEST->imp->src->imp->isucc;

    if ( !IsConst( hi ) )
      hi = FindImport( l, hi->eport );

    for ( x = l->L_BODY->exp; x != NULL; x = x->esucc )
      if ( IsArrayBase( x->info ) )
	  break;

    y = l->L_RET->imp;

    SPRINTF( fn, "%c%s", GetTypePrefix( x->dst->exp->info ), root );

    PrintIndentation( indent );
    FPRINTF( output, "FirstMinMax( " );
    PrintTemp( y ); 
    FPRINTF( output, ", " );
    PrintTemp( FindImport( l, x->eport ) ); 
    FPRINTF( output, ", " );
    PrintTemp( lo ); 
    FPRINTF( output, ", " );
    PrintTemp( hi ); 
    FPRINTF( output, ", " );
    PrintTemp( iv ); 
    FPRINTF( output, ", %s, %s );\n", x->dst->exp->info->tname, 
	     BindInterfaceName( fn, FOR_FORTRAN, 's' )     );

    PrintReturnRapUp( indent, l->L_RET );
    PrintConsumerModifiers( indent, l );
}

int
BasicTypeSize(i)
PINFO i;
{
        switch(i->type)
        {
                case IF_DOUBLE:
                        return(sizeof(double));
                case IF_CHAR:
                        return(sizeof(char));
                case IF_REAL:
                        return(sizeof(float));
                case IF_ARRAY:
/*
                        return(BasicTypeSize(i->A_ELEM));
*/
                default:
                        return(sizeof(int *));
        }
}

#define BIGINT 0xFFFFFFFF

int
LCMSize(g)
PNODE g;
{
#if !defined(CACHE_LINE)
	return(0);
#else
        PEDGE e;
        unsigned int sizes[255];
        int i;
        int last;
        unsigned int min = BIGINT;

	for(i=0; i < 255; i++)
		sizes[i] = BIGINT;
        i = 0;
        for(e = g->imp; e != NULL; e = e->isucc)
        {
                if(e->info->type == IF_ARRAY)
                {
                        sizes[i] = BasicTypeSize(e->info->A_ELEM);
                	if(min > sizes[i])
                       		min = sizes[i];
                	i++;
                }
        }

        last = i;

        for(i=0; i < last; i++)
        {
                if((CACHE_LINE % sizes[i]) != 0)
                        return(-1);
        }

	if(min != BIGINT)
		return(min);
	else
        	return(0);
#endif
}

