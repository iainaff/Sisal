#include "world.h"


/**************************************************************************/
/* GLOBAL **************         If1Write          ************************/
/**************************************************************************/
/* PURPOSE: PRINT ALL IF1 TYPES, STAMPS, AND FUNCTION GRAPHS TO output.   */
/*          THE TYPE INFORMATION IS FOUND TRAVERSING THE BINARY TREE      */
/*          itree; THE STAMPS ARE FOUND IN ARRAY stamps; AND THE FUNCTION */
/*          GRAPHS ARE FOUND TRAVERSING glstop. NODES ARE RELABELED AND   */
/*          ALL PORTS OF COMPOUND NODES INCLUDING THEIR SUBGRAPHS ARE     */
/*          RENUMBERED. PORT NUMBER AND NODE LABEL CONVENTIONS MAY HAVE   */
/*          BEEN VIOLATED DURING GRAPH OPTIMIATION. IT IS EASIER TO FIX   */
/*          THIS HERE!  THE SIDE EFFECTS OF CONSTANT FOLDING ARE UNDONE   */
/*          (SIGNED CONSTANTS ARE ALLOWED IF sgnok IS TRUE).              */
/**************************************************************************/

void If1Write()
{
  register PNODE f;

  AddTimeStamp();

  WriteInfo();
  WriteStamps();

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    WriteNodes( cfunct = f );
}

/* $Log$
 * Revision 1.3  1993/03/23  22:48:59  miller
 * date problem
 *
 * Revision 1.2  1994/03/11  18:20:44  miller
 * Added time stamp to all output IF1 and IF2
 *
 * Revision 1.1  1993/01/21  23:28:50  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
