/**************************************************************************/
/* FILE   **************       ArchiveStuff.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/* FILE   **************       ArchiveStuff.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

/* ------------------------------------------------------------ */
/* Add a name to a namelink list                                */
/* ------------------------------------------------------------ */
void AddName(list,nm)
     namelink   **list;
     char       *nm;
{
  namelink      *work;

  work = (namelink*)(malloc(sizeof(namelink))); 
  work->name = nm; 
  work->next = *list; 
  *list = work; 
}

/* ------------------------------------------------------------ */
/* Build the export list for the current IF1/2 internal tree    */
/* ------------------------------------------------------------ */
void BuildHaveList(HaveP)
     namelink   **HaveP;
{
  PNODE         F;

  for(F = glstop->gsucc; F; F = F->gsucc) {
    switch ( F->type ) {
    case IFXGraph:
      AddName(HaveP,F->G_NAME);
      break;
    default:
      ;
    }
  }
}

/* ------------------------------------------------------------ */
/* Dump out a namelink list to stdout                           */
/* ------------------------------------------------------------ */
void DumpList(p)
     namelink   *p;
{
  for(; p; p = p->next) {
    if ( *p->name ) printf("%s ",p->name);
  }
  putchar('\n');
}

/* ------------------------------------------------------------ */
/* Search for a name in a namelink list                         */
/* ------------------------------------------------------------ */
int InNameList(name,list)
     char       *name;
     namelink   *list;
{
  namelink      *nl;
  char          lowername[MAX_PATH],*p;

  for(strcpy(lowername,name), p=lowername; *p; p++) {
    if (isupper(*p)) *p = tolower(*p);
  }
  for(nl=list; nl; nl = nl->next) {
    if ( strcmp(nl->name,lowername) == 0 ) return 1;
  }

  return 0;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:10  patmiller
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
 * Revision 1.1  1993/06/15  20:47:00  miller
 * Library support.
 * */
