#include "world.h"

int  token = ' ';
char buffer[BUFFER_SIZE];

int    line   = 1;
int    maxint = 1;
PRAGS  pragmas;

void ReadItems()
{
  token = ' ';
  line = 1;
  maxint = 1;
}

/* $Log$
 * Revision 1.2  1993/06/14  20:44:02  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.1  1993/01/21  23:30:11  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
