#ifndef PROCESS_H
#define PROCESS_H

/**************************************************************************/
/* FILE   **************         process.h         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#define FOR_NOTHING   0
#define FOR_SHUTDOWN  1

#define SPAWN_SIMPLE  0
#define SPAWN_COMPLEX 1

#define DEFAULT_NUM_WORKERS       1
#define DEFAULT_DSA_SIZE          2000000
#define DEFAULT_XFT_THRESHOLD     100
#define DEFAULT_LOOP_SLICES       1
#define DEFAULT_LOOP_FACTOR       2
#define DEFAULT_ARRAY_EXPANSION   100

#include "process-implementation.h"

struct ActRec {
  POINTER ArgPointer;                     /* TASK ARGUMENT               */
  int     AuxArgument;                    /* AUXILIARY TASK ARGUMENT     */
  void    (*ChildCode)();                 /* TASK ADDRESS                */
  int     SliceBounds[3];                 /* LOOP SLICE CONTROL INFO     */
  struct  ActRec *NextAR;                 /* FORWARD QUEUE LINK          */
  int     Done;                           /* IS THIS TASK DONE YET?      */
  int     pid;
  int     Flush;
#if defined(NON_COHERENT_CACHE)
  char    pad[CACHE_LINE];
#endif
  };

struct ActRecCache {
  struct ActRec *Head;                    /* HEAD OF QUEUE               */
  struct ActRec *Tail;                    /* TAIL OF QUEUE               */
#if defined(NON_COHERENT_CACHE)
  char   pad1[CACHE_LINE - (2 * sizeof(struct ActRect *))];
#endif
  LOCK_TYPE      Mutex;                   /* QUEUE LOCK                  */
#if defined(NON_COHERENT_CACHE)
  char   pad2[CACHE_LINE - (1 * sizeof(LOCK_TYPE))];
#endif
  };


#define MYINFO(x)  (&(AllWorkerInfo[(x)]))
#define MyInfo     (&(AllWorkerInfo[GetProcId]))

struct WorkerInfo {
  double FlopInfo;                      /* WAS FLOP INFORMATION GATHERED */
  double FlopCountA;                                    /* FLOP COUNTERS */
  double FlopCountL;
  double FlopCountI;

  double CopyInfo;                      /* WAS COPY INFORMATION GATHERED */
  double ATAttempts;                               /* DATA COPY COUNTERS */
  double ATCopies;
  double ANoOpAttempts;
  double ANoOpCopies;
  double RNoOpAttempts;
  double RNoOpCopies;
  double ADataCopies;
  double RBuilds;

  int    DsaHelp;                             /* COUNT OF DsaHelp CALLS  */

  int    StorageUsed;                         /* WANTED AND USED STORAGE */
  int    StorageWanted;

  struct timeval WallTimeBuffer;              /* WALL CLOCK START TIME   */
  double WallTime;                            /* ELAPSED WALL CLOCK TIME */
  double CpuTime;                             /* ELAPSED CPU TIME        */
  long   ClkTck;                              /* CLOCK TICK FROM SYSCONF */
#if defined(NON_COHERENT_CACHE)
  char   pad[CACHE_LINE];
#endif
  };

#ifdef GatherCopyInfo
#define SaveCopyInfo      { MyInfo->CopyInfo = 1.0;     }
#define IncRBuilds        { MyInfo->RBuilds++;          }
#define IncATAttempts     { MyInfo->ATAttempts++;       }
#define IncATCopies       { MyInfo->ATCopies++;         }
#define IncANoOpAttempts  { MyInfo->ANoOpAttempts++;    }
#define IncANoOpCopies    { MyInfo->ANoOpCopies++;      }
#define IncRNoOpAttempts  { MyInfo->RNoOpAttempts++;    }
#define IncRNoOpCopies    { MyInfo->RNoOpCopies++;      }
#define IncDataCopies(x)  { MyInfo->ADataCopies += (x); }
#else
#define SaveCopyInfo
#define IncRBuilds
#define IncATAttempts
#define IncATCopies
#define IncANoOpAttempts
#define IncANoOpCopies
#define IncRNoOpAttempts
#define IncRNoOpCopies
#define IncDataCopies(x)
#endif

