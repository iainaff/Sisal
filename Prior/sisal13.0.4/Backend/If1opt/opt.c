/* opt.c,v
 * Revision 12.7  1992/11/04  22:04:59  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:39  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

FILE *input;               /* IF1 INPUT  FILE POINTER            */
FILE *output;              /* IF1 OUTPUT FILE POINTER            */
FILE *infoptr;              /* IF1 INFO OUTPUT FILE POINTER        */
char infofile[200];

char *program = "if1opt";           /* PROGRAM NAME                       */

static int norm    = TRUE;          /* PERFORM GRAPH NORMALIZATION?       */
static int fission = TRUE;          /* PERFORM RECORD AND ARRAY FISSION?  */
static int cascade = TRUE;	    /* Look for loop test cascades	  */
static int invar   = TRUE;          /* PERFORM LOOP INVARIANT REMOVAL?    */
       int Oinvar  = TRUE;
static int cse     = TRUE;          /* PERFORM CSE?                       */
static int gcse    = TRUE;          /* PERFORM GLOBAL CSE?                */
       int tgcse   = TRUE;          /* TRY AND FORCE GCSE IMPORVEMENTS?   */
static int fold    = TRUE;          /* PERFORM CONSTANT FOLDING?          */
static int dead    = TRUE;          /* PERFORM DEAD CODE REMOVAL?         */
static int InLineExpansion  = TRUE;	 /* PERFORM INLINE EXPANSION? */
       int intrinsics = FALSE;		/* Fold constants in math intrinsics */
       int inlineall = FALSE;         /* INLINE EVERYTHING BUT REQUESTS?     */
       int inter     = FALSE;         /* INLINE EXPAND IN INTERACTIVE MODE?  */
       int AggressiveVectors = FALSE; /* Aggressive fusion of vector loops */


       int explode  = FALSE;         /* EXPLODE FORALL NODES?                */
static int explodeI = FALSE;         /* EXPLODE ONLY INNERMOST FORALL NODES? */


static int strip   = TRUE;          /* DO RETURN NODE STRIPPING?          */
       int dope    = TRUE;          /* PEFORM DOPE VECTOR OPTIMIZATIONS   */
       int amove   = TRUE;          /* PERFORM ANTI-MOVEMENT              */
       int asetl   = FALSE;         /* REMOVE UNNECESSARY ASetL NODES?    */
       int ifuse   = TRUE;          /* PERFORM FORALL FUSION?             */
       int sfuse   = TRUE;          /* PERFORM SELECT FUSION?             */
       int dfuse   = TRUE;          /* PERFORM FORALL DEPENDENT FUSION?   */
       int info	   = FALSE;	    /* DUMP OPTIMIZATION INFORMATION?     */
       int sgnok   = TRUE;          /* ALLOW SIGNED ARITHMETIC CONSTANTS? */
       int slfis   = TRUE;          /* PERFORM STREAM LOOP FISSION?       */
       int native  = FALSE;         /* FLAG NODES NOT SUPPORTED IN NATIVE */
       int split   = TRUE;          /* PERFORM FORALL SPLITTING?          */
       int invert  = TRUE;          /* PERFORM LOOP INVERSION?            */
       int vec     = FALSE;         /* VECTOR MODE?                       */
       int concur  = FALSE;         /* CONCURRENT MODE?                   */
       int cRay    = FALSE;         /* COMPILING FOR THE CRAY?            */
       int alliantfx = FALSE;       /* COMPILING FOR THE ALLIANT?         */
       int agcse   = FALSE;         /* DO ANTI-GLOBAL CSE?                */

       int DeBuG   = FALSE;         /* OPT FOR PROGRAM DEBUGGING?         */
       int noassoc = FALSE;         /* DISABLE ASSOCIATIVE TRANSFORMS     */

       int normidx = TRUE;        /* NORMALIZE ARRAY INDEXING OPERATIONS? */
       int fchange = TRUE;        /* WAS A FUSION DONE?                   */

       int glue    = FALSE;      /* ELIMINATE DEAD FUNCTION CALLS?        */

       int prof    = FALSE;      /* PROFILE THE OPTIMIZER?                */

