if (argv[idx][0] == '-' ) {                                              /* M */
  CorrectUsage = NULL;                                                   /* a */
  switch( argv[idx][1]) {                                                /* c */
                                                                         /* h */
   case '%':                                                             /* i */
    if ( strcmp(argv[idx]+1,"%") == 0 ) {                                /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                              -%                              */     /* G */
  /*   Insert SISAL source line comments in the generated code    */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      ShowSource = TRUE;                                                 /* r */
                                                                         /* a */
      break;                                                             /* t */
    }                                                                    /* e */
    goto OptionError;                                                    /* d */
                                                                         /*   */
   case 'A':                                                             /* D */
    if ( isdigit(argv[idx][2]) ) {                                       /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                           -A<num>                            */     /* o */
  /*        Set loop parallelization ``At'' level to <num>        */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      atlevel = argv[idx];                                               /* h */
                                                                         /* a */
      break;                                                             /* n */
    }                                                                    /* g */
    goto OptionError;                                                    /* e */
                                                                         /*   */
   case 'C':                                                             /* M */
    if ( strcmp(argv[idx]+1,"C") == 0 ) {                                /* a */
                                                                         /* c */
  /* ------------------------------------------------------------ */     /* h */
  /*                              -C                              */     /* i */
  /*                 Stop after generating C code                 */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      stopC = TRUE;                                                      /* G */
                                                                         /* e */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"CPP") == 0 ) {                       /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                             -CPP                             */     /* t */
  /*   Run the SISAL files through the C preprocessor and stop    */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      stopCPP = TRUE;                                                    /* D */
      nocpp = FALSE;                /* -CPP implies -cpp PJM 10/92 */    /* o */
                                                                         /*   */
      break;                                                             /* N */
    } else if ( strncmp(argv[idx]+1,"CC=",3) == 0 ) {                    /* o */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /*   */
  /*                          -CC=<path>                          */     /* C */
  /*                 Use <path> as the C compiler                 */     /* h */
  /* ------------------------------------------------------------ */     /* a */
    CorrectUsage = "Usage: -CC=<path>";                                  /* n */
    if ( argv[idx][3] == '\0' ) goto OptionError;                        /* g */
                                                                         /* e */
      cc = &argv[idx][4];                                                /*   */
      newcc = TRUE;                                                      /* M */
                                                                         /* a */
      break;                                                             /* c */
    }                                                                    /* h */
    goto OptionError;                                                    /* i */
                                                                         /* n */
   case 'D':                                                             /* e */
    if ( strcmp(argv[idx]+1,"D") == 0 ) {                                /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                              -D                              */     /* n */
  /*                Use DEBUG version of frontend                 */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      debug = TRUE;                                                      /* t */
      DeBuG = TRUE;                                                      /* e */
                                                                         /* d */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"DI") == 0 ) {                        /* D */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /*   */
  /*                             -DI                              */     /* N */
  /*                    Compile to DI stye IF1                    */     /* o */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /*   */
      stopDI = TRUE;                                                     /* C */
                                                                         /* h */
      break;                                                             /* a */
    } else {                                                             /* n */
                                                                         /* g */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -D<defn>                           */     /*   */
  /*               Define macro for C preprocessor                */     /* M */
  /* ------------------------------------------------------------ */     /* a */
    CorrectUsage = "Usage: -D<defn>";                                    /* c */
    if ( argv[idx][1] == '\0' ) goto OptionError;                        /* h */
                                                                         /* i */
      cppoptions[++cppoptioncnt] = argv[idx];                            /* n */
                                                                         /* e */
      break;                                                             /*   */
    }                                                                    /* G */
                                                                         /* e */
   case 'F':                                                             /* n */
    if ( strncmp(argv[idx]+1,"FF=",3) == 0 ) {                           /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                          -FF=<path>                          */     /* t */
  /*              Use <path> as the FORTRAN compiler              */     /* e */
  /* ------------------------------------------------------------ */     /* d */
    CorrectUsage = "Usage: -FF=<path>";                                  /*   */
    if ( argv[idx][3] == '\0' ) goto OptionError;                        /* D */
                                                                         /* o */
      ff = &argv[idx][4];                                                /*   */
      newff = TRUE;                                                      /* N */
                                                                         /* o */
      break;                                                             /* t */
    }                                                                    /*   */
    goto OptionError;                                                    /* C */
                                                                         /* h */
   case 'I':                                                             /* a */
    if ( strcmp(argv[idx]+1,"IF0") == 0 ) {                              /* n */
                                                                         /* g */
  /* ------------------------------------------------------------ */     /* e */
  /*                             -IF0                             */     /*   */
  /*                   Compile to IF0 and stop                    */     /* M */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* c */
      stopIF0 = TRUE;                                                    /* h */
                                                                         /* i */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"IF1") == 0 ) {                       /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                             -IF1                             */     /* e */
  /*                   Compile to IF1 and stop                    */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* r */
      stopIF1 = TRUE;                                                    /* a */
                                                                         /* t */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"IF3") == 0 ) {                       /* d */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* D */
  /*                             -IF3                             */     /* o */
  /*                        Add IF3 output                        */     /*   */
  /* ------------------------------------------------------------ */     /* N */
                                                                         /* o */
      stopIF3 = TRUE;                                                    /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( isdigit(argv[idx][2]) ) {                                /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                           -I<num>                            */     /* g */
  /*            Set assumed iteration count for loops             */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* M */
      iter = argv[idx];                                                  /* a */
      iter[1] = '@';                                                     /* c */
                                                                         /* h */
      break;                                                             /* i */
    } else {                                                             /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -I<path>                           */     /* G */
  /*              Search <path> for C include files               */     /* e */
  /* ------------------------------------------------------------ */     /* n */
    CorrectUsage = "Usage: -I<path>";                                    /* e */
    if ( argv[idx][1] == '\0' ) goto OptionError;                        /* r */
                                                                         /* a */
      cppoptions[++cppoptioncnt] = argv[idx];                            /* t */
                                                                         /* e */
      break;                                                             /* d */
    }                                                                    /*   */
                                                                         /* D */
   case 'K':                                                             /* o */
    if ( strcmp(argv[idx]+1,"Keep") == 0 ) {                             /*   */
                                                                         /* N */
  /* ------------------------------------------------------------ */     /* o */
  /*                            -Keep                             */     /* t */
  /*       Keep intermediate files instead of deleting them       */     /*   */
  /* ------------------------------------------------------------ */     /* C */
                                                                         /* h */
      Keep = TRUE;                                                       /* a */
      TMPDIR="";                                                         /* n */
                                                                         /* g */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* M */
                                                                         /* a */
   case 'L':                                                             /* c */
    if ( isdigit(argv[idx][2]) ) {                                       /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                           -L<num>                            */     /* e */
  /*        Set nested parallelization threshold to <num>         */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
      level = argv[idx];                                                 /* n */
      level[1] = 'L';                                                    /* e */
                                                                         /* r */
      break;                                                             /* a */
    } else if ( strncmp(argv[idx]+1,"Loopstyle=",10) == 0 ) {            /* t */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* d */
  /*                    -Loopstyle=<stylechar>                    */     /*   */
  /*    Set the default loop parallelism style to <stylechar>     */     /* D */
  /* ------------------------------------------------------------ */     /* o */
    CorrectUsage = "Usage: -Loopstyle=<stylechar>";                      /*   */
    if ( argv[idx][10] == '\0' ) goto OptionError;                       /* N */
                                                                         /* o */
      LoopStyle[2] = argv[idx][11];                                      /* t */
                                                                         /*   */
      break;                                                             /* C */
    }                                                                    /* h */
    goto OptionError;                                                    /* a */
                                                                         /* n */
   case 'M':                                                             /* g */
    if ( strcmp(argv[idx]+1,"MEM") == 0 ) {                              /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* M */
  /*                             -MEM                             */     /* a */
  /*              Stop after memory allocation phase              */     /* c */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* i */
      stopMEM = TRUE;                                                    /* n */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"MONO") == 0 ) {                      /* G */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* n */
  /*                            -MONO                             */     /* e */
  /*          Stop after compiling and linking IF1 files          */     /* r */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* t */
      stopMONO = TRUE;                                                   /* e */
                                                                         /* d */
      break;                                                             /*   */
    }                                                                    /* D */
    goto OptionError;                                                    /* o */
                                                                         /*   */
   case 'N':                                                             /* N */
    if ( strcmp(argv[idx]+1,"NoSliceThrottle") == 0 ) {                  /* o */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /*   */
  /*                       -NoSliceThrottle                       */     /* C */
  /*   Turn off the slice throttle feature (MinSlice estimate)    */     /* h */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* n */
      MinSliceThrottle = FALSE;                                          /* g */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( isdigit(argv[idx][2]) ) {                                /* M */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* c */
  /*                           -N<num>                            */     /* h */
  /*        Set nested parallelization threshold to <num>         */     /* i */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      level = argv[idx];                                                 /*   */
      level[1] = 'L';                                                    /* G */
                                                                         /* e */
      break;                                                             /* n */
    }                                                                    /* e */
    goto OptionError;                                                    /* r */
                                                                         /* a */
   case 'O':                                                             /* t */
    if ( strcmp(argv[idx]+1,"O") == 0 ) {                                /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                              -O                              */     /* D */
  /*                  Use enabled optimizations                   */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      noimp = FALSE;                                                     /* o */
      bounds = FALSE;                                                    /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( strcmp(argv[idx]+1,"OPT") == 0 ) {                       /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                             -OPT                             */     /* g */
  /*                 Stop after IF1 optimizations                 */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* M */
      stopOPT = TRUE;                                                    /* a */
                                                                         /* c */
      break;                                                             /* h */
    }                                                                    /* i */
    goto OptionError;                                                    /* n */
                                                                         /* e */
   case 'P':                                                             /*   */
    if ( strcmp(argv[idx]+1,"PART") == 0 ) {                             /* G */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* n */
  /*                            -PART                             */     /* e */
  /*                Stop after partitioning phase                 */     /* r */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* t */
      stopPART = TRUE;                                                   /* e */
                                                                         /* d */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"Prof") == 0 ) {                      /* D */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -Prof                             */     /* N */
  /*                  Profile compiler execution                  */     /* o */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /*   */
      Prof = TRUE;                                                       /* C */
      verbose = TRUE;                                                    /* h */
                                                                         /* a */
      break;                                                             /* n */
    } else if ( isdigit(argv[idx][2]) ) {                                /* g */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -P<num>                            */     /* M */
  /*    Assume <num> processors are available for partitioning    */     /* a */
  /* ------------------------------------------------------------ */     /* c */
                                                                         /* h */
      procs = argv[idx];                                                 /* i */
      procs[1] = 'P';                                                    /* n */
                                                                         /* e */
      break;                                                             /*   */
    }                                                                    /* G */
    goto OptionError;                                                    /* e */
                                                                         /* n */
   case 'S':                                                             /* e */
    if ( strcmp(argv[idx]+1,"S") == 0 ) {                                /* r */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* t */
  /*                              -S                              */     /* e */
  /*           Stop after generating assembly language            */     /* d */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* D */
      stopS = TRUE;                                                      /* o */
      TMPDIR="";                                                         /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"SliceThrottle") == 0 ) {             /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                        -SliceThrottle                        */     /* h */
  /*    Turn on the slice throttle feature (MinSlice estimate)    */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      MinSliceThrottle = TRUE;                                           /* e */
                                                                         /*   */
      break;                                                             /* M */
    }                                                                    /* a */
    goto OptionError;                                                    /* c */
                                                                         /* h */
   case 'U':                                                             /* i */
    if ( strcmp(argv[idx]+1,"UP") == 0 ) {                               /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                             -UP                              */     /* G */
  /*             Stop after update in place analysis              */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      stopUP = TRUE;                                                     /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else {                                                             /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -U<defn>                           */     /* D */
  /*            Undefine definition for C preprocessor            */     /* o */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -U<defn>";                                    /* N */
    if ( argv[idx][1] == '\0' ) goto OptionError;                        /* o */
                                                                         /* t */
      cppoptions[++cppoptioncnt] = argv[idx];                            /*   */
                                                                         /* C */
      break;                                                             /* h */
    }                                                                    /* a */
                                                                         /* n */
   case 'a':                                                             /* g */
    if ( strcmp(argv[idx]+1,"aggvector") == 0 ) {                        /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* M */
  /*                          -aggvector                          */     /* a */
  /*          Aggressively fuse independent vector loops          */     /* c */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* i */
      AggressiveVector =TRUE;                                            /* n */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"alliantfx") == 0 ) {                 /* G */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* n */
  /*                          -alliantfx                          */     /* e */
  /*               Compile for Alliant architecture               */     /* r */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* t */
      novec = FALSE;                                                     /* e */
      avector = TRUE;                                                    /* d */
      alliantfx = TRUE;                                                  /*   */
                                                                         /* D */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"avector") == 0 ) {                   /*   */
                                                                         /* N */
  /* ------------------------------------------------------------ */     /* o */
  /*                           -avector                           */     /* t */
  /*                  Use Alliant style vectors                   */     /*   */
  /* ------------------------------------------------------------ */     /* C */
                                                                         /* h */
      novec = FALSE;                                                     /* a */
      avector =TRUE;                                                     /* n */
                                                                         /* g */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* M */
                                                                         /* a */
   case 'b':                                                             /* c */
    if ( strcmp(argv[idx]+1,"batch") == 0 ) {                            /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                            -batch                            */     /* e */
  /*       ...options  Submit as NQS batch job (Cray only)        */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
#ifdef Cray                                                              /* n */
      SubmitNQS(argc,argv,idx);                                          /* e */
#endif                                                                   /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"bind") == 0 ) {                      /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -bind                             */     /* D */
  /*   Array descriptor data will not change in calls to SISAL    */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      BindProcessors = TRUE;                                             /* o */
                                                                         /* t */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"bounds") == 0 ) {                    /* C */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* a */
  /*                           -bounds                            */     /* n */
  /*          Generate code to check for various errors           */     /* g */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      bounds = TRUE;                                                     /* M */
                                                                         /* a */
      break;                                                             /* c */
    }                                                                    /* h */
    goto OptionError;                                                    /* i */
                                                                         /* n */
   case 'c':                                                             /* e */
    if ( strcmp(argv[idx]+1,"c") == 0 ) {                                /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                              -c                              */     /* n */
  /*  Compile to .o file (requires -e <funct> or -main <funct>)   */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      smodule = TRUE;                                                    /* t */
      stopc = TRUE;                                                      /* e */
      TMPDIR="";                                                         /* d */
                                                                         /*   */
      break;                                                             /* D */
    } else if ( strcmp(argv[idx]+1,"call") == 0 ) {                      /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                        -call <funct>                         */     /* o */
  /*               Call <funct> instead of inlining               */     /* t */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -call <funct>";                               /* C */
    if ( (++idx) >= argc ) goto OptionError;                             /* h */
                                                                         /* a */
      calloptions[++calloptioncnt] = argv[idx];                          /* n */
                                                                         /* g */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"concur") == 0 ) {                    /*   */
                                                                         /* M */
  /* ------------------------------------------------------------ */     /* a */
  /*                           -concur                            */     /* c */
  /*                   Generate concurrent code                   */     /* h */
  /* ------------------------------------------------------------ */     /* i */
                                                                         /* n */
      concur = TRUE;                                                     /* e */
                                                                         /*   */
      break;                                                             /* G */
    } else if ( strcmp(argv[idx]+1,"copyinfo") == 0 ) {                  /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                          -copyinfo                           */     /* r */
  /*            Gather inforation on aggregate copying            */     /* a */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /* e */
      cpyinfo = TRUE;                                                    /* d */
                                                                         /*   */
      break;                                                             /* D */
    } else if ( strcmp(argv[idx]+1,"cpp") == 0 ) {                       /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                             -cpp                             */     /* o */
  /*            Run the C preprocessor on SISAL files             */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      nocpp = FALSE;                                                     /* h */
                                                                         /* a */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"cray") == 0 ) {                      /* g */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -cray                             */     /* M */
  /*                    Cross compile for Cray                    */     /* a */
  /* ------------------------------------------------------------ */     /* c */
                                                                         /* h */
      novec = FALSE;                                                     /* i */
      cvector = TRUE;                                                    /* n */
      cRay = TRUE;                                                       /* e */
      movereads = TRUE;                                                  /*   */
      chains = TRUE;                                                     /* G */
      newchains = TRUE;                                                  /* e */
      stopC = TRUE;                                                      /* n */
                                                                         /* e */
      break;                                                             /* r */
    } else if ( strcmp(argv[idx]+1,"cvector") == 0 ) {                   /* a */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -cvector                           */     /* d */
  /*                Get vectorization information                 */     /*   */
  /* ------------------------------------------------------------ */     /* D */
                                                                         /* o */
      novec = FALSE;                                                     /*   */
      cvector = TRUE;                                                    /* N */
      movereads = TRUE;                                                  /* o */
      chains = TRUE;                                                     /* t */
      newchains = TRUE;                                                  /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strncmp(argv[idx]+1,"cc=",3) == 0 ) {                    /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                       -cc=<directive>                        */     /* e */
  /*             Supply <directive> to the C compiler             */     /*   */
  /* ------------------------------------------------------------ */     /* M */
    CorrectUsage = "Usage: -cc=<directive>";                             /* a */
    if ( argv[idx][3] == '\0' ) goto OptionError;                        /* c */
                                                                         /* h */
      ccoptions[++ccoptionscnt] = &argv[idx][4];                         /* i */
                                                                         /* n */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* G */
                                                                         /* e */
   case 'd':                                                             /* n */
    if ( strcmp(argv[idx]+1,"d") == 0 ) {                                /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                              -d                              */     /* t */
  /*                          Debug mode                          */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      debug = TRUE;                                                      /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"db") == 0 ) {                        /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                        -db <database>                        */     /*   */
  /*        Use <database> as inter-module data repository        */     /* C */
  /* ------------------------------------------------------------ */     /* h */
    CorrectUsage = "Usage: -db <database>";                              /* a */
    if ( (++idx) >= argc ) goto OptionError;                             /* n */
                                                                         /* g */
      mdb = argv[idx];                                                   /* e */
                                                                         /*   */
      break;                                                             /* M */
    } else if ( strcmp(argv[idx]+1,"dfuse") == 0 ) {                     /* a */
                                                                         /* c */
  /* ------------------------------------------------------------ */     /* h */
  /*                            -dfuse                            */     /* i */
  /*                Preform dependent loop fusion                 */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      nodfuse = FALSE;                                                   /* G */
                                                                         /* e */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"double_real") == 0 ) {               /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                         -double_real                         */     /* t */
  /*        Treat all SISAL real data as double_real data.        */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      dbl = TRUE;                                                        /* D */
                                                                         /* o */
      break;                                                             /*   */
    }                                                                    /* N */
    goto OptionError;                                                    /* o */
                                                                         /* t */
   case 'e':                                                             /*   */
    if ( strcmp(argv[idx]+1,"e") == 0 ) {                                /* C */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* a */
  /*                          -e <funct>                          */     /* n */
  /*                Use <funct> as an entry point                 */     /* g */
  /* ------------------------------------------------------------ */     /* e */
    CorrectUsage = "Usage: -e <funct>";                                  /*   */
    if ( (++idx) >= argc ) goto OptionError;                             /* M */
                                                                         /* a */
      eoptions[++eoptioncnt] = argv[idx];                                /* c */
                                                                         /* h */
      break;                                                             /* i */
    } else if ( strcmp(argv[idx]+1,"explode") == 0 ) {                   /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -explode                           */     /* G */
  /*                  Aggressively explode loops                  */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      explode = TRUE;                                                    /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"explodeI") == 0 ) {                  /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                          -explodeI                           */     /* D */
  /*             Aggressively explode innermost loops             */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      explode  = TRUE;                                                   /* o */
      explodeI = TRUE;                                                   /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( strcmp(argv[idx]+1,"externC") == 0 ) {                   /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                       -externC <funct>                       */     /* g */
  /*          Consider <funct> available as a C external          */     /* e */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -externC <funct>";                            /* M */
    if ( (++idx) >= argc ) goto OptionError;                             /* a */
                                                                         /* c */
      coptions[++coptioncnt] = argv[idx];                                /* h */
                                                                         /* i */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"externFORTRAN") == 0 ) {             /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                    -externFORTRAN <funct>                    */     /* e */
  /*       Consider <funct> available as a FORTRAN external       */     /* n */
  /* ------------------------------------------------------------ */     /* e */
    CorrectUsage = "Usage: -externFORTRAN <funct>";                      /* r */
    if ( (++idx) >= argc ) goto OptionError;                             /* a */
                                                                         /* t */
      foptions[++foptioncnt] = argv[idx];                                /* e */
                                                                         /* d */
      break;                                                             /*   */
    }                                                                    /* D */
    goto OptionError;                                                    /* o */
                                                                         /*   */
   case 'f':                                                             /* N */
    if ( strcmp(argv[idx]+1,"fflopinfo") == 0 ) {                        /* o */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /*   */
  /*                      -fflopinfo <funct>                      */     /* C */
  /*       Write floating point counts in <funct> to stderr       */     /* h */
  /* ------------------------------------------------------------ */     /* a */
    CorrectUsage = "Usage: -fflopinfo <funct>";                          /* n */
    if ( (++idx) >= argc ) goto OptionError;                             /* g */
                                                                         /* e */
      calloptions[++calloptioncnt] = argv[idx];                          /*   */
      flopoptions[++flopoptioncnt] = argv[idx];                          /* M */
      flpinfo = TRUE;                                                    /* a */
                                                                         /* c */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"flopinfo") == 0 ) {                  /* i */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                          -flopinfo                           */     /*   */
  /*          Write all floating point counts to s.info           */     /* G */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* n */
      flpinfo = TRUE;                                                    /* e */
                                                                         /* r */
      break;                                                             /* a */
    } else if ( strcmp(argv[idx]+1,"forC") == 0 ) {                      /* t */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* d */
  /*                            -forC                             */     /*   */
  /*             Compile into a program callable by C             */     /* D */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /*   */
      forC = TRUE;                                                       /* N */
                                                                         /* o */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"forFORTRAN") == 0 ) {                /*   */
                                                                         /* C */
  /* ------------------------------------------------------------ */     /* h */
  /*                         -forFORTRAN                          */     /* a */
  /*          Compile into a program callable by FORTRAN          */     /* n */
  /* ------------------------------------------------------------ */     /* g */
                                                                         /* e */
      forF = TRUE;                                                       /*   */
                                                                         /* M */
      break;                                                             /* a */
    } else if ( strcmp(argv[idx]+1,"freeall") == 0 ) {                   /* c */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* i */
  /*                           -freeall                           */     /* n */
  /*                 Force release of all storage                 */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* G */
      freeall = TRUE;                                                    /* e */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"fuse") == 0 ) {                      /* r */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* t */
  /*                            -fuse                             */     /* e */
  /*                     Perform loop fusion                      */     /* d */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* D */
      noifuse  = FALSE;                                                  /* o */
      nodfuse = FALSE;                                                   /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( strncmp(argv[idx]+1,"ff=",3) == 0 ) {                    /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                       -ff=<directive>                        */     /* h */
  /*        Give <directive> to the local FORTRAN compiler        */     /* a */
  /* ------------------------------------------------------------ */     /* n */
    CorrectUsage = "Usage: -ff=<directive>";                             /* g */
    if ( argv[idx][3] == '\0' ) goto OptionError;                        /* e */
                                                                         /*   */
      ffoptions[++ffoptionscnt] = &argv[idx][4];                         /* M */
                                                                         /* a */
      break;                                                             /* c */
    } else if ( strncmp(argv[idx]+1,"front=",6) == 0 ) {                 /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                      -front=<frontend>                       */     /* e */
  /*        Select SISAL1.2 or SISAL90 language definition        */     /*   */
  /* ------------------------------------------------------------ */     /* G */
    CorrectUsage = "Usage: -front=<frontend>";                           /* e */
    if ( argv[idx][6] == '\0' ) goto OptionError;                        /* n */
                                                                         /* e */
      if (strcmp(&argv[idx][strlen("-front=")], "SISAL90")==0) {         /* r */
        front_vers = 90;                                                 /* a */
      }                                                                  /* t */
                                                                         /* e */
      break;                                                             /* d */
    }                                                                    /*   */
    goto OptionError;                                                    /* D */
                                                                         /* o */
   case 'g':                                                             /*   */
    if ( strcmp(argv[idx]+1,"glue") == 0 ) {                             /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                            -glue                             */     /*   */
  /*    Disable the optimization of non-inlined function calls    */     /* C */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* a */
      SISglue = TRUE;                                                    /* n */
                                                                         /* g */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* M */
                                                                         /* a */
   case 'h':                                                             /* c */
    if ( strcmp(argv[idx]+1,"help") == 0 ) {                             /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                            -help                             */     /* e */
  /*                        Same as -usage                        */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
      PrintUsageTable(argv,idx);                                         /* n */
                                                                         /* e */
      break;                                                             /* r */
    } else if ( strcmp(argv[idx]+1,"hybrid") == 0 ) {                    /* a */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -hybrid                            */     /* d */
  /*               Generate both C and FORTRAN code               */     /*   */
  /* ------------------------------------------------------------ */     /* D */
                                                                         /* o */
      hybrid = TRUE;                                                     /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( isdigit(argv[idx][2]) ) {                                /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                           -h<num>                            */     /* h */
  /*        Slice loops only if cost is greater than <num>        */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      huge = argv[idx];                                                  /* e */
      huge[1] = 'H';                                                     /*   */
                                                                         /* M */
      break;                                                             /* a */
    }                                                                    /* c */
    goto OptionError;                                                    /* h */
                                                                         /* i */
   case 'i':                                                             /* n */
    if ( strcmp(argv[idx]+1,"icse") == 0 ) {                             /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                            -icse                             */     /* e */
  /*        Push identical operations out of conditionals         */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* r */
      noaggressive = FALSE;                                              /* a */
                                                                         /* t */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"info") == 0 ) {                      /* d */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* D */
  /*                            -info                             */     /* o */
  /*                Produce an information listing                */     /*   */
  /* ------------------------------------------------------------ */     /* N */
                                                                         /* o */
      info |= ParseMap("1");                                             /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( strcmp(argv[idx]+1,"inlineall") == 0 ) {                 /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                          -inlineall                          */     /* g */
  /*                     Inline all functions                     */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* M */
      inlineall = TRUE;                                                  /* a */
                                                                         /* c */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"inter") == 0 ) {                     /* i */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                            -inter                            */     /*   */
  /*         Interactively select functions for inlining          */     /* G */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* n */
      inter = TRUE;                                                      /* e */
                                                                         /* r */
      break;                                                             /* a */
    } else if ( isdigit(argv[idx][2]) ) {                                /* t */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* d */
  /*                           -i<num>                            */     /*   */
  /*            Set assumed iteration count for loops             */     /* D */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /*   */
      iter = argv[idx];                                                  /* N */
      iter[1] = '@';                                                     /* o */
                                                                         /* t */
      break;                                                             /*   */
    } else if ( strncmp(argv[idx]+1,"info=",5) == 0 ) {                  /* C */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* a */
  /*                      -info=<rangelist>                       */     /* n */
  /*                     Get more information                     */     /* g */
  /* ------------------------------------------------------------ */     /* e */
    CorrectUsage = "Usage: -info=<rangelist>";                           /*   */
    if ( argv[idx][5] == '\0' ) goto OptionError;                        /* M */
                                                                         /* a */
      info |= ParseMap(argv[idx]+6);                                     /* c */
      infos = argv[idx]+6;                                               /* h */
                                                                         /* i */
      break;                                                             /* n */
    }                                                                    /* e */
    goto OptionError;                                                    /*   */
                                                                         /* G */
   case 'l':                                                             /* e */
    if ( strcmp(argv[idx]+1,"listing") == 0 ) {                          /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* r */
  /*                           -listing                           */     /* a */
  /*        Produce a root.lst file for each root.sis file        */     /* t */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* d */
      list = TRUE;                                                       /*   */
                                                                         /* D */
      break;                                                             /* o */
    } else {                                                             /*   */
                                                                         /* N */
  /* ------------------------------------------------------------ */     /* o */
  /*                         -l<library>                          */     /* t */
  /*                     Add a loader library                     */     /*   */
  /* ------------------------------------------------------------ */     /* C */
    CorrectUsage = "Usage: -l<library>";                                 /* h */
    if ( argv[idx][1] == '\0' ) goto OptionError;                        /* a */
                                                                         /* n */
      file_ld[++ldcnt] = argv[idx];                                      /* g */
                                                                         /* e */
      break;                                                             /*   */
    }                                                                    /* M */
                                                                         /* a */
   case 'm':                                                             /* c */
    if ( strcmp(argv[idx]+1,"main") == 0 ) {                             /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                        -main <funct>                         */     /* e */
  /*               Use <funct> as main entry point                */     /*   */
  /* ------------------------------------------------------------ */     /* G */
    CorrectUsage = "Usage: -main <funct>";                               /* e */
    if ( (++idx) >= argc ) goto OptionError;                             /* n */
                                                                         /* e */
      eoptions[++eoptioncnt] = argv[idx];                                /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"makeloopreport") == 0 ) {            /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                    -makeloopreport <file>                    */     /* D */
  /*   Create a loop report showing the partitioner's decisions   */     /* o */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -makeloopreport <file>";                      /* N */
    if ( (++idx) >= argc ) goto OptionError;                             /* o */
                                                                         /* t */
      LoopReportOut = argv[idx];                                         /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"maxconcur") == 0 ) {                 /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                          -maxconcur                          */     /* e */
  /*    Disable cost estimate.  Parallelize all eligible loops    */     /*   */
  /* ------------------------------------------------------------ */     /* M */
                                                                         /* a */
      concur = TRUE;                                                     /* c */
      huge  = "-H1.0";                                                   /* h */
      procs = "-P4000000";                                               /* i */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"module") == 0 ) {                    /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -module                            */     /* n */
  /*             Compile files into a callable module             */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      smodule = TRUE;                                                    /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"movereads") == 0 ) {                 /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                          -movereads                          */     /*   */
  /*                 Move read operations (Cray)                  */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      movereads = TRUE;                                                  /*   */
                                                                         /* C */
      break;                                                             /* h */
    }                                                                    /* a */
    goto OptionError;                                                    /* n */
                                                                         /* g */
   case 'n':                                                             /* e */
    if ( strcmp(argv[idx]+1,"nancy") == 0 ) {                            /*   */
                                                                         /* M */
  /* ------------------------------------------------------------ */     /* a */
  /*                            -nancy                            */     /* c */
  /*        Do not use original Cray microtasking software        */     /* h */
  /* ------------------------------------------------------------ */     /* i */
                                                                         /* n */
      useORTS = FALSE;                                                   /* e */
                                                                         /*   */
      break;                                                             /* G */
    } else if ( strcmp(argv[idx]+1,"newchains") == 0 ) {                 /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                          -newchains                          */     /* r */
  /*                    Form Cray X-MP Chains                     */     /* a */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /* e */
      newchains = TRUE;                                                  /* d */
                                                                         /*   */
      break;                                                             /* D */
    } else if ( strcmp(argv[idx]+1,"nltss") == 0 ) {                     /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                            -nltss                            */     /* o */
  /*            Use NLTSS pragmas to generate vectors             */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      nltss = TRUE;                                                      /* h */
                                                                         /* a */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"noOinvar") == 0 ) {                  /* g */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                          -noOinvar                           */     /* M */
  /*          Do not remove invariants from inner loops           */     /* a */
  /* ------------------------------------------------------------ */     /* c */
                                                                         /* h */
      noOinvar = TRUE;                                                   /* i */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"noaimp") == 0 ) {                    /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -noaimp                            */     /* n */
  /*              Do not optimize array dereferences              */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      noaimp = TRUE;                                                     /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"noamove") == 0 ) {                   /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                           -noamove                           */     /*   */
  /*   Do not apply anti-movement optimization (from compounds)   */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      noamove = TRUE;                                                    /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"noassoc") == 0 ) {                   /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                           -noassoc                           */     /* e */
  /*     Disable parallel reduction of associative operations     */     /*   */
  /* ------------------------------------------------------------ */     /* M */
                                                                         /* a */
      noassoc = TRUE;                                                    /* c */
                                                                         /* h */
      break;                                                             /* i */
    } else if ( strcmp(argv[idx]+1,"nobip") == 0 ) {                     /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -nobip                            */     /* G */
  /*                         Disable BIP                          */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      nobip = TRUE;                                                      /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"nobipmv") == 0 ) {                   /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -nobipmv                           */     /* D */
  /*       Do not allow buffer movement in the C generator        */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      bipmv = FALSE;                                                     /* o */
                                                                         /* t */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"nobounds") == 0 ) {                  /* C */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* a */
  /*                          -nobounds                           */     /* n */
  /*       Do not generate code to check for various errors       */     /* g */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      bounds = FALSE;                                                    /* M */
                                                                         /* a */
      break;                                                             /* c */
    } else if ( strcmp(argv[idx]+1,"nobrec") == 0 ) {                    /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                           -nobrec                            */     /* e */
  /*              Disable basic record optimization               */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
      nobrec = TRUE;                                                     /* n */
                                                                         /* e */
      break;                                                             /* r */
    } else if ( strcmp(argv[idx]+1,"nocagg") == 0 ) {                    /* a */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -nocagg                            */     /* d */
  /*     Do not mark constant aggregates for static building      */     /*   */
  /* ------------------------------------------------------------ */     /* D */
                                                                         /* o */
      nocagg = TRUE;                                                     /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"nochains") == 0 ) {                  /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                          -nochains                           */     /* h */
  /*                   Disable vector chaining                    */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      chains    = FALSE;                                                 /* e */
      newchains = FALSE;                                                 /*   */
                                                                         /* M */
      break;                                                             /* a */
    } else if ( strcmp(argv[idx]+1,"nocom") == 0 ) {                     /* c */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* i */
  /*                            -nocom                            */     /* n */
  /*          Disable Sequent code improvement migration          */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* G */
      nocom = TRUE;                                                      /* e */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"noconcur") == 0 ) {                  /* r */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* t */
  /*                          -noconcur                           */     /* e */
  /*                Disable concurrent processing                 */     /* d */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* D */
      concur = FALSE;                                                    /* o */
                                                                         /*   */
      break;                                                             /* N */
    } else if ( strcmp(argv[idx]+1,"nocpp") == 0 ) {                     /* o */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -nocpp                            */     /* C */
  /*         Do not run the C preprocessor on SISAL files         */     /* h */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* n */
      nocpp = TRUE;                                                      /* g */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"nocse") == 0 ) {                     /* M */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* c */
  /*                            -nocse                            */     /* h */
  /*               Disable common subr. eliminator                */     /* i */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      nocse = TRUE;                                                      /*   */
                                                                         /* G */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"nodead") == 0 ) {                    /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* r */
  /*                           -nodead                            */     /* a */
  /*              No dead code removal in optimizer               */     /* t */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* d */
      dead = FALSE;                                                      /*   */
                                                                         /* D */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"nodfuse") == 0 ) {                   /*   */
                                                                         /* N */
  /* ------------------------------------------------------------ */     /* o */
  /*                           -nodfuse                           */     /* t */
  /*                Disable dependent loop fusion                 */     /*   */
  /* ------------------------------------------------------------ */     /* C */
                                                                         /* h */
      nodfuse = TRUE;                                                    /* a */
                                                                         /* n */
      break;                                                             /* g */
    } else if ( strcmp(argv[idx]+1,"nodope") == 0 ) {                    /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* M */
  /*                           -nodope                            */     /* a */
  /*            Do not apply dope vector optimizations            */     /* c */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* i */
      nodope = TRUE;                                                     /* n */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"nofcopy") == 0 ) {                   /* G */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* n */
  /*                           -nofcopy                           */     /* e */
  /*                           Unknown?                           */     /* r */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* t */
      nofcopy = TRUE;                                                    /* e */
                                                                         /* d */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"nofiss") == 0 ) {                    /* D */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -nofiss                            */     /* N */
  /*                Do not attempt record fission                 */     /* o */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /*   */
      norecf = TRUE;                                                     /* C */
                                                                         /* h */
      break;                                                             /* a */
    } else if ( strcmp(argv[idx]+1,"nofold") == 0 ) {                    /* n */
                                                                         /* g */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -nofold                            */     /*   */
  /*            Do not attempt to fold constant values            */     /* M */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* c */
      nofold = TRUE;                                                     /* h */
                                                                         /* i */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"nofuse") == 0 ) {                    /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                           -nofuse                            */     /* e */
  /*                   Disable all loop fusion                    */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* r */
      noifuse = TRUE;                                                    /* a */
      nodfuse = TRUE;                                                    /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"nogcse") == 0 ) {                    /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                           -nogcse                            */     /*   */
  /*        Do not attempt global common subr. elimination        */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      nogcse = TRUE;                                                     /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"noif1opt") == 0 ) {                  /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                          -noif1opt                           */     /* e */
  /*                 Turn off IF1 code improvers                  */     /*   */
  /* ------------------------------------------------------------ */     /* M */
                                                                         /* a */
      noif1opt = TRUE;                                                   /* c */
                                                                         /* h */
      break;                                                             /* i */
    } else if ( strcmp(argv[idx]+1,"noifuse") == 0 ) {                   /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -noifuse                           */     /* G */
  /*               Disable independent loop fusion                */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      noifuse = TRUE;                                                    /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"noimp") == 0 ) {                     /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -noimp                            */     /* D */
  /*      Compile with the C compiler's optimizers disabled       */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      noimp = TRUE;                                                      /* o */
                                                                         /* t */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"noimp") == 0 ) {                     /* C */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* a */
  /*                            -noimp                            */     /* n */
  /*           Set -nvnoopt loader option for the Crays           */     /* g */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      noimp = TRUE;                                                      /* M */
                                                                         /* a */
      break;                                                             /* c */
    } else if ( strcmp(argv[idx]+1,"noinline") == 0 ) {                  /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                          -noinline                           */     /* e */
  /*                   Do not inline functions                    */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
      noinline = TRUE;                                                   /* n */
                                                                         /* e */
      break;                                                             /* r */
    } else if ( strcmp(argv[idx]+1,"noinvar") == 0 ) {                   /* a */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -noinvar                           */     /* d */
  /*             Disable invariant removal optimizer              */     /*   */
  /* ------------------------------------------------------------ */     /* D */
                                                                         /* o */
      noinvar = TRUE;                                                    /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"noinvert") == 0 ) {                  /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                          -noinvert                           */     /* h */
  /*          Do not perform loop inversion optimization          */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      noinvert = TRUE;                                                   /* e */
                                                                         /*   */
      break;                                                             /* M */
    } else if ( strcmp(argv[idx]+1,"noload") == 0 ) {                    /* a */
                                                                         /* c */
  /* ------------------------------------------------------------ */     /* h */
  /*                           -noload                            */     /* i */
  /*      Do not create the executable, stop with a .o file       */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      stopc = TRUE;                                                      /* G */
      TMPDIR="";                                                         /* e */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"nomem") == 0 ) {                     /* r */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* t */
  /*                            -nomem                            */     /* e */
  /*               Disable build in place analysis                */     /* d */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* D */
      noif2mem = TRUE;                                                   /* o */
      preb     = "-Y0";                                                  /*   */
                                                                         /* N */
      break;                                                             /* o */
    } else if ( strcmp(argv[idx]+1,"nomig") == 0 ) {                     /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                            -nomig                            */     /* h */
  /*            Do not migrate operations toward users            */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      nomig = TRUE;                                                      /* e */
                                                                         /*   */
      break;                                                             /* M */
    } else if ( strcmp(argv[idx]+1,"nomovereads") == 0 ) {               /* a */
                                                                         /* c */
  /* ------------------------------------------------------------ */     /* h */
  /*                         -nomovereads                         */     /* i */
  /*                  Move array read operations                  */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      movereads = FALSE;                                                 /* G */
                                                                         /* e */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"nonormidx") == 0 ) {                 /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                          -nonormidx                          */     /* t */
  /*               Do not normalize array indexing                */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      nonormidx = TRUE;                                                  /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"noopt") == 0 ) {                     /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                            -noopt                            */     /*   */
  /*                   Disable all optimization                   */     /* C */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* a */
      noopt = TRUE;                                                      /* n */
      noimp = TRUE;                                                      /* g */
      bounds = FALSE;                                                    /* e */
                                                                         /*   */
      break;                                                             /* M */
    } else if ( strcmp(argv[idx]+1,"nopreb") == 0 ) {                    /* a */
                                                                         /* c */
  /* ------------------------------------------------------------ */     /* h */
  /*                           -nopreb                            */     /* i */
  /*                   Disable array prebuilds                    */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /*   */
      preb = "-Y0";                                                      /* G */
                                                                         /* e */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"norag") == 0 ) {                     /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                            -norag                            */     /* t */
  /*               Generate code called by FORTRAN                */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      norag = TRUE;                                                      /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"noregs") == 0 ) {                    /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                           -noregs                            */     /*   */
  /*     Do not assign register prefixes to generated C code      */     /* C */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* a */
      noregs = TRUE;                                                     /* n */
                                                                         /* g */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"noscalar") == 0 ) {                  /*   */
                                                                         /* M */
  /* ------------------------------------------------------------ */     /* a */
  /*                          -noscalar                           */     /* c */
  /*                 Disable scalar optimizations                 */     /* h */
  /* ------------------------------------------------------------ */     /* i */
                                                                         /* n */
      noscalar = TRUE;                                                   /* e */
                                                                         /*   */
      break;                                                             /* G */
    } else if ( strcmp(argv[idx]+1,"nosfuse") == 0 ) {                   /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -nosfuse                           */     /* r */
  /*                    Disable select fusion                     */     /* a */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /* e */
      nosfuse = TRUE;                                                    /* d */
                                                                         /*   */
      break;                                                             /* D */
    } else if ( strcmp(argv[idx]+1,"nosplit") == 0 ) {                   /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                           -nosplit                           */     /* o */
  /*                Do not preform loop splitting                 */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      nosplit = TRUE;                                                    /* h */
                                                                         /* a */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"nostr") == 0 ) {                     /* g */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -nostr                            */     /* M */
  /*      Do not attempt to identify single threaded streams      */     /* a */
  /* ------------------------------------------------------------ */     /* c */
                                                                         /* h */
      nostr = TRUE;                                                      /* i */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"nostrip") == 0 ) {                   /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -nostrip                           */     /* n */
  /*             Do not perform return node stripping             */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      nostrip = TRUE;                                                    /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"notgcse") == 0 ) {                   /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                           -notgcse                           */     /*   */
  /*         Do not force global common subr. elimination         */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      notgcse = TRUE;                                                    /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"nounroll") == 0 ) {                  /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                          -nounroll                           */     /* e */
  /*                    Disable loop unrolling                    */     /*   */
  /* ------------------------------------------------------------ */     /* M */
                                                                         /* a */
      unroll = "-U0";                                                    /* c */
                                                                         /* h */
      break;                                                             /* i */
    } else if ( strcmp(argv[idx]+1,"noup") == 0 ) {                      /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                            -noup                             */     /* G */
  /*               Disable update in place analysis               */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      noif2up = TRUE;                                                    /* r */
      preb  = "-Y0";                                                     /* a */
                                                                         /* t */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"novector") == 0 ) {                  /* d */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* D */
  /*                          -novector                           */     /* o */
  /*                    Disable vectorization                     */     /*   */
  /* ------------------------------------------------------------ */     /* N */
                                                                         /* o */
      novec = TRUE;                                                      /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( isdigit(argv[idx][2]) ) {                                /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                           -n<num>                            */     /* g */
  /*    Consider only loops nested <num> deep for concurrency     */     /* e */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* M */
      level = argv[idx];                                                 /* a */
      level[1] = 'L';                                                    /* c */
                                                                         /* h */
      break;                                                             /* i */
    }                                                                    /* n */
    goto OptionError;                                                    /* e */
                                                                         /*   */
   case 'o':                                                             /* G */
    if ( strcmp(argv[idx]+1,"o") == 0 ) {                                /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                         -o <target>                          */     /* r */
  /*   Name the output <target> not s.out or <source_prefix>.o    */     /* a */
  /* ------------------------------------------------------------ */     /* t */
    CorrectUsage = "Usage: -o <target>";                                 /* e */
    if ( (++idx) >= argc ) goto OptionError;                             /* d */
                                                                         /*   */
      aabs = argv[idx];                                                  /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"oo") == 0 ) {                        /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                         -oo <target>                         */     /*   */
  /*        Just like -o, but sisal extensions are legal.         */     /* C */
  /* ------------------------------------------------------------ */     /* h */
    CorrectUsage = "Usage: -oo <target>";                                /* a */
    if ( (++idx) >= argc ) goto OptionError;                             /* n */
                                                                         /* g */
      NameSafety = FALSE;                                                /* e */
      aabs = argv[idx];                                                  /*   */
                                                                         /* M */
      break;                                                             /* a */
    }                                                                    /* c */
    goto OptionError;                                                    /* h */
                                                                         /* i */
   case 'p':                                                             /* n */
    if ( strcmp(argv[idx]+1,"prof") == 0 ) {                             /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                            -prof                             */     /* e */
  /*      Generate a time execution profile of the compiler       */     /* n */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* r */
      prof = TRUE;                                                       /* a */
      verbose = TRUE;                                                    /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"progress") == 0 ) {                  /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                          -progress                           */     /*   */
  /*        Generate progress report in C code generation         */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      prog = TRUE;                                                       /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( isdigit(argv[idx][2]) ) {                                /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                           -p<num>                            */     /* e */
  /*            Assume <num> processors for partioning            */     /*   */
  /* ------------------------------------------------------------ */     /* M */
                                                                         /* a */
      procs = argv[idx];                                                 /* c */
      procs[1] = 'P';                                                    /* h */
                                                                         /* i */
      break;                                                             /* n */
      } else if ( strncmp(argv[idx]+1,"patch=",6) == 0 && isdigit(argv[idx][7]) ) {
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                         -patch=<num>                         */     /* G */
  /*   Apply a dynamic patch to one of the backend applications   */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      if ( PatchCount >= sizeof(Patches)/sizeof(Patches[0]) ) {          /* r */
        Error2( argv[idx] , "-- Too many patches" );                     /* a */
      }                                                                  /* t */
                                                                         /* e */
      Patches[PatchCount] = (char *) malloc(strlen(argv[idx]));          /* d */
      sprintf(Patches[PatchCount],"-p%s",argv[idx]+7);                   /*   */
      PatchCount++;                                                      /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( argv[idx][2] == 'b' && isdigit(argv[idx][3]) ) {         /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                           -pb<num>                           */     /*   */
  /*            Set array prebuild dimension to <num>             */     /* C */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* a */
      preb = &(argv[idx][1]);                                            /* n */
      preb[0] = '-';                                                     /* g */
      preb[1] = 'Y';                                                     /* e */
                                                                         /*   */
      break;                                                             /* M */
    }                                                                    /* a */
    goto OptionError;                                                    /* c */
                                                                         /* h */
   case 'r':                                                             /* i */
    if ( strcmp(argv[idx]+1,"r") == 0 ) {                                /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                          -r <funct>                          */     /* G */
  /*          Consider <funct> available as a reduction           */     /* e */
  /* ------------------------------------------------------------ */     /* n */
    CorrectUsage = "Usage: -r <funct>";                                  /* e */
    if ( (++idx) >= argc ) goto OptionError;                             /* r */
                                                                         /* a */
      roptions[++roptioncnt] = argv[idx];         /* reduction function */
      /* calloptions[++calloptioncnt] = argv[idx];*//* inline off */     /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"real") == 0 ) {                      /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                            -real                             */     /*   */
  /*        Treat all SISAL double_real data as real data.        */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      flt = TRUE;                                                        /*   */
                                                                         /* C */
      break;                                                             /* h */
    }                                                                    /* a */
    goto OptionError;                                                    /* n */
                                                                         /* g */
   case 's':                                                             /* e */
    if ( strcmp(argv[idx]+1,"sdbx") == 0 ) {                             /*   */
                                                                         /* M */
  /* ------------------------------------------------------------ */     /* a */
  /*                            -sdbx                             */     /* c */
  /*    Generate code to interface with the symbolic debugger     */     /* h */
  /* ------------------------------------------------------------ */     /* i */
                                                                         /* n */
      sdbx = TRUE;                                                       /* e */
      bounds = FALSE;   /* The TRUE default was interfering with other checks */
                                                                         /*   */
      break;                                                             /* G */
    } else if ( strcmp(argv[idx]+1,"seq") == 0 ) {                       /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                             -seq                             */     /* r */
  /*               Compile for sequential execution               */     /* a */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /* e */
      concur = FALSE;                                                    /* d */
      novec = TRUE;                                                      /*   */
                                                                         /* D */
      break;                                                             /* o */
    }                                                                    /*   */
    goto OptionError;                                                    /* N */
                                                                         /* o */
   case 't':                                                             /* t */
    if ( strcmp(argv[idx]+1,"tgcse") == 0 ) {                            /*   */
                                                                         /* C */
  /* ------------------------------------------------------------ */     /* h */
  /*                            -tgcse                            */     /* a */
  /*            Force global Common Subr. Elimination             */     /* n */
  /* ------------------------------------------------------------ */     /* g */
                                                                         /* e */
      notgcse = FALSE;                                                   /*   */
                                                                         /* M */
      break;                                                             /* a */
    } else if ( strcmp(argv[idx]+1,"time") == 0 ) {                      /* c */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* i */
  /*                        -time <funct>                         */     /* n */
  /*           Generating timing code for this function           */     /* e */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -time <funct>";                               /* G */
    if ( (++idx) >= argc ) goto OptionError;                             /* e */
                                                                         /* n */
      calloptions[++calloptioncnt] = argv[idx];                          /* e */
      timeoptions[++timeoptioncnt] = argv[idx];                          /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"timeall") == 0 ) {                   /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -timeall                           */     /* D */
  /*            Generate timing code for all functions            */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      timeall = TRUE;                                                    /* o */
      noinline = TRUE;                                                   /* t */
                                                                         /*   */
      break;                                                             /* C */
    } else if ( strcmp(argv[idx]+1,"tmpdir") == 0 ) {                    /* h */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* n */
  /*                      -tmpdir <dirname>                       */     /* g */
  /*           Put intermediate temp files in <dirname>           */     /* e */
  /* ------------------------------------------------------------ */     /*   */
    CorrectUsage = "Usage: -tmpdir <dirname>";                           /* M */
    if ( (++idx) >= argc ) goto OptionError;                             /* a */
                                                                         /* c */
      TMPDIR = argv[idx];                                                /* h */
                                                                         /* i */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"trace") == 0 ) {                     /* e */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* G */
  /*                        -trace <funct>                        */     /* e */
  /*          Generating tracing code for this function           */     /* n */
  /* ------------------------------------------------------------ */     /* e */
    CorrectUsage = "Usage: -trace <funct>";                              /* r */
    if ( (++idx) >= argc ) goto OptionError;                             /* a */
                                                                         /* t */
      calloptions[++calloptioncnt] = argv[idx];                          /* e */
      traceoptions[++traceoptioncnt] = argv[idx];                        /* d */
                                                                         /*   */
      break;                                                             /* D */
    } else if ( strcmp(argv[idx]+1,"traceall") == 0 ) {                  /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                          -traceall                           */     /* o */
  /*           Generate tracing code for all functions            */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      traceall = TRUE;                                                   /* h */
      noinline = TRUE;                                                   /* a */
                                                                         /* n */
      break;                                                             /* g */
    }                                                                    /* e */
    goto OptionError;                                                    /*   */
                                                                         /* M */
   case 'u':                                                             /* a */
    if ( strcmp(argv[idx]+1,"usage") == 0 ) {                            /* c */
                                                                         /* h */
  /* ------------------------------------------------------------ */     /* i */
  /*                            -usage                            */     /* n */
  /*                      Produce this list                       */     /* e */
  /* ------------------------------------------------------------ */     /*   */
    PrintUsageTable(argv,idx);                                           /* G */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"useloopreport") == 0 ) {             /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /* r */
  /*                    -useloopreport <file>                     */     /* a */
  /*          Use loopreport from a previous compilation          */     /* t */
  /* ------------------------------------------------------------ */     /* e */
    CorrectUsage = "Usage: -useloopreport <file>";                       /* d */
    if ( (++idx) >= argc ) goto OptionError;                             /*   */
                                                                         /* D */
      LoopReportIn = argv[idx];                                          /* o */
                                                                         /*   */
      break;                                                             /* N */
    } else if ( isdigit(argv[idx][2]) ) {                                /* o */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -u<num>                            */     /* C */
  /*         Unroll loops with less than <num> iterations         */     /* h */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* n */
      unroll = argv[idx];                                                /* g */
      unroll[1] = 'U';                                                   /* e */
                                                                         /*   */
      break;                                                             /* M */
    }                                                                    /* a */
    goto OptionError;                                                    /* c */
                                                                         /* h */
   case 'v':                                                             /* i */
    if ( strcmp(argv[idx]+1,"v") == 0 ) {                                /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                              -v                              */     /* G */
  /*                         Verbose mode                         */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
      verbose = TRUE;                                                    /* r */
                                                                         /* a */
      break;                                                             /* t */
    } else if ( strcmp(argv[idx]+1,"vector") == 0 ) {                    /* e */
                                                                         /* d */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -vector                            */     /* D */
  /*                     Enable vectorization                     */     /* o */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* N */
      novec = FALSE;                                                     /* o */
#ifndef CRAY                                                             /* t */
#ifndef ALLIANT                                                          /*   */
      cvector   = TRUE;             /* RIGHT NOW, THE CRAY IS THE DEFAULT */
      movereads = TRUE;                                                  /* C */
      chains    = TRUE;                                                  /* h */
      newchains = TRUE;                                                  /* a */
#endif                                                                   /* n */
#endif                                                                   /* g */
                                                                         /* e */
      break;                                                             /*   */
    }                                                                    /* M */
    goto OptionError;                                                    /* a */
                                                                         /* c */
   case 'w':                                                             /* h */
    if ( strcmp(argv[idx]+1,"w") == 0 ) {                                /* i */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                              -w                              */     /*   */
  /*                   Disable warning messages                   */     /* G */
  /* ------------------------------------------------------------ */     /* e */
                                                                         /* n */
      Warnings = FALSE;                                                  /* e */
                                                                         /* r */
      break;                                                             /* a */
    }                                                                    /* t */
    goto OptionError;                                                    /* e */
                                                                         /* d */
   case 'x':                                                             /*   */
    if ( strcmp(argv[idx]+1,"xchains") == 0 ) {                          /* D */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -xchains                           */     /* N */
  /*                    Allow vector chaining                     */     /* o */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /*   */
      chains = TRUE;                                                     /* C */
                                                                         /* h */
      break;                                                             /* a */
    }                                                                    /* n */
    goto OptionError;                                                    /* g */
   default: goto OptionError;                                            /* e */
  }                                                                      /*   */
}                                                                        /* M */
