/* part.c,v
 * Revision 12.7  1992/11/04  22:05:09  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:37  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input;		/* IF2 INPUT  FILE POINTER */
FILE *output;		/* IF2 OUTPUT FILE POINTER */
FILE *infoptr; 			/* IF2 INFO OUTPUT FILE POINTER */
char infofile[200];

char *program    = "if2part";	/* PROGRAM NAME */

char DefaultStyle = 'R';	/* Defer loop parallelism decision */

int   sgnok      = TRUE;	/* ARE SIGNED ARITHMETIC CONSTANTS ALLOWED? */
int   info	 = FALSE;	/* GENERATE INFORMATION ABOUT OPTIMIZATIONS? */

int   streams    = FALSE;
int   cycle      = FALSE;
int   nopred     = FALSE;

int   prof       = FALSE;	/* PROFILE THE OPTIMIZER? */

int   dovec      = FALSE;

int   procs      = 1;		/* # OF AVAILABLE PROCESSORS IN THE SYSTEM  */
int   cRay       = FALSE;	/* VECTORIZE FOR A CRAY? */
int   vadjust    = 1;		/* VECTORIZATION COST ADJUSTMENT */

FILE  *REPORT_IN = NULL;	/* Where to read/write loop report */
FILE  *REPORT_OUT= NULL;

static char  pstmp[100];	/* CSU-PARTITION STAMP COMMENTARY */
static char *ofile = NULL;	/* OUTPUT FILE NAME */

/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF A FILE.  THE FIRST  */
/*          ENCOUNTERED FILE IS FOR IF2 INPUT.  THE SECOND ENCOUNTERED    */
/*          FILE IS FOR IF2 OUTPUT.  File 3 is the loop report output	  */
/*	    file.  A dash means no file is to be created.  File 4 is the  */
/*	    loop report input file.  ANY OTHER FILES ON THE COMMAND LINE  */
/*          CAUSE AN ERROR MESSAGE TO BE PRINTED.                         */
/*									  */
/*	    FILES:							  */
/*		0:	-> Cost file					  */
/*		1:	-> Update-in-place IF2 file			  */
/*		2:	-> Partitioned IF2 file				  */
/*		3:	-> Loop report output				  */
/*		4:	-> Loop report input				  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/*		-i	-> Print optimization information to stderr	  */
/*		-i<num>	-> Print more information			  */
/*		-p<num>	-> Apply dynamic patch <num>			  */
/*		-s	-> Signed constants aren't allowed		  */
/*		-v	-> Enable vectorizer				  */
/*		-w	-> Suppress warning messages			  */

/*		-A	-> Set the atlevel to allow inner concurency	  */
/*		-C	-> Vectorize for a Cray X-MP			  */
/*		-H#	-> Slice threshold				  */
/*		-L#	-> Nested parallelization threshold		  */
/*		-P#	-> Processors available in the system		  */
/*		-R	-> I'm not really sure what this does?		  */
/*		-S<c>	-> Set loop parallelism style to <c>		  */
/*		-Xfile	-> Module Data Base file name			  */
/*		-V	-> Print vectorization information to stderr	  */
/*		-V<num>	-> Print more vectorization information		  */
/*		-W	-> Profile the optimizer			  */

/*		-@#	-> Assumed number of loop iterations		  */
/**************************************************************************/

