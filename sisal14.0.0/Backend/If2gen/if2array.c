/* if2array.c,v
 * Revision 12.7  1992/11/04  22:05:00  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:59  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      GetSisalInfo         ************************/
/**************************************************************************/
/* PURPOSE: RETURN SISAL SOURCE INFORMATION FOR NODE n IN BUFFER buf. Buf */
/*          IS TAKEN AS AN ARGUMENT AND RETURNED AS A RESULT.             */
/**************************************************************************/

char *GetSisalInfo( n, buf )
PNODE  n;
char  *buf;
{
  if ( n->line <= 0 ) {
    SPRINTF( buf, "\"%s,%s,line=lost\"",
	     (n->file == NULL)? "unknown.sis" : n->file,
	     (n->funct == NULL)? "unknown(...)" : n->funct );
	   
    return( buf );
    }

  SPRINTF( buf, "\"%s,%s,line=%d\"",
	   (n->file == NULL)? "unknown.sis" : n->file,
	   (n->funct == NULL)? "unknown(...)" : n->funct, n->line );

  return( buf );
}


/**************************************************************************/
/* GLOBAL ***********      GetSisalInfoOnEdge        **********************/
/**************************************************************************/
/* PURPOSE: RETURN SISAL SOURCE INFORMATION FOR NODE n IN BUFFER buf. Buf */
/*          IS TAKEN AS AN ARGUMENT AND RETURNED AS A RESULT.             */
/**************************************************************************/

char *GetSisalInfoOnEdge( e, buf )
PEDGE e;
char  *buf;
{
  if ( e->line > 0 ) 
    {
      SPRINTF( buf, "\"%s,%s,line=%d\"",
	      (e->file == NULL)? "unknown.sis" : e->file,
	      (e->funct == NULL)? "unknown(...)" : e->funct, e->line );
      return( buf );
    }

  else if (e->dst->line > 0)
    {
      SPRINTF( buf, "\"%s,%s,line=%d\"",
	      (e->dst->file == NULL)? "unknown.sis" : e->dst->file,
	      (e->dst->funct == NULL)? "unknown(...)" : e->dst->funct, 
	      e->dst->line );
      return( buf );
    }

  else if (e->src->line > 0)
    {
      SPRINTF( buf, "\"%s,%s,line=%d\"",
	      (e->src->file == NULL)? "unknown.sis" : e->src->file,
	      (e->src->funct == NULL)? "unknown(...)" : e->src->funct, 
	      e->src->line );
      return( buf );
    }
  
  else
    {
      SPRINTF( buf, "\"%s,%s,line=lost\"",
	      (e->file == NULL)? "unknown.sis" : e->file,
	      (e->funct == NULL)? "unknown(...)" : e->funct );
      return( buf );
    }
}


/**************************************************************************/
/* GLOBAL **************      PrintBoundsCheck     ************************/
/**************************************************************************/
/* PURPOSE: PRINT A BOUNDS CHECK FOR ARRAY a AND INDEX i FOR NODE n.      */
/**************************************************************************/

void PrintBoundsCheck( indent, n, a, i )
int   indent;
PNODE n;
PEDGE a;
PEDGE i;
{
  char buf[200];

  if ( sdbx ) {
    FPRINTF( output, "SdbxBoundsCheck%d( ", (i != NULL)? 1 : 2 );
    PrintTemp( a );

    if ( i != NULL ) {
      FPRINTF( output, ", " );
      PrintTemp( i );
      }

    FPRINTF( output, ");\n" );
    return;
    }

  FPRINTF( output, "BoundsCheck%d( ", (i != NULL)? 1 : 2 );
	   
  PrintTemp( a );

  FPRINTF( output, ", \"%s\"", (a->name == NULL)? "lost" : a->name );

  if ( i != NULL ) {
    FPRINTF( output, ", \"%s\", ", (i->name == NULL)? "lost" : i->name );
    PrintTemp( i );
    }

  FPRINTF( output, ", %s", GetSisalInfo( n, buf ) );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintRagged         ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY MEMORY RAGGED MANAGEMENT NODE n TO output.        */
/**************************************************************************/

void PrintRagged( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );

  FPRINTF( output, "RaggedManager( rag%d, ", n->imp->iport );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************     PrintPSMemAllocDVI    ************************/
/**************************************************************************/
/* PURPOSE: PRINT POINTER SWAP MemAllocDVI NODE n TO output.              */
/**************************************************************************/

void PrintPSMemAllocDVI( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );
  FPRINTF( output, "PSMAllocDVI( " );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc->isucc );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintPSScatter      ************************/
