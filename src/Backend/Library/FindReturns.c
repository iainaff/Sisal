/**************************************************************************/
/* FILE   **************       FindReturns.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

static char *sys_returns[RETURNS_CODE_LAST-RETURNS_CODE_FIRST+1] = {
    "*sysUSER",
    "*sysSUM",
    "*sysPRODUCT",
    "*sysLEAST",
    "*sysGREATEST",
    "*sysCATENATE",
    "*sysARRAY",
    "*sysSTREAM",
    "*sysHISTOGRAM",
    "*sysVALUE"
};

/**************************************************************************/
/* GLOBAL *******************    FindReturnsCode   ************************/
/**************************************************************************/
/* PURPOSE: FIND THE CODE CORRESPONDING TO A RETURNS CLAUSE.              */
/**************************************************************************/

int
FindReturnsCode( returns )
  char *returns;
{
  int   code = RETURNS_USER;

  if (returns[0]=='*') {
      switch (returns[4]) {
      case 'A':
          if (strcmp(returns, sys_returns[RETURNS_ARRAY])==0)
              code = RETURNS_ARRAY;
          break;
      case 'C':
          if (strcmp(returns, sys_returns[RETURNS_CATENATE])==0)
              code = RETURNS_CATENATE;
          break;
      case 'G':
          if (strcmp(returns, sys_returns[RETURNS_GREATEST])==0)
              code = RETURNS_GREATEST;
          break;
      case 'H':
          if (strcmp(returns, sys_returns[RETURNS_HISTOGRAM])==0)
              code = RETURNS_HISTOGRAM;
          break;
      case 'L':
          if (strcmp(returns, sys_returns[RETURNS_LEAST])==0)
              code = RETURNS_LEAST;
          break;
      case 'P':
          if (strcmp(returns, sys_returns[RETURNS_PRODUCT])==0)
              code = RETURNS_PRODUCT;
          break;
      case 'S':
          if (strcmp(returns, sys_returns[RETURNS_SUM])==0)
              code = RETURNS_SUM;
          else if (strcmp(returns, sys_returns[RETURNS_STREAM])==0)
              code = RETURNS_STREAM;
          break;
      case 'V':
          if (strcmp(returns, sys_returns[RETURNS_VALUE])==0)
              code = RETURNS_VALUE;
          break;
      }
  }
  return code;
}

/**************************************************************************/
/* GLOBAL *******************    FindReturnsString   **********************/
/**************************************************************************/
/* PURPOSE: FIND THE STRING CORRESPONDING TO A RETURNS CODE.              */
/**************************************************************************/

char* FindReturnsString( returns_code )
  int returns_code;
{
  if (returns_code>=RETURNS_CODE_FIRST && returns_code<=RETURNS_CODE_LAST)
      return sys_returns[returns_code];
  else
      return NULL;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:17  patmiller
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
 * Revision 1.3  1994/07/21  14:25:26  solomon
 * Added "*sysVALUE" to the sys_returns array and added the appropriate
 * case for "*sysVALUE."
 *
 * Revision 1.2  1994/06/07  14:41:43  solomon
 * Added "*sysUSER" to sys_returns[].
 *
 * Revision 1.1  1994/05/03  18:25:23  denton
 * Added FindReturns and RETURNS_* for Sisal90 returns clauses.
 *
 *
 */
