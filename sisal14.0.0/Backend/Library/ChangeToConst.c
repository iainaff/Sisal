#include "world.h"


/**************************************************************************/
/* GLOBAL **************       ChangeToConst       ************************/
/**************************************************************************/
/* PURPOSE: BLINDLY CHANGE EDGE d INTO CONSTANT c.                        */
/**************************************************************************/

void ChangeToConst( d, c )
PEDGE d;
PEDGE c;
{
  d->CoNsT = c->CoNsT;
  d->info  = c->info; 
  d->src   = NULL;
  d->esucc = NULL;
  d->epred = NULL;
  d->eport = CONST_PORT;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:08  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
