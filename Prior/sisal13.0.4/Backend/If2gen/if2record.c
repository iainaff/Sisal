/* if2record.c,v
 * Revision 12.7  1992/11/04  22:05:03  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:03  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PrintUGetTag        ************************/
/**************************************************************************/
/* PURPOSE: PRINT UNION GET TAG NODE n TO output.                         */
/**************************************************************************/

void PrintUGetTag( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );
  PrintTemp( n->exp );
  FPRINTF( output, " = ((%s*)", n->imp->info->sname );
  PrintTemp( n->imp );
  FPRINTF( output, ")->Tag;\n" );
}


/**************************************************************************/
/* GLOBAL **************       PrintUElement       ************************/
/**************************************************************************/
/* PURPOSE: PRINT UElement NODE n TO output. THE DEREFERENCED FIELD IS    */
/*          ASSUMED THE FIRST ENTRY IN THE SUBGRAPHS TAG LIST.            */
/**************************************************************************/

void PrintUElement( indent, n )
int   indent;
PNODE n;
{
    PrintIndentation( indent );

    PrintTemp( n->exp );
    FPRINTF( output, " = " );
    PrintFldRef( n->imp->info->sname, (char*)NULL, n->imp, "Data.Fld", 
                 n->imp->src->G_TAGS->datum                  );
    FPRINTF( output, ";\n" );

    PrintProducerModifiers( indent, n );
    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************        PrintUBuild        ************************/
/**************************************************************************/
/* PURPOSE: PRINT UNION RBuild NODE n TO output.                          */
/**************************************************************************/

void PrintUBuild( indent, n )
int   indent;
PNODE n;
{
    PrintIndentation( indent );

    FPRINTF( output, "UBld( " );
    PrintTemp( n->exp );
    FPRINTF( output, ", %s, %d, %d );\n", n->exp->info->sname,
	     n->exp->sr, n->imp->iport - 1                  );

    PrintFldAssgn( indent, n->exp->info->sname, (char*)NULL, n->exp, 
		   "Data.Fld", n->imp->iport - 1, n->imp   );
}


/**************************************************************************/
/* GLOBAL **************        PrintRBuild        ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD BUILD NODE n TO output. IF n IS A CONSTANT NODE  */
/*          THEN ONLY THE LOCK IS INITIALIZED.                            */
/**************************************************************************/

void PrintRBuild( indent, n )
int   indent;
PNODE n;
{
    register PEDGE i;

    if ( n->cmark ) {
	PrintIndentation( indent );
	FPRINTF( output, "%s( ", GetIncRefCountName( n->exp->info ) );
	PrintTemp( n->exp );
	FPRINTF( output, ", %s, %d );\n", n->exp->info->sname, 1 );
	return;
	}

    PrintIndentation( indent );

    FPRINTF( output, "RBld( " );
    PrintTemp( n->exp );
    FPRINTF( output, ", %s, %d );\n", n->exp->info->sname, n->exp->sr );

    for ( i = n->imp; i != NULL; i = i->isucc )
	PrintFldAssgn( indent, n->exp->info->sname, (char*)NULL, n->exp,
			"Fld", i->iport, i );
}


/**************************************************************************/
/* GLOBAL **************       PrintRElements      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD ELEMENTS NODE n TO output.                       */
/**************************************************************************/

void PrintRElements( indent, n )
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

        PrintIndentation( indent );

        PrintTemp( e );
        FPRINTF( output, " = " );
        PrintFldRef( n->imp->info->sname, (char*)NULL, n->imp, "Fld", e->eport );
        FPRINTF( output, ";\n" );

	for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
	    if ( ee->eport == e->eport )
                ee->eport = -(ee->eport);
	}

    PrintProducerModifiers( indent, n );
    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************       PrintRReplace       ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD REPLACE NODE n TO output. IF n->nmark IS TRUE,   */
/*          AN ATTEMPT IS NOT MADE TO RECYCLE REPLACED AGGREGATES.        */
/**************************************************************************/

void PrintRReplace( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  if ( n->exp->temp != n->imp->temp )
    PrintAssgn( indent, n->exp, n->imp );

  PrintSetRefCount( indent, n->exp, n->exp->sr, FALSE );

  for ( i = n->imp->isucc; i != NULL; i = i->isucc ) {
    if ( !( n->nmark ) )
      if ( !IsBasic( i->info ) ) {
	PrintIndentation( indent );

	FPRINTF( output, "%s( (((%s*)", 
		 GetFreeName( i->info ), n->imp->info->sname );
	PrintTemp( n->imp );
	FPRINTF( output, ")->Fld%d) );\n", i->iport - 1 );
        }
	
    PrintFldAssgn( indent, n->exp->info->sname, (char*)NULL, n->exp,
		   "Fld", i->iport - 1, i                  );
    }
}


