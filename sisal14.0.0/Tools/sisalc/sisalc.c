/*************************************************************************/
/* FILE   **************          sisalc.c         ************************/
/************************************************************************ **/
/* Author: Patrick Miller December 27 2000                                */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "sisalInfo.h"
#include "sisalPaths.h"
#include "charStarQueue.h"
#include "option.h"
#include "sisalc.h"

/* ----------------------------------------------- */
/* We define queues for all the extensions that we */
/* support                                         */
/* ----------------------------------------------- */
static charStarQueue* sisFiles = 0;
static charStarQueue* if1Files = 0;
static charStarQueue* monoFiles = 0;
static charStarQueue* optFiles = 0;
static charStarQueue* memFiles = 0;
static charStarQueue* upFiles = 0;
static charStarQueue* partFiles = 0;
static charStarQueue* cFiles = 0;
static charStarQueue* f77Files = 0;
static charStarQueue* objectFiles = 0;

/* ----------------------------------------------- */
/* Other queues                                    */
/* ----------------------------------------------- */
static charStarQueue* compilerOptions = 0;
static charStarQueue* loadOptions = 0;
static charStarQueue* reductionFunctions = 0; /* not supported */
static charStarQueue* inlineProtectedFunctions = 0; /* not supported */
static charStarQueue* temporaryFiles = 0; /* Files to delete */

/* ----------------------------------------------- */
/* Hardwired options not supported by command line */
/* ----------------------------------------------- */
static int profiling = 0;
static int eliminateDeadCode = 1;
static int optimizeInlined = 0;
static int associativeReductions = 0;
static int aggressiveCSE = 1;
static int aggressiveVectors = 0;
static int nodeStripping = 0;
static int inlining = 1;
static int normalizedArrayIndexing = 1;
static int crayStyleVectors = 0;
static int FORTRANIntrinsicBitFunctions = 1;
static int parallelismLoopNesting = 1;
static int bindProcessors = 1;

/* OPTIONS */
static int verbose; /* -v --verbose (false) Echo commands to stderr before executing them */
static int help; /* --help --usage (false) Display information on options and arguments */
static int debug; /* -n --just-print (false) Don't actually run any commands, just print them. */
static int stopIF1; /* -IF1 (false) Stop after generating .if1 files*/
static int stopMONO; /* -MONO (false) Stop after generating .mono file */
static int stopOPT; /* -OPT (false) Stop after generating .opt file*/
static int stopMEM; /* -MEM (false) Stop after generating .mem file*/
static int stopUP; /* -UP (false) Stop after generating .up file */
static int stopPART; /* -PART (false) Stop after generating .part file */
static int stopC; /* -C (false) Stop after generating .c file */
static int stopObject; /* -c (false) Stop after generating .o files */
static int showSource; /* -% (false) Show comments in generated source code */
static int forC; /* -forC (false) Compile to call from C  */
static int forFORTRAN; /* -forFORTRAN (false) Compile to call from FORTRAN */
static int suppressWarnings; /* -w  --warn (false) Suppress warning messages */
static int optimize; /* -<no->opt (true) Control Sisal backend optimization (on by default) */
static int concurrent; /* -<no->concur (true) Build for serial or concurrent (default) */
static int vector; /* -<no->vector (false) Generate code for vectorized loops (off by default) */
static int aggresiveVector; /* -<no->aggvector (false) Apply aggressive vectorization (off by default) */
static int descriptorBinding; /* -<no->descriptor-bind (true) Assume the shape of arrays through the FORTRAN and C FLI is fixed (default) */
static int bounds; /* -<no->bounds (true) Control bounds checking (on by default) */
static int sliceThrottle; /* -<no->slice-throttle (true) Control slice throttle feature that runs short parallel loops serially (on by default) */
static int keepIntermediate; /* -keep --keep-intermediate (false) Do not discard temporary files */
static int usingFORTRAN; /* --<no->using-FORTRAN (false) Use if linking .o files from FORTRAN */
static int processors = 1; /* -p=count --processors=count (1) Partition for this many cpus */
static int assumedIterations = 100; /* -iter=count --assumed-iterations=count () Assume this many iterations for loops to drive partition */
static int minimumParallelLoopCost = 7000; /* --parallel-cost-threshold=cost () Parallelize loops that have a predicted cost higher than this */