/**************************************************************************/
/* PURPOSE: PRINT POINTER SWAP SCATTER NODE n TO output.                  */
/**************************************************************************/

void PrintPSScatter( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );
  FPRINTF( output,  "PSScatter( " );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc->isucc );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintPSManager      ************************/
/**************************************************************************/
/* PURPOSE: PRINT POINTER SWAP MANAGER NODE n OF TYPE nm TO output.       */
/**************************************************************************/

void PrintPSManager( indent, n, nm )
int    indent;
PNODE  n;
char  *nm;
{
  PrintIndentation( indent );
  FPRINTF( output, "PSManager%s( ", nm );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintPSFree         ************************/
/**************************************************************************/
/* PURPOSE: PRINT POINTER SWAP FREE NODE n OF TYPE nm TO output.          */
/**************************************************************************/

void PrintPSFree( indent, n, nm, g )
int    indent;
PNODE  n;
char  *nm;
PNODE  g;
{
  register PNODE nn;

  if ( !freeall ) {
    /* SKIP DEALLOCATIONS IF THEY ARE THE LAST THINGS DONE IN THE PROGRAM */
    if ( sequential && standalone && IsXGraph( g ) ) {
      /* IF COMPLEX LIES AHEAD THEN FREE THE STORAGE */
      for ( nn = n->nsucc; nn != NULL; nn = nn->nsucc )
	if ( IsCompound( nn ) || IsCall( nn ) )
	  break;

      if ( nn == NULL )
	return;
      }
    }

  PrintIndentation( indent );

  if ( n->imp->isucc == NULL ) {
    FPRINTF( output, "PSFree%s( ", nm );
    PrintTemp( n->imp );
    FPRINTF( output, " );\n" );
    return;
    }

  FPRINTF( output, "PSFree%sX( ", nm );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", %s", GetFreeName( n->imp->isucc->info ) );
  FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintPSAlloc        ************************/
/**************************************************************************/
/* PURPOSE: PRINT POINTER SWAP ALLOCATION NODE n OF TYPE nm TO output.    */
/**************************************************************************/

void PrintPSAlloc( indent, n, nm )
int    indent;
PNODE  n;
char  *nm;
{
  register int   c;
  register PEDGE i;

  PrintIndentation( indent );

  if (n->norm > 0) {
  	FPRINTF( output, "SkiPSMAlloc%s( %d, %s, %s, ", nm, n->norm, n->gname, 
	   	n->usucc->exp->info->A_ELEM->A_ELEM->tname );
	FPRINTF(stderr, "SKI PS NORM %d\n", n->norm);
  } else
  	FPRINTF( output, "PSMAlloc%s( %s, %s, ", nm, n->gname, 
	   n->usucc->exp->info->A_ELEM->A_ELEM->tname );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", " );

  for ( c = 2, i = n->imp->isucc->isucc; i != NULL; i = i->isucc, c++ ) {
    PrintTemp( i );
    FPRINTF( output, ", " );
    }

  for ( ; c != 6; c++ )
    FPRINTF( output, "0, " );

  FPRINTF( output, " %d );\n", n->imp->iport );
}


/**************************************************************************/
/* GLOBAL **************       PrintMemAlloc       ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY MEMORY ALLOCATION NODE n TO output.               */
/**************************************************************************/

