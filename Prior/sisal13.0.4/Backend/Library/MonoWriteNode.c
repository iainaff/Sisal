#include "world.h"


/**************************************************************************/
/* GLOBAL **************   MonoWriteNodes      ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE NODES, EDGES, AND  LITERALS  OF THE  MODULE TO      */
/*          output.                                                       */
/**************************************************************************/

void MonoWriteNodes()
{
  register PNODE n;

  for ( n = nhead; n != NULL; n = n->next ) {
    switch ( n->type ) {
     case IFEdge:
      FPRINTF( output, "E %2d %2d   %2d %2d  %2d", n->snode, n->eport,
	      n->dnode, n->iport, n->info->label                   );
      break;

     case IFLiteral:
      FPRINTF( output, "L         %2d %2d  %2d", n->dnode, n->iport,
	      n->info->label                                     );

      MonoWriteConst( n->CoNsT, n->info );
      break;

     case IFCBegin:
      (void)fputc( '{', output );
      break;

     case IFCEnd:
      FPRINTF( output, "} %s", n->CoNsT );
      break;

     case IFIGraph:
      Error2( "WriteNodes", "IFIGraph ENCOUNTERED" );
      break;

     case IFXGraph:
      FPRINTF( output, "X %2d", n->info->label );
      FPRINTF( output, " \"%s\"", n->CoNsT );
      break;

     case IFLGraph:
      FPRINTF( output, "G %2d", n->info->label );
      FPRINTF( output, " \"%s\"", n->CoNsT );
      break;

     case IFSGraph:
      FPRINTF( output, "G %2d", GRAPH_LABEL );
      break;

     default:
      FPRINTF( output, "N %2d %2d", n->label, n->type );
      break;
    }

    WritePragmas( n );
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:49  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:52  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
