/**************************************************************************/
/* FILE   **************          report.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:57:51  patmiller
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
 * Revision 1.8  1994/04/18  19:23:47  denton
 * Removed remaining gcc warnings.
 *
 * Revision 1.7  1993/03/23  22:43:54  miller
 * date problem
 *
 * Revision 1.6  1994/03/16  19:45:08  miller
 * Forgot argument to macro.
 *
 * Revision 1.5  1994/03/11  18:32:37  miller
 * Timestamp control on loop report.
 *
 * Revision 1.4  1993/01/21  21:42:40  miller
 * Changed a varname from line to sourceline so that it doesn't shadow
 * a global var.
 *
 * Revision 1.3  1993/01/15  22:15:48  miller
 * Added capability to read reports to the old ability to write them.
 * Changed the loop count field to the ID field.  Files are now chosen
 * and open as  options rather than just writing to stderr.  Added an
 * UpdatedLoopPragmas function that updates concurrency or vector pragmas.
 *
 * Revision 1.2  1993/01/07  00:39:26  miller
 * Make changes for LINT and combined files.
 *
 * Revision 1.1  1992/12/10  22:59:14  miller
 * Makefile, world.h changed to reflect new IFX.h
 * Modified the partitioner to allow a loop report (incomplete).
 */
/**************************************************************************/

#include "world.h"

/* ------------------------------------------------------------ */
/* A structure to hold important partition parameters           */
/* ------------------------------------------------------------ */
typedef struct {
  int           ID;             /* Compound ID for loop */

  unsigned      HasS:1;         /* Field active marks */
  unsigned      HasP:1;
  unsigned      HasV:1;
  unsigned      HasMS:1;
  unsigned      HasST:1;
  unsigned      HasLS:1;
  unsigned      HasCS:1;

  unsigned      S : 1;          /* Marks and fields */
  unsigned      P : 1;
  unsigned      V : 1;
  char          *MS;
  char          ST;
  char          *LS;
  double        CS;
} LoopReportType;

static LoopReportType *LoopReport;
static int            RecordCount = 0;
static int            LoopCount; /* Used to count number of elig. loops */

/**************************************************************************/
/* LOCAL  **************        CheckPragma        ************************/
/**************************************************************************/
/* PURPOSE:  See if the comment is a pragma (active comment).             */
/**************************************************************************/
static void CheckPragma(s)
     char   *s;
{
  /* ------------------------------------------------------------ */
  /* All comments start with '#$<char>'                           */
  /* ------------------------------------------------------------ */
  if ( s[1] != '$' || s[2] == '\0' || s[3] == '\0' ) return;

  /* ------------------------------------------------------------ */
  /* Execute the appropriate action                               */
  /* ------------------------------------------------------------ */
  switch ( s[2] ) {
   case 'T':
    if ( IsStamp('T') &&
        strcmp(s+4,GetStampString('T')) != 0
        ) {
      FPRINTF(stderr,"***WARNING: This loop report was prepared for a file\n");
      FPRINTF(stderr,"            with a different time stamp.\n%svs\n%s\n",
              s+4,GetStampString('T')+1);
    }
    break;

   default:                     /* Ignore non-pragmas */
    ;
  }
}

/**************************************************************************/
/* MACRO  **************        GetQuoteBuf        ************************/
/**************************************************************************/
/* PURPOSE:  Used in ReadReport to extract a quoted buffer.               */
/**************************************************************************/
#define GetQuoteBuf(Buf) { \
  if ( *p != '"' ) goto BadLine; \
  for(q=Buf,p++; *p && *p != '"'; p++) *(q++) = *p; \
  if ( *p != '"' ) goto BadLine; \
}

/**************************************************************************/
/* GLOBAL **************        ReadReport         ************************/
/**************************************************************************/
/* PURPOSE:  Read in the updates to apply from the report file            */
/**************************************************************************/

