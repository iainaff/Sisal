/* pprint.c,v
 * Revision 12.7  1992/11/04  22:05:12  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:05  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input  = stdin;                  /* IF1 INPUT  FILE POINTER         */
FILE *output = stdout;                 /* IF1 OUTPUT FILE POINTER         */

char *program = "if2pprint";           /* PROGRAM NAME                    */
int   nopp    = FALSE;                 /* ONLY PERFORM DFORDER CHECKS?    */
int   cycle;


/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF A FILE.  THE FIRST  */
/*          ENCOUNTERED FILE IS FOR IF1 INPUT.  THE SECOND ENCOUNTERED    */
/*          FILE IS FOR IF1 OUTPUT.  ANY OTHER FILES ON THE COMMAND LINE  */
/*          CAUSE AN ERROR MESSAGE TO BE PRINTED.                         */
/*									  */
/*	    FILES:							  */
/*		0:	-> IF2 file					  */
/*		1:	-> Pretty-printed IF2 file			  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/**************************************************************************/

static void ParseCommandLine( argc, argv )
int    argc;
char **argv;
{
    register char *c;
    register int   fmode = 0;
    register int   idx;
    register FILE *fd;

    for ( idx = 1; idx < argc; ++idx ) {
        if ( *(c = argv[ idx ]) != '-' ) {
            switch ( fmode ) {
                case 0: 
                    if ( (fd = fopen( c, "r" )) == NULL )
			Error2( "CAN'T OPEN", c );

		    input = fd;

                    AssignSourceFileName( c );

                    fmode++;
                    break;

                case 1:
                    if ( (fd = fopen( c, "w" )) == NULL )
			Error2( "CAN'T OPEN", c );

		    output = fd;

		    fmode++;
                    break;

                default:
                    Error2( "ILLEGAL ARGUMENT", c );
                }

            continue;
            }

        switch ( *( ++c ) ) {
	    case 'n':
		nopp = TRUE;
		break;

            case '\0':
		fmode++;
                break;

            default:
                Error2( "ILLEGAL ARGUMENT", --c );
            }
        }
}


/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: READ AND ECHO AN IF2 FILE IN A HUMAN READABLE FORMAT. TYPE    */
/*          INFORMATION IS NOT PRINTED. NOTE, THIS VERSION DOES NOT PRINT */
/*          ARTIFICIAL DEPENDENCE EDGES.                                  */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
    ParseCommandLine( argc, argv );

    If2Read();

    If2PPrint();

    Stop( OK );
}
