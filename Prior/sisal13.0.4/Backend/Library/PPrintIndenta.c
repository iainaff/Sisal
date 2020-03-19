#include "world.h"


/**************************************************************************/
/* STATIC **************     PPrintIndentation     ************************/
/**************************************************************************/
/* PURPOSE: PRINT ln FOLLOWED BY indent BLANKS AND indent TO output.      */
/**************************************************************************/

void PPrintIndentation( indent, ln )
int indent;
int ln;
{
  register int i;

  FPRINTF( output, "%4.4d", ln );

  for ( i = 0; i <= indent; i++ )
    FPRINTF( output, " " );

  FPRINTF( output, "%d: ", indent );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:50  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:54  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