void PrintMemAlloc( indent, n )
int   indent;
PNODE n;
{
  register PEDGE se;

  PrintIndentation( indent );

  if ( !(n->wmark) && n->norm > 0 ) {
    FPRINTF ( output, "Ski");
    FPRINTF (stderr, "Ski normalization by %d\n", n->norm);
  }

  if ( n->wmark )
    FPRINTF( output, "Ragged" );

  if ( n->type == IFMemAllocDV )
    FPRINTF( output, "MAllocDV( " );
  else if ( n->type == IFMemAllocDVI )
    FPRINTF( output, "MAllocDVI( " );
  else
    FPRINTF( output, "MAlloc( " );

  if ( !(n->wmark) && n->norm > 0 )
  	FPRINTF( output, "%d,", n->norm );

  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", %s", n->exp->info->A_ELEM->A_ELEM->tname );

  if ( n->type == IFMemAllocDVI ) {
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc );
    FPRINTF( output, ", %d", n->exp->sr );

    se = n->imp->isucc->isucc;
    }
  else
    se = n->imp->isucc;

  if ( n->wmark ) {
    if ( se == NULL )
      Error2( "PrintMemAlloc", "se IS NULL" );

    FPRINTF( output, ", " );
    PrintTemp( se );
    }

  FPRINTF( output, " )\n" );
}


/**************************************************************************/
/* GLOBAL **************      PrintGABase          ************************/
/**************************************************************************/
/* PURPOSE: PRINT GetArrayBase NODE n TO output.                          */
/**************************************************************************/

void PrintGABase( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );

  FPRINTF( output, "GABase( %s, ", n->exp->info->tname );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, " );\n" );

  /* SHOULD NOT BE NEEDED! */
  PrintProducerModifiers( indent, n );
  PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************    PrintOptAElement       ************************/
/**************************************************************************/
/* PURPOSE: PRINT OptAElement NODE n TO output.                           */
/**************************************************************************/

void PrintOptAElement( indent, n )
int   indent;
PNODE n;
{
    PrintIndentation( indent );

    FPRINTF( output, "%s( %s, ", GetMacro( n ), n->exp->info->tname );
    PrintTemp( n->exp );
    FPRINTF( output, ", " );
    PrintTemp( n->imp );
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc );
    FPRINTF( output, " );\n" );

    PrintProducerModifiers( indent, n );
    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************      PrintArrayMacro      ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY MACRO REPRESENTING NODE n TO output.  THE MACRO   */
/*          NAME IS pre || macro || component type.  THE ARGUMENT LIST    */
/*          HAS FORMAT ( n->exp, n->imps... ). NODE n CAN EXPORT EITHER   */
/*          AN ARRAY OR BUFFER REPRESENTING AN ARRAY.                     */
/**************************************************************************/

void PrintArrayMacro( indent, pre, macro, n )
int    indent;
char  *pre;
char  *macro;
PNODE  n;
{
    register PEDGE i;

    PrintIndentation( indent );

    if ( IsBuffer( n->imp->info ) )
        FPRINTF( output, "%s%s( %s, ", pre, macro, 
		 n->exp->info->A_ELEM->A_ELEM->tname );
    else
        FPRINTF( output, "%s%s( %s, ", pre, macro, n->imp->info->A_ELEM->tname);

    PrintTemp( n->exp );

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	FPRINTF( output, ", " );
	PrintTemp( i );
	}
	
    FPRINTF( output, " );\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintAReplace       ************************/
/**************************************************************************/
/* PURPOSE: PRINT AReplace NODE n TO OUTPUT. IF nmark IS FALSE THEN AN    */
/*          ATTEMPT IS MADE TO DEALLOCATE THE REPLACED COMPONENTS.        */
/**************************************************************************/

void PrintAReplace( indent, n )
int   indent;
PNODE n;
{
    register PEDGE i;
    register PINFO ci;
    register int   c;
    register PEDGE ab;
    register int   opt = FALSE;
	     char  buf[100];

    if ( n->exp->temp != n->imp->temp )
	PrintAssgn( indent, n->exp, n->imp );

    PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE );

    ci = n->exp->info->A_ELEM;
    ab = FindImport( n, MAX_PORT_NUMBER );

    if ( (ab != NULL) && aimp )
        if ( (ab->rc == 0) && (UsageCount( ab->src, ab->eport ) == 1) )
	    opt = TRUE;

    if ( (!IsBasic( ci )) && (!n->nmark) )
	SPRINTF( buf, "DARepl( %s, ", GetFreeName( ci ) );
    else
	SPRINTF( buf, "ARepl( " );

    for ( c = 0, i = n->imp->isucc->isucc; i != NULL; c++ ) {
	if ( (c > 0) && opt ) {
	    PrintIndentation( indent );
	    FPRINTF( output, "IncArrayBase( %s, ", ci->tname );
	    PrintTemp( ab );
	    FPRINTF( output, " );\n" );
	    }

	PrintIndentation( indent );

	FPRINTF( output, "%s%s %s, ", (ab != NULL)? "Opt" : "", buf, ci->tname);
	PrintTemp( n->exp );
	FPRINTF( output, ", " );
	PrintTemp( n->imp->isucc );

	if ( !opt && (c > 0) )
	    FPRINTF( output, " + %d", c );

	FPRINTF( output, ", " );
        PrintTemp( i );

	if ( ab != NULL ) {
	    FPRINTF( output, ", " );
	    PrintTemp( ab );
	    }

        FPRINTF( output, " );\n" );

	if ( (i = i->isucc) == ab )
	    break;
	}
}


