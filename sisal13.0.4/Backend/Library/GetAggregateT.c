#include "world.h"

/**************************************************************************/
/* GLOBAL **************     GetAggregateType      ************************/
/**************************************************************************/
/* PURPOSE:  Find the indicated Array[t], Stream[t], or Multiple[t]	  */
/*	     type.  If not found, make an anonymous new type		  */
/**************************************************************************/
PINFO
GetAggregate(AggregateType,T,name)
     int	AggregateType;
     PINFO	T;
     char       *name;
{
  register PINFO i;

  /* ------------------------------------------------------------ */
  /* See if one already exists with the proper structure	  */
  /* ------------------------------------------------------------ */
  for ( i = ihead; i != NULL; i = i->next ) {
    if ( i->type == AggregateType && i->info1 == T ) return i;
  }
  
  return NewType(AggregateType,T->label,0,name);
}

/* $Log$
 * Revision 1.1  1994/05/25  19:44:32  denton
 * Added FindLargestLabel to return the value of LargestTypeLabelSoFar.
 * Shortened name of GetAggregateType.c to remove ar warnings.
 *
 * */
