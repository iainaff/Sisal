#include "world.h"

/**************************************************************************/
/* GLOBAL **************       AddTimeStamp        ************************/
/**************************************************************************/
/* PURPOSE: Add a stamp with the current time of day to the list of IFx   */
/*	    Stamps to display.						  */
/**************************************************************************/

void AddTimeStamp()
{
  time_t	timeofday;
  char		*asciitime;
  char		*p;

  timeofday = (time_t)time(NULL);
  asciitime = asctime(localtime(&timeofday));
  for(p=asciitime;*p;p++) if ( *p == '\n' ) { *p = '\0'; break; }

  AddStamp('T',asciitime);
}
