#include "world.h"


/**************************************************************************/
/* GLOBAL **************        ReadString         ************************/
/**************************************************************************/
/* PURPOSE: READ A STRING TERMINATED BY delim AND RETURN A COPY. THE      */
/*          TRMINATOR  WHITE_CHARS REPRESENTS EITHER EOLN, TAB, OR BLANK. */
/*          THE DELIMITER IS NOT KEPT AS PART OF THE RETURNED STRING.     */
/*          WHEN DONE, token WILL CONTAIN THE CHARACTER FOLLOWING THE     */
/*          TERMINATION CHARACTER UNLESS IT IS EOLN. THE BACKSLASH        */
/*          CHARACTER (\) ACTS AS AN ESCAPE UNLESS THE STRING IS TO END   */
/*          WITH WHITE_CHAR.  AT NO TIME IS A LINE BOUNDARY BROKEN.       */
/**************************************************************************/

char *ReadString( delim )
char delim;
{
  register int  idx   = 0;
  register char prev1 = '\0';
  register char prev2 = '\0';

  for (;;) {
    if ( delim == WHITE_CHARS ) {
      if ( IsWhite( token ) )
	break;
    }
    else if ( (delim == token) && ((prev1 != '\\') || (prev2 == '\\')) )
      break;

    if ( IsEoln( token ) )
      ReadError( "ILLEGAL STRING SYNTAX--UNEXPECTED EOLN" );

    buffer[idx++] = token;

    if ( idx >= BUFFER_SIZE )
      ReadError( "ReadString BUFFER OVERFLOW" );

    prev2 = prev1;
    prev1 = token;
    NextToken;
  }

  buffer[idx] = '\0';

  if ( !IsEoln( token ) )
    NextToken;

  return( CopyString( buffer ) );
}

/* $Log$
 * Revision 1.2  1994/04/01  00:02:54  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.1  1993/01/21  23:30:18  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
