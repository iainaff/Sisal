/* gen.c,v
 * Revision 12.7  1992/11/04  22:05:00  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:57  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


FILE *input;                  /* IF2 INPUT  FILE POINTER         */
FILE *output;                 /* IF2 OUTPUT FILE POINTER         */
FILE *infoptr;                 /* IF2 Info OUTPUT FILE POINTER         */
FILE *infoptr1;                 /* IF2 Info OUTPUT FILE POINTER         */
FILE *infoptr2;                 /* IF2 Info OUTPUT FILE POINTER         */
FILE *infoptr3;                 /* IF2 Info OUTPUT FILE POINTER         */
FILE *infoptr4;                 /* IF2 Info OUTPUT FILE POINTER         */
char infofile1[200];
char infofile2[200];
char infofile3[200];
char infofile4[200];

char *program    = "if2gen";           /* PROGRAM NAME                    */

int   info	 = FALSE;    /* GENERATE INFORMATION ABOUT OPTIMIZATIONS? */
int   regok      = TRUE;     /* ASSIGN register PREFIXES?                 */
int   dbl        = FALSE;    /* TREAT real DATA AS double_real?           */
int   flt        = FALSE;    /* TREAT double_real DATA AS real?           */
int   aimp       = TRUE;     /* OPTIMIZE ARRAY DEREFERENCE OPERATIONS?    */
int   if2opt     = TRUE;     /* OPTIMIZE GatherAT NODES?                  */
int   if3show    = FALSE;    /* Show IF3 variable temporaries?            */
int   timeall	 = FALSE;	/* Time all functions */
int   traceall	 = FALSE;	/* Trace all functions */

int   bounds     = FALSE;    /* GENERATE BOUNDS CHECKING?                 */
int   sdbx       = FALSE;    /* GENERATE SDBX CODE?                       */
int   SISdebug   = FALSE;    /* REMOVE DEAD FUNCTION CALLS?               */
int   assoc      = TRUE;     /* DO ASSOCIATIVE TRANSFORMATIONS?           */
int   gdata      = TRUE;     /* PREPARE GLOBAL DATA?                      */

int   vec        = FALSE;    /* FURTHER VECTORIZE CODE?                   */
int   rag        = TRUE;     /* IDENTIFY RAGGED MEM-ALLOCS?               */
int   bip        = TRUE;     /* BIP OPTIMIZATION?                         */
int   bipmv      = TRUE;

int   fva        = FALSE;    /* FORCE ALLIANT VECTORIZATION PRAGMAS?      */
int   fvc        = FALSE;    /* FORCE CRAY    VECTORIZATION PRAGMAS?      */
int   nltss      = FALSE;    /* COMPILE FOR NLTSS C-COMPILER?             */

int   oruntime   = FALSE;    /* USE ORIGINAL SISAL MICROTASKING SOFTWARE  */

int   standalone = FALSE;    /* CALLED FROM THE OPERATING SYSTEM?  */
int   bindtosisal = FALSE;    /* BIND INTERFACE CALLS TO SISAL?     */

int   freeall    = FALSE;    /* FORCE RELEASE OF ALL STORAGE?             */

int   cRay       = FALSE; 
int   alliantfx  = FALSE;

int   intrinsics = FALSE; /* RECOGNIZE LOGICAL FUNCTIONS: and,or,xor,not? */

int   movereads  = FALSE;                  /* MOVE ARRAY READ OPERATIONS? */
int   xmpchains  = FALSE;               /* FORM CHAINS FOR THE CRAY X-MP? */
int   newchains  = FALSE;

int   gshared    = FALSE;   /* GLOBAL SHARED DATA DECLARATIONS SUPPORTED! */

int   Iupper     = FALSE;           /* INTERFACE NAME GENERATION COMMANDS */
int   IunderR    = FALSE;
int   IunderL    = FALSE;

int   share      = TRUE;            /* TRY AND SHARE POINTER SWAP STORAGE */

int   prof       = FALSE;           /* PROFILE THE OPTIMIZER?             */

static char *ofile = NULL;                            /* OUTPUT FILE NAME */

