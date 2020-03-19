/* pprint.c,v
 * Revision 12.7  1992/11/04  22:04:59  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:39  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input  = stdin;                  /* IF1 INPUT  FILE POINTER         */
FILE *output = stdout;                 /* IF1 OUTPUT FILE POINTER         */

char *program = "if1pprint";           /* PROGRAM NAME                    */

/* int iter  = FALSE; */                                          /* NEVER USED */
int sgnok = TRUE;
int noassoc = TRUE;
int glue  = FALSE;

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
/*		0:	-> input IF1					  */
/*		1:	-> pretty-printed IF1				  */
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
/* PURPOSE: READ AND ECHO AN IF1 FILE IN A HUMAN READABLE FORMAT. TYPE    */
/*          INFORMATION IS NOT PRINTED.                                   */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
    ParseCommandLine( argc, argv );

    If1Read();

    If1PPrint();

    Stop( OK );
}
