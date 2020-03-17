/**************************************************************************/
/* FILE   **************            ld.c           ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "world.h"


#define MAX_TABLE_ENTRIES (400)

FILE *input;                    /* IF1 INPUT  FILE POINTER */
FILE *output;                   /* IF1 OUTPUT FILE POINTER */

char *program   = "if1ld";              /* PROGRAM NAME */

char *entryt[MAX_TABLE_ENTRIES];        /* ENTRY POINT NAME TABLES */
int   entrys[MAX_TABLE_ENTRIES];
int   etop = -1;

char *fortt[MAX_TABLE_ENTRIES];         /* Fortran list */
int   ftop = -1;

char *ct[MAX_TABLE_ENTRIES];            /* C function list */
int   ctop = -1;

char *reduct[400];                      /* REDUCTION NAME TABLE */
int   rtop = -1;

char *furl = NULL;                      /* Q Stamp string to use */

char *libraries[MAX_TABLE_ENTRIES];     /* List of libraries to link in */
int  librarycount = 0;


int   pmodule  = FALSE;                 /* PROGRAM MODULE? */

int   smodule  = FALSE;                 /* SISAL MODULE? */
int   forF     = FALSE;                 /* PROGRAM MODULE TYPES */
int   forC     = FALSE;
int   monolith = FALSE;                 /* MONOLITHIC PROGRAM MODULE? */

int   prof     = FALSE;                 /* PROFILE THE OPTIMIZER? */

static char *ofile = NULL;              /* OUTPUT FILE NAME */

static char *archiver = "if1archive";   /* Archive pathname */
static int verbose    = 0;              /* Display actions */
static int PID;                         /* Unique ID for creating tmpfiles */

/**************************************************************************/
/* LOCAL  **************    PlaceInEntryTable      ************************/
/**************************************************************************/
/* PURPOSE: PLACE nm IN THE ENTRY NAME TABLE.                             */
/**************************************************************************/

void PlaceInEntryTable( nm )
char *nm;
{
  register int idx;

  /* CHECK IF ALREADY IN THE OTHER TABLES */
  for ( idx = 0; idx <= ftop; idx++ )
    if ( strcmp( nm, fortt[idx] ) == 0 )
      Error2( "OPTIONS -e AND -f CONFLICT FOR:", nm );

  for ( idx = 0; idx <= ctop; idx++ )
    if ( strcmp( nm, ct[idx] ) == 0 )
      Error2( "OPTIONS -e AND -c CONFLICT FOR:", nm );

  for ( idx = 0; idx <= etop; idx++ )
    if ( strcmp( nm, entryt[idx] ) == 0 )
      return;

  if ( ++etop > MAX_TABLE_ENTRIES )
    Error2( "PlaceInEntryTable", "entryt OVERFLOW" );

  entryt[etop] = nm;
  entrys[etop] = FALSE;
}


/**************************************************************************/
/* LOCAL  **************      PlaceInCTable        ************************/
/**************************************************************************/
/* PURPOSE: PLACE nm IN THE C NAME TABLE.                                 */
/**************************************************************************/

void   PlaceInCTable( nm )
char *nm;
{
  register int idx;

  /* CHECK IF ALREADY IN THE OTHER TABLES */
  for ( idx = 0; idx <= ftop; idx++ )
    if ( strcmp( nm, fortt[idx] ) == 0 )
      Error2( "OPTIONS -c AND -f CONFLICT FOR:", nm );

  for ( idx = 0; idx <= etop; idx++ )
    if ( strcmp( nm, entryt[idx] ) == 0 )
      Error2( "OPTIONS -c AND -e CONFLICT FOR:", nm );

  for ( idx = 0; idx <= ctop; idx++ )
    if ( strcmp( nm, ct[idx] ) == 0 )
      return;

  if ( ++ctop > MAX_TABLE_ENTRIES )
    Error2( "PlaceInCTable", "ct OVERFLOW" );

  ct[ctop] = nm;
}


/**************************************************************************/
/* LOCAL  **************    PlaceInFortranTable    ************************/
/**************************************************************************/
/* PURPOSE: PLACE nm IN THE FORTRAN NAME TABLE.                           */
/**************************************************************************/