FILE *hyfd = NULL;                              /* HYBRID FILE DESCRIPTOR */
char *hybrid = NULL;                                  /* HYBRID FILE NAME */

static int  useF = FALSE;               /* CALL THE FORTRAN VECTOR LOOPS? */

int nobrec = FALSE;                 /* DISABLE BASIC RECORD OPTIMIZATION? */ 

int CodeComments = FALSE;           /* DISABLE BASIC RECORD OPTIMIZATION? */ 

int MinSliceThrottle = TRUE;	/* Throttle parallelism if minslice to small */
static int   tracecnt = -1;                    /* TRACE FUNCTION LIST INDEX */
static char *trace[200];                       /* TRACE FUNCTION LIST       */

static int   timecnt = -1;                    /* TIME FUNCTION LIST INDEX */
static char *timelist[200];                   /* TIME FUNCTION LIST       */

static int   flopcnt = -1;                    /* FLOP FUNCTION LIST INDEX */
static char *flop[200];                       /* FLOP FUNCTION LIST       */


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
/*		0:	-> Partitioned IF2 file				  */
/*		1:	-> C code file					  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/*		-a	-> DON'T OPTIMIZE ARRAY DEREFERENCE NODES	  */
/*		-d	-> TREAT real DATA AS double_real		  */
/*		-f	-> TREAT double_real DATA AS real		  */
/*		-i	-> PRINT GENERATION FEEDBACK TO stderr.		  */
/*		-i<num>	-> Print more information			  */
/*		-p<num>	-> Apply dynamic patch <num>			  */
/*		-m	-> Turn off minslice throttle			  */
/*		-r	-> DON"T ASSIGN register PREFIXES		  */
/*		-t	-> Produce IF3 variable temporary file		  */
/*		-u	-> DON'T OPTIMIZE GatherAT NODES		  */
/*		-w	-> Suppress warning messages			  */
/*		-x	-> CALL THE FORTRAN VECTOR ROUTINES		  */
/*		-y	-> Do not allow associative transformations	  */

/*		-A	-> ALLIANT					  */
/*		-B	-> GENERATE BOUNDS CHECKING			  */
/*		-C	-> CRAY X-MP					  */
/*		-G	-> GLOBAL SHARED DATA SUPPORTED			  */
/*		-K<file>-> Hybrid file name				  */
/*		-O	-> USE ORIGINAL MICROTASKING SOFTWARE		  */
/*		-P	-> DUMP PROGRESS PRINT MESSAGE			  */
/*		-T f	-> INTERACTIVE TRACE OF FUNCTION f.		  */
/*		-TT	-> Interactive trace of all functions.		  */
/*		-U	-> Use FORTRAN intrinsic functions		  */
/*		-W	-> Profile the optimizer			  */
/*		-X	-> FURTHER VECTORIZE CODE?			  */
/*		-Y<num>	-> Set depth of pointer swap search		  */

/*		-=	-> DO NOT TRY AND SHARE POINTER SWAP STORAGE	  */
/*		-%	-> Generate inline code comments		  */
/*		-#	-> NO BASIC RECORD OPTIMIZATION			  */
/*		-& f	-> TIME THE EXECUTION OF FUNCTION f.		  */
/*		-&&	-> Time the execution of all functions		  */
/*		-@ f	-> Flop counts for function f.			  */
/*		-{	-> SDBX and bounds checking			  */

/*		-0	-> Move BIP's					  */
/*		-1	-> Identify ragged mem-allocs			  */
/*		-2	-> MOVE ARRAY READ OPERATIONS			  */
/*		-3	-> FORM Cray X-MP CHAINS			  */
/*		-4	-> Bind interface calls to Sisal		  */
/*		-5	-> Force alliant vectorization pragmas		  */
/*		-6	-> Force cray vectorization pragmas		  */
/*		-7	-> Compile for NLTSS C compiler			  */
/*		-8	-> Remove dead function calls (<not active>)	  */
/*		-9	-> Force release of all storage			  */
/**************************************************************************/

