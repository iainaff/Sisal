/**************************************************************************/
/* FILE   **************       MonoIf1Read.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

/**************************************************************************/
/* GLOBAL **************      MonoIf1Read      ************************/
/**************************************************************************/
/* PURPOSE: READ IF1 INFORMATION FROM input AND BUILD ITS INTERNAL FORM.  */
/*          A PARTIAL SYNTAX CHECK IS MADE.  IN GENERAL SEMANTIC AND      */
/*          SYNTACTIC CORRECTNESS IS ASSUMED.                             */
/**************************************************************************/

void MonoIf1Read()
{
  register int     label, type;
  register int     snode, eport, dnode, iport;
  register int     ref1,  ref2;
  register char   *s;

  token = ' ';
  line  = 1;

  for (;;) {
    switch ( ReadLineId() ) {
     case 'C':
      ReadStamp();
      break;

     case 'E':
      snode = ReadInteger();
      eport = ReadInteger();
      dnode = ReadInteger();
      iport = ReadInteger();
      type  = ReadInteger();

      ReadPragmas();

      MonoMakeEdge( snode, eport, dnode, iport, type );
      break;

     case 'L':
      dnode = ReadInteger();
      iport = ReadInteger();
      type  = ReadInteger();
      s     = ReadLiteral( TRUE );

      ReadPragmas();

      MonoMakeConst( dnode, iport, type , s );
      break;

     case 'G':
      type  = ReadInteger();
      s     = ReadLiteral( FALSE );

      ReadPragmas();

      if ( s == NULL )
        MonoMakeGraph( IFSGraph, type, (char*)NULL );
      else
        MonoMakeGraph( IFLGraph, type, s );

      break;

     case 'I':
      type  = ReadInteger();
      s     = ReadLiteral( TRUE );

      ReadPragmas();

      MonoMakeGraph( IFIGraph, type, s );
      break;

     case 'X':
      type  = ReadInteger();
      s     = ReadLiteral( TRUE );

      ReadPragmas();

      MonoMakeGraph( IFXGraph, type, s );
      break;

     case 'N':
      label = ReadInteger();
      type  = ReadInteger();

      ReadPragmas();

      MonoMakeNode( label, type );
      break;

     case 'T':
      label = ReadInteger();
      type  = ReadInteger();
    
      if ( type != IF_UNKNOWN ) {
        ref1 = ReadInteger();

        switch ( type ) {
         case IF_FIELD:
         case IF_FUNCTION:
         case IF_TAG:
         case IF_TUPLE:
          ref2 = ReadInteger();
          break;

         default:
          ref2 = 0;
          break;
        }
      }

      ReadPragmas();

      MakeInfo( label, type, ref1, ref2 );
      break;

     case '{':
      ReadPragmas();
      MonoMakeCompound( IFCBegin, (char*)NULL );
      break;

     case '}':
      s = MonoReadAssocList();

      ReadPragmas();
      MonoMakeCompound( IFCEnd, s );
      break;

     case EOF:
      goto Done;

     default:
      ReadError( "ILLEGAL LINE IDENTIFIER" );
    }

    EatLine;
  }

 Done:
  return;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:28  patmiller
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
 * Revision 1.1  1993/04/16  19:00:36  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:49  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
