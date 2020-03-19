/**************************************************************************/
/* FILE   **************         CaseCmp.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************           CaseCmp         ************************/
/**************************************************************************/
/* PURPOSE:  Case insenstivie compare of two strings                      */
/**************************************************************************/
int CaseCmp(s1, s2)
register char *s1, *s2;
{
    register unsigned char c1, c2;

    while (*s1) {
        if (*s1++ != *s2++) {
            if (!s2[-1])
                return 1;
            c1 = toupper(s1[-1]);
            c2 = toupper(s2[-1]);
            if (c1 != c2)
                return c1 - c2;
        }
    }
    if (*s2)
        return -1;
    return 0;
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
 * Revision 1.1  1993/11/12  20:06:07  miller
 * Added case insensitive comparison routine
 *
 */
