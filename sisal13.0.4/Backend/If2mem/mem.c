/* mem.c,v
 * Revision 12.7  1992/11/04  22:05:07  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:24  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input;                  /* IF1 INPUT  FILE POINTER         */
FILE *output;                 /* IF1 OUTPUT FILE POINTER         */
FILE *infoptr;                 		/* IF1 INFO OUTPUT FILE POINTER    */
char infofile[200];

char *program    = "if2mem";           /* PROGRAM NAME                    */

int   sgnok      = TRUE;	/* ARE SIGNED ARITHMETIC CONSTANTS ALLOWED?  */
int   info	 = FALSE;	/* GENERATE INFORMATION ABOUT OPTIMIZATIONS? */
int   minopt     = FALSE;	/* ONLY PERFORM MINIMAL OPTIMIZATIONS?       */
int   sdbx       = FALSE;	/* COMPILE FOR sdbx?                         */

int   Oinvar     = TRUE;
int   invar      = TRUE;                 /* GRAPH CLEANUP DISABLE FLAGS   */
int   cse        = TRUE; 
int   gcse       = TRUE;
int   fold       = TRUE;

int   prof       = FALSE;                /* PROFILE THE OPTIMIZER?        */

int   fover      = FALSE;                /* OVERRIDE AGather FILTERS?     */

int   glue       = FALSE;                /* REMOVE DEAD FUNCTION CALLS?   */

static char  mstmp[100];     /* CSU-MEMORY PREALLOCATION STAMP COMMENTARY */
static char *ofile = NULL;

/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF A FILE.  THE FIRST  */
/*          ENCOUNTERED FILE IS FOR IF1 INPUT.  THE SECOND ENCOUNTERED    */
/*          FILE IS FOR IF1 OUTPUT.  ANY OTHER FILES ON THE COMMAND LINE  */
/*          CAUSE AN ERROR MESSAGE TO BE PRINTED.			  */
/*									  */
/*	    FILES:							  */
/*		0:	-> Optimized IF1 file				  */
/*		1:	-> Memory mangaged IF2 file			  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/*		-a	-> EQUIVALENT TO -v, -c, -g, and -f.		  */
/*		-c	-> DISABLE COMMON SUBEXPRESSION REMOVAL		  */
/*		-f	-> DISABLE CONSTANT FOLDING			  */
/*		-g	-> DISABLE GLOBAL CSE REMOVAL			  */
/*		-i	-> PRINT OPTIMIZATION INFORMATION TO stderr.	  */
/*		-i<num>	-> Print more information to stderr.		  */
/*		-m	-> PERFORM MINIMAL OPTIMIZATIONS		  */
/*		-o	-> OVERRIDE AGather FILTERS			  */
/*		-p<num>	-> Apply dynamic patch <num>			  */
/*		-s	-> SIGNED CONSTANTS AREN'T ALLOWED		  */
/*		-v	-> DISABLE INVARIANT REMOVAL			  */
/*		-w	-> Suppress warning messages			  */

/*		-V	-> Do not remove invariants from outer loops	  */
/*		-W	-> Profile the program				  */

/*		-8	-> Do not remove inlined functions		  */

/*		-^	-> SDBX mode					  */
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
		    ofile = c;

		    fmode++;
                    break;

                default:
                    Error2( "ILLEGAL ARGUMENT", c );
                }

            continue;
            }

        switch ( *( ++c ) ) {

	  /* ------------------------------------------------------------ */
	  /* Suppress warning messages					  */
	  /* ------------------------------------------------------------ */
	case 'w':
	  Warnings = FALSE;
	  break;

	  /* ------------------------------------------------------------ */
	  /* Apply dynamic patch					  */
	  /* ------------------------------------------------------------ */
	case 'p':
	  if ( *c ) AddPatch(atoi(c));
	  break;

	case '^':
	  sdbx = TRUE;
	  break;

	case 'W':
	  prof = TRUE;
	  break;

	case '\0':
	  fmode++;
	  break;

	case '8':
	  glue = TRUE;
	  break;

	case 'o':
	  fover = TRUE;
	  break;

	case 'V':
	  Oinvar = FALSE;
	  break;

	case 'v':
	  invar = FALSE;
	  break;

	case 'c':
	  cse = FALSE;
	  break;

	case 'g':
	  gcse = FALSE;
	  break;

	case 'f':
	  fold = FALSE;
	  break;

	case 'a':
	  invar = FALSE;
	  cse   = FALSE;
	  gcse  = FALSE;
	  fold  = FALSE;

	  break;

	case 'm':
	  minopt = TRUE;
	  break;

	case 's':
	  sgnok = FALSE;
	  break;

	case 'i':
	  info = TRUE;
	  if ( isdigit((int)(c[1])) ) info=atoi(c+1);
	  break;

        case 'F' :
          strcpy (infofile, c+1);
          break;

	default:
	  Error2( "ILLEGAL ARGUMENT", --c );
	}
        }
}


