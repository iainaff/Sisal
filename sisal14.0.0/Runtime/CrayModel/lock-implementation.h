/************************************************************************\
 * Cray
\************************************************************************/
#define LOCKSDEFINED

typedef int LOCK_TYPE;
typedef int BARRIER_TYPE;

extern LOCK_TYPE TheFirstLock;

#define MY_SLOCK(lp)          while ( LOCKTEST(lp) != 0 )
#define MY_SUNLOCK(lp)        LOCKOFF(lp)
#define MY_SINIT_LOCK(lp)     LOCKASGN(lp)

#ifdef CRAYXY
#define MY_LOCK(lp)           _semts(31)
#define MY_UNLOCK(lp)         { _cmr(); _semclr(31); }
#define MY_INIT_LOCK(lp)
#define MY_LOCK_BACKUP(lp)    _semts(31)
#define MY_UNLOCK_BACKUP(lp)  { _cmr(); _semclr(31); }
#define MY_LOCKASGN           { LOCKASGN(&TheFirstLock); _semclr(31); }
#endif

#ifdef CRAY2
#define MY_LOCK(lp)           _getsem()
#define MY_UNLOCK(lp)         _csm()
#define MY_INIT_LOCK(lp)
#define MY_LOCK_BACKUP(lp)    _getsem()
#define MY_UNLOCK_BACKUP(lp)  _csm()
#define MY_LOCKASGN           { LOCKASGN(&TheFirstLock); _csm(); }
#endif

#ifdef CRAYT3D
#define MY_LOCK(lp)           while ( LOCKTEST(&TheFirstLock) != 0 )
#define MY_UNLOCK(lp)         LOCKOFF(&TheFirstLock)
#define MY_INIT_LOCK(lp)
#define MY_LOCK_BACKUP(lp)    while ( LOCKTEST(&TheFirstLock) != 0 )
#define MY_UNLOCK_BACKUP(lp)  LOCKOFF(&TheFirstLock)
#define MY_LOCKASGN           { LOCKASGN(&TheFirstLock); }
#endif

#define INIT_BARRIER(b,limit)  BARASGN(b,&limit)
#define WAIT_BARRIER(b)        BARSYNC(b)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL
