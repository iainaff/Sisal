/************************************************************************\
 * Sequential UNIX
\************************************************************************/
#define LOCKSDEFINED

typedef unsigned char LOCK_TYPE;
typedef int           BARRIER_TYPE;

#define	MY_SLOCK(lp) {if (*lp == 'L') SisalError("","MY_SLOCK FAILED"); *lp = 'L'; }
#define MY_SUNLOCK(lp) {*lp = 'U';}
#define MY_SINIT_LOCK(lp) {*lp = 'U';}

#define	MY_LOCK(lp) {if (*lp == 'L') SisalError("","MY_LOCK FAILED"); *lp = 'L'; }
#define MY_UNLOCK(lp) {*lp = 'U';}
#define MY_INIT_LOCK(lp) {*lp = 'U';}

#define MY_LOCK_BACKUP(lp) {if (*lp == 'L') SisalError("","MY_LOCK_BACKUP FAILED"); *lp = 'L';}
#define MY_UNLOCK_BACKUP(lp) {*lp = 'U';}

#define INIT_BARRIER(b,lm)
#define WAIT_BARRIER(b)

#define FLUSHALL
#define FLUSH(start,end)
#define FLUSHLINE(addr)
#define CACHESYNC
#define FFL