#ifdef GatherFlopInfo
#define SaveFlopInfo       { MyInfo->FlopInfo = 1.0;      }
#define IncFlopCountA(x)   { MyInfo->FlopCountA += (x);   }
#define IncFlopCountL(x)   { MyInfo->FlopCountL += (x);   }
#define IncFlopCountI(x)   { MyInfo->FlopCountI += (x);   }
#else
#define SaveFlopInfo
#define IncFlopCountA(x)
#define IncFlopCountL(x)
#define IncFlopCountI(x)
#endif

#define IncStorageUsed(y,x)   { MYINFO(y)->StorageUsed   += x;   }
#define IncStorageWanted(y,x) { MYINFO(y)->StorageWanted += x;   }
#define IncDsaHelp(y)         { MYINFO(y)->DsaHelp += 1;         }


#define OPEN(f,n,m) if ( ((f) = fopen( (n), (m) )) == (FILE *) NULL )\
                      SisalError( "CAN'T OPEN", (n) )


#define SDBX_INT    0
#define SDBX_FPE    1
#define SDBX_ESTART 2
#define SDBX_ESTOP  3
#define SDBX_BDS    4
#define SDBX_DB0    5
#define SDBX_PUSH   6
#define SDBX_POP    7
#define SDBX_SENTER 8
#define SDBX_SEXIT  9
#define SDBX_NONE   10
#define SDBX_IERR   11
#define SDBX_ERR    12

#define SDBX_INT    0
#define SDBX_PTR    1
#define SDBX_DBL    2

struct SdbxValue {
  char          *Name;
  unsigned char  Kind;
  unsigned char  ArrayType;
  unsigned char  Active;
  void           (*PrintRoutine)();

  union {
    int     InT;
    POINTER PtR;
    double  DbL;
    } Value;
  };

struct SdbxInfo {
  int     Action;
  char   *File;
  char   *Function;
  int     Line;
  int     ScopeStackTop;
  int     ScopeSize;
  struct  SdbxValue **ScopeStack;
  char  **FunctionList;
  };

extern struct SdbxValue  *SdbxScope;
extern int                SdbxScopeSize;
extern struct SdbxValue  *SdbxCurrentScope;
extern struct SdbxInfo    SdbxState;
extern struct SdbxInfo    SdbxAction;
extern char             **SdbxCurrentFunctionList;

extern void SdbxMonitor PROTO((int));
extern void SdbxHandler PROTO((int));


extern int p_procnum;

extern struct ActRec  **RListFrontD;

extern int     Sequential;
extern int     UsingSdbx;

extern POINTER SharedMalloc PROTO((int));
extern void    AcquireSharedMemory PROTO((int));
extern void    ReleaseSharedMemory PROTO((void));
extern void    StartWorkers PROTO((void));
extern void    StartWorkersWithEntry PROTO((void (*entry)()));
extern void    StopWorkers PROTO((void));
extern void    AbortParallel PROTO((void));

extern void    DumpRunTimeInfo PROTO((void));
extern void    InitSisalRunTime PROTO((void));

extern void    Wait PROTO((int));
extern void    Sync PROTO((struct ActRec*,struct ActRec*));

extern void    InitSpawn PROTO((void));
extern void    SpawnSlices PROTO((int, void (*ChildCode)(void),POINTER,int,int,int,int));
extern void    OptSpawnSlices PROTO((struct ActRec*,int));
extern void    OptSpawnSlicesFast PROTO((struct ActRec*,int));
extern void    BuildSlices PROTO((int,struct ActRec**,int*,void (*ChildCode)(void),POINTER,int,int,int,int,int,int));

extern void    EnterWorker PROTO((int));
extern void    LeaveWorker PROTO((void));
extern void    InitWorkers PROTO((void));

extern void    SisalMain PROTO((POINTER));

extern void           InitReadyList PROTO((void));
extern struct ActRec *RListDeQ PROTO((void));
extern void           RListEnQ PROTO((struct ActRec*,struct ActRec*));

extern void    StopTimer PROTO((void));
extern void    StartTimer PROTO((void));

/* ------------------------------------------------------------ */

