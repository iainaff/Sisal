#include "world.h"
#include "usage.h"		/* One-liner descr. of options */


#define FIBREIN		(1)
#define FIBREOUT	(2)
#define SINFO		(3)

#define GET_Tmp(y)        (Tmp = atoi( &(argv[idx][(y)]) ))

char	*SINFOFile	= "s.info";	/* Default information file */
char	*iformat	= "%d\n";
char	*fformat	= "%.6e\n";
char	*dformat	= "%15e\n";
char	*nformat	= "nil\n";
char	*cformat	= "'%c'\n";
char	*cformat2	= "'\\%03o'\n";
char	*bformat	= "%c\n";

#ifndef MAXARGS
#define MAXARGS  500
#endif

char  *av [MAXARGS];		/* COMMAND LINE OF CURRENT COMPILATION PHASE */

#if defined(NO_STATIC_SHARED)
struct shared_s LSR;	/* READ ARGS. LOCALLY BEFORE COPYING TO DYNAMIC MEM. */
#endif

/**************************************************************************/
/* LOCAL  **************        SubmitNQS          ************************/
/**************************************************************************/
/* PURPOSE:  Resubmit this job as an NQS batch job			  */
/**************************************************************************/
static void SubmitNQS(argc,argv,nqs_idx)
     int	argc,nqs_idx;
     char	*argv[];
{
#if defined(CRAY) && !defined(CRAYT3D)
  int		status = 1;
  int		i,avcnt;
  char		*p;
  char		scriptname[100];
  FILE		*script;
  int		s;
  int		id;

  /* ------------------------------------------------------------ */
  /* Open a temp file for the script				  */
  /* ------------------------------------------------------------ */
  sprintf(scriptname,"#NQS%d",getpid());
  script = fopen(scriptname,"w");

  for(i=0;i<nqs_idx;i++) {
    for(p = argv[i]; *p; p++) {
      switch(*p) {
      case '\'':	fputs("\"'\"",script); break;
      case '!':		fputs("\"\\!\"",script); break;
	
      case ' ': case '\t': case '$': case '(': case ')': case ';':
      case '<': case '>':  case '?': case '`': case '{': case '}':
      case '\\': case '&': case '|': case '~':
	FPRINTF(script,"'%c'",*p); break;
      default:
	fputc(*p,script);
      }
    }
    fputc(' ',script);
  }
  fputc('\n',script);
  fprintf(script,"/bin/rm -f '%s'\n",scriptname);
  fclose(script);

  /* ------------------------------------------------------------ */
  /* Build up the queue command...				  */
  /* ------------------------------------------------------------ */
  avcnt = 0;
  av[avcnt++] = "qsub";
  av[avcnt++] = scriptname;
  for(i=nqs_idx+1; i<argc; i++) av[avcnt++] = argv[i];
  av[avcnt] = (char*)NULL;

  /* ------------------------------------------------------------ */
  /* ...and submit it for execution				  */
  /* ------------------------------------------------------------ */
  id = fork();

  if ( id == -1) {
    FPRINTF(stderr, "submit failed: no more processes allowed\n" );
    status = 1;
  } else if ( id == 0 ) {
    execvp( av[0], av );
    FPRINTF(stderr,"Can't find command: %s\n",av[0]);
    unlink(scriptname);
    exit(1);
  }

  while ( id != wait( &s ) ) ;
  if ( s ) {
    FPRINTF(stderr,"%s %s failed\n",av[0],scriptname);
    status = s;
    unlink(scriptname);
  }

#else /* !CRAY */
  int status = 1;
  FPRINTF(stderr, "submit failed: NQS not implemented\n" );
#endif /* CRAY */

  exit(status);
}

/**************************************************************************/
/* GLOBAL  **************       ParseCEscapes       ***********************/
/**************************************************************************/
/* PURPOSE:  Convert C string escapes into internal representation	  */
/**************************************************************************/
char *ParseCEscapes(s)
     char	*s;
{
  char	*buf = (char*)(malloc(strlen(s)+1));
  char	*p = buf;
  int	sum,i;

  while(*s) {
    if ( *s == '\\' ) {		/* Check for escapes */
      s++;
      switch( *s ) {
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7':
	/* Octal escape */
	sum = (int)(*s++ - '0');
	for(i=1;i<3;i++) {
	  if ( *s < '0' || *s > '7' ) break;
	  sum = 8*sum + (int)(*s++ - '0');
	}
	*p++ = (char)(sum);
	break;

      case 'n': *p++ = '\n'; s++; break;
      case 'f': *p++ = '\f'; s++; break;
      case 'b': *p++ = '\b'; s++; break;
      case 'r': *p++ = '\r'; s++; break;
      case 't': *p++ = '\t'; s++; break;

      default:
	*p++ = *s++;
      }
    } else {
      *p++ = *s++;		/* Just copy the character */
    }
  }

  *p = '\0';

  return buf;
}