static void ParseCommandLine( argc, argv )
int    argc;
char **argv;
{
    register char *c;
    register int   fmode = 0;
    register int   idx;
    register FILE *fd;
	     char  buf[200];

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
	  /* Turn off MinSlice throttle					  */
	  /* ------------------------------------------------------------ */
	case 'm':
	  MinSliceThrottle = FALSE;
	  break;

	  /* ------------------------------------------------------------ */
	  /* Apply dynamic patch					  */
	  /* ------------------------------------------------------------ */
	case 'p':
	  if ( *c ) AddPatch(atoi(c));
	  break;

	  /* ------------------------------------------------------------ */
	  /* Insert source line comments in code			  */
	  /* ------------------------------------------------------------ */
	case '%':
	  CodeComments = TRUE;
	  break;

	case '#':
	  nobrec = TRUE;
	  break;

	case '=':
	  share = FALSE;
	  break;

	case 'K':
	  SPRINTF( buf, "%s.f", c+1 );

	  if ( (hyfd = fopen( buf, "w" )) == NULL )
	    Error2( "CAN'T OPEN", buf );

	  hybrid = CopyString( c+1 );

	  FPRINTF( hyfd, "       subroutine %7.7s()\n", hybrid );
	  FPRINTF( hyfd, "       return\n" );
	  FPRINTF( hyfd, "       end\n\n" );
	  break;

	case '@':
	  flop[++flopcnt] = LowerCase( argv[++idx], FALSE, FALSE );
	  break;

	case '&':
	  if ( c[1] == '&' ) {
	    timeall = TRUE;
	  } else {
	    timelist[++timecnt] = LowerCase( argv[++idx], FALSE, FALSE );
	  }
	  break;

	case 'T':
	  if ( c[1] == 'T' ) {
	    traceall = TRUE;
	  } else {
	    trace[++tracecnt] = LowerCase( argv[++idx], FALSE, FALSE );
	  }
	  break;

	case 'P':
	  bip = FALSE;
	  break;

	case 'W':
	  prof = TRUE;
	  break;

	case 'U':
	  intrinsics = TRUE;
	  break;

	case '1':
	  rag = FALSE;
	  break;

	case '0':
	  bipmv = FALSE;
	  break;

	case '9':
	  freeall = TRUE;
	  break;

	case '4':
	  bindtosisal = TRUE;
	  break;

	case '8':
	  SISdebug = TRUE;
	  break;

	case '5':
	  fva = TRUE;
	  break;

	case '7':
	  nltss = TRUE;
	  break;

	case '6':
	  fvc = TRUE;
	  break;

	case 'y':
	  assoc = FALSE;
	  break;

	case '3':
	  if ( *(c+1) == 'N' )
	    newchains = TRUE;

	  xmpchains = TRUE;
	  break;

	case '2':
	  movereads = TRUE;
	  break;

	case 'O':
	  oruntime = TRUE;
	  break;

	case '\0':
	  fmode++;
	  break;

	case 'x':
	  useF = TRUE;
	  break;

	case 'X':
	  vec = TRUE;
	  break;

	case '{':
	  sdbx = TRUE;
	  bounds = TRUE;
	  break;

	case 'B':
	  bounds = TRUE;
	  break;

	case 'G':
	  gshared = TRUE;
	  break;

	case 'Y':
	  max_dims = atoi( c+1 );
	  break;

	case 'C':
	  cRay = TRUE;
	  break;

	case 'd':
	  dbl = TRUE;
	  break;

	case 't':
	  if3show = TRUE;
	  break;

	case 'u':
	  if2opt = FALSE;
	  break;

	case 'f':
	  flt = TRUE;
	  break;

	case 'r':
	  regok = FALSE;
	  break;

	case 'A':
	  alliantfx = TRUE;
	  break;

	case 'a':
	  aimp = FALSE;
	  break;

	case 'i':
	  info = TRUE;
	  if ( isdigit((int)(c[1])) ) info=atoi(c+1);
	  break;

       case 'F' :
          strcpy (infofile1, c+1);
          break;

	default:
	  Error2( "ILLEGAL ARGUMENT", --c );
	}
        }

  if ( sdbx || bounds ) {
    max_dims = 0; /* DISABLE PREBUILDING */
    bip = FALSE;
    aimp = FALSE;
    }
}


