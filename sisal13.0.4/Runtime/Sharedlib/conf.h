#define POINTER   char*

#define True      1
#define False     0

/* ------------------------------------------------------------ */
/* Numeric sizes						*/
/* ------------------------------------------------------------ */

#if defined(__STDC__) || defined(_ANSI_C_SOURCE)
#ifndef INT_MAX
#include <limits.h>
#endif /* INT_MAX */
#if !defined(FLT_MAX) || !defined(DBL_MIN)
#include <float.h>
#endif /* FLT_MAX */
#define MaxInt          INT_MAX
#define MinInt          INT_MIN
#define MaxFloat        FLT_MAX
#define MinFloat        FLT_MIN
#define MaxDouble       DBL_MAX
#define MinDouble       DBL_MIN

#else

#define MaxInt          ((int)(((unsigned int)-1) >> 1))
#define MinInt          -MaxInt

#ifdef CRAY
#define MaxFloat         2.7e2465
#define MinFloat        -2.7e2465
#define MaxDouble        2.7e2465
#define MinDouble       -2.7e2465
#define float double			/* float is double */
#endif

#if SUNIX || RS6000
#define MaxFloat         1.7e38
#define MinFloat        -1.7e38
#define MaxDouble        1.7e38
#define MinDouble       -1.7e38
#endif

#if SYMMETRY || BALANCE || ALLIANT || ENCORE || SGI || SUN
#define MaxFloat         1.9e38
#define MinFloat        -1.9e38
#define MaxDouble       1.7e308
#define MinDouble      -1.7e308
#endif
#endif /* __STDC__ */

/* ------------------------------------------------------------ */
/* Unavailable keywords						*/
/* ------------------------------------------------------------ */

#if SYMMETRY || BALANCE
#else
#define shared
#endif

#ifndef SGI
#ifndef LINUX
#define volatile
#endif
#endif

/* POINTER to CHARACTER CAST TO DO POINTER to CHARACTER MATHEMATICS */
#define PCMCAST char*

/* ------------------------------------------------------------ */
/* Alignment size						*/
/* ------------------------------------------------------------ */

#ifdef ALLIANT
#define ALIGN_SIZE SIZEOF(double)
#endif

#if CRAY
#define ALIGN_SIZE SIZEOF(int)
#endif

#ifdef SGI
#define ALIGN_SIZE SIZEOF(char*)
#endif

#ifdef HPUXPA
#define ALIGN_SIZE SIZEOF(double)
#endif

#ifndef ALIGN_SIZE
#define ALIGN_SIZE SIZEOF(double)
#endif

#if defined(NON_COHERENT_CACHE)
#undef ALIGN_SIZE
#define ALIGN_SIZE CACHE_LINE
#endif

#define ALIGN(x,y)       ((x)((((int)y)+(ALIGN_SIZE))&(~((ALIGN_SIZE)-1))))
#define SIZE_OF(x)       (((SIZEOF(x)/(ALIGN_SIZE))+1)*(ALIGN_SIZE))
#define ALIGNED_INC(x,y) (((PCMCAST)y)+SIZE_OF(x))
