#include "world.h"

/* *****                                             */
/* ***** SISAL Run Time Support Software             */
/* ***** Parallel dynamic storage allocation section */
/* ***** Author:   R. R. Oldehoeft                   */
/* ***** Modifier: D. C. Cann                        */

/* shared LOCK_TYPE clock; */

#define NR_ZERO_BL   1
static int nr_zero_bl = NR_ZERO_BL;

#if defined(DEBUG_DSA)
#define ALLOC1 "ALLOC1"
#define ALLOC2 "ALLOC2"
#define ALLOC3 "ALLOC3"
#define ALLOC4 "ALLOC4"
#define ALLOC5 "ALLOC5"
#define ALLOC6 "ALLOC6"
#define ALLOC7 "ALLOC7"
#define ALLOC8 "ALLOC8"
static short dsacounter = 0;
static int verbose = 0;
#endif

struct top { 
  struct top *frwd, *bkwd;
  struct top *dnext;
  int size;
  int lsize;
  int PId;
#if defined(DEBUG_DSA)
  short counter;
#endif
  char status;
#if defined(NON_COHERENT_CACHE)
  char pad1[CACHE_LINE - (3 * sizeof(struct top *)) - (4 * sizeof(int))];
#endif
  LOCK_TYPE lock;
#if defined(NON_COHERENT_CACHE)
  char pad2[CACHE_LINE - (1 * sizeof(LOCK_TYPE))];
#endif
  };

struct bot { 
  struct top *top_ptr; 
#if defined(NON_COHERENT_CACHE)
  char pad[CACHE_LINE - (1 * sizeof(int *))];
#endif
};


#define   TOPSIZE     SIZE_OF(struct top)
#define   BOTSIZE     SIZE_OF(struct bot)
#define	  SIZETAGS    (TOPSIZE + BOTSIZE)


#if !defined(NO_STATIC_SHARED)

shared struct top *caches;

#if defined(DIST_DSA)


static int xfthresh;
static int maxsize;

#else

struct bot *dsorg;
struct top *zero_bl;

int     *zb_start;
LOCK_TYPE *coal_lock;

struct top *btop;

static int xfthresh;
static int maxsize;

#endif
#endif

#if !defined(DIST_DSA)

#ifdef DEBUG_DSA
int dallocs = 0;
int dfrees  = 0;
int dbytes  = 0;
#endif


void ShutDownDsa()
{
#if defined(DEBUG_DSA)
  FPRINTF( stderr, "D - (ShutDownDsa) Allocs %d Frees %d Lost Bytes %d\n", 
         dallocs, dfrees, dbytes );
#endif
}


