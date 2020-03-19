/* sisal.h,v
 * Revision 12.7  1992/11/04  22:05:04  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:06  miller
 * Initial RCS Version by Cann
 * */

#include "sisalrt.h"

#define Nil   0

#define FUNCTION    POINTER
#define Error(x,y)  SisalError( x, y )

#define ProgressMessage fprintf( stderr, " -P-\n" ); fflush( stderr )


/* CRAY BUG WORK AROUND */

#define MyBCopy(x,y,z) \
{ \
  if ( z != 0 ) \
    memmove(y,x,z); \
}


/* ------------------------------------------------------------ */
/* TRACING */
#define Trace(writer,f) \
{void writer(); static int _ID = 0; _Tracer_(args,writer,f,&_ID); }

/* ************   MISCELLANEOUS  MACROS   ************ */

#define etothe exp

#define LockParent    MY_LOCK( UtilityLock )
#define UnlockParent  MY_UNLOCK( UtilityLock )

#define SLockParent    MY_SLOCK( SUtilityLock )
#define SUnlockParent  MY_SUNLOCK( SUtilityLock )

/*
** Spawn( x = loop type, y = child address, z = task frame, w = lower bound,
**        u = upper bound, size, norm )
** OptSpawn( tt = task frame structure type ...)
** BSlices( x,			SIMPLE or COMPLEX spawn
	    tt,			Frame type for casting
	    y,			Child body function
	    z,			Frame
	    w,			Low value
	    u,			High value
	    Min Slice Size,	Minimum acceptable slice
	    LoopSlices,		Number of slices to make
            size,               LCM of loop output sizes
            norm                normalization distance
	  )
*/
#define Spawn(x,y,z,w,u,Size, Norm)  SpawnSlices(x,y,(POINTER)z,w,u, Size, Norm)

#define OptSpawn(tt,x,y,z,w,u, Size, Norm) \
    OptSpawnSlices(((tt*)z)->FirstAR,((tt*)z)->Count)

#define OptSpawnFast(tt,x,y,z,w,u, Size, Norm) \
    OptSpawnSlicesFast(((tt*)z)->FirstAR,((tt*)z)->Count)

#define BSlices(x,tt,y,z,w,u,MinSlice, LoopSlice, Size, Norm) \
    BuildSlices(x,		       /* Type of Spawn */ \
		&(((tt*)z)->FirstAR),  /* To return location of first slice */\
		&(((tt*)z)->Count),    /* To return number of slices made */ \
		y,		       /* Child Code */ \
		(POINTER)z,	       /* Task frame block */ \
		w,		       /* Low value */ \
		u,		       /* High value */ \
		MinSlice,	       /* Min. accepatable slice */ \
		LoopSlices,	       /* How many slices to attempt */ \
		Size, \
		Norm)

/* ------------------------------------------------------------ */
#define BlockSpawn(x,y,z,w,u)    SpawnBlockSlices(y,(POINTER)z,w,u)
#define BlockBSlices(x,tt,y,z,w,u,MinSlice,LoopSlice, Size, Norm) \
    BuildBlockSlices(		/* Don't need loop type */ \
		     &(((tt*)z)->FirstAR), \
		     &(((tt*)z)->Count), \
		     y, \
		     (POINTER)z, \
		     w, \
		     u, \
		     MinSlice, \
		     LoopSlice)

/* for POWER4 */
/* ------------------------------------------------------------ */
#define CachedSpawn(x,y,z,w,u,s,Size,Norm) \
		SpawnCachedSlices(y,(POINTER)z,w,u,s,Size,Norm)
#define CachedBSlices(x,tt,y,z,w,u,MinSlice,LoopSlice,Size, Norm) \
    BuildCachedSlices(		/* Don't need loop type */ \
		     &(((tt*)z)->FirstAR), \
		     &(((tt*)z)->Count), \
		     y, \
		     (POINTER)z, \
		     w, \
		     u, \
		     MinSlice, \
		     LoopSlice, \
		     Size, \
		     Norm)

/* ------------------------------------------------------------ */
#define StridedSpawn(x,y,z,w,u)    SpawnStridedSlices(y,(POINTER)z,w,u)
#define StridedBSlices(x,tt,y,z,w,u,MinSlice,LoopSlice, Size, Norm) \
    BuildStridedSlices(		/* Don't need loop type */ \
		     &(((tt*)z)->FirstAR), \
		     &(((tt*)z)->Count), \
		     y, \
		     (POINTER)z, \
		     w, \
		     u, \
		     MinSlice, \
		     LoopSlice)

/* ------------------------------------------------------------ */
#define GSSSpawn(x,y,z,w,u)    SpawnGssSlices(y,(POINTER)z,w,u)
#define GSSBSlices(x,tt,y,z,w,u,MinSlice,LoopSlice, Size, Norm) \
    BuildGssSlices(		/* Don't need loop type */ \
		     &(((tt*)z)->FirstAR), \
		     &(((tt*)z)->Count), \
		     y, \
		     (POINTER)z, \
		     w, \
		     u, \
		     MinSlice, \
		     LoopSlice)

/* x = sisal function; y = formal arguments */
#define Call(x,y)  x( y )

/*
** IncRefCount( x = target; y = x's type; z = increment ) 
** SetRefCount( ... )
*/
#define IncRefCount(x,y,z) \
{ \
  MY_LOCK( &((y*)x)->Mutex ); \
  ((y*)x)->RefCount += z; \
  FLUSHLINE(&(((y*)x)->RefCount)); \
  CACHESYNC;\
  MY_UNLOCK( &((y*)x)->Mutex );\
  FLUSHLINE( &((y*)x)->Mutex );\
}

#define SetRefCount(x,y,z)  ((y*)x)->RefCount = z


/* ************ DEBUG TOKEN DUMP ROUTINES ************ */


static void IntDump( Value, Name )
int   Value;
char *Name;
{
  fprintf( stderr, "TOKEN: (%s,val=%d)\n", Name, Value );
}

static void RealDump( Value, Name )
double  Value;
char   *Name;
{
  fprintf( stderr, "TOKEN: (%s,val=%lf)\n", Name, Value );
}

static void ArrayDump( Ptr, Name )
POINTER  Ptr;
char    *Name;
{
  register ARRAYP Array = (ARRAYP) Ptr;

  fprintf( stderr, "TOKEN: (%s,lo=%d,size=%d)\n",
	   Name, Array->LoBound, Array->Size   );
  fprintf( stderr, "Array: 0x%x (%d), sizeof: %d\n",Array,Array,sizeof(*Array));
  fprintf( stderr, "  LoBound: %d (0x%x)\n",Array->LoBound,Array->LoBound);
  fprintf( stderr, "  Size: %d (0x%x)\n",Array->Size,Array->Size);
  fprintf( stderr, "  Phys: 0x%x (%d)\n",Array->Phys,Array->Phys);
  fprintf( stderr, "  Mutex: %d\n",Array->Mutex);
  fprintf( stderr, "  RefCount: %d\n",Array->RefCount);
  fprintf( stderr, "  Mutable: %d\n",Array->Mutable);
  fflush(stderr);
}


/*
** DivByZeroCheck( u = op1, v = op2, w = na1, x = na2, y = message )
** SdbxDivByZeroCheck( v = op2 )
*/
#define DivByZeroCheck(u,v,w,x,y) \
{ \
  if ( 0 == v ) { \
    RealDump( (double) u, w ); \
    RealDump( (double) v, x ); \
    SisalError( y, "DIVISION BY ZERO!" ); \
    } \
}

