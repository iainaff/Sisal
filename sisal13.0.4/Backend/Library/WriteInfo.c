#include "world.h"


/**************************************************************************/
/* GLOBAL **************         WriteInfo         ************************/
/**************************************************************************/
/* PURPOSE: WRITE THE INFO NODE LIST HEADED BY ihead TO OUTPUT.           */
/**************************************************************************/

void WriteInfo()
{
  register PINFO i;
  register int   t;

  for ( i = ihead; i != NULL; i = i->next ) {
    if ( IsBasic(i) )
      t = IF_BASIC;
    else
      t = i->type;

    FPRINTF( output, "T %2d %2d", i->label, t );

    switch ( i->type ) {
     case IF_REDUCTION:
      FPRINTF( output," %2d",(i->R_SETUP == NULL_INFO)? 0 : i->R_SETUP->label);
      FPRINTF( output," %2d",(i->R_MAP == NULL_INFO)? 0 : i->R_MAP->label);
      break;

     case IF_FUNCTION:
      FPRINTF( output, " %2d", (i->F_IN == NULL)? 0 : i->F_IN->label);
      FPRINTF( output, " %2d", (i->F_OUT == NULL)? 0: i->F_OUT->label );
      break;

     case IF_STREAM:
     case IF_MULTIPLE:
     case IF_ARRAY:
     case IF_BUFFER:
      FPRINTF( output, " %2d   ", i->A_ELEM->label );
      break;

     case IF_UNION:
     case IF_RECORD:
      FPRINTF( output, " %2d   ", i->R_FIRST->label );
      break;

     case IF_TUPLE: 
     case IF_FIELD:
     case IF_TAG:
     case IF_SET:
      FPRINTF( output, " %2d", i->L_SUB->label );
      FPRINTF( output, " %2d", 
	      (i->L_NEXT == NULL)? 0 : i->L_NEXT->label );
					 
      break;

     case IF_UNKNOWN:
      FPRINTF( output, "      " );
      break;

     default:
      FPRINTF( output, " %2d   ", i->type - BASE_CODE_FIRST );
      break;
    }

    WritePragmas( i );
  }
}

/* $Log$
 * Revision 1.6  1994/05/04  18:11:02  denton
 * R_BODY->R_MAP; FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.5  1994/04/14  21:40:19  solomon
 * Added case IF_REDUCTION.  Also added checking for i->F_OUT == NULL in
 * case IF_FUNCTION.
 *
 * Revision 1.4  1994/03/09  23:14:49  miller
 * Changes for the new frontend -- Added a new typecode (IF_SET)
 *
 * Revision 1.3  1994/02/15  23:19:26  miller
 * Modifications to allow new kinds of IF1 types (sets, complex, etc...)
 * The makenames utility was modified to check count information.
 *
 * Revision 1.2  1993/11/12  19:58:16  miller
 * Support for IF90 typeset type
 *
 * Revision 1.1  1993/01/21  23:30:47  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
