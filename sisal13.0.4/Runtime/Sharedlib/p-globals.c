#include "world.h"

#if defined(NO_STATIC_SHARED)

extern struct shared_s LSR;

void
InitSharedGlobals()
{
	NumWorkers       = DEFAULT_NUM_WORKERS;
	DsaSize          = DEFAULT_DSA_SIZE;

	BindParallelWork = TRUE;

	XftThreshold     = DEFAULT_XFT_THRESHOLD;
	LoopSlices       = -1;
	GatherPerfInfo   = FALSE;
	ArrayExpansion   = DEFAULT_ARRAY_EXPANSION;
	NoFibreOutput    = FALSE;

	UsingSdbx        = FALSE;

	Sequential       = FALSE;

#if defined(NON_COHERENT_CACHE)
	DefaultLoopStyle = 'C';	/* Cached loops is the default */
#else
	DefaultLoopStyle = 'B';	/* Blocked loops is the default */
#endif

	OneLevelParallel = FALSE;
}

#else

int     NumWorkers       = DEFAULT_NUM_WORKERS;
int     DsaSize          = DEFAULT_DSA_SIZE;

int     BindParallelWork = FALSE;

int     XftThreshold     = DEFAULT_XFT_THRESHOLD;
int     LoopSlices       = -1;
int     GatherPerfInfo   = FALSE;
int     ArrayExpansion   = DEFAULT_ARRAY_EXPANSION;
int     NoFibreOutput    = FALSE;

int     UsingSdbx        = FALSE;

int     Sequential       = FALSE;

#if defined(NON_COHERENT_CACHE)
char	DefaultLoopStyle = 'C';	/* Cached loops is the default */
#else
char	DefaultLoopStyle = 'B';	/* Blocked loops is the default */
#endif

int     OneLevelParallel = FALSE;
#endif

POINTER SisalMainArgs;

/*
FILE   *FibreInFd  = stdin;
FILE   *FibreOutFd = stdout;
FILE   *PerfFd     = stderr;
*/
FILE   *FibreInFd  = NULL;
FILE   *FibreOutFd = NULL;
FILE   *PerfFd     = NULL;


char	ArgumentString[1024] = "";
