#include "world.h"

/**************************************************************************/
/*                                                                        */
/* NAME                                                                   */
/*    CheckEdge                                                           */
/*                                                                        */
/* DESCRIPTION                                                            */
/*    Checks to make sure that edge e is a valid edge (i.e: makes sure it */
/*    has a valid source and destination pair).                           */
/*                                                                        */
/* PARAMETERS                                                             */
/*    e              The edge that is being checked.                      */
/*                                                                        */
/* RETURNS                                                                */
/*    Nothing.                                                            */
/*                                                                        */
/**************************************************************************/

void CheckEdge(e)
PEDGE e;
{
  PNODE n;
  
  if (e->src == NULL) return;

  for (n=e->dst; n; n=n->npred)
    {
      if (n == e->src) return;
      if IsGraph(n) break;
    }
  for (n=e->dst->nsucc; n; n=n->nsucc)
    {
      if (n == e->src) return;
      if (IsGraph(n)) break;
    }
  FPRINTF(stderr,"Attempted to write illegal edge:\n");
  FPRINTF(stderr, "E %2d %2d   %2d %2d  %2d\n", e->src->label, abs( e->eport ),
	  e->dst->label, abs( e->iport ), e->info->label);
  exit(1);
}
  
