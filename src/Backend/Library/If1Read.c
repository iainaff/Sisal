/**************************************************************************/
/* FILE   **************         If1Read.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************          If1Read          ************************/
/**************************************************************************/
/* PURPOSE: READ IF1 INFORMATION FROM input AND BUILD ITS INTERNAL FORM.  */
/*          A PARTIAL SYNTAX CHECK IS MADE.  IN GENERAL SEMANTIC AND      */
/*          SYNTACTIC CORRECTNESS IS ASSUMED.                             */
/**************************************************************************/

void If1Read()
{
  register int     label, type,  cnt;
  register int     snode, eport, dnode, iport;
  register int     ref1,  ref2;
  register char   *s;
  register PALIST  l;

  EnterScope();

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
      MakeEdge( snode, eport, dnode, iport, type );
      break;

     case 'L':
      dnode = ReadInteger();
      iport = ReadInteger();
      label = ReadInteger();
      s     = ReadLiteral( TRUE );

      ReadPragmas();
      MakeConst( dnode, iport, label, s );
      break;

     case 'G':
      type = ReadInteger();
      s    = ReadLiteral( FALSE );

      ReadPragmas();

      if ( s == NULL )
        MakeGraph( IFSGraph, type, (char*)NULL );
      else
        MakeGraph( IFLGraph, type, s );

      break;

     case 'I':
      type = ReadInteger();
      s    = ReadLiteral( TRUE );

      ReadPragmas();
      MakeGraph( IFIGraph, type, s );
      break;

     case 'X':
      type = ReadInteger();
      s    = ReadLiteral( TRUE );

      ReadPragmas();
      MakeGraph( IFXGraph, type, s );
      break;

     case 'N':
      label = ReadInteger();
      type  = ReadInteger();
    
      ReadPragmas();
      MakeNode( label, type );
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
         case IF_SET:
         case IF_REDUCTION:
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
      EnterScope();
      break;

     case '}':
      label = ReadInteger();
      type  = ReadInteger();
      cnt   = ReadInteger();
      l     = ReadAssocList( cnt );

      ReadPragmas();

      /* PERFORM SCOPE EXIT FOR COMPOUND NODE */
      MakeCompound( label, type, cnt, l );

      break;

     case EOF:
      goto Done;

     default:
      ReadError( "ILLEGAL LINE IDENTIFIER" );
    }

    EatLine;
  }

 Done:
  ExitScope();
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:23  patmiller
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
 * Revision 1.3  1994/03/09  23:14:46  miller
 * Changes for the new frontend -- Added a new typecode (IF_SET)
 *
 * Revision 1.2  1993/11/12  19:56:30  miller
 * Support for IF90 typeset type
 *
 * Revision 1.1  1993/01/21  23:28:48  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
