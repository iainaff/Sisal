/**************************************************************************/
/* FILE   **************        BuildItems.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

int    tbase            = 0;    /* Label IF1 types from 0 (first file) */
/*int    tmax           = 0;     Biggest type (not used) */

char  *stamps[128] =            /* STAMPS FOR ANY CHARACTER       */
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

char  *sfile  = NULL;           /* SISAL FILE BEING READ */
PNODE  cfunct = NULL;           /* FUNCTION BEING BUILT OR EXAMINED */

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
PNODE nhash[MAX_HASH];
PNODE nprd              = NULL;
PNODE nlstop            = NULL; /* NODE  LIST STACK TOP */
char  *sfunct           = NULL;
PINFO  ihead            = NULL; /* SYMBOL TABLE HEAD POINTER */
PINFO  itail            = NULL; /* SYMBOL TABLE TAIL POINTER */
PNODE  nhead            = NULL; /* NODE LIST HEAD POINTER */
PNODE  ntail            = NULL; /* NODE LIST TAIL POINTER */
PNODE  glstop           = NULL;

PINFO ptr_real          = NULL; /* VARIOUS SYMBOL TABLE ENTRIES */
PINFO ptr_double        = NULL;
PINFO ptr_integer       = NULL;
PINFO ptr               = NULL;
PINFO integer           = NULL;

void BuildItems()
{
  int           i;

  tbase         = 0;
  /* tmax       = 0;*/
  LargestTypeLabelSoFar         = 0;
  for(i=0;i<128;i++) stamps[i] = NULL;
  sfile         = NULL;
  cfunct        = NULL;
  nprd          = NULL;
  nlstop        = NULL;
  sfunct        = NULL;
  ihead         = NULL;
  itail         = NULL;
  nhead         = NULL;
  ntail         = NULL;
  glstop        = NULL;

  ptr_real      = NULL;
  ptr_double    = NULL;
  ptr_integer   = NULL;
  ptr           = NULL;
  integer       = NULL;

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
 * Revision 1.3  1994/05/25  23:35:00  solomon
 * Changed variable tmax to LargestTypeLabelSoFar.
 *
 * Revision 1.2  1993/06/14  20:43:59  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.1  1993/01/21  23:28:00  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
