#ifndef SISALC_H
#define SISALC_H

/**************************************************************************/
/* FILE   **************          sisalc.h         ************************/
/**************************************************************************/
/* Author: Patrick Miller December 31 2000                                */
/* Copyright (C) 2000 Patrick Miller                                      */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalInfo.h"
#include "charStarQueue.h"

extern void compilerError PROTO((char* msg));
extern int Submit PROTO((charStarQueue** queueP));

extern void sisal PROTO((char* sis,
                         char* if1,
                         char* bindir,
                         char* datadir
                         ));

extern void if1ld PROTO((charStarQueue** if1Files,
                         char* mono,
                         char* bindir,
                         int warning,
                         int profile,
                         
                         int forFortran,
                         int forC,
			 int separateCompilation,

                         charStarQueue** entries,
                         charStarQueue** entriesForC,
                         charStarQueue** entriesForFORTRAN,
                         charStarQueue** reductionFunctions,

                         char* QStamps
                         ));

extern void if1opt PROTO((char* file,
                          char* result,
                          char* bindir,
                          int suppressWarnings, /* suppress warnings? */
                          int profiling, /* profiling */
                          
                          charStarQueue** inlineProtectedFunctions, /* functions NOT to inline */
                          int eliminateDeadCode, /* dead code elimination */
                          int optimizeInlined, /* optimize inlined calls */
                          int associativeReductions, /* do non-associative reductions for speed? */
                          int aggressiveCSE, /* Agressively move operations out of loops */
                          int aggressiveVectors, /* Aggressively detect vectors */
                          int nodeStripping, /* strip out nodes */
                          int concurrent, /* Is target parallel? */
                          int inlining, /* Inline function calls? */
                          int normalizedArrayIndexing, /* normalized array indexing */
                          int crayStyleVectors, /* Use Cray styled vectors? */
                          int FORTRANIntrinsicBitFunctions,
                          int optimize /* Perform aggressive optimization */
                          ));

extern void if2up PROTO((char* file,
                         char* result,
                         char* bindir,
                         int suppressWarnings, /* suppress warnings? */
                         int profiling /* profiling */
                         ));

extern void if2mem PROTO((char* file,
                          char* result,
                          char* bindir,
                          int suppressWarnings, /* suppress warnings? */
                          int profiling, /* profiling */
                          
                          int optimize, /* Perform aggressive optimization */
                          int optimizeInlined /* optimize inlined calls */
                          ));

extern void if2part PROTO((char* file,
                           char* result,
                           char* bindir,
                           char* datadir,
                           int suppressWarnings, /* suppress warnings? */
                           int profiling, /* profiling */
                           
                           int associativeReductions,
                           int parallelismLoopNesting,
                           int vectors,
                           int processors,
                           int crayStyleVectors,
                           int minimumParallelLoopCost,
                           int assumedIterationCount
                           ));
                   
extern void if2gen PROTO((char* file,
                          char* result,
                          char* bindir,
                          int suppressWarnings, /* suppress warnings? */
                          int profiling, /* profiling */
                          
                          int sliceThrottle,
                          int showSource,
                          int bindProcessors,
                          int associativeReductions,
                          int FORTRANIntrinsicBitFunctions,
                          int vector,
                          int bounds,
                          int fileio
                          ));

extern void compile PROTO((char* file,
                           char* result,
                           char* includedir,
                           char* compiler,
                           char* flags
                           ));

extern void linker PROTO((char* loader,
                          char* target,
                          char* libdir,
                          charStarQueue** objectFiles,
                          charStarQueue** loadOptions,
                          char* LDFLAGS,
                          char* linkWithF77,
                          int forFORTRAN,
                          int forC
                          ));

#endif
