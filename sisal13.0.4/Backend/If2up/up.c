/* up.c,v
 * Revision 12.7  1992/11/04  22:05:13  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:05  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input;                  /* IF2 INPUT  FILE POINTER         */
FILE *output;                 /* IF2 OUTPUT FILE POINTER         */
FILE *infoptr;                 /* IF2 INFO OUTPUT FILE POINTER         */
FILE *infoptr2;                 /* IF2 INFO OUTPUT FILE POINTER         */
char infofile[200];
char infofile2[200];

char *program    = "if2up";            /* PROGRAM NAME                    */

int   sgnok      = TRUE;     /* ARE SIGNED ARITHMETIC CONSTANTS ALLOWED?  */
int   info	 = FALSE;    /* GENERATE INFORMATION ABOUT OPTIMIZATIONS? */
int   minopt     = FALSE;    /* ONLY PERFORM MINIMAL OPTIMIZATIONS?       */
int   cagg       = TRUE;     /* MARK CONSTANT AGGREGATES?                 */
int   ststr      = TRUE;     /* IDENTIFY SINGLE THREADED STREAMS?         */
int   mig        = TRUE;     /* MIGRATE NODES TOWARD USES?                */
int   prof       = FALSE;    /* PROFILE THE OPTIMIZER?                    */
int   seqimp     = FALSE;
int   sdbx       = FALSE;    /* COMPILE FOR SDBX                          */

static char  ustmp[100];     /* CSU-UPDATE INPLACE STAMP COMMENTARY       */
static char *ofile = NULL;   /* OUTPUT FILE NAME                          */

/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF A FILE.  THE FIRST  */
/*          ENCOUNTERED FILE IS FOR IF2 INPUT.  THE SECOND ENCOUNTERED    */
/*          FILE IS FOR IF2 OUTPUT.  ANY OTHER FILES ON THE COMMAND LINE  */
/*          CAUSE AN ERROR MESSAGE TO BE PRINTED.			  */
/*									  */
/*	    FILES:							  */
/*		0:	-> Memory managed IF2 file			  */
/*		1:	-> Update-in-place IF2 file			  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/*		-c	-> DON'T MARK CONSTANT AGGREGATES		  */
/*		-i	-> PRINT OPTIMIZATION INFORMATION TO stderr.	  */
/*		-i<num>	-> Print more information to stderr.		  */
/*		-l	-> DON'T MIGRATE NODES TOWARD USES		  */
/*		-m	-> PERFORM MINIMAL OPTIMIZATIONS		  */
/*		-p<num>	-> Apply dynamic patch <num>			  */
/*		-s	-> SIGNED CONSTANTS AREN'T ALLOWED		  */
/*		-t	-> DON'T IDENTIFY SINGLE THREADED STREAMS	  */
/*		-w	-> Suppress warning messages			  */

/*		-I	-> ENABLE SEQUENT CODE IMPROVEMENT MIGRATION	  */
/*		-W	-> Profile the program				  */

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

	case '\0':
	  fmode++;
	  break;

	case 'W':
	  prof = TRUE;
	  break;

	case 't':
	  ststr = FALSE;
	  break;

	case 'l':
	  mig = FALSE;
	  break;

	case 'c':
	  cagg = FALSE;
	  break;

	case 'm':
	  minopt = TRUE;
	  cagg   = FALSE;
	  break;

	case 's':
	  sgnok = FALSE;
	  break;

	case 'I':
	  seqimp = TRUE;
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
/* PURPOSE: PERFORM UPDATE-IN-PLACE ANALYSIS.  THE INPUT FILE MUST BE     */
/*          DATA FLOW ORDERED, NORMALIZED, MONOLITHIC, AND NOT HAVE       */
/*          OFFSETS ASSIGNED (BY THE LLNL UTILITY if1offset). FURTHER,    */
/*          MINIMAL BUILD-IN-PLACE ANALYSIS MUST HAVE ALREADY BEEN DONE.  */
/*          THE 'U' STAMP IS APPENDED TO THE RESULTING OUTPUT.            */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
    register FILE *fd;
    int i3 = I_Info3;

    /* fix by dj raymond 25 nov 2000 */
    input = stdin;
    output = stdout;

    ParseCommandLine( argc, argv );

    if (RequestInfo (I_Info3, info)) 
	if ((infoptr = fopen(infofile, "a")) == NULL)
		infoptr = stderr;

    if (RequestInfo (I_Info2, info))  {
	strncpy (infofile2,infofile, strlen(infofile) - 1);
	strcat (infofile2, "2");
	if ((infoptr2 = fopen(infofile2, "a")) == NULL)
		infoptr2 = stderr;
    }

    if (info > i3 && (RequestInfo(I_Info2, info) || RequestInfo(I_Info1, info))
	&& RequestInfo(I_Info3, info))
           FPRINTF (infoptr, "\n\f\n\n");

    StartProfiler();
    If2Read();
    StopProfiler( "If2Read" );

    (void)fclose( input );	/* AS IT MAY BE THE OUTPUT FILE */

    if ( !IsStamp( DFORDERED ) )
	Error1( "IF2 INPUT IS NOT DFOrdered" );

    if ( IsStamp( OFFSETS ) )
	Error1( "OFFSETS ASSIGNED---NoOp NODES NOT IMPLEMENTED" );

    if ( !IsStamp( NORMALIZED ) )
	Error1( "NORMALIZATION REQUIRED" );

    if ( !IsStamp( BUILDINPLACE ) )
	Error1( "MINIMAL BUILD-IN-PLACE REQUIRED" );

    /* if ( !IsStamp( MONOLITH ) ) */ /* NEW CANN 2/92 */
	/* Error1( "MONOLITHIC INPUT REQUIRED" ); */

    if ( RequestInfo(I_Info3,info)  ) {
      FPRINTF( infoptr, "**** COPY ELIMINATIONS\n\n" );
    }

    StartProfiler();
    If2Up( sdbx );
    StopProfiler( "If2Up" );

    SPRINTF( ustmp, "    CSU -> UPDATE-IN-PLACE: %s%s%s%s",
	            ( minopt )? " minopt" : "", ( cagg )?   "cagg"    : "",
		    ( ststr )?  " ststr"  : "", ( sgnok  )? " sgnok"  : "" );

    if ( univso )
	AddStamp( UNIVSTROWNER, "    CSU -> UNIVERSAL STREAM OWNERSHIP" );

    AddStamp( UPDATEINPLACE,  ustmp );
    if (RequestInfo (I_Info3, info) && infoptr!=stderr)
        fclose (infoptr);
    if (RequestInfo (I_Info2, info) && infoptr2!=stderr)
        fclose (infoptr2);

    /* OPEN THE OUTPUT FILE AND WRITE THE OPTIMIZED PROGRAM               */

    if ( ofile != NULL ) {
        if ( (fd = fopen( ofile, "w" )) == NULL )
            Error2( "CAN'T OPEN", ofile );

            output = fd;
	    }

    StartProfiler();
    If2Write();
    StopProfiler( "If2Read" );

    Stop( OK );
}
