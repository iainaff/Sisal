#include "world.h"


/**************************************************************************/
/* GLOBAL **************        ReadLiteral        ************************/
/**************************************************************************/
/* PURPOSE: READ A LITERAL DELIMITED BY DOUBLE QUOTES OPTIONALY PROCEEDED */
/*          BY ONE OR MORE TAB OR BLANK CHARACTERS (BUT CONTAINED ON ONE  */
/*          LINE). THE LITERAL ITSELF CAN BE A STRING DELIMIED BY DOUBLE  */
/*          QUOTES, SINGLE QUOTES, OR NO QUOTES.  A COPY OF THE LITERAL   */
/*          MINUS ANY DELIMITERS (OUTER OR INNER) IS RETURNED. WHEN DONE, */
/*          token WILL CONTAINS  THE CHARACTER  FOLLOWING  THE  FINAL     */
/*          DELIMITER.  LINE BOUNDARIES ARE NOT BROKEN.  IF A LITERAL IS  */
/*          NOT FOUND, ReadError IS CALLED IF abort IS TRUE.  THE LITERAL */
/*          "error" IS SPECIAL, REPRESENTED INTERNALLY AS A NULL POINTER. */
/**************************************************************************/

char *ReadLiteral( abort )
int abort;
{
  register char *s;

  EatSpaces;

  if ( token != '\"' ) {
    if ( abort ) 
      ReadError( "LITERAL EXPECTED" );

    return( NULL );
  }

  NextToken;

  switch ( token ) {
   case '\n':
    ReadError( "UNEXPECTED EOLN" );

   case '\"':
    NextToken;

    s = ReadString( '\"' );

    if ( token != '\"' )
      ReadError( "LITERAL DELIMITER EXPECTED" );

    NextToken;
    break;

   case '\'':
    NextToken;
    s = ReadString( '\'' );

    if ( token != '\"' )
      ReadError( "LITERAL DELIMITER EXPECTED" );

    NextToken;
    break;

   default:
    s = ReadString( '\"' );

    if ( strcmp( s, ERROR_CONSTANT ) == 0 ) {
      /* free( s ); */
      s = NULL;
    }

    break;
  }

  return( s );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:14  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