static char  istmp[100];     /* CSU-IF1 OPTIMIZATION STAMP COMMENT STRING */
static char *ofile = NULL;   /* NAME OF THE OUTPUT FILE                   */


/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF A FILE.  THE FIRST  */
/*          ENCOUNTERED FILE IS FOR IF1 INPUT.  THE SECOND ENCOUNTERED    */
/*          FILE IS FOR IF1 OUTPUT.  ANY OTHER FILES ON THE COMMAND LINE  */
/*          CAUSE AN ERROR MESSAGE TO BE PRINTED.                         */
/*                                                                        */
/*	    FILES:							  */
/*		0:	-> IF1 monolith					  */
/*		1:	-> Optimized IF1				  */
/*									  */
/*          OPTIONS:							  */
/*		-	-> Skip a standard file (in, out, etc..)	  */

/*		-a	-> DO ONLY NORMALIZATION AND DEAD CODE REMOVAL	  */
/*		-c	-> NO CSE					  */
/*		-d	-> NO DEAD CODE REMOVAL				  */
/*		-e	-> FLAG NODES ONLY SUPPORTED BY DI		  */
/*		-f	-> NO CONSTANT FOLDING				  */
/*		-g	-> NO GLOBAL CSE				  */
/*		-j	-> Do not do select loop fusion			  */
/*		-i	-> GENERATE LISTING OF GATHERED INFO		  */
/*		-i<num>	-> Generate more listing of gathered info	  */
/*		-l	-> ENABLE SETL REMOVAL				  */
/*		-n	-> NO NORMALIZATION				  */
/*		-p<num>	-> Apply dynamic patch <num>			  */
/*		-r	-> NO RECORD FISSION				  */
/*		-s	-> SIGNED CONSTANTS AREN'T ALLOWED		  */
/*		-t	-> DO INTERACTIVE EXPANSION			  */
/*		-u	-> NO FORALL FUSION				  */
/*		-v	-> NO INVARIANT MOVEMENT			  */
/*		-w	-> Supress warnings messages			  */
/*		-x	-> NO EXPANSION					  */
/*		-y	-> NO STREAM LOOP FISSION			  */
/*		-z	-> Do not do dependent loop fusion		  */

/*		-A	-> COMPILING FOR THE ALLIANT			  */
/*		-AggV	-> DO AGGRESSIVE FUSION of VECTOR LOOPS		  */
/*		-C	-> COMPILING FOR THE CRAY XMP			  */
/*		-D	-> Don't perform dope vector optimizations	  */
/*		-G	-> DO ANTI-GLOBAL CSE				  */
/*		-I	-> Generate vector information listing		  */
/*		-I<num>	-> Generate more vector information listing	  */
/*		-M	-> Fold constants in math intrinsics		  */
/*		-N	-> DON'T NORMALIZE ARRAY INDEXING		  */
/*		-R	-> Compiling for concurrent computation		  */
/*		-S	-> NO FORALL SPLITTING				  */
/*		-T	-> Explode forall nodes				  */
/*		-U#	-> FORALL ITERATION UNROLLING VALUE		  */
/*		-V	-> Do not remove invariants in outer level loops  */
/*		-W	-> Do profiling					  */
/*		-X	-> Vectorize					  */
/*		-Y	-> Allow constant folding and assoc. transforms.  */
/*		-Z	-> Do not perform loop inversion optimization	  */

/*		-1	-> DON'T FORCE GCSE IMPORVEMENTS		  */
/*		-3	-> Turn on debugging				  */
/*		-6	-> Strip return nodes				  */
/*		-8	-> Do not eliminate dead function calls		  */
/*		-9	-> Do not perform anti-movement			  */

