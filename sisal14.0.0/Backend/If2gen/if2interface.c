/* if2interface.c,v
 * Revision 12.7  1992/11/04  22:05:01  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:00  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int rocnt = 0;          /* COUNT OF READ-ONLY INTERFACE AGGREGATES */
static int acnt  = 0;          /* COUNT OF INTERFACE AGGREGATES           */


/**************************************************************************/
/* GLOBAL **************     WriteInterfaceInfo    ************************/
/**************************************************************************/
/* PURPOSE: WRITE INTERFACE INFORMATION TO stderr.                        */
/**************************************************************************/

void WriteInterfaceInfo()
{
  FPRINTF( infoptr, "\n **** INTERFACE OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr, " Array Input Arguments:           %d\n", acnt  );
  FPRINTF( infoptr, " Array Read-Only Input Arguments: %d\n", rocnt );
}


/**************************************************************************/
/* LOCAL  **************     GetComponentType      ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE COMPONENT TYPE STRUCTURE FOR ARRAY i.              */
/**************************************************************************/

static PINFO GetComponentType( i )
PINFO i;
{
  register PINFO ii;

  for ( ii = i->A_ELEM; ii != NULL; ii = ii->A_ELEM )
    switch ( ii->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
      case IF_CHAR:
        return( ii );

      case IF_ARRAY:
        continue;

      default:
        Error2( "MIXED LANGUAGE PROGRAMMING", 
		"ILLEGAL INTERFACE COMPONENT TYPE" );
      }

  Error2( "GetComponentType", "FOR LOOP FAILURE" );
  return NULL;
}


/**************************************************************************/
/* LOCAL  **************           GetDim          ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE DIMENSIONALITY OF ARRAY i.                         */
/**************************************************************************/

static int GetDim( i )
PINFO i;
{
  register PINFO ii;
  register int   c;

  for ( c = 0, ii = i->A_ELEM; ii != NULL; ii = ii->A_ELEM ) {
    c++;

    switch ( ii->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
      case IF_CHAR:
        return( c );

      case IF_ARRAY:
        break;

      default:
        Error2( "Mixed Language Programming", "ILLEGAL INTERFACE TYPE" );
      }
    }

  Error2( "GetDim", "FOR LOOP FAILURE" );
  return( -1 );
}


/**************************************************************************/
/* GLOBAL **************        GetLanguage        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE SOURCE LANGUAGE OF FUNCTION f.                     */
/**************************************************************************/

int GetLanguage( f )
PNODE f;
{
  if ( IsIGraph( f ) ) {
    if ( f->mark == 'f' ) /* NEW CANN 2/92 */
      return( FOR_FORTRAN );

    if ( f->mark == 'c' ) /* NEW CANN 2/92 */
      return( FOR_C );

    if ( f->mark == 'i' ) /* NEW CANN 2/92 */
      return( FOR_C );

    return( FOR_SISAL );
    }

  return( FOR_SISAL );
}


/**************************************************************************/
/* GLOBAL **************     BindInterfaceName     ************************/
/**************************************************************************/
/* PURPOSE: CONVERT FUNCTION NAME nm TO THE APPROPRIATE FORM FOR LANGUAGE */
/*          lang AND RETURN A COPY.                                       */
/**************************************************************************/


char *BindInterfaceName( nm, lang, mark )
char *nm;
int   lang;
char  mark;
{
  register char *p;
           char buf[100];

  if ( lang == FOR_C )
    p = LowerCase( nm, FALSE, FALSE );
  else if ( lang == FOR_FORTRAN ) {
    if ( Iupper )
      p = UpperCase( nm, IunderL, IunderR ); 
    else
      p = LowerCase( nm, IunderL, IunderR );
    }

  /* else if ( entry_point && (!standalone) ) { */

  /* NEW CANN 2/92: forF or forC AND ENTRY POINT */
  else if ( mark == 'c' || mark == 'f'  ) {
    SPRINTF( buf, "_%s", UpperCase( nm, TRUE, FALSE ) );
    p = CopyString( buf );
    }

  else 
    p = UpperCase( nm, TRUE, FALSE );

  return( p );
}


static PEDGE FindDescriptor( n, f, me )
PNODE n;
PNODE f;
int   me;
{
  register PEDGE i;
  register PINFO out;
  register int   ac;
  register int   c;
  register int   wanted;

  for ( i = n->imp; i != NULL; i = i->isucc )
    if ( i->isucc == NULL )
      break;

  for ( ac = 0, out = f->info->F_OUT; out != NULL; out = out->L_NEXT )
    if ( IsArray( out->L_SUB ) )
      ac++;

  if ( ac <= 0 )
    Error2( "FindDescriptor", "INTERFACE ARRAY OUTPUTS NOT FOUND" );

  if ( i == NULL )
    Error2( "FindDescriptor", "INTERFACE ARRAY DESCRIPTOR NOT FOUND" );

  wanted = ac - me + 1;

  for ( c = 1; i != NULL; i = i->ipred, c++ )
    if ( c == wanted )
      break;

  if ( i == NULL )
    Error2( "FindDescriptor", "INTERFACE ARRAY DESCRIPTOR NOT FOUND (2)" );

  if ( !IsArray( i->info ) )
    Error2( "FindDescriptor", "INTERFACE DESCRIPTOR NOT AN ARRAY" );

  if ( !IsInteger( i->info->A_ELEM ) )
    Error2( "FindDescriptor", "INTERFACE DESCRIPTOR NOT AN ARRAY OF INTEGER" );

  return( i );
}


/**************************************************************************/
/* LOCAL  **************    PrintInterfaceCall     ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE INVOCATION n OF FUNCTION f TO output.         */
/**************************************************************************/

void PrintInterfaceCall( indent, n, f )
int   indent;
PNODE n;
PNODE f;
{
  register PEDGE i;
  register int   c;
  register int   lang;
  register PEDGE dv;
  register PINFO CompType;
  register PINFO tct;
  register int   td;
  register int   d;
  register PINFO rt;
  register int   r;
  register int   ac;
  register int   cmp;
  register PEDGE e;
  register int   arr;

  FPRINTF( output, "/* INTERFACE MODULE CALL */\n" );

  lang = GetLanguage( f );

  switch ( lang ) {
    case FOR_C:
    case FOR_FORTRAN:
      FPRINTF( output, "{\n" );

      /* DECLARE STORAGE FOR PASS BY REFERENCE OPERATION */
      for ( i = n->imp->isucc; i != NULL; i = i->isucc ) {
	if ( IsArray( i->info ) ) {
          CompType = GetComponentType( i->info );
	  FPRINTF( output, "%s *itmp%d;\n", CompType->tname, i->iport );
	  continue;
	  }

	if ( lang != FOR_C )
	  FPRINTF( output, "%s itmp%d;\n", i->info->tname, i->iport );
	}

/* CANN NEW 3/92 */

      /* cmp == TRUE IF 2 OR MORE RETURN VALUES OR 1 ARRAY RESULT */
      cmp = (f->info->F_OUT->L_NEXT == NULL)? FALSE : TRUE;
      cmp = (IsArray(f->info->F_OUT->L_SUB))? TRUE : cmp;

      for ( arr=FALSE,r=0, rt = f->info->F_OUT; rt != NULL; rt = rt->L_NEXT ) {
	r++;

        if ( IsArray( rt->L_SUB ) ) {
          CompType = GetComponentType( rt->L_SUB );
          d  = GetDim( rt->L_SUB );
	  FPRINTF( output, "%s *iret%d;\n", CompType->tname, r );
	  arr = TRUE;
	  }
	else if ( cmp ) {
	  FPRINTF( output, "%s iret%d;\n", rt->L_SUB->tname, r );
	  }
	}

      if ( r > 0 )
        FPRINTF( output, "int iretsz;\n" );
      if ( arr )
        FPRINTF( output, "POINTER IArr;\n" );
/* END CANN NEW 3/92 */

      /* ASSIGN VALUES TO PASS BY REFERENCE STORAGE */
      for ( i = n->imp->isucc; i != NULL; i = i->isucc ) {
        if ( IsArray( i->info ) ) {
          if ( !IsArray( i->info->A_ELEM ) ) {
            FPRINTF( output, "PrepArr1( " );
            FPRINTF( output, " itmp%d, ", i->iport );
            PrintTemp( i );
            FPRINTF( output, ", %s );\n", i->info->A_ELEM->tname );
          } else {
            FPRINTF( output, "PrepArr2( " );
            FPRINTF( output, " itmp%d, ", i->iport );
            PrintTemp( i );
            tct = GetComponentType( i->info );
            td  = GetDim( i->info );
            FPRINTF( output, ", %s, %d, %d, ", tct->tname, td, 3+(td*5) );
	    FPRINTF( output, "I%s );\n", i->info->wname );
	    i->info->touch5 = TRUE;
	    }

          continue;
	  }

        if ( lang != FOR_C ) {
          FPRINTF( output, "itmp%d = ", i->iport );
          PrintTemp( i );
          FPRINTF( output, ";\n" );
          }
        }

/* CANN NEW 3/92 */
      for ( ac=0, r=0, rt = f->info->F_OUT; rt != NULL; rt = rt->L_NEXT ) {
	r++;

        /* PREPARE FOR AN ARRAY OUTPUT */
        if ( IsArray( rt->L_SUB ) ) {
	  ac++;

          CompType = GetComponentType( rt->L_SUB );
          d  = GetDim( rt->L_SUB );
	  dv = FindDescriptor( n, f, ac );

	  if ( bounds || sdbx )
	    FPRINTF( output, "IDescriptorCheck( %d, itmp%d );\n", d,dv->iport);

	  FPRINTF( output, "iretsz =" );

	  for ( c = 0; c < d; c++ )
	    FPRINTF( output, "%s(itmp%d[%d]-itmp%d[%d]+1) ",
		     (c != 0)? "* " : " ",
                     dv->iport, 3+(c*5)+1, dv->iport, 3+(c*5)+0 ); 

          FPRINTF( output, ";\n" );

	  if ( c == 1 )
	    FPRINTF( output, "iretsz = (iretsz<=0)? 0 : iretsz;\n" );
          else
	    FPRINTF( output, "iretsz = (iretsz<=0)? 1 : iretsz;\n" );

	  if ( c == 1 ) {
	    if ( (e = FindExport(n,r)) == NULL ) {
	      FPRINTF( output, 
		       "iret%d = (%s*) Alloc(iretsz*sizeof(%s));\n", 
		       r, CompType->tname, CompType->tname );
	    } else {
	      FPRINTF( output, 
		       "PrepRetArr1( IArr, iret%d, %s, iretsz, itmp%d );\n",
		       r, CompType->tname, dv->iport );
	      PrintTemp( e );
	      FPRINTF( output, " = IArr;\n" );
	      }
	    }
	  else
	    FPRINTF( output, 
		     "iret%d = (%s*) Alloc(iretsz*sizeof(%s));\n", 
		     r, CompType->tname, CompType->tname );
	  }
        }
/* END CANN NEW 3/92 */

      /* MAKE THE CALL */
      if ( cmp )
        FPRINTF( output, "%s( ", n->imp->CoNsT );
      else {
        if ( n->exp != NULL ) { 
          PrintTemp( n->exp );
          FPRINTF( output, " = %s( ", n->imp->CoNsT );
          }
        else
          FPRINTF( output, "%s( ", n->imp->CoNsT );
        }

      for ( c = 1, i = n->imp->isucc; i != NULL; i = i->isucc, c++ ) {
        if ( IsArray( i->info ) )
          FPRINTF( output, "itmp%d", i->iport );
        else if ( lang != FOR_C )
          FPRINTF( output, "&itmp%d", i->iport );
        else 
          PrintTemp( i );

        if ( i->isucc != NULL )
          FPRINTF( output, ", " );

        if ( c % 5 == 0 && i->isucc != NULL )
          FPRINTF( output, "\n  " );
	}

/* CANN NEW 3/92 */
      if ( cmp ) {
        for ( r = 0, rt = f->info->F_OUT; rt != NULL; rt = rt->L_NEXT ) {
	  r++;
	  if ( IsArray( rt->L_SUB ) )
	    FPRINTF( output, ", iret%d", r );
          else
	    FPRINTF( output, ", &iret%d", r );
	  }
        }
/* END CANN NEW 3/92 */

      FPRINTF( output, " );\n" );

      /* FREE INPUT INTERFACE STORAGE */ 
      for ( i = n->imp->isucc; i != NULL; i = i->isucc )
        if ( IsArray( i->info ) )
          if ( IsArray( i->info->A_ELEM ) )
            FPRINTF( output, "DeAlloc( itmp%d );\n", i->iport );

/* CANN NEW 3/92 */
      if ( cmp ) {
        for ( ac=0, r=0, rt = f->info->F_OUT; rt != NULL; rt = rt->L_NEXT ) {
	  r++;

	  if ( (e = FindExport(n,r)) == NULL ) {
            if ( IsArray( rt->L_SUB ) ) {
	      ac++;
              FPRINTF( output, "DeAlloc( iret%d );\n", r );
	      }

	    continue;
	    }

          /* FREE OUTPUT INTERFACE STORAGE */ 
          if ( IsArray( rt->L_SUB ) ) {
	    ac++;

            CompType = GetComponentType( rt->L_SUB );
            d  = GetDim( rt->L_SUB );

	    dv = FindDescriptor( n, f, ac );

	    if ( d == 1 )
	      continue;

	    rt->L_SUB->touch4 = TRUE;

	    PrintTemp( e );
	    FPRINTF( output, " = " );
	    FPRINTF( output, "I%s( FALSE, iret%d, itmp%d );\n", 
		     rt->L_SUB->rname, r, dv->iport );

            FPRINTF( output, "DeAlloc( iret%d );\n", r );
	    continue;
	    }

	  /* SAVE THE SCALAR (IF USED BY THE CALLER!) */
	  if ( (e = FindExport(n,r)) == NULL )
	    continue;

	  PrintTemp( e );
          FPRINTF( output, " = iret%d;\n", r );
	  }
	}
/* END CANN NEW 3/92 */

      FPRINTF( output, "}\n" );
      break;

    default:
      Error2( "PrintInterfaceCall", "ILLEGAL LANGUAGE TYPE" );
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************     PrintWriteArrayOp     ************************/
/**************************************************************************/
/* PURPOSE: PRINT WRITE OPERATION FOR ARRAY i WITH COMPONENT TYPE ct TO   */
/*          output.                                                       */
/**************************************************************************/

static void PrintWriteArrayOp( indent, dst, src, dd, d, i, CompType )
int    indent;
char  *dst;
char  *src;
int    dd;
int    d;
PINFO  i;
PINFO  CompType;
{
  char nsrc[100];
  char ndst[100];
  char buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s *dbase%d;\n", CompType->tname, dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  i%d;\n", "int", dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  dsize%d;\n", "int", dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  lsize%d;\n", "int", dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s *p%d;\n", i->A_ELEM->tname, dd );


  PrintIndentation( indent+2 );
  FPRINTF( output, "dsize%d = dinfo[%d].DSize;\n", dd, dd-1 );

  PrintIndentation( indent+2 );
  FPRINTF( output, "lsize%d = dinfo[%d].LSize;\n", dd, dd-1 );


  PrintIndentation( indent+2 );
  FPRINTF( output, "dbase%d = ((%s*)%s)+(dsize%d*dinfo[%d].Offset);\n",
           dd, CompType->tname, dst, dd, dd-1                               );


  PrintIndentation( indent+2 );
  FPRINTF( output, "PAElm( p%d, (*%s), dinfo[%d].SLow, %s );\n", 
           dd, src, dd-1, i->A_ELEM->tname                    );

  if ( d == 1 ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "if ( dinfo[%d].Mutable )\n", dd-1 );

    PrintIndentation( indent+4 );
    FPRINTF( output, "if ( p%d == dbase%d ) goto MoveOn;\n", dd, dd );
    }


  if ( d == 1 ) {
    PrintVECTOR();
    PrintASSOC();
    SPRINTF( buf, "dbase%d", dd );
    PrintSAFE( buf );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "for ( i%d = 0; i%d < lsize%d; i%d++ ) {\n", 
           dd, dd, dd, dd                                    );


  if ( d == 1 ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "*dbase%d = p%d[i%d];\n", dd, dd, dd );

    PrintIndentation( indent+4 );
    FPRINTF( output, "dbase%d += dsize%d;\n", dd, dd );
  } else {
    SPRINTF( nsrc, "(&p%d[i%d])", dd, dd );
    SPRINTF( ndst, "dbase%d", dd );

    PrintWriteArrayOp( indent+4, ndst, nsrc, dd+1, d-1, i->A_ELEM, CompType );

    PrintIndentation( indent+4 );
    FPRINTF( output, "dbase%d += dsize%d;\n", dd, dd );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  if ( d == 1 )
    FPRINTF( output, "MoveOn:;\n" );

  /* PrintIndentation( indent+2 ); */
  /* FPRINTF( output, "SFreeArr( (*%s) );\n", src ); */

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************   PrintIntWriteRoutine    ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE ARRAY WRITE ROUTINES FOR FUNCTION f TO output.*/
/**************************************************************************/

static void PrintIntWriteRoutine( i )
register PINFO i;
{
  register PINFO CompType;
  register int   d;

  CompType = GetComponentType( i );
  d  = GetDim( i );

  FPRINTF( output, "\nstatic void I%s( arr, out, info )\n", i->wname );
  FPRINTF( output, "%-6s  arr;\n", i->tname );
  FPRINTF( output, "%-6s *out;\n", CompType->tname );
  FPRINTF( output, "%-6s *info;\n", "int" );
  FPRINTF( output, "{\n" );
  FPRINTF( output, "  DIMINFO dinfo[%d];\n", d );

  /* FPRINTF( output, "  Sequential = TRUE;\n" ); */

  if ( bounds || sdbx ) 
    FPRINTF( output, "IDescriptorCheck( %d, info );\n", d );

  FPRINTF( output, "  InitDimInfo( FALSE, %d, dinfo, info );\n", d );

  PrintWriteArrayOp( 2, "out", "(&arr)", 1, d, i, CompType );

  /* fprintf( output, "  Sequential = FALSE;\n" ); */

  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************      PrintReadArray       ************************/
/**************************************************************************/
/* PURPOSE: PRINT READ OPERATION FOR ARRAY i WITH COMPONENT TYPE ct TO    */
/*          output.                                                       */
/**************************************************************************/

static void PrintReadArray( indent, src, dst, dd, d, i, CompType )
int    indent;
char  *src;
char  *dst;
int    dd;
int    d;
PINFO  i;
PINFO  CompType;
{
  char nsrc[100];
  char ndst[100];
  char buf[100];

  PrintIndentation( indent );
  FPRINTF( output, "{\n" );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s *sbase%d;\n", CompType->tname, dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  i%d;\n", "int", dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s *p%d;\n", i->A_ELEM->tname, dd );


  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  dsize%d;\n", "int", dd );

  PrintIndentation( indent+2 );
  FPRINTF( output, "register %-7s  lsize%d;\n", "int", dd );


  PrintIndentation( indent+2 );
  FPRINTF( output, "dsize%d = dinfo[%d].DSize;\n", dd, dd-1 );

  PrintIndentation( indent+2 );
  FPRINTF( output, "lsize%d = dinfo[%d].LSize;\n", dd, dd-1 );


  PrintIndentation( indent+2 );
  FPRINTF( output, "sbase%d = ((%s*)%s)+(dsize%d*dinfo[%d].Offset);\n",
           dd, CompType->tname, src, dd, dd-1                               );


  if ( d == 1 ) {
    PrintIndentation( indent+2 );
    FPRINTF( output, "if ( dinfo[%d].Mutable ) {\n", dd-1 );

    PrintIndentation( indent+4 );
    FPRINTF( output, 
             "OptInitIArr( %s, p%d, lsize%d, dinfo[%d].SLow, %s, sbase%d );\n",
             dst, dd, dd, dd-1, i->A_ELEM->tname, dd                         );

    PrintIndentation( indent+4 );
    FPRINTF( output, "goto MoveOn;\n" );

    PrintIndentation( indent+4 );
    FPRINTF( output, "}\n" );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "InitIArr( %s, p%d, lsize%d, dinfo[%d].SLow, %s );\n",
           dst, dd, dd, dd-1, i->A_ELEM->tname                         );


  if ( d == 1 ) {
    PrintVECTOR();
    PrintASSOC();
    SPRINTF( buf, "p%d", dd );
    PrintSAFE( buf );
    }

  PrintIndentation( indent+2 );
  FPRINTF( output, "for ( i%d = 0; i%d < lsize%d; i%d++ ) {\n", 
           dd, dd, dd, dd                                    );


  if ( d == 1 ) {
    PrintIndentation( indent+4 );
    FPRINTF( output, "p%d[i%d] = *sbase%d;\n", dd, dd, dd );

    PrintIndentation( indent+4 );
    FPRINTF( output, "sbase%d += dsize%d;\n", dd, dd );
  } else {
    SPRINTF( ndst, "(&p%d[i%d])", dd, dd );
    SPRINTF( nsrc, "sbase%d", dd );

    PrintReadArray( indent+4, nsrc, ndst, dd+1, d-1, i->A_ELEM, CompType );

    PrintIndentation( indent+4 );
    FPRINTF( output, "sbase%d += dsize%d;\n", dd, dd );
    }

  PrintIndentation( indent+4 );
  FPRINTF( output, "}\n" );

  if ( d == 1 )
    FPRINTF( output, "MoveOn:;\n" );

  /* OUTERMOST LEVEL!!!! */
  if ( dd == 1 )
     FPRINTF( output, "  ((ARRAYP)arr)->Mutable = dinfo[0].Mutable;\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* LOCAL  **************       PrintReadRoutine    ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE ARRAY READ ROUTINE TO output.                 */
/**************************************************************************/

static void PrintReadRoutine( i )
register PINFO i;
{
  register PINFO CompType;
  register int   d;

  CompType = GetComponentType( i );
  d  = GetDim( i );

  FPRINTF( output, "\nstatic %s I%s( ronly, in, info )\n", i->tname, i->rname );
  FPRINTF( output, "int ronly;\n" );
  FPRINTF( output, "%-6s *in;\n", CompType->tname );
  FPRINTF( output, "%-6s *info;\n", "int" );
  FPRINTF( output, "{\n" );
  FPRINTF( output, "  DIMINFO dinfo[%d];\n", d );
  FPRINTF( output, "  %-7s arr;\n", i->tname );

  if ( bounds || sdbx ) 
    FPRINTF( output, "IDescriptorCheck( %d, info );\n", d );

  FPRINTF( output, "  InitDimInfo( ronly, %d, dinfo, info );\n", d );

  PrintReadArray( 2, "in", "(&arr)", 1, d, i, CompType );

  FPRINTF( output, "  return( arr );\n" );
  FPRINTF( output, "}\n" );
}


/**************************************************************************/
/* GLOBAL **************       GenIsReadOnly          ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF AGGREGATE eport OF NODE n IS READ-ONLY.        */
/**************************************************************************/

int GenIsReadOnly( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;
  register PNODE f;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( e->cm == -1 || e->pm > 0 || e->wmark )
      return( FALSE );

    switch ( e->dst->type ) {
      case IFALimL:
      case IFALimH:
      case IFPrefixSize:
      case IFASize:
      case IFSaveCallParam:
      case IFSaveSliceParam:
	break;

      case IFCall:
	if ( (f = FindFunction( e->dst->imp->CoNsT )) == NULL)
	  Error2( "GenIsReadOnly", "FindFunction FOR CALL FAILED" );

	if ( IsIGraph( f ) )
	  if ( f->mark != 's' ) /* CANN NEW 2/92 */
	    break;

	return( FALSE );

      case IFLoopPoolEnq:
      case IFOptLoopPoolEnq:
	if ( (f = FindFunction( e->dst->usucc->G_NAME )) == NULL)
	  Error2( "GenIsReadOnly", "FindFunction FOR SLICE BODY NAME FAILED" );

	if ( !GenIsReadOnly( f, e->iport ) )
	  return( FALSE );

	break;

      case IFOptAElement:
	if ( e->info->type != IF_PTR )
	  break;

	if ( !GenIsReadOnly( e->dst, 1 ) )
	  return( FALSE );

	break;

      case IFAElement:
	if ( IsBasic( e->info->A_ELEM ) )
	  break;
      case IFGetArrayBase:
      case IFAssign:
	if ( !GenIsReadOnly( e->dst, 1 ) )
	  return( FALSE );

	break;

      case IFForall:
	if ( IsExport( e->dst->F_GEN, e->iport ) != 0 )
	  return( FALSE );
	if ( IsExport( e->dst->F_RET, e->iport ) != 0 )
	  return( FALSE );

	if ( !GenIsReadOnly( e->dst->F_BODY, e->iport ) )
	  return( FALSE );

	break;

      case IFSelect:
        if ( IsExport( e->dst->S_TEST, e->iport ) != 0 )
          return( FALSE );

        if ( IsExport( e->dst->S_CONS, e->iport ) != 0 )
	  if ( !GenIsReadOnly( e->dst->S_CONS, e->iport ) )
	    return( FALSE );

        if ( IsExport( e->dst->S_ALT, e->iport ) != 0 )
	  if ( !GenIsReadOnly( e->dst->S_ALT, e->iport ) )
	    return( FALSE );

        break;

      case IFLoopA:
      case IFLoopB:
	if ( IsExport( e->dst->L_INIT, e->iport ) != 0 )
	  return( FALSE );
	if ( IsExport( e->dst->L_TEST, e->iport ) != 0 )
	  return( FALSE );
	if ( IsExport( e->dst->L_RET,  e->iport ) != 0 )
	  return( FALSE );

	if ( !GenIsReadOnly( e->dst->L_BODY, e->iport ) )
	  return( FALSE );

	break;

      default:
	return( FALSE );
      }
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     PrintIntReadOp        ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE READ OPERATION FOR f (NAMED nm) TO output.    */
/**************************************************************************/

static void PrintIntReadOp( nm, f, lang )
char  *nm;
PNODE  f;
int    lang;
{
  register PINFO  i;
  register int    c;
  register int    ronly;
           char   buf[100];

  for ( c = 1, i = f->info->F_IN; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "args->In%d", c );
    ronly = FALSE;

    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, "  %s = *In%d;\n", buf, c );
        break;

      case IF_ARRAY:
	acnt++;

	/* OPTIMIZE READ-ONLY TRANSFERS INTO SISAL */
	if ( GenIsReadOnly( f, c ) ) {
	  ronly = TRUE;
	  rocnt++;
	  }

	if ( bindtosisal && ronly ) {
	  FPRINTF( output, "  if ( _a_%s->In%d == NULL )\n", nm, c );
	  PrintIndentation( 2 );
	  }

        FPRINTF( output, "  %s = I%s( %s, In%d, InI%d );\n", buf,
			 i->L_SUB->rname, (ronly)? "TRUE" : "FALSE",
			 c, c                                       );
        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************     PrintIntWriteOp       ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE WRITE OPERATION FOR f TO output.              */
/**************************************************************************/

static void PrintIntWriteOp( f, lang )
PNODE f;
int   lang;
{
  register PINFO  i;
  register int    c;
           char   buf[100];

  for ( c = 1, i = f->info->F_OUT; i != NULL; i = i->L_NEXT, c++ ) {
    SPRINTF( buf, "args->Out%d", c );

    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, "  *Out%d = %s;\n", c, buf );
        break;

      case IF_ARRAY:
        FPRINTF( output, "  I%s( %s, Out%d, OutI%d );\n", i->L_SUB->wname, 
                 buf, c, c                                             );
        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    PrintInterfaceHeader   ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE FUNCTION HEADER FOR f WITH NAME nm TO output. */
/**************************************************************************/

static void PrintInterfaceHeader( nm, f, lang )
char  *nm;
PNODE  f;
int    lang;
{
  register PINFO  i;
  register int    c;
  register PINFO  ii;

  FPRINTF( output, "\nstatic %s *_a_%s;\n", f->info->sname, nm );
  FPRINTF( output, "static int _f_%s = TRUE;\n", nm );

  FPRINTF( output, "\nvoid %s(", nm );

  for ( c = 1, i = f->info->F_IN; i != NULL; i = i->L_NEXT, c++ ) {
    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, " In%d,", c );
        break;

      case IF_ARRAY:
        FPRINTF( output, " In%d, InI%d,", c, c );
        break;

      default:
        Error2( "Mixed Language Programming:", "ILLEGAL INTERFACE INPUT TYPE" );
      }

    if ( (c % 5) == 0 )
      FPRINTF( output, "\n  " );
    }

  if ( ((c-1) % 5) != 0 )
    FPRINTF( output, "\n  " );

  for ( c = 1, i = f->info->F_OUT; i != NULL; i = i->L_NEXT, c++ )  {
    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, " Out%d", c );

        if ( i->L_NEXT != NULL )
          FPRINTF( output, "," );

        break;

      case IF_ARRAY:
        FPRINTF( output, " Out%d, OutI%d", c, c );

        if ( i->L_NEXT != NULL )
          FPRINTF( output, "," );

        break;

      default:
        Error2( "Mixed Language Programming:", "ILLEGAL OUTPUT TYPE" );
      }

    if ( (c % 5) == 0 && (i->L_NEXT != NULL) )
      FPRINTF( output, "\n  " );
    }

  FPRINTF( output, " )\n" );

  /* PRINT FUNCTION HEADER ARGUMENT DECLARATIONS */
  for ( c = 1, i = f->info->F_IN; i != NULL; i = i->L_NEXT, c++ ) 
    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, "%-6s *In%d;\n", i->L_SUB->tname, c );
        break;

      case IF_ARRAY:
        ii = GetComponentType( i->L_SUB );

        FPRINTF( output, "%-6s *In%d;\n", ii->tname, c );
        FPRINTF( output, "%-6s *InI%d;\n", "int", c ); 
        break;

      default:
        Error2( "Mixed Language Programming:", "ILLEGAL OUTPUT TYPE" );
      }

  for ( c = 1, i = f->info->F_OUT; i != NULL; i = i->L_NEXT, c++ ) 
    switch( i->L_SUB->type ) {
      case IF_INTEGER:
      case IF_REAL:
      case IF_DOUBLE:
        FPRINTF( output, "%-6s *Out%d;\n", i->L_SUB->tname, c );
        break;

      case IF_ARRAY:
        ii = GetComponentType( i->L_SUB );

        FPRINTF( output, "%-6s *Out%d;\n", ii->tname, c );
        FPRINTF( output, "%-6s *OutI%d;\n", "int", c ); 
        break;

      default:
        Error2( "Mixed Language Programming:", "ILLEGAL OUTPUT TYPE" );
      }
}


/**************************************************************************/
/* GLOBAL **************  PrintInterfaceUtilities  ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE UTILITIES TO output.                          */
/**************************************************************************/

void PrintInterfaceUtilities()
{
  register PINFO i;

  for ( i = ihead; i != NULL; i = i->next ) {
    if ( i->touch4 ) {
      PrintReadRoutine( i );
      i->touch4 = FALSE;
      }

    if ( i->touch5 ) {
      PrintIntWriteRoutine( i );
      i->touch5 = FALSE;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************      PrintInterface       ************************/
/**************************************************************************/
/* PURPOSE: PRINT INTERFACE FUNCTION FOR SISAL FUNCTION f.                */
/**************************************************************************/

void PrintInterface( f )
PNODE f;
{
  register PINFO i;
  register int   c;
  register int   lang;
  register char *nm;

  lang = (f->mark == 'c')? FOR_C : FOR_FORTRAN;

  /* MARK THE READ ROUTINES */
  for ( i = f->info->F_IN; i != NULL; i = i->L_NEXT ) {
    if ( !IsArray( i->L_SUB ) )
      continue;

    i->L_SUB->touch4 = TRUE;
    }

  /* MARK THE WRITE ROUTINES */
  for ( i = f->info->F_OUT; i != NULL; i = i->L_NEXT ) {
    if ( !IsArray( i->L_SUB ) )
      continue;

    i->L_SUB->touch5 = TRUE;
    }

  /* SKIP LEADING 2 UNDERSCORES-------v */
  /* nm = BindInterfaceName( &(f->G_NAME[2]), lang, FALSE ); */
  nm = BindInterfaceName( &(f->G_NAME[2]), lang, 's' ); /* NEW CANN 2/92 */

  PrintInterfaceHeader( nm, f, lang );

  FPRINTF( output, "{\n" );
  FPRINTF( output, "  register %s *args;\n\n", f->info->sname );

  FPRINTF( output, "#ifdef CInfo\n" );
  FPRINTF( output, "  SaveCopyInfo;\n" );
  FPRINTF( output, "#endif\n" );

  FPRINTF( output, "#ifdef FInfo\n" );
  FPRINTF( output, "  SaveFlopInfo;\n" );
  FPRINTF( output, "#endif\n" );

  if ( bindtosisal ) {
    FPRINTF( output, "  if ( _f_%s ) {\n", nm );
    PrintIndentation( 2 );
    }

  PrintIndentation( 2 );
  FPRINTF( output, "_a_%s = (%s*) Alloc( sizeof( %s ) );\n", 
		   nm, f->info->sname, f->info->sname       );

  if ( bindtosisal ) {
    for ( c = 1, i = f->info->F_IN; i != NULL; i = i->L_NEXT, c++ )
      switch( i->L_SUB->type ) {
        case IF_ARRAY:
          FPRINTF( output, "    _a_%s->In%d = NULL;\n", nm, c );
	  break;

	default:
	  break;
        }

    FPRINTF( output, "  }\n" );
    }

  FPRINTF( output, "  args = _a_%s;\n", nm );

  if ( gdata )
    FPRINTF( output, "  InitGlobalData();\n" );

  PrintIntReadOp( nm, f, lang );

  if ( sdbx ) {
    FPRINTF( output, "SdbxCurrentFunctionList = MyFunctionList;\n" );
    FPRINTF( output, "SdbxMonitor( SDBX_ESTART );\n" );
    }

  FPRINTF( output, "  %s( (POINTER) args );\n", f->G_NAME );

  if ( sdbx ) {
    FPRINTF( output, "SdbxMonitor( SDBX_ESTOP );\n" );
    }

  PrintInputDeallocs( nm, 2, f );

  PrintIntWriteOp( f, lang );

  PrintOutputDeallocs( 2, f );

  if ( !bindtosisal )
    FPRINTF( output, "  DeAlloc( (POINTER) args );\n" );

  if ( bindtosisal )
    FPRINTF( output, "  _f_%s = FALSE;\n", nm );

  FPRINTF( output, "}\n" );
}
