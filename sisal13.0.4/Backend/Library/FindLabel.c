#include "world.h"


/**************************************************************************/
/* GLOBAL *********************      FindLabel   **************************/
/**************************************************************************/
/* PURPOSE: FIND THE LABEL CORRESPONDING TO A BASIC TYPE.                 */
/**************************************************************************/

int
FindLabel( type )
  int   type;
{
  PINFO i;

  for ( i = ihead; i != NULL; i = i->next )
    if ( type == i->type )
        return i->label;

  return 0;
}

/* $Log$
 * Revision 1.1  1994/01/28  00:09:31  miller
 * new man pages and new helper functions for library
 *
 * */