/*		-@#	-> Assumed iteration count			  */
/*		-+	-> Explode all forall loops			  */
/*		-$	-> Inline all (nonrecursive) functions		  */
/*		-# f	-> DON'T INLINE FUNCTION f			  */
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

	case 'W':
	  prof = TRUE;
	  break;

	case 'x':
	  InLineExpansion = FALSE;
	  break;

	case '#':
	  noin[++noincnt] = LowerCase( argv[++idx], FALSE, FALSE );
	  break;

	case '6':
	  strip = FALSE;
	  break;

	case 'N':
	  normidx = FALSE;
	  break;

	case 'M':
	  intrinsics = TRUE;
	  break;

	case '1':
	  tgcse = FALSE;
	  break;

	case '\0':
	  fmode++;
	  break;

	case 'S':
	  split = FALSE;
	  break;

	case 'Z':
	  invert = FALSE;
	  break;

	case 'V':
	  Oinvar = FALSE;
	  break;

	case 'e':
	  native = TRUE;
	  break;

	case 'D':
	  dope = FALSE;
	  break;

	case 'a':
	  maxunroll = 0;
	  fission = FALSE;
	  invar   = FALSE;
	  Oinvar  = FALSE;
	  cse     = FALSE;
	  gcse    = FALSE;
	  tgcse   = FALSE;
	  fold    = FALSE;
	  strip   = FALSE;
	  dope    = FALSE;
	  ifuse   = FALSE;
	  sfuse   = FALSE;
	  dfuse   = FALSE;
	  split   = FALSE;
	  invert  = FALSE;
	  normidx = FALSE;
	  break;

	case 'Y':
	  fold  = FALSE;
	  noassoc = TRUE;
	  break;

	case 'y':
	  slfis = FALSE;
	  break;

	case 'n':
	  norm = FALSE;
	  break;

	case 'l':
	  asetl = TRUE;
	  break;

	case 'A':
	  if ( strcmp(c,"AggV") == 0 )  {
	    AggressiveVectors = TRUE;
	  } else {
	    alliantfx = TRUE;
	  }
	  break;

	case 'C':
	  cRay = TRUE;
	  break;

	case 'X':
	  vec = TRUE;
	  break;

	case 'R':
	  concur = TRUE;
	  break;

	case 'U':
	  if ( maxunroll == 0 )
	    break;

	  maxunroll = atoi( c + 1 );

	  if ( maxunroll < 0 )
	    maxunroll = 0;

	  break;

	case 'r':
	  fission = FALSE;
	  break;

	case 'v':
	  invar = FALSE;
	  break;

	case 'c':
	  cse = FALSE;
	  break;

	case 'G':
	  agcse = TRUE;
	  break;

	case 'g':
	  gcse = FALSE;
	  break;

	case 'f':
	  fold = FALSE;
	  break;

	case '8':
	  glue = TRUE;
	  break;

	case 'd':
	  dead = FALSE;
	  break;

	case 'j':
	  sfuse = FALSE;
	  break;

	case 'u':
	  ifuse = FALSE;
	  break;

	case 'z':
	  dfuse = FALSE;
	  break;

	case '3':
	  DeBuG = TRUE;
	  break;

	case '9':
	  amove = FALSE;
	  break;

	case '@':
	  iter = (double) atoi( (c+1) );
	  break;

	case 'i':
	  info = TRUE;
	  if ( isdigit((int)(c[1])) ) info=atoi(c+1);
	  break;

        case 'F' :
          strcpy (infofile, c+1);
	  break;

	case '+':
	  explode  = TRUE;
	  explodeI = TRUE;
	  break;

	case 'T':
	  explode = TRUE;
	  break;

	case '$':
	  inlineall = TRUE;
	  break;

	case 't':
	  inter = TRUE;
	  break;

	case 's':
	  sgnok = FALSE;
	  break;

	default:
	  Error2( "ILLEGAL ARGUMENT", --c );
	}
        }
}