void ReadReport()
{
  char          sourceline[1024],*p,Buf[1024],*q;
  int           lineno = 0;
  int           i;
  LoopReportType *LR;
  int           len;

  if ( !REPORT_IN ) return;

  /* ------------------------------------------------------------ */
  /* Skip to first non-comment line                               */
  /* ------------------------------------------------------------ */
  while ( (lineno++,fgets(sourceline,sizeof(sourceline),REPORT_IN)) ) {
    if ( sourceline[0] != '#' ) break;
    CheckPragma(sourceline);
  }
  if ( feof(REPORT_IN) ) return;

  /* ------------------------------------------------------------ */
  /* Get the count of report records                              */
  /* ------------------------------------------------------------ */
  RecordCount = 0;
  (void)sscanf(sourceline,"%d",&RecordCount);
  if ( RecordCount <= 0 ) return;

  /* ------------------------------------------------------------ */
  /* Allocate space for the records and initialize                */
  /* ------------------------------------------------------------ */
  LoopReport = (LoopReportType*)MyAlloc((int)(sizeof(LoopReportType)*RecordCount));
  for(i=0; i<RecordCount; i++) {
    LR = LoopReport+i;
    LR->ID      = 0;
    LR->HasS    = FALSE;
    LR->HasP    = FALSE;
    LR->HasV    = FALSE;
    LR->HasMS   = FALSE;
    LR->HasST   = FALSE;
    LR->HasLS   = FALSE;
    LR->HasCS   = FALSE;
  }

  /* ------------------------------------------------------------ */
  /* Read in RecordCount loop records                                     */
  /* ------------------------------------------------------------ */
  i = 0;
  while ( (lineno++,fgets(sourceline,sizeof(sourceline),REPORT_IN)) ) {
    if ( sourceline[0] == '#' ) { CheckPragma(sourceline); continue; }

    /* ------------------------------------------------------------ */
    /* Check to make sure we have allocated sufficient records */
    p = sourceline;
    if ( i >= RecordCount ) goto BadLine;

    /* ------------------------------------------------------------ */
    /* Get the ID */
    for(; *p; p++) if (*p == ':') break;
    if ( !p ) goto BadLine;
    p++;
    LR = LoopReport+i;
    LR->ID = atoi(sourceline);
    if ( LR->ID <= 0 ) goto BadLine;

    /* ------------------------------------------------------------ */
    /* While there's still stuff, look for fields */
    while(*p && *p != ';') {
      while(*p && isspace(*p)) p++; /* Skip blanks */

      len = strlen(p);
      if ( len >= 3 && strncmp(p,"S=",2) == 0 ) {
        LR->HasS = TRUE; LR->S = atoi(p+2); p += 3;

      } else if ( len >= 3 && strncmp(p,"P=",2) == 0 ) {
        LR->HasP = TRUE; LR->P = atoi(p+2); p += 3;

      } else if ( len >= 3 && strncmp(p,"V=",2) == 0 ) {
        LR->HasV = TRUE; LR->V = atoi(p+2); p += 3;

      } else if ( len >= 4 && strncmp(p,"MS=",3) == 0 ) {
        LR->HasMS = TRUE;
        p += 3;
        GetQuoteBuf(Buf);
        LR->MS = CopyString(Buf);
        p++;

      } else if ( len >= 4 && strncmp(p,"ST=",3) == 0 ) {
        LR->HasST = TRUE; LR->ST = p[3]; p += 4;

      } else if ( len >= 5 && strncmp(p,"LS=",3) == 0 ) {
        LR->HasLS = TRUE;
        p += 3;
        GetQuoteBuf(Buf);
        LR->LS = CopyString(Buf);
        p++;

      } else if ( len >= 6 && strncmp(p,"CS=",2) == 0 ) {
        LR->HasCS = TRUE;
        p += 3;
        LR->CS = atof(p);

        while(*p && (isdigit(*p)
                     || (*p=='e')
                     || (*p=='E')
                     || (*p=='+')
                     || (*p=='-')
                     || (*p=='.')
                     )) p++;
      } else {
        goto BadLine;
      }
    }

    /* ------------------------------------------------------------ */
    i++;                        /* Move to next record */
  }

  (void)fclose(REPORT_IN);
  return;

  /* ------------------------------------------------------------ */
  /* ------------------------------------------------------------ */
  /* ------------------------------------------------------------ */
 BadLine:
  FPRINTF(stderr,"%s: Loop Report line %d -->%s\n",__FILE__,lineno,p);
  Error1("Syntax error");
}

