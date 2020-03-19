/**************************************************************************/
/* FILE   **************        AssignTags.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

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

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:12  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.2  1994/03/03  20:05:12  solomon
 * Added some tests to help prevent failing when dealing with invalid
 * if1 code.
 *
 * Revision 1.1  1993/01/21  23:27:57  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
