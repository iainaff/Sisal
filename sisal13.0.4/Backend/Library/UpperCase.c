#include "world.h"


/**************************************************************************/
/* GLOBAL **************         UpperCase         ************************/
/**************************************************************************/
/* PURPOSE: CONVERT THE LOWER CASE LETTERS OF NAME n TO UPPER CASE. THIS  */
/*          ROUTINE APPEND A '_' PREFIX AND SUFFIX TO n IF underl AND     */
/*          underr ARE TRUE RESPECTIVELY.                                 */
/**************************************************************************/

char *UpperCase( n, underl, underr )
char *n;
int   underl;
int   underr;
{
  register char *p;
  char  buf[100];

  SPRINTF( buf, "%s%s%s", (underl)? "_" : "", n, (underr)? "_" : "" );

  for ( p = buf; *p != '\0'; p++ )
    if ( (*p >= 'a') && (*p <= 'z') )
      *p = 'A' + (*p - 'a'); 
    else if ( *p == '.' )
      *p = '_';

  return( CopyString( buf ) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
