#ifndef SISALC_H
#define SISALC_H

#include "sisalInfo.h"
#include "charStarQueue.h"

extern void compilerError(char* msg);
extern int Submit(charStarQueue** queueP);

extern void sisal(char* sis,
                  char* if1,
                  char* bindir,
                  char* datadir
                  );

extern void if1ld(charStarQueue** if1Files,
                  char* mono,
                  char* bindir,
                  int warning,
                  int profile,

                  int forFortran,
                  int forC,
                  charStarQueue** entries,
                  charStarQueue** entriesForC,
                  charStarQueue** entriesForFORTRAN,
                  charStarQueue** reductionFunctions,
                  char* QStamps
                  );

extern void if1opt(char* file,
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
                   );

extern void if2up(char* file,
           char* result,
           char* bindir,
           int suppressWarnings, /* suppress warnings? */
           int profiling /* profiling */
           );

extern void if2mem(char* file,
                   char* result,
                   char* bindir,
                   int suppressWarnings, /* suppress warnings? */
                   int profiling, /* profiling */

                   int optimize, /* Perform aggressive optimization */
                   int optimizeInlined /* optimize inlined calls */
                   );

extern void if2part(char* file,
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
                    );
                   
extern void if2gen(char* file,
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
                   int bounds
                   );

extern void compile(char* file,
                    char* result,
                    char* includedir,
                    char* compiler,
                    char* flags
                    );

extern void linker(char* loader,
                   char* target,
                   char* libdir,
                   charStarQueue** objectFiles,
                   charStarQueue** loadOptions,
                   char* LDFLAGS,
                   char* linkWithF77
                   );

#endif