#define SdbxDivByZeroCheck(v) \
{ \
  if ( 0 == v ) \
    SdbxMonitor( SDBX_DB0 ); \
}


/* ************   FUNCTION TIMER MACROS   ************ */


#ifndef StartFunctionTimer
#define StartFunctionTimer(x) \
  double Et1,Et2; \
  Et1 = TSECND();
#endif

#ifndef StopFunctionTimer
#define StopFunctionTimer(x) \
  Et2 = TSECND(); \
  LockParent; \
  fprintf( stderr, "%s: Execution Time = %lf\n", (x), Et2-Et1 ); \
  fflush( stderr ); \
  UnlockParent;
#endif

/* ************       RECORD MACROS      ************ */


/* x = pointer target; y = pointer source; z = structure name */
#define CRNoOp(x,y,z) \
{ \
  IncRNoOpAttempts; \
  if ( ((z*)y)->RefCount != 1 ) { \
    IncRNoOpCopies; \
    IncDataCopies( sizeof( z ) ); \
    x = Alloc( sizeof( z ) ); \
    *((z*)x) = *((z*)y); \
    MY_INIT_LOCK( &((z*)x)->Mutex )
	
/* x = pointer target; y = pointer source */
#define EndCRNoOp(x,y) \
    } \
  else \
    x = y; \
}

/* x = pointer target; y = pointer source; z = structure name */
#define RNoOp(x,y,z) \
{ \
  IncRNoOpAttempts; \
  IncRNoOpCopies; \
  IncDataCopies( sizeof( z ) ); \
  x = Alloc( sizeof( z ) ); \
  *((z*)x) = *((z*)y); \
  MY_INIT_LOCK( &((z*)x)->Mutex ); \
}

/* x = pointer target; y = structure name; z = initial reference count */
#define RBld(x,y,z) \
{ \
  IncRBuilds; \
  x = Alloc( sizeof( y ) ); \
  MY_INIT_LOCK( &((y*)x)->Mutex ); \
  ((y*)x)->RefCount = z; \
}

/* w = union tag */
#define UBld(x,y,z,w) \
{ \
  x = Alloc( sizeof( y ) ); \
  MY_INIT_LOCK( &((y*)x)->Mutex ); \
  ((y*)x)->RefCount = z; \
  ((y*)x)->Tag = w; \
}


/* ************       BASIC MACROS       ************ */


/* x = target, y = operand 1; z = operand 2, w = boolean (if given) */
#define OptMod(x,y,z) x %= z
#define Mod(x,y,z)    x  = y % z

#define OptMinus(x,y,z) x -= z
#define Minus(x,y,z)    x  = y - z

static int DivByZero() { return( 0 ); }

#define OptDivZero(x,y,z)  x /= DivByZero()
#define DivZero(x,y,z)     x  = y / DivByZero()

#define OptDiv(x,y,z)      x /= z
#define Div(x,y,z)         x  = y / z

#define OptDiv2(x,y,z)     x  >>= 1
#define Div2(x,y,z)        x  = y >> 1

#define BOptOr(x,y,z,w)    if ( w ) x |= z
#define COptOr(x,y,z)      x |= y
#define OptOr(x,y,z)       x |= z
#define Or(x,y,z)          x  = y | z

#define BOptPlus(x,y,z,w)  if ( w ) x += z
#define COptPlus(x,y,z)    x += y
#define OptPlus(x,y,z)     x += z
#define Plus(x,y,z)        x  = y + z

#define BOptAnd(x,y,z,w)   if ( w ) x &= z
#define COptAnd(x,y,z)     x &= y
#define OptAnd(x,y,z)      x &= z
#define And(x,y,z)         x  = y & z

#define BOptTimes(x,y,z,w) if ( w ) x *= z
#define COptTimes(x,y,z)   x *= y
#define OptTimes(x,y,z)    x *= z
#define Times(x,y,z)       x  = y * z

#define BOptMin(x,y,z,w)   if ( w ) if ( y > z ) y = z
#define COptMin(x,y,z)     if ( z > y ) z = y
#define OptMin(x,y,z)      if ( y > z ) y = z
#define Min(x,y,z)         if ( y > z ) x = z; else x = y

#define BOptMax(x,y,z,w)   if ( w ) if ( y < z ) y = z
#define COptMax(x,y,z)     if ( z < y ) z = y
#define OptMax(x,y,z)      if ( y < z ) y = z
#define Max(x,y,z)         if ( y < z ) x = z; else x = y

#define NotE(x,y,z)        x = (y != z)
#define LessE(x,y,z)       x = (y <= z)
#define Less(x,y,z)        x = (y < z)
#define Equal(x,y,z)       x = (y == z)
#define Great(x,y,z)       x = (y > z)
#define GreatEqual(x,y,z)  x = (y >= z)

#define IPow(x,y,z)        x = rint(pow( (double) y, (double) z ))
#define Pow(x,y,z)         x = pow( (double) y, (double) z )

/* x = target; y = operand */
#define Neg(x,y)         x = -(y)
#define Not(x,y)         x = !(y)
#define Bool(x,y)        x = (char) y
#define Char(x,y)        x = (char) y
#define Dbl(x,y)         x = y
#define Trunc(x,y)       x = y
#define Sngle(x,y)       x = y
#define Int(x,y)         x = y
#define Floor(x,y)       x = floor( (double) y )

#define OptAbs(x,y)      if ( x < 0 ) x = -(x)
#define Abs(x,y)         if ( y < 0 ) x = -(y); else x = y

#if ALLAINT || CRAY || SGI
#define OptFAbs(x,y)      x = fabs(x) 
#define FAbs(x,y)         x = fabs(y)
#else
#define OptFAbs(x,y)      if ( x < 0 ) x = -(x)
#define FAbs(x,y)         if ( y < 0 ) x = -(y); else x = y
#endif


/* *************     ARRAY  MACROS     ************* */


/*
** InitDV( x = target array, rs = result size, lb = lower bound w = buffer,
**         q = component type ) XX
*/
#define InitDV(x,rs,lb,w,q) \
{ \
  ((ARRAYP)x)->Size = rs; \
  ((ARRAYP)x)->LoBound = lb; \
  ((ARRAYP)x)->Phys = w->Phys; \
  ((ARRAYP)x)->Base = (POINTER) (((q*)(w->Base))-lb); \
}


/*
** BoundsCheck1( z = array, u=array na, v=index na,  w = index, x = message )
** BoundsCheck2( .. )
** SdbxBoundsCheck1( z = array, w = index, ) 
** SdbxBoundsCheck2( z = array ) 
*/
#define BoundsCheck1(z,u,v,w,x) \
{ \
  if ( ((ARRAYP)z)->LoBound > (w) ) { \
    ArrayDump( z, u ); IntDump( w, v ); \
    SisalError( (x), "ARRAY SUBSCRIPT VIOLATION [LOW]" ); \
    } \
  if ( ((ARRAYP)z)->LoBound + ((ARRAYP)z)->Size <= (w) ) { \
    ArrayDump( z, u ); IntDump( w, v ); \
    SisalError( (x), "ARRAY SUBSCRIPT VIOLATION [HIGH]" ); \
    } \
}

#define BoundsCheck2(z,u,x) \
{ \
  if ( ((ARRAYP)z)->Size <= 0 ) { \
    ArrayDump( z, u ); \
    SisalError( (x), "ARRAY SUBSCRIPT VIOLATION ON REMOVAL" ); \
    } \
}

