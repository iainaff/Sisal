if (argv[idx][0] == '-' ) {                                              /* M */
  CorrectUsage = NULL;                                                   /* a */
  switch( argv[idx][1]) {                                                /* c */
                                                                         /* h */
   case 'X':                                                             /* i */
    if ( strcmp(argv[idx]+1,"X") == 0 ) {                                /* n */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                              -X                              */     /* G */
  /*       All remaining arguments treated as FIBRE inputs        */     /* e */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* e */
            idx++;                  /* Skip to next argument */          /* r */
            while ( idx < argc ) {                                       /* a */
              (void)strcat(ArgumentString,argv[idx]);                    /* t */
              (void)strcat(ArgumentString," ");                          /* e */
              idx++;                                                     /* d */
            }                                                            /*   */
                                                                         /* D */
      break;                                                             /* o */
    }                                                                    /*   */
    goto OptionError;                                                    /* N */
                                                                         /* o */
   case 'a':                                                             /* t */
    if ( argv[idx][2] == 'x' && isdigit(argv[idx][3]) ) {                /*   */
                                                                         /* C */
  /* ------------------------------------------------------------ */     /* h */
  /*                           -ax<num>                           */     /* a */
  /*                  Set array expansion factor                  */     /* n */
  /* ------------------------------------------------------------ */     /* g */
                                                                         /* e */
      if ( GET_Tmp( 3 ) < 0 ) goto OptionError;                          /*   */
      ArrayExpansion = Tmp;                                              /* M */
                                                                         /* a */
      break;                                                             /* c */
    }                                                                    /* h */
    goto OptionError;                                                    /* i */
                                                                         /* n */
   case 'b':                                                             /* e */
    if ( strcmp(argv[idx]+1,"b") == 0 ) {                                /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                              -b                              */     /* n */
  /*  Use distributed run queue system to bind work to processes  */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      BindParallelWork = TRUE;                                           /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"bformat") == 0 ) {                   /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                   -bformat <formatstring>                    */     /*   */
  /*           Change the default boolean output format           */     /* N */
  /* ------------------------------------------------------------ */     /* o */
    CorrectUsage = "Usage: -bformat <formatstring>";                     /* t */
    if ( (++idx) >= argc ) goto OptionError;                             /*   */
                                                                         /* C */
      bformat = ParseCEscapes(argv[idx]);                                /* h */
                                                                         /* a */
      break;                                                             /* n */
    } else if ( strcmp(argv[idx]+1,"blocked") == 0 ) {                   /* g */
                                                                         /* e */
  /* ------------------------------------------------------------ */     /*   */
  /*                           -blocked                           */     /* M */
  /*    Use a blocked scheduling algorithm for loops (default)    */     /* a */
  /* ------------------------------------------------------------ */     /* c */
                                                                         /* h */
      DefaultLoopStyle = 'B';                                            /* i */
                                                                         /* n */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* G */
                                                                         /* e */
   case 'c':                                                             /* n */
    if ( strcmp(argv[idx]+1,"cached") == 0 ) {                           /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                           -cached                            */     /* t */
  /*   Use cached scheduling algorithm instead of block slicing   */     /* e */
  /* ------------------------------------------------------------ */     /* d */
                                                                         /*   */
      DefaultLoopStyle = 'C';                                            /* D */
                                                                         /* o */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"cformat") == 0 ) {                   /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                   -cformat <formatstring>                    */     /*   */
  /* Change the default integer output format for printable characters */
  /* ------------------------------------------------------------ */     /* C */
    CorrectUsage = "Usage: -cformat <formatstring>";                     /* h */
    if ( (++idx) >= argc ) goto OptionError;                             /* a */
                                                                         /* n */
      cformat = ParseCEscapes(argv[idx]);                                /* g */
                                                                         /* e */
      break;                                                             /*   */
    } else if ( strcmp(argv[idx]+1,"cformat2") == 0 ) {                  /* M */
                                                                         /* a */
  /* ------------------------------------------------------------ */     /* c */
  /*                   -cformat2 <formatstring>                   */     /* h */
  /*    Change the default uprintable character output format     */     /* i */
  /* ------------------------------------------------------------ */     /* n */
    CorrectUsage = "Usage: -cformat2 <formatstring>";                    /* e */
    if ( (++idx) >= argc ) goto OptionError;                             /*   */
                                                                         /* G */
      cformat2 = ParseCEscapes(argv[idx]);                               /* e */
                                                                         /* n */
      break;                                                             /* e */
    }                                                                    /* r */
    goto OptionError;                                                    /* a */
                                                                         /* t */
   case 'd':                                                             /* e */
    if ( strcmp(argv[idx]+1,"dformat") == 0 ) {                          /* d */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* D */
  /*                   -dformat <formatstring>                    */     /* o */
  /*         Change the default double real output format         */     /*   */
  /* ------------------------------------------------------------ */     /* N */
    CorrectUsage = "Usage: -dformat <formatstring>";                     /* o */
    if ( (++idx) >= argc ) goto OptionError;                             /* t */
                                                                         /*   */
      dformat = ParseCEscapes(argv[idx]);                                /* C */
                                                                         /* h */
      break;                                                             /* a */
    } else if ( argv[idx][2] == 's' && isdigit(argv[idx][3]) ) {         /* n */
                                                                         /* g */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -ds<num>                           */     /*   */
  /*  Initialize the shared data management pool to <num> bytes.  */     /* M */
  /* ------------------------------------------------------------ */     /* a */
                                                                         /* c */
      if ( GET_Tmp( 3 ) <= 0 ) goto OptionError;                         /* h */
      DsaSize = Tmp;                                                     /* i */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( argv[idx][2] == 'x' && isdigit(argv[idx][3]) ) {         /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -dx<num>                           */     /* n */
  /* Set the exact fit storage allocation threshold to <num> bytes. */   /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      if ( GET_Tmp( 3 ) < 0 ) goto OptionError;                          /* t */
                                                                         /* e */
      XftThreshold = Tmp;                                                /* d */
                                                                         /*   */
      break;                                                             /* D */
    }                                                                    /* o */
    goto OptionError;                                                    /*   */
                                                                         /* N */
   case 'f':                                                             /* o */
    if ( strcmp(argv[idx]+1,"fformat") == 0 ) {                          /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                   -fformat <formatstring>                    */     /* h */
  /*            Change the default real  output format            */     /* a */
  /* ------------------------------------------------------------ */     /* n */
    CorrectUsage = "Usage: -fformat <formatstring>";                     /* g */
    if ( (++idx) >= argc ) goto OptionError;                             /* e */
                                                                         /*   */
      fformat = ParseCEscapes(argv[idx]);                                /* M */
                                                                         /* a */
      break;                                                             /* c */
    }                                                                    /* h */
    goto OptionError;                                                    /* i */
                                                                         /* n */
   case 'g':                                                             /* e */
    if ( strcmp(argv[idx]+1,"gss") == 0 ) {                              /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                             -gss                             */     /* n */
  /* Use guided self scheduling algorithm instead of block slicing */    /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      DefaultLoopStyle = 'G';                                            /* t */
                                                                         /* e */
      break;                                                             /* d */
    }                                                                    /*   */
    goto OptionError;                                                    /* D */
                                                                         /* o */
   case 'h':                                                             /*   */
    if ( strcmp(argv[idx]+1,"help") == 0 ) {                             /* N */
                                                                         /* o */
  /* ------------------------------------------------------------ */     /* t */
  /*                            -help                             */     /*   */
  /*                        Same as -usage                        */     /* C */
  /* ------------------------------------------------------------ */     /* h */
                                                                         /* a */
      PrintUsageTable(argv,idx);                                         /* n */
                                                                         /* g */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* M */
                                                                         /* a */
   case 'i':                                                             /* c */
    if ( strcmp(argv[idx]+1,"iformat") == 0 ) {                          /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                   -iformat <formatstring>                    */     /* e */
  /*           Change the default integer output format           */     /*   */
  /* ------------------------------------------------------------ */     /* G */
    CorrectUsage = "Usage: -iformat <formatstring>";                     /* e */
    if ( (++idx) >= argc ) goto OptionError;                             /* n */
                                                                         /* e */
      iformat = ParseCEscapes(argv[idx]);                                /* r */
                                                                         /* a */
      break;                                                             /* t */
    }                                                                    /* e */
    goto OptionError;                                                    /* d */
                                                                         /*   */
   case 'l':                                                             /* D */
    if ( argv[idx][2] == 's' && isdigit(argv[idx][3]) ) {                /* o */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* N */
  /*                           -ls<num>                           */     /* o */
  /*             Set the default slice count to <num>             */     /* t */
  /* ------------------------------------------------------------ */     /*   */
                                                                         /* C */
      if ( GET_Tmp( 3 ) <= 0 ) goto OptionError;                         /* h */
                                                                         /* a */
      LoopSlices = Tmp;                                                  /* n */
                                                                         /* g */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* M */
                                                                         /* a */
   case 'm':                                                             /* c */
    if ( strcmp(argv[idx]+1,"m") == 0 ) {                                /* h */
                                                                         /* i */
  /* ------------------------------------------------------------ */     /* n */
  /*                              -m                              */     /* e */
  /*    Indicate the master process on distributed processors.    */     /*   */
  /* ------------------------------------------------------------ */     /* G */
                                                                         /* e */
      /* return */;                                                      /* n */
                                                                         /* e */
      break;                                                             /* r */
    }                                                                    /* a */
    goto OptionError;                                                    /* t */
                                                                         /* e */
   case 'n':                                                             /* d */
    if ( strcmp(argv[idx]+1,"nb") == 0 ) {                               /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                             -nb                              */     /*   */
  /*                  Do not bind parallel work                   */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      BindParallelWork = FALSE;                                          /*   */
                                                                         /* C */
      break;                                                             /* h */
    } else if ( strcmp(argv[idx]+1,"nformat") == 0 ) {                   /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                   -nformat <formatstring>                    */     /* e */
  /*          Change the default null type output format          */     /*   */
  /* ------------------------------------------------------------ */     /* M */
    CorrectUsage = "Usage: -nformat <formatstring>";                     /* a */
    if ( (++idx) >= argc ) goto OptionError;                             /* c */
                                                                         /* h */
      nformat = ParseCEscapes(argv[idx]);                                /* i */
                                                                         /* n */
      break;                                                             /* e */
    } else if ( strcmp(argv[idx]+1,"nostrings") == 0 ) {                 /*   */
                                                                         /* G */
  /* ------------------------------------------------------------ */     /* e */
  /*                          -nostrings                          */     /* n */
  /*   Do not print array[character] in string (quoted) format    */     /* e */
  /* ------------------------------------------------------------ */     /* r */
                                                                         /* a */
      FibreStrings = FALSE;                                              /* t */
                                                                         /* e */
      break;                                                             /* d */
    } else if ( strcmp(argv[idx]+1,"nqs") == 0 ) {                       /*   */
                                                                         /* D */
  /* ------------------------------------------------------------ */     /* o */
  /*                             -nqs                             */     /*   */
  /*       ...options  Submit as NQS batch job (Cray only)        */     /* N */
  /* ------------------------------------------------------------ */     /* o */
                                                                         /* t */
      SubmitNQS(argc,argv,idx);                                          /*   */
                                                                         /* C */
      break;                                                             /* h */
    }                                                                    /* a */
    goto OptionError;                                                    /* n */
                                                                         /* g */
   case 'r':                                                             /* e */
    if ( strcmp(argv[idx]+1,"r") == 0 ) {                                /*   */
                                                                         /* M */
  /* ------------------------------------------------------------ */     /* a */
  /*                              -r                              */     /* c */
  /*  Append resource utilization information to the file s.info  */     /* h */
  /* ------------------------------------------------------------ */     /* i */
                                                                         /* n */
      GatherPerfInfo = TRUE;                                             /* e */
                                                                         /*   */
      break;                                                             /* G */
    }                                                                    /* e */
    goto OptionError;                                                    /* n */
                                                                         /* e */
   case 's':                                                             /* r */
    if ( strcmp(argv[idx]+1,"strided") == 0 ) {                          /* a */
                                                                         /* t */
  /* ------------------------------------------------------------ */     /* e */
  /*                           -strided                           */     /* d */
  /* Use strided loop scheduling algorithm instead of block slicing */   /*   */
  /* ------------------------------------------------------------ */     /* D */
                                                                         /* o */
      DefaultLoopStyle = 'S';                                            /*   */
                                                                         /* N */
      break;                                                             /* o */
    }                                                                    /* t */
    goto OptionError;                                                    /*   */
                                                                         /* C */
   case 't':                                                             /* h */
    if ( strncmp(argv[idx]+1,"tr=",3) == 0 ) {                           /* a */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* g */
  /*                       -tr=<attribute>                        */     /* e */
  /*                 Interface to tracer function                 */     /*   */
  /* ------------------------------------------------------------ */     /* M */
    CorrectUsage = "Usage: -tr=<attribute>";                             /* a */
    if ( argv[idx][3] == '\0' ) goto OptionError;                        /* c */
                                                                         /* h */
       ParseTracerCommand(argv[idx]+4);                                  /* i */
                                                                         /* n */
      break;                                                             /* e */
    }                                                                    /*   */
    goto OptionError;                                                    /* G */
                                                                         /* e */
   case 'u':                                                             /* n */
    if ( strcmp(argv[idx]+1,"usage") == 0 ) {                            /* e */
                                                                         /* r */
  /* ------------------------------------------------------------ */     /* a */
  /*                            -usage                            */     /* t */
  /*                      Produce this list                       */     /* e */
  /* ------------------------------------------------------------ */     /* d */
    PrintUsageTable(argv,idx);                                           /*   */
      break;                                                             /* D */
    }                                                                    /* o */
    goto OptionError;                                                    /*   */
                                                                         /* N */
   case 'w':                                                             /* o */
    if ( isdigit(argv[idx][2]) ) {                                       /* t */
                                                                         /*   */
  /* ------------------------------------------------------------ */     /* C */
  /*                           -w<num>                            */     /* h */
  /*           Set number of worker processes to <num>            */     /* a */
  /* ------------------------------------------------------------ */     /* n */
                                                                         /* g */
      if ( GET_Tmp( 2 ) <= 0 ) goto OptionError;                         /* e */
                                                                         /*   */
      if ( Tmp > MAX_PROCS ) goto OptionError;                           /* M */
                                                                         /* a */
      NumWorkers = Tmp;                                                  /* c */
                                                                         /* h */
      break;                                                             /* i */
    }                                                                    /* n */
    goto OptionError;                                                    /* e */
                                                                         /*   */
   case 'z':                                                             /* G */
    if ( strcmp(argv[idx]+1,"z") == 0 ) {                                /* e */
                                                                         /* n */
  /* ------------------------------------------------------------ */     /* e */
  /*                              -z                              */     /* r */
  /*              Do not print the program's output.              */     /* a */
  /* ------------------------------------------------------------ */     /* t */
                                                                         /* e */
          NoFibreOutput = TRUE;                                          /* d */
                                                                         /*   */
      break;                                                             /* D */
    }                                                                    /* o */
    goto OptionError;                                                    /*   */
   default: goto OptionError;                                            /* N */
  }                                                                      /* o */
}                                                                        /* t */