/**************************************************************************/
/* LOCAL  **************    CheckRefCountOps       ************************/
/**************************************************************************/
/* PURPOSE: MAKE SURE REFERENCE COUNT OPERATIONS ARE CONSISTENT AND       */
/*          CONSTANT PROPAGATION TOOK PLACE.                              */
/**************************************************************************/

static void CheckRefCountOps( g )
PNODE g;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE ee;
  register PNODE sg;
  register int   eport;
  register int   sr;
  register int   pm;
  register PEDGE i;

  for ( n = g; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( i = n->imp; i != NULL; i = i->isucc )
	if ( IsConst( i ) )
	  Error2( "CheckRefCountOps", "CONSTANT NOT PROPAGATED" );

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	CheckRefCountOps( sg );
      }

    for ( e = n->exp; e != NULL; e = e->esucc ) {
      eport = e->eport;

      if ( eport <= 0 ) {
	e->eport = abs( eport );
	continue;
	}

      sr = e->sr;
      pm = e->pm;

      if ( pm < 0 )
	Error2( "CheckRefCountOps", "pm < 0" );

      if ( sr < 0 )
	Error2( "CheckRefCountOps", "sr < 0" );

      if ( sr > 0 &&  pm > 0 )
	Error2( "CheckRefCountOps", "sr > 0 AND pm > 0" );

      /* VERIFY sr AND pm VALUES FOR eport */
      for ( ee = e; ee != NULL; ee = ee->esucc ) {
	if ( ee->eport != eport )
	  continue;

	if ( ee->pm != pm )
	  Error2( "CheckRefCountOps", "ee->pm != pm" );

	if ( ee->sr != sr )
	  Error2( "CheckRefCountOps", "ee->sr != sr" );
	}

      for ( ee = e; ee != NULL; ee = ee->esucc ) {
	if ( ee->eport != eport )
	  continue;

      /* NOT IF e IS THE LAST EDGE IN THE LIST OR FIRST EDGE IN THE SEQUENCE */
	if ( ee != e && e->esucc != NULL )
	  ee->eport = -(ee->eport);
        }
      }
    }
}


/**************************************************************************/
/* LOCAL  **************      BindCallNames        ************************/
/**************************************************************************/
/* PURPOSE: ADJUST THE CALL NAMES FOR GRAPH g.                            */
/**************************************************************************/

static void BindCallNames( g )
PNODE g;
{
  register PNODE  n;
  register PNODE  sg;
  register PNODE  f;
  register char  *s;
  register int    i;

  /* MARK FUNCTIONS THAT REQUIRE FLOP DUMP CODE */
  if ( !IsSGraph( g ) && g->G_NAME != NULL && flopcnt > -1 ) {
    s = LowerCase( g->G_NAME, FALSE, FALSE );

    for ( i = 0; i <= flopcnt; i++ )
      if ( strcmp( s, flop[i] ) == 0 )
	break;

    if ( i <= flopcnt )
      g->flp = TRUE;

    /* free( s ); */
    }

  /* MARK FUNCTIONS THAT REQUIRE TIMING CODE */
  if ( !IsSGraph( g ) && g->G_NAME != NULL && (timeall || (timecnt > -1))) {
    s = LowerCase( g->G_NAME, FALSE, FALSE );

    if ( timeall ) {
      g->time = TRUE;
    } else {
      for ( i = 0; i <= timecnt; i++ )
	if ( strcmp( s, timelist[i] ) == 0 )
	  break;

      if ( i <= timecnt )
	g->time = TRUE;

      /* free( s ); */
    }
  }

  /* MARK FUNCTIONS THAT REQUIRE TRACING CODE */
  if ( !IsSGraph( g ) && g->G_NAME != NULL && (traceall || (tracecnt > -1))) {
    s = LowerCase( g->G_NAME, FALSE, FALSE );

    if ( traceall ) {
      g->trace = TRUE;
    } else {
      for ( i = 0; i <= tracecnt; i++ )
	if ( strcmp( s, trace[i] ) == 0 )
	  break;

      if ( i <= tracecnt )
	g->trace = TRUE;

      /* free( s ); */
    }
  }

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	BindCallNames( sg );

    if ( !IsCall( n ) )
      continue;

    f = FindFunction( n->imp->CoNsT );

    n->imp->CoNsT = BindInterfaceName( n->imp->CoNsT, 
				       GetLanguage( f ), f->mark );
    }
}