void PlaceInFortranTable( nm )
char *nm;
{
  register int idx;

  /* CHECK IF ALREADY IN THE OTHER TABLES */
  for ( idx = 0; idx <= ctop; idx++ )
    if ( strcmp( nm, ct[idx] ) == 0 )
      Error2( "OPTIONS -f AND -c CONFLICT FOR:", nm );

  for ( idx = 0; idx <= etop; idx++ )
    if ( strcmp( nm, entryt[idx] ) == 0 )
      Error2( "OPTIONS -f AND -e CONFLICT FOR:", nm );

  for ( idx = 0; idx <= ftop; idx++ )
    if ( strcmp( nm, fortt[idx] ) == 0 )
      return;

  if ( ++ftop > MAX_TABLE_ENTRIES )
    Error2( "PlaceInFortranTable", "fortt OVERFLOW" );

  fortt[ftop] = nm;
}


/**************************************************************************/
/* LOCAL  **************    PlaceInReductionTable  ************************/
/**************************************************************************/
/* PURPOSE: PLACE nm IN THE REDUCTION NAME TABLE.                         */
/**************************************************************************/

void PlaceInReductionTable( nm )
char *nm;
{
  register int idx;

  /* CHECK IF ALREADY IN THE OTHER TABLES */
  for ( idx = 0; idx <= ftop; idx++ )
    if ( strcmp( nm, fortt[idx] ) == 0 )
      Error2( "OPTIONS -r AND -f CONFLICT FOR:", nm );

  for ( idx = 0; idx <= ctop; idx++ )
    if ( strcmp( nm, ct[idx] ) == 0 )
      Error2( "OPTIONS -r AND -c CONFLICT FOR:", nm );

  for ( idx = 0; idx <= rtop; idx++ )
    if ( strcmp( nm, reduct[idx] ) == 0 )
      return;

  if ( ++rtop > MAX_TABLE_ENTRIES )
    Error2( "PlaceInReductionTable", "reduct OVERFLOW" );

  reduct[rtop] = nm;
}


/**************************************************************************/
/* LOCAL  **************      ParseCommandLine     ************************/
/**************************************************************************/
/* PURPOSE: PARSE THE COMMAND LINE argv CONTAINING argc ENTRIES. THE 1ST  */
/*          ENTRY (argv[0]) IS IGNORED.   IF AN ARGUMENT DOES NOT BEGIN   */
/*          WITH A DASH, IT IS CONSIDERED THE NAME OF AN INPUT FILE. THE  */
/*          NAME OF EACH INPUT FILE IS STORED IN THE ARRAY files.         */
/*                                                                        */
/*          FILES:                                                        */
/*               0:             -> IF1 or Archive file                    */
/*               ...                                                      */
/*               n-1:           -> IF1 or Archive file                    */
/*                                                                        */
/*          OPTIONS:                                                      */
/*              -c funct        -> C INTERFACE FUNCTION                   */
/*              -e funct        -> PROGRAM ENTRY POINT                    */
/*              -f funct        -> FORTRAN INTERFACE FUNCTION             */
/*              -o outfile      -> WRITE MONOLITH TO outfile              */
/*              -r funct        -> REDUCTION INTERFACE FUNCTION           */
/*              -p<num>         -> Apply dynamic patch <num>              */
/*              -v              -> Verbose                                */
/*              -w              -> Supress warnings messages              */
/*              -A<path>        -> Archiver path                          */
/*              -C              -> Compiling for C                        */
/*              -F              -> Compiling for FORTRAN                  */
/*              -F<str>         -> QStamp entry                           */
/*              -S              -> Compiling for Sisal                    */
/*              -W              -> Turn on profiling                      */
/**************************************************************************/