static char* CC = SISAL_CC; /* CC=flags () Compile using this C compiler */
static char* CFLAGS = SISAL_CFLAGS; /* CFLAGS=flags () Use these C compiler flags */
static char* F77 = SISAL_F77; /* F77=compiler () Compile using this FORTRAN compiler */
static char* FFLAGS = SISAL_FFLAGS; /* FFLAGS=flags () Use these FORTRAN flags */
static char* LD = SISAL_CC; /* LD=flags () Use these FORTRAN flags */
static char* LDFLAGS = SISAL_LDFLAGS; /* LDFLAGS=flags () Add to link/loader */
static char* target = "s.out"; /* -o  () Executable output file name (default s.out) */
static char* mainFunction = "main"; /* -main  () Name of main function (default main) */
static char* TMPDIR = 0; /* -tmpdir  () Where to put temporary files */

static charStarQueue* entries = 0; /* -e --external-entry () Preserve function as an entry point*/
static charStarQueue* externC = 0; /* -externC --extern-C-function Declare this function available as a C external */
static charStarQueue* externFORTRAN = 0; /* -externFORTRAN --extern-FORTRAN-function Declare this function available as a FORTRAN external */
/* OPTIONS */

static option_t options[] = {   /* OPTIONS */
    /* Autogenerated using generateOptions.py on Fri Dec 29 16:33:00 2000 */
    {"-v","--verbose",defaultFalse,"Echo commands to stderr before executing them",
     "Echo commands to stderr before executing them",
     &verbose,0,0,0},
    {"--help","--usage",defaultFalse,"Display information on options and arguments",
     "Display information on options and arguments",
     &help,0,0,0},
    {"-n","--just-print",defaultFalse,"Don't actually run any commands, just print them.",
     "Don't actually run any commands, just print them.",
     &debug,0,0,0},
    {"-IF1",0,defaultFalse,"Stop after generating .if1",
     "Stop after generating .if1",
     &stopIF1,0,0,0},
    {"-MONO",0,defaultFalse,"Stop after generating .mono file",
     "Stop after generating .mono file",
     &stopMONO,0,0,0},
    {"-OPT",0,defaultFalse,"Stop after generating .opt",
     "Stop after generating .opt",
     &stopOPT,0,0,0},
    {"-MEM",0,defaultFalse,"Stop after generating .mem",
     "Stop after generating .mem",
     &stopMEM,0,0,0},
    {"-UP",0,defaultFalse,"Stop after generating .up file",
     "Stop after generating .up file",
     &stopUP,0,0,0},
    {"-PART",0,defaultFalse,"Stop after generating .part file",
     "Stop after generating .part file",
     &stopPART,0,0,0},
    {"-C",0,defaultFalse,"Stop after generating .c file",
     "Stop after generating .c file",
     &stopC,0,0,0},
    {"-c",0,defaultFalse,"Stop after generating .o files",
     "Stop after generating .o files",
     &stopObject,0,0,0},
    {"-%",0,defaultFalse,"Show comments in generated source code",
     "Show comments in generated source code",
     &showSource,0,0,0},
    {"-forC",0,defaultFalse,"Compile to call from C",
     "Compile to call from C",
     &forC,0,0,0},
    {"-forFORTRAN",0,defaultFalse,"Compile to call from FORTRAN",
     "Compile to call from FORTRAN",
     &forFORTRAN,0,0,0},
    {"-w","--warn",defaultFalse,"Suppress warning messages",
     "Suppress warning messages",
     &suppressWarnings,0,0,0},
    {"-<no->opt",0,defaultTrue,"Control Sisal backend optimization (on by default)",
     "Control Sisal backend optimization (on by default)",
     &optimize,0,0,0},
    {"-<no->concur",0,defaultTrue,"Build for serial or concurrent (default)",
     "Build for serial or concurrent (default)",
     &concurrent,0,0,0},
    {"-<no->vector",0,defaultFalse,"Generate code for vectorized loops (off by default)",
     "Generate code for vectorized loops (off by default)",
     &vector,0,0,0},
    {"-<no->aggvector",0,defaultFalse,"Apply aggressive vectorization (off by default)",
     "Apply aggressive vectorization (off by default)",
     &aggresiveVector,0,0,0},
    {"-<no->descriptor-bind",0,defaultTrue,"Assume the shape of arrays through the FORTRAN and C FLI is fixed (default)",
     "Assume the shape of arrays through the FORTRAN and C FLI is fixed (default)",
     &descriptorBinding,0,0,0},
    {"-<no->bounds",0,defaultTrue,"Control bounds checking (on by default)",
     "Control bounds checking (on by default)",
     &bounds,0,0,0},
    {"-<no->slice-throttle",0,defaultTrue,"Control slice throttle feature that runs short parallel loops serially (on by default)",
     "Control slice throttle feature that runs short parallel loops serially (on by default)",
     &sliceThrottle,0,0,0},
    {"-keep","--keep-intermediate",defaultFalse,"Do not discard temporary files",
     "Do not discard temporary files",
     &keepIntermediate,0,0,0},
    {"--<no->using-FORTRAN",0,defaultFalse,"Use if linking .o files from FORTRAN",
     "Use if linking .o files from FORTRAN",
     &usingFORTRAN,0,0,0},
    {"-p","--processors",defaultInitialized,"Partition for this many cpus 1",
     "Partition for this many cpus 1",
     &processors,0,0,0},
    {"-iter","--assumed-iterations",defaultInitialized,"Assume this many iterations for loops to drive partition 100",
     "Assume this many iterations for loops to drive partition 100",
     &assumedIterations,0,0,0},
    {"--parallel-cost-threshold",0,defaultInitialized,"Parallelize loops that have a predicted cost higher than this 7000",
     "Parallelize loops that have a predicted cost higher than this 7000",
     &minimumParallelLoopCost,0,0,0},
    {"CC",0,fetchStringEqual,"Compile using this C compiler",
     "Compile using this C compiler",
     0,0,&CC,0},
    {"CFLAGS",0,fetchStringEqual,"Use these C compiler flags",
     "Use these C compiler flags",
     0,0,&CFLAGS,0},
    {"F77",0,fetchStringEqual,"Compile using this FORTRAN compiler",
     "Compile using this FORTRAN compiler",
     0,0,&F77,0},
    {"FFLAGS",0,fetchStringEqual,"Use these FORTRAN flags",
     "Use these FORTRAN flags",
     0,0,&FFLAGS,0},
    {"LD",0,fetchStringEqual,"Use these FORTRAN flags",
     "Use these FORTRAN flags",
     0,0,&LD,0},
    {"LDFLAGS",0,fetchStringEqual,"Add to link/loader",
     "Add to link/loader",
     0,0,&LDFLAGS,0},
    {"-o",0,fetchStringNext,"Executable output file name (default s.out)",
     "Executable output file name (default s.out)",
     0,0,&target,0},
    {"-main",0,fetchStringNext,"Name of main function (default main)",
     "Name of main function (default main)",
     0,0,&mainFunction,0},
    {"-tmpdir",0,fetchStringNext,"Where to put temporary files",
     "Where to put temporary files",
     0,0,&TMPDIR,0},
    {"-e","--external-entry",appendQueue,"Preserve function as an entry",
     "Preserve function as an entry",
     0,0,0,&entries},
    {"-externC","--extern-C-function",appendQueue,"Declare this function available as a C external",
     "Declare this function available as a C external",
     0,0,0,&externC},
    {"-externFORTRAN","--extern-FORTRAN-function",appendQueue,"Declare this function available as a FORTRAN external",
     "Declare this function available as a FORTRAN external",
     0,0,0,&externFORTRAN},
/* OPTIONS */
   
   /* ----------------------------------------------- */
   /* Standard -I -D -U options for CC and F77        */
   /* ----------------------------------------------- */
   {"-I*",0,prefixedOption,"Additional include paths",
    "These are passed unmodified to the C and F77 compilers",
    0,0,0,&compilerOptions},
   {"-D*",0,prefixedOption,"Pre-proccesor symbols for C and F77",
    "These are passed unmodified to the C and F77 compilers",
    0,0,0,&compilerOptions},
   {"-U*",0,prefixedOption,"Remove pre-proccesor symbols for C and F77",
    "These are passed unmodified to the C and F77 compilers",
    0,0,0,&compilerOptions},

   /* ----------------------------------------------- */
   /* Sisal file forms                                */
   /* ----------------------------------------------- */
   {"*.sis",0,suffixedFile,"Sisal source file",
    "You may include multiple Sisal files.  They are monolithically merged",
    0,0,0,&sisFiles},

   {"*.if1",0,suffixedFile,"Sisal initial intermediate file",
    "You may include multiple IF1 files.  They are monolithically merged",
    0,0,0,&if1Files},

   {"*.mono",0,suffixedFile,"Sisal monolith",
    "The Sisal monolith is a merged grouping of all Sisal functions (1 per compilation)",
    0,0,0,&monoFiles},

   {"*.opt",0,suffixedFile,"Optimized monolith",
    "The optimized Sisal monolith (1 per compilation)",
    0,0,0,&optFiles},

   {"*.mem",0,suffixedFile,"Memory optimized monolith (IF2)",
    "Memory optimized intermediate (1 per compilation)",
    0,0,0,&memFiles},

   {"*.up",0,suffixedFile,"Update-in-place monolith (IF2)",
    "Memory optimized intermediate (1 per compilation)",
    0,0,0,&upFiles},

   {"*.part",0,suffixedFile,"Partitioned monolith (IF2)",
    "Memory optimized intermediate (1 per compilation)",
    0,0,0,&partFiles},

   {"*.c",0,suffixedFile,"C source file",
    "You may include previously generated code (the -C option) or user supplied C source",
    0,0,0,&cFiles},

   {"*.f",0,suffixedFile,"F77 source file",
    "User supplied FORTRAN77 source",
    0,0,0,&f77Files},

   {"*.F",0,suffixedFile,"F77 source file",
    "User supplied FORTRAN77 source",
    0,0,0,&f77Files},

   {"*.f77",0,suffixedFile,"F77 source file",
    "User supplied FORTRAN77 source",
    0,0,0,&f77Files},

   {"*.o",0,suffixedFile,"object file",
    "User supplied object file",
    0,0,0,&objectFiles},

   /* ----------------------------------------------- */
   /* Catch all                                       */
   /* ----------------------------------------------- */
   {"*",0,catchAll,"Options to pass to the link stage",
    "System specific loader options",
    0,0,0,&loadOptions},
   {0}
};

