/**************************************************************************/
/* FILE   **************        ReadStamp.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         ReadStamp         ************************/
/**************************************************************************/
/* PURPOSE: READ A STAMP AND ENTER IT IN STAMP TABLE.  THE STAMP SYNTAX   */
/*          IS C$ FOLLOWED BY ONE OR MORE TAB OR BLANK CHARACTERS UP TO A */
/*          NON-EOLN CHARACTER (THE STAMP).  THE REMAINDER OF THE LINE IS */
/*          STAMP DOCUMENTATION. WHEN DONE, BARING ANY TROUBLE, token WILL*/
/*          CONTAIN THE EOLN CHARACTER.  IT IS ASSUMED THAT THE CALLER    */
/*          RECOGNIZED THE C OF C$ AND TOKEN CONTAINS THE $?.  IF A $ IS  */
/*          NOT SEEN, THE LINE IS A COMMENT TO BE DISCARDED BY THE CALLER.*/
/**************************************************************************/

void ReadStamp()
{
  register char stamp;
  register int  kind;

  if ( token == '$' ) {
    NextToken;
    EatSpaces;

    stamp = token;

    if ( IsEoln( token ) )
      ReadError( "ILLEGAL STAMP SYNTAX" );

    NextToken;
    AddStamp( stamp, ReadString( '\n' ) ); 
    return;
  }

  /* CHECK IF COMMENT STRING DEFINES A NAME TABLE ENTRY OF FORM: CE$name, */
  /* CF$name, OR CC$name.  THE LATTER (C FUNCTION NAME) IS THE DEFAULT    */
  /* UNRESOLVED FUNCTION TYPE. */

  switch ( kind = token ) {
   case 'C':
   case 'F':
   case 'E':
    NextToken;

    if ( token != '$' )
      return;

    NextToken;

    if ( IsEoln( token ) )
      return;

    switch ( kind ) {
     case 'E':
      PlaceInEntryTable( LowerCase( ReadString( '\n' ), FALSE, FALSE ) );
      break;

     case 'F':
      PlaceInFortranTable( LowerCase( ReadString( '\n' ), FALSE, FALSE ) );
      break;

     default:
      PlaceInCTable( LowerCase( ReadString( '\n' ), FALSE, FALSE ) );
      break;
    }

   default:
    break;
  }
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:33  patmiller
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
 * Revision 1.1  1993/01/21  23:30:17  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