#define SdbxBoundsCheck1(z,w) \
{ \
  if ( ((ARRAYP)z)->LoBound > (w) ) \
    SdbxMonitor( SDBX_BDS ); \
  if ( ((ARRAYP)z)->LoBound + ((ARRAYP)z)->Size <= (w) ) \
    SdbxMonitor( SDBX_BDS ); \
}

#define SdbxBoundsCheck2(z) \
{ \
  if ( ((ARRAYP)z)->Size <= 0 ) \
    SdbxMonitor( SDBX_BDS ); \
}


/* 
** ALimL( x = target, y = array ) XX
** ALimH( ... ) XX
** ASize( ... ) XX
*/
#define ALimL(x,y) x = ((ARRAYP)y)->LoBound
#define ALimH(x,y) x = ((ARRAYP)y)->LoBound + ((ARRAYP)y)->Size - 1
#define ASize(x,y) x = ((ARRAYP)y)->Size


/* 
** ASetL( x = component type, y = target array, z = not used, w = lower bound ) XX
** AAdj( ..., q = upper bound ) XX
*/
#define ASetL(x,y,z,w) \
{ \
  ((ARRAYP)y)->Base = (POINTER) \
	              (((x*)(((ARRAYP)y)->Base))+(((ARRAYP)y)->LoBound-w)); \
  ((ARRAYP)y)->LoBound = w; \
}

#define AAdj(x,y,z,w,q) \
{ \
  register int size; \
  ((ARRAYP)y)->LoBound = w; \
  if ( (size = q - w + 1) < 0 ) \
    ((ARRAYP)y)->Size = 0; \
  else \
    ((ARRAYP)y)->Size = size; \
}


/* 
** AElm( x = component type, y = target, z = array, w = index ) XX
*/
#define AElm(x,y,z,w) y = ((x*)(((ARRAYP)z)->Base))[w]

/* 
** OptAElm( x = component type, y = target, z = base pointer, w = index ) XX
*/
#define OptAElm(x,y,z,w) y = ((x*)z)[w]

/* 
** GABase( x = component type, y = target, z = array ) XX
*/
#define GABase(x,y,z) y = (x) (((ARRAYP)z)->Base)

/*
** ARElm( x = component type, y = target, z = array, w = index, _field ) XX
*/
#define ARElm(x,y,z,w,_field) y = ((x*)(((ARRAYP)z)->Base))[w]._field


/*
** ARemH( x = component type, y = target array, z = not used ) XX
** OptARemH( ... ) XX
** ARemL( ... ) XX
** OptARemL( ... ) XX
*/
#define ARemH(x,y,z) \
{ \
  register PHYSP Phys = ((ARRAYP)y)->Phys; \
  ((ARRAYP)y)->Size--; \
  if ( Phys->RefCount == 1 ) \
    Phys->Free++; \
}

#define OptARemH(x,y,z) { ((ARRAYP)y)->Size--; ((ARRAYP)y)->Phys->Free++; }
#define OptARemL(x,y,z) { ((ARRAYP)y)->LoBound++; ((ARRAYP)y)->Size--; }
#define ARemL(x,y,z)    { ((ARRAYP)y)->LoBound++; ((ARRAYP)y)->Size--; }


/*
** PANoOp( x = target array, y = source array, z = free function ) XX
** PCANoOp( .. ) XX
*/
#define PANoOp(x,y,z) \
{ \
  x = (POINTER) Alloc( sizeof(ARRAY) ); \
  *((ARRAYP)x) = *((ARRAYP)y); \
  ((ARRAYP)x)->RefCount = 1; \
  MY_INIT_LOCK( &((ARRAYP)x)->Mutex ); \
  MY_LOCK( &((ARRAYP)x)->Phys->Mutex ); \
  ((ARRAYP)x)->Phys->RefCount++; \
  FLUSHLINE(&(((ARRAYP)x)->Phys->RefCount)); \
  CACHESYNC;\
  MY_UNLOCK( &((ARRAYP)x)->Phys->Mutex ); \
  FLUSHLINE( &((ARRAYP)x)->Phys->Mutex ); \
  z( y ); \
}

#define PCANoOp(x,y,z) \
{ \
  if ( ((ARRAYP)y)->RefCount != 1 ) { \
    x = (POINTER) Alloc( sizeof(ARRAY) ); \
    *((ARRAYP)x) = *((ARRAYP)y); \
    ((ARRAYP)x)->RefCount = 1; \
    MY_INIT_LOCK( &((ARRAYP)x)->Mutex ); \
    MY_LOCK( &((ARRAYP)x)->Phys->Mutex ); \
    ((ARRAYP)x)->Phys->RefCount++; \
    FLUSHLINE(&(((ARRAYP)x)->Phys->RefCount)); \
    CACHESYNC;\
    MY_UNLOCK( &((ARRAYP)x)->Phys->Mutex ); \
    FLUSHLINE( &((ARRAYP)x)->Phys->Mutex ); \
    z( y ); \
    } \
  else \
    x = y; \
}


/*
** ABElmCpy0( x = copy function, y = buffer ptr, z = component type, 
**            w = source array, cs = copy size ) XX
** ABElmCpy1( ... ) XX
** ABElmCpyN( ..., rs = result size ) XX
*/
#define ABElmCpy0(x,y,z,w,cs) \
{ \
  IncDataCopies( (cs) * sizeof( z ) ); \
  x(y->Base,(POINTER)(((z*)(((ARRAYP)w)->Base))+((ARRAYP)w)->LoBound),cs); \
}

#define ABElmCpy1(x,y,z,w,cs) \
{ \
  IncDataCopies( (cs) * sizeof( z ) ); \
  x( (POINTER)(((z*)(y->Base))+1), \
     (POINTER)(((z*)(((ARRAYP)w)->Base))+((ARRAYP)w)->LoBound), cs ); \
}

#define ABElmCpyN(x,y,z,w,rs) \
{ \
  register int sz1; \
  register int sz2; \
  sz2 = ((ARRAYP)w)->Size; \
  sz1 = rs - sz2; \
  IncDataCopies( sz2 * sizeof( z ) ); \
  x((POINTER)(((z*)(y->Base))+ sz1), \
    (POINTER)(((z*)(((ARRAYP)w)->Base))+((ARRAYP)w)->LoBound), sz2 ); \
}


/* 
** AAElmCpy( x = copy function, y = target array, z = source array,
**           w = component type )
*/
#define AAElmCpy(x,y,z,w) \
{ \
  IncDataCopies( ((ARRAYP)z)->Size * sizeof(w) ); \
  x( ((ARRAYP)y)->Phys->Base, (POINTER) (((w*)(((ARRAYP)z)->Base)) + \
     ((ARRAYP)z)->LoBound), ((ARRAYP)z)->Size ); \
}


/*
** DoABld( ... ) XX
*/
#define DoABld(x,y,z) \
{ \
  ABld( x, y, z ); \
  IncATAttempts; \
  IncATCopies; \
}

/*
** AddHAT( x = target array, y = source array, z = value, w = buffer,
**         q = copy function name, v = component type, rs = result size,
**         lb = result lower bound ) YY
** AddLAT( ... ) YY
** PAddLAT( ... ) YY
** PAddHAT( ... ) YY
**
** AddHATDVI( ... ) RR
** PAddHATDVI( ... ) RR
** AddLATDVI( ... ) RR
** PAddLATDVI( ... ) RR
**
** AddHATDV( ... ) RR
** PAddHATDV( ... ) RR
** AddLATDV( ... ) RR
** PAddLATDV( ... ) RR
**
** OptAddHAT( ... ) XX
** OptPAddHAT( ... ) XX
** OptAddLAT( ... ) XX
** OptPAddLAT( ... ) XX
*/