void InitDsa( size, xft )
int size;
int xft;
{
   register struct top *cu, *nx;
   register struct bot *cubot;
   register int         i, inc, roundsize;

#if defined(DEBUG_DSA)
#if defined(VERBOSE)
   verbose = 1;
#endif
#endif

   if ( NumWorkers == 1 ) nr_zero_bl = 1;

   /* MY_SINIT_LOCK(&clock); */
   caches  = (struct top*) SharedMalloc( SIZEOF(struct top) * MAX_PROCS );
   /* ALLOCATE SOME EXTRA JUST TO MAKE SURE EVERY THING IS OK!!! HELP :-) */
   zero_bl = (struct top*) SharedMalloc( SIZEOF(struct top) * (nr_zero_bl+5) );

   zb_start  = (int*) SharedMalloc( SIZEOF(int) );
   *zb_start = 0;

   coal_lock = (LOCK_TYPE*) SharedMalloc( SIZEOF(LOCK_TYPE) );

   for(i=0; i<NumWorkers; i++){
     caches[i].size  = 0;
     caches[i].lsize = 0;
     caches[i].frwd  = 0;
     caches[i].bkwd  = 0;
     }

   xfthresh = xft >= 0 ? xft : 0;

   /* Begin with an allocated bottom tag boundary (whose top part is */
   /* the boundary at the other end of the space!).                  */

   roundsize = ALIGN(int,size);
   dsorg = (struct bot *) SharedMalloc( roundsize );
   maxsize   = roundsize - 2 * SIZETAGS;

   /* Surround the space with an allocated bottom tag before and an
      allocated top tag after. */
   btop = (struct top *)((PCMCAST)dsorg + roundsize - TOPSIZE);
   btop->status = 'A'; 
   btop->size   = -1;
   btop->frwd   = btop;
   MY_SINIT_LOCK(&(btop->lock));
   btop->bkwd   = btop;

   dsorg->top_ptr = btop;

   /* Set up the space between the boundaries as free blocks, with one of  */
   /* the zero size blocks preceding each one, all in a doubly linked list */

   cu = (struct top *)((PCMCAST)dsorg + BOTSIZE);

   inc  = ((int) ((PCMCAST)btop - (PCMCAST)dsorg)) / nr_zero_bl;
   inc -= (inc % ALIGN_SIZE);

   for( i = 0; i < nr_zero_bl - 1; i++ ){
     /* Make a free block at cu */
     nx = (struct top *)((PCMCAST)cu + inc);

     cubot = (struct bot *)((PCMCAST)nx - BOTSIZE);
     cubot->top_ptr = cu;

     cu->status = 'F';
     cu->size   = (int) ((PCMCAST)cubot - (PCMCAST)cu - TOPSIZE);
     MY_SINIT_LOCK(&(cu->lock));

     /* Link it between zero size blocks */
     zero_bl[i].frwd   = cu;
     zero_bl[i].status = 'F';
     zero_bl[i].size   = 0;

     MY_SINIT_LOCK(&(zero_bl[i].lock));

     cu->bkwd = &(zero_bl[i]);
     cu->frwd = &(zero_bl[i+1]);

     zero_bl[i+1].bkwd = cu;
     cu = nx;
     }

  /* Construct last free block and link in */
  cu->frwd   = &(zero_bl[0]);
  cu->bkwd   = &(zero_bl[nr_zero_bl-1]);
  cu->status = 'F';

  cubot = (struct bot *)((PCMCAST)btop - BOTSIZE);
  cubot->top_ptr = cu;

  cu->size = (int) (( (PCMCAST)btop - (PCMCAST)cu ) - SIZETAGS);

  MY_SINIT_LOCK(&(cu->lock));
  zero_bl[nr_zero_bl-1].frwd   = cu;
  zero_bl[nr_zero_bl-1].status = 'F';
  zero_bl[nr_zero_bl-1].size   = 0;
  MY_SINIT_LOCK(&(zero_bl[nr_zero_bl-1].lock));

  zero_bl[0].bkwd = cu;

  /* Unlock the coalescing lock */
  MY_SINIT_LOCK(coal_lock);

  FLUSHALL;
}

#if defined(DEBUG_DSA)
printDSA()
{
  struct top *cu, *pr, *back;
  int pID, mystart;
  int i, counter;

  GETPROCID(pID);
  pr = &(zero_bl[ pID ]);
  back = pr;

  for (i=0; ; i++) {
    cu = pr->frwd;
    counter = cu->counter;
    FPRINTF(stderr, "%d %d\n", i, counter);
    if ( cu == back )
        break;
    pr = cu;
  }
  return;
}
#endif

/* Allocate storage from the boundary tag-managed pool */

