/* PThreadsModel */

#define GetProcId  p_procnum
#define GETPROCID(x) x = GetProcId
extern BARRIER_TYPE *MyInitBarrier PROTO((int));