/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: IMPROVE THE QUALITY OF THE IF1 INPUT FILE BY PERFORMING THE   */
/*          FOLLOWING OPTIMIZATIONS: GRAPH NORMALIZATION, GRAPH EXPANSION,*/
/*          INVARIANT REMOVAL, RECORD FISSION, CSE, GLOBAL CSE, CONSTANT  */
/*          FOLDING, AND DEAD CODE REMOVAL. IF THE INPUT IS NOT DATA FLOW */
/*          ORDERED OR CONTAINS NoOp NODES (INSERTED BY THE LLNL UTILITY  */
/*          if1offset), AN ERROR MESSAGE IS PRINTED AND EXECUTION STOPS.  */
/*          IF GRAPH NORMALIZATION, LOOP FISSION, AND DEAD CODE REMOVAL   */
/*          ARE PERFORMED, A 'N' STAMP IS ADDED TO THE OUTPUT. THE 'I'    */
/*          STAMP IS ADDED TO DOCUMENT OTHER PERFORMED OPTIMIZATIONS.     */
/*          NOTE, IF THE INPUT FILE NAME IS THE SAME AS THE OUTPUT FILE   */
/*          NAME, THE CONTENTS OF THE INPUT FILE IS OVERWRITTEN.          */
/**************************************************************************/

