/**************************************************************************/
/* FILE   **************         Patches.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

static int      Patches[100];
static int      PatchCount = 0;

/**************************************************************************/
/* GLOBAL **************         UsingPatch        ************************/
/**************************************************************************/
/* PURPOSE: True iff the numbered patch is active.                        */
/**************************************************************************/
int UsingPatch(P)
     int        P;
{
  int           i;

  for(i=0; i<PatchCount; i++) if ( Patches[PatchCount] == P ) return TRUE;

  return FALSE;
}

/**************************************************************************/
/* GLOBAL **************          AddPatch         ************************/
/**************************************************************************/
/* PURPOSE:  Add a patch to the list of active patches.                   */
/**************************************************************************/
void AddPatch(P)
     int        P;
{
  if ( PatchCount > sizeof(Patches)/sizeof(Patches[0]) ) {
    Error1("Too many patches being applied");
  }

  Patches[PatchCount++] = P;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:30  patmiller
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
 * Revision 1.1  1993/02/24  18:35:27  miller
 * This maintains a list of ``dynamic patches'' which allow me to add
 * temporary fixes that only a few people need.
 *
 */
