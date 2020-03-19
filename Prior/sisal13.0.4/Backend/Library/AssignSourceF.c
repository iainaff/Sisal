#include "world.h"


/**************************************************************************/
/* GLOBAL **************    AssignSourceFileName   ************************/
/**************************************************************************/
/* PURPOSE: IF PRESENT, REPLACE FILE NAME n's ".if1" SUFFIX WITH ".sis".  */
/*          N is UPDATED IN SITU AND ASSIGNED TO sfile.                   */
/**************************************************************************/

void AssignSourceFileName( n )
char *n;
{
  register char *p,*nm;
  int		len;

  len = strlen(n);
  nm = (char*)(malloc(len+1));
  strcpy(nm,n);

  for ( p = nm + (len - 1); p != nm; p-- )
    if ( *p == '.' )
      if ( strcmp( p, ".if1" ) == 0 ) {
	*(++p) = 's'; *(++p) = 'i'; *(++p) = 's';
	break;
      }

  sfile = nm;
}

/* $Log$
 * Revision 1.1  1993/11/10  20:05:26  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/04/16  18:59:54  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:56  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