static char *btAlloc(size)
register int size;
{
   register struct top *cu, *pr, *back;
   struct top          *newtop;
   char                *addr;
   struct bot          *cubot, *newbot;
   int                  mystart, lsize;
   int                  pID;

   lsize = size;
   size  = ALIGN(int,size);

   GETPROCID(pID);

   IncStorageUsed(pID,(size+SIZETAGS));
   IncStorageWanted(pID,(size));

   /* Get a free block */
   if ( NumWorkers > NR_ZERO_BL )
     *zb_start = mystart = (*zb_start + 1 + pID) % nr_zero_bl;
   else
     mystart = pID;

   pr = &(zero_bl[ mystart ]);

   back = pr;

   MY_SLOCK(&(pr->lock));

   /*
    * flushes in this section are to keep processor from reading stale
    * data on the next go around
    */
   for ( ;; ) {
     cu = pr->frwd;
     FLUSHLINE(&(pr->frwd));

     if ( cu == back ) {
       MY_SUNLOCK(&(pr->lock));
       FLUSHLINE(&(pr->lock));
       return( 0 );
       }

     MY_SLOCK(&(cu->lock));

     if( (cu->size >= size ) && (cu->status != 'A'))
       break;

     FLUSHLINE(&(cu->size));
     FLUSHLINE(&(cu->status));
     MY_SUNLOCK(&(pr->lock)); 
     FLUSHLINE(&(pr->lock)); 
     pr = cu;
     }


   if (cu->size - size <= xfthresh + SIZETAGS){
     /* Exact fit (or close enough).  Unlink cu */
     pr->frwd = cu->frwd;

     cu->frwd->bkwd = pr;
     cu->status     = 'A';
     cu->PId        = pID;
     cu->lsize      = lsize;
#if defined(DEBUG_DSA)
     cu->counter    = dsacounter++;
#endif

     addr = (char *) ((PCMCAST)cu + TOPSIZE);
     FLUSHLINE(&(pr->frwd));
     FLUSHLINE(&(cu->frwd->bkwd));
     FLUSH(cu,cu+sizeof(*cu));
   }else{
     /* Split this large block to satisfy request */
     cubot  = (struct bot *)((PCMCAST)cu + cu->size + TOPSIZE);

     addr   = (char *)((PCMCAST)cubot - size);

     newtop = (struct top *)((PCMCAST)addr - TOPSIZE);
     newbot = (struct bot *)((PCMCAST)newtop - BOTSIZE);

     /* Reduce size of cu and make new bottom tag */
     cu->size -= size + SIZETAGS;
     FLUSHLINE(&(cu->size));

     newbot->top_ptr = cu;

     /* Make tags for new allocated block */
     newtop->size   = size;
     newtop->status = 'A';
     newtop->PId    = pID;
     newtop->lsize  = lsize;
#if defined(DEBUG_DSA)
     newtop->counter    = dsacounter++;
#endif

     MY_SINIT_LOCK(&(newtop->lock));

     cubot->top_ptr = newtop;
     FLUSH(cubot,cubot+sizeof(*cubot));
     FLUSH(newbot,newbot+sizeof(*newbot));
     FLUSH(newtop,newtop+sizeof(*newtop));
   }

  MY_SUNLOCK(&(pr->lock));
  FLUSHLINE(&(pr->lock));
  MY_SUNLOCK(&(cu->lock));
  FLUSHLINE(&(cu->lock));

  return(addr);
}


