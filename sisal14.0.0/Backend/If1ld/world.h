/* $Log$
 * Revision 1.9  1994/04/15  15:50:21  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.8  1993/03/23  22:09:58  miller
 * no change
 *
 * Revision 1.7  1994/03/11  23:09:24  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.6  1993/01/14  22:23:47  miller
 * Fixed up some pragma stuff in ifxstuff and world.
 *
 * Revision 1.5  1993/01/07  00:37:07  miller
 * Make changes for LINT.
 * */

#include "sisalInfo.h"
#include "../Library/IFX.h"

/* ------------------------------------------------------------ */
extern int   pmodule;		/* PROGRAM MODULE? */
extern int   smodule;		/* SISAL MODULE? */
extern int   forF;		/* PROGRAM MODULE TYPES */
extern int   forC;
extern int   monolith;		/* MONOLITHIC PROGRAM MODULE? */

/* ------------------------------------------------------------ */
/* if1smash.c */
extern void	LoadSmashTypes();

/* if1names.c */
extern void	AddToImportList();
extern void	AddToNameList();
extern void	CheckForUnresolvedNames();
