/* M */  char *Usage[] = {
/* a */  "-X                    All remaining arguments treated as FIBRE inputs",
/* c */  "-ax<num>              Set array expansion factor",
/* h */  "-b                    Use distributed run queue system to bind work to processes",
/* i */  "-bformat <formatstring> Change the default boolean output format",
/* n */  "-blocked              Use a blocked scheduling algorithm for loops (default)",
/* e */  "-cached               Use cached scheduling algorithm instead of block slicing",
/*   */  "-cformat <formatstring> Change the default integer output format for printable characters",
/* G */  "-cformat2 <formatstring> Change the default uprintable character output format",
/* e */  "-dformat <formatstring> Change the default double real output format",
/* n */  "-ds<num>              Initialize the shared data management pool to <num> bytes.",
/* e */  "-dx<num>              Set the exact fit storage allocation threshold to <num> bytes.",
/* r */  "-fformat <formatstring> Change the default real  output format",
/* a */  "-gss                  Use guided self scheduling algorithm instead of block slicing",
/* t */  "-help                *Same as -usage",
/* e */  "-iformat <formatstring> Change the default integer output format",
/* d */  "-ls<num>              Set the default slice count to <num>",
/*   */  "-m                    Indicate the master process on distributed processors.",
/* D */  "-nb                  *Do not bind parallel work",
/* o */  "-nformat <formatstring> Change the default null type output format",
/*   */  "-nostrings            Do not print array[character] in string (quoted) format",
/* N */  "-nqs                  ...options  Submit as NQS batch job (Cray only)",
/* o */  "-r                    Append resource utilization information to the file s.info",
/* t */  "-strided              Use strided loop scheduling algorithm instead of block slicing",
/*   */  "-tr=<attribute>       Interface to tracer function",
/* C */  "-usage                Produce this list",
/* h */  "-w<num>               Set number of worker processes to <num>",
/* a */  "-z                    Do not print the program's output.",
/* n */   NULL };
/* g */  static void PrintUsageTable(argv,idx)
/* e */  char *argv[]; int idx;
/*   */  {
/* M */    char **p,*q,*r,buf[128],lbuf[128],c;
/* a */    int len,cnt=0;
/* c */    if (argv[idx+1]) {
/* h */       while(argv[++idx]) {
/* i */         len=0;q=argv[idx];r=buf;
/* n */         while(*q) {len++; c= *q++; if (isupper(c)) c=tolower(c);*r++=c;};
/* e */         *r='\0';
/*   */        for (p=Usage;*p;p++) {
/* G */          q= *p;r=lbuf;
/* e */          while(*q) {c= *q++; if (isupper(c)) c=tolower(c); *r++=c;};
/* n */           *r='\0';
/* e */          for(q=lbuf;*q;q++) if (strncmp(q,buf,len)==0) { (void)puts(*p); cnt++; break; }
/* r */        }
/* a */      }
/* t */      if (!cnt) puts("No matches");
/* e */    } else {
/* d */      for (p=Usage;*p;p++) (void)puts(*p);
/*   */    }
/* D */    exit(1);
/* o */  }