static void btDeAlloc(ptr)
register struct top *ptr;
{
  register struct top *bl_above, *bl_below, *pr, *cu;
  struct bot          *bot_above, *cubot;
  int                  mystart;
  int                  pID;

  GETPROCID(pID);

  bl_below  = (struct top *)((PCMCAST)ptr + ptr->size + SIZETAGS);
  FLUSHLINE(&(ptr->size));
  bot_above = (struct bot *)((PCMCAST)ptr - BOTSIZE);

  /* Become the only coalescing process (sigh) */
  MY_SLOCK(coal_lock);

  /* Attempt to coalesce the free block below with this block */
  if( bl_below != btop ){
    for (;;) {
      MY_SLOCK(&(bl_below->lock));

      if ( bl_below->status != 'F' ) {
        FLUSHLINE(&(bl_below->status));
        MY_SUNLOCK(&(bl_below->lock)); 
        FLUSHLINE(&(bl_below->lock)); 
	goto tryabove;
        }

      pr = bl_below->bkwd;
      FLUSHLINE(&(bl_below->bkwd));
      MY_SUNLOCK(&(bl_below->lock));
      FLUSHLINE(&(bl_below->lock));

      MY_SLOCK(&(pr->lock));
      if( pr->status == 'F' && pr->frwd == bl_below )
	break;
      FLUSH(pr,pr+sizeof(*pr));
      MY_SUNLOCK(&(pr->lock));
      FLUSHLINE(&(pr->lock));
      }

    MY_SLOCK(&(bl_below->lock));
    pr->frwd = bl_below->frwd;

    bl_below->frwd->bkwd = pr;

    cubot = (struct bot *)((PCMCAST)bl_below + bl_below->size + TOPSIZE);
    cubot->top_ptr = ptr;

    ptr->size += bl_below->size + SIZETAGS;

    FLUSHLINE(&(ptr->size));
    FLUSHLINE(&(cubot->top_ptr));
    FLUSH(pr,pr+sizeof(*pr));
    FLUSH(bl_below,bl_below+sizeof(*bl_below));
    MY_SUNLOCK(&(pr->lock));
    FLUSHLINE(&(pr->lock));
    MY_SUNLOCK(&(bl_below->lock));
    FLUSHLINE(&(bl_below->lock));
    }

tryabove:

  /* Attempt to coalesce with the block above */
  if ( bot_above != dsorg ) {
    for (;;) {
      bl_above = bot_above->top_ptr;

      MY_SLOCK(&(bl_above->lock));
      if( (PCMCAST)ptr == ((PCMCAST)bl_above + bl_above->size + SIZETAGS))
        break;
      FLUSHLINE(&(bot_above->top_ptr));
      FLUSHLINE(&(bl_above->size));
      MY_SUNLOCK(&(bl_above->lock));
      FLUSHLINE(&(bl_above->lock));
      }

    if ( bl_above->status == 'F' ) {
      bl_above->size += ptr->size + SIZETAGS;

      cubot = (struct bot *)((PCMCAST)ptr + ptr->size + TOPSIZE);
      cubot->top_ptr = bl_above;

      FLUSHLINE(&(ptr->size));
      FLUSHLINE(&(cubot->top_ptr));
      FLUSH(bl_above,bl_above+sizeof(*bl_above));
      MY_SUNLOCK(&(bl_above->lock));
      FLUSHLINE(&(bl_above->lock));
      MY_SUNLOCK(coal_lock);

      /* Report new block size */
      return;
      }

    FLUSHLINE(&(bl_above->status));
    MY_SUNLOCK(&(bl_above->lock));
    FLUSHLINE(&(bl_above->lock));
    }

  /* Cannot merge.  Add this block to the free list */
  MY_SUNLOCK(coal_lock);

  if ( NumWorkers > NR_ZERO_BL )
    *zb_start = mystart = (*zb_start + 1 + pID) % nr_zero_bl;
  else
    mystart = pID;

  pr = &(zero_bl[ mystart ]);
  MY_SLOCK(&(pr->lock));

  cu = pr->frwd;

  if ( pr != cu )
    MY_SLOCK(&(cu->lock));

  pr->frwd = ptr;

  ptr->bkwd   = pr;
  ptr->frwd   = cu;
  ptr->status = 'F';

  cu->bkwd = ptr;

  if(pr != cu)
  {
  	FLUSHLINE(&(pr->frwd));
  }
  else
  {
	FLUSH(cu,cu+sizeof(*cu));
  }
  FLUSH(ptr,ptr+sizeof(*ptr));
  MY_SUNLOCK(&(pr->lock));
  FLUSHLINE(&(pr->lock));

  if ( pr != cu )
  {
    FLUSH(cu,cu+sizeof(*cu)); 
    MY_SUNLOCK(&(cu->lock));
    FLUSHLINE(&(cu->lock));
  }
}

/* Return blocks from cache p to the boundary tag pool */

static void OldZap(p)
struct top *p;
{
  struct top *q;

#if defined(DEBUG_DSA)
  FPRINTF( stderr, "D - IN OldZAP!!!\n" );
#endif

  do {
    for ( q = p->bkwd; q != 0; q = p->bkwd ) {
      p->bkwd = q->bkwd;
      btDeAlloc(q);
      }

    q = p;
    p = p->frwd;
    btDeAlloc(q);
    }
  while( p != 0 );
}


/* "Shape up" the dsa system in a last ditch attempt to avoid deadlock */
/* on data memory.                                                     */

void DsaHelp()
{
  register int pID;

  GETPROCID(pID);

  IncDsaHelp(pID);

  if ( caches[pID].frwd != 0 ) {
    OldZap(caches[pID].frwd); 
    caches[pID].frwd = 0;
    }
}


POINTER Alloc(size)
register int size;
{
  register struct top *cu, *pr;
  register char       *addr;
  int                  pID;

  GETPROCID(pID);
  pr = &caches[pID];

  /* Search for a block of exactly the right size. */
  for ( cu = pr->frwd; cu != 0; cu = cu->frwd ){
    if ( cu->lsize == size ) {
      if (cu->bkwd == 0){
        addr = (char *)((PCMCAST)cu + TOPSIZE);
#if defined(DEBUG_DSA)
        cu->status = 'A';
        dallocs++;
        if (verbose)
          FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC1 );
        dbytes += size;
#endif
        pr->frwd = cu->frwd;
        return( (POINTER) addr);
        }

      /* There are at least two blocks this size. */
      addr = (char *)((PCMCAST)(cu->bkwd) + TOPSIZE);
#if defined(DEBUG_DSA)
      cu->bkwd->status = 'A';
      if (verbose)
        FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC2 );
      dallocs++;
      dbytes += size;
#endif
      cu->bkwd = cu->bkwd->bkwd;
      return( (POINTER) addr);
      }

    pr = cu;
    }

  addr = btAlloc( size );

  if ( addr != 0 ) {
#if defined(DEBUG_DSA)
    if (verbose)
      FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC3 );
    dallocs++;
    dbytes += size;