#define AddHAT(x,y,z,w,q,v,rs,lb) \
{ \
  x = (POINTER) Alloc( sizeof(ARRAY) ); \
  IncATAttempts; \
  IncATCopies; \
  InitDV(x,rs,lb,w,v); \
  ((v*)(w->Base))[(rs)-1] = z; \
  MY_INIT_LOCK( &(((ARRAYP)x)->Mutex) ); \
  w->Phys->RefCount++; \
  ABElmCpy0(q,w,v,y,rs-1); \
}

#define AddHATDVI(x,y,z,w,q,v,rs,lb) \
{ \
  x = w->Phys->Dope; \
  IncATAttempts; \
  IncATCopies; \
  InitDV(x,rs,lb,w,v); \
  ((v*)(w->Base))[(rs)-1] = z; \
  ABElmCpy0(q,w,v,y,rs-1); \
}

#define AddHATDV(x,y,z,w,q,v,rs,lb) \
{ \
  x = w->Phys->Dope; \
  IncATAttempts; \
  IncATCopies; \
  ((v*)(w->Base))[(rs)-1] = z; \
  ABElmCpy0(q,w,v,y,rs-1); \
}

#define OptAddHAT(x,y,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  IncATCopies; \
  ((v*)(w->Base))[(rs)-1] = z; \
  ABElmCpy0(q,w,v,y,rs-1); \
}

#define AddLAT(x,y,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  IncATCopies; \
  x = (POINTER) Alloc( sizeof(ARRAY) ); \
  InitDV(x,rs,lb,w,v); \
  *((v*)(w->Base)) = z; \
  MY_INIT_LOCK( &(((ARRAYP)x)->Mutex) ); \
  w->Phys->RefCount++; \
  ABElmCpy1(q,w,v,y,rs-1); \
}

#define AddLATDVI(x,y,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  IncATCopies; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,v); \
  *((v*)(w->Base)) = z; \
  ABElmCpy1(q,w,v,y,rs-1); \
}

#define AddLATDV(x,y,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  IncATCopies; \
  x = w->Phys->Dope; \
  *((v*)(w->Base)) = z; \
  ABElmCpy1(q,w,v,y,rs-1); \
}

#define OptAddLAT(x,y,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  IncATCopies; \
  *((v*)(w->Base)) = z; \
  ABElmCpy1(q,w,v,y,rs-1); \
}

#define PAddLAT(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  InitDV(x,rs,lb,w,v); \
  *((v*)(w->Base)) = z; \
}

#define PAddLATDVI(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,v); \
  *((v*)(w->Base)) = z; \
}

#define PAddLATDV(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  x = w->Phys->Dope; \
  *((v*)(w->Base)) = z; \
}

#define OptPAddLAT(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  *((v*)(w->Base)) = z; \
}

#define PAddHAT(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  ((v*)(w->Base))[(rs)-1] = z; \
  ((ARRAYP)x)->Size = rs; \
}

#define PAddHATDVI(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,v); \
  ((v*)(w->Base))[(rs)-1] = z; \
}

#define PAddHATDV(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  x = w->Phys->Dope; \
  ((v*)(w->Base))[(rs)-1] = z; \
}

#define OptPAddHAT(x,z,w,q,v,rs,lb) \
{ \
  IncATAttempts; \
  ((v*)(w->Base))[(rs)-1] = z; \
}

/*
** AAddH( x = target array, y = source array, z = value, q = copy function, 
**        v = component type, u = array dealloc function ) XX
** AAddHX( ..., u = array dealloc function, w = component dealloc function ) XX
**        
*/
#define AAddH(x,y,z,q,v,u) \
{ \
  register PHYSP Phys = ((ARRAYP)y)->Phys; \
  register v *vaddr; \
  IncATAttempts; \
  if ( ((ARRAYP)y)->RefCount == 1 && Phys->RefCount == 1 && Phys->Free > 0 ) \
    x = y; \
  else { \
    IncATCopies; \
    x = ArrayDuplicatePlus( y, sizeof(v) ); \
    AAElmCpy( q, x, y, v ); \
    u( y ); \
    Phys = ((ARRAYP)x)->Phys; \
    } \
  vaddr = ((v*)((ARRAYP)x)->Base)+((ARRAYP)x)->LoBound+(((ARRAYP)x)->Size++); \
  *vaddr = z; \
  if ( vaddr >= (((v*)Phys->Base) + Phys->Size) ) \
    Phys->Size++; \
  Phys->Free--; \
}


#define AAddHX(x,y,z,q,v,u,w) \
{ \
  register PHYSP Phys = ((ARRAYP)y)->Phys; \
  register v *vaddr; \
  IncATAttempts; \
  if ( ((ARRAYP)y)->RefCount == 1 && Phys->RefCount == 1 && Phys->Free > 0 ) \
    x = y; \
  else { \
    IncATCopies; \
    x = ArrayDuplicatePlus( y, sizeof(v) ); \
    AAElmCpy( q, x, y, v ); \
    u( y ); \
    Phys = ((ARRAYP)x)->Phys; \
    } \
  vaddr = ((v*)((ARRAYP)x)->Base)+((ARRAYP)x)->LoBound+(((ARRAYP)x)->Size++); \
  if ( vaddr < (((v*)Phys->Base) + Phys->Size) ) \
    w( *vaddr ); \
  else \
    Phys->Size++; \
  *vaddr = z; \
  Phys->Free--; \
}



/*
** CCCatAT( x = target array, y = first source array, z = second source array,
**          w = buffer, v = copy function, q = component type,
**          rs = result size, lb = result lower bound ) YY
** PCCatAT( ... ) YY
** CPCatAT( ... ) YY
** PPCatAT( ... ) YY
**
** CCCatATDVI( ... ) LL
** PCCatATDVI( ... ) LL
** CPCatATDVI( ... ) LL
** PPCatATDVI( ... ) LL
**
**
** CCCatATDV( ... ) LL
** PCCatATDV( ... ) LL
** CPCatATDV( ... ) LL
** PPCatATDV( ... ) LL
**
** OptCCCatAT( ... ) XX
** OptPCCatAT( ... ) XX
** OptCPCatAT( ... ) XX
** OptPPCatAT( ... ) XX
*/
#define CCCatAT(x,y,z,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  IncATCopies; \
  x = (POINTER) Alloc( sizeof(ARRAY) ); \
  InitDV(x,rs,lb,w,q); \
  MY_INIT_LOCK( &(((ARRAYP)x)->Mutex) ); \
  w->Phys->RefCount++; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
  ABElmCpyN(v,w,q,z,rs); \
}

#define CCCatATDVI(x,y,z,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  IncATCopies; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,q); \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
  ABElmCpyN(v,w,q,z,rs); \
}

#define CCCatATDV(x,y,z,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  IncATCopies; \
  x = w->Phys->Dope; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
  ABElmCpyN(v,w,q,z,rs); \
}

#define OptCCCatAT(x,y,z,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  IncATCopies; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
  ABElmCpyN(v,w,q,z,rs); \
}

#define PCCatAT(x,z,w,v,q,rs,lb) \
{ \
  ABElmCpyN(v,w,q,z,rs); \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  ((ARRAYP)x)->Size = rs; \
}

#define PCCatATDVI(x,z,w,v,q,rs,lb) \
{ \
  ABElmCpyN(v,w,q,z,rs); \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,q); \
}

#define PCCatATDV(x,z,w,v,q,rs,lb) \
{ \
  ABElmCpyN(v,w,q,z,rs); \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  x = w->Phys->Dope; \
}

