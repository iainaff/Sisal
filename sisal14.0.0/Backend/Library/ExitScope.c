#include "world.h"


/**************************************************************************/
/* GLOBAL **************         ExitScope         ************************/
/**************************************************************************/
/* PURPOSE: PERFORM A PARTIAL SCOPE EXIT BY POPING THE TOP NODE LIST      */
/*          HEADER OFF THE NODE LIST HEADER STACK.  THIS ROUTINE IS ONLY  */
/*          CALLED FROM If1Read AFTER EOF IS ENCOUNTERED SO TO PRESERVE   */
/*          gsltop AS A POINTER TO ALL OUTER MOST GRAPHS THAT WERE BUILT  */
/*          (THE FUNCTIONS).                                              */
/**************************************************************************/

void ExitScope()
{
  PopNodeList();
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:26  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