#endif
    return(  (POINTER) addr );
    }

  /* flush the cache into boundary tag system. NOTE: ALL THE CACHES, */
  /* AND NOT JUST THIS ONE, SHOULD BE FLUSHED.                       */
  DsaHelp();
  
  /* try boundary tag system one more time */
  addr = btAlloc( size );

  if ( addr != 0 ) {
#if defined(DEBUG_DSA)
    if (verbose)
      FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC4 );
    dallocs++;
    dbytes += size;
#endif
    return(  (POINTER) addr );
    }

  SisalError( "Alloc", "ALLOCATION FAILURE: increase -ds value" );

  return NULL;
}


void DeAllocToBt( x )
POINTER x;
{
   btDeAlloc( (struct top *)((PCMCAST)x - TOPSIZE) );
}


void DeAlloc( x )
POINTER x;
{
   register struct top *pr, *cu, *ptr;
   register int size;
   register int pID;

#ifdef DEBUG_DSA
if ( x == NULL ) SisalError( "DeAlloc", "NULL POINTER ON DeAlloc!!!" );
#endif

   ptr  = (struct top *)((PCMCAST)x - TOPSIZE);

   size = ptr->lsize;

#if defined(DEBUG_DSA)
   if ( ptr->status != 'A' ) SisalError( "DeAlloc", "MULTIPLE DEALLOCS!!!" );
   ptr->status = 'C';
   if (verbose)
     FPRINTF( stderr, "%x %d FREE\n", x, size );
   dfrees++;
   dbytes -= size;
#endif

   if ( Sequential )
     pr  = &caches[ptr->PId];
   else {
     GETPROCID(pID);
     pr = &caches[ptr->PId = pID];
     }

   cu = pr->frwd;

   for ( ;; ) {
     if ( cu == 0 ) {
       pr->frwd = ptr;
       ptr->bkwd = 0;
       ptr->frwd = 0;
       return;
       }

     if ( cu->lsize == size ) {
       ptr->bkwd = cu->bkwd;
       cu->bkwd  = ptr;
       return;
       }

     pr = cu;
     cu = pr->frwd;
     }
}

#else /* DIST_DSA */


shared LOCK_TYPE *Dsa_lock;

struct dsa_vars_s
{
	struct bot *p_dsorg;
	struct top *p_zero_bl;
	int *p_zb_start;
	struct top *p_btop;
};

#define dsorg (Private_vars[GetProcId].p_dsorg)
#define zero_bl (Private_vars[GetProcId].p_zero_bl)
#define zb_start (Private_vars[GetProcId].p_zb_start)
#define btop (Private_vars[GetProcId].p_btop)

#if defined(NO_STATIC_SHARED)
struct dsa_vars_s *Private_vars;
#else
struct dsa_vars_s Private_vars[MAX_PROCS];
#endif



#ifdef DEBUG_DSA
int dallocs = 0;
int dfrees  = 0;
int dbytes  = 0;
#endif


void ShutDownDsa()
{
#if defined(DEBUG_DSA)
  FPRINTF( stderr, "D - (ShutDownDsa) Allocs %d Frees %d Lost Bytes %d\n", 
         dallocs, dfrees, dbytes );
#endif
}


InitDsaCaches(size,xft)
int size;
int xft;
{
   int i;
   int roundsize;

   caches  = (struct top*) SharedMalloc( SIZEOF(struct top) * MAX_PROCS );
   Dsa_lock = (LOCK_TYPE *)SharedMalloc(sizeof(LOCK_TYPE));
   MY_INIT_LOCK(Dsa_lock);

#if defined(NO_STATIC_SHARED)
   Private_vars = (struct dsa_vars_s *)SharedMalloc(sizeof(struct dsa_vars_s));
#endif

   for(i=0; i<NumWorkers; i++){
     caches[i].size  = 0;
     caches[i].lsize = 0;
     caches[i].frwd  = 0;
     caches[i].bkwd  = 0;
     bzero(&(Private_vars[i]),sizeof(struct dsa_vars_s));
     }

   xfthresh = xft >= 0 ? xft : 0;
   maxsize   = roundsize - 2 * SIZETAGS;

}


