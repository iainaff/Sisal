#include "world.h"


/**************************************************************************/
/* GLOBAL **************          If2Read          ************************/
/**************************************************************************/
/* PURPOSE: READ IF2 INFORMATION FROM input AND BUILD ITS INTERNAL FORM.  */
/*          A PARTIAL SYNTAX CHECK IS MADE.  IN GENERAL SEMANTIC AND      */
/*          SYNTACTIC CORRECTNESS IS ASSUMED.                             */
/**************************************************************************/

void If2Read()
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

     case 'D':
      snode = ReadInteger();
      dnode = ReadInteger();

      MakeAde( snode, dnode );
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

/* $Log$
 * Revision 1.1  1993/01/21  23:28:52  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
