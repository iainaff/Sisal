#include "world.h"

int LargestTypeLabelSoFar = 0;

/**************************************************************************/
/* GLOBAL **************         FindLargestLabel     *********************/
/**************************************************************************/
/* PURPOSE: RETURN THE LAST TYPE LABEL USED.                              */
/**************************************************************************/


int FindLargestLabel( )
{
  return LargestTypeLabelSoFar;
}

/* $Log$
 * Revision 1.1  1994/05/25  19:44:31  denton
 * Added FindLargestLabel to return the value of LargestTypeLabelSoFar.
 * Shortened name of GetAggregateType.c to remove ar warnings.
 *
 * */