void InitDsa( size, xft )
int size;
int xft;
{
   register struct top *cu, *nx;
   register struct bot *cubot;
   register int         i, inc, roundsize;


   roundsize = ALIGN(int,size);
   /*
    * Need this lock because SharedMalloc must be called from within a
    * locked region.
    */
   MY_LOCK(Dsa_lock);
   zero_bl = (struct top*) SharedMalloc( SIZEOF(struct top));

   zb_start  = (int*) SharedMalloc( SIZEOF(int) );
   *zb_start = 0;


   /* Begin with an allocated bottom tag boundary (whose top part is */
   /* the boundary at the other end of the space!).                  */

   dsorg = (struct bot *) SharedMalloc( roundsize );
   MY_UNLOCK(Dsa_lock);


   /* Surround the space with an allocated bottom tag before and an
      allocated top tag after. */
   btop = (struct top *)((PCMCAST)dsorg + roundsize - TOPSIZE);
   btop->status = 'A'; 
   btop->size   = -1;
   btop->frwd   = btop;
   btop->bkwd   = btop;

   dsorg->top_ptr = btop;

   /* Set up the space between the boundaries as free blocks, with one of  */
   /* the zero size blocks preceding each one, all in a doubly linked list */

   cu = (struct top *)((PCMCAST)dsorg + BOTSIZE);

   inc  = ((int) ((PCMCAST)btop - (PCMCAST)dsorg)) / nr_zero_bl;
   inc -= (inc % ALIGN_SIZE);

   for( i = 0; i < nr_zero_bl - 1; i++ ){
     /* Make a free block at cu */
     nx = (struct top *)((PCMCAST)cu + inc);

     cubot = (struct bot *)((PCMCAST)nx - BOTSIZE);
     cubot->top_ptr = cu;

     cu->status = 'F';
     cu->size   = (int) ((PCMCAST)cubot - (PCMCAST)cu - TOPSIZE);

     /* Link it between zero size blocks */
     zero_bl[i].frwd   = cu;
     zero_bl[i].status = 'F';
     zero_bl[i].size   = 0;


     cu->bkwd = &(zero_bl[i]);
     cu->frwd = &(zero_bl[i+1]);

     zero_bl[i+1].bkwd = cu;
     cu = nx;
     }

  /* Construct last free block and link in */
  cu->frwd   = &(zero_bl[0]);
  cu->bkwd   = &(zero_bl[nr_zero_bl-1]);
  cu->status = 'F';

  cubot = (struct bot *)((PCMCAST)btop - BOTSIZE);
  cubot->top_ptr = cu;

  cu->size = (int) (( (PCMCAST)btop - (PCMCAST)cu ) - SIZETAGS);

  zero_bl[nr_zero_bl-1].frwd   = cu;
  zero_bl[nr_zero_bl-1].status = 'F';
  zero_bl[nr_zero_bl-1].size   = 0;

  zero_bl[0].bkwd = cu;

FLUSHALL;


}

/* Allocate storage from the boundary tag-managed pool */

static char *btAlloc(size)
register int size;
{
   register struct top *cu, *pr, *back;
   struct top          *newtop;
   char                *addr;
   struct bot          *cubot, *newbot;
   int                  lsize;
   int                  pID;

   lsize = size;
   size  = ALIGN(int,size);

   GETPROCID(pID);

   IncStorageUsed(pID,(size+SIZETAGS));
   IncStorageWanted(pID,(size));

   pr = zero_bl;

   back = pr;


   /*
    * flushes in this section are to keep processor from reading stale
    * data on the next go around
    */
   for ( ;; ) {
     cu = pr->frwd;
     FLUSHLINE(&(pr->frwd));

     if ( cu == back ) {
       return( 0 );
       }


     if( (cu->size >= size ) && (cu->status != 'A'))
       break;

     FLUSHLINE(&(cu->size));
     FLUSHLINE(&(cu->status));
     pr = cu;
     }


   if (cu->size - size <= xfthresh + SIZETAGS){
     /* Exact fit (or close enough).  Unlink cu */
     pr->frwd = cu->frwd;

     cu->frwd->bkwd = pr;
     cu->status     = 'A';
     cu->PId        = pID;
     cu->lsize      = lsize;

     addr = (char *) ((PCMCAST)cu + TOPSIZE);
     FLUSHLINE(&(pr->frwd));
     FLUSHLINE(&(cu->frwd->bkwd));
     FLUSH(cu,cu+sizeof(*cu));
   }else{
     /* Split this large block to satisfy request */
     cubot  = (struct bot *)((PCMCAST)cu + cu->size + TOPSIZE);

     addr   = (char *)((PCMCAST)cubot - size);

     newtop = (struct top *)((PCMCAST)addr - TOPSIZE);
     newbot = (struct bot *)((PCMCAST)newtop - BOTSIZE);

     /* Reduce size of cu and make new bottom tag */
     cu->size -= size + SIZETAGS;
     FLUSHLINE(&(cu->size));

     newbot->top_ptr = cu;

     /* Make tags for new allocated block */
     newtop->size   = size;
     newtop->status = 'A';
     newtop->PId    = pID;
     newtop->lsize  = lsize;


     cubot->top_ptr = newtop;
     FLUSH(cubot,cubot+sizeof(*cubot));
     FLUSH(newbot,newbot+sizeof(*newbot));
     FLUSH(newtop,newtop+sizeof(*newtop));
   }


  return(addr);
}


