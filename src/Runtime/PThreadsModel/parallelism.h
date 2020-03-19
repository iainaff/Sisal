#ifndef PARALLELISM_H
#define PARALLELISM_H

/**************************************************************************/
/* FILE   **************       parallelism.h       ************************/
/**************************************************************************/
/* Author: Dave Raymond                                                   */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Update: Patrick Miller -- Thread improvements (Jan 2001)               */
/* Copyright (C) 2000 Dave Raymond                                        */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/************************************************************************\
 * POSIX threads
\************************************************************************/

static pthread_key_t sisalKey;
static int* identifiers = 0;
static pthread_t *sisalThread = 0;

void ReleaseSharedMemory()
{
  free( SharedBase );
}

/**************************************************************************/
/* GLOBAL **************         sisalGetID        ************************/
/**************************************************************************/
/* Return a unique thread ID. 0 for master, 1..n-1 for workers            */
/**************************************************************************/
int sisalGetID() {
   void* dataP = 0;
   int* tidP = 0;

   /* ----------------------------------------------- */
   /* If not initialized, must be in master           */
   /* ----------------------------------------------- */
   if ( identifiers == 0 ) return 0;
   
   /* ----------------------------------------------- */
   /* Thread ID is stashed in thread specific area    */
   /* ----------------------------------------------- */
   dataP = pthread_getspecific(sisalKey);
   if ( !dataP ) {
      SisalError( "sisalGetID", "no thread id data!!!" );
   }

   /* ----------------------------------------------- */
   /* Here's the thread data!                         */
   /* ----------------------------------------------- */
   tidP = (int*)dataP;
   return *tidP;
}

static void* Transfer( dataP )
     void* dataP;
{
   int* tidP = (int*)dataP;

   /* ----------------------------------------------- */
   /* Set up thread identifier so GetProcId works     */
   /* ----------------------------------------------- */
   if ( pthread_setspecific(sisalKey,dataP) != 0 ) {
      SisalError( "StartWorkers", "pthread_setspecific FAILED" );
   }
   
#if defined(DIST_DSA)
   if (dataP != 0) {
      InitDsa(DsaSize/(2*(NumWorkers - 1)), XftThreshold);
   }
#endif

   EnterWorker( *tidP );

   if ( GetProcId != 0 ) {
      LeaveWorker();
   }
   return NULL;
}

void AcquireSharedMemory( NumBytes ) 
int NumBytes;
{
  SharedSize = NumBytes + 100000;

  SharedBase = SharedMemory = (char *) malloc( SharedSize-40 );

  if ( SharedMemory == (char *) NULL )
    SisalError( "AcquireSharedMemory", "malloc FAILED" );
}

void StartWorkers()
{
   int i;

   /* ----------------------------------------------- */
   /* Build worker sized arrays to hold threads       */
   /* ----------------------------------------------- */
   if ( NumWorkers <= 0 ) NumWorkers = 1;
   sisalThread = (pthread_t*)malloc(NumWorkers*sizeof(pthread_t));
   identifiers = (int*)malloc(NumWorkers*sizeof(int));

   /* ----------------------------------------------- */
   /* Associate a key for identifier info             */
   /* ----------------------------------------------- */
   if ( pthread_key_create(&sisalKey,0) != 0 ) {
      SisalError( "StartWorkers", "pthread_key_create FAILED" );
   }

   /* ----------------------------------------------- */
   /* Setup the master thread info                    */
   /* ----------------------------------------------- */
   identifiers[0] = 0;
   if ( pthread_setspecific(sisalKey,identifiers+0) != 0 ) {
      SisalError( "StartWorkers", "pthread_setspecific FAILED" );
   }
   
   /* ----------------------------------------------- */
   /* Start worker threads (this thread is master)    */
   /* ----------------------------------------------- */
   for(i=1;i<NumWorkers;++i) {
      identifiers[i] = i;
      if ( pthread_create( sisalThread+i, 0, Transfer, identifiers+i) != 0 ) {
         SisalError( "StartWorkers", "create FAILED" );
      }
   }

   /* ----------------------------------------------- */
   /* Master thread runs too...                       */
   /* ----------------------------------------------- */
   Transfer(identifiers+0 );
}

void StopWorkers()
{
  *SisalShutDown = TRUE;
  LeaveWorker();
}

void AbortParallel()
{
  (void)exit( 1 );
}

BARRIER_TYPE *MyInitBarrier(limit)
  int limit;
{
  BARRIER_TYPE *bar;

  if ( (bar = malloc(sizeof(BARRIER_TYPE))) == NULL ) {
    SisalError( "MyInitBarrier", "barrier malloc FAILED" );
    return NULL;
  }
  if ( MY_INIT_LOCK(&bar->lock) ) {
    SisalError( "MyInitBarrier", "barrier lock FAILED" );
    return NULL;
  }
  bar->count = limit;

  return bar;
}

/**************************************************************************/
/* GLOBAL **************            void           ************************/
/**************************************************************************/
/* Yield thread control to waiting process                                */
/**************************************************************************/
void sisalYield() {
#ifdef HAVE_SCHED_YIELD
    sched_yield();
#else
    pthread_yield();
#endif
}

void MyBarrier( bar )
  BARRIER_TYPE *bar;
{
  /*
   * Block for lock, then decrement waiting thread count.
   */
  if ( MY_LOCK(&bar->lock) ) {
    SisalError( "MyBarrier", "barrier lock FAILED" );
  }
  bar->count--;
  if (MY_UNLOCK(&bar->lock)) {
    SisalError( "MyBarrier", "barrier unlock FAILED" );
  }

  /*
   * Busy-wait loop, checking for done.
   */
  while (bar->count>0) {
     sisalYield();
  }

  return;
}

#endif
