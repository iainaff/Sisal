/**************************************************************************/
/* FILE   **************      FastAreNodesE.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************     FastAreNodesEqual     ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF TWO NODES ARE EQUIVALENT, RETURNING TRUE IF THEY ARE */
/*          AND FALSE IF THEY ARE NOT. THIS ROUTINE, DIFFERING FROM       */
/*          AreNodesEqual ASSUMES n1 AND n2 ARE FROM THE SAME SCOPE!      */
/**************************************************************************/

int FastAreNodesEqual( n1, n2 )
PNODE n1;
PNODE n2;
{
  register PEDGE i1;
  register PEDGE i2;

  if ( !IsSimple( n1 ) )
    return( FALSE );

  if ( n1->type != n2->type )
    return( FALSE );

  if ( glue )
    if ( IsCall( n1 ) )
      return( FALSE );

  switch ( n1->type ) {
   case IFABuild:case IFSBuild: /*cy--same semantics*/
    /* BUG FIX FOR EMPTY ARRAY BUILDS --- 7/14/89 CANN */
    if ( n1->imp->isucc == NULL ) return( FALSE );
    /* END BUG FIX */

   case IFAFill:
   case IFRBuild:
   case IFAGather:
   case IFSGather:
    if ( n1->exp->info->type != n2->exp->info->type )
      return( FALSE );

   default:
    break;
  }


  switch ( n1->type ) {
   case IFAFill:
    /* CANN 6/20/90 */
    if ( DontCombineFill( n1 ) ) return ( FALSE );
    if ( DontCombineFill( n2 ) ) return ( FALSE );
    break;

    /* TAKE ADVANTAGE OF COMMUTATIVITY */

   case IFPlus:
   case IFTimes:
    if ( noassoc ) break;

    /* IF A = C THEN B MUST = D */
    if ( AreEdgesEqual( n1->imp, n2->imp ) )
      if ( AreEdgesEqual( n1->imp->isucc, n2->imp->isucc ) )
        return( TRUE );

    /* IF A = D THEN B MUST = C */
    if ( AreEdgesEqual( n1->imp, n2->imp->isucc ) )
      if ( AreEdgesEqual( n1->imp->isucc, n2->imp ) )
        return( TRUE );

    return( FALSE );

   default:
    break;
  }

  i2 = n2->imp; 
    

  for ( i1 = n1->imp; i1 != NULL; i1 = i1->isucc ) {
    if ( i2 == NULL )
      return( FALSE );

    /* Nasty bug where RBuild for unions overly optimized */
    if ( i1->iport != i2->iport ) return FALSE;
    if ( !AreEdgesEqual( i1, i2 ) )
      return( FALSE );

    i2 = i2->isucc;
  }

  /* UNEQUAL NUMBER OF IMPORTS */
  if ( i2 != NULL )
    return( FALSE );

  return( TRUE );  
}

/*
 * $Log$
 * Revision 1.4  2002/11/21 04:05:05  patmiller
 * Continued updates.  A number of 15 year old bugs have been
 * fixed up:
 *
 * 1) Merging union values where the tags are different, but the value
 *    is the same (e.g. all tags are NULL type)
 *
 * 2) Literals were capped at size 127 bytes! (now 10240)
 *
 * Revision 1.3  2002/11/09 07:40:31  patmiller
 * Mistakenly assumed that edges into different
 * ports of a RBuild (when used as UBuild) could
 * be merged.  I'm surprised this was never exercised.
 *
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:15  patmiller
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
 * Revision 1.2  1994/08/29  08:01:38  chad
 * New STREAM node numbers (333 to ...) have been added.
 * Predicates for identifying STREAM nodes also added.
 *
 * Revision 1.1  1993/04/16  19:00:13  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:47  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
