#ifndef WORLD_INCLUDED
#define WORLD_INCLUDED
/* ------------------------------------------------------------ */

#include "sisalInfo.h"
#include "IFX.h"

#define WHITE_CHARS      '\0'

#define MAX_HASH	(11)
#define MAX_NHASH	(11)


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
/* ------------------------------------------------------------ */
#endif

extern FILE *infoptr;	/* info output file */

/* $Log$
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

