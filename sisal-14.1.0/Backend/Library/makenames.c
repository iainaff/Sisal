/**************************************************************************/
/* FILE   **************        makenames.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

char    *Compound[IF_COMPOUND_LAST-IF_COMPOUND_FIRST+1];
char    *Graphs[IF_GRAPH_LAST-IF_GRAPH_FIRST+1];
char    *Simple[IF_SIMPLE_LAST-IF_SIMPLE_FIRST+1];
char    *Macros[IF_SIMPLE_LAST-IF_SIMPLE_FIRST+1];
char    *Other[1 + IF_OTHER_LAST-IF_OTHER_FIRST+1];
int      OtherOp[1 + IF_OTHER_LAST-IF_OTHER_FIRST+1];

#define NewCopy(s) ((char*)(strcpy((char*)(malloc(strlen(s)+1)),s)))

int
main(int argc, char** argv)
{
  char          line[1024], name[100], Mac[100], *p;
  int           OpCode, MaxCompound = -1, MaxSimple = -1, MaxGraph = -1;
  int           OtherCount = 0, i, j, LowATcode = 9999, HighATcode = -1;

  int           SimpleCount = 0;
  int           CompoundCount = 0;
  int           GraphCount = 0;
  int           AtCount = 0;
  int           NodeType = IF_NODE_COMPOUND;
  int           Testcount;

  /* ------------------------------------------------------------ */
  /* Initialize tables                                            */
  /* ------------------------------------------------------------ */
  for ( i = 0; i <= IF_SIMPLE_LAST-IF_SIMPLE_FIRST; i++ ) {
    Simple[i] = NULL;
  }
  for ( i = 0; i <= IF_SIMPLE_LAST-IF_SIMPLE_FIRST; i++ ) {
    Macros[i] = NULL;
  }
  for ( i = 0; i <= IF_COMPOUND_LAST-IF_COMPOUND_FIRST; i++ ) {
    Compound[i] = NULL;
  }
  for ( i = 0; i <= IF_GRAPH_LAST-IF_GRAPH_FIRST; i++ ) {
    Graphs[i] = NULL;
  }
  for ( i = 0; i <= 1 + IF_OTHER_LAST-IF_OTHER_FIRST; i++ ) {
    Other[i] = NULL;
  }
  for ( i = 0; i <= 1 + IF_OTHER_LAST-IF_OTHER_FIRST; i++ ) {
    OtherOp[i] = 0;
  }

  /* ------------------------------------------------------------ */
  /* Skip lines up to the start of definitions...                 */
  /* ------------------------------------------------------------ */
  while(fgets(line,sizeof(line),stdin)) {
    if ( strncmp(line, "#define IF_NODE_FIRST", 21) == 0 ) break;
  }

  /* ------------------------------------------------------------ */
  /* Now, decode each line in turn...                             */
  /* ------------------------------------------------------------ */
  while(fgets(line,sizeof(line),stdin)) {
    if ( line[0]=='#' && strncmp(line, "#define IF", 10) == 0 ) {
        sscanf(line+7,"%s %d",name,&OpCode);

        /* ------------------------------------------------------------ */
        /* Make sure it looks like an opcode                            */
        /* Order is COMPOUND, SIMPLE, GRAPH, OTHER.                     */
        /* ------------------------------------------------------------ */
        if ( name[2] < 'A' || name[2] > 'Z' ) {
            if ( strncmp(name, "IF_SIMPLE_FIRST", 15) == 0 )
                NodeType = IF_NODE_SIMPLE;
            else if ( strncmp(name, "IF_GRAPH_FIRST", 14) == 0 ) 
                NodeType = IF_NODE_GRAPH;
            else if ( strncmp(name, "IF_GRAPH_LAST", 13) == 0 ) 
                NodeType = IF_NODE_OTHER;
            else if ( strncmp(name, "IF_NODE_LAST", 12) == 0 ) 
                goto Done;
            continue;
        }

        /* ------------------------------------------------------------ */
        /* Put in the name table as appropriate                         */
        /* ------------------------------------------------------------ */

        switch (NodeType) {
        case IF_NODE_COMPOUND:
          CompoundCount++;

          if ( OpCode > MaxCompound ) MaxCompound = OpCode;
          Compound[OpCode] = NewCopy(name);
          break;

        case IF_NODE_SIMPLE:
          SimpleCount++;
          if ( strcmp(name,"IFAAddHAT") == 0 ) LowATcode = OpCode;
          if ( strcmp(name,"IFDiv2") == 0 ) HighATcode = OpCode;
          
          if ( OpCode > MaxSimple ) MaxSimple = OpCode;
          Simple[OpCode-IF_SIMPLE_FIRST] = NewCopy(name);
          /* If this has a defined macro (a = name in a comment), use */
          /* it as the macro name.  Otherwise, use the IF name */
          for ( p=line+10; ; ) {
            if ( *p=='\0' ) {
                strcpy(Mac, name+2);
                break;
            }
            if ( *p++=='/' && 
                 *p++=='*' && 
                 *p++==' ' && 
                 *p++=='=' && 
                 *p++==' ') {
                 sscanf(p, "%s", Mac);
                 break;
            } 
          }
          Macros[OpCode-IF_SIMPLE_FIRST] = NewCopy(Mac);
          break;

        case IF_NODE_GRAPH:
          GraphCount++;

          if ( OpCode > MaxGraph ) MaxGraph = OpCode;
          Graphs[OpCode-IF_GRAPH_FIRST] = NewCopy(name);
          break;

        case IF_NODE_OTHER:
          Other[OtherCount]     = NewCopy(name);
          OtherOp[OtherCount]   = OpCode;
          OtherCount++;
          break;

        default:
          fprintf(stderr,"WARNING:  missing node category\n");
        }
    }
  }

 Done:
  /* ------------------------------------------------------------ */
  /* Build the AtCode count                                       */
  /* ------------------------------------------------------------ */
  for(i=LowATcode; i <= HighATcode; i++) {
    if ( Simple[i-IF_SIMPLE_FIRST] ) AtCount++;
  }


  puts("/* This code is mechanically produced.  Make changes in IFX.h */");
  puts("/* and ``make nametabl.c'' to reproduce. */\n");
  puts("#include \"world.h\"");
  puts("static char Undefined[] = \"***NoDefinition\";\n");

  /* ------------------------------------------------------------ */
  printf("static char\t*cnames[%d] = {\n",MaxCompound+1);
  for(i=0;i<=MaxCompound;i++) {
    printf("/* %4d */ %c%s%c%c\n",
           i,
           (Compound[i])?('"'):(' '),
           (Compound[i])?(Compound[i]+2):("Undefined"),
           (Compound[i])?('"'):(' '),
           (i==MaxCompound)?(' '):(',')
           );
  }
  puts("};\n");

  /* ------------------------------------------------------------ */
  printf("static char\t*snames[%d] = {\n",MaxSimple-IF_SIMPLE_FIRST+1);
  for(i=IF_SIMPLE_FIRST;i<=MaxSimple;i++) {
    printf("/* %4d */ %c%s%c%c\n",
           i,
           (Simple[i-IF_SIMPLE_FIRST])?('"'):(' '),
           (Simple[i-IF_SIMPLE_FIRST])?(Simple[i-IF_SIMPLE_FIRST]+2):("Undefined"),
           (Simple[i-IF_SIMPLE_FIRST])?('"'):(' '),
           (i==MaxSimple)?(' '):(',')
           );
  }
  puts("};\n");

  /* ------------------------------------------------------------ */
  printf("static char\t*macros[%d] = {\n",MaxSimple-IF_SIMPLE_FIRST+1);
  for(i=IF_SIMPLE_FIRST;i<=MaxSimple;i++) {
    printf("/* %4d */ %c%s%c%c\n",
           i,
           (Macros[i-IF_SIMPLE_FIRST])?('"'):(' '),
           (Macros[i-IF_SIMPLE_FIRST])?(Macros[i-IF_SIMPLE_FIRST]):("Undefined"),
           (Macros[i-IF_SIMPLE_FIRST])?('"'):(' '),
           (i==MaxSimple)?(' '):(',')
           );
  }
  puts("};\n");

  /* ------------------------------------------------------------ */
  printf("static char\t*gnames[%d] = {\n",MaxGraph-IF_GRAPH_FIRST+1);
  for(i=IF_GRAPH_FIRST;i<=MaxGraph;i++) {
    printf("/* %4d */ %c%s%c%c\n",
           i,
           (Graphs[i-IF_GRAPH_FIRST])?('"'):(' '),
           (Graphs[i-IF_GRAPH_FIRST])?(Graphs[i-IF_GRAPH_FIRST]+2):("Undefined"),
           (Graphs[i-IF_GRAPH_FIRST])?('"'):(' '),
           (i==MaxGraph)?(' '):(',')
           );
  }
  puts("};\n");

  /* ------------------------------------------------------------ */
  /* ------------------------------------------------------------ */
  /* ------------------------------------------------------------ */
  for(j=0; j<2; j++) {
    if ( j == 0 ) {
      puts(  "char *GetNodeName(n)");
      puts(  "  PNODE n;");
      puts(  "{");
      puts(  "  int opcode = n->type;");
    } else {
      puts(  "char *OpCodeNameOf(opcode)");
      puts(  "  int opcode;");
      puts(  "{");
    }
    printf("  if ( opcode>=IF_COMPOUND_FIRST && opcode<=IF_COMPOUND_LAST )\n");
    printf("    return cnames[opcode-IF_COMPOUND_FIRST];\n");
    printf("  if ( opcode>=IF_SIMPLE_FIRST && opcode<=IF_SIMPLE_LAST )\n");
    printf("    return snames[opcode-IF_SIMPLE_FIRST];\n");
    printf("  if ( opcode>=IF_GRAPH_FIRST && opcode<=IF_GRAPH_LAST )\n");
    printf("    return gnames[opcode-IF_GRAPH_FIRST];\n");
    for(i=0; i<OtherCount; i++) {
      printf("  if ( opcode == %d ) return \"%s\";\n",OtherOp[i],Other[i]+2);
    }
    puts(  "  return Undefined;");
    puts(  "}");
  }

  puts("char *GetMacro(n)");
  puts("PNODE n;");
  puts("{");
  puts(  "  int opcode = n->type;");
  printf("  if ( opcode>=IF_SIMPLE_FIRST && opcode<=IF_SIMPLE_LAST )\n");
  printf("    return macros[opcode-IF_SIMPLE_FIRST];\n");
  puts("  FPRINTF( stderr, \"HELP: n->type = %d n->if1line %d\\n\", n->type, n->if1line );");
  puts("  Error1( \"GetMacro: REQUEST FOR GRAPH OR COMPOUND MACRO NAME\" );");
  puts("  return NULL;");
  puts("}\n");


  puts("char *GraphName(i) int i; { return gnames[i]; }\n");
  puts("char *SimpleName(i) int i; { return snames[i]; }\n");
  puts("char *CompoundName(i) int i; { return cnames[i]; }\n");
  puts("char *AtName(i) int i; { return snames[IFAAddLAT+i-IFAAddH]; }\n");

  /* ------------------------------------------------------------ */
  /* See if we can verify the counts...                           */
  /* ------------------------------------------------------------ */
  while(fgets(line,sizeof(line),stdin)) {

    if ( strncmp(line,"#define IF1SimpleNodes",22) == 0 ) {
      sscanf(line,"#define IF1SimpleNodes (%d)",&Testcount);
      if ( Testcount != SimpleCount ) {
        fprintf(stderr,"WARNING:  Simple count mismatch!!!\n");
        fprintf(stderr,"#define IF1SimpleNodes\t\t(%d)\n",SimpleCount);
      }
    }

    if ( strncmp(line,"#define IF1CompoundNodes",24) == 0 ) {
      sscanf(line,"#define IF1CompoundNodes (%d)",&Testcount);
      if ( Testcount != CompoundCount ) {
        fprintf(stderr,"WARNING:  Compound count mismatch!!!\n");
        fprintf(stderr,"#define IF1CompoundNodes\t(%d)\n",CompoundCount);
      }
    }

    if ( strncmp(line,"#define IF1GraphNodes",21) == 0 ) {
      sscanf(line,"#define IF1GraphNodes (%d)",&Testcount);
      if ( Testcount != GraphCount ) {
        fprintf(stderr,"WARNING:  Graph count mismatch!!!\n");
        fprintf(stderr,"#define IF1GraphNodes\t\t(%d)\n",GraphCount);
      }
    }


    if ( strncmp(line,"#define IF2AtNodes",18) == 0 ) {
      sscanf(line,"#define IF2AtNodes (%d)",&Testcount);
      if ( Testcount != AtCount ) {
        fprintf(stderr,"WARNING:  At count mismatch!!!\n");
        fprintf(stderr,"#define IF2AtNodes\t\t(%d)\n",AtCount);
      }
    }

  }
  return 0;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:46  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:37  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.7  1994/06/06  23:31:19  denton
 * Added low and high markers for node categories: COMPOUND, SIMPLE, GRAPH, OTHER.
 *
 * Revision 1.6  1994/04/15  15:51:57  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.5  1994/02/17  18:09:36  denton
 * Fixed compiler warning
 *
 * Revision 1.4  1994/02/15  23:19:28  miller
 * Modifications to allow new kinds of IF1 types (sets, complex, etc...)
 * The makenames utility was modified to check count information.
 *
 * Revision 1.3  1993/06/02  23:46:04  miller
 * A cast of strcpy (in the NewCopy macro) was causing problems for
 * GCC.
 *
 * Revision 1.2  1993/02/24  18:34:49  miller
 * Fixed a problem with the generations of AtNode names.
 *
 * Revision 1.1  1993/01/21  23:30:56  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