void ParseCommandLine( argc, argv )
int   argc;
char *argv[];
{
  register int   idx;
  int   Tmp;
  int   FibreFileMode;
  char	*CorrectUsage;

#if defined(NO_STATIC_SHARED)
  /* ------------------------------------------------------------ */
  /* Now initialize the to default values only for the master     */
  /* ------------------------------------------------------------ */
  if (p_procnum == 0)
        InitSharedGlobals();
#endif

  FibreFileMode = FIBREIN;

  for ( idx = 1; idx < argc; idx++ ) {
    CorrectUsage = NULL;	/* No error reasons yet */

    /* ------------------------------------------------------------ */
    /* Look for filename style arguments first			    */
    /* ------------------------------------------------------------ */
    if ( argv[idx][0] != '-' ) {
      switch ( FibreFileMode ) {
       case FIBREIN:
	OPEN( FibreInFd, argv[idx], "r" );
	break;

       case FIBREOUT:
	OPEN( FibreOutFd, argv[idx], "w" );
	break;

       case SINFO:
	SINFOFile = argv[idx];
	GatherPerfInfo = TRUE;	/* specifying a file implies -r */
	break;

       default:
	goto OptionError;
      }

      FibreFileMode++;
      continue;
    }

    /* ------------------------------------------------------------ */
    /* Handle empty file names					    */
    /* ------------------------------------------------------------ */
    if ( argv[idx][1] == '\0' ) {
      FibreFileMode++;
      continue;
    }

    /* ------------------------------------------------------------ */
    /* Look for -option style arguments				    */
    /* ------------------------------------------------------------ */
#include "options.h"
    /* ------------------------------------------------------------ */
    /* Do not change options.h directly.  Modify the options	    */
    /* file and run ``make newoptions''.  This will update the	    */
    /* osc.m man page file and update options.h and usage.h	    */
    /* This makefile target requires the parseopts tool		    */
    /* (available separately).					    */
    /* ------------------------------------------------------------ */

  }

  /* ------------------------------------------------------------ */
  /* Open performance info file if needed			  */
  /* ------------------------------------------------------------ */
  if ( GatherPerfInfo ) OPEN( PerfFd, SINFOFile, "a" );

  /* ------------------------------------------------------------ */
  /* Check GSS/LS conflict					  */
  /* ------------------------------------------------------------ */
  if ( LoopSlices == -1 )
    LoopSlices = NumWorkers;
  else if ( DefaultLoopStyle == 'G' )
    SisalError( "COMMAND LINE CONFLICT", "-gss AND -ls" );

  return;

 OptionError:
  if ( CorrectUsage ) FPRINTF(stderr,"%s\n",CorrectUsage);
  SisalError( "ILLEGAL COMMAND LINE ARGUMENT",(argv[idx])?(argv[idx]):"" );
} 


static void PrintExecutionTimes()
{
  struct WorkerInfo *InfoPtr;
  int    Worker;
  double CpuUse;
  int    NumIterations;

  FPRINTF( PerfFd, "  CpuTime  WallTime    CpuUse\n" );

#ifdef ALLIANT
  NumIterations = 1;
#else
  NumIterations = NumWorkers;
#endif

  for ( Worker = 0; Worker < NumIterations; Worker++ ) {
    InfoPtr = &(AllWorkerInfo[ Worker ]);

    if ( InfoPtr->WallTime != 0.0 ) {
      CpuUse  = InfoPtr->CpuTime;
      CpuUse /= InfoPtr->WallTime;
      }
    else
      CpuUse = 0.0;

    FPRINTF( PerfFd, " %8.4f %9.4f %8.1f%%\n",
             InfoPtr->CpuTime, InfoPtr->WallTime, CpuUse * 100.0 );
    }
}


