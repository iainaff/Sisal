#include "world.h"


/**************************************************************************/
/* GLOBAL **************        AssignTags         ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TAGS FROM THE ASSOCIATION LIST lst TO SUBGRAPHS OF     */
/*          COMPOUND NODE dad. THE GRAPHS OF INTEREST ARE THOSE ADDRESSED */
/*          BY glstop. THESE SUBGRAPH GRAPHS ARE UPDATED TO ADDRESS dad.  */
/*          THE GRAPH FOUND BY THE nth ASSOCIATION LIST ENTRY IS ASSIGNED */
/*          THE TAG n.                                                    */
/**************************************************************************/

void AssignTags( lst, dad )
PALIST lst;
PNODE  dad;
{
  register PNODE n;
  register int   tag;

  for ( n = glstop->gsucc; n !=NULL; n = n->gsucc )
    n->G_DAD = dad;


  for ( tag = 0; lst != NULL; lst = lst->next, tag++ ) {
    if ((n = FindGraph( lst->datum )) == NULL) break; 
    
    if ( n->G_TAGS == NULL )
      n->G_TAGS = AssocListAlloc( tag );
    else
      n->G_TAGS = LinkAssocLists( n->G_TAGS, AssocListAlloc( tag ) );
  }
}

/* $Log$
 * Revision 1.2  1994/03/03  20:05:12  solomon
 * Added some tests to help prevent failing when dealing with invalid
 * if1 code.
 *
 * Revision 1.1  1993/01/21  23:27:57  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