/**************************************************************************/
/* LOCAL  **************          cleanup          ************************/
/**************************************************************************/
/* Remove temporary files on exit                                         */
/**************************************************************************/
static void cleanup() {
   while( temporaryFiles ) {
      char* file = dequeue(&temporaryFiles);
      if ( verbose ) fprintf(stderr,"rm %s\n",file);
      unlink(file);
   }
}

/**************************************************************************/
/* GLOBAL **************       compilerError       ************************/
/**************************************************************************/
/* Write an error, cleanup, and quit                                      */
/**************************************************************************/
void compilerError(char* msg) {
   fprintf(stderr,"ERROR: %s\n",msg);
   cleanup();
   exit(1);
}

int Submit(charStarQueue** queueP) {
   char* argv[10240];
   int argc = 0;
   int pid;
   int status = 0;
   
   while(*queueP) {
      argv[argc++] = dequeue(queueP);
      if ( verbose ) fprintf(stderr,"%s ",argv[argc-1]);
   }
   argv[argc] = 0;
   if (verbose) fputs("\n",stderr);

   pid = fork();
   if ( pid < 0 ) {
      perror("sisalc");
      exit(1);
   } else if ( pid == 0 ) {
      execv(argv[0],argv);
      perror("sisalc");
      exit(1);
   } else {
      while( pid != wait(&status) );
   }
   return status;
}

