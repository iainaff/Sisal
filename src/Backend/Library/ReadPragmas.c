/**************************************************************************/
/* FILE   **************       ReadPragmas.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        ReadPragmas        ************************/
/**************************************************************************/
/* PURPOSE: READ NAME, FILE, LINE, AND MARK PRAGMAS: %na=name, %sf=file,  */
/*          %sl=line, %fn=funct, %mk=mark, %sr=VALUE, %pm=VALUE,          */
/*          %cm=VALUE, %pl=VALUE, AND %cc=COST. OTHERS ARE SILENTLY       */
/*          IGNORED.                                                      */
/*                                                                        */
/*          THE NAME (name), FILE (file), AND FUNCTION (funct) MUST       */
/*          MUST CONTAIN AT LEAST ONE NON-WHITE CHARACTER; THE LINE       */
/*          (line) MUST BE AN UNSIGNED NUMBER; AND THE MARK MUST BE A     */
/*          SINGLE NON-WHITE CHARACTER. WHEN DONE, token WILL             */
/*          CONTAIN THE EOLN CHARACTER.  COST IS A DOUBLE.                */
/**************************************************************************/

#define DecodePragma(fld,converter,err) \
{                                       \
  if ( !HaveArgument ) ReadError(err);  \
  name = ReadString(WHITE_CHARS);       \
  if ( *name == '\0' ) ReadError(err);  \
  pragmas.fld = converter(name);        \
  (void)(free(name));                   \
}
#define NamePragma(fld,err)             \
{                                       \
  if ( !HaveArgument ) ReadError(err);  \
  name = ReadString(WHITE_CHARS);       \
  if ( *name == '\0' ) ReadError(err);  \
  pragmas.fld = name;                   \
}

#define HashP(First,Second)      ((First-'a')*26+(Second))

