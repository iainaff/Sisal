/* if2select.c,v
 * Revision 12.7  1992/11/04  22:05:03  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        PrintSelect        ************************/
/**************************************************************************/
/* PURPOSE: PRINT Select NODE n TO output.                                */
/**************************************************************************/

void PrintSelect( indent, n )
int   indent;
PNODE n;
{
    PrintIndentation( indent );

    FPRINTF( output, "if ( " );

    if ( n->usucc != NULL ) {
	if ( IsNot( n->usucc ) )
	    FPRINTF( output, " !( " );

        PrintTemp( n->usucc->imp );

	switch ( n->usucc->type ) {
	    case IFGreat:
		FPRINTF( output, " > " );
		goto DyadicFinish;

	    case IFGreatEqual:
		FPRINTF( output, " >= " );
		goto DyadicFinish;

	    case IFLess:
		FPRINTF( output, " < " );
		goto DyadicFinish;

	    case IFLessEqual:
		FPRINTF( output, " <= " );
		goto DyadicFinish;

	    case IFEqual:
		FPRINTF( output, " == " );
		goto DyadicFinish;

	    case IFNotEqual:
		FPRINTF( output, " != " );
		goto DyadicFinish;

	    case IFPlus:
		FPRINTF( output, " || " );
		goto DyadicFinish;

	    case IFTimes:
		FPRINTF( output, " && " );

DyadicFinish:
		PrintTemp( n->usucc->imp->isucc );
		break;

	    case IFNot:
		FPRINTF( output, " )" );
		break;
	    }

	}
    else
        PrintTemp( n->S_TEST->imp );

    FPRINTF( output, " ) {\n" );

	PrintProducerModifiers( indent + 2, n->S_CONS );
	PrintGraph( indent + 2, n->S_CONS );
	PrintProducerModifiers( indent + 2, n );
	PrintConsumerModifiers( indent + 2, n->S_CONS );

    PrintIndentation( indent );
    FPRINTF( output, "} else {\n" );

	PrintProducerModifiers( indent + 2, n->S_ALT );
	PrintGraph( indent + 2, n->S_ALT );
	PrintProducerModifiers( indent + 2, n );
	PrintConsumerModifiers( indent + 2, n->S_ALT );

    PrintIndentation( indent );
    FPRINTF( output, "}\n" );

    PrintConsumerModifiers( indent, n );
}


/**************************************************************************/
/* GLOBAL **************        PrintTagCase       ************************/
/**************************************************************************/
/* PURPOSE: PRINT TagCase NODE n TO output.                               */
/**************************************************************************/

void PrintTagCase( indent, n )
int   indent;
PNODE n;
{
  register PALIST c;
  register PNODE sg;

  PrintIndentation( indent );

  FPRINTF( output, "switch ( " );
  PrintTemp( n->usucc->exp );
  FPRINTF( output, " ) {\n" );

  for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
    for ( c = sg->G_TAGS; c != NULL; c = c->next ) {
      PrintIndentation( indent + 2 );
      FPRINTF( output, "case %d:\n", c->datum );
      }

    if ( sg->G_TAGS == NULL )
      continue;

    PrintProducerModifiers( indent + 4, sg );
    PrintGraph( indent + 4,sg );
    PrintProducerModifiers( indent + 4, n );
    PrintConsumerModifiers( indent + 4, sg );

    PrintIndentation( indent + 4 );
    FPRINTF( output, "break;\n" );
    }

  PrintIndentation( indent + 2 );
  FPRINTF( output, "default:\n" );
  PrintIndentation( indent + 4 );
  FPRINTF( output, "Error( \"TagCase\", \"ILLEGAL UNION TAG\" );\n" );

  PrintIndentation( indent );
  FPRINTF( output, "}\n" );

  PrintConsumerModifiers( indent, n );
}