static char* generateFilename(char* basis, char* extension, int isFinal) {
   char* newName = malloc(MAXPATHLEN);
   char* dot = 0;

   strcpy(newName,basis);
   dot = rindex(newName,'.');
   if ( dot ) {
      strcpy(dot,extension);
   } else {
      strcat(newName,extension);
   }
   if ( !isFinal && !keepIntermediate ) {
      enqueue(&temporaryFiles,newName);
   }
   return newName;
}

/**************************************************************************/
/* GLOBAL **************            main           ************************/
/**************************************************************************/
/* Parse options and apply appropriate commands                           */
/**************************************************************************/
int main(int argc, char** argv) {
   char* file = 0;
   char* result = 0;
   char majorVersion[MAXPATHLEN];
   char binDirectory[MAXPATHLEN];
   char dataDirectory[MAXPATHLEN];
   char libDirectory[MAXPATHLEN];
   char includeDirectory[MAXPATHLEN];
   
   setOptionDefaults(options);
   optionScan(argc-1,argv+1,options,exitIfNotFound);

   /* ----------------------------------------------- */
   /* Set important directories                       */
   /* ----------------------------------------------- */
   strcpy(majorVersion,SISAL_VERSION);
   if ( index(majorVersion,'.') ) {
      *index(majorVersion,'.') = 0;
   }
   sprintf(binDirectory,"%s/bin/sisal%s",SISAL_PATH_PREFIX,majorVersion);
   sprintf(dataDirectory,"%s/share/sisal%s",SISAL_PATH_PREFIX,majorVersion);
   sprintf(libDirectory,"%s/lib/sisal%s",SISAL_PATH_PREFIX,majorVersion);
   sprintf(includeDirectory,"%s/include/sisal%s",SISAL_PATH_PREFIX,majorVersion);
           
   /* ----------------------------------------------- */
   /* See if we want a usage string                   */
   /* ----------------------------------------------- */
   if ( help ) {
      optionUsage(stderr);
      exit(0);
   }

   /* ----------------------------------------------- */
   /* Apply frontend to all .sis files                */
   /* ----------------------------------------------- */
   while(sisFiles) {
      file = dequeue(&sisFiles);
      result = generateFilename(file,".if1",stopIF1);
      sisal(file,      /* source */
            result,    /* result file */
            binDirectory, /* Where to locate frontend */
            dataDirectory /* Where to find parser data files */
                     );
      enqueue(&if1Files,result);
   }
   if ( stopIF1 ) goto finish;

   /* ----------------------------------------------- */
   /* Apply the loader to all .if1 files              */
   /* ----------------------------------------------- */
   if ( if1Files ) {
      result = generateFilename(if1Files->charStar,".mono",stopMONO);
      if1ld(
            &if1Files, /* files to merge */
            result, /* Where to put merged result */
            binDirectory, /* directory to find if1ld executable */
            suppressWarnings, /* Suppress warnings? */
            profiling, /* profiling */

            forFORTRAN,
            forC,
            &entries, /* Mark these as entry points */
            &externC, /* Foriegn externals */
            &externFORTRAN, /* Foriegn externals */
            &reductionFunctions, /* Reduction functions (not currently supported) */
            "-FUR" /* Q stamp to apply to merged executable */
            );
      enqueue(&monoFiles,result);
   }
   if ( stopMONO ) goto finish;

   /* ----------------------------------------------- */
   /* Optimize the monolith                           */
   /* ----------------------------------------------- */
   while(monoFiles) {
      file = dequeue(&monoFiles);
      result = generateFilename(file,".opt",stopOPT);
      if1opt(file,
             result,
             binDirectory,
             suppressWarnings, /* suppress warnings? */
             profiling, /* profiling */

             &inlineProtectedFunctions, /* functions NOT to inline */
             eliminateDeadCode, /* dead code elimination */
             optimizeInlined, /* optimize inlined calls */
             associativeReductions, /* do non-associative reductions for speed? */
             aggressiveCSE, /* Agressively move operations out of loops */
             aggressiveVectors, /* Aggressively detect vectors */
             nodeStripping, /* strip out nodes */
             concurrent, /* Is target parallel? */
             inlining, /* Inline function calls? */
             normalizedArrayIndexing, /* normalized array indexing */
             crayStyleVectors, /* Use Cray styled vectors? */
             FORTRANIntrinsicBitFunctions,
             optimize /* Perform aggressive optimization */
             );
      enqueue(&optFiles,result);
   }
   if ( stopOPT ) goto finish;

   /* ----------------------------------------------- */
   /* Build IF2 memory model                          */
   /* ----------------------------------------------- */
   while(optFiles) {
      file = dequeue(&optFiles);
      result = generateFilename(file,".mem",stopMEM);
      if2mem(file,
             result,
             binDirectory,
             suppressWarnings, /* suppress warnings? */
             profiling, /* profiling */

             eliminateDeadCode, /* dead code elimination */
             optimizeInlined /* optimize inlined calls */
             );
      enqueue(&memFiles,result);
   }
   if ( stopMEM ) goto finish;

   /* ----------------------------------------------- */
   /* Apply update in place optimization              */
   /* ----------------------------------------------- */
   while(memFiles) {
      file = dequeue(&memFiles);
      result = generateFilename(file,".up",stopUP);
      if2up(file,
            result,
            binDirectory,
            suppressWarnings,
            profiling
            );
      enqueue(&upFiles,result);
   }
   if ( stopUP ) goto finish;

   /* ----------------------------------------------- */
   /* Apply partitioner                               */
   /* ----------------------------------------------- */
   while(upFiles) {
      file = dequeue(&upFiles);
      result = generateFilename(file,".part",stopPART);
      if2part(file,
              result,
              binDirectory,
              dataDirectory,
              suppressWarnings,
              profiling,

              associativeReductions,
              parallelismLoopNesting,
              vector,
              processors,
              crayStyleVectors,
              minimumParallelLoopCost,
              assumedIterations
              );
      enqueue(&partFiles,result);
   }
   if ( stopPART ) goto finish;

   /* ----------------------------------------------- */
   /* Generate C code                                 */
   /* ----------------------------------------------- */
   while(partFiles) {
      file = dequeue(&partFiles);
      result = generateFilename(file,".c",stopC);
      if2gen(file,
             result,
             binDirectory,
             suppressWarnings,
             profiling,

             sliceThrottle,
             showSource,
             bindProcessors,
             associativeReductions,
             FORTRANIntrinsicBitFunctions,
             vector,
             bounds
             );
      enqueue(&cFiles,result);
   }
   if ( stopC ) goto finish;

   /* ----------------------------------------------- */
   /* Compile C and F77 files                         */
   /* ----------------------------------------------- */
   while(cFiles) {
      file = dequeue(&cFiles);
      result = generateFilename(file,".o",stopObject);
      compile(file,result,includeDirectory,CC,SISAL_CFLAGS);
      enqueue(&objectFiles,result);
   }
   while(f77Files) {
      usingFORTRAN = 1; /* Used in linker below */
      file = dequeue(&f77Files);
      result = generateFilename(file,".o",stopObject);
      compile(file,result,F77,(SISAL_F77_DASHI_OK)?(includeDirectory):(0),SISAL_FFLAGS);
      enqueue(&objectFiles,result);
   }
   if ( stopObject ) goto finish;

   /* ----------------------------------------------- */
   /* Load an executable                              */
   /* ----------------------------------------------- */
   linker(LD,
          target,
          libDirectory,
          &objectFiles,
          &loadOptions,
          LDFLAGS,
          (usingFORTRAN)?(SISAL_FLIBS):0
          );

 finish:
   cleanup();
   return 0;
}









