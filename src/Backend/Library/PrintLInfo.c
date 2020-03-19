/**************************************************************************/
/* FILE   **************        PrintLInfo.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

void PrintLInfo( lvl, plvl, c, loop )
     int  lvl;
     int  plvl;
     char  *c;
     PNODE  loop;
{
  register int i;
  char     lbuf[1024];          /* Assemble the loop info string */
  char     *lb;                 /* Pointer within lbuf */
  char     fname[100];          /* Want to restructure the file name */
  char     *dotp;               /* Pointer to .sis */
  char     cbuf[100];           /* Holds the cost field */
  /* char          *source;*/           /* Source line from file.sis */
  static int TrailingNL = FALSE; /* True iff we just double spaced */

  /* ------------------------------------------------------------ */
  /* Output the source line (if it requested and available)       */
  /* ------------------------------------------------------------ */
  if ( RequestInfo(I_Info4,info) && (IsForall(loop) || IsLoop(loop)) &&
        (loop->smark || loop->vmark || loop->reason1 || loop->reason2) ) {
 /*   if ( (IsForall(loop) || IsLoop(loop)) && (loop->F_GEN->G_NODES) ) {
      source = GetSourceLine(loop->F_GEN->G_NODES);
    } else {
      source = GetSourceLine(loop);
    }
    if ( source ) {
      if ( !TrailingNL ) (void)fputc('\n',infoptr);
      FPRINTF(infoptr," # %s\n",source);
    } */
    TrailingNL = FALSE;

  /* ------------------------------------------------------------ */
  /* Output the Compound ID #                                     */
  /* ------------------------------------------------------------ */
  if ( IsForall(loop) ) {
    SPRINTF( lbuf, "%4d:%d   ",loop->ID,plvl);
  } else {
    SPRINTF( lbuf, "%4d:     ",loop->ID);
  }
  lb=lbuf+6;                    /* Skip past stuff */

  /* ------------------------------------------------------------ */
  /* Remove the .sis extension from the file name                 */
  /* ------------------------------------------------------------ */
  (void)strcpy(fname,loop->file);
  dotp = strrchr(fname,'.');
  if ( dotp ) *dotp = '\0';

  /* ------------------------------------------------------------ */
  /* Output the loop record                                       */
  /* ------------------------------------------------------------ */
  for ( i=0; i < lvl*2; i++ ) *(lb++) = ' ';
  if ( IsForall(loop) ) {
    SPRINTF( lb, "%s%s%s%s%s%s [%s,%d]",
            (loop->ThinCopy)? "THIN-COPY " : "",
            (loop->smark)? "CONCURRENT " : "",
            (loop->vmark)? "VECTOR " : "",
            (loop->pmark)? "TASK " : "",
            (loop->ThinCopy || loop->smark || loop->vmark || loop->pmark)
            ? "":"SEQUENTIAL ",
            c,
            fname, loop->line );
  } else {
    SPRINTF( lb,"%s%s [%s,%d]",(loop->pmark)? "TASK ":"",c,fname,loop->line);
  }

  /* ------------------------------------------------------------ */
  /* Add in the cost and Minimum Slice count if known             */
  /* ------------------------------------------------------------ */
  if ( loop->ccost > 0.0 ) {
    if ( loop->MinSlice ) {
      SPRINTF( cbuf, (loop->ccost < 1e6)?"[cost=%g/%s]":"[cost=%6.1e/%s]",
              loop->ccost,loop->MinSlice );
    } else {
      SPRINTF( cbuf, (loop->ccost < 1e6)?"[cost=%g]":"[cost=%6.1e]",
              loop->ccost );
    }
    (void)strcat(lb,cbuf);
  }

  /* ------------------------------------------------------------ */
  /* Pad to at least column 64 and add the function name          */
  /* ------------------------------------------------------------ */
  lb = strchr(lb,'\0');
  do { *(lb++) = ' ';} while (lb-lbuf < 64);
  (void)strcpy(lb,loop->funct);

  /* ------------------------------------------------------------ */
  /* Truncate at 80 chars if too long and print                   */
  /* ------------------------------------------------------------ */
  if ( (int)strlen(lbuf) > 80 ) strcpy(lbuf+79,"$");
  FPRINTF(infoptr,"%s\n",lbuf);

  /* ------------------------------------------------------------ */
  /* If we know why we aren't vectorized, report it               */
  /* ------------------------------------------------------------ */
  if ( loop->reason1 ) {
    for ( i=0; i < lvl*2+6; i++ ) (void)fputc(' ',infoptr);
    FPRINTF(infoptr,"!Vector: %s\n",loop->reason1);
  }

  /* ------------------------------------------------------------ */
  /* If we know why we aren't concurrent, report it               */
  /* ------------------------------------------------------------ */
  if ( loop->reason2 ) {
    for ( i=0; i < lvl*2+6; i++ ) (void)fputc(' ',infoptr);
    FPRINTF(infoptr,"!Concurrent: %s\n",loop->reason2);
  }

  /* ------------------------------------------------------------ */
  /* Need a trailing newline to clean up some problems            */
  /* ------------------------------------------------------------ */
  if (loop->reason1 || loop->reason2) {
    (void)fputc('\n',infoptr);
    TrailingNL = TRUE;
  }
  }  
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:31  patmiller
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
 * Revision 1.10  1994/06/16  21:31:59  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.9  1994/04/15  15:51:53  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.8  1994/04/05  23:02:27  denton
 * Cast for ANSI C.
 *
 * Revision 1.7  1994/04/01  00:02:50  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.6  1994/03/31  01:46:33  denton
 * Replace anachronistic BSD functions, index->strchr, rindex->strrchr
 *
 * Revision 1.5  1993/03/23  22:53:36  miller
 * date problem
 *
 * Revision 1.4  1994/03/18  20:11:11  miller
 * Had indentation wrong on vector/conc. reason.
 *
 * Revision 1.3  1994/03/18  18:33:18  miller
 * Added support to display the PARALLELISM level rather than the simple
 * loop nesting depth.  I also changed the format for For Initial loops.
 * They are not marked SEQUENTIAL so they are easier to read.  They only
 * have marks if they are TASK loops (stream producers).  I had a bug
 * where I used a %d format for MinSlice instead of %s.  I missed it when
 * doing the MinSlice conversion.
 *
 * Revision 1.2  1994/03/11  18:22:07  miller
 * Support for the Info mask
 *
 * Revision 1.1  1993/01/21  23:30:02  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