/**************************************************************************/
/* GLOBAL **************      PrintRecordNoOp      ************************/
/**************************************************************************/
/* PURPOSE: PRINT RECORD NoOp NODE n TO output.  IF THE INPUT HAS AN R    */
/*          MARK, COPYING ISN'T DONE.  AN r MARK RESULTS IN CONDITIONAL   */
/*          COPYING AND NO MARK ALWAYS RESULTS IN COPYING.                */
/**************************************************************************/

void PrintRecordNoOp( indent, n )
int   indent;
PNODE n;
{
    register char  *opcode;
    register PINFO  i;
    register int    c;

    if ( n->imp->rmark1 == RMARK ) {
	if ( n->imp->temp != n->exp->temp )
	    PrintAssgn( indent, n->exp, n->imp );

	return;
	}

    if ( n->imp->rmark1 == rMARK )
	opcode = "CRNoOp";
    else
	opcode = "RNoOp";

    PrintIndentation( indent );

    FPRINTF( output, "%s( ", opcode );
    PrintTemp( n->exp );
    FPRINTF( output, ", " );
    PrintTemp( n->imp );
    FPRINTF( output, ", %s );\n", n->exp->info->sname );

    for ( c = 1, i = n->exp->info->R_FIRST; i != NULL; i = i->L_NEXT, c++ ) {
	if ( IsBasic( i->L_SUB ) )
	    continue;

	PrintIndentation( indent );

        FPRINTF( output, "%s( ((%s*)", GetIncRefCountName( n->exp->info ),
		 n->exp->info->sname                                    );
	PrintTemp( n->exp );
	FPRINTF( output, ")->Fld%d, %s, 1 );\n", c, i->L_SUB->sname );
	}

    PrintConsumerModifiers( indent, n );

    if ( n->imp->rmark1 == rMARK ) {
        PrintIndentation( indent );

        FPRINTF( output, "EndCRNoOp( " );
	PrintTemp( n->exp );
	FPRINTF( output, ", " );
	PrintTemp( n->imp );
	FPRINTF( output, " );\n" );
	}
}


/**************************************************************************/
/* GLOBAL **************       PrintBRAStore       ************************/
/**************************************************************************/
/* PURPOSE: PRINT BRAStore NODE n OF AGatherAT NODE gat TO output.        */
/**************************************************************************/

void PrintBRAStore( indent, n, gat )
int   indent;
PNODE n;
PNODE gat;
{
  register PNODE l;
  register PEDGE i;

  l = gat->exp->dst->G_DAD;

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    PrintIndentation( indent );

    FPRINTF( output, "((%s*)", n->exp->info->tname );
    PrintTemp( gat->exp );

    if ( l->vmark || l->smark ) {
      FPRINTF( output, ")[" );
      PrintTemp( l->F_GEN->imp );
      FPRINTF( output, "].Fld%d = ", i->iport );
      PrintTemp( i );
      FPRINTF( output, ";\n" );
    } else {
      FPRINTF( output, ")->Fld%d =", i->iport );
      PrintTemp( i );
      FPRINTF( output, ";\n" );
      }
    }

    if ( !( l->vmark || l->smark ) ) {
      PrintIndentation( indent );
      PrintTemp( gat->exp );
      FPRINTF( output, " = (POINTER)(((%s*)", n->exp->info->tname );
      PrintTemp( gat->exp );
      FPRINTF( output, ")+1);\n" );
      }
}


/**************************************************************************/
/* GLOBAL **************        PrintBRBuild       ************************/
/**************************************************************************/
/* PURPOSE: PRINT BRBuild NODE n TO output.                               */
/**************************************************************************/

void PrintBRBuild( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  if ( n->cmark )
    return;

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    PrintIndentation( indent );
    PrintTemp( n->exp );
    FPRINTF( output, ".Fld%d = ", i->iport );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
    }
}


/**************************************************************************/
/* GLOBAL **************    PrintBROptAElement     ************************/
/**************************************************************************/
/* PURPOSE: PRINT AOptAElement-BRElement PAIR ROOTED BY NODE n TO stderr. */
/**************************************************************************/