/**************************************************************************/
/* GLOBAL **************       PrintABuild         ************************/
/**************************************************************************/
/* PURPOSE: PRINT ABuild NODE n TO output.                                */
/**************************************************************************/

void PrintABuild( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  PrintIndentation( indent );

  FPRINTF( output, "DoABld( " );
  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", %d );\n", n->exp->sr );

  for ( i = n->imp->isucc; i != NULL; i = i->isucc ) {
    PrintIndentation( indent );

    FPRINTF( output, "GathUpd( %s, ", n->exp->info->A_ELEM->tname );
    PrintTemp( n->exp );
    FPRINTF( output, ", " );
    PrintTemp( i );
    FPRINTF( output, " );\n" );
    }
}


/**************************************************************************/
/* GLOBAL **************       PrintABuildAT       ************************/
/**************************************************************************/
/* PURPOSE: PRINT ABuildAT NODE n TO output.                              */
/**************************************************************************/

void PrintABuildAT( indent, n )
int   indent;
PNODE n;
{
    register PEDGE i;
    register PEDGE b;
    register int   c;
    register int   s;

    if ( n->cmark ) {
	PrintIndentation( indent );
	FPRINTF( output, "%s( ", GetIncRefCountName( n->exp->info ) );
	PrintTemp( n->exp );
	FPRINTF( output, ", %s, %d );\n", n->exp->info->sname, 1 );
	return;
	}

    /* FIND THE BUFFER IMPORT AND THE NUMBER OF INITIAL ELEMENTS */
    for ( s = 0, i = n->imp->isucc; i != NULL; i = i->isucc )
	if ( i->isucc != NULL ) 
	  s++; 
	else 
	  b = i;


    /* PART OF VECTORIZED ReduceAT COMPUTATION? */
    if ( n->nmark && IsSGraph( b->src ) )
      if ( b->src->G_DAD->vmark ) {
        for ( c = 0, i = n->imp->isucc; i != b; i = i->isucc, c++ ) {
          PrintIndentation( indent );
	  FPRINTF( output, "((%s*)", n->exp->info->A_ELEM->tname );
	  PrintTemp( b );
	  FPRINTF( output, ")[%d] = ", c );
	  PrintTemp( i );
	  FPRINTF( output, ";\n" );
	  }

	return;
	}

    PrintIndentation( indent );

    switch ( n->type ) {
      case IFABuildATDVI:
        FPRINTF( output, "BldATDVI( %s, ", n->exp->info->A_ELEM->tname );
	break;

      case IFABuildATDV:
        FPRINTF( output, "BldATDV( %s, ", n->exp->info->A_ELEM->tname );
	break;

      default:
        FPRINTF( output, "%s%s( %s, ", (n->nmark)? "Opt" : "", GetMacro( n ),  
	         n->exp->info->A_ELEM->tname );
        break;
      }
	     
    PrintTemp( n->exp );
    FPRINTF( output, ", " );
    PrintTemp( n->imp );

    FPRINTF( output, ", %d, ", s );
    PrintTemp( b );
    FPRINTF( output, " );\n" );

    if ( n->imp->isucc != b ) {
      PrintIndentation( indent );
      FPRINTF( output, "{\n" );

      PrintIndentation( indent );
      FPRINTF( output, "  register %s *pp;\n", n->exp->info->A_ELEM->tname );

      PrintIndentation( indent );
      FPRINTF( output, "  pp = (%s*) ", n->exp->info->A_ELEM->tname );
      PrintTemp( b );
      FPRINTF( output, "->Base;\n" );

      for ( c = 0, i = n->imp->isucc; i != b; i = i->isucc, c++ ) {
	PrintIndentation( indent );

	FPRINTF( output, "  pp[%d] = ", c );
	PrintTemp( i );
	FPRINTF( output, ";\n" );
	}

      PrintIndentation( indent );
      FPRINTF( output, "}\n" );
      }

    if ( !(n->nmark) )
      PrintSetRefCount( indent, n->exp, n->exp->sr, FALSE );
}