static void ParseCommandLine( argc, argv )
int    argc;
char **argv;
{
  register char *c;
  register int   fmode = 0;
  register int   idx;
  register FILE *fd;
  register double huge = -1.0;
  register int   iter = -1;
  register int   rcf = FALSE;
  char	   *report_out_name  = NULL;

  for ( idx = 1; idx < argc; ++idx ) {
    if ( *(c = argv[ idx ]) != '-' ) {
      switch ( fmode ) {
       case 0:
	/* ------------------------------------------------------------ */
	/* Cost file							*/
	/* ------------------------------------------------------------ */
	ReadCostFile( c );
	fmode++; rcf = TRUE;
	break;

       case 1:
	/* ------------------------------------------------------------ */
	/* Input file							*/
	/* ------------------------------------------------------------ */
	if ( (fd = fopen( c, "r" )) == NULL )
	  Error2( "CAN'T OPEN", c );

	input = fd;

	AssignSourceFileName( c );

	fmode++;
	break;

       case 2:
	/* ------------------------------------------------------------ */
	/* Output file							*/
	/* ------------------------------------------------------------ */
	ofile = c;

	fmode++;
	break;

       case 3:
	/* ------------------------------------------------------------ */
	/* Report output file						*/
	/* ------------------------------------------------------------ */
	report_out_name = c;

	fmode++;
	break;

       case 4:
	/* ------------------------------------------------------------ */
	/* Report input file						*/
	/* ------------------------------------------------------------ */
	REPORT_IN = fopen(c,"r");
	if ( !REPORT_IN ) Error2("Bad REPORT input file",c);
	fmode++;
	break;

       default:
	Error2( "ILLEGAL ARGUMENT", c );
      }

      continue;
    }

    switch ( *( ++c ) ) {

      /* ------------------------------------------------------------ */
      /* Suppress warning messages				      */
      /* ------------------------------------------------------------ */
    case 'w':
      Warnings = FALSE;
      break;


      /* ------------------------------------------------------------ */
      /* Apply dynamic patch					      */
      /* ------------------------------------------------------------ */
    case 'p':
      if ( *c ) AddPatch(atoi(c));
      break;

      /* ------------------------------------------------------------ */
      /* MODULE DATABASE FILE NAME				      */
      /* ------------------------------------------------------------ */
    case 'X':
      mdbfile = c+1;
      break;

      /* ------------------------------------------------------------ */
      /* Profile the optimizer					      */
      /* ------------------------------------------------------------ */
    case 'W':
      prof = TRUE;
      break;

      /* ------------------------------------------------------------ */
      /* Vectorizing for Cray X-MP arch.			      */
      /* ------------------------------------------------------------ */
    case 'C':
      cRay = TRUE;
      vadjust = atoi( c+1 );
      break;

      /* ------------------------------------------------------------ */
      /* Skip to next standard file				      */
      /* ------------------------------------------------------------ */
    case '\0':
      fmode ++;
      break;

      /* ------------------------------------------------------------ */
      /* Disllow signed constants in IF2			      */
      /* ------------------------------------------------------------ */
    case 's':
      sgnok = FALSE;
      break;

      /* ------------------------------------------------------------ */
      /* Set the parallel loop style				      */
      /* ------------------------------------------------------------ */
    case 'S':
      DefaultStyle = c[1];
      if ( !DefaultStyle ) DefaultStyle = 'R';
      break;

      /* ------------------------------------------------------------ */
      /* I don't really know what this does (PJM)		      */
      /* ------------------------------------------------------------ */
    case 'R':
      nopred = TRUE;
      break;

      /* ------------------------------------------------------------ */
      /* Set the information collection level			      */
      /* ------------------------------------------------------------ */
    case 'i':
      info = TRUE;
      if ( isdigit((int)(c[1])) ) info=atoi(c+1);
      break;

    case 'F' :
      strcpy (infofile, c+1);
      break;

      /* ------------------------------------------------------------ */
      /* Max number of processors available			      */
      /* ------------------------------------------------------------ */
    case 'P':
      if ( (procs = atoi( c + 1 )) <= 0 )
	Error2( "ILLEGAL ARGUMENT", --c );
      break;

      /* ------------------------------------------------------------ */
      /* Slice threshold					      */
      /* ------------------------------------------------------------ */
    case 'H':
      if ( (huge = atof( c + 1 )) < 0.0 )
	Error2( "ILLEGAL ARGUMENT", --c );
      break;

      /* ------------------------------------------------------------ */
      /* Allow vectorization					      */
      /* ------------------------------------------------------------ */
    case 'v':
      dovec = TRUE;
      break;

      /* ------------------------------------------------------------ */
      /* Set the level of inner loops you wish parallelized	      */
      /* ------------------------------------------------------------ */
    case 'A':
      if ( (atlevel = atoi( c + 1 )) < 0 )
	Error2( "ILLEGAL ARGUMENT", --c );
      break;

      /* ------------------------------------------------------------ */
      /* Consider parallel loops no deeper than #		      */
      /* ------------------------------------------------------------ */
    case 'L':
      if ( (level = atoi( c + 1 )) < 0 )
	Error2( "ILLEGAL ARGUMENT", --c );
      break;

      /* ------------------------------------------------------------ */
      /* Assumed number of loop iterations			      */
      /* ------------------------------------------------------------ */
    case '@':
      if ( (iter = atoi( c + 1 )) < 0 )
	Error2( "ILLEGAL ARGUMENT", --c );
      break;

      /* ------------------------------------------------------------ */
      /* Anything else is an error				      */
      /* ------------------------------------------------------------ */
    default:
      Error2( "ILLEGAL ARGUMENT", --c );
    }
  }


  /* ------------------------------------------------------------ */
  /* Open the report output file				  */
  /* ------------------------------------------------------------ */
  if ( report_out_name ) {
    REPORT_OUT = fopen(report_out_name,"w");
    if ( !REPORT_OUT ) Error2("Bad REPORT output file",report_out_name);
  }

  if ( !rcf ) Error1( "COST FILE NOT PROVIDED ON COMMAND LINE" );

  if ( huge != -1.0 ) SliceThreshold = huge;

  if ( iter != -1 ) {
    Iterations    = (double) iter;
    ArrayCopyCost = (double) iter;
  }
}


