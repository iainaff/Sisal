/* $Log$
 * Revision 1.2  1994/04/15  15:50:28  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.1  1993/01/14  22:25:57  miller
 * Added a compound node identifier field to the PRAGMA set.  Modified the
 * vinfo and info options (-I and -i).  Modified the if1split to use
 * a pragma %TC (ThinCopy) instead of line number to identify split off
 * loop bodies.
 * */

#include "world.h"

/**************************************************************************/
/* GLOBAL  **************       NewCompoundID        **********************/
/**************************************************************************/
/* PURPOSE: Give a single node a new identifier				  */
/**************************************************************************/
void NewCompoundID(n)
     PNODE	n;
{
  static int	ID=0;

  n->ID = ++ID;
}

/**************************************************************************/
/* LOCAL  ***************     AssignCompoundIDs      **********************/
/**************************************************************************/
/* PURPOSE: Recursively assign ID numbers to each compound node.	  */
/**************************************************************************/
static void AssignIDs(g)
     PNODE	g;
{
  PNODE		n,subg;

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
/* PURPOSE: Give compound nodes unique identifiers so that we can	  */
/*	    manipulate them (with the loop report, etc..)		  */
/**************************************************************************/

void AssignCompoundIDs()
{
  PNODE		f;

  /* Assign IDs within each function */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    AssignIDs(f);
  }
}
