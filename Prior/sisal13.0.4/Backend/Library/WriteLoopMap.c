#include "world.h"


/**************************************************************************/
/* GLOBAL **************       WriteLoopMake       ************************/
/**************************************************************************/
/* PURPOSE:  Writes out the information for all loops in program.	  */
/**************************************************************************/
void WriteLoopMap( msg )
     char *msg;
{
  register PNODE f;

  FPRINTF( infoptr, "\n%s\n\n",msg );
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) ) continue;

    FPRINTF( infoptr, " FUNCTION %s\n",f->G_NAME);
    WriteTheLMap( 1,1,f );	/* Starting level 1, par-loop level 1 */
  }
}

/* $Log$
 * Revision 1.4  1994/06/16  21:32:05  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.3  1993/03/23  22:51:11  miller
 * date problem
 *
 * Revision 1.2  1994/03/18  18:34:03  miller
 * Added a parallelism level to the loop map.
 *
 * Revision 1.1  1993/01/21  23:30:48  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
