#include "world.h"


/**************************************************************************/
/* GLOBAL **************       MakeCompound        ************************/
/**************************************************************************/
/* PURPOSE: MAKE A COMPOUND NODE.  ALL THE SUBGRAPHS HAVE BEEN BUILD AND  */
/*          ARE ADDRESSED BY glstop (IN ORDER OF ENCOUNTER). THE CURRENT  */
/*          NODE LIST ADDRESSED BY nlstop IS REMOVED TO UNCOVER THE SCOPE */
/*          DEFINING THE COMPOUND NODE AND TO COMPLETE CONSTRUCTION OF    */
/*          THE LAST SUBGRAPH.  THEN THE SUBGRAPHS ARE LINKED TO THE      */
/*          COMPOUND NODE, COMPLETING SCOPE EXIT.                         */
/**************************************************************************/

void MakeCompound( label, type, cnt, lst )
int    label;
int    type;
int    cnt;
PALIST lst;
{
  register PNODE n;

  PopNodeList();

  n          = FindNode( label, type );
  n->if1line = line;

  NodeAssignPragmas( n );

  n->C_SUBS = glstop->gsucc;

  if ( glstop->gsucc != NULL )
    glstop->gsucc->gpred = n;

  n->C_SCNT = cnt;
  n->C_ALST = lst;

  AssignTags( lst, n );

  PopGraphList();
}

/* $Log$
 * Revision 1.2  1994/01/28  00:09:39  miller
 * new man pages and new helper functions for library
 *
 * Revision 1.1  1993/01/21  23:29:33  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