void DumpRunTimeInfo()
{
  register struct WorkerInfo *InfoPtr;
  register int    Worker;
  register double CopyInfo, ATAttempts, ATCopies, ANoOpAttempts;
  register double RBuilds;
  register double ANoOpCopies, RNoOpAttempts, RNoOpCopies, ADataCopies;
  register int    StorageUsed, StorageWanted, DsaHelp;
  register double FlopInfo, FlopCountA, FlopCountL, FlopCountI;

  FlopInfo = FlopCountA = FlopCountL = FlopCountI = 0.0;

  CopyInfo = RBuilds = ATAttempts = ATCopies = ANoOpAttempts = 0.0;
  ANoOpCopies = RNoOpAttempts = RNoOpCopies = ADataCopies = 0.0;
  StorageUsed =  StorageWanted = DsaHelp = 0;

  for ( Worker = 0; Worker < NumWorkers; Worker++ ) {
    InfoPtr = &(AllWorkerInfo[ Worker ]);

    CopyInfo      += InfoPtr->CopyInfo;
    FlopInfo      += InfoPtr->FlopInfo;

    FlopCountA    += InfoPtr->FlopCountA;
    FlopCountL    += InfoPtr->FlopCountL;
    FlopCountI    += InfoPtr->FlopCountI;

    RBuilds       += InfoPtr->RBuilds;
    ATAttempts    += InfoPtr->ATAttempts;
    ATCopies      += InfoPtr->ATCopies;
    ANoOpAttempts += InfoPtr->ANoOpAttempts;
    ANoOpCopies   += InfoPtr->ANoOpCopies;
    RNoOpAttempts += InfoPtr->RNoOpAttempts;
    RNoOpCopies   += InfoPtr->RNoOpCopies;
    ADataCopies   += InfoPtr->ADataCopies;

    DsaHelp        += InfoPtr->DsaHelp;
    StorageUsed    += InfoPtr->StorageUsed;
    StorageWanted  += InfoPtr->StorageWanted;
    }

  FPRINTF( PerfFd, "\n\n\n" );
  FPRINTF( PerfFd, "  Workers   DsaSize  ExactFit  DsaHelps\n" );
  FPRINTF( PerfFd, "%9d %8db %8db %9d\n\n", 
	           NumWorkers, DsaSize, XftThreshold, DsaHelp );

  switch ( DefaultLoopStyle ) {
   case 'G':
    FPRINTF( PerfFd, "     MemW      MemU   GssFact   ArrayEx\n" );
    FPRINTF( PerfFd, "%8db %8db %9d %9d\n\n", 
	    StorageWanted, StorageUsed, 1, ArrayExpansion );
    break;

   default:
    FPRINTF( PerfFd, "     MemW      MemU  LpSliceV   ArrayEx\n" );
    FPRINTF( PerfFd, "%8db %8db %9d %9d\n\n", 
	    StorageWanted, StorageUsed, LoopSlices, ArrayExpansion );
  }

  PrintExecutionTimes();

  if ( CopyInfo > 0.0 ) {
    FPRINTF( PerfFd, "\n              AtOps            AtCopies"  );
    FPRINTF( PerfFd, "               AcOps            AcCopies\n" );
    FPRINTF( PerfFd, " %18.0f  %18.0f  %18.0f  %18.0f\n\n", ATAttempts, 
		       ATCopies, ANoOpAttempts, ANoOpCopies          );

    FPRINTF( PerfFd, "              RcOps            RcCopies" );
    FPRINTF( PerfFd, "           CharMoves\n" );
    FPRINTF( PerfFd, " %18.0f  %18.0f  %18.0f\n", RNoOpAttempts, 
		       RNoOpCopies, ADataCopies               );

    FPRINTF( PerfFd, "            RBuilds\n" );
    FPRINTF( PerfFd, " %18.0f\n", RBuilds );
    }

  if ( FlopInfo > 0.0 ) {
    FPRINTF( PerfFd, "\n FlopCounts (ARITHMETIC): %18.0f\n", FlopCountA );
    FPRINTF( PerfFd, "               (LOGICAL): %18.0f\n", FlopCountL );
    FPRINTF( PerfFd, "             (INTRINSIC): %18.0f\n", FlopCountI );
    }

  FPRINTF( PerfFd, "\n" );
}


void InitSisalRunTime()
{
#if defined(DIST_DSA)
  AcquireSharedMemory( DsaSize * MAX_PROCS );
#else
  AcquireSharedMemory( DsaSize );
#endif

#if defined(DIST_DSA)
  InitDsaCaches( DsaSize/2, XftThreshold );
  InitDsa( DsaSize/2, XftThreshold );
#else
  InitDsa( DsaSize, XftThreshold );
#endif

  InitErrorSystem();
  InitWorkers();
  InitReadyList();
  InitSignalSystem();
  InitSpawn();
}