static int btDeAlloc(ptr)
register struct top *ptr;
{
  register struct top *bl_above, *bl_below, *pr, *cu;
  struct bot          *bot_above, *cubot;
  int                  pID;

  GETPROCID(pID);

  bl_below  = (struct top *)((PCMCAST)ptr + ptr->size + SIZETAGS);
  FLUSHLINE(&(ptr->size));
  bot_above = (struct bot *)((PCMCAST)ptr - BOTSIZE);

  /* Become the only coalescing process (sigh) */

  /* Attempt to coalesce the free block below with this block */
  if( bl_below != btop ){
    for (;;) {

      if ( bl_below->status != 'F' ) {
        FLUSHLINE(&(bl_below->status));
	goto tryabove;
        }

      pr = bl_below->bkwd;
      FLUSHLINE(&(bl_below->bkwd));

      if( pr->status == 'F' && pr->frwd == bl_below )
	break;
      FLUSH(pr,pr+sizeof(*pr));
      }

    pr->frwd = bl_below->frwd;

    bl_below->frwd->bkwd = pr;

    cubot = (struct bot *)((PCMCAST)bl_below + bl_below->size + TOPSIZE);
    cubot->top_ptr = ptr;

    ptr->size += bl_below->size + SIZETAGS;

    FLUSHLINE(&(ptr->size));
    FLUSHLINE(&(cubot->top_ptr));
    FLUSH(pr,pr+sizeof(*pr));
    FLUSH(bl_below,bl_below+sizeof(*bl_below));
    }

tryabove:

  /* Attempt to coalesce with the block above */
  if ( bot_above != dsorg ) {
    for (;;) {
      bl_above = bot_above->top_ptr;

      if( (PCMCAST)ptr == ((PCMCAST)bl_above + bl_above->size + SIZETAGS))
        break;
      FLUSHLINE(&(bot_above->top_ptr));
      FLUSHLINE(&(bl_above->size));
      }

    if ( bl_above->status == 'F' ) {
      bl_above->size += ptr->size + SIZETAGS;

      cubot = (struct bot *)((PCMCAST)ptr + ptr->size + TOPSIZE);
      cubot->top_ptr = bl_above;

      FLUSHLINE(&(ptr->size));
      FLUSHLINE(&(cubot->top_ptr));
      FLUSH(bl_above,bl_above+sizeof(*bl_above));

      /* Report new block size */
      return;
      }

    FLUSHLINE(&(bl_above->status));
    }

  /* Cannot merge.  Add this block to the free list */

  pr = zero_bl;

  cu = pr->frwd;


  pr->frwd = ptr;

  ptr->bkwd   = pr;
  ptr->frwd   = cu;
  ptr->status = 'F';

  cu->bkwd = ptr;

  if(pr != cu)
  {
  	FLUSHLINE(&(pr->frwd));
  }
  else
  {
	FLUSH(cu,cu+sizeof(*cu));
  }
  FLUSH(ptr,ptr+sizeof(*ptr));

  if ( pr != cu )
  {
    FLUSH(cu,cu+sizeof(*cu)); 
  }
}

/* Return blocks from cache p to the boundary tag pool */