void main( argc, argv )
int    argc;
char **argv;
{
  register FILE *fd;
  register int   i;
  register int   c;

  /* fix by dj raymond 25 nov 2000 */
  input = stdin;
  output = stdout;

  ParseCommandLine( argc, argv );

  if ( DeBuG ) {
    InLineExpansion = fission = split = fold = vec = concur = FALSE;
    inlineall = invert = dfuse = ifuse = gcse = dope = FALSE;
    strip = explode = explodeI = cse = invar = FALSE;
    maxunroll = 0;
    }

  if (RequestInfo(I_Info1, info)) 
	if ((infoptr = fopen (infofile, "a")) == NULL)
		infoptr = stderr;

  StartProfiler();
  If1Read();
  (void)fclose( input );       /* AS IT MIGHT BE THE SOON TO BE WRITTEN FILE */
  StopProfiler( "If1Read" );

  if ( !IsStamp( DFORDERED ) )
    Error1( "IF1 INPUT IS NOT DFOrdered" );

  if ( IsStamp( OFFSETS ) )
    Error1( "OFFSETS ASSIGNED---NoOp NODES NOT IMPLEMENTED" );


  /* START BY CLEANING EVERYTHING UP! */
  StartProfiler();
  if ( dead ) If1Clean();
  StopProfiler( "If1Clean" );

  StartProfiler();
  If1Reduce();
  StopProfiler( "If1Reduce" );

  StartProfiler();
  if ( norm ) If1Normalize();
  StopProfiler( "If1Normalize" );

/*  if ( RequestInfo(I_Info1,info) ) {
    FPRINTF( infoptr, "\n****GRAPH NORMALIZATION AND EXPANSION\n" );
    If1Count( "BEFORE EXPANSION AFTER NORMALIZATION" );
    } */

  /* DO IT NOW SO LOOP FUSION DOES NOT UNDO STREAM LOOP FISSION */

  if ( norm && dead && native && sgnok ) 
    AddStamp( NORMALIZED, "  CSU -> NORMALIZED: native, dead" );

  if ( RequestInfo(I_Info1,info) ) 
    FPRINTF( infoptr, "**** SIMPLE OPTIMIZATIONS\n\n" );

  StartProfiler();
  if ( InLineExpansion ) If1Inline();
  CallReorder();
  StopProfiler( "If1Inline" );

  /* ------------------------------------------------------------ */
  /* Give ID numbers to all compound nodes for later reference */
  AssignCompoundIDs();

  if ( RequestInfo(I_Info1,info) ) {
    /*If1Count( "AFTER GRAPH NORMALIZATION AND EXPANSION" );*/
  }

/*  if ( RequestInfo(I_Info1,info)) {
    WriteLoopMap( "BEFORE FUSION OPTIMIZATIONS" );
    (void)fputc('\n',infoptr);
  } */

  /* INITIALIZE DEAD NODE COUNTERS SO NOT TO REFLECT ACTION TAKEN DURING */
  /* NORMALIZATION AND GRAPH EXPANSION                                   */
  ikcnt = unnec = unused = dscnt = dccnt = agcnt = 0;

  /* OK, CLEAN EVERYTHING AGAIN! */
  StartProfiler();
  if ( dead )
    If1Clean();
  StopProfiler( "If1Clean" );


  /* DO EVERYTHING 6 TIMES */
  for ( i = 0; i <= 5; i++ ) {
    StartProfiler();
    if ( fission ) If1Fission();
    StopProfiler( "If1Fission" );

    StartProfiler();
    if ( split ) If1Split();
    StopProfiler( "If1Split" );

    StartProfiler();
    if ( fold ) If1Fold();
    StopProfiler( "If1Fold" );

    StartProfiler();
    if ( invar ) If1Invar();
    StopProfiler( "If1Invar" );

    StartProfiler();
    if ( vec && i == 2 ) If1Vec();
    StopProfiler( "If1Vec" );

    StartProfiler();
    if ( (vec || concur) && i == 2 ) If1Par();
    StopProfiler( "If1Par" );

    StartProfiler();
    if ( invert && i != 0 ) If1Invert();
    StopProfiler( "If1Invert" );

    /* DO CSE AND FUSION UNTIL NO MORE CHANGES */
    fchange = TRUE;
    c = 0;
    while ( fchange ) {
      c++;

      StartProfiler();
      If1Cse( cse, strip && (c == 1) && (i == 4) );
      StopProfiler( "If1Cse" );

      fchange = FALSE;

      StartProfiler();
      if ( dfuse && i == 4 ) If1DFusion();
      StopProfiler( "If1DFusion" );

      StartProfiler();
      if ( ifuse && i == 4 ) If1IFusion();
      StopProfiler( "If1IFusion" );
      }

    StartProfiler();
    if ( gcse ) If1GCse();
    StopProfiler( "If1GCse" );

    StartProfiler();
    if ( dope && i == 1 ) If1Dope();
    StopProfiler( "If1Dope" );

    StartProfiler();
    if ( maxunroll > 0 && i == 2 ) If1Unroll();
    StopProfiler( "If1Unroll" );

    StartProfiler();
    if ( explode && i == 3 ) If1Explode( explodeI );
    StopProfiler( "If1Explode" );

    /* Test cascadence */
    StartProfiler();
    if ( cascade ) If1TestCascade();
    StopProfiler( "If1TestCascade" );

    StartProfiler();
    if ( dead ) If1Clean();
    StopProfiler( "If1Clean" );
    }

  if ( RequestInfo(I_Info1,info) ) {
    /*WriteLoopMap( "AFTER FUSION OPTIMIZATIONS" );*/
    WriteFusionInfo(); 
    WriteReduceInfo(); 
    WriteFissionInfo(); 
    WriteUnrollInfo();
    WriteSplitInfo();
   /* WriteFoldInfo();
    WriteInvarInfo();
    WriteCseInfo();
    WriteGCseInfo();*/
    WriteConcurInfo();
    WriteInvertInfo();
    WriteDopeInfo();
    WriteExplodeInfo();
    WriteCleanInfo();  

    /*If1Count( "AFTER MACHINE INDEPENDENT OPTIMIZATIONS" );*/
    }

  /*if ( RequestInfo(I_Info1,info) )
    WriteLoopMap( "AFTER FUSION OPTIMIZATIONS" );*/

  SPRINTF( istmp, "  CSU -> IF1 OPTS:%s%s%s%s%s%s%s%s%s%s",
           ( InLineExpansion)?" inline"  : "", ( invar )? " invar" : "",
           ( fission )?       " fiss"    : "", ( cse )?   " cse"   : "",
           ( ifuse && cse )?  " fuse"    : "", ( gcse )?  " gcse"  : "",
           ( asetl && norm )? " asetl"   : "", ( fold )?  " fold"  : "",
           ( sgnok )?         " sgnok"   : "", ( slfis )? " sfiss" : ""  );

  AddStamp( IF1OPTIMIZED,  istmp );
  if (RequestInfo(I_Info1, info) && infoptr!=stderr)
    fclose (infoptr);

  /* OPEN THE OUTPUT FILE AND WRITE THE OPTIMIZED PROGRAM               */

  if ( ofile != NULL ) {
    if ( (fd = fopen( ofile, "w" )) == NULL )
      Error2( "CAN'T OPEN", ofile );

    output = fd;
    }

  StartProfiler();
  If1Write();
  StopProfiler( "If1Write" );

  Stop( OK );
}