static void ParseCommandLine( argc, argv )
int    argc;
char **argv;
{
    register char *c;
    register int   idx;

    for ( idx = 1; idx < argc; ++idx ) {
      if ( *(c = argv[ idx ]) == '-' )
        switch ( *( ++c ) ) {

          /* ------------------------------------------------------------ */
          /* Suppress warning messages                                    */
          /* ------------------------------------------------------------ */
        case 'w':
          Warnings = FALSE;
          break;

          /* ------------------------------------------------------------ */
          /* Apply dynamic patch                                          */
          /* ------------------------------------------------------------ */
        case 'p':
          if ( *c ) AddPatch(atoi(c));
          break;

          /* ------------------------------------------------------------ */
          /* C interface function                                         */
          /* ------------------------------------------------------------ */
        case 'c':               /* SEE ReadTheIf1Files!!! */
          if ( ++idx >= argc )
            Error1( "USAGE: -c function" );

          PlaceInCTable( LowerCase( argv[ idx ], FALSE, FALSE ) );
          break;

          /* ------------------------------------------------------------ */
          /* Program entry point                                          */
          /* ------------------------------------------------------------ */
        case 'e':               /* SEE ReadTheIf1Files!!! */
          if ( ++idx >= argc )
            Error1( "USAGE: -e function" );

          PlaceInEntryTable( LowerCase( argv[ idx ], FALSE, FALSE ) );
          break;

          /* ------------------------------------------------------------ */
          /* Fortran interface function                                   */
          /* ------------------------------------------------------------ */
        case 'f':               /* SEE ReadTheIf1Files!!! */
          if ( ++idx >= argc )
            Error1( "USAGE: -f function" );
          PlaceInFortranTable( LowerCase( argv[ idx ], FALSE, FALSE ) );
          break;

          /* ------------------------------------------------------------ */
          /* Output filename                                              */
          /* ------------------------------------------------------------ */
        case 'o':               /* SEE ReadTheIf1Files!!! */
          if ( ++idx >= argc )
            Error1( "USAGE: -o outfile" );

          ofile = argv[ idx ];
          break;

          /* ------------------------------------------------------------ */
          /* Reduction function                                           */
          /* ------------------------------------------------------------ */

        case 'r':               /* SEE ReadTheIf1Files!!! */
          if ( ++idx >= argc )
            Error1( "USAGE: -r function" );
          PlaceInReductionTable( LowerCase( argv[ idx ], FALSE, FALSE ) );
          break;

          /* ------------------------------------------------------------ */
          /* Verbose (list internal commands)                             */
          /* ------------------------------------------------------------ */
        case 'v':
          verbose = 1;
          break;

          /* ------------------------------------------------------------ */
          /* Archive program pathname                                     */
          /* ------------------------------------------------------------ */
        case 'A':
          archiver = c+1;
          break;

          /* ------------------------------------------------------------ */
          /* Compiling for C                                              */
          /* ------------------------------------------------------------ */
        case 'C':
          forC = TRUE;
          break;

          /* ------------------------------------------------------------ */
          /* Compiling for FORTRAN                                        */
          /* ------------------------------------------------------------ */
        case 'F':
          if ( *(c+1) != '\0' ) {
            furl = c+1;
            break;
          }

          forF = TRUE;
          break;

          /* ------------------------------------------------------------ */
          /* Compiling for SISAL */
          /* ------------------------------------------------------------ */
        case 'S':
          smodule = TRUE;
          break;

          /* ------------------------------------------------------------ */
          /* Display Profiling information                                */
          /* ------------------------------------------------------------ */
        case 'W':
          prof = TRUE;
          break;

        default:
          Error2( "ILLEGAL ARGUMENT", --c );
        }
    }

    pmodule = !smodule;

    if ( forF && forC ) Error1( "-forFORTRAN and -forC conflict" );

    if ( (forF || forC) && smodule ) { 
      Error1( "-forC or -forFORTRAN and -smodule conflict" );
    }

    if ( (!(forC || forF)) && !smodule ) {
      if ( etop > 0 ) {
        Error1( "only one entry point allowed in a program module" );
      }
    }

    /* GUT CHECKS! */
    if ( !(pmodule || smodule) ) {
      Error2( "ParseCommandLine", "NEITHER pmodule OR smodule ENCOUNTERED!!!");
    }

    if ( pmodule && smodule ) {
      Error2( "ParseCommandLine", "pmodule AND smodule ENCOUNTERED!!!" );
    }
}

/**************************************************************************/
/* LOCAL  **************       LoadIF1File         ************************/
/**************************************************************************/
/* PURPOSE:  Open an IF1 file and merge it into the growing monolith.     */
/*           The file to open is ``actual'', the name to use is ``name''  */
/*           These names can be the same, but may differ if a tempfile    */
/*           has been pulled out of an archive.                           */
/**************************************************************************/
static void LoadIF1File(actual,name)
     char       *actual,*name;
{
  input = fopen( actual, "r" );
  if ( !input ) Error2( "CAN'T OPEN", actual );

  AssignSourceFileName( name ); 
  PrepareForNextFile();

  MonoIf1Read();
  (void)fclose( input );
}