#define OptPCCatAT(x,z,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  ABElmCpyN(v,w,q,z,rs); \
}

#define CPCatAT(x,y,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  InitDV(x,rs,lb,w,q); \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
}

#define CPCatATDVI(x,y,w,v,q,rs,lb) \
{ \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,q); \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
}

#define CPCatATDV(x,y,w,v,q,rs,lb) \
{ \
  x = w->Phys->Dope; \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
}

#define OptCPCatAT(x,y,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  IncATCopies; \
  ABElmCpy0(v,w,q,y,((ARRAYP)y)->Size); \
}

#define PPCatAT(x,w,v,q,rs,lb) \
{ \
  ((ARRAYP)x)->Size = rs; \
  IncATAttempts; \
  IncATAttempts; \
}

#define PPCatATDVI(x,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  x = w->Phys->Dope; \
  InitDV(x,rs,lb,w,q); \
}

#define PPCatATDV(x,w,v,q,rs,lb) \
{ \
  IncATAttempts; \
  IncATAttempts; \
  x = w->Phys->Dope; \
}

#define OptPPCatAT(x,w,v,q,rs,lb) IncATAttempts; IncATAttempts

/*
** ANoOp( x = target array, y = source array, z = component type,
**        q = copy function, w = dealloc function ) XX
** COANoOp( ... ) XX
*/
#define ANoOp(x,y,z,q,w) \
{ \
  IncANoOpAttempts; \
  IncANoOpCopies; \
  x = ArrayDuplicate( y, sizeof( z ) ); \
  AAElmCpy( q, x, y, z ); \
  w( y ); \
}

#define COANoOp(x,y,z,q,w) \
{ \
  IncANoOpAttempts; \
  if ( ((ARRAYP)y)->RefCount == 1 && ((ARRAYP)y)->Phys->RefCount == 1 ) \
    x = y; \
  else { \
    IncANoOpCopies; \
    x = ArrayDuplicate( y, sizeof( z ) ); \
    AAElmCpy( q, x, y, z ); \
    w( y ); \
    } \
}


/*
** DARepl( x = component dealloc function, z = component type, 
**         w = target array, q = index, v = value, ab = base pointer ) XX
** OptDARepl( ... ) XX
** IncArrayBase( ... )
*/
#define IncArrayBase(z,ab) ((z*)ab)++

#define DARepl(x,z,w,q,v) \
{ \
  x( ((z*)(((ARRAYP)w)->Base))[q] ); \
  ((z*)(((ARRAYP)w)->Base))[q] = v; \
}

#define OptDARepl(x,z,w,q,v,ab) \
{ \
  x( ((z*)ab)[q] ); \
  ((z*)ab)[q] = v; \
}


/*
** ARepl( z = component type, w = target array, q = index, v = value ) XX
** OptARepl( ..., ab = array base ) XX
*/
#define ARepl(z,w,q,v)       ((z*)(((ARRAYP)w)->Base))[q] = v
#define OptARepl(z,w,q,v,ab) ((z*)ab)[q] = v


/*
 * For power 4 to allocate shared variables (which cannot be declared 
 * statically.
 */
#define VarMalloc(x) (SharedMalloc(sizeof(*(x))))

/* SKI */
/*
** SkiMAlloc( vvv = norm, x = target buffer, y = size, z = component type )
**
** SkiMAllocDV( ... )
** SkiMAllocDVI( lb = lower bound, rc = reference count )
*/

#define SkiMAlloc(vvv,x,y,z) \
{ \
  register PHYSP Phys; \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*(y+vvv))); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 0; \
  Phys->Free = 0; \
  x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Base = x->Base = (POINTER) (((z*)x->Base) + vvv); \
}

#define SkiMAllocDVI(vvv,x,y,z,lb,rc) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  ((ARRAYP)Dv)->RefCount = rc; \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*(y+vvv))); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Base = x->Base = (POINTER) (((z*)x->Base) + vvv); \
  Phys->Dope = Dv; \
  InitDV(Dv,y,lb,x,z); \
}

#define SkiMAllocDV(vvv,x,y,z) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*(y+vvv))); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Base = x->Base = (POINTER) (((z*)x->Base) + vvv); \
  Phys->Dope = Dv; \
}
/* SKI */


/*
** MAlloc( x = target buffer, y = size, z = component type )
**
** MAllocDV( ... )
** MAllocDVI( lb = lower bound, rc = reference count )
** RaggedMAlloc( ... as = allocation size )
** RaggedMAllocDV( ... as = allocation size )
** RaggedMAllocDVI( ... as = allocation size )
** RaggedManager( rag = ragged buffer, y = target, sz = source )
*/

#define RaggedManager(rag,y,sz) \
  if ( sz > rag ) { \
    if ( sz < ArrayExpansion ) \
      y = ArrayExpansion; \
    else \
      y = ((sz/ArrayExpansion)+1)*ArrayExpansion; \
    rag = y;  \
    } \
  else y = rag;

#define MAlloc(x,y,z) \
{ \
  register PHYSP Phys; \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*y)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 0; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
}

#define MAllocDVI(x,y,z,lb,rc) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  ((ARRAYP)Dv)->RefCount = rc; \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*y)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Dope = Dv; \
  InitDV(Dv,y,lb,x,z); \
}

#define MAllocDV(x,y,z) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*y)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Dope = Dv; \
}


#define RaggedMAlloc(x,y,z,as) \
{ \
  register PHYSP Phys; \
  x->Phys = Phys = (PHYSP) Alloc( SIZE_OF(PHYS)+(sizeof(z)*(as)) ); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 0; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
}

#define RaggedMAllocDVI(x,y,z,lb,rc,as) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  ((ARRAYP)Dv)->RefCount = rc; \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*(as))); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Dope = Dv; \
  InitDV(Dv,y,lb,x,z); \
}

#define RaggedMAllocDV(x,y,z,as) \
{ \
  register PHYSP Phys; \
  register POINTER Dv; \
  Dv = (POINTER) Alloc(sizeof(ARRAY)); \
  MY_INIT_LOCK( &(((ARRAYP)Dv)->Mutex) ); \
  x->Phys = Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(z)*(as))); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = x->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Phys->Dope = Dv; \
}


/* 
** BldAT( x = component type, y = target array, z = lower bound, w = size,
**        q = buffer )
**
** BldATDV( ... ) ZZ
** BldATDVI( ... ) ZZ
**
** OptBldAT( ... ) ZZ
*/

#define BldAT(x,y,z,w,q) \
{ \
  y = (POINTER) Alloc( sizeof(ARRAY) ); \
  IncATAttempts; \
  InitDV(y,w,z,q,x); \
  MY_INIT_LOCK( &(((ARRAYP)y)->Mutex) ); \
  q->Phys->RefCount++; \
}

#define BldATDV(x,y,z,w,q) \
{ \
  y = q->Phys->Dope; \
  IncATAttempts; \
}

#define BldATDVI(x,y,z,w,q) \
{ \
  y = q->Phys->Dope; \
  InitDV(y,w,z,q,x); \
  IncATAttempts; \
}

#define OptBldAT(x,y,z,w,q) IncATAttempts;


/* 
** ShiftB( x = component type, y = buffer target, z = buffer source, 
**         w = number )
** OptShiftB( ... )
** VecOptShiftB( ..., y = POINTER target, z = POINTER source, ... )
*/
#define ShiftB(x,y,z,w) \
{ \
  y->Phys = z->Phys; \
  y->Base = (POINTER) ( ((x*)z->Base) + (w) ); \
}
#define OptShiftB(x,y,z,w)    y->Base = (POINTER) (((x*)y->Base)+(w))
#define VecOptShiftB(x,y,z,w) y = (POINTER) (((x*)y)+(w))


