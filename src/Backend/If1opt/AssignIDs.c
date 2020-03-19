/**************************************************************************/
/* FILE   **************        AssignIDs.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:10  patmiller
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
 * Revision 1.2  1994/04/15  15:50:28  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.1  1993/01/14  22:25:57  miller
 * Added a compound node identifier field to the PRAGMA set.  Modified the
 * vinfo and info options (-I and -i).  Modified the if1split to use
 * a pragma %TC (ThinCopy) instead of line number to identify split off
 * loop bodies.
 * */
/**************************************************************************/


#include "world.h"

/**************************************************************************/
/* GLOBAL  **************       NewCompoundID        **********************/
/**************************************************************************/
/* PURPOSE: Give a single node a new identifier                           */
/**************************************************************************/
void NewCompoundID(n)
     PNODE      n;
{
  static int    ID=0;

  n->ID = ++ID;
}

/**************************************************************************/
/* LOCAL  ***************     AssignCompoundIDs      **********************/
/**************************************************************************/
/* PURPOSE: Recursively assign ID numbers to each compound node.          */
/**************************************************************************/
static void AssignIDs(g)
     PNODE      g;
{
  PNODE         n,subg;

  /* Assign ID's in node order.  Recursively assign within compounds */
  for ( n = g; n != NULL; n = n->nsucc ) {

    if ( IsCompound(n) ) {
      NewCompoundID(n);
      for ( subg = n->C_SUBS; subg; subg = subg->gsucc ) AssignIDs(subg);
    }
  }
}


/**************************************************************************/
/* GLOBAL  **************     AssignCompoundIDs      **********************/
/**************************************************************************/
/* PURPOSE: Give compound nodes unique identifiers so that we can         */
/*          manipulate them (with the loop report, etc..)                 */
/**************************************************************************/

void AssignCompoundIDs()
{
  PNODE         f;

  /* Assign IDs within each function */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    AssignIDs(f);
  }
}