void PrintBROptAElement( indent, n )
int   indent;
PNODE n;      /* IFBRElement NODE */
{
  register PEDGE e;
  register PEDGE ee;
  register PNODE ae;

  PEDGE laste = NULL;

  ae = n->imp->src;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport < 0 ) {
      e->eport = -(e->eport);
      continue;
      }

    /* BUG FIX TO USE ARRAY INDEX TEMPORARY BEFORE REASSIGNING */
    if (!strcmp(e->temp->name, ae->imp->isucc->temp->name)) {
	laste = e;
        continue;
    }

    PrintIndentation( indent );

    /* PRINT THE ARRAY RECORD DEREFERENCE USING GABASE */
    if (aimp) {
      PrintTemp( e );
      FPRINTF( output, " = " );
      FPRINTF( output, "((%s*)", ae->exp->info->tname );
      PrintTemp( ae->imp );
      FPRINTF( output, ")[" );
      PrintTemp( ae->imp->isucc );
      FPRINTF( output, "]" );
      FPRINTF( output, ".Fld%d;\n", e->eport );

    /* PRINT THE ARRAY RECORD DEREFERENCE WITHOUT USING GABASE */
    } else {
      FPRINTF( output, "ARElm( %s, ", ae->exp->info->tname );
      PrintTemp( e );
      FPRINTF( output, ", " );
      PrintTemp( ae->imp );
      FPRINTF( output, ", " );
      PrintTemp( ae->imp->isucc );
      FPRINTF( output, ", Fld%d );\n", e->eport );
    }

    for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
      if ( ee->eport == e->eport )
        ee->eport = -(ee->eport);
  }

  /* USE TEMPORARY EDGE ALSO SEEN AS AN IMPORT */
  if ( laste!=NULL ) {

      PrintIndentation( indent );

      /* PRINT THE ARRAY RECORD DEREFERENCE USING GABASE */
      if (aimp) {
        PrintTemp( laste );
        FPRINTF( output, " = " );
        FPRINTF( output, "((%s*)", ae->exp->info->tname );
        PrintTemp( ae->imp );
        FPRINTF( output, ")[" );
        PrintTemp( ae->imp->isucc );
        FPRINTF( output, "]" );
        FPRINTF( output, ".Fld%d;\n", laste->eport );

      /* PRINT THE ARRAY RECORD DEREFERENCE WITHOUT USING GABASE */
      } else {
        FPRINTF( output, "ARElm( %s, ", ae->exp->info->tname );
        PrintTemp( laste );
        FPRINTF( output, ", " );
        PrintTemp( ae->imp );
        FPRINTF( output, ", " );
        PrintTemp( ae->imp->isucc );
        FPRINTF( output, ", Fld%d );\n", laste->eport );
      }

      for ( ee = laste->esucc; ee != NULL; ee = ee->esucc )
        if ( ee->eport == laste->eport )
          ee->eport = -(ee->eport);
  }

  PrintProducerModifiers( indent, ae );
  PrintConsumerModifiers( indent, ae );
}


/**************************************************************************/
/* GLOBAL **************      PrintBRElements      ************************/
/**************************************************************************/
/* PURPOSE: PRINT BRElements NODE n TO output.                            */
/**************************************************************************/

void PrintBRElements( indent, n )
int   indent;
PNODE n;
{
  /*
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    PrintIndentation( indent );
    PrintTemp( e );
    FPRINTF( output, " = " );
    PrintTemp( n->imp );
    FPRINTF( output, ".Fld%d;\n", e->eport );
    } 
  */

  register PEDGE e;
  register PEDGE ee;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport < 0 ) {
      e->eport = -(e->eport);
      continue;
      }

    PrintIndentation( indent );
    PrintTemp( e );
    FPRINTF( output, " = " );
    PrintTemp( n->imp );
    FPRINTF( output, ".Fld%d;\n", e->eport );

    for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
      if ( ee->eport == e->eport )
        ee->eport = -(ee->eport);
    }
}


/**************************************************************************/
/* GLOBAL **************      PrintBRReplace       ************************/
/**************************************************************************/
/* PURPOSE: PRINT BRReplace NODE n TO output.                             */
/**************************************************************************/

void PrintBRReplace( indent, n )
int   indent;
PNODE n;
{
  register PEDGE i;

  if ( n->exp->temp != n->imp->temp )
    PrintAssgn( indent, n->exp, n->imp );

  /* DO THE REPLACEMENTS */
  for ( i = n->imp->isucc; i != NULL; i = i->isucc ) {
    PrintIndentation( indent );
    PrintTemp( n->exp );
    FPRINTF( output, ".Fld%d = ", i->iport - 1 );
    PrintTemp( i );
    FPRINTF( output, ";\n" );
    }
}


/**************************************************************************/
/* GLOBAL **************      PrintUTagTest        ************************/
/**************************************************************************/
/* PURPOSE: PRINT UNION TAG TEST NODE n TO output.                        */
/**************************************************************************/

void PrintUTagTest( indent, n )
int   indent;
PNODE n;
{
  PrintIndentation( indent );
  PrintTemp( n->exp );
  FPRINTF( output, " = ( ((%s*)", n->imp->info->sname );
  PrintTemp( n->imp );
  FPRINTF( output, ")->Tag == %d )? TRUE : FALSE;\n", n->imp->iport );
}