static int OldZap(p)
struct top *p;
{
  struct top *q;

#if defined(DEBUG_DSA)
  FPRINTF( stderr, "D - IN OldZAP!!!\n" );
#endif

  do {
    for ( q = p->bkwd; q != 0; q = p->bkwd ) {
      p->bkwd = q->bkwd;
      btDeAlloc(q);
      }

    q = p;
    p = p->frwd;
    btDeAlloc(q);
    }
  while( p != 0 );
}


POINTER Alloc(size)
register int size;
{
  register struct top *cu, *pr;
  register char       *addr;
  int                  pID;

  GETPROCID(pID);
  pr = &caches[pID];

  /* Search for a block of exactly the right size. */
  for ( cu = pr->frwd; cu != 0; cu = cu->frwd ){
    if ( cu->lsize == size ) {
      if (cu->bkwd == 0){
        addr = (char *)((PCMCAST)cu + TOPSIZE);
#if defined(DEBUG_DSA)
        cu->status = 'A';
        dallocs++;
        if (verbose)
          FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC5 );
        dbytes += size;
#endif
        pr->frwd = cu->frwd;
        return( (POINTER) addr);
        }

      /* There are at least two blocks this size. */
      addr = (char *)((PCMCAST)(cu->bkwd) + TOPSIZE);
#if defined(DEBUG_DSA)
      cu->bkwd->status = 'A';
      if (verbose)
        FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC6 );
      dallocs++;
      dbytes += size;
#endif
      cu->bkwd = cu->bkwd->bkwd;
      return( (POINTER) addr);
      }

    pr = cu;
    }

  addr = btAlloc( size );

  if ( addr != 0 ) {
#if defined(DEBUG_DSA)
    if (verbose)
      FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC7 );
    dallocs++;
    dbytes += size;
#endif
    return(  (POINTER) addr );
    }

  /* flush the cache into boundary tag system. NOTE: ALL THE CACHES, */
  /* AND NOT JUST THIS ONE, SHOULD BE FLUSHED.                       */
  DsaHelp();
  
  /* try boundary tag system one more time */
  addr = btAlloc( size );

  if ( addr != 0 ) {
#if defined(DEBUG_DSA)
    if (verbose)
      FPRINTF( stderr, "%x %d %s\n", addr, size, ALLOC8 );
    dallocs++;
    dbytes += size;
#endif
    return(  (POINTER) addr );
    }

  SisalError( "Alloc", "ALLOCATION FAILURE: increase -ds value" );

  return NULL;
}


/* "Shape up" the dsa system in a last ditch attempt to avoid deadlock */
/* on data memory.                                                     */

int DsaHelp()
{
  register int pID;

  GETPROCID(pID);

  IncDsaHelp(pID);

  if ( caches[pID].frwd != 0 ) {
    OldZap(caches[pID].frwd); 
    caches[pID].frwd = 0;
    }
}


void DeAllocToBt( x )
POINTER x;
{
   btDeAlloc( (struct top *)((PCMCAST)x - TOPSIZE) );
}


void DeAlloc( x )
POINTER x;
{
   register struct top *pr, *cu, *ptr;
   register int size;
   register int pID;

#ifdef DEBUG_DSA
if ( x == NULL ) SisalError( "DeAlloc", "NULL POINTER ON DeAlloc!!!" );
#endif

   ptr  = (struct top *)((PCMCAST)x - TOPSIZE);

   size = ptr->lsize;

#if defined(DEBUG_DSA)
   if ( ptr->status != 'A' ) SisalError( "DeAlloc", "MULTIPLE DEALLOCS!!!" );
   ptr->status = 'C';
   if (verbose)
     FPRINTF( stderr, "%x %d FREE\n", x, size );
   dfrees++;
   dbytes -= size;
#endif

   if ( Sequential )
     pr  = &caches[ptr->PId];
   else {
     GETPROCID(pID);
     pr = &caches[ptr->PId = pID];
     }

   cu = pr->frwd;

   for ( ;; ) {
     if ( cu == 0 ) {
       pr->frwd = ptr;
       ptr->bkwd = 0;
       ptr->frwd = 0;
       return;
       }

     if ( cu->lsize == size ) {
       ptr->bkwd = cu->bkwd;
       cu->bkwd  = ptr;
       return;
       }

     pr = cu;
     cu = pr->frwd;
     }
}


#endif /* DIST_DSA */