/**************************************************************************/
/* LOCAL  **************         ReadLibrary       ************************/
/**************************************************************************/
/* PURPOSE:  ReadLibrary attempts to satisfy unresolved imported functions*/
/*           by loading a file stored in the archive ``lib''.  The list of*/
/*           unresolved imports is found in ``Need''.                     */
/**************************************************************************/
static int ReadLibrary(lib,Need)
     char       *lib;
     namelink   *Need;
{
  FILE          *LIBF;
  char          line[MAX_PATH];
  char          havename[MAX_PATH];
  char          command[MAX_PATH];
  char          tmpfile[MAX_PATH];
  char          *p;
  int           stat;

  /* ------------------------------------------------------------ */
  /* Open the library                                             */
  /* ------------------------------------------------------------ */
  LIBF = fopen(lib,"r");
  if ( !LIBF ) Error2("Can't open ",lib);

  /* ------------------------------------------------------------ */
  /* Start reading header lines.  When you come to a file body,   */
  /* stop reading.  Check the SYMDEF line for items on the Need   */
  /* list.  If found, crack the .if1 file out of the archive,     */
  /* load it, delete the temporary, and return indicating success */
  /* ------------------------------------------------------------ */
  while (fgets(line,sizeof(line),LIBF)) {
    /* ------------------------------------------------------------ */
    /* Get the filename of the archive component                    */
    /* ------------------------------------------------------------ */
    if ( line[0] == '-' ) break;

    /* ------------------------------------------------------------ */
    /* Scan the Have list (SYMDEF's)                                */
    /* ------------------------------------------------------------ */
    do {
      fscanf(LIBF," %s",havename);

      /* ------------------------------------------------------------ */
      /* The symdef line ends with a starred entry                    */
      /* ------------------------------------------------------------ */
      if ( havename[0] == '*' ) {
        fgets(line,sizeof(line),LIBF);
        break;
      }

      /* ------------------------------------------------------------ */
      /* If we need what the component provides, load it              */
      /* ------------------------------------------------------------ */
      if ( InNameList(havename,Need) ) {
        for(p=line; *p; p++) if (*p=='\n') *p = '\0';

        sprintf(command,"%s %d x %s %s\n",archiver,PID,lib,line);
        if (verbose) fputs(command,stderr);
        stat = system(command);
        if ( stat ) Error2("Archiver command failed ",archiver);

        sprintf(tmpfile,"#tmp%d.if1",PID);
        LoadIF1File(tmpfile,line);

        stat = unlink(tmpfile);
        if (verbose) fprintf(stderr,"unlink %s\n",tmpfile);
        if ( stat ) {
          perror(tmpfile);
          Error2("Cannot delete ",tmpfile);
        }

        return 1;               /* A load was made */
      }
    } while (1);                /* Keep looking until the star entry */
  }

  return 0;                     /* No load was made */
}

/**************************************************************************/
/* LOCAL  **************       ReadTheIf1Files     ************************/
/**************************************************************************/
/* PURPOSE: READ ALL IF1 INPUT FILES LISTED IN THE ARRAY argv AND BUILD   */
/*          THE MONOLITH.  IF A FILE CANNOT BE OPENED, AN ERROR MESSAGE   */
/*          IS PRINTED.                                                   */
/**************************************************************************/
static void ReadTheIf1Files( argc, argv )
int    argc;
char **argv;
{
  register int   idx;
  char     *dotp;

  for ( idx = 1; idx < argc; idx++ ) {
    /* ------------------------------------------------------------ */
    /* Ignore command line options (Careful, some have arguments).  */
    /* ------------------------------------------------------------ */
    if ( *(argv[ idx ]) == '-' ) {
      switch ( argv[idx][1] ) {
      case 'o': case 'e': case 'f': case 'c': case 'r':
        idx++;                  /* SKIP OUTPUT FILE NAME OR ENTRY POINT NAME */
        break;

      default:
        break; 
      }

      continue;
    }

    /* ------------------------------------------------------------ */
    /* Skip librarys during this pass.  They will be resolved after */
    /* all the .if1 files have been loaded.                         */
    /* ------------------------------------------------------------ */
    dotp = strrchr(argv[idx],'.');
    if ( dotp && strcmp(dotp,".A") == 0 ) {
      libraries[librarycount++] = argv[idx];
      continue;
    }

    LoadIF1File(argv[idx],argv[idx]);
  }
}