/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO PREALLOCATE STORAGE FOR ARRAYS, RESULTING IN THE   */
/*          PLACEMENT OF IF2 NODES IN THE OUTPUT FILE.  THE INPUT MUST    */
/*          BE DATA FLOW  ORDERED,  NOT HAVE  OFFSETS ASSIGNED (DONE BY   */
/*          THE  LLNL UTILITY if1offset),  AND  BE NORMALIZED. A 'B'      */
/*          STAMP IS PLACED IN THE OUTPUT FILE. NOTE, IF THE INPUT FILE   */
/*          NAME IS THE SAME AS THE OUTPUT FILE NAME THEN THE CONTENTS    */
/*          OF THE INPUT FILE IS OVERWRITTEN.                             */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
    register FILE *fd;
    int i2 = I_Info2;

    /* fix by dj raymond 25 nov 2000 */
    input = stdin;
    output = stdout;

    ParseCommandLine( argc, argv );

    if (RequestInfo(I_Info2, info)) 
	if ((infoptr = fopen(infofile, "a")) == NULL)
		infoptr = stderr;

    if (info > i2 && RequestInfo(I_Info1, info) && RequestInfo(I_Info2, info))
        FPRINTF (infoptr, "\n\f\n\n");

    StartProfiler();
    If1Read();
    StopProfiler( "If1Read" );

    (void)fclose( input );	/* AS IT MAY BE THE OUTPUT FILE */

    if ( !IsStamp( DFORDERED ) )
	Error1( "IF1 INPUT IS NOT DFOrdered" );

    if ( IsStamp( OFFSETS ) )
	Error1( "OFFSETS ASSIGNED---NoOp NODES NOT IMPLEMENTED" );

    if ( !IsStamp( NORMALIZED ) )
	Error1( "IF1 FILE IS NOT NORMALIZED" );

    if ( RequestInfo(I_Info2,info)  ) {
        FPRINTF( infoptr, "**** MEMORY MANAGEMENT\n\n" );
	/*CountNodesAndEdges( "BEFORE ARRAY MEMORY OPTIMIZATION" );*/
	}

    StartProfiler();
    If2Mem();
    StopProfiler( "If2Mem" );

    StartProfiler();
    If2Clean();
    StopProfiler( "If2Clean" );

   if ( RequestInfo(I_Info2,info)  ) {
      WriteIf2memPicture();
    }


    if ( !sdbx )
      WriteIf2memWarnings();

    SPRINTF( mstmp, "   CSU -> BUILD-IN-PLACE: %s%s%s%s%s%s%s",
		        ( invar )?  " invar"  : "",   ( cse )?   " cse"   : "",
		        ( gcse )?   " gcse"   : "",   ( fold )?  " fold"  : "",
		        ( sgnok )?  " sgnok"  : "",   ( fover )? " fover" : "",
			( minopt )? " minopt" : ""                           );

    AddStamp( BUILDINPLACE,  mstmp );
    if ( RequestInfo(I_Info2,info) && infoptr!=stderr ) {
        fclose (infoptr);
        }

    /* OPEN THE OUTPUT FILE AND WRITE THE OPTIMIZED PROGRAM               */

    if ( ofile != NULL ) {
        if ( (fd = fopen( ofile, "w" )) == NULL )
            Error2( "CAN'T OPEN", ofile );

            output = fd;
	    }

    StartProfiler();
    If2Write();
    StopProfiler( "If2Write" );

    Stop( OK );
}
