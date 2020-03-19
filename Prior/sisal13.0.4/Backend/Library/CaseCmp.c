#include "world.h"


/**************************************************************************/
/* GLOBAL **************           CaseCmp         ************************/
/**************************************************************************/
/* PURPOSE:  Case insenstivie compare of two strings			  */
/**************************************************************************/
int CaseCmp(s1, s2)
register char *s1, *s2;
{
    register unsigned char c1, c2;

    while (*s1) {
        if (*s1++ != *s2++) {
            if (!s2[-1])
                return 1;
            c1 = toupper(s1[-1]);
            c2 = toupper(s2[-1]);
            if (c1 != c2)
                return c1 - c2;
        }
    }
    if (*s2)
        return -1;
    return 0;
}

/* $Log$
 * Revision 1.1  1993/11/12  20:06:07  miller
 * Added case insensitive comparison routine
 * */