/*
** GathATInit( x = component type, y = pointer target, z = buffer ) 
** VecSliceGathATInit( w = lower bound piece or loop, ..., z = base pointer ) 
** VecGathATInit( ... ) 
** VecRedATInit( ..., u = size ) 
*/
#define GathATInit(x,y,z)           y = (POINTER) z->Base
#define VecSliceGathATInit(w,x,y,z) y = (POINTER) z
#define VecGathATInit(w,x,y,z)      y = (POINTER) (((x*)z->Base) - w )
#define VecRedATInit(w,x,y,z,u)     y = (POINTER) (((x*)z->Base)-((w)*(u)))


/*
** GathATupd( x = component type, y = pointer target, z = value ) 
** VecGathATUpd( w = index, ... ) 
*/
#define GathATUpd(x,y,z) \
{ \
  *((x*)y) = z; \
  y = (POINTER)(((x*)y) + 1); \
}
#define VecGathATUpd(w,x,y,z) ((x*)y)[w] = z

/*
** GathUpd( x = component type, y = target array, z = value )  XX
** BGathUpd( ..., w = boolean )  XX
** BGathUpdX( ..., w = boolean, u = deallocation function )
*/
#define GathUpd(x,y,z)       AGather( y, z, x )
#define BGathUpd(x,y,z,w)    if ( w ) { AGather( y, z, x ) }
#define BGathUpdX(x,y,z,w,u) if ( w ) { AGather( y, z, x ); } else u( z )
  


/*
** FvUpdX( x = target, y = value, z = boolean, w = dealloc function ) XX
** BFvUpdX( ... ) XX
** InitFvUpdX( ... ) XX
** FvUpd( ... ) XX
** BFvUpd( ... ) XX
*/
#define BFvUpd(x,y,z) if ( z ) x = y
#define FvUpd(x,y)    x = y
#define InitFvUpdX(x) x = (POINTER) NULL;

#define BFvUpdX(x,y,z,w) \
{ \
  if ( z ) { \
    if ( x != (POINTER) NULL ) \
      w( x ); \
    x = y; \
    } \
  else \
    w( y ); \
}

#define FvUpdX(x,y,w) \
{ \
  if ( x != (POINTER) NULL ) { \
    w( x ); \
    } \
  x = y; \
}


/*
** RCat( x = component type, y = target array, z = source array, 
**       q = component struct name ) XX
** BRCat( ... [w=boolean] ... ) XX
** RCatX( ... ) XX
** BRCatX( ... ) XX
*/

#define RCat(x,y,z,q) \
{ \
  register x   *Src; \
  register int  Num = ((ARRAYP)z)->Size; \
  if ( Num > 0 ) { \
    Src = &(((x*)(((ARRAYP)z)->Base))[((ARRAYP)z)->LoBound]); \
    while ( Num-- > 0 ) { \
      AGather( y, *Src, x ); \
      Src++; \
      } \
    } \
}

#define BRCat(x,y,z,w,q) \
{ \
  register x   *Src; \
  register int  Num; \
  if ( w ) { \
    Num = ((ARRAYP)z)->Size; \
    if ( Num > 0 ) { \
      Src =  &(((x*)(((ARRAYP)z)->Base))[((ARRAYP)z)->LoBound]); \
      while ( Num-- > 0 ) { \
        AGather( y, *Src, x ); \
	Src++; \
	} \
      } \
    } \
}

#define RCatX(x,y,z,q) \
{ \
  register x   *Src; \
  register int  Num = ((ARRAYP)z)->Size; \
  if ( Num > 0 ) { \
    Src = &(((x*)(((ARRAYP)z)->Base))[((ARRAYP)z)->LoBound]); \
    while ( Num-- > 0 ) { \
      IncRefCount( (*Src), q, 1 ); \
      AGather( y, *Src, x ); \
      Src++; \
      } \
    } \
}

#define BRCatX(x,y,z,w,q) \
{ \
  register x   *Src; \
  register int  Num; \
  if ( w ) { \
    Num = ((ARRAYP)z)->Size; \
    if ( Num > 0 ) { \
      Src =  &(((x*)(((ARRAYP)z)->Base))[((ARRAYP)z)->LoBound]); \
      while ( Num-- > 0 ) { \
        IncRefCount( (*Src), q, 1 ); \
        AGather( y, *Src, x ); \
	Src++; \
	} \
      } \
    } \
}

/*
** RedATInit( x = pointer target, y = buffer ) 
** PRedATInit( ... ) 
*/
#define RedATInit(x,y) x = y->Base
#define PRedATInit(x,y) \
{ \
  MY_LOCK( &y->Phys->Mutex ); \
  y->Phys->RefCount++; \
  FLUSHLINE(&(y->Phys->RefCount)); \
  CACHESYNC;\
  MY_UNLOCK( &y->Phys->Mutex ); \
  FLUSHLINE( &(y->Phys->Mutex) ); \
}

/*
** PRedATRapUp( x = buffer ) 
*/
#define PRedATRapUp(x) \
{ \
  MY_LOCK( &x->Phys->Mutex ); \
  x->Phys->RefCount--; \
  FLUSHLINE(&(x->Phys->RefCount)); \
  CACHESYNC;\
  MY_UNLOCK( &x->Phys->Mutex ); \
  FLUSHLINE( &x->Phys->Mutex ); \
}


/*
** RCatAT( x = component type name, y = pointer dst, z = source array,
**         q = copy function name, rs = result size ) YY
** RCatATX( ... ) YY
*/
#define RCatAT(x,y,z,q,rs) \
{ \
  register int sz; \
  sz = ((ARRAYP)z)->Size; \
  q( (x*)y, (((x*)(((ARRAYP)z)->Base)) + ((ARRAYP)z)->LoBound), sz ); \
  y = (POINTER) (((x*)y) + sz); \
}

#define RCatATX(x,y,z,q,rs) RCatAT(x,y,z,q,rs)

/*
** BufCpy( x = destination buffer, y = source buffer, w = shift size )
** VecBufCpy( x = destination POINTER, ... )
*/
#define BufCpy(x,y,w) \
{\
  x->Base = y->Base; \
  x->Phys = y->Phys; \
}
#define VecBufCpy(x,y,w) x = (POINTER) y->Base;
  

/* 
** SliceBufCpy( z = component type, x = destination buffer, y = base address,
**              w = shift size, u = slice's lower bound )
** VecSliceBufCpy( ..., x = destination POINTER, ... )
*/
#define SliceBufCpy(z,x,y,w,u)    x->Base = (POINTER) (((z*)y) + ((u) * (w)))
#define VecSliceBufCpy(z,x,y,w,u) x = (POINTER) (((z*)y) + ((u) * (w)))


/*********** ARRAY: MIXED LANGUAGE MACROS ************/


/*
** InitIArr( x = pointer to target array, y = base, z = size, w = lower bound,
**           v = component type )
** OptInitIArr( ..., q = pointer to the source array )
*/
#define InitIArr(x,y,z,w,v) \
{ \
  register PHYSP Phys; \
  register ARRAYP Array; \
  Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(v)*z)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = z; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Array = (ARRAYP) Alloc( sizeof(ARRAY) ); \
  Array->Size = z; \
  Array->LoBound = w; \
  Array->Phys = Phys; \
  Array->Base = (POINTER) ( ((v*)(Phys->Base)) - w ); \
  Array->RefCount = 1; \
  MY_INIT_LOCK( &(Array->Mutex) ); \
  *x = (POINTER) Array; \
  y = (v*)(Phys->Base); \
}

