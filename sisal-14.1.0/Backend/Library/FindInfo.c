/**************************************************************************/
/* FILE   **************         FindInfo.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        FindInfo           ************************/
/**************************************************************************/
/* PURPOSE: LOCATE AND RETURN THE INFO NODE WITH LABEL label + tbase IN   */
/*          THE INFO LIST HEADED BY ihead.  IF THE NODE DOES NOT EXIST,   */
/*          IT IS ALLOCATED AND INSERTED INFO THE LIST. NULL IS RETURNED  */
/*          IF THE REQUESTED LABEL IS 0. Tbase IS ADDED TO ALL LABELS TO  */
/*          MAKE THEM UNIQUE ACROSS ALL FILES.  Tmax  IS USED  TO DEFINE  */
/*          tbase FOR THE NEXT READ FILE (IF1LD only)                     */
/**************************************************************************/

PINFO FindInfo( label, type )
int label;
int type;
{
  if ( label <= 0 )
    return( NULL );

  label += tbase;

  if ( LargestTypeLabelSoFar < label )
    LargestTypeLabelSoFar = label;

  if ( ihead == NULL )
    return( ihead = itail = InfoAlloc( label, type ) );

  return( LookupInfo( label, type ) );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:16  patmiller
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
 * Revision 1.2  1994/04/14  21:43:22  solomon
 * Changed variable tmax to variable LargestTypeLabelSoFar.  Variable tmax
 * was used for a couple different things.  LargestTypeSoFar is used only
 * for types.  Also added code to update LargestTypeSoFar when appropriate.
 *
 * Revision 1.1  1993/01/21  23:28:32  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
