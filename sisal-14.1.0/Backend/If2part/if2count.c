/**************************************************************************/
/* FILE   **************         if2count.c        ************************/
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
 * Revision 1.1.1.1  2000/12/31 17:57:47  patmiller
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
 * Revision 1.9  1994/06/16  21:31:09  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.8  1993/03/23  22:43:25  miller
 * date problem
 *
 * Revision 1.7  1994/03/18  18:38:02  miller
 * Now listing parallelism level in loop map.
 *
 * Revision 1.6  1994/03/11  18:32:19  miller
 * MinSlice as a char*
 *
 * Revision 1.5  1993/01/14  22:29:05  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 *
 * Revision 1.4  1993/01/07  00:39:20  miller
 * Make changes for LINT and combined files.
 *
 * Revision 1.3  1992/12/10  22:59:08  miller
 * Makefile, world.h changed to reflect new IFX.h
 * Modified the partitioner to allow a loop report (incomplete).
 *
 * Revision 1.2  1992/11/06  19:32:57  miller
 * 921007: Modifiy loop reporting for partitioner (Seq. Loop problem)
 *
 * Revision 12.8  1992/11/05  23:30:21  miller
 * 921007: Modifiy loop reporting for partitioner (Seq. Loop problem)
 *
 * Revision 12.8  1992/10/30  23:28:49  miller
 * 921007: Modifiy loop reporting for partitioner (Seq. Loop problem)
 *
 * Revision 12.7  1992/10/21  18:09:35  miller
 * Initial RCS Version by Cann
 */
/**************************************************************************/

#include "world.h"


static int fnodes;                                /* COUNT OF Forall NODES */
static int slices;          /* COUNT OF Forall NODES MARKED FOR SLICING    */
static int vecs;         /* COUNT OF Forall NODES MARKED FOR VECTORIZATION */
static int slvecs;       /* COUNT OF Forall NODES MARKED FOR VECTORIZATION */
                         /* AND SLICING                                    */

static int strcps;          /* COUNT OF STREAM CONSUMER AND PRODUCER TASKS */
static int syncs;           /* COUNT OF SYNCRONIZATION OPERATIONS          */


/**************************************************************************/
/* LOCAL  **************       WriteTaskInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE INFORMATION FOR TASK n TO stderr. Msg IDENTIFIES THE    */
/*          TASK'S TYPE. IF msg IS NULL, n'S SYMBOLIC NAME IS WRITTEN.    */
/**************************************************************************/

static void WriteTaskInfo( msg, n, indent )
char *msg;
PNODE n;
int   indent;
{
    register    int     i;
    static      int     LoopCount = 0;

    /* ------------------------------------------------------------ */
    FPRINTF( infoptr, "%d\t", ++LoopCount );
    for ( i = 0; i < indent; i++ ) 
        FPRINTF( infoptr, " " );

    FPRINTF( infoptr, "%d: %s", indent, msg );

    FPRINTF( infoptr, " (%s) ", cfunct->G_NAME );
    if ( n->MinSlice ) {
      FPRINTF( infoptr, "[cost=%g,%s]: ", n->ccost,n->MinSlice );
    } else {
      FPRINTF( infoptr, "[cost=%g]: ", n->ccost );
    }
    FPRINTF( infoptr, "(%s,%s,%d)\n", n->file, n->funct, n->line);

    if ( n->reason2) {
      FPRINTF( infoptr, "\t");
      for ( i = 0; i < indent+3; i++ ) FPRINTF( infoptr, " " );
      FPRINTF( infoptr, "*** %s\n",n->reason2);
    }
}


/**************************************************************************/
/* LOCAL  **************         WriteMap          ************************/
/**************************************************************************/
/* PURPOSE: WRITE THE PARTITION MAP OF GRAPH g TO stderr AND INCREMENT    */
/*          THE APPROPRIATE COUNTERS.                                     */
/**************************************************************************/

static void WriteMap( g, indent, plvl )
PNODE g;
int   indent;
int   plvl;
{
    register PNODE n;

    for ( n = g; n != NULL; n = n->nsucc ) {
        if ( n->wmark )
            syncs++;

        switch ( n->type ) {
            case IFForall:
                fnodes++;
                if ( n->pmark ) goto StreamProducer;
                PrintLInfo(indent,plvl,"Forall",n);

                if ( n->smark ) {
                  if ( n->vmark ) {
                    slvecs++;
                  } else {
                    slices++;
                  }
                } else if ( n->vmark ) {
                  vecs++;
                }

                for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
                  WriteMap( g, indent + 1, plvl+1 );

                break;

            case IFLoopA:
            case IFLoopB:
               StreamProducer:
                if ( n->pmark ) {
                  strcps++;

                  WriteTaskInfo( "Stream Task", n, indent );

                } else {
                  PrintLInfo(indent,plvl,"For initial",n);
                }

                for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
                  WriteMap( g, indent + 1, plvl );
                break;


            case IFSelect:
            case IFTagCase:
                for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
                    WriteMap( g, indent, plvl );

                break;

            default:
                break;
            }
        }
}


/**************************************************************************/
/* GLOBAL **************       PartIf2Count        ************************/
/**************************************************************************/
/* PURPOSE: PRINT A PARTITION MAP OF ALL FUNCTIONS TO stderr.             */
/**************************************************************************/

void PartIf2Count()
{
    register PNODE f;

    fnodes = vecs = slices = strcps = syncs = 0;

    FPRINTF( infoptr, "\n **** PARTITION AND VECTORIZATION MAP (procs=%d)\n\n",
                     procs );

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
        FPRINTF( infoptr, " FUNCTION %s [cost=%g] [apl=%d] [pbsy=%d]\n",
                         f->G_NAME, f->ccost, f->level, f->pbusy   );

        WriteMap( cfunct = f, 1, 1 );
        }

    if ( RequestInfo(I_Info4,info)  ) {
      FPRINTF( infoptr, "\n **** OCCURRENCE COUNTS\n\n" );
      FPRINTF( infoptr,   " Forall Nodes:                       %d\n", fnodes  );
      FPRINTF( infoptr,   " Sliced Forall Nodes:                %d\n", slices  );
      FPRINTF( infoptr,   " Vectorized Forall Nodes:            %d\n", vecs    );
      FPRINTF( infoptr,   " Sliced and Vectorized Forall Nodes: %d\n", slvecs  );
      FPRINTF( infoptr,   " Stream Consumer/Producer Tasks:     %d\n", strcps  );
      FPRINTF( infoptr,   " Syncronization Operations:          %d\n", syncs   );
    }
}