/**************************************************************************/
/* GLOBAL **************      WriteReport          ************************/
/**************************************************************************/
/* PURPOSE: Write out the loop report file                                */
/**************************************************************************/
static void WriteReport(f)
     PNODE      f;
{
  PNODE         n,g;

  for ( n = f; n != NULL; n = n->nsucc ) {

    switch ( n->type ) {
     case IFForall:
      FPRINTF( REPORT_OUT,"%3d:\tS=%d\tP=%d\tV=%d",
              n->ID,n->smark,n->pmark,n->vmark
              );
      if ( n->ccost > 0.0)FPRINTF( REPORT_OUT,"\tCS=%g",        n->ccost);
      if ( n->MinSlice )  FPRINTF( REPORT_OUT,"\tMS=\"%s\"",    n->MinSlice);
      if ( n->Style )     FPRINTF( REPORT_OUT,"\tST=%c",        n->Style);
      if ( n->LoopSlice ) FPRINTF( REPORT_OUT,"\tLS=\"%s\"",    n->LoopSlice);
      (void)fputs(";\n",REPORT_OUT);

      for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) WriteReport( g );
      break;

     case IFLoopA:
     case IFLoopB:
     case IFSelect:
     case IFTagCase:
      for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) WriteReport( g );
      break;

     default:
      break;
    }
  }
}

/**************************************************************************/
/* LOCAL  **************        LoopCounter        ************************/
/**************************************************************************/
/* PURPOSE: Count the number of forall loops to report on                 */
/**************************************************************************/
static void LoopCounter(f)
     PNODE      f;
{
  PNODE         n,g;

  for ( n = f; n != NULL; n = n->nsucc ) {

    switch ( n->type ) {
     case IFForall:
      LoopCount++;
      for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) LoopCounter( g );
      break;

     case IFLoopA:
     case IFLoopB:
     case IFSelect:
     case IFTagCase:
      for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) LoopCounter( g );
      break;

     default:
      break;
    }
  }
}

/**************************************************************************/
/* GLOBAL **************     UpdatedLoopPragmas    ************************/
/**************************************************************************/
/* PURPOSE:  Search the Record database for the indicated compound.  If   */
/*           found, then update the record.  It returns TRUE if it        */
/*           it changes any fields.                                       */
/**************************************************************************/

int UpdatedLoopPragmas(n,RMask)
     PNODE      n;
     unsigned   RMask;          /* Bit mask selector for fields */
{
  int           ID = n->ID;     /* ID to search for */
  int           i;
  int           Updated;        /* TRUE iff some update made */
  LoopReportType *LR;           /* Points to good report record */

  /* ------------------------------------------------------------ */
  /* First, find a record in the LoopReport */
  for(i=0; i < RecordCount; i++) if ( LoopReport[i].ID == ID ) break;
  if ( i >= RecordCount ) return FALSE; /* If not there, no update */

  /* ------------------------------------------------------------ */
  /* Now, look for fields to update */
  LR = LoopReport+i;
  Updated = FALSE;

  if ( LR->HasS && (RMask & R_SMARK) ) {
    n->smark = LR->S;
    Updated = TRUE;
  }

  if ( LR->HasP && (RMask & R_PMARK) ) {
    n->pmark = LR->P;
    Updated = TRUE;
  }

  if ( LR->HasV && (RMask & R_VMARK) ) {
    n->vmark = LR->V;
    Updated = TRUE;
  }

  if ( LR->HasMS && (RMask & R_MinSlice) ) {
    n->MinSlice = LR->MS;
    Updated = TRUE;
  }

  if ( LR->HasST && (RMask & R_Style) ) {
    n->Style = LR->ST;
    Updated = TRUE;
  }

  if ( LR->HasLS && (RMask & R_LoopSlice) ) {
    n->LoopSlice = LR->LS;
    Updated = TRUE;
  }

  if ( LR->HasCS && (RMask & R_Cost) ) {
    n->ccost = LR->CS;
    Updated = TRUE;
  }

  return Updated;
}

/**************************************************************************/
/* GLOBAL **************      PartitionReport      ************************/
/**************************************************************************/
/* PURPOSE: To write out a report on loop concurrency                     */
/**************************************************************************/
void PartitionReport()
{
  register PNODE f;

  /* ------------------------------------------------------------ */
  /* Make sure we know where to write it */  
  if ( !REPORT_OUT ) return;

  /* ------------------------------------------------------------ */
  /* Count the loops                                              */
  /* ------------------------------------------------------------ */
  LoopCount = 0;
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) LoopCounter(f);

  /* ------------------------------------------------------------ */
  /* Write up a report containing the important pragmas for loops */
  /* ------------------------------------------------------------ */
  if ( IsStamp('T') ) FPRINTF( REPORT_OUT, "#$T %s\n",GetStampString('T'));
  FPRINTF( REPORT_OUT, "# Loop Report\n" );
  FPRINTF( REPORT_OUT, "%d\n",LoopCount);
  FPRINTF( REPORT_OUT, "#\t S\t P\t V\t  MIN\n" );
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) WriteReport( f );
  (void)fclose( REPORT_OUT );
}
