/**************************************************************************/
/* FILE   **************       IsIntrinsic.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      IsIntrinsic          ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF nm IS AN INTRINSIC.                            */
/**************************************************************************/

int IsIntrinsic( nm, i )
char  *nm;
PINFO  i;
{
  register char *p;

  p = LowerCase( nm, FALSE, FALSE );

  /* MAP TO C OPERATORS! */
  if ( strcmp( p, "or" ) == 0 ) goto DyadicInt;
  if ( strcmp( p, "xor" ) == 0 ) goto DyadicInt;
  if ( strcmp( p, "and" ) == 0 ) goto DyadicInt;
  if ( strcmp( p, "shiftl" ) == 0 ) goto DyadicInt;
  if ( strcmp( p, "shiftr" ) == 0 ) goto DyadicInt;
  if ( strcmp( p, "not" ) == 0 ) goto MonadicInt;

  /* From math.h */
  if ( strcmp( p, "log" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "log10" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "etothe" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "sin" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "cos" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "tan" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "asin" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "acos" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "atan" ) == 0 ) goto MonadicMix;
  if ( strcmp( p, "atan2" ) == 0 ) goto DyadicMix;
  if ( strcmp( p, "sqrt" ) == 0 ) goto MonadicMix;

  goto ErrExit;

 MonadicInt:
  if ( i->F_IN == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT != NULL ) goto ErrExit;
  if ( !IsInteger( i->F_IN->L_SUB ) ) goto ErrExit;
  if ( i->F_OUT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsInteger( i->F_OUT->L_SUB ) ) goto ErrExit;

  free( p );
  return( TRUE );

 DyadicInt:
  if ( i->F_IN == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsInteger( i->F_IN->L_SUB ) ) goto ErrExit;
  if ( !IsInteger( i->F_IN->L_NEXT->L_SUB ) ) goto ErrExit;

  if ( i->F_OUT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsInteger( i->F_OUT->L_SUB ) ) goto ErrExit;

  free( p );
  return( TRUE );

 MonadicMix:
  if ( i->F_IN == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT != NULL ) goto ErrExit;
  if ( !IsArithmetic( i->F_IN->L_SUB ) ) goto ErrExit;
  if ( i->F_OUT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsArithmetic( i->F_OUT->L_SUB ) ) goto ErrExit;

  free( p );
  return( TRUE );

 DyadicMix:
  if ( i->F_IN == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT == NULL ) goto ErrExit;
  if ( i->F_IN->L_NEXT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsArithmetic( i->F_IN->L_SUB ) ) goto ErrExit;
  if ( !IsArithmetic( i->F_IN->L_NEXT->L_SUB ) ) goto ErrExit;

  if ( i->F_OUT->L_NEXT != NULL ) goto ErrExit;
  if ( !IsArithmetic( i->F_OUT->L_SUB ) ) goto ErrExit;

  free( p );
  return( TRUE );


 ErrExit:
  free( p );
  return( FALSE );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:25  patmiller
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
 * Revision 1.2  1994/07/21  16:28:35  denton
 * Added atan2
 *
 * Revision 1.1  1993/01/21  23:29:11  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
