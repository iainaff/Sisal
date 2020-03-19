#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************          world.h          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "sisalInfo.h"
#include "IFX.h"

#define WHITE_CHARS      '\0'

#define MAX_HASH        (11)
#define MAX_NHASH       (11)


/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define NextToken        token = fgetc( input )

#define IsSpace(x)       ( ((x) == ' ') || ((x) == '\t') )
#define IsEoln(x)        ( (x) == '\n' )
#define IsWhite(x)       ( IsSpace(x) || IsEoln(x) )
#define IsDigit(x)       ( ((x) >= '0') && ((x) <= '9') )
#define IsEqual(x)       ( (x) == '=' )

#define EatLine          while ( !IsEoln( token ) ) NextToken; NextToken; line++
#define EatSpaces        while ( IsSpace( token ) ) NextToken

#define SliceThreshold      mcosts[0]
#define Iterations          mcosts[1]
#define IntrinsicCost       mcosts[2]
#define DefaultFunctionCost mcosts[3]
#define RefCntIncrementCost mcosts[4]
#define RefCntDecrementCost mcosts[5]
#define DopeVectorCopyCost  mcosts[6]
#define RecordCopyCost      mcosts[7]
#define ArrayCopyCost       Iterations
#define DeallocCost         mcosts[8]
#define IntegerCost         mcosts[9]
#define RealCost            mcosts[10]
#define DoubleCost          mcosts[11]

extern FILE *infoptr;   /* info output file */

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
 * Revision 1.8  1994/06/16  21:32:08  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.7  1994/04/15  15:51:58  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.6  1994/04/01  00:02:58  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.5  1993/06/14  20:44:05  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.4  1993/03/23  22:53:17  miller
 * Time include file problem for timestamps.
 *
 * Revision 1.3  1994/03/11  23:09:55  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.2  1994/03/11  18:23:19  miller
 * Added time stamp to all output IF1 and IF2
 *
 * Revision 1.1  1993/01/21  23:30:57  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 * Revision 1.4  1993/01/14  22:32:13  miller
 * Had originally forgotten to include IsABRecord.c in release.  Made
 * fixes to CopyString for lint.  Completely changed the way ReadPragmas
 * works.  Now, instead of a switch for each lead letter and if-then-else
 * for each sub letter, it just hashes to a single switch.  This made
 * it much shorter and more legible.  The WriteLoopMap and PrintLInfo
 * changes were made to make the loop reporting consistent across utilities
 * and to report the information more clearly.
 *
 * Revision 1.3  1993/01/08  17:29:51  miller
 * I don't remember what I needed to touch up.
 *
 * Revision 1.2  1993/01/07  00:12:19  miller
 * Added $Log$
 * Added Revision 1.2  2001/01/02 09:16:46  patmiller
 * Added Now ANSI compliant, but still a pthread problem
 * Added
 * Added Revision 1.1.1.1  2000/12/31 17:58:37  patmiller
 * Added Well, here is the first set of big changes in the distribution
 * Added in 5 years!  Right now, I did a lot of work on configuration/
 * Added setup (now all autoconf), breaking out the machine dependent
 * Added #ifdef's (with a central acconfig.h driven config file), changed
 * Added the installation directories to be more gnu style /usr/local
 * Added (putting data in the /share/sisal14 dir for instance), and
 * Added reduced the footprint in the top level /usr/local/xxx hierarchy.
 * Added
 * Added I also wrote a new compiler tool (sisalc) to replace osc.  I
 * Added found that the old logic was too convoluted.  This does NOT
 * Added replace the full functionality, but then again, it doesn't have
 * Added 300 options on it either.
 * Added
 * Added Big change is making the code more portably correct.  It now
 * Added compiles under gcc -ansi -Wall mostly.  Some functions are
 * Added not prototyped yet.
 * Added
 * Added Next up: Full prototypes (little) checking out the old FLI (medium)
 * Added and a new Frontend for simpler extension and a new FLI (with clean
 * Added C, C++, F77, and Python! support).
 * Added
 * Added Pat
 * Added
 * Added
 * Revision 1.8  1994/06/16  21:32:08  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.7  1994/04/15  15:51:58  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.6  1994/04/01  00:02:58  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.5  1993/06/14  20:44:05  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.4  1993/03/23  22:53:17  miller
 * Time include file problem for timestamps.
 *
 * Revision 1.3  1994/03/11  23:09:55  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.2  1994/03/11  18:23:19  miller
 * Added time stamp to all output IF1 and IF2
 *
 * Revision 1.1  1993/01/21  23:30:57  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 * Revision 1.4  1993/01/14  22:32:13  miller
 * Had originally forgotten to include IsABRecord.c in release.  Made
 * fixes to CopyString for lint.  Completely changed the way ReadPragmas
 * works.  Now, instead of a switch for each lead letter and if-then-else
 * for each sub letter, it just hashes to a single switch.  This made
 * it much shorter and more legible.  The WriteLoopMap and PrintLInfo
 * changes were made to make the loop reporting consistent across utilities
 * and to report the information more clearly.
 *
 * Revision 1.3  1993/01/08  17:29:51  miller
 * I don't remember what I needed to touch up.
 * header to all .c files.  Cleaned up so that everything passes
 * LINT.  The added files represent duplicates not found before (mostly
 * from the if[12]timer files).
 * */

#endif