#define OptInitIArr(x,y,z,w,v,q) \
{ \
  register PHYSP Phys; \
  register ARRAYP Array; \
  Phys = (PHYSP) Alloc(SIZE_OF(PHYS)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = z; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = (POINTER) q; \
  Array = (ARRAYP) Alloc( sizeof(ARRAY) ); \
  Array->Size = z; \
  Array->LoBound = w; \
  Array->Phys = Phys; \
  Array->Base = (POINTER) ( ((v*)(Phys->Base)) - w ); \
  Array->RefCount = 1; \
  MY_INIT_LOCK( &(Array->Mutex) ); \
  *x = (POINTER) Array; \
}

/*
** SFreeArr( x = source array )
*/
#define SFreeArr(x) \
{ \
  register PHYSP Phys; \
  if ( (--(((ARRAYP)x)->RefCount)) < 1 )  { \
    Phys = ((ARRAYP)x)->Phys; \
    if ( (--(Phys->RefCount)) < 1 ) \
      DeAlloc( (POINTER) Phys ); \
    DeAlloc( (POINTER) x ); \
    } \
}

/*
** PAElm( x = target, y = source array, z = index, w = component type )
*/
#define PAElm(x,y,z,w) x = &(((w*)(((ARRAYP)y)->Base))[z])


/***************** FOREIGN INTERFACE MACROS  *********************/

/*
** PrepArr1( z = target pointer, w = source pointer, x = component type )
** PrepArr2( z = target pointer, w = source ponter, x = component type,
**           d = number of dimensions, y = interface vector size,
**           f = WriteArr function )
** PrepRetArr1( z=target pointer, w=phys-base, x = component type, y = size,
**              u=interface descriptor )
*/
#define PrepArr1(z,w,x) z = ((x*)(((ARRAYP)w)->Base))+((ARRAYP)w)->LoBound

#define PrepArr2(z,w,x,d,y,f) \
{ \
  int  idc,dsz,dlo,isz; \
  int  iv[y],*pv; \
  POINTER iarr; \
  iv[0] = 0; \
  iv[1] = 0; \
  iv[2] = 0; \
  pv = &(iv[3]); \
  iarr = w; \
  idc = 0; \
  isz = 1; \
  for (;;) { \
    ASize( dsz, iarr ); \
    ALimL( dlo, iarr ); \
    pv[0] = 1; \
    pv[1] = dsz; \
    pv[2] = 1; \
    pv[3] = dsz; \
    pv[4] = dlo; \
    idc++; \
    isz *= dsz; \
    if ( idc >= d ) break; \
    AElm(POINTER,iarr,iarr,dlo); \
    pv += 5; \
    } \
  z = (x*) Alloc( isz*sizeof(x) ); \
  f( w, z, iv ); \
}

#define PrepRetArr1(z,w,x,y,u) \
{ \
  register PHYSP Phys; \
  register ARRAYP Array; \
  Phys = (PHYSP) Alloc(SIZE_OF(PHYS)+(sizeof(x)*y)); \
  MY_INIT_LOCK( &Phys->Mutex ); \
  Phys->Size = y; \
  Phys->RefCount = 1; \
  Phys->Free = 0; \
  Phys->Base = (POINTER) ALIGNED_INC(PHYS,Phys); \
  Array = (ARRAYP) Alloc( sizeof(ARRAY) ); \
  Array->Size = u[6]-u[5]+1; \
  Array->LoBound = u[7]; \
  Array->Phys = Phys; \
  Array->Base = (POINTER) ( ((x*)(Phys->Base)) + (u[5]-u[3]) - u[7] ); \
  Array->RefCount = 1; \
  MY_INIT_LOCK( &(Array->Mutex) ); \
  z = (POINTER) Array; \
  w = (x*)(Phys->Base); \
}


/***************** SPECIAL VECTOR MACROS *********************/


#define FirstSum(x,y,l,h,v,t,w) \
{ \
  t  vX; \
  int lX,hX; \
  lX = l; \
  hX = h; \
  vX = v; \
  w( ((t*)(x)), &(((t*)y)[(lX)-1]), &lX, &hX, &vX ); \
}

#define TrI(x,y,z,l,h,v,t,w) \
{ \
  t  vX; \
  int lX,hX; \
  lX = l; \
  hX = h; \
  vX = v; \
  w( ((t*)(x)), &(((t*)y)[(lX)-1]), &(((t*)z)[(lX)-1]), &lX, &hX, &vX ); \
}

#define FirstMinMax(y,x,l,h,v,t,w) \
{ \
  int  vX; \
  int lX,hX; \
  extern int w(); \
  lX = l; \
  hX = h; \
  vX = v; \
  y = w( &(((t*)x)[(vX)]), &lX, &hX, &vX ); \
}

/***************** SPECIAL INTRINSIC MACROS *********************/

/*
** monadic( x = target, y = source )
** dyadic( ..., z = second source )
*/

#ifdef _INTRINSICS_

#define _Sand(x,y,z)    x = y & z
#define _Sor(x,y,z)     x = y | z
#define _Sxor(x,y,z)    x = y ^ z
#define _Snot(x,y)      x = ~y
#define _Sshiftl(x,y,z) x = y << z
#define _Sshiftr(x,y,z) x = y >> z

#define _Ssin(x,y)      x = sin((double)y)
#define _Scos(x,y)      x = cos((double)y)
#define _Stan(x,y)      x = tan((double)y)
#define _Sasin(x,y)     x = asin((double)y)
#define _Sacos(x,y)     x = acos((double)y)
#define _Satan(x,y)     x = atan((double)y)
#define _Satan2(x,y,z)  x = atan2((double)y,(double)z)
#define _Ssqrt(x,y)     x = sqrt((double)y)
#define _Slog(x,y)      x = log((double)y)
#define _Slog10(x,y)    x = log10((double)y)
#define _Setothe(x,y)   x = exp((double)y)

#ifdef SGI
#define _Sfsin(x,y)      x = fsin((float)y)
#define _Sfcos(x,y)      x = fcos((float)y)
#define _Sftan(x,y)      x = ftan((float)y)
#define _Sfasin(x,y)     x = fasin((float)y)
#define _Sfacos(x,y)     x = facos((float)y)
#define _Sfatan(x,y)     x = fatan((float)y)
#define _Sfatan2(x,y,z)  x = fatan2((float)y,(double)z)
#define _Sfsqrt(x,y)     x = fsqrt((float)y)
#define _Sflog(x,y)      x = flog((float)y)
#define _Sflog10(x,y)    x = flog10((float)y)
#define _Sfetothe(x,y)   x = fexp((float)y)
#else
#define _Sfsin(x,y)      x = sin((double)y)
#define _Sfcos(x,y)      x = cos((double)y)
#define _Sftan(x,y)      x = tan((double)y)
#define _Sfasin(x,y)     x = asin((double)y)
#define _Sfacos(x,y)     x = acos((double)y)
#define _Sfatan(x,y)     x = atan((double)y)
#define _Sfatan2(x,y,z)  x = atan2((double)y,(double)z)
#define _Sfsqrt(x,y)     x = sqrt((double)y)
#define _Sflog(x,y)      x = log((double)y)
#define _Sflog10(x,y)    x = log10((double)y)
#define _Sfetothe(x,y)   x = exp((double)y)
#endif

#endif


/* **********  ARRAY POINTER SWAP MACROS ********** */

/*
** PSMAllocDVI( x=target buffer pointer, y=size, z=lower bound, w = ps array ) X
*/
#define PSMAllocDVI(x,y,z,w) \
{ \
  x->Phys = ((ARRAYP)w)->Phys; \
  x->Base = x->Phys->Base; \
}

/*
** PSScatter( x = target ps array, y = source ps array, z = lower bound, 
**            w = loop index ) X
*/
#define PSScatter(x,y,z,w)  x = (POINTER) (*(((POINTER*)y)+w-z))
 
/*
**
** PSManagerSwap( x = target ps array, y = source ps buffer pointer )
** PSManager( x = target ps array, y = source ps buffer pointer )  X
*/
#define PSManagerSwap(x,y) \
{ \
  ((PSBUFFERP)y)->Temp    = ((PSBUFFERP)y)->Current; \
  ((PSBUFFERP)y)->Current = ((PSBUFFERP)y)->Next; \
  ((PSBUFFERP)y)->Next    = ((PSBUFFERP)y)->Temp; \
  x = ((PSBUFFERP)y)->Current; \
}

#define PSManager(x,y) \
{ \
  x = ((PSBUFFERP)y)->Current; \
  ((PSBUFFERP)y)->Temp = x; \
}

/*
**
** PSFreeOne( x = target ps buffer pointer ) X
** PSFreeTwo( .. )
** PSFreeSpare1( .. )
** PSFreeSpare2( .. )
** PSFreeSpare1X( .., y = spare array, z = dealloc function )
** PSFreeSpare2X( .., y )
**
*/
#define PSFreeOne(x) FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current );

#define PSFreeTwo(x) \
{ \
  FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current ); \
  FreePointerSwapStorage( x, ((PSBUFFERP)x)->Next    ); \
}