#if defined(NO_STATIC_SHARED)
struct shared_s
{
        int     NumWorkers;
        int     DsaSize; 
        int     BindParallelWork;
        int     OneLevelParallel;
        int     UseGss;
        int     UseStride;
        int     DEFStride;
        int     XftThreshold;
        int     WstWindowSize;
        int     LoopSlices;
        int     GatherPerfInfo;
        int     ArrayExpansion;
        int     NoFibreOutput;
        int     UsePrivateMemory;
        char    DefaultLoopStyle;
        int     UsingSdbx;
        int     Sequential;
        int     *SisalShutDown;
        struct  WorkerInfo *AllWorkerInfo;
        BARRIER_TYPE Fb;
        BARRIER_TYPE Sb;
        int     UnderDBX;
        struct  ActRecCache *ARList;
        struct  top *caches;
#if !defined(DIST_DSA)
        struct  bot *dsorg;
        struct  top *zero_bl;
        int     *zb_start;
        LOCK_TYPE *coal_lock;
        struct  top *btop;
#else
        LOCK_TYPE *Dsa_lock;
#endif
        int     xfthresh;
        int     maxsize;
        char    *SharedMemory;
        char    *SharedBase;
        int     SharedSize;
        LOCK_TYPE *UtilityLock;
        LOCK_TYPE *SUtilityLock;
        void    (*Entry_point)();
};

extern void InitSharedGlobals PROTO((void));
extern int sdebug;

extern struct shared_s SR;
extern struct shared_s *SRp;

#define NumWorkers (SRp->NumWorkers)
#define DsaSize (SRp->DsaSize) 
#define BindParallelWork (SRp->BindParallelWork)
#define OneLevelParallel (SRp->OneLevelParallel)
#define UseGss (SRp->UseGss)
#define UseStride (SRp->UseStride)
#define DEFStride (SRp->DEFStride)
#define XftThreshold (SRp->XftThreshold)
#define WstWindowSize (SRp->WstWindowSize)
#define LoopSlices (SRp->LoopSlices)
#define GatherPerfInfo (SRp->GatherPerfInfo)
#define ArrayExpansion (SRp->ArrayExpansion)
#define NoFibreOutput (SRp->NoFibreOutput)
#define UsePrivateMemory (SRp->UsePrivateMemory)
#define DefaultLoopStyle (SRp->DefaultLoopStyle)
#define UsingSdbx (SRp->UsingSdbx)
#define Sequential (SRp->Sequential)
#define SisalShutDown (SRp->SisalShutDown)
#define AllWorkerInfo (SRp->AllWorkerInfo)
#define UnderDBX (SRp->UnderDBX)
#define ARList (SRp->ARList)
#define caches (SRp->caches)
#if !defined(DIST_DSA)
#define zero_bl (SRp->zero_bl)
#define dsorg (SRp->dsorg)
#define btop (SRp->btop)
#define coal_lock (SRp->coal_lock)
#define zb_start (SRp->zb_start)
#else
#define Dsa_lock (SRp->Dsa_lock)
#endif
#define xfthresh (SRp->xfthresh)
#define maxsize (SRp->maxsize)
#define SharedMemory (SRp->SharedMemory)
#define SharedBase (SRp->SharedBase)
#define SharedSize (SRp->SharedSize)
#define UtilityLock (SRp->UtilityLock)
#define SUtilityLock (SRp->SUtilityLock)
#define Entry_point (SRp->Entry_point)

/* ------------------------------------------------------------ */

#else
extern int     NumWorkers;
extern int     DsaSize;
extern int     BindParallelWork;
extern int     UseGss;
extern int     UseStride;
extern int     DEFStride;
extern int     XftThreshold;
extern int     WstWindowSize;
extern int     LoopSlices;
extern int     GatherPerfInfo;
extern int     ArrayExpansion;
extern int     NoFibreOutput;
extern int     UsePrivateMemory;
extern char    DefaultLoopStyle;
extern volatile int *SisalShutDown;
extern struct WorkerInfo *AllWorkerInfo;
extern int     OneLevelParallel;
extern LOCK_TYPE *UtilityLock;
extern LOCK_TYPE *SUtilityLock;
extern void (*Entry_point) PROTO((void));
#if defined(DIST_DSA)
extern LOCK_TYPE *Dsa_lock;
#endif
#endif

/* ------------------------------------------------------------ */

extern void    InitErrorSystem PROTO((void));
extern int     SisalError PROTO((char*,char*));

extern void    DsaInit PROTO((void));
extern POINTER Alloc PROTO((int));
extern void    DeAlloc PROTO((POINTER));
extern void    DoDeAllocWork PROTO((void));
extern void    DeAllocToBt PROTO((POINTER));

extern void    FreePointerSwapStorage PROTO((POINTER,POINTER));
extern POINTER AllocPointerSwapStorage PROTO((POINTER,int));
extern POINTER SkiAllocPointerSwapStorage PROTO((int,POINTER,int));

#endif