/**************************************************************************/
/* GLOBAL **************       PrintAAddHLAT       ************************/
/**************************************************************************/
/* PURPOSE: PRINT AAddLAT OR AAddHAT NODE n TO output.                    */
/**************************************************************************/

void PrintAAddHLAT( indent, n )
int   indent;
PNODE n;
{
    char *s;
    int   assgnOK;

    assgnOK = TRUE;

    switch ( n->type ) {
      case IFAAddHATDVI:
	s = "HATDVI" ;
	assgnOK = FALSE;
	break;

      case IFAAddHATDV:
	s = "HATDV";
	assgnOK = FALSE;
	break;

      case IFAAddLATDVI:
        s = "LATDVI";
	assgnOK = FALSE;
	break;

      case IFAAddLATDV:
	s = "LATDV";
	assgnOK = FALSE;
	break;

      case IFAAddHAT:
	s = "HAT";
	break;

      case IFAAddLAT:
	s = "LAT";
	break;

      default:
	Error2( "PrintAAddHLAT", "ILLEGAL NODE TYPE" );
      }

    if ( !(n->nmark) )
      if ( n->imp->pmark )
        if ( n->imp->temp != n->exp->temp )
          if ( assgnOK )
	    PrintAssgn( indent, n->exp, n->imp );

    PrintIndentation( indent );

    FPRINTF( output, "%s%sAdd%s( ",
	     (n->nmark)? "Opt" : "", (n->imp->pmark)? "P" : "", s );

    PrintTemp( n->exp );
    FPRINTF( output, ", " );

    if ( !(n->imp->pmark) ) {
      PrintTemp( n->imp );
      FPRINTF( output, ", " );
      }

    PrintTemp( n->imp->isucc );
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc->isucc );
    FPRINTF( output, ", %s, %s, ", GetCopyFunction( n->exp->info ),
	     n->exp->info->A_ELEM->tname                         );
    PrintTemp( n->imp->isucc->isucc->isucc );
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc->isucc->isucc->isucc );
    FPRINTF( output, " );\n" );

    if ( !(n->nmark) ) {
      if ( n->imp->pmark )
        PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE );
      else
        PrintSetRefCount( indent, n->exp, n->exp->sr, FALSE );
      }

    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************        PrintAAddH         ************************/
/**************************************************************************/
/* PURPOSE: PRINT AAddH NODE n TO output.                                 */
/**************************************************************************/

void PrintAAddH( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );

  FPRINTF( output, "AAddH%s( ", (IsBasic( n->exp->info->A_ELEM))? "" : "X" );

  PrintTemp( n->exp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp );
  FPRINTF( output, ", " );
  PrintTemp( n->imp->isucc );
  FPRINTF( output, ", " );

  FPRINTF( output, "%s, %s, %s", GetCopyFunction( n->exp->info ),
	   n->exp->info->A_ELEM->tname, GetFreeName( n->exp->info ) );

  if ( IsBasic(n->exp->info->A_ELEM) )
    FPRINTF( output, " );\n" );
  else
    FPRINTF( output, ", %s );\n", GetFreeName( n->exp->info->A_ELEM ) );

  PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE );
}


/**************************************************************************/
/* GLOBAL **************      PrintACatenateAT     ************************/
/**************************************************************************/
/* PURPOSE: PRINT ACatenateAT NODE n TO output.                           */
/**************************************************************************/

