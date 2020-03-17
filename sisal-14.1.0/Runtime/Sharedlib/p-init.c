/**************************************************************************/
/* FILE   **************          p-init.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalrt.h"
#include "usage.h"              /* One-liner descr. of options */


#define FIBREIN         (1)
#define FIBREOUT        (2)
#define SINFO           (3)

#define GET_Tmp(y)        (Tmp = atoi( &(argv[idx][(y)]) ))

char    *SINFOFile      = "s.info";     /* Default information file */
char    *iformat        = "%d ";
char    *fformat        = "%.6e ";
char    *dformat        = "%15e ";
char    *nformat        = "nil ";
char    *cformat        = "'%c' ";
char    *cformat2       = "'\\%03o' ";
char    *bformat        = "%c ";

char  *av [1024];               /* COMMAND LINE OF CURRENT COMPILATION PHASE */

#if defined(NO_STATIC_SHARED)
struct shared_s LSR;    /* READ ARGS. LOCALLY BEFORE COPYING TO DYNAMIC MEM. */
#endif

/**************************************************************************/
/* GLOBAL  **************       ParseCEscapes       ***********************/
/**************************************************************************/
/* PURPOSE:  Convert C string escapes into internal representation        */
/**************************************************************************/
char *ParseCEscapes(s)
     char       *s;
{
  char  *buf = (char*)(malloc(strlen(s)+1));
  char  *p = buf;
  int   sum,i;

  while(*s) {
    if ( *s == '\\' ) {         /* Check for escapes */
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
      *p++ = *s++;              /* Just copy the character */
    }
  }

  *p = '\0';

  return buf;
}

char** sisal_save_argv = 0;

void ParseCommandLine( argc, argv )
int   argc;
char *argv[];
{
  register int   idx;
  int   Tmp;
  int   FibreFileMode;
  char  *CorrectUsage;
  int i;

  sisal_save_argv = malloc((argc+1)*sizeof(char*));
  sisal_save_argv[0] = argv[0];
  sisal_save_argv[1] = 0;

#if defined(NO_STATIC_SHARED)
  /* ------------------------------------------------------------ */
  /* Now initialize the to default values only for the master     */
  /* ------------------------------------------------------------ */
  if (p_procnum == 0)
        InitSharedGlobals();
#endif

  FibreFileMode = FIBREIN;

  for ( idx = 1; idx < argc; idx++ ) {
    if ( argv[idx][0] != '-' ) goto OptionError;

    /* ------------------------------------------------------------ */
    /* Look for -option style arguments                             */
    /* ------------------------------------------------------------ */
#include "options.h"
    /* ------------------------------------------------------------ */
    /* Do not change options.h directly.  Modify the options        */
    /* file and run ``make newoptions''.  This will update the      */
    /* osc.m man page file and update options.h and usage.h         */
    /* This makefile target requires the parseopts tool             */
    /* (available separately).                                      */
    /* ------------------------------------------------------------ */

  }

  /* ------------------------------------------------------------ */
  /* Open performance info file if needed                         */
  /* ------------------------------------------------------------ */
 Finalize:
  if ( GatherPerfInfo ) OPEN( PerfFd, SINFOFile, "a" );

  /* ------------------------------------------------------------ */
  /* Check GSS/LS conflict                                        */
  /* ------------------------------------------------------------ */
  if ( LoopSlices == -1 )
    LoopSlices = NumWorkers;
  else if ( DefaultLoopStyle == 'G' )
    SisalError( "COMMAND LINE CONFLICT", "-gss AND -ls" );

  return;

 OptionError:
  for(i=idx;i<argc;++i) {
    sisal_save_argv[i-idx+1] = argv[i];
  }    
  sisal_save_argv[i-idx+1] = 0;
  goto Finalize;
} 


static void PrintExecutionTimes()
{
  struct WorkerInfo *InfoPtr;
  int    Worker;
  double CpuUse;
  int    NumIterations;

  FPRINTF( PerfFd, "  CpuTime  WallTime    CpuUse\n" );

  NumIterations = NumWorkers;

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