void ReadPragmas()
{
  register char *name;
  register int   ThisLine;
  register int   oldmaxint;
  int           FirstLetter;    /* First char in pragma name */
  int           SecondLetter;   /* Second char in pragma name */
  int           HaveArgument;   /* TRUE iff pragma of form %xx=arg */

  PragInitPragmas( &pragmas );

  for (;;) {
    EatSpaces;

    if ( IsEoln( token ) ) 
      break;

    if ( token != '%' ) {
      NextToken;
      continue;
    }

    NextToken;

    /* ------------------------------------------------------------ */
    /* Get the letters that form the pragma.  Also check if it has an */
    /* argument */
    FirstLetter = token;        NextToken;
    SecondLetter = token;       NextToken;
    if ( IsEqual(token) ) {
      NextToken;
      HaveArgument = TRUE;
    } else {
      HaveArgument = FALSE;
    }

    /* ------------------------------------------------------------ */
    /* Choose the appropriate decoder for this combination */
    switch ( HashP(FirstLetter,SecondLetter) ) {
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* ESTIMATED COST                                               */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('c','c'):
      DecodePragma(ccost,atof,"ILLEGAL cc PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* CONSUMER MODIFY (Ref count)                                  */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('c','m'):
      DecodePragma(cm,atoi,"ILLEGAL cm PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* FUNCTION NAME                                                */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('f','n'):
      NamePragma(funct,"ILLEGAL fn PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* MARK                                                         */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('m','k'):
      if ( IsWhite( token ) ) ReadError( "ILLEGAL mk PRAGMA" );

      switch ( token ) {
       case 'V':
        if ( !AllowVMarks ) break;
        pragmas.vmark = TRUE;   /* fall through */
       case 's': case 'c': case 'f': case 'd': case 'i': case 'e':
        pragmas.mark = token;
        break;

       case 'I':
        pragmas.imark = TRUE;
        break;

       case 'p':
        pragmas.Pmark = TRUE;
        break;

       case 'P':
        pragmas.pmark = TRUE;
        break;

       case 'R':
        pragmas.rmark1 = RMARK;
        break;

       case 'r':
        pragmas.rmark1 = rMARK;
        break;

       case 'O':
        pragmas.omark1 = TRUE;
        break;

       case 'D':
        pragmas.dmark = TRUE;
        break;

       case 'L':
        pragmas.lmark = TRUE;
        break;

       case 'E':
        pragmas.emark = TRUE;
        break;

       case 'F':
        pragmas.fmark = TRUE;
        break;

       case 'C':
        pragmas.cmark = TRUE;
        break;

       case '@':
        pragmas.umark = TRUE;
        break;

       case 'N':
        pragmas.nmark = TRUE;
        break;

       case 'B':
        pragmas.bmark = TRUE;
        recursive = TRUE;
        break;

       case 'W':
        pragmas.wmark = TRUE;
        break;

       case 'S':
        pragmas.smark = TRUE;
        break;

       case 'Y':
        pragmas.Fmark = TRUE;
        break;

       default:
        ReadError( "ILLEGAL mk PRAGMA MARK" );
      }
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* NAME                                                         */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('n','a'):
      NamePragma(name,"ILLEGAL na PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* PL REFERENCE COUNT                                           */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('p','l'):
      DecodePragma(pl,atoi,"ILLEGAL pl PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* PRODUCER MODIFY                                              */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('p','m'):
      DecodePragma(pm,atoi,"ILLEGAL pm PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* SOURCE FILE                                                  */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('s','f'):
      NamePragma(file,"ILLEGAL sf PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* SOURCE LINE                                                  */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('s','l'):
      oldmaxint = maxint;       /* 2/92 WATCH FOR sl=9999999 */
      ThisLine = ReadInteger();
      maxint = oldmaxint;       /* 2/92 WATCH FOR sl=9999999 */

      pragmas.line = ThisLine;
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* SET REFERENCE COUNT                                          */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('s','r'):
      DecodePragma(sr,atoi,"ILLEGAL sr PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* TEMPORARY NAME                                               */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('t','n'):
      pragmas.reason1 = ReadLiteral(FALSE);
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* EDGE TYPE NAME                                               */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('e','t'):
      NamePragma(reason2,"ILLEGAL et PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Compound ID number                                           */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('I','D'):
      DecodePragma(ID,atoi,"ILLEGAL ID PRAGMA");
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Loop Slice formula                                           */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('L','S'):
      if ( !HaveArgument || IsWhite(token) ) ReadError("ILLEGAL LS PRAGMA");
      pragmas.LoopSlice = ReadLiteral(FALSE);
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Lazy Node/Edge                                               */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('L','Z'):
      if ( HaveArgument ) ReadError("ILLEGAL LZ PRAGMA");
      pragmas.lazy = TRUE;
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Mininum Slice Width                                          */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('M','S'):
      if ( !HaveArgument || IsWhite(token) ) ReadError("ILLEGAL MS PRAGMA");
      pragmas.MinSlice = ReadLiteral(FALSE);
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Loop Parallelism Style                                       */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('S','T'):
      if ( !HaveArgument || IsWhite(token) ) ReadError("ILLEGAL ST PRAGMA");
      pragmas.Style = token;
      NextToken;
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Thin Copy mark for loops                                     */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     case HashP('T','C'):
      if ( HaveArgument ) ReadError("ILLEGAL TC PRAGMA");
      pragmas.ThinCopy = TRUE;
      break;

      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /* Silently ignore others                                       */
      /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     default:
      ;
    }
  }
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:33  patmiller
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
 * Revision 1.9  1994/07/15  18:35:20  denton
 * Make tn a "" pragma
 *
 * Revision 1.8  1994/07/15  16:42:43  denton
 * Added tn and et for IF3.
 *
 * Revision 1.7  1994/04/01  00:02:52  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.6  1994/03/24  19:39:48  denton
 * Distributed DSA, non-coherent cache, non-static shared memory.
 *
 * Revision 1.5  1994/02/17  17:29:16  denton
 * Reduction 'd'
 *
 * Revision 1.4  1993/06/14  20:44:04  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.3  1993/03/23  22:50:17  miller
 * VMark problem
 *
 * Revision 1.2  1994/03/11  18:22:44  miller
 * MinSlice is now a char*
 *
 * Revision 1.1  1993/01/21  23:30:16  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