void PrintACatenateAT( indent, n )
int   indent;
PNODE n;
{
    char buf[100];
    char *s;
    int  assgnOK;

    assgnOK = TRUE;

    switch ( n->type ) {
      case IFACatenateATDVI:
	s = "DVI";
	assgnOK = FALSE;
	break;

      case IFACatenateATDV:
	s = "DV";
	assgnOK = FALSE;
	break;

      default:
	s = "";
        break;
      }

    if ( n->imp->pmark ) {
	if ( !(n->nmark) )
	  if ( n->imp->temp != n->exp->temp )
            if ( assgnOK )
	      PrintAssgn( indent, n->exp, n->imp );

	if ( !(n->imp->isucc->pmark) )
	    SPRINTF( buf, "PCCatAT" );
        else
	    SPRINTF( buf, "PPCatAT" );
	}
    else if ( n->imp->isucc->pmark ) {
	if ( !(n->nmark) )
	  if ( n->imp->isucc->temp != n->exp->temp )
            if ( assgnOK )
	      PrintAssgn( indent, n->exp, n->imp->isucc );

	SPRINTF( buf, "CPCatAT" );
        }
    else
	SPRINTF( buf, "CCCatAT" );

    PrintIndentation( indent );

    FPRINTF( output, "%s%s", (n->nmark)? "Opt" : "", buf );

    FPRINTF( output, "%s( ", s );

    PrintTemp( n->exp );
    FPRINTF( output, ", " );

    if ( strcmp( buf, "CCCatAT" ) == 0 ) {
      PrintTemp( n->imp );
      FPRINTF( output, ", " );
      PrintTemp( n->imp->isucc );
      FPRINTF( output, ", " );
    } else {
      if ( strcmp( buf, "PPCatAT" ) != 0 ) {
        if ( strcmp( buf, "CPCatAT" ) == 0 ) {
          PrintTemp( n->imp );
          FPRINTF( output, ", " );
        } else {
          PrintTemp( n->imp->isucc );
          FPRINTF( output, ", " );
          }
	}
      }

    PrintTemp( n->imp->isucc->isucc );
    FPRINTF( output, ", %s, %s, ", GetCopyFunction( n->exp->info ),
	     n->exp->info->A_ELEM->tname                        );
    PrintTemp( n->imp->isucc->isucc->isucc );
    FPRINTF( output, ", " );
    PrintTemp( n->imp->isucc->isucc->isucc->isucc );
    FPRINTF( output, " );\n" );

    if ( !(n->nmark) ) {
      if ( n->imp->pmark || n->imp->isucc->pmark ) {
        PrintSetRefCount( indent, n->exp, n->exp->sr, TRUE  );
	}
      else
        PrintSetRefCount( indent, n->exp, n->exp->sr, FALSE );
      }

    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************       PrintArrayNoOp      ************************/
/**************************************************************************/
/* PURPOSE: PRINT ARRAY NoOp NODE n TO output.                            */
/**************************************************************************/

void PrintArrayNoOp( indent, n )
int   indent;
PNODE n;
{
    register PEDGE i; 

    if ( n->imp->rmark1 == RMARK )
	if ( n->imp->omark1 || n->imp->pmark ) {           /* A TRUE NoOp */
	    if ( n->imp->temp != n->exp->temp )
	        PrintAssgn( indent, n->exp, n->imp );

if ( n->exp != NULL )
  if ( IsAReplace( n->exp->dst ) )
    if ( n->exp->dst->exp->sr == 1 )
      n->exp->dst->exp->sr = 0;

            return;
	    }

    i = n->imp;

    if ( i->pmark ) {
	PrintIndentation( indent );

	FPRINTF( output, "P%sANoOp( ", (i->rmark1 == rMARK)? "C" : "" );
	PrintTemp( n->exp );
	FPRINTF( output, ", " );
	PrintTemp( i );
	FPRINTF( output, ", %s );\n", GetFreeName( i->info ) );

	return;
	}

    PrintIndentation( indent );

    if ( i->rmark1 == rMARK )
	FPRINTF( output, "COANoOp( " );
    else
	FPRINTF( output, "ANoOp( " );

    PrintTemp( n->exp );
    FPRINTF( output, ", " );
    PrintTemp( n->imp );
    FPRINTF( output, ", %s, %s, %s );\n", n->exp->info->A_ELEM->tname,
	     GetCopyFunction( n->exp->info ), GetFreeName( n->exp->info ) );
}
