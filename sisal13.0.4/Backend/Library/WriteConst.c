#include "world.h"


/**************************************************************************/
/* GLOBAL **************         WriteConst        ************************/
/**************************************************************************/
/* PURPOSE: WRITE A CONSTANT TO output.  IF THE CONSTANT IS REPRESENTED   */
/*          BY A NULL POINTER, THEN IT IS AN ERROR CONSTANT.  ELSE THE    */
/*          APPROPRIATE DELIMITERS ARE DETERMINED BASED ON THE CONSTANTS  */
/*          TYPE. THE PORT NUMBERS MAY BE NEGATIVE.                       */
/**************************************************************************/

void WriteConst( c )
PEDGE c;
{
  FPRINTF( output, "L         %2d %2d  %2d", c->dst->label,
	  abs( c->iport ), c->info->label         );

  if ( c->CoNsT == NULL ) {
    FPRINTF( output, " \"%s\"", ERROR_CONSTANT );
    return;
  }

  if ( IsDefArrayBuf( c->dst ) && (c->isucc == NULL) ) {
    FPRINTF( output, " \"%s\"", c->CoNsT ); /* BUFFER LITERAL */
    return;
  }

  switch ( c->info->type ) {
   case IF_CHAR:
    FPRINTF( output, " \"\'%s\'\"", c->CoNsT );
    break;

   case IF_ARRAY:
   case IF_STREAM:
    FPRINTF( output, " \"\"%s\"\"", c->CoNsT );
    break;

   default:
    FPRINTF( output, " \"%s\"", c->CoNsT );
    break;
  }
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:44  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
