#include "world.h"

static int	Patches[100];
static int	PatchCount = 0;

/**************************************************************************/
/* GLOBAL **************         UsingPatch        ************************/
/**************************************************************************/
/* PURPOSE: True iff the numbered patch is active.			  */
/**************************************************************************/
int UsingPatch(P)
     int	P;
{
  int		i;

  for(i=0; i<PatchCount; i++) if ( Patches[PatchCount] == P ) return TRUE;

  return FALSE;
}

/**************************************************************************/
/* GLOBAL **************          AddPatch         ************************/
/**************************************************************************/
/* PURPOSE:  Add a patch to the list of active patches.			  */
/**************************************************************************/
void AddPatch(P)
     int	P;
{
  if ( PatchCount > sizeof(Patches)/sizeof(Patches[0]) ) {
    Error1("Too many patches being applied");
  }

  Patches[PatchCount++] = P;
}

/* $Log$
 * Revision 1.1  1993/02/24  18:35:27  miller
 * This maintains a list of ``dynamic patches'' which allow me to add
 * temporary fixes that only a few people need.
 * */