/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: PERFORM GRAPH PARTITIONING ANALYSIS. THE INPUT FILE MUST BE   */
/*          DATA FLOW ORDERED, NORMALIZED, MONOLITHIC, AND NOT HAVE       */
/*          OFFSETS ASSIGNED (BY THE LLNL UTILITY if1offset). FURTHER,    */
/*          MINIMAL BUILD-IN-PLACE AND UPDATE-IN-PLACE ANALYSIS MUST HAVE */
/*          ALREADY BEEN DONE. THE 'P' STAMP IS APPENDED TO THE RESULTING */
/*          OUTPUT.                                                       */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
    register FILE *fd;
    int i4 = I_Info4;

    /* fix by dj raymond 25 nov 2000 */
    input = stdin;
    output = stdout;

    ParseCommandLine( argc, argv );

    if (RequestInfo(I_Info4, info)) 
	if ((infoptr = fopen (infofile, "a")) == NULL)
		infoptr = stderr;
    

    if (info > i4 && (RequestInfo(I_Info3, info) || RequestInfo(I_Info2, info) ||
		RequestInfo(I_Info1, info)) && RequestInfo(I_Info4, info))
	FPRINTF (infoptr, "\n\f\n\n");

    StartProfiler();
    If2Read();
    StopProfiler( "If2Read" );

    /* ------------------------------------------------------------ */
    /* If there is a loop report, read it now			    */
    /* ------------------------------------------------------------ */
    if ( REPORT_IN ) ReadReport();

    ReadModuleDataBase();

    (void)fclose( input );	/* AS IT MAY BE THE OUTPUT FILE */

    if ( !IsStamp( DFORDERED ) )
	Error1( "IF2 INPUT IS NOT DFOrdered" );

    if ( IsStamp( OFFSETS ) )
	Error1( "OFFSETS ASSIGNED---NoOp NODES NOT IMPLEMENTED" );

    if ( !IsStamp( NORMALIZED ) )
	Error1( "NORMALIZATION REQUIRED" );

    if ( !IsStamp( BUILDINPLACE ) )
	Error1( "MINIMAL BUILD-IN-PLACE REQUIRED" );

    if ( !IsStamp( UPDATEINPLACE ) )
	Error1( "MINIMAL UPDAYE-IN-PLACE REQUIRED" );

    /* if ( !IsStamp( MONOLITH ) ) */ /* NEW CANN */
	/* Error1( "MONOLITHIC INPUT REQUIRED" ); */

    if ( RequestInfo(I_Info4,info) ) {
      FPRINTF( infoptr, "**** CONCURRENTIZATION\n\n" );
    }

    StartProfiler();
    If2Cost();
    StopProfiler( "If2Cost" );

    StartProfiler();
    If2Part();
    StopProfiler( "If2Part" );

    if ( RequestInfo(I_Info4,info)) {
      PartIf2Count();
    }

    SPRINTF( pstmp, "     CSU -> PARTITIONED: %s",
	            ( sgnok  )? " sgnok"  : ""  );

    AddStamp( PARTITIONED,  pstmp );

    AddModuleStamp();
    WriteModuleDataBase();

    if (RequestInfo(I_Info4, info) && infoptr!=stderr)
        fclose (infoptr);

    /* OPEN THE OUTPUT FILE AND WRITE THE OPTIMIZED PROGRAM               */

    if ( ofile != NULL ) {
        if ( (fd = fopen( ofile, "w" )) == NULL )
            Error2( "CAN'T OPEN", ofile );

            output = fd;
	    }


    StartProfiler();
    If2Write();
    StopProfiler( "If2Write" );

    /* ------------------------------------------------------------ */
    /* Output the loop report */
    StartProfiler();
    PartitionReport();
    StopProfiler( "Partition Report" );

    Stop( OK );
}