#define PSFreeSpare1(x) \
{ \
  if ( ((PSBUFFERP)x)->Temp == NULL ) \
    FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current ); \
  else \
    FreePointerSwapFramework( x, ((PSBUFFERP)x)->Current ); \
} 

#define PSFreeSpare2(x) \
{ \
  if ( ((PSBUFFERP)x)->Temp == NULL ) \
    FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current ); \
  else \
    FreePointerSwapFramework( x, ((PSBUFFERP)x)->Current ); \
  FreePointerSwapStorage( x, ((PSBUFFERP)x)->Next ); \
}

#define PSFreeSpare1X(x,y,z) \
{ \
  if ( ((PSBUFFERP)x)->Temp == NULL ) \
    FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current ); \
  else { \
    FreePointerSwapFramework( x, ((PSBUFFERP)x)->Current ); \
    z(y); \
    } \
} 

#define PSFreeSpare2X(x,y,z) \
{ \
  if ( ((PSBUFFERP)x)->Temp == NULL ) \
    FreePointerSwapStorage( x, ((PSBUFFERP)x)->Current ); \
  else {\
    FreePointerSwapFramework( x, ((PSBUFFERP)x)->Current ); \
    z(y); \
    } \
  FreePointerSwapStorage( x, ((PSBUFFERP)x)->Next ); \
}

/*
** PSMAllocOne( x=ps buffer, y=component type, w=target,v1-v2 info,u=info top) X
** PSMAllocTwo( ... )
** PSMAllocSpare1( ... )
** PSMAllocSpare2( ... )
** PSMAllocCond( ... )
*/
#define PSMAllocOne(x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  x.Next = NULL; \
  w = (POINTER) &(x); \
}

#define PSMAllocTwo(x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  x.Next = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  w = (POINTER) &(x); \
}

#define PSMAllocSpare1(x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  x.Next = NULL; \
  w = (POINTER) &(x); \
}

#define PSMAllocSpare2(x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  x.Next = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
  w = (POINTER) &(x); \
}

#define PSMAllocCond(x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  if ( x.Temp == NULL ) { \
    x.Info[0] = v0; \
    x.Info[1] = v1; \
    x.Info[2] = v2; \
    x.Info[3] = v3; \
    x.Info[4] = v4; \
    x.Info[5] = v5; \
    x.InfoTop = u; \
    x.Temp = NULL; \
    x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
    x.Next = NULL; \
    w = (POINTER) &(x); \
  } else if ( x.Info[0] == v0 && x.Info[1] == v1 && x.Info[2] == v2 && \
            x.Info[3] == v3 && x.Info[4] == v4 && x.Info[5] == v5  )  {\
    w = (POINTER) &(x); \
  } else { \
    FreePointerSwapStorage( &(x), x.Current ); \
    x.Info[0] = v0; \
    x.Info[1] = v1; \
    x.Info[2] = v2; \
    x.Info[3] = v3; \
    x.Info[4] = v4; \
    x.Info[5] = v5; \
    x.InfoTop = u; \
    x.Temp = NULL; \
    x.Current = AllocPointerSwapStorage( &(x), sizeof( y ) ); \
    x.Next = NULL; \
    w = (POINTER) &(x); \
    } \
}


/* SKI */
/*
** SkiPSMAllocOne( vvv = norm, x=ps buffer, y=component type, 
**                 w=target,v1-v2 info,u=info top) X
** SkiPSMAllocTwo( ... )
** SkiPSMAllocSpare1( ... )
** SkiPSMAllocSpare2( ... )
** SkiPSMAllocCond( ... )
*/
#define SkiPSMAllocOne(vvv,x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  x.Next = NULL; \
  w = (POINTER) &(x); \
}

#define SkiPSMAllocTwo(vvv,x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  x.Next = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  w = (POINTER) &(x); \
}

#define SkiPSMAllocSpare1(vvv,x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  x.Next = NULL; \
  w = (POINTER) &(x); \
}

#define SkiPSMAllocSpare2(vvv,x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  x.Info[0] = v0; \
  x.Info[1] = v1; \
  x.Info[2] = v2; \
  x.Info[3] = v3; \
  x.Info[4] = v4; \
  x.Info[5] = v5; \
  x.InfoTop = u; \
  x.Temp = NULL; \
  x.Current = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  x.Next = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
  w = (POINTER) &(x); \
}

#define SkiPSMAllocCond(vvv,x,y,w,v0,v1,v2,v3,v4,v5,u) \
{ \
  if ( x.Temp == NULL ) { \
    x.Info[0] = v0; \
    x.Info[1] = v1; \
    x.Info[2] = v2; \
    x.Info[3] = v3; \
    x.Info[4] = v4; \
    x.Info[5] = v5; \
    x.InfoTop = u; \
    x.Temp = NULL; \
    x.Current = SkiAllocPointerSwapStorage( vvv, &(x), sizeof( y ) ); \
    x.Next = NULL; \
    w = (POINTER) &(x); \
  } else if ( x.Info[0] == v0 && x.Info[1] == v1 && x.Info[2] == v2 && \
            x.Info[3] == v3 && x.Info[4] == v4 && x.Info[5] == v5  )  {\
    w = (POINTER) &(x); \
  } else { \
    FreePointerSwapStorage( &(x), x.Current ); \
    x.Info[0] = v0; \
    x.Info[1] = v1; \
    x.Info[2] = v2; \
    x.Info[3] = v3; \
    x.Info[4] = v4; \
    x.Info[5] = v5; \
    x.InfoTop = u; \
    x.Temp = NULL; \
    x.Current = SkiAllocPointerSwapStorage( vvv,  &(x), sizeof( y ) ); \
    x.Next = NULL; \
    w = (POINTER) &(x); \
    } \
}
