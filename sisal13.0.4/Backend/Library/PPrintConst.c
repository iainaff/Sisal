#include "world.h"


/**************************************************************************/
/* STATIC **************        PPrintConst        ************************/
/**************************************************************************/
/* PURPOSE: PRINT CONSTANT c TO output.                                   */
/**************************************************************************/

void PPrintConst( c )
PEDGE c;
{
  if ( c->CoNsT == NULL ) {
    FPRINTF( output, "\"%s\"", ERROR_CONSTANT );
    return;
  }

  if ( IsDefArrayBuf( c->dst ) && (c->isucc == NULL) ) {
    FPRINTF( output, "\"%s\"", c->CoNsT ); /* BUFFER LITERAL */
    return;
  }

  switch ( c->info->type ) {
   case IF_CHAR:
    FPRINTF( output, "\"\'%s\'\"", c->CoNsT );
    break;

   case IF_ARRAY:
   case IF_STREAM:
    FPRINTF( output, "\"\"%s\"\"", c->CoNsT );
    break;

   default:
    FPRINTF( output, "\"%s\"", c->CoNsT );
    break;
  }
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:53  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
