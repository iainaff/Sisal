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

/* $Log$
 * Revision 1.1  1993/01/21  23:30:17  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