/**************************************************************************/
/* LOCAL  **************       BuildNeedList       ************************/
/**************************************************************************/
/* PURPOSE:  Convert the import/export list into a namelink list.  Care   */
/*           must be taken to remove inames that were instantiated (are   */
/*           on the xnames list) from the final list.                     */
/**************************************************************************/
static void BuildNeedList(NeedP)
     namelink   **NeedP;
{
  PNAME         F,G;

  for(F = inames; F; F = F->next) {
    for(G = xnames; G; G = G->next) {
      if ( strcmp(G->name,F->name) == 0 ) goto skip;
    }
    AddName(NeedP,F->name);
  skip:
    ;
  }
}

/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* PURPOSE: COMBINE ONE OR MORE IF1 FILES TO FORM AN IF1 MODULE TYPE.     */
/*          NOTE, IF THE OUTPUT FILE NAME IS THE SAME AS ONE OF THE INPUT */
/*          FILE NAMES, THE CONTENTS OF THE INPUT FILE IS OVERWRITTEN.    */
/**************************************************************************/

int main( argc, argv )
int    argc;
char **argv;
{
    register FILE *fd;
    namelink      *Need;
    int           i;
    int           MadeLoad;

    /* fix by dj raymond 25 nov 2000 */
    output = stdout;

    PID = getpid();

    ParseCommandLine( argc, argv );

    /* ------------------------------------------------------------ */
    /* Load all specified IF1 files and create a list of libraries  */
    /* to search for unresolved references.                         */
    /* ------------------------------------------------------------ */
    StartProfiler();
    ReadTheIf1Files( argc, argv );
    StopProfiler( "ReadTheIf1Files" );

    /* ------------------------------------------------------------ */
    /* Read the libraries as needed.  Repeated passes are made over */
    /* the list of libraries until no new loads are made.  The libs */
    /* are always searched in order and no more than one load is    */
    /* made on a pass.                                              */
    /* ------------------------------------------------------------ */
    StartProfiler();
    do {
      Need = NULL;
      BuildNeedList(&Need);

      MadeLoad = 0;
      if ( Need ) {
        for(i=0;i<librarycount;i++) {
          MadeLoad = ReadLibrary(libraries[i],Need);
          if ( MadeLoad ) break;
        }
      }
    } while (MadeLoad);
    StopProfiler( "Library Loads" );


    /* ------------------------------------------------------------ */
    /* Smashcase, smashtypes, and name checks                       */
    /* ------------------------------------------------------------ */
    if ( etop < 0 ) {
      if ( smodule ) {
	PNAME x;
	for(x=xnames; x; x = x->next) {
	  if ( x->node->type == IFXGraph ) {
	    PlaceInEntryTable(x->node->CoNsT);
	  }
	}
      } else {
	PlaceInEntryTable("main");
      }
    }

    StartProfiler();
    LoadSmashTypes();

    CheckForUnresolvedNames();
    StopProfiler( "Smash and Check" );

    /* ------------------------------------------------------------ */
    /* Bookkeeping                                                  */
    /* ------------------------------------------------------------ */
    if ( monolith ) AddStamp( MONOLITH, " CSU -> MONOLITHIC" );
    if ( furl )     AddStamp( QMODE, furl );

    /* ------------------------------------------------------------ */
    /* OPEN THE OUTPUT FILE AND WRITE THE STUFF                     */
    /* ------------------------------------------------------------ */
    if ( ofile != NULL ) {
      if ( (fd = fopen( ofile, "w" )) == NULL ) Error2( "CAN'T OPEN", ofile );
      output = fd;
    }

    StartProfiler();
    MonoIf1Write();
    StopProfiler( "MonoIf1Write" );

    return OK;
}

/*
 * $Log$
 * Revision 1.4  2003/06/15 06:56:39  patmiller
 * Fixes for separate compilation.  Uncovered a serious leak with
 * separate compilation globals.  Set refcount to 1gig to mask
 * for now.
 *
 * Revision 1.3  2001/01/02 09:16:44  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:09  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.13  1994/04/18  19:23:19  denton
 * Removed remaining gcc warnings.
 *
 * Revision 1.12  1994/04/15  15:50:20  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.11  1994/03/31  01:46:17  denton
 * Replace anachronistic BSD functions, index->strchr, rindex->strrchr
 *
 * Revision 1.10  1994/03/24  22:28:55  denton
 * Distributed DSA, non-coherent cache, non-static shared memory.
 *
 * Revision 1.9  1994/02/17  17:55:54  denton
 * Reduction flags
 *
 * Revision 1.8  1993/11/30  00:22:59  miller
 * Added support for (not completed) archiver.
 * */
