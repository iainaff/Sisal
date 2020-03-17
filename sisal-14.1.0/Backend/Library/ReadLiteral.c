/**************************************************************************/
/* FILE   **************       ReadLiteral.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

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

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:32  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.1  1993/01/21  23:30:14  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