/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: GENERATE C VERSION OF IF2 INPUT PROGRAM.  THE INPUT MUST BE   */
/*          MONOLITHIC, DATA FLOW ORDERED, NORMALIZED, AND NOT HAVE       */
/*          OFFSETS ASSIGNED---DONE BY if1offset (LLNL). FURTHER AT LEAST */
/*          MININAL BUILD-IN-PLACE AND UPDATE-IN-PLACE ANALYSIS MUST HAVE */
/*          BEEN DONE.                                                    */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
  register FILE  *fd;
  register PNODE  f;
  register char  *s;

  /* fix by dj raymond 25 nov 2000 */
  input = stdin;
  output = stdout;

/*  int i5 = I_Info5; */

  ParseCommandLine( argc, argv );

  if (RequestInfo(I_Info1, info))  
	if((infoptr1 = fopen(infofile1, "a")) == NULL)
		infoptr1 = stderr;

  if (RequestInfo(I_Info2, info)) { 
        strncpy (infofile2,infofile1, strlen(infofile1) - 1);
	strcat (infofile2, "2");
	if((infoptr2 = fopen(infofile2, "a")) == NULL)
		infoptr2 = stderr;
  }

  if (RequestInfo(I_Info3, info)) { 
        strncpy (infofile3,infofile1, strlen(infofile1) - 1);
	strcat (infofile3, "3");
	if((infoptr3 = fopen(infofile3, "a")) == NULL)
		infoptr3 = stderr;
  }

  if (RequestInfo(I_Info4, info)) { 
        strncpy (infofile4,infofile1, strlen(infofile1) - 1);
	strcat (infofile4, "4");
	if((infoptr4 = fopen(infofile4, "a")) == NULL)
		infoptr4 = stderr;
  }

  StartProfiler();
  If2Read();
  StopProfiler( "If2Read" );
  (void)fclose( input );	/* AS IT MAY BE THE OUTPUT FILE */

  if ( !IsStamp( DFORDERED ) )
	Error1( "IF2 INPUT IS NOT DFOrdered" );

  if ( IsStamp( OFFSETS ) )
    Error1( "HELP! OFFSETS ASSIGNED" );

  if ( !IsStamp( NORMALIZED ) )
    Error1( "NORMALIZATION REQUIRED" );

  if ( !IsStamp( BUILDINPLACE ) )
    Error1( "MINIMAL BUILD-IN-PLACE REQUIRED" );

  /* if ( !IsStamp( MONOLITH ) ) */ /* NEW CANN 2/92 */
    /* Error1( "MONOLITHIC INPUT REQUIRED" ); */

  if ( !IsStamp( UPDATEINPLACE ) )
    Error1( "MINIMAL UPDATE-IN-PLACE REQUIRED" );

  /* NEW CANN 2/92 */
  if ( !IsStamp( QMODE ) )
    Error1( "QMODE REQUIRED: RERUN THROUGH if1ld" );

  s = GetStampString( QMODE );

  while ( *s == ' ' || *s == '\t' ) s++;

  if ( *s == 'U' ) {
    if ( *(s+1) == 'R' )
      IunderR = TRUE;
    else if ( *(s+1) == 'L' )
      IunderL = TRUE;
    else if ( *(s+1) == 'N' ) {
      IunderR = FALSE;
      IunderL = FALSE;
      }
    else
      Iupper = TRUE;
    }
  else
    Error2( "ILLEGAL QMODE", s );
  /* END NEW CANN 2/92 */

  /* OPEN THE C CODE OUTPUT FILE */

  if ( ofile != NULL ) {
    if ( (fd = fopen( ofile, "w" )) == NULL )
      Error2( "CAN'T OPEN", ofile );

    output = fd;
    }

  StartProfiler();

  /* MAKE THE SPECIAL POINTER TYPE SYMBOL TABLE ENTIRES AND FIX NAMES */
  MakePtrTypes();

  MarkRecursiveFunctions();

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) BindCallNames( f );

  /* ------------------------------------------------------------ */
  /* First bind all names and then run the graph preparation.  If */
  /* you don't, then co-recursive routines cause if2gen to quit.  */
  /* ------------------------------------------------------------ */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    f->G_NAME = BindInterfaceName( f->G_NAME, GetLanguage( f ), f->mark );
  }
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) PrepareGraph( f );


  /* NEW CANN 2/92 */
  for ( standalone = TRUE, f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    /* CALLED FROM THE OPERATING SYSTEM? */
    if ( f->mark == 's' || f->mark == 'c' || f->mark == 'f' )
      standalone = FALSE;
    }

  /* FURTHER OPTIMIZE THE IF2 GRAPHS */
  If2Prebuild0();
  If2Opt();
  If2AImp();

  /* PULL PARALLEL CONSTRUCTS INTO THEIR OWN GRAPHS AND PUSH NODES AND */
  /* DO POINTER SWAP TRANSFORMATIONS.                                  */
  If2Vectorize( useF );

  If2Prebuild1();
  If2Yank0();
  If2Prebuild2();
  If2Yank1();

  /* MAKE ALL ENTRIES IN THE SYMBOL TABLE UNIQUE */
  GenSmashTypes();

  CheckParallelFunctions(); /* NEW CANN FOR MODULE DATA BASE 2/92 */

  StopProfiler( "GraphPreparations..." );

  /* GENERATE THE C CODE */

  StartProfiler();

  /* ------------------------------------------------------------ */
  /* Print header, forward functions, and types			  */
  /* ------------------------------------------------------------ */
  PrintFilePrologue();

  /* ------------------------------------------------------------ */
  /* Print out each of the function bodies (including SliceBodies)*/
  /* ------------------------------------------------------------ */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    /* FINAL CHECK THAT EVERYTHING IS GO FOR C GENERATION FOR f */
    sequential = !(f->Pmark);

    CheckRefCountOps( f );

    InitializeSymbolTable();

    AssignTemps( f );

    if ( sdbx )
      BuildAndPrintSdbxScope( f );

    /* ------------------------------------------------------------ */
    /* 1. Open function with args and temps			    */
    /* 2. Executable statements					    */
    /* 3. Close function					    */
    /* ------------------------------------------------------------ */
    PrintFunctPrologue( f );
    PrintGraph( 2, f );
    PrintFunctEpilogue( f );
    }

  /* ------------------------------------------------------------ */
  /* The Epilogue contains startup and I/O routines		  */
  /* ------------------------------------------------------------ */
  PrintFileEpilogue();

  StopProfiler( "Temp Assignment and C Code Generation" );

  if ( RequestInfo(I_Info1, info)) 
    WriteIf2AImpInfo2();

  if ( RequestInfo(I_Info2, info)) 
    WritePrebuildInfo();

  if ( RequestInfo(I_Info3, info)) 
    WriteIf2OptInfo();

  if ( RequestInfo(I_Info4, info)) {
    WriteIf2OptInfo2();
    WriteYankInfo();
     }

/*  if ( RequestInfo(I_Info5, info)) {
    WriteVectorInfo();
    WriteIf2OptInfo();
    WritePrebuildInfo();
    WriteIf2AImpInfo();
    WriteInterfaceInfo();
    WriteYankInfo();
  } */

  if (RequestInfo(I_Info1, info) && infoptr1!=stderr)
      fclose (infoptr1);
  if (RequestInfo(I_Info2, info) && infoptr2!=stderr)
      fclose (infoptr2);
  if (RequestInfo(I_Info3, info) && infoptr3!=stderr)
      fclose (infoptr3);
  if (RequestInfo(I_Info4, info) && infoptr4!=stderr)
      fclose (infoptr4);

  if ( if3show ) {
      output = stdout;
      If2Write();
  }

  Stop( OK );
}
