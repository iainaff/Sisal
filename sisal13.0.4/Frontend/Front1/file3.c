#include "frontend.h"

int CANN_GlueMode = 0;

/* CANN ********** */
static PBBLOCK bpool;
static int     bpidx = MAX_BBLOCKS+100;

static PSBLOCK spool;
static int     spidx = MAX_SBLOCKS+100;

PSBLOCK MySBlockAlloc()
{
  if ( spidx >= MAX_SBLOCKS ) {
    spool = (PSBLOCK) Malloc( sizeof(SBLOCK)*MAX_SBLOCKS );
    spidx = 0;
    }
  return( &(spool[spidx++]) );
}

PBBLOCK MyBBlockAlloc()
{
  if ( bpidx >= MAX_BBLOCKS ) {
    bpool = (PBBLOCK) Malloc( sizeof(BBLOCK)*MAX_BBLOCKS );
    bpidx = 0;
    }
  return( &(bpool[bpidx++]) );
}
/* ***** CANN ***** */

/* Include global variables here */
/*#TITLE  IF1INIT VARIABLES         IFNAug83   Standard IF1 Variables*/
Static printable nodename[ifmaxnode + 1];
Static printable errvname[ifmaxerror + 1];
Static printable typename[ifmaxtype + 1];
Static printable basname[ifmaxbasic + 1];
Static printable graphname[19];
Static ndclasstype nodeclass[ifmaxnode + 1];   /* dlz - 8/87 */
Static FILE *source, *diag;
Static infilelist infile;
Static parrec *outfile;
/* these vars are needed by module using the graph */
Static stentry *typetable[entrymax];
/* Static */ int tthwm;   /* symbol table high water mark */
Static int univmodulecnt, univnodecnt, univedgecnt;
Static linkrec *funclist;
Static node *firstfunction, *module;
Static stryng stamp['[' - 'A'];
Static long stampset[9];
Static semanticrec *topsemstk;   /* Top of Semantic Stack */
Static namelistrec *definelist;
/* hold the list of function names
                                     listed in the  stmt that have
                                     not been declared yet.  Should be
                                     empty at the end of the module.  */
Static int boostfac;
/* Used to help determine which binary
                                     operation has higher precedence when
                                     () are involved.*/
Static long binaryops;
/* Set used to distiguish binary
                                     operators from others*/
Static long arithtypes;   /* holds TT labels for int,real,doub */
Static long algebtypes;   /* holds TT labels for int,real,doub,bool */
Static long streamorarray;
/* holds TT code for stream and
                                                                       array types*/
Static namelistrec *includelist;
/* holds stack of names of files that have
                                   been included.  List is checked to avoid
                                   recursive includes*/
Static boolean typedefsfinished;
/* Flag to say type def sections is done,
                                   used to know whether to add each piece
                                   of a type into the table or just
                                   smash it in when it is complete*/
/*------------------- SYMBOL TABLE VARS ----------------*/
Static treestackobj *currentlevel;   /* pointer to current Lex Level */
/*------------------- DEBUGGER VARS -------------------*/
Static long breakpointset[maxsemanticnumber / 32 + 2];
    /* Set of semantic routines to break on */
Static int currentstep, stepcount;
Static boolean semtrace;
/* Print out names of semantic routines
                                     as they are called it set*/
/*-------------------- COMMAND LINE PARAMS ------------*/
Static parrec *xrefparam;
/*--------------------- XREF --------------------------*/
Static xrefrec idunsorted[1000];
Static int idsorted[1000];
Static int idxrefmax;
Static inputbuffer *linebuf, *oldline;
Static unchar tokenstart;
Static int linenumber, tokenlineno;
Static boolean endfile;
Static int parsestack[maxstack + 1];
Static Char stringspace[maxstring + 1];
Static boolean epsprod[maxprod + 1];
Static int stringptr;
Static short stackptr;
Static short oldtoken;
Static boolean echoed;
Static ptabrec *table[maxsym + 3];
Static stryng symtab[maxsym + 3];
Static stringrec symkluge[maxsym + 3];
Static int prodspace[maxprodspace + 1];
/* all the productions. (strings of symbols) */
Static prodrec productions[maxprod + 1];   /* pointers into prodspace*/
Static short prodspceptr;
Static int numprods, numsymbols;
Static stryng inputtoken;
Static int toconsume, column;
/* !! variables for error correction */
Static int myinfinity, numterms;
Static rabrec *readaheadbuf;   /*head of buffer of inserted tokens*/
Static FILE *ptableout, *etableout;
/* index : intfile; */
Static FILE *inputfile;   /* file to be parsed */
Static boolean peeking;
Static int errorcount;
Static int deltriedhisto[maxhisto + 1];
    /* histograms of deletions and insertions */
Static int delmadehisto[maxhisto + 1], inshisto[maxhisto + 1];
Static int tokinserted, tokdeleted;
Static char insertspace[maxinsertspace + 1];
Static int deletecosts[maxterm + 1];
Static insertstringrec costtable[maxsym + 3];
Static pretabarray prefixtable;
Static int prefixindex[maxterm + 1];   /*sks*/
Static char curerrsym;   /* current terminal symbol resident in prefixtable*/
Static boolean errortables;
    /* whether error correction tables were created */
Static int insertptr, savinsertptr, numdeltried;
    /* number of deletions considered */
Static FILE *reservedwords;
/*---------- Current State of the editor ----------*/
Static boolean withininputmode, quittingtime;   /* time to leave the editor */
Static int tokenposition;   /* character position within a token */
/*--------- variables for prettyprinting-------------*/
Static short tempptr;
Static boolean readingbinaryfile;   /* true only when reading a binary file */
Static unchar debugflags[32];
/*-------- variables for maintaining stringspace ---------*/
Static extstringspace *firststring;   /* pointer to the first extent */
Static extstringspace *curextent;   /* pointer to current Extent */
Static int curbase;
/* index of the first element
                                             of CurExtent */
/*--------- variables for the scanner ---------*/
Static short token;
Static int inttokenval;
Static double realtokenval;
Static int tokenstrlength, ordzero;
Static boolean eolnflag;
Static Char globalchar;
Static short savetoken;
Static Char newcommand;
/* digits          : set of char; */
/* idcharset       : set of char; */
/* resword         : array [ ResWordIndex ] of StringRec; */
/* following added by sks 83/1/31 */
Static int singlechar['\177' - ' '];   /* token numbers */
Static int reuseindex;
Static Char reusestring[maxbackup + 1];   /* pos 0 not used */
Static int statehistory[maxbackup + 1];
Static long badcharset[9];
/*------- Super Record, where most of the files are ----------*/
Static tablerec p;
Static FILE *listfile;
Static boolean listing, xref;
Static int rea;
Static Char debugchar;
Static boolean debugcg, debug, cortrace, firsttime, done, commentflag,
	       includeflag;
Static includestackrec *topincludestk;
Static stryng curinputfile, newfname;
Static int newlinenum;
Static parrec *listingparam, *debugparam, *pass1param;
Static int semerrcnt;
Static parrec *listfparam;
Static stryng directoryname, defaultdirectoryname, filename;
Static parrec *directoryparam, *paramlist;
Static boolean passerr;
Static stryng listfilename, stampstr, versionstr, versionnum;


/* Include all desired functions and procedures here */
/*#TITLE  STRUTL  ROUTINES        Jan83   String Handling Routines.*/
/* macro function IsDigit( C: char ): boolean */
/* macro function IsHexOnly( C: char ): boolean */
/* macro function IsLowerCase( C: char ): boolean */
/* macro function IsUpperCase( C: char ): boolean */
/* macro procedure MakeLowerCase( var C: char ) */
/* macro procedure MakeUpperCase( var C: char ) */
/* macro function StringLength( S: Stryng ): integer */
/* macro function IsEmptyString( S: Stryng ): boolean */
extern graph *newgraphptr PV();


Static Char stringchar(s, n)
stryng *s;
int n;
{
  if (n > s->len)
    return ' ';
  else
    return (s->str[n - 1]);
}


Static Void clearstring(s, first, last)
stryng *s;
char first, last;
{
  char i;

  for (i = first - 1; i < last; i++)
    s->str[i] = ' ';
  s->len = first - 1;
}


Static Void clearstringtoend(s)
stryng *s;
{
  /* rky Oct87 */
  char i;

  for (i = s->len; i < maxstringchars; i++)
    s->str[i] = ' ';
}


/* macro procedure InitString( S: Stryng )  */
Static Void insertchar(s, c, p)
stryng *s;
Char c;
char p;
{
  int i;

  if (s->len < p - 1)
    s->len = p - 1;
  if (s->len != maxstringchars)
    s->len++;
  for (i = s->len; i > p; i--)
    s->str[i - 1] = s->str[i - 2];
  s->str[p - 1] = c;
}


Static Void concatchar(s, c)
stryng *s;
Char c;
{
  if (s->len < maxstringchars) {
    s->len++;
    s->str[s->len - 1] = c;
  }
}


Static Void concatnchars(s, c, n)
stryng *s;
Char c;
int n;
{
  int i;

  for (i = 1; i <= n; i++)
    concatchar(s, c);
}


/* macro procedure CharString( S: Stryng; C: Char ) */
Static Void stripspaces(s)
stryng *s;
{
  boolean spaces;

  spaces = (s->len > 0);
  while (spaces) {
    if (s->str[s->len - 1] != ' ') {
      spaces = false;
      break;
    }
    s->len--;
    if (s->len == 0)
      spaces = false;
  }
}


Static Void string10(s, s10)
stryng *s;
Char *s10;
{
  int i;

  for (i = 0; i <= 9; i++)
    s->str[i] = s10[i];
  clearstring(s, 11, maxstringchars);
  stripspaces(s);
}


Static Void string20(s, s20)
stryng *s;
Char *s20;
{
  int i;

  for (i = 0; i <= 19; i++)
    s->str[i] = s20[i];
  clearstring(s, 21, maxstringchars);
  stripspaces(s);
}


Static Void stringn(s, sn, count)
stryng *s;
Char *sn;
int count;
{
  /* Name:                   StringN
     Purpose:                Build a Stryng of a specified number of characters
                             from the given character string.  Trailing blanks
                             are not deleted.
     Method:                 This is a slight generalization of the String10
                             routine of STRUTL which is needed for debug messages.
                             The STRUTL package is used to do the work.
     Calling Form:           StringN(string, 'message string', 14 )
     Author:                 J. Engle, 3/13/84
     Modified:
     Input Arguments:        sN     : the character string to put into s
                             Count  : the number of characters (including trailing
                                        blanks) of sN to use
     Output Arguments:       s      : the resulting Stryng
     Input/Output Arguments: (none)
     Packages Used:          STRUTL.ROUTINES
     Notes:
       */
  int realcount;   /*refinement of the argument Count*/
  int i;   /*loop index*/

  realcount = count;
  if (count > maxstringchars)
    realcount = maxstringchars;
  for (i = 0; i < realcount; i++)
    s->str[i] = sn[i];
  if (realcount < maxstringchars)
    clearstring(s, realcount + 1, maxstringchars);
  s->len = realcount;
}


Static Void insertstring(s, t, p)
stryng *s, *t;
char p;
{
  int i, j, FORLIM;

  if (s->len < p - 1)
    s->len = p - 1;
  if (s->len + t->len <= maxstringchars)
    s->len += t->len;
  else
    s->len = maxstringchars;
  FORLIM = p + t->len - 1;
  for (i = s->len - 1; i >= FORLIM; i--)
    s->str[i] = s->str[i - t->len];
  if (s->len < p + t->len)
    j = s->len;
  else
    j = p + t->len - 1;
  for (i = p; i <= j; i++)
    s->str[i - 1] = t->str[i - p];
}


/* macro procedure ConcatString( var S: Stryng; T: Stryng ) */
Static Void readstring(fil, s)
FILE *fil;
stryng *s;
{
  Char ch;

  s->len = 0;
  while (!P_eoln(fil) && s->len < maxstringchars) {
    s->len++;
    ch = getc(fil);
    if (ch == '\n')
      ch = ' ';
    s->str[s->len - 1] = ch;
  }
  if (s->len < maxstringchars)
    clearstring(s, s->len + 1, maxstringchars);
  fscanf(fil, "%*[^\n]");
  getc(fil);
}


Static boolean readstringok(fil, s)
FILE *fil;
stryng *s;
{
  /* Same as ReadString, but returns FALSE if line had more than MaxStringChars. */
  boolean Result;
  Char ch;

  s->len = 0;
  while (!P_eoln(fil) && s->len < maxstringchars) {
    s->len++;
    ch = getc(fil);
    if (ch == '\n')
      ch = ' ';
    s->str[s->len - 1] = ch;
  }
  if (s->len < maxstringchars)
    clearstring(s, s->len + 1, maxstringchars);
  Result = P_eoln(fil);
  fscanf(fil, "%*[^\n]");
  getc(fil);
  return Result;
}


Local boolean isletter(c)
Char c;
{
  if (islower(c))
    return true;
  else if (isupper(c))
    return true;
  else if (c == '_' || c == '-' || c == '.')
    return true;
  else
    return false;
}  /* IsLetter */


Static Void readidentifier(fil, s)
FILE *fil;
stryng *s;
{
  /* reads a string of the form [Letter | Digit]* */
  Char ch;
  boolean finished;

  finished = false;
  s->len = 0;
  while (!finished) {
    if (P_eoln(fil)) {
      finished = true;
      break;
    }
    ch = P_peek(fil);
/* p2c: mono.bin.noblank.p, line 1639:
 * Note: File parameter fil needs its associated buffers [318] */
    if (!(isletter(ch) || isdigit(ch))) {
      finished = true;
      break;
    }
/* p2c: mono.bin.noblank.p, line 1643:
 * Note: File parameter fil needs its associated buffers [318] */
    getc(fil);
    s->len++;
    s->str[s->len - 1] = ch;   /* with */
  }
}  /* ReadIdentifier */


Static Void readifstring(fil, s)
FILE *fil;
stryng *s;
{
  /* Should be used to read a string of characters between
     double quote symbols.  Sets the Length field and pads
     with blank spaces. */
  char i;

  s->len = 0;
  while ((!P_eoln(fil)) & (P_peek(fil) != '"'))
    getc(fil);
/* p2c: mono.bin.noblank.p, line 1660:
 * Note: File parameter fil needs its associated buffers [318] */
/* p2c: mono.bin.noblank.p, line 1661:
 * Note: File parameter fil needs its associated buffers [318] */
  if (!P_eoln(fil))
    getc(fil);
/* p2c: mono.bin.noblank.p, line 1663:
 * Note: File parameter fil needs its associated buffers [318] */
  while ((!P_eoln(fil)) & (P_peek(fil) != '"')) {
/* p2c: mono.bin.noblank.p, line 1664:
 * Note: File parameter fil needs its associated buffers [318] */
    s->len++;
    s->str[s->len - 1] = getc(fil);
    if (s->str[s->len - 1] == '\n')
      s->str[s->len - 1] = ' ';
  }
  if (!P_eoln(fil))
    getc(fil);
/* p2c: mono.bin.noblank.p, line 1669:
 * Note: File parameter fil needs its associated buffers [318] */
  /* now pad with blanks */
  for (i = s->len; i < maxstringchars; i++)
    s->str[i] = ' ';
}  /* ReadIFString */


Static Void readliteralstring(fil, s)
FILE *fil;
stryng *s;
{
  /* Preconditions: (1) string is less than MaxStringChar characters
                   (2) string begins and ends in double quotes
    PostConditions: outside quotes are stripped and string is read
                    into variable S.
    Example:  ""apple"" is read as: "apple"
              "'apple'" is read as: 'apple'
              "apple"   is read as:  apple
    Note:  Backslash is a quoting character
    Example:  ""\""" is read as: "\""
  */
  Char ch;
  int i;
  boolean finished;

  /* read the first two characters, the first must be a double quote */
/* p2c: mono.bin.noblank.p, line 1693:
 * Note: File parameter fil needs its associated buffers [318] */
  getc(fil);
  ch = getc(fil);
  if (ch == '\n')
    ch = ' ';
  s->len = 1;
  s->str[s->len - 1] = ch;
  finished = false;
  while (!finished) {
    ch = getc(fil);
    if (ch == '\n')
      ch = ' ';
    if (ch == '"') {
      finished = true;
      break;
    }
    if (ch != '\\') {
      s->len++;
      s->str[s->len - 1] = ch;
      continue;
    }
    ch = getc(fil);
    if (ch == '\n')
      ch = ' ';
    s->str[s->len] = '\\';
    s->len += 2;
    s->str[s->len - 1] = ch;
  }
  /* check to see if string ends in two double quotes, if so,
     one belongs as the last character of the string */
  if (P_peek(fil) == '"') {
    getc(fil);
    s->len++;
    s->str[s->len - 1] = '"';
  }
/* p2c: mono.bin.noblank.p, line 1717:
 * Note: File parameter fil needs its associated buffers [318] */
/* p2c: mono.bin.noblank.p, line 1719:
 * Note: File parameter fil needs its associated buffers [318] */
  for (i = s->len; i < maxstringchars; i++)
    s->str[i] = ' ';
}  /* ReadLiteralString */


Static Void writestring(fil, s)
FILE *fil;
stryng *s;
{
  int i, FORLIM;

  FORLIM = s->len;
  for (i = 0; i < FORLIM; i++)
    putc(s->str[i], fil);
}


Static Void writenstring(fil, s, n)
FILE *fil;
stryng s;
int n;
{
  int i;

  for (i = 1; i <= n; i++) {
    if (i > s.len)
      putc(' ', fil);
    else
      putc(s.str[i - 1], fil);
  }
}


Static Void substring(t, s, first, last)
stryng *t, *s;
char first;
int last;
{
  int i, FORLIM;

  if (last > s->len)
    last = s->len;
  if (first > last)
    first = last + 1;
  else {
    FORLIM = last - first;
    for (i = 0; i <= FORLIM; i++)
      t->str[i] = s->str[i + first - 1];
  }
  clearstring(t, last - first + 2, maxstringchars);
}


Static Void deletestring(s, first, last)
stryng *s;
char first;
int last;
{
  int i, offset, FORLIM;

  if (last > s->len)
    last = s->len;
  if (first > last)
    first = last + 1;
  offset = last - first + 1;
  FORLIM = s->len;
  for (i = last; i < FORLIM; i++)
    s->str[i - offset] = s->str[i];
  clearstring(s, s->len - offset + 1, s->len);
}


Static Char uppercase(c)
Char c;
{
  if (islower(c))
    return z_toupper(c);
  else
    return c;
}


Static Void stringuppercase(s)
stryng *s;
{
  int i, FORLIM;

  FORLIM = s->len;
  for (i = 0; i < FORLIM; i++) {
    if (islower(s->str[i]))
      s->str[i] = z_toupper(s->str[i]);
  }
}


Static Char lowercase(c)
Char c;
{
  if (isupper(c))
    return z_tolower(c);
  else
    return c;
}


Static Void stringlowercase(s)
stryng *s;
{
  int i, FORLIM;

  /*StringLowerCase*/
  FORLIM = s->len;
  for (i = 0; i < FORLIM; i++) {
    if (isupper(s->str[i]))
      s->str[i] = z_tolower(s->str[i]);
  }
}


Static boolean matchstrings(s1, s2, pos)
stryng *s1, *s2;
char pos;
{
  int i, ix;
  boolean res;
  int FORLIM;

  /* Convert to offset in index */
  ix = pos - 1;
  /* Check first string is long enough */
  if (s2->len < s1->len + ix)
    return false;
  else {
    res = true;
    FORLIM = s1->len;
    for (i = 0; i < FORLIM; i++) {
      if (s2->str[i + ix] != s1->str[i])
	res = false;
    }
    return res;
  }
}


Static boolean equalstrings(s1, s2)
stryng *s1, *s2;
{
  if (s1->len == s2->len)
    return (matchstrings(s1, s2, 1));
  else
    return false;
}


Static boolean equivstrings(s1, s2)
stryng *s1, *s2;
{
  /* True iff S1=S2 ignoring case differences.  Doesn't change S1 or S2.
     rky added 25May88.  Gotos are fun! */
  boolean Result;
  int i, diff, caseshift;
  stryng *WITH;

  WITH = s1;   /*with*/
  if (WITH->len != s2->len)
    goto _L2;
  Result = true;
  caseshift = 'A' - 'a';
  i = 0;
_L1:   /* top of loop */
  if (i == WITH->len)
    goto _L3;
  i++;
  diff = s2->str[i - 1] - WITH->str[i - 1];
  if (diff == 0)
    goto _L1;
  if (diff == caseshift) {
    if (islower(WITH->str[i - 1]))
      goto _L1;
    /* else fall through to fail */
  } else if (diff == -caseshift) {
    if (isupper(WITH->str[i - 1]))
      goto _L1;
    /* else fall through to fail */
  }
_L2:
  Result = false;   /*fail*/
_L3:   /*succeed*/
  return Result;

  /* else fall through to fail */
}


Static int findchar(c, s, pos)
Char c;
stryng *s;
char pos;
{
  int Result, i;

  Result = 0;
  for (i = s->len; i >= pos; i--) {
    if (s->str[i - 1] == c)
      Result = i;
  }
  return Result;
}


Static int findstring(s, t, pos)
stryng *s, *t;
char pos;
{
  int chpos;
  boolean match;

  if (s->len == 0)
    return pos;
  else {
    match = false;
    chpos = pos - 1;
    do {
      chpos = findchar(s->str[0], t, chpos + 1);
      if (chpos != 0)
	match = matchstrings(s, t, chpos);
    } while (!(match || chpos == 0));
    return chpos;
  }
}


Static int findlastchar(c, s)
Char c;
stryng *s;
{
  int Result, i, FORLIM;

  Result = 0;
  FORLIM = s->len;
  for (i = 1; i <= FORLIM; i++) {
    if (s->str[i - 1] == c)
      Result = i;
  }
  return Result;
}


Static Void numberstring(s, n, base)
stryng *s;
int n, base;
{
  int sign, ch;

  mymemcpy(s->str, blankstring, sizeof(stryngar));
  s->len = 0;
  if (n < 0)
    sign = -1;
  else
    sign = 1;
  do {
    ch = sign * (n % base);
/* p2c: mono.bin.noblank.p, line 1906:
 * Note: Using % for possibly-negative arguments [317] */
    if (ch < 10)
      ch += '0';
    else
      ch += '7';
    insertchar(s, ch, 1);
    n /= base;
  } while (n != 0);
  if (sign == -1)
    insertchar(s, '-', 1);
}


Static Void integerstring(s, n, width)
stryng *s;
int n, width;
{
  numberstring(s, n, 10);
  while (s->len < width)
    insertchar(s, ' ', 1);
}


Static int charval(ch)
Char ch;
{
  ch = uppercase(ch);
  if (isdigit(ch))
    return (ch - '0');
  else {
    if (ch >= 'A' && ch <= 'F')
      return (ch - 55);
    else
      return 16;
  }
}


Static long stringnumber(s, pos, base)
stryng *s;
int *pos, base;
{
  long maxdivbase, maxmodbase, newval, res;
  boolean neg;

  /* StringNumber */
  neg = false;
  if (base < 2 || base > 16)
    base = 16;
  maxdivbase = maxbigint / base;
  maxmodbase = maxbigint % base;
  res = 0;
  while (*pos < s->len && s->str[*pos - 1] == ' ')
    (*pos)++;
  if (s->str[*pos - 1] == '-' || s->str[*pos - 1] == '+') {
    if (s->str[*pos - 1] == '-')
      neg = true;
    (*pos)++;
  }
  while (*pos < s->len && s->str[*pos - 1] == ' ')
    (*pos)++;
  while ((res >= 0) && (charval(s->str[*pos - 1]) < base)) {
    newval = charval(s->str[*pos - 1]);
    if (res < maxdivbase || res == maxdivbase && newval <= maxmodbase) {
      res = res * base + newval;
      (*pos)++;
    } else
      res = -maxbigint;
  }
  if (neg)
    return (-res);
  else
    return res;
}


/*#TITLE  IF1INIT ROUTINES          IFNAug83   Standard IF1 Routines*/
Static Void initnames()
{
  int i;

  /* InitNames */
  /* Node Identifiers */
  for (i = 0; i <= ifmaxnode; i++)
    mymemcpy(nodename[i], "*************** ", sizeof(printable));
  /* Compound Nodes */
  mymemcpy(nodename[ifnselect], "Select          ", sizeof(printable));
  mymemcpy(nodename[ifnloopb], "LoopB           ", sizeof(printable));
  mymemcpy(nodename[ifnloopa], "LoopA           ", sizeof(printable));
  mymemcpy(nodename[ifnforall], "Forall          ", sizeof(printable));
  mymemcpy(nodename[ifntagcase], "TagCase         ", sizeof(printable));
  mymemcpy(nodename[ifnmodule], "Module          ", sizeof(printable));
  mymemcpy(nodename[ifnifthenelse], "IfThenElse      ", sizeof(printable));
  mymemcpy(nodename[ifniter], "Iter            ", sizeof(printable));
  /* Simple Node */
  mymemcpy(nodename[ifnaaddh], "AAddH           ", sizeof(printable));
  mymemcpy(nodename[ifnaaddl], "AAddL           ", sizeof(printable));
  mymemcpy(nodename[ifnaextract], "AExtract        ", sizeof(printable));
  mymemcpy(nodename[ifnabuild], "ABuild          ", sizeof(printable));
  mymemcpy(nodename[ifnacatenate], "ACatenate       ", sizeof(printable));
  mymemcpy(nodename[ifnaelement], "AElement        ", sizeof(printable));
  mymemcpy(nodename[ifnafill], "AFill           ", sizeof(printable));
  mymemcpy(nodename[ifnagather], "AGather         ", sizeof(printable));
  mymemcpy(nodename[ifnaisempty], "AIsEmpty        ", sizeof(printable));
  mymemcpy(nodename[ifnalimh], "ALimH           ", sizeof(printable));
  mymemcpy(nodename[ifnaliml], "ALimL           ", sizeof(printable));
  mymemcpy(nodename[ifnaremh], "ARemH           ", sizeof(printable));
  mymemcpy(nodename[ifnareml], "ARemL           ", sizeof(printable));
  mymemcpy(nodename[ifnareplace], "AReplace        ", sizeof(printable));
  mymemcpy(nodename[ifnascatter], "AScatter        ", sizeof(printable));
  mymemcpy(nodename[ifnasetl], "ASetL           ", sizeof(printable));
  mymemcpy(nodename[ifnasize], "ASize           ", sizeof(printable));
  mymemcpy(nodename[ifnabs], "Abs             ", sizeof(printable));
/* CANN PEEK */  mymemcpy(nodename[ifnpeek], "Peek            ", sizeof(printable));
  mymemcpy(nodename[ifnbindarguments], "BindArguments   ", sizeof(printable));
  mymemcpy(nodename[ifnbool], "Bool            ", sizeof(printable));
  mymemcpy(nodename[ifncall], "Call            ", sizeof(printable));
  mymemcpy(nodename[ifnchar], "Char            ", sizeof(printable));
  mymemcpy(nodename[ifndiv], "Div             ", sizeof(printable));
  mymemcpy(nodename[ifndouble], "Double          ", sizeof(printable));
  mymemcpy(nodename[ifnequal], "Equal           ", sizeof(printable));
  mymemcpy(nodename[ifnexp], "Exp             ", sizeof(printable));
  mymemcpy(nodename[ifnfirstvalue], "FirstValue      ", sizeof(printable));
  mymemcpy(nodename[ifnfinalvalue], "FinalValue      ", sizeof(printable));
  mymemcpy(nodename[ifnfloor], "Floor           ", sizeof(printable));
  mymemcpy(nodename[ifnint], "Int             ", sizeof(printable));
  mymemcpy(nodename[ifniserror], "IsError         ", sizeof(printable));
  mymemcpy(nodename[ifnless], "Less            ", sizeof(printable));
  mymemcpy(nodename[ifnlessequal], "LessEqual       ", sizeof(printable));
  mymemcpy(nodename[ifnmax], "Max             ", sizeof(printable));
  mymemcpy(nodename[ifnmin], "Min             ", sizeof(printable));
  mymemcpy(nodename[ifnminus], "Minus           ", sizeof(printable));
  mymemcpy(nodename[ifnmod], "Mod             ", sizeof(printable));
  mymemcpy(nodename[ifnneg], "Neg             ", sizeof(printable));
  mymemcpy(nodename[ifnnoop], "NoOp            ", sizeof(printable));
  mymemcpy(nodename[ifnnot], "Not             ", sizeof(printable));
  mymemcpy(nodename[ifnnotequal], "NotEqual        ", sizeof(printable));
  mymemcpy(nodename[ifnplus], "Plus            ", sizeof(printable));
  mymemcpy(nodename[ifnrangegenerate], "RangeGenerate   ", sizeof(printable));
  mymemcpy(nodename[ifnrbuild], "RBuild          ", sizeof(printable));
  mymemcpy(nodename[ifnrelements], "RElements       ", sizeof(printable));
  mymemcpy(nodename[ifnrreplace], "RReplace        ", sizeof(printable));
  mymemcpy(nodename[ifnreduce], "Reduce          ", sizeof(printable));
  mymemcpy(nodename[ifnredleft], "ReduceLeft      ", sizeof(printable));
  mymemcpy(nodename[ifnredright], "ReduceRight     ", sizeof(printable));
  mymemcpy(nodename[ifnredtree], "ReduceTree      ", sizeof(printable));
  mymemcpy(nodename[ifnallbutlastvalue], "AllButLastVal   ", sizeof(printable));
  mymemcpy(nodename[ifnsingle], "Single          ", sizeof(printable));
  mymemcpy(nodename[ifntimes], "Times           ", sizeof(printable));
  mymemcpy(nodename[ifntrunc], "Trunc           ", sizeof(printable));
  mymemcpy(nodename[ifnaprefixsize], "APrefixSize     ", sizeof(printable));
  mymemcpy(nodename[ifnareplacen], "AReplaceN       ", sizeof(printable));
  mymemcpy(nodename[ifnspawn], "Spawn           ", sizeof(printable));
  mymemcpy(nodename[ifnfilter], "Filter          ", sizeof(printable));
  mymemcpy(nodename[ifnashift], "AShift          ", sizeof(printable));
  mymemcpy(nodename[ifnarraytobuf], "ArrayToBuf      ", sizeof(printable));
  mymemcpy(nodename[ifnrecordtobuf], "RecordToBuf     ", sizeof(printable));
  mymemcpy(nodename[ifnshiftbuffer], "ShiftBuffer     ", sizeof(printable));
  mymemcpy(nodename[ifnabufscatter], "ABufScatter     ", sizeof(printable));
  mymemcpy(nodename[ifnabufpartition], "ABufPartition   ", sizeof(printable));
  mymemcpy(nodename[ifnrbufelements], "RBufElements    ", sizeof(printable));
  mymemcpy(nodename[ifnabuildat], "ABuildAt        ", sizeof(printable));
  mymemcpy(nodename[ifnafillat], "AFillAt         ", sizeof(printable));
  mymemcpy(nodename[ifnareplaceat], "AReplaceAt      ", sizeof(printable));
  mymemcpy(nodename[ifnasetlat], "ASetLAt         ", sizeof(printable));
  mymemcpy(nodename[ifnaelementsat], "AElementsAt     ", sizeof(printable));
  mymemcpy(nodename[ifnacatenateat], "ACatenateAt     ", sizeof(printable));
  mymemcpy(nodename[ifnaremlat], "ARemLAt         ", sizeof(printable));
  mymemcpy(nodename[ifnaremhat], "ARemHAt         ", sizeof(printable));
  mymemcpy(nodename[ifnaaddlat], "AAddLAt         ", sizeof(printable));
  mymemcpy(nodename[ifnaaddhat], "AAddHAt         ", sizeof(printable));
  mymemcpy(nodename[ifnaextractat], "AExtractAt      ", sizeof(printable));
  mymemcpy(nodename[ifnrbuildat], "RBuildAt        ", sizeof(printable));
  mymemcpy(nodename[ifnrelementsat], "RElementsAt     ", sizeof(printable));
  mymemcpy(nodename[ifnagatherat], "AGatherAt       ", sizeof(printable));
  mymemcpy(nodename[ifnreduceat], "ReduceAt        ", sizeof(printable));
  mymemcpy(nodename[ifnfinalvalueat], "FinalValueAt    ", sizeof(printable));
  mymemcpy(nodename[ifndefarraybuf], "DefArrayBuf     ", sizeof(printable));
  mymemcpy(nodename[ifndefrecordbuf], "DefRecordBuf    ", sizeof(printable));
  mymemcpy(nodename[ifnmemalloc], "MemAlloc        ", sizeof(printable));
  mymemcpy(nodename[ifncondsetrefcnt], "CondSetRefCnt   ", sizeof(printable));
  /* Graph Names */
  mymemcpy(graphname[(long)ifgfunction], "Function        ", sizeof(printable));
  mymemcpy(graphname[(long)ifgselector], "Selector        ", sizeof(printable));
  mymemcpy(graphname[(long)ifgalternative], "Alternative     ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgvariant], "Variant         ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopainit], "LoopAInit       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopabody], "LoopABody       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopatest], "LoopATest       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopareturns], "LoopAReturns    ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgloopbinit], "LoopBInit       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopbbody], "LoopBBody       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopbtest], "LoopBTest       ", sizeof(printable));
  mymemcpy(graphname[(long)ifgloopbreturns], "LoopBReturns    ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgforallgenerator], "ForallGenerate  ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgforallbody], "ForallBody      ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgforallreturns], "ForallReturns   ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgifpredicate], "IfPredicate     ",
	 sizeof(printable));
  mymemcpy(graphname[(long)ifgiftrue], "IfTrue          ", sizeof(printable));
  mymemcpy(graphname[(long)ifgiffalse], "IfFalse         ", sizeof(printable));
  mymemcpy(graphname[(long)ifgiterbody], "IterBody        ", sizeof(printable));
  /* Error Values */
  mymemcpy(errvname[ifeerror], "Error           ", sizeof(printable));
  mymemcpy(errvname[ifeundef], "Undef           ", sizeof(printable));
  mymemcpy(errvname[ifebroken], "Broken          ", sizeof(printable));
  mymemcpy(errvname[ifemiselt], "MisElt          ", sizeof(printable));
  mymemcpy(errvname[ifeposover], "PosOver         ", sizeof(printable));
  mymemcpy(errvname[ifenegover], "NegOver         ", sizeof(printable));
  mymemcpy(errvname[ifeposunder], "PosUnder        ", sizeof(printable));
  mymemcpy(errvname[ifenegunder], "NegUnder        ", sizeof(printable));
  mymemcpy(errvname[ifezerodivide], "ZeroDivide      ", sizeof(printable));
  mymemcpy(errvname[ifeunknown], "Unknown         ", sizeof(printable));
  mymemcpy(errvname[ifenoerror], "NoError         ", sizeof(printable));
  mymemcpy(errvname[ifenovalue], "NoValue         ", sizeof(printable));
  mymemcpy(errvname[ifmrnovalue], "NoMRValue       ", sizeof(printable));
  /* Type Values */
  mymemcpy(typename[iftbasic], "Basic           ", sizeof(printable));
  mymemcpy(typename[iftfunctiontype], "Function        ", sizeof(printable));
  mymemcpy(typename[iftarray], "Array           ", sizeof(printable));
  mymemcpy(typename[iftstream], "Stream          ", sizeof(printable));
  mymemcpy(typename[iftmultiple], "Multiple        ", sizeof(printable));
  mymemcpy(typename[iftrecord], "Record          ", sizeof(printable));
  mymemcpy(typename[ifttuple], "Tuple           ", sizeof(printable));
  mymemcpy(typename[iftunion], "Union           ", sizeof(printable));
  mymemcpy(typename[iftfield], "Field           ", sizeof(printable));
  mymemcpy(typename[ifttag], "Tag             ", sizeof(printable));
  mymemcpy(typename[iftwild], "Wild            ", sizeof(printable));
  mymemcpy(typename[iftbuffer], "Buffer          ", sizeof(printable));
  /* Base Type Values */
  mymemcpy(basname[ifbboolean], "Boolean         ", sizeof(printable));
  mymemcpy(basname[ifbcharacter], "Character       ", sizeof(printable));
  mymemcpy(basname[ifbinteger], "Integer         ", sizeof(printable));
  mymemcpy(basname[ifbreal], "Real            ", sizeof(printable));
  mymemcpy(basname[ifbdouble], "Double          ", sizeof(printable));
  mymemcpy(basname[ifbnull], "Null            ", sizeof(printable));
  mymemcpy(basname[ifbwild], "Wild            ", sizeof(printable));
  /* Node Classifications */
  for (i = 0; i <= ifmaxnode; i++)
    nodeclass[i] = unknown;
  /* Compound Nodes */
  nodeclass[ifnselect] = controlabstr;
  nodeclass[ifnloopb] = controlabstr;
  nodeclass[ifnloopa] = controlabstr;
  nodeclass[ifnforall] = controlabstr;
  nodeclass[ifntagcase] = controlabstr;
  nodeclass[ifniter] = controlabstr;
  nodeclass[ifnifthenelse] = controlabstr;
  /* Not real sure about these .... */
  nodeclass[ifnmodule] = unclassified;
  nodeclass[ifngraph] = graphnode;
  nodeclass[ifncondsetrefcnt] = unclassified;
  /* Simple Node */
  nodeclass[ifnaaddh] = structurebuild;
  nodeclass[ifnaaddl] = structurebuild;
  nodeclass[ifnaextract] = structureaccess;
  nodeclass[ifnabuild] = structurebuild;
  nodeclass[ifnacatenate] = structurebuild;
  nodeclass[ifnaelement] = structureaccess;
  nodeclass[ifnafill] = structurebuild;
  nodeclass[ifnagather] = structurebuild;
  nodeclass[ifnaisempty] = structureaccess;
  nodeclass[ifnalimh] = structureaccess;
  nodeclass[ifnaliml] = structureaccess;
  nodeclass[ifnaremh] = structureaccess;
  nodeclass[ifnareml] = structureaccess;
  nodeclass[ifnareplace] = structurebuild;
  nodeclass[ifnascatter] = multiplegenr;
  nodeclass[ifnasetl] = indexchange;
  nodeclass[ifnasize] = structureaccess;
  nodeclass[ifnabs] = unaryalgeb;

/* PEEK CANN */ nodeclass[ifnpeek] = unclassified;

  nodeclass[ifnbindarguments] = functioninvoke;
  nodeclass[ifnbool] = unaryalgeb;
  nodeclass[ifncall] = functioninvoke;
  nodeclass[ifnchar] = unaryalgeb;
  nodeclass[ifndiv] = binaryalgeb;
  nodeclass[ifndouble] = unaryalgeb;
  nodeclass[ifnequal] = binaryalgeb;
  nodeclass[ifnexp] = binaryalgeb;
  nodeclass[ifnfirstvalue] = multiplefilter;
  nodeclass[ifnfinalvalue] = multiplefilter;
  nodeclass[ifnfloor] = unaryalgeb;
  nodeclass[ifnint] = unaryalgeb;
  nodeclass[ifniserror] = unaryalgeb;
  nodeclass[ifnless] = binaryalgeb;
  nodeclass[ifnlessequal] = binaryalgeb;
  nodeclass[ifnmax] = binaryalgeb;
  nodeclass[ifnmin] = binaryalgeb;
  nodeclass[ifnminus] = binaryalgeb;
  nodeclass[ifnmod] = binaryalgeb;
  nodeclass[ifnneg] = unaryalgeb;
  nodeclass[ifnnoop] = unaryalgeb;
  nodeclass[ifnnot] = unaryalgeb;
  nodeclass[ifnnotequal] = binaryalgeb;
  nodeclass[ifnplus] = binaryalgeb;
  nodeclass[ifnrangegenerate] = multiplegenr;
  nodeclass[ifnrbuild] = structurebuild;
  nodeclass[ifnrelements] = structureaccess;
  nodeclass[ifnrreplace] = structurebuild;
  nodeclass[ifnreduce] = multiplereduct;
  nodeclass[ifnredleft] = multiplereduct;
  nodeclass[ifnredright] = multiplereduct;
  nodeclass[ifnredtree] = multiplereduct;
  nodeclass[ifnallbutlastvalue] = multiplefilter;
  nodeclass[ifnsingle] = unaryalgeb;
  nodeclass[ifntimes] = unaryalgeb;
  nodeclass[ifntrunc] = unaryalgeb;
  nodeclass[ifnaprefixsize] = structureaccess;
  nodeclass[ifnareplacen] = structurebuild;
  nodeclass[ifnspawn] = controlabstr;
  nodeclass[ifnfilter] = multiplefilter;
  nodeclass[ifnashift] = indexchange;
  /*IF2 Nodes*/
  nodeclass[ifnarraytobuf] = bufferbuild;
  nodeclass[ifnrecordtobuf] = bufferbuild;
  nodeclass[ifnshiftbuffer] = indexchange;
  nodeclass[ifnabufscatter] = buffergenr;
  nodeclass[ifnabufpartition] = bufferaccess;
  nodeclass[ifnrbufelements] = bufferaccess;
  nodeclass[ifnabuildat] = bufferbuild;
  nodeclass[ifnafillat] = bufferbuild;
  nodeclass[ifnareplaceat] = bufferbuild;
  nodeclass[ifnasetlat] = indexchange;
  nodeclass[ifnaelementsat] = bufferaccess;
  nodeclass[ifnacatenateat] = bufferbuild;
  nodeclass[ifnaremlat] = bufferaccess;
  nodeclass[ifnaremhat] = bufferaccess;
  nodeclass[ifnaaddlat] = bufferbuild;
  nodeclass[ifnaaddhat] = bufferbuild;
  nodeclass[ifnaextractat] = bufferaccess;
  nodeclass[ifnrbuildat] = bufferbuild;
  nodeclass[ifnrelementsat] = bufferaccess;
  nodeclass[ifnagatherat] = bufferreduct;
  nodeclass[ifnreduceat] = bufferreduct;
  nodeclass[ifnfinalvalueat] = bufferreduct;
  nodeclass[ifndefarraybuf] = bufferdefine;
  nodeclass[ifndefrecordbuf] = bufferdefine;
  nodeclass[ifnmemalloc] = bufferdefine;
}


/* #include "../util/c.h" */
/* copyright (C) 1986 by the Regents of the University of California */
/* extern int creat PP((Char *name, int mode)); */
/* extern int open PP((Char *name, int mode)); */
/* extern int close PP((int fildes)); */
/* extern Void exit PP((int code)); */


/*#TITLE  FILUTL  ROUTINES        Jul83   File Handling Routines.*/
/*
**  Code for BuildName and Splitname tends to be common to all systems
*/
Static Void buildname(newname, dev, name, ext)
stryng *newname, dev, name, ext;
{
  if (dev.len > 0)
    *newname = dev;
  else {
    mymemcpy(newname->str, blankstring, sizeof(stryngar));
    newname->len = 0;
  }
  if (newname->len < maxstringchars)
    insertstring(newname, &name, newname->len + 1);
  if (ext.len <= 0)
    return;
  concatchar(newname, '.');
  if (newname->len < maxstringchars)
    insertstring(newname, &ext, newname->len + 1);
}


Static Void splitname(origname, dev, name, ext)
stryng *origname, *dev, *name, *ext;
{
  int pos, len;

  /* SplitName */
  *name = *origname;
  pos = findlastchar('.', name);
  if (pos > 0) {
    len = name->len;
    substring(ext, name, pos + 1, len);
    deletestring(name, pos, len);
  } else {
    mymemcpy(ext->str, blankstring, sizeof(stryngar));
    ext->len = 0;
  }
  pos = 0;
  if (pos > 0) {
    substring(dev, name, 1, pos - 1);
    deletestring(name, 1, pos);
  } else {
    mymemcpy(dev->str, blankstring, sizeof(stryngar));
    dev->len = 0;
  }
}


Static Void defaultfilename(name, defname)
stryng *name, *defname;
{
  stryng namedev, namenom, nameext, defdev, defnom, defext;

  /* DefaultFileName */
  splitname(name, &namedev, &namenom, &nameext);
  splitname(defname, &defdev, &defnom, &defext);
  if (namedev.len == 0)
    namedev = defdev;
  if (namenom.len == 0)
    namenom = defnom;
  if (nameext.len == 0)
    nameext = defext;
  buildname(name, namedev, namenom, nameext);
}


/*
**  The rest of the functions tend to be system specific
*/
Static boolean openread(fil, nom)
FILE **fil;
stryng *nom;
{
  boolean Result;
  stryng s;
  int mode, filedescriptor;
  Char STR1[256];

  s = *nom;
  stripspaces(&s);
  concatchar(&s, '\0');
  mode = 0;   /* mode=0 to read */
  filedescriptor = open(s.str, mode);
  if (filedescriptor == -1)
    return false;
  Result = true;
  filedescriptor = close(filedescriptor);
  if (*fil != NULL) {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = freopen(STR1, "r", *fil);
  } else {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = fopen(STR1, "r");
  }
  if (*fil == NULL)
    _EscIO(FileNotFound);
  return Result;
}


Static boolean openintread(fil, nom)
FILE **fil;
stryng *nom;
{
  boolean Result;
  stryng s;
  int mode, filedescriptor;
  Char STR1[256];

  s = *nom;
  stripspaces(&s);
  concatchar(&s, '\0');
  mode = 0;   /* mode=0 to read */
  filedescriptor = open(s.str, mode);
  if (filedescriptor == -1)
    return false;
  Result = true;
  filedescriptor = close(filedescriptor);
  if (*fil != NULL) {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = freopen(STR1, "r", *fil);
  } else {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = fopen(STR1, "r");
  }
  if (*fil == NULL)
    _EscIO(FileNotFound);
  return Result;
}


Static boolean canread(nom)
stryng *nom;
{
  boolean Result;
  int mode, filedescriptor;
  stryng s;

  s = *nom;
  stripspaces(&s);
  concatchar(&s, '\0');
  mode = 0;   /* mode=0 to read */
  filedescriptor = open(s.str, mode);
  if (filedescriptor == -1)
    return false;
  Result = true;
  filedescriptor = close(filedescriptor);
  return Result;
}


/* procedure CloseRead( var Fil: Text ); is a no-op on Unix*/
Static boolean openwrite(fil, nom)
FILE **fil;
stryng *nom;
{
  boolean Result;
  int mode, filedescriptor;
  stryng s;
  Char STR1[256];

  s = *nom;
  stripspaces(&s);
  concatchar(&s, '\0');
  mode = 1;   /* mode=1 to write */
  filedescriptor = open(s.str, mode);
  if (filedescriptor == -1) {
    mode = 438;
    filedescriptor = creat(s.str, mode);
    if (filedescriptor == -1)
      return false;
    Result = true;
    filedescriptor = close(filedescriptor);
    if (*fil != NULL) {
      sprintf(STR1, "%.*s", maxstringchars, s.str);
      *fil = freopen(STR1, "w", *fil);
    } else {
      sprintf(STR1, "%.*s", maxstringchars, s.str);
      *fil = fopen(STR1, "w");
    }
    if (*fil == NULL)
      _EscIO(FileNotFound);
    return Result;
  }
  Result = true;
  filedescriptor = close(filedescriptor);
  if (*fil != NULL) {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = freopen(STR1, "w", *fil);
  } else {
    sprintf(STR1, "%.*s", maxstringchars, s.str);
    *fil = fopen(STR1, "w");
  }
  if (*fil == NULL)
    _EscIO(FileNotFound);
  return Result;
}


Static boolean canwrite(nom)
stryng *nom;
{
  boolean Result;
  int mode, filedescriptor;
  stryng s;

  s = *nom;
  stripspaces(&s);
  concatchar(&s, '\0');
  mode = 1;   /* mode=1 to write */
  filedescriptor = open(s.str, mode);
  if (filedescriptor != -1) {
    Result = true;
    filedescriptor = close(filedescriptor);
    return Result;
  }
  mode = 0;   /* mode=0 to read */
  filedescriptor = open(s.str, mode);
  if (filedescriptor != -1) {   /*can read but can't write*/
    filedescriptor = close(filedescriptor);
    return false;
  }
  mode = 438;
  filedescriptor = creat(s.str, mode);
  if (filedescriptor == -1)
    return false;
  Result = true;
  filedescriptor = close(filedescriptor);
  return Result;

  /*can't read or write; try to create it*/
}


/* procedure CloseWrite( var Fil: Text ); is a no-op on Unix*/
Static Void flushwrite(fil)
FILE *fil;
{
  fflush(fil);
  P_ioresult = 0;
}


/* procedure Page( var Fil: Text ); is defined on unix */
Static Void gettermline(termline, termtext)
stryng *termline, *termtext;
{
  writestring(stdout, termtext);
  readstring(stdin, termline);
}


Static Void getcommandline(cmdline)
stryng *cmdline;
{
  stryng sarg;
  stryngar arg;
  int narg, i;

  /* GetCommandLine */
  mymemcpy(cmdline->str, blankstring, sizeof(stryngar));
  cmdline->len = 0;
  narg = P_argc - 1;
  /* narg is the number of arguments */
  for (i = 1; i <= narg; i++) {
    P_sun_argv(arg, sizeof(stryngar), i);
	/* assigns the i-th argument to arg */
    mymemcpy(sarg.str, arg, sizeof(stryngar));
    sarg.len = maxstringchars;
    stripspaces(&sarg);
    concatchar(&sarg, ' ');
    if (cmdline->len < maxstringchars)
      insertstring(cmdline, &sarg, cmdline->len + 1);
  }
}


/* TO FIX A FILE REMOVAL BUG: CANN 1/92 */
Void CANN_exit(status)
int status;
{
  if (CANN_source != NULL ) {
    fclose(CANN_source);
    sprintf(dummystring, "rm %s", CANN_source_file );
    system(dummystring);
    }

  exit(status);
}


/*#TITLE  PARUTL  ROUTINES        Oct82   Parameter Handling Routines.*/
Static parrec *defineparameter(parlist, long_, short_, parsort, pos1, pos2)
parrec **parlist;
Char *long_, *short_;
partyp parsort;
int pos1, pos2;
{
  parrec *newpar;
  PBBLOCK b;

  /* DefineParameter */
  /* newpar = (parrec *)Malloc(sizeof(parrec)); */
  b = MyBBlockAlloc(); /* CANN */
  newpar = &(b->pr); /* CANN */

  mymemcpy(newpar->longname, long_, sizeof(str10));
  mymemcpy(newpar->shortname, short_, sizeof(str10));
  newpar->partype = parsort;
  newpar->normpos = pos1;
  newpar->specpos = pos2;
  newpar->nextpar = NULL;
  mymemcpy(newpar->parvalue.str, blankstring, sizeof(stryngar));
  newpar->parvalue.len = 0;
  newpar->nextpar = *parlist;
  *parlist = newpar;
  return newpar;
}


/*macro procedure InitParameter( Param: ParRef ); */
Static Void setparameter(param, paramvalue)
parrec *param;
stryng *paramvalue;
{
  int pos;

  /* SetParameter */
  if (((1L << ((long)param->partype)) &
       ((1L << ((long)ifilpar)) | (1L << ((long)ofilpar)))) != 0)
    defaultfilename(&param->parvalue, paramvalue);
  else if (param->parvalue.len == 0) {
    switch (param->partype) {   /* Case */

    case restpar:
    case strpar:
      param->parvalue = *paramvalue;
      break;

    case flagpar:
    case boolpar:
      stringuppercase(paramvalue);
      string10(&param->parvalue, "YES       ");
      if (!equalstrings(paramvalue, &param->parvalue)) {
	string10(&param->parvalue, "NO        ");
	if (!equalstrings(paramvalue, &param->parvalue))
	  deletestring(&param->parvalue, 1, 2);
      }
      break;

    case intpar:
      pos = 1;
      (Void)stringnumber(paramvalue, &pos, 10);
      if (pos == paramvalue->len + 1)
	param->parvalue = *paramvalue;
      break;
    }
  }
  *paramvalue = param->parvalue;
}


/*macro function ParamSet( Par: ParRef ): Boolean; */
/*macro procedure StrParValue( var Str: Stryng; Par: ParRef ); */
Static boolean boolparvalue(par)
parrec *par;
{
  if (((1L << ((long)par->partype)) &
       ((1L << ((long)boolpar)) | (1L << ((long)flagpar)))) != 0)
    return (par->parvalue.len == 3);
  else
    return false;
}


Static long intparvalue(par)
parrec *par;
{
  int pos;

  pos = 1;
  return (stringnumber(&par->parvalue, &pos, 10));
}


Static Char gettoken(token, cmd)
stryng *token, *cmd;
{
  int pos;
  boolean quote, done;
  Char leadchar, ch;
  Char STR1[256], STR2[256];

  /* GetToken */
  quote = false;
  done = false;
  pos = 1;
  while ((cmd->len > 0) & (stringchar(cmd, 1) == ' '))
    deletestring(cmd, 1, 1);
  leadchar = stringchar(cmd, 1);
  if (leadchar == ' ' || leadchar == ' ' || leadchar == '-')
    deletestring(cmd, 1, 1);
  else
    leadchar = ' ';
  do {
    ch = stringchar(cmd, pos);
    if (pos > cmd->len)
      done = true;
    else {
      if (ch == '\'') {
	deletestring(cmd, pos, pos);
	if (quote & (stringchar(cmd, pos) == '\''))
	  pos++;
	else
	  quote = !quote;
      } else {
	if (quote)
	  pos++;
	else {
	  sprintf(STR1, "%c", ch);
	  sprintf(STR2, "%c", leadchar);
	  if ( ( ch == ' ' ) |	/* CHANGED PJM - 921003 (dash arg problem) */
	      ((strcmp(STR1, parargchar) == 0) & (strcmp(STR2, parflagchar) == 0)))
	    done = true;
	  else {
	    if (ch == ' ')
	      deletestring(cmd, pos, pos);
	    else
	      pos++;
	  }
	}
      }
    }
  } while (!done);
  substring(token, cmd, 1, pos - 1);
  deletestring(cmd, 1, pos - 1);
  return leadchar;
}


Static boolean parsecommandline(parlist)
parrec *parlist;
{
  boolean Result;
  stryng commline, flagline, testline, tokenline;
  parrec *parpoint, *foundpar;
  int pospar;
  Char leadchar;
  boolean firsthalf;
  Char STR1[256], STR2[256], STR3[256];

  /* ParseCommandLine */
  getcommandline(&commline);
  stripspaces(&commline);
  Result = (commline.len > 0);
  insertchar(&commline, ' ', 1);
  firsthalf = true;
  pospar = 0;
  while (commline.len > 0) {
    leadchar = gettoken(&tokenline, &commline);
    foundpar = NULL;
    sprintf(STR1, "%c", leadchar);
    /* See if this could be a Flag */
    if (!strcmp(STR1, parflagchar)) {
      parpoint = parlist;
      flagline = tokenline;
      stringuppercase(&flagline);
      /* Try to Locate the Flag */
      while (parpoint != NULL && foundpar == NULL) {
	string10(&testline, parpoint->longname);
	if (equalstrings(&flagline, &testline))
	  foundpar = parpoint;
	else {
	  string10(&testline, parpoint->shortname);
	  if (equalstrings(&flagline, &testline))
	    foundpar = parpoint;
	}
	parpoint = parpoint->nextpar;
      }
      if (foundpar == NULL) {
	/* No Flag found so convert to Positional parameter */
	insertchar(&tokenline, '-', 1);
	leadchar = ' ';
      } else {
	if (foundpar->partype == flagpar)
	  string10(&tokenline, "YES       ");
	else {
	  /* Check for an argument separator if one is required */
	  if (strcmp(parargchar, " ")) {
	    sprintf(STR2, "%c", stringchar(&commline, 1));
	    if (!strcmp(STR2, parargchar))
	      deletestring(&commline, 1, 1);
	    else
	      foundpar = NULL;
	  }
	  /* Pick up argument */
	  if (foundpar != NULL) {
	    if (foundpar->partype == restpar) {
	      while (stringchar(&commline, 1) == ' ')
		deletestring(&commline, 1, 1);
	      tokenline = commline;
	      mymemcpy(commline.str, blankstring, sizeof(stryngar));
	      commline.len = 0;
	    } else {
	      leadchar = gettoken(&tokenline, &commline);
	      sprintf(STR2, "%c", leadchar);
	      if (!strcmp(STR2, parflagchar))
		insertchar(&tokenline, '-', 1);
	      else
		leadchar = '-';
	    }
	  }
	}
      }
    }
    sprintf(STR1, "%c", leadchar);
    /* If no Suitable Parameter found, assume Positional */
    if (strcmp(STR1, parflagchar)) {
      sprintf(STR2, "%c", leadchar);
      /* Deal with Seperators */
      if (!strcmp(STR2, parsepchar))
	pospar++;
      else {
	sprintf(STR3, "%c", leadchar);
	if (!strcmp(STR3, parsplitchar)) {
	  pospar = 1;
	  firsthalf = false;
	}
      }
      parpoint = parlist;
      while (parpoint != NULL && foundpar == NULL) {
	if (!strcmp(parsplitchar, " ")) {
	  if (parpoint->normpos == pospar)
	    foundpar = parpoint;
	} else {
	  if (parpoint->specpos == pospar &&
	      firsthalf == (parpoint->partype == ofilpar))
	    foundpar = parpoint;
	}
	parpoint = parpoint->nextpar;
      }
    }
    /* If Parameter found then set any argument */
    if (foundpar != NULL)
      setparameter(foundpar, &tokenline);
  }
  return Result;
}


Static Void getparamvalue(param, prompt)
parrec *param;
stryng prompt;
{
  stryng parvalue;

  concatchar(&prompt, ':');
  concatchar(&prompt, ' ');
  gettermline(&parvalue, &prompt);
  setparameter(param, &parvalue);
}


Static Void defaultext(name, oldname, ext)
stryng *name, *oldname;
Char *ext;
{
  stryng dev, nom;

  splitname(oldname, &dev, &nom, name);
  string10(name, ext);
  buildname(name, dev, nom, *name);
}


Static Void askparam(par, prompt)
parrec *par;
Char *prompt;
{
  stryng promptstring;

  mymemcpy(par->parvalue.str, blankstring, sizeof(stryngar));
  par->parvalue.len = 0;
  string20(&promptstring, prompt);
  getparamvalue(par, promptstring);
}


Static Void askordefault(par, def, inter, prompt)
parrec *par;
stryng *def;
boolean inter;
Char *prompt;
{
  stryng prstring;

  /* AskOrDefault */
  if (par->parvalue.len <= 0) {
    if (inter) {
      string20(&prstring, prompt);
      if (def->len > 0) {
	concatchar(&prstring, ' ');
	concatchar(&prstring, '[');
	if (prstring.len < maxstringchars)
	  insertstring(&prstring, def, prstring.len + 1);
	concatchar(&prstring, ']');
      }
      getparamvalue(par, prstring);
    }
    if (par->parvalue.len <= 0)
      setparameter(par, def);
  }
  *def = par->parvalue;
}


Static Void readcommandline(infile, outfile, paramlist)
parrec **infile;
parrec **outfile, **paramlist;
{
  boolean interact;
  stryng infilename;
  int i, zero;
  Char ch;
  Char s1[10], s2[10];

  zero = '0';
  for (i = 1; i <= maxinfile; i++) {
    mymemcpy(s1, "IN        ", 10L);
    mymemcpy(s2, "I         ", 10L);
    ch = i + zero;
    s1[2] = ch;
    s2[1] = ch;
    infile[i - 1] = defineparameter(paramlist, s1, s2, ifilpar, i, 1);
  }
  *outfile = defineparameter(paramlist, "OUT       ", "O         ", ofilpar,
			     11, 1);
  interact = parsecommandline(*paramlist);
  interact = (infile[0]->parvalue.len <= 0);
  mymemcpy(infilename.str, blankstring, sizeof(stryngar));
  infilename.len = 0;
  askordefault(infile[0], &infilename, interact, "Input file name     ");
  if (interact)
    infile[0]->parvalue = infilename;
  if ((*outfile)->parvalue.len <= 0) {
    if (infile[0]->parvalue.len > 0)
      (*outfile)->parvalue = infile[0]->parvalue;
  }

/* CANN 1/92 for tmp-file management */
{
stryng hohoho;
stryng s;

defaultext(&hohoho, &(infile[0]->parvalue), "spp       ");
s = hohoho;
stripspaces(&s);
concatchar(&s, '\0');
strcpy( CANN_source_file, s.str );
}
/* CANN 1/92 */
}  /* ReadCommandLine */


Static boolean openinputfile(filename, source)
stryng *filename;
FILE **source;
{
  /* Insures an '.if1' extension is on the file name and then tries to
     open it for reading.  Returns true if file opens properly.
   */
  defaultext(filename, filename, "if1       ");
  return (openread(source, filename));
}  /* OpenInputFile */


Static boolean openoutputfile(filename, source)
stryng *filename;
FILE **source;
{
  /* Insures an '.if1' extension is on the file name and then tries to
     open it for writing.  Returns true if file opens properly.
   */
  defaultext(filename, filename, "if1       ");
  return (openwrite(source, filename));
}  /* OpenOutputFile */


Static boolean openlistingfile(listfilename, list)
stryng listfilename;
FILE **list;
{  /* OpenOutputFile */
  stryng dev, name, ext;

  /* Make sure OutFileName has .pp extension */
  splitname(&listfilename, &dev, &name, &ext);
  defaultext(&listfilename, &name, "pp        ");
  /* Try to open for writing */
  if (canwrite(&listfilename))
    return (openwrite(list, &listfilename));
  else {
    printf("\n***** Could open listing file: ");
    writestring(stdout, &listfilename);
    printf(" *****\n");
    return false;
  }
}  /* OpenListingFile */


Static boolean openuncompfile(uncompfilename, uncomp)
stryng uncompfilename;
FILE **uncomp;
{  /* OpenOutputFile */
  stryng dev, name, ext;

  /* Make sure UncompFileName has .pp extension */
  splitname(&uncompfilename, &dev, &name, &ext);
  defaultext(&uncompfilename, &name, "unc       ");
  /* Try to open for writing */
  if (canwrite(&uncompfilename))
    return (openwrite(uncomp, &uncompfilename));
  else {
    printf("\n***** Could open un-comp output file: ");
    writestring(stdout, &uncompfilename);
    printf(" *****\n");
    return false;
  }
}  /* OpenUnCompFile */


Static stentry *newtypealloc(sort)
char sort;
{
  /*Allocates new type record, initializes fields, returns pointer*/
  stentry *t;
  PBBLOCK b;

  /* t = (stentry *)Malloc(sizeof(stentry)); */
  b = MyBBlockAlloc(); /* CANN */
  t = &(b->i); /* CANN */

  t->stid = -1;
  t->stlabel = -1;
  t->stequivchain = t;
  mymemcpy(t->stliteral.str, blankstring, sizeof(stryngar));
  t->stliteral.len = 0;
  t->stsize = 0;
  t->strecurflag = false;
  t->stsort = sort;
  switch (t->stsort) {   /*with*/

  case iftwild:   /*nothing*/
    break;

  case iftbasic:
    t->UU.stbasic = ifbwild;
    break;

  case iftfunctiontype:
    t->UU.U3.starg = NULL;
    t->UU.U3.stres = NULL;
    break;

  case iftarray:
  case iftstream:
  case iftmultiple:
  case iftrecord:
  case iftunion:
  case iftbuffer:
    t->UU.stbasetype = NULL;
    break;

  case iftfield:
  case ifttuple:
  case ifttag:
    t->UU.U2.stelemtype = NULL;
    t->UU.U2.stnext = NULL;
    break;
  }/*case*/
  return t;
}  /*NewTypeAlloc*/


Static port *newedgealloc(sort)
portsort sort;
{
  /*Allocates new Edge (Port) record, initializes fields, returns pointer*/
  port *e;
  PBBLOCK b;

  switch (sort) {

  case ptlit:
    /* e = (port *)Malloc(sizeof(port)); */
    b = MyBBlockAlloc(); /* CANN */
    e = &(b->p); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3070:
 * Note: No SpecialMalloc form known for PORT.PTLIT [187] */
  case ptedge:
    /* e = (port *)Malloc(sizeof(port)); */
    b = MyBBlockAlloc(); /* CANN */
    e = &(b->p); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3071:
 * Note: No SpecialMalloc form known for PORT.PTEDGE [187] */
  case ptdep:
    /* e = (port *)Malloc(sizeof(port)); */
    b = MyBBlockAlloc(); /* CANN */
    e = &(b->p); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3072:
 * Note: No SpecialMalloc form known for PORT.PTDEP [187] */
  case ptundef:
    /* e = (port *)Malloc(sizeof(port)); */
    b = MyBBlockAlloc(); /* CANN */
    e = &(b->p); /* CANN */
    break;
  }/*case*/
  e->pttype = NULL;
  e->pttonode = NULL;
  e->pttoport = -1;
  e->ptid = -1;
  e->ptmark = byval;
  e->ptclass = pcuncoded;
  e->ptif1line = -1;
  e->ptsrcline = -1;
  e->ptwiline = -1;
  mymemcpy(e->ptname.str, blankstring, sizeof(stryngar));
  e->ptname.len = 0;
  e->ptlbound = -SHORT_MAX;
  e->ptubound = -SHORT_MAX;   /*dlz - ???, but canonical*/
  e->ptsetrc = -SHORT_MAX;
  e->ptconmodrc = -SHORT_MAX;
  e->ptprodmodrc = -SHORT_MAX;
  e->ptdfaddr = -SHORT_MAX;
  e->ptmraddr = 0;
  e->pttonext = NULL;
  e->ptnextedge = NULL;
  e->ptsort = sort;
  switch (e->ptsort) {   /*with*/

  case ptedge:
    e->UU.U1.ptfrnode = NULL;
    e->UU.U1.ptfrnext = NULL;
    e->UU.U1.ptfrport = -1;
    break;

  case ptdep:
    e->UU.U1.ptfrnode = NULL;
    e->UU.U1.ptfrnext = NULL;
    e->UU.U1.ptfrport = 0;
    e->pttoport = 0;
    string10(&e->ptname, "DEPENDENCE");
    break;

  case ptlit:
    mymemcpy(e->UU.ptlitvalue.str, blankstring, sizeof(stryngar));
    e->UU.ptlitvalue.len = 0;
    break;

  case ptundef:   /*nothing*/
    break;
  }/*case*/
  return e;
}  /*NewEdgeAlloc*/


Static node *newnodealloc(sort)
nodesort sort;
{
  /*Allocates new Node record, initializes fields, returns pointer*/
  node *n;
  PBBLOCK b; /* CANN */

  switch (sort) {

  case ndatomic:
    /* n = (node *)Malloc(sizeof(node)); */
    b = MyBBlockAlloc(); /* CANN */
    n = &(b->n); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3109:
 * Note: No SpecialMalloc form known for NODE.NDATOMIC [187] */
  case ndgraph:
    /* n = (node *)Malloc(sizeof(node)); */
    b = MyBBlockAlloc(); /* CANN */
    n = &(b->n); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3110:
 * Note: No SpecialMalloc form known for NODE.NDGRAPH [187] */
  case ndcompound:
    /* n = (node *)Malloc(sizeof(node)); */
    b = MyBBlockAlloc(); /* CANN */
    n = &(b->n); /* CANN */
    break;

/* p2c: mono.bin.noblank.p, line 3111:
 * Note: No SpecialMalloc form known for NODE.NDCOMPOUND [187] */
  case ndundef:
    /* n = (node *)Malloc(sizeof(node)); */
    b = MyBBlockAlloc(); /* CANN */
    n = &(b->n); /* CANN */
    break;
  }/*case*/
  n->ndid = -1;
  n->ndlabel = -1;
  n->ndcode = -1;
  n->ndmisc.numb = 0;
  n->ndline = -1;
  n->ndsrcline = -1;
  n->ndwiline = -1;
  n->ndxcoord = -SHORT_MAX;
  n->ndycoord = -SHORT_MAX;
  n->ndparent = NULL;
  n->ndnext = NULL;
  n->ndnextinline = NULL;
  n->ndilist = NULL;
  n->ndolist = NULL;
  n->nddepilist = NULL;
  n->nddepolist = NULL;
  n->ndfrequency = -1.0;
  n->ndexpanded = -1;
  n->ndsort = sort;
  switch (n->ndsort) {   /*with*/

  case ndatomic:   /*nothing*/
    break;

  case ndgraph:
    n->UU.U1.ndlink = NULL;
    n->UU.U1.ndtype = NULL;
    n->UU.U1.ndfirstmro = 0;
    n->UU.U1.ndlastmro = 0;
    n->ndlabel = 0;
    break;

  case ndcompound:
    n->UU.U2.ndsubsid = NULL;
    n->UU.U2.ndassoc = NULL;
    break;

  case ndundef:   /*nothing*/
    break;
  }/*case*/
  return n;
}  /*NewNodeAlloc*/


Static Void initstamps()
{
  Char c;
  stryng name;

  mymemcpy(name.str, blankstring, sizeof(stryngar));
  name.len = 0;
  for (c = 'A'; c <= 'Z'; c++) {
    P_addset(stampset, c);
    stamp[c - 'A'] = name;
  }
}  /* InitStamps */


Static Void removestamp(ch)
Char ch;
{
  P_remset(stampset, ch);
}  /* RemoveStamp */


Static Void addstamp(ch, str)
Char ch;
stryng str;
{
  P_addset(stampset, ch);
  stamp[ch - 'A'] = str;
}  /* AddStamp */


Static boolean stampisset(ch)
Char ch;
{
  return P_inset(ch, stampset);
}  /* StampIsSet */


Static boolean stampismissing(ch)
Char ch;
{
  return (!P_inset(ch, stampset));
}  /* StampIsMissing */


/* The following function give information about Nodes */
/* macro function NodeId ( gnode : NDPtr) : integer; */
/* macro function IsSimple( N: NdPtr ) : boolean */
/* macro function IsCompound( N: NdPtr ) : boolean */
/* macro function IsGraph( N: NdPtr ) : boolean */
/* macro function IsEmptyGraph( N: NdPtr ) : boolean */
/* IsEmptyGraph returns true if the graph has no internal nodes
   Assumes:(graph^.NDSort = NDGraph)       */
/*macro function IsLastNodeInGraph( N: NdPtr ) : boolean */
/*macro function NotEndOfGraph( N: NdPtr ) : boolean */
/*macro function IsFirstNodeInGraph( N: NdPtr ) : boolean */
/* assumes N <> nil */
Static int numbofsubgraphs(cnode)
node *cnode;
{
  /* Assumes cnode <> nil and (cnode^.NDSort = NDCompound)
     returns the number of subgraphs of this compound node,
     if a value of zero is returned, you know something is wrong.  */
  int count;
  graph *tmp;

  count = 0;
  tmp = cnode->UU.U2.ndsubsid;
  while (tmp != NULL) {
    count++;
    tmp = tmp->grnext;
  }
  return count;
}  /* NumbOfSubgraphs */


Static int numbnodesingraph(gnode)
node *gnode;
{
  /* Assumes gnode <> nil and (gnode^.NDSort = NDGraph)
     returns the number of nodes within this graph, the
     graphnode itself is not counted and and the compound
     nodes are counted only once (ie, we do not dive inside
     compounds. Empty graphs return a count of zero.  */
  int count;
  node *tmp;

  count = 0;
  tmp = gnode->ndnext;
  while (tmp != NULL) {
    count++;
    tmp = tmp->ndnext;
  }
  return count;
}  /* NumbNodesInGraph */


/* macro procedure NameOfGraph( var S: stryng; G: NdPtr ) */
/* macro function NumbOfAlternatives( N: NDPtr ) : integer */
/* NumbOfAlternatives assumes (N^.NDSort = NDCompound)
 One child graph is the selector graph, the others are Alternatives */
/* macro function NodeKind( N: NDPtr ) : integer */
Static int numberofwiredinputports(n)
node *n;
{
  /* Assumes N <> nil */
  port *eptr;
  int count;

  eptr = n->ndilist;
  count = 0;
  while (eptr != NULL) {
    count++;
    eptr = eptr->pttonext;
  }
  return count;
}  /* NumberOfInputPorts */


Static int numberofwiredoutputports(n)
node *n;
{
  /* Assumes N <> nil and edges are ordered by port numbers */
  port *e;
  int count, portnum;

  e = n->ndolist;
  count = 0;
  portnum = -1;
  while (e != NULL) {
    /* assumes edges are in order by port number */
    if (portnum != e->UU.U1.ptfrport) {
      count++;
      portnum = e->UU.U1.ptfrport;
    }
    e = e->UU.U1.ptfrnext;
  }
  return count;
}  /* NumberOfWiredOutputPorts */


Static int largestinputportnumber(n)
node *n;
{
  /* Assumes N <> nil
     returns the largest port number on the nodes input port list.
     NOTE: edges are ordered in the list by increasing port number */
  port *e;

  e = n->ndilist;
  if (e == NULL)
    return 0;
  else {
    while (e->pttonext != NULL)
      e = e->pttonext;
    return (e->pttoport);
  }
}  /* LargestInputPortNumber */


Static int largestoutputportnumber(n)
node *n;
{
  /* Assumes N <> nil
     returns largest port number on nodes output list */
  port *e;

  e = n->ndolist;
  if (e == NULL)
    return 0;
  else {
    while (e->UU.U1.ptfrnext != NULL)
      e = e->UU.U1.ptfrnext;
    return (e->UU.U1.ptfrport);
  }
}  /* LargestOutputPortNumber */


Static int nodelabel(n)
node *n;
{
  if (n == NULL)
    return -1;
  else
    return (n->ndlabel);
}  /* NodeLabel */


/* macro function FunctionKind( F : NDPtr ) : LinkSort; */
/* macro function IsLocalFunction( fungraph: NDPtr) : boolean; */
/* Assumes fungraph <> nil and fungraph is a function graph */
/* macro function IsGlobalFunction( fungraph: NDPtr) : boolean; */
/* Assumes fungraph <> nil and fungraph is a function graph */
/* The following functions give information about edges */
/* macro function IsEdge( E: EGPtr ) : boolean */
/* macro function IsLiteral( E: EGPtr ) : boolean */
/* macro function IsDependence( E: EGPtr ) : boolean */
Static boolean isboundaryedge(e)
port *e;
{
  /* Assumes E <> nil */
  /* returns true if E is incident on a graph boundary */
  boolean isgraphedge;

  if (e->ptsort == ptedge)
    isgraphedge = (e->UU.U1.ptfrnode->ndsort == ndgraph);
  else
    isgraphedge = false;
  return (isgraphedge || e->pttonode->ndsort == ndgraph);
}  /* IsBoundaryEdge */


/* macro function EdgeId( E: EGPtr ) : integer */
/* macro procedure NameOfEdge( var S: stryng; E: EGPtr ) */
/* macro procedure ValueOfLiteral( var S: stryng; E: EGPtr ) */
/* macro function ValueOfSetRC( E : EGPtr ) : integer; */
/* macro function ValueOfModRC(E : EGPtr ) : integer; */
/* macro function ValueOfProdRC(E : EGPtr ) : integer; */
/* macro function ConsumerNodeOfEdge( E : EGPtr ) : NDPtr; */
Static node *producernodeofedge(e)
port *e;
{
  /* Assumes E <> nil
     if edge is a literal there is no producer node and nil is returned */
  if (e->ptsort == ptedge || e->ptsort == ptdep)
    return (e->UU.U1.ptfrnode);
  else
    return NULL;
}  /* ProducerNodeOfEdge */


/* macro TypeOfEdge( E: EGPtr ) : STPtr */
/* macro DataSizeOfEdge( E: EGPtr ) : real */
/* macro SetDataSizeOfEdge( E : EGPtr; Value : real ); */
/* macro CardinalityOfEdge( E: EGPtr ) : real */
/* macro SetCardinalityOfEdge( E : EGPtr; Value : real ); */
/* macro SetCommOfEdge( E : EGPtr; Value : boolean); */
/* macro IsEdgeCommunicated( E : EGptr) : boolean;  */
/* macro SetRCOfEdge( E : EGPtr; Value : integer); */
/* macro SetProdModRCOfEdge( E : EGPtr; Value : integer); */
/* macro SetConModRCOfEdge( E : EGPtr; Value : integer); */
Static int producerportnumber(e)
port *e;
{
  /* Assumes E <> nil */
  if (e->ptsort == ptedge)
    return (e->UU.U1.ptfrport);
  else
    return -1;
}  /* ProducerPortNumber */


/* macro ConsumerPortNumber( E: EGPtr ) : integer */
/* Assumes E <> nil */
/* The following functions are used to traverse the graph */
Static node *tonamedgraph(name)
stryng name;
{
  /* returns the graphnode of the function with the given name.
     If no such function exists then it returns nill.
     rky changed 25May88 to use EquivName instead of StringLowerCase and
     EqualStrings. */
  boolean found;
  linkrec *funptr;

  funptr = funclist;
  found = false;
  do {
    if (funptr == NULL)
      found = true;
    else {
      if (equivstrings(&name, &funptr->lkname))
	found = true;
      else
	funptr = funptr->lknext;
    }
  } while (!found);
  if (funptr == NULL)
    return NULL;
  else
    return (funptr->lkgraph);
}  /* ToNamedGraph */


/* macro function ToFirstNodeInGraph( N: NdPtr ) : integer */
/* toFirstNodeInGraph assumes (graph^.NDSort = NDGraph)
 returns the first scheduled to be executed in this graph,
 if the Graph is empty thein it returns nil       */
/* macro ExecTimeOfNode( N : NDPtr ) : real */
/* macro SetExecTimeOfNode( N : NDPtr; Value : real ); */
/* macro ProcNumberOfNode( N : NDPtr ) : integer */
/* macro SetProcNumberOfNode( N : NDPtr; Value : integer ); */
/* macro SchedNumberOfNode( N : NDPtr ) : integer */
/* macro SetSchedNumberOfNode( N : NDPtr; Value : integer ); */
/* macro FrequencyOfNode( N : NDPtr ) : real */
/* macro SetFrequencyOfNode( N : NDPtr; Value : real ); */
/* macro IsNodeExpanded( N : NDPtr) : boolean; */
/* macro SetExpandedValueOfNode(N : NDPtr; Val : boolean); */
Static node *tolastnodeingraph(n)
node *n;
{
  /* assumes N <> nil and (N^.NDSort = NDGraph) */
  /* IfEmptyGraph(N) then it returns nil */
  node *last;

  if (n->ndnext == NULL)
    return NULL;
  else {
    last = n->ndnext;
    while (last->ndnext != NULL)
      last = last->ndnext;
    return last;
  }
}  /* ToLastNodeInGraph */


/*macro function ToNextNode( N: NdPtr ) : NdPtr */
/* ToNextNode assumes gnode <> nil
   Returns next node in this graph to be executed,
   returns nil if no next node*/
Static node *toprevnode(n)
node *n;
{
  /* Assumes N <> nil */
  /* if (N^.NDSort = NDGraph) or (n = n^.NDParent^.NDNext) then returns nil
     otherwise it returns the node previously scheduled to be
     executed. */
  node *prev;

  if (n->ndsort == ndgraph)
    return NULL;
  else if (n->ndparent->ndnext == n)
    return NULL;
  else {
    prev = n->ndparent->ndnext;
    while (prev->ndnext != n)
      prev = prev->ndnext;
    return prev;
  }
}  /* ToPrevNode */


/*macro ToEnclosingGraph( GNode: NdPtr ) : NdPtr */
/* ToEnclosingGraph assumes gnode <> nil and ( not (gnode^.NDSort = NDGraph))
 returns a pointer to the graphnode surrounding this one. */
/*macro function N: NdPtr ^.NDNext : NdPtr */
/* ^.NDNext assumes gnode <> nil
   Returns next node in this graph to be executed,
   returns nil if no next node*/
/*macro function ToFirstChildGraph( cnode : NDPtr) : NDPtr */
/* Assumes (cnode^.NDSort = NDCompound), with at least one subgraph.
   Result is a pointer to first subgraph of this compound node.  rky 8/87 */
Static node *tonextchildgraph(child)
node *child;
{
  /* Assumes child points to a subgraph of a compound node.  rky 8/87
     Result is a pointer to next subgraph of the same compound node, or nil.  */
  graph *tmp;

  tmp = child->ndparent->UU.U2.ndsubsid;
  while (tmp->grnode != child)
    tmp = tmp->grnext;
  if (tmp->grnext == NULL)
    return NULL;
  else
    return (tmp->grnext->grnode);
}  /* ToNextChildGraph */


Static node *tochildgraph(cnode, gindex)
node *cnode;
int gindex;
{
  /* Assumes (cnode^.NDSort = NDCompound) and Gindex >= 0
     returns a pointer to the i'th subgraph of this compound
     node where i is the value of Gindex.  If no such subgraph
     exists, it returns the nill pointer.      */
  graph *tmp;

  tmp = cnode->UU.U2.ndsubsid;
  while (tmp != NULL && gindex > 0) {
    gindex--;
    tmp = tmp->grnext;
  }
  if (tmp == NULL)
    return NULL;
  else
    return (tmp->grnode);
}  /* ToChildGraph */


Static node *toenclosingcompound(gnode)
node *gnode;
{
  /* Assumes gnode <> nil
     returns a pointer to the closest enclosing Compound node
     if one exists.  Returns nil if such a node doesn't exist.
   */
  if (gnode == module)
    return NULL;
  else {
    if (gnode->ndsort != ndgraph)
      gnode = gnode->ndparent;
    /* gnode now points to a graph node */
    gnode = gnode->ndparent;
    /* gnode now points to a compound node */
    if (gnode == module)
      return NULL;
    else
      return gnode;
  }
}  /* ToEnclosingCompound */


Static node *toenclosingfunction(n)
node *n;
{
  /* ToEnclosingFunction assumes N <> nil and N <> Module,
     it returns a pointer to the function graph that properly contains
     the node N. */
  if (n == NULL || n == module)
    return NULL;
  else {
    if (n->ndsort != ndgraph)
      n = n->ndparent;
    /* from this point on N will always point to a graph node */
    while (n->UU.U1.ndlink == NULL) {
      /* N points to a subgraph of a compound node */
      n = n->ndparent->ndparent;
    }
    return n;
  }
}  /* ToEnclosingFunction */


Static node *toinitgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     If the node is not a LoopB or LoopA node then return nil */
  if (n->ndcode == ifnloopa || n->ndcode == ifnloopb)
    return (n->UU.U2.ndsubsid->grnode);
  else
    return NULL;
}  /* ToInitGraph */


Static node *tobodygraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     If the node is not a LoopB or LoopA node then return nil */
  if (n->ndcode == ifnloopa || n->ndcode == ifnloopb)
    return (n->UU.U2.ndsubsid->grnext->grnext->grnode);
  else
    return NULL;
}  /* ToBodyGraph */


Static node *totestgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     If the node is not a LoopB or LoopA node then return nil */
  if (n->ndcode == ifnloopa || n->ndcode == ifnloopb)
    return (n->UU.U2.ndsubsid->grnext->grnode);
  else
    return NULL;
}  /* ToTestGraph */


Static node *toreturnsgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     If the node is not a LoopB or LoopA node then return nil */
  if (n->ndcode == ifnloopa || n->ndcode == ifnloopb)
    return (n->UU.U2.ndsubsid->grnext->grnext->grnext->grnode);
  else
    return NULL;
}  /* ToReturnsGraph */


Static node *toiterbodygraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     if N is not a Iter node, then return nil */
  if (n->ndcode == ifniter)
    return (n->UU.U2.ndsubsid->grnode);
  else
    return NULL;
}  /* ToIterBodyGraph */


Static node *toselectorgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     If N is not a Select node, then return nil */
  if (n->ndcode == ifnselect)
    return (n->UU.U2.ndsubsid->grnode);
  else
    return NULL;
}  /* ToSelectorGraph */


Static node *toalternativegraph(n, i)
node *n;
int i;
{
  /* Assumes (N^.NDSort = NDCompound)
     If N is not a Select node, then return nil */
  if (n->ndcode == ifnselect)
    return (tochildgraph(n, i + 1));
  else
    return NULL;
}  /* ToAlternativeGraph */


Static node *toforallgeneratorgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     if N is not a Forall node, then return nil */
  if (n->ndcode == ifnforall)
    return (n->UU.U2.ndsubsid->grnode);
  else
    return NULL;
}  /* ToForallGeneratorGraph */


Static node *toforallbodygraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     if N is not a Forall node, then return nil */
  if (n->ndcode == ifnforall)
    return (n->UU.U2.ndsubsid->grnext->grnode);
  else
    return NULL;
}  /* ToForallBodyGraph */


Static node *toforallreturnsgraph(n)
node *n;
{
  /* Assumes (N^.NDSort = NDCompound)
     if N is not a Forall node, then return nil */
  if (n->ndcode == ifnforall)
    return (n->UU.U2.ndsubsid->grnext->grnext->grnode);
  else
    return NULL;
}  /* ToForallReturnsGraph */


Static node *totagcasesubgraph(t, tagnum)
node *t;
int tagnum;
{
  assoclist *alist;

  alist = t->UU.U2.ndassoc;
  while (tagnum > 0) {
    if (alist == NULL)
      tagnum = 0;
    else {
      tagnum--;
      alist = alist->next;
    }
  }
  if (alist == NULL)
    return NULL;
  else
    return (tochildgraph(t, alist->graphnum));
}  /* ToTagCaseSubgraph */


/* macro function NumberOfPredicates( C : NDPtr ) : integer */
/*C is an IfThenElse node.  Returns the number of predicate subgraphs */
/* macro function ToNthPredicateGraph( C : NDPtr; N : integer ) : NDPtr */
/*C is an IfThenElse node, N = 1, 2, ....      */
/*Returns a pointer to the N'th predicate subgraph      */
/* macro function ToNthTrueGraph( C : NDPtr; N : integer ) : NDPtr */
/*C is an IfThenElse node, N = 0, 1, ....      */
/*Returns a pointer to the N'th true subgraph      */
/* macro function ToFalseGraph( N : NDPtr ) : NDPtr */
/* N is an IfThenElse node.Returns a pointer to the false subgraph */
Static port *getinputedge(n, portnum)
node *n;
int portnum;
{
  /* If no edge is connected to this port number then nil is returned */
  port *eptr;
  boolean found;

  eptr = n->ndilist;
  found = false;
  while (!found) {
    if (eptr == NULL) {
      found = true;
      break;
    }
    if (eptr->pttoport == portnum)
      found = true;
    else
      eptr = eptr->pttonext;
  }
  return eptr;
}  /* GetInputEdge */


Static port *getoutputedge(n, portnum)
node *n;
int portnum;
{
  /* If no edge is connected to this port number then nil is returned */
  port *eptr;
  boolean found;

  eptr = n->ndolist;
  found = false;
  while (!found) {
    if (eptr == NULL) {
      found = true;
      break;
    }
    if (eptr->UU.U1.ptfrport == portnum)
      found = true;
    else
      eptr = eptr->UU.U1.ptfrnext;
  }
  return eptr;
}  /* GetOutputEdge */


Static port *nextoutputedgesameport(e)
port *e;
{
  /* Returns nil if no next edge exists */
  int portnum;

  portnum = e->UU.U1.ptfrport;
  e = e->UU.U1.ptfrnext;
  if (e != NULL) {
    if (e->UU.U1.ptfrport > portnum)
      e = NULL;
  }
  return e;
}  /* NextOutputEdgeSamePort */


/* Assumes N <> nil */
/* Assumes N <> nil */
/* Assumes E <> nil */
/* Assumes N <> nil */
/* Assumes N <> nil */
/* Assumes E <> nil */
/* dlz - 8/87 instead of for-loops */
/* over port range      */
Static port *nextinputedgenextport(e, p)
port *e;
int p;
{
  /* Added 21Sept87 rky.  Does same job as GetInputEdge, but faster.
     Returns the edge attached to input port p, where E is a non-nil edge
     attached to a further-left input port of the same node.
     Returns nil if no input edge exists on port p.  */
  port *nie;

  nie = e->pttonext;
  if (nie == NULL)
    return NULL;
  else if (nie->pttoport == p)
    return nie;
  else
    return NULL;
}  /* NextInputEdgeNextPort */


Static port *nextoutputedgenextport(e)
port *e;
{
  /* Added 15-Oct-87 dlz ; Returns the next non-fanout output edge,
     as above, for speed... */
  int p;

_L1:
  p = e->UU.U1.ptfrport;
  e = e->UU.U1.ptfrnext;
  if (e != NULL) {
    if (e->UU.U1.ptfrport == p)
      goto _L1;
  }
  return e;
}


Static port *inputedgegeport(n, p)
node *n;
int p;
{
  port *e;
  boolean found;

  e = n->ndilist;
  found = false;
  while (!found) {
    if (e == NULL) {
      found = true;
      break;
    }
    if (e->pttoport >= p)
      found = true;
    else
      e = e->pttonext;
  }
  return e;
}  /* InputEdgeGEPort */


Static port *outputedgegeport(n, p)
node *n;
int p;
{
  port *e;
  boolean found;

  e = n->ndolist;
  found = false;
  while (!found) {
    if (e == NULL) {
      found = true;
      break;
    }
    if (producerportnumber(e) >= p)
      found = true;
    else
      e = e->UU.U1.ptfrnext;
  }
  return e;
}  /* OutputEdgeGEPort */


Static node *getnodewithlabel(n, l)
node *n;
int l;
{
  boolean found, finished;

  found = false;
  finished = false;
  while (!(finished || found)) {
    if (n == NULL) {
      finished = true;
      break;
    }
    if (n->ndlabel == l) {
      found = true;
      break;
    }
    if (n->ndlabel > l)
      finished = true;
    else
      n = n->ndnext;
  }
  if (found)
    return n;
  else
    return NULL;
}  /* GetNodeWithLabel */


Static node *getnodewithid(f, id)
node *f;
int id;
{
  node *n, *gn;
  graph *g;
  boolean found;

  n = f;
  found = false;
  while (!found && n != NULL) {
    if (n->ndid == id) {
      found = true;
      break;
    }
    if (n->ndsort != ndcompound) {
      n = n->ndnext;
      continue;
    }  /* N is compound */
    g = n->UU.U2.ndsubsid;
    while (!found && g != NULL) {
      gn = getnodewithid(g->grnode, id);
      if (gn == NULL)   /*not found .. go to next subgraph*/
	g = g->grnext;
      else {
	/*found .. save the node and signal completion*/
	n = gn;
	found = true;
      }  /* else */
    }  /* while */
    if (!found)   /*move to next node*/
      n = n->ndnext;
  }
  return n;

  /* N is simple or a graph node .. move to next */
}  /* GetNodeWithID */


Static node *tonextfunction(f)
node *f;
{
  if (f == NULL)
    return NULL;
  else if (f->UU.U1.ndlink->lknext == NULL)
    return NULL;
  else
    return (f->UU.U1.ndlink->lknext->lkgraph);
}  /* ToNextFunction */


/* Functions used to examine and add to the type table */
/* macro function NameOfType( T: STPtr ) : Stryng */
/* macro WhichEntryType( S: STPtr ) : STENtry */
/* macro WhichBasicType( S: STPtr ) : STPtr */
/* macro function TypeOfGraph( G : NDPtr ) : STPtr; */
/* macro function BaseOfMultipleType( M: STPtr ) : STPtr; */
/* macro function BaseOfArrayType( M: STPtr ) : STPtr; */
/* macro function BaseOfTupleType( M: STPtr ) : STPtr; */
/* macro function BaseOfBufferType( M: STPtr ) : STPtr; */
/* macro function BaseOfStreamType( S: STPtr ) : STPtr; */
/* macro function GetFirstFunRes( N: NDPtr ) : STPtr; */
/* macro function GetFirstFunArg( N: NDPtr ) : STPtr; */
Static stentry *getnextfunarg(s)
stentry *s;
{
  if (s->stsort != ifttuple)
    return NULL;
  else
    return (s->UU.U2.stnext);
}  /* GetNextFunArg */


Static stentry *getnextfunres(s)
stentry *s;
{
  if (s->stsort != ifttuple)
    return NULL;
  else
    return (s->UU.U2.stnext);
}  /* GetNextFunRes */


Static int numberoffunargs(t)
stentry *t;
{
  /* Assumes T <> nil */
  stentry *temp;
  int count;

  count = 0;
  temp = t->UU.U3.starg;
  while (temp != NULL) {
    count++;
    if (temp->stsort == ifttuple)
      temp = temp->UU.U2.stnext;
    else
      temp = NULL;
  }
  return count;
}  /* NumberOfFunArgs */


Static int numberoffunres(t)
stentry *t;
{
  /* Assumes T <> nil */
  stentry *temp;
  int count;

  count = 0;
  temp = t->UU.U3.stres;
  while (temp != NULL) {
    count++;
    if (temp->stsort == ifttuple)
      temp = temp->UU.U2.stnext;
    else
      temp = NULL;
  }
  return count;
}  /* NumberOfFunRes */


Static stentry *typeofnthfunarg(f, n)
stentry *f;
int n;
{
  /* Return the type of the Nth function input argument */
  if (n < 1 || f->stsort != iftfunctiontype)
    return NULL;
  else {
    f = f->UU.U3.starg;
    while (n > 1) {
      n--;
      if (f != NULL)
	f = f->UU.U2.stnext;
    }
    if (f == NULL)
      return NULL;
    else
      return (f->UU.U2.stelemtype);
  }
}  /* TypeOfNthFunArg */


Static stentry *typeofnthfunres(f, n)
stentry *f;
int n;
{
  /* Returns the type of the Nth function output argument */
  if (n < 1 || f->stsort != iftfunctiontype)
    return NULL;
  else {
    f = f->UU.U3.stres;
    while (n > 1) {
      n--;
      if (f != NULL)
	f = f->UU.U2.stnext;
    }
    if (f == NULL)
      return NULL;
    else
      return (f->UU.U2.stelemtype);
  }
}  /* TypeOfNthFunRes */


Static int largestfieldnumberofrecord(r)
stentry *r;
{
  /* Returns the number of Fields defined for this record type */
  stentry *f;
  int count;

  if (r->stsort != iftrecord)
    return -1;
  else {
    count = -1;
    f = r->UU.stbasetype;
    while (f != NULL) {
      count++;
      f = f->UU.U2.stnext;
    }
    return count;
  }
}  /* LargestFieldNumberOfRecord */


Static Void typeandnameofnthfield(r, n, ftype, fname)
stentry *r;
int n;
stentry **ftype;
stryng *fname;
{
  /* Pre:  R is a RECORD type entry and N is an integer greater
          than or equal to zero.
    Post: The type and name of the Nth field ofrecord R are placed in
          FType and FName.
    Note:  If R is not a record entry, or N < 0, or N > the largest
           field number defined for this record (using a zero based
           numbering scheme) then Nil is placed in FType and FName is
           empty
  */
  mymemcpy(fname->str, blankstring, sizeof(stryngar));
  fname->len = 0;
  if (n < 0 || r->stsort != iftrecord) {
    *ftype = NULL;
    return;
  }
  r = r->UU.stbasetype;
  while (n > 0) {
    n--;
    if (r != NULL)
      r = r->UU.U2.stnext;
  }
  if (r == NULL)
    *ftype = NULL;
  else {
    *ftype = r->UU.U2.stelemtype;
    *fname = r->stliteral;
  }
}  /* TypeOfNthField */


Static stentry *typeofnthfield(r, n)
stentry *r;
int n;
{
  /* Pre:  R is a RECORD type entry and N is an integer greater
          than or equal to zero.
    Post: The type of the Nth field ofrecord R is returned
    Note:  If R is not a record entry, or N < 0, or N > the largest
           field number defined for this record (using a zero based
           numbering scheme) then Nil is returned.
  */
  if (n < 0 || r->stsort != iftrecord)
    return NULL;
  else {
    r = r->UU.stbasetype;
    while (n > 0) {
      n--;
      if (r != NULL)
	r = r->UU.U2.stnext;
    }
    if (r == NULL)
      return NULL;
    else
      return (r->UU.U2.stelemtype);
  }
}  /* TypeOfNthField */


Static int largesttagnumberofunion(u)
stentry *u;
{
  /* Returns the number of Tags defined for this Union type */
  stentry *f;
  int count;

  if (u->stsort != iftunion)
    return -1;
  else {
    count = -1;
    f = u->UU.stbasetype;
    while (f != NULL) {
      count++;
      f = f->UU.U2.stnext;
    }
    return count;
  }
}  /* LargestTagNumberOfUnion */


Static Void typeandnameofnthtag(u, n, ttype, tname)
stentry *u;
int n;
stentry **ttype;
stryng *tname;
{
  /* Pre:  U is a UNION type entry and N is an integer greater
          than or equal to zero.
    Post: The type of the Nth tag of the union type U is placed in TType and
          the name of the Nth tag is placed in TName.
    Note:  If U is not a union entry, or N < 0, or N > the largest
           tag defined for this union then TType = nil and TName is
           empty.
  */
  mymemcpy(tname->str, blankstring, sizeof(stryngar));
  tname->len = 0;
  if (n < 0 || u->stsort != iftunion) {
    *ttype = NULL;
    return;
  }
  u = u->UU.stbasetype;
  while (n > 0) {
    n--;
    if (u != NULL)
      u = u->UU.U2.stnext;
  }
  if (u == NULL)
    *ttype = NULL;
  else {
    *ttype = u->UU.U2.stelemtype;
    *tname = u->stliteral;
  }
}  /* TypeOfNthTag */


Static stentry *typeofnthtag(u, n)
stentry *u;
int n;
{
  /* Pre:  U is a UNION type entry and N is an integer greater
          than or equal to zero.
    Post: The type of the Nth tag of the union type U is returned
    Note:  If U is not a union entry, or N < 0, or N > the largest
           tag defined for this union then Nil is returned.
  */
  if (n < 0 || u->stsort != iftunion)
    return NULL;
  else {
    u = u->UU.stbasetype;
    while (n > 0) {
      n--;
      if (u != NULL)
	u = u->UU.U2.stnext;
    }
    if (u == NULL)
      return NULL;
    else
      return (u->UU.U2.stelemtype);
  }
}  /* TypeOfNthTag */


/* The following is a collection of functions that allow run-time additions
   to the Type Table.  Entries are constructed, then smashed into the table
   before being assigned to data objects.
 */
/*macro function MakeFunctionType () : STPtr;*/
/*macro function MakeRecordType () : STPtr;*/
/*macro function MakeUnionType () : STPtr;*/
Static stentry *searchtypetable(kind, x, y, name)
char kind;
stentry *x, *y;
stryng name;
{
  /* Search the Type Table for an entry of the given Kind that has:
      if Kind in [Stream, Array, Multiple, Record, Union, Buffer ] then
        X is its STBaseType
      if Kind in [Field, Tag] then
        X is its STElemType and Y is its STNext
    If an entry is not found, then return nil
  */
  int i;
  stentry *tbl;
  boolean found;

  found = false;
  i = 1;
  while (!found && i <= tthwm) {
    tbl = typetable[i - 1];
    if (tbl == NULL) {
      i++;
      continue;
    }
    if (tbl->stsort != kind) {
      i++;
      continue;
    }
    switch (kind) {

    case iftwild:
    case iftbasic:
      found = true;
      break;

    case iftmultiple:
    case iftarray:
    case iftstream:
    case iftrecord:
    case iftunion:
    case iftbuffer:
      if (tbl->UU.stbasetype == x)
	found = true;
      else
	i++;
      break;

    case iftfield:
    case ifttag:
      if ((tbl->UU.U2.stelemtype == x && tbl->UU.U2.stnext == y) &
	  equalstrings(&tbl->stliteral, &name))
	found = true;
      else
	i++;
      break;

    case ifttuple:
      if (tbl->UU.U2.stelemtype == x && tbl->UU.U2.stnext == y)
	found = true;
      else
	i++;
      break;

    case iftfunctiontype:
      if (tbl->UU.U3.starg == x && tbl->UU.U3.stres == y)
	found = true;
      else
	i++;
      break;
    }/* case */
  }  /* while */
  if (found)
    return tbl;
  else
    return NULL;
}  /* SearchTypeTable */


Static Void addfunargtype(f, arg)
stentry *f, *arg;
{
  stentry *x, *y;

  if (f->stsort != iftfunctiontype) {
    printf("ERROR: (AddFunArgType) first argument is not a function\n");
    return;
  }
  if (arg->stlabel <= 0) {
    printf("ERROR: (AddFunArgType) Arg Component not in type table\n");
    return;
  }
  x = newtypealloc(ifttuple);
  x->UU.U2.stelemtype = arg;
  y = f->UU.U3.starg;
  /* Now link X onto the end of the arg list */
  if (y == NULL) {
    f->UU.U3.starg = x;   /*first in list*/
    return;
  }
  while (y->UU.U2.stnext != NULL)
    y = y->UU.U2.stnext;
  y->UU.U2.stnext = x;
}  /* AddFunArgType */


Static Void addfunrestype(f, res)
stentry *f, *res;
{
  stentry *x, *y;

  if (f->stsort != iftfunctiontype) {
    printf("ERROR: (AddFunResType) first argument is not a function\n");
    return;
  }
  if (res->stlabel <= 0) {
    printf("ERROR: (AddFunResType) Res. Component not in type table\n");
    return;
  }
  x = newtypealloc(ifttuple);
  x->UU.U2.stelemtype = res;
  y = f->UU.U3.stres;
  /* Now link X onto the end of R's list */
  if (y == NULL) {
    f->UU.U3.stres = x;   /*first in list*/
    return;
  }
  while (y->UU.U2.stnext != NULL)
    y = y->UU.U2.stnext;
  y->UU.U2.stnext = x;
}  /* AddFunResType */


/* The maximum number of Equivalence classes handled */

#define maxclass        1000


/* Local variables for smashtypes: */
struct LOC_smashtypes {
  int lastclass;
  stentry *classtable[maxclass + 1];
  stentry *lastinclass[maxclass + 1];
} ;

Local int equivclass(s, LINK)
stentry *s;
struct LOC_smashtypes *LINK;
{
  /* EquivClass */
  if (s != NULL)
    return (s->stid);
  else
    return (-SHORT_MAX);
}

Local boolean sameequivclass(a, b, LINK)
stentry *a, *b;
struct LOC_smashtypes *LINK;
{
  boolean Result, first;

  /* SameEquivClass */
  switch (a->stsort) {   /* case */

  case iftwild:
    Result = true;
    break;

  case iftbasic:
    Result = (a->UU.stbasic == b->UU.stbasic);
    break;

  case iftfunctiontype:
    Result = (equivclass(a->UU.U3.starg, LINK) == equivclass(b->UU.U3.starg,
		LINK)) & (equivclass(a->UU.U3.stres, LINK) ==
			  equivclass(b->UU.U3.stres, LINK));
    break;

  case iftarray:
  case iftstream:
  case iftmultiple:
  case iftrecord:
  case iftunion:
  case iftbuffer:
    Result = (equivclass(a->UU.stbasetype, LINK) ==
	      equivclass(b->UU.stbasetype, LINK));
    break;

  case iftfield:
  case ifttag:
    if (equivclass(a->UU.U2.stelemtype, LINK) ==
	equivclass(b->UU.U2.stelemtype, LINK)) {
      if (a->UU.U2.stnext != NULL && b->UU.U2.stnext != NULL)
	first = (equivclass(a->UU.U2.stnext, LINK) ==
		 equivclass(b->UU.U2.stnext, LINK));
      else
	first = (a->UU.U2.stnext == NULL && b->UU.U2.stnext == NULL);
    } else
      first = false;
    Result = first & equalstrings(&a->stliteral, &b->stliteral);
    break;

  case ifttuple:
    if (equivclass(a->UU.U2.stelemtype, LINK) ==
	equivclass(b->UU.U2.stelemtype, LINK)) {
      if (a->UU.U2.stnext != NULL && b->UU.U2.stnext != NULL)
	Result = (equivclass(a->UU.U2.stnext, LINK) ==
		  equivclass(b->UU.U2.stnext, LINK));
      else
	Result = (a->UU.U2.stnext == NULL && b->UU.U2.stnext == NULL);
    } else
      Result = false;
    break;
  }
  return Result;
}

Local Void createnewequivclass(r, LINK)
stentry *r;
struct LOC_smashtypes *LINK;
{
  /* CreateNewEquivClass */
  if (LINK->lastclass < maxclass)
    LINK->lastclass++;
  else
    printf("SmashType: Out of room for equivalence classes\n");
  r->stid = LINK->lastclass;
  r->stequivchain = NULL;
  LINK->classtable[LINK->lastclass] = r;
  LINK->lastinclass[LINK->lastclass] = r;
}

Local Void removefromequivclass(old, exile, LINK)
stentry *old, *exile;
struct LOC_smashtypes *LINK;
{
  /* RemoveFromEquivClass */
  if (exile == LINK->lastinclass[old->stid])
    LINK->lastinclass[old->stid] = old;
  old->stequivchain = exile->stequivchain;
}

Local Void addtoequivclass(representative, newmember, LINK)
stentry *representative, *newmember;
struct LOC_smashtypes *LINK;
{
  int classnum;

  /* AddToEquivClass */
  classnum = representative->stid;
  newmember->stid = classnum;
  newmember->stequivchain = NULL;
  LINK->lastinclass[classnum]->stequivchain = newmember;
  LINK->lastinclass[classnum] = newmember;
}

Local Void initequivclasses(LINK)
struct LOC_smashtypes *LINK;
{
  /* InitEquivClasses looks at every entry in the type table and places*/
  /* then non-nil ones into one of 12 equivilence classes:*/
  /*IFTBasicIFTRecord*/
  /*IFTFunctionTypeIFTUnion*/
  /*IFTArrayIFTField*/
  /*IFTStreamIFTTuple*/
  /*IFTMultipleIFTTag*/
  /*IFTWildIFTBuffer*/
  /* WARNING: Some equivalence classes may be left empty! */
  /* imports TypeTable, EntryMax*/
  /* exports LastClass (set to highest class USED */
  stentry *s;
  int entry_, class_, FORLIM;

  /* InitEquivClasses */
  for (class_ = 1; class_ <= maxclass; class_++) {
    LINK->classtable[class_] = NULL;
    LINK->lastinclass[class_] = NULL;
  }
  FORLIM = tthwm;
  for (entry_ = 1; entry_ <= FORLIM; entry_++) {
    if (typetable[entry_ - 1] != NULL) {
      s = typetable[entry_ - 1];
      s->stlabel = entry_;
      s->stid = s->stsort + 1;
      s->stequivchain = NULL;
      if (LINK->classtable[s->stid] == NULL) {  /* First in list */
	LINK->classtable[s->stid] = s;
	LINK->lastinclass[s->stid] = s;
      } else {  /* Insert at end of list */
	LINK->lastinclass[s->stid]->stequivchain = s;
	LINK->lastinclass[s->stid] = s;
      }
    }
  }
  for (class_ = 1; class_ <= ifmaxtype + 1; class_++) {
    if (LINK->classtable[class_] != NULL)
      LINK->lastclass = class_;
  }
}

/* macro function NextInEquivClass( M : STPtr ) : STPtr; */
/* macro function MoreInEquivClass( M : STPtr ) : boolean; */
Local Void gatherothers(prev, representative, LINK)
stentry *prev, **representative;
struct LOC_smashtypes *LINK;
{
  /* GatherOthers tries to remove elements in the Class headed by */
  /* Prev and moves them to the class headed by Representative*/
  stentry *potentialmember;

  /* macro procedure Mark( Entry : STPtr ); */
  /* macro function IsMarked( Entry : STPtr ): boolean; */
  /* macro procedure RemoveMark( var Entry : STPtr ); */
  /* GatherOthers */
  /* ^.STLabel := -^.STLabel all entries equivalent to Representative */
  potentialmember = (*representative)->stequivchain;
  while (potentialmember != NULL) {
    if (sameequivclass(*representative, potentialmember, LINK))
      potentialmember->stlabel = -potentialmember->stlabel;
    potentialmember = potentialmember->stequivchain;
  }
  /* remove all marked entries */
  removefromequivclass(prev, *representative, LINK);
  createnewequivclass(*representative, LINK);
  while (prev->stequivchain != NULL) {
    potentialmember = prev->stequivchain;
    if (potentialmember->stlabel < 0) {
      potentialmember->stlabel = -potentialmember->stlabel;
      removefromequivclass(prev, potentialmember, LINK);
      addtoequivclass(*representative, potentialmember, LINK);
    } else
      prev = potentialmember;
  }
}

Local Void pointtohead(LINK)
struct LOC_smashtypes *LINK;
{
  /* PointToHead makes the STEquivChain of each entry point at the */
  /*  representative of the equivalence class, and places a -1*/
  /*  in the representative's STId field (so that it will be*/
  /*  dumped in the node dumping process*/
  /* 83/10/4  sks*/
  int thisclass;
  stentry *temp, *member, *representative;
  int FORLIM;
  stentry *WITH;

  /* PointToHead */
  FORLIM = LINK->lastclass;
  for (thisclass = 1; thisclass <= FORLIM; thisclass++) {
    if (LINK->classtable[thisclass] != NULL) {
      representative = LINK->classtable[thisclass];
      representative->stid = -1;
      member = representative;
      while (member != NULL) {
	WITH = member;
	temp = WITH->stequivchain;
	WITH->stequivchain = representative;
	member = temp;   /* with */
      }
    }  /* if */
  }
}


/* -------------------------------------------------------------------- */
/* --------------------------- SmashTypes ----------------------------- */
Static Void smashtypes()
{
  /* Smashtypes maps types that are structurally equivalent into the same*/
  /* type number.Note that SISAL type equivalence is not guaranteed, since */
  /* the names of fields and tags of smashed types may not be the same.*/
  /* imports:TypeTable*/
  /* exports:LastClass*/
  /*ClassTable*/
  /*.STEquivChain*/
  /*.STLabel*/
  /*.STId*/
  /* 83/10/1 sks*/
  struct LOC_smashtypes V;
  boolean changed;
  int class_;
  stentry *member, *previous, *representative;
  int FORLIM;

  /* SmashTypes */
  initequivclasses(&V);
  changed = true;
  while (changed) {   /* while */
    changed = false;
    FORLIM = V.lastclass;
    for (class_ = 1; class_ <= FORLIM; class_++) {
      representative = V.classtable[class_];
      previous = representative;
      if (previous == NULL)
	member = NULL;
      else
	member = previous->stequivchain;
      while (member != NULL) {
	if (sameequivclass(representative, member, &V)) {
	  previous = member;
	  member = previous->stequivchain;
	} else {
	  /* Begin another class and grab all others */
	  /* from this one that belong in it */
	  changed = true;
	  gatherothers(previous, &member, &V);
	  member = NULL;
	}
      }  /* while Member<>nil */
    }  /* for */
  }
  pointtohead(&V);
}

#undef maxclass


/* --------------------------- SmashTypes ----------------------------- */
/* -------------------------------------------------------------------- */
Static Void adjustgraphptrs()
{
  int typenum, FORLIM;
  stentry *WITH;

  FORLIM = tthwm;
  for (typenum = 0; typenum < FORLIM; typenum++) {
    if (typetable[typenum] != NULL) {
      WITH = typetable[typenum];
      switch (WITH->stsort) {

      case iftwild:
      case iftbasic:
	/* blank case */
	break;

      case iftfunctiontype:
	if (WITH->UU.U3.starg != NULL)
	  WITH->UU.U3.starg = WITH->UU.U3.starg->stequivchain;
	if (WITH->UU.U3.stres != NULL)
	  WITH->UU.U3.stres = WITH->UU.U3.stres->stequivchain;
	break;

      case iftarray:
      case iftstream:
      case iftmultiple:
      case iftrecord:
      case iftunion:
      case iftbuffer:
	if (WITH->UU.stbasetype != NULL)
	  WITH->UU.stbasetype = WITH->UU.stbasetype->stequivchain;
	break;

      case iftfield:
      case ifttuple:
      case ifttag:
	if (WITH->UU.U2.stelemtype != NULL)
	  WITH->UU.U2.stelemtype = WITH->UU.U2.stelemtype->stequivchain;
	if (WITH->UU.U2.stnext != NULL)
	  WITH->UU.U2.stnext = WITH->UU.U2.stnext->stequivchain;
	break;
      }/* case */
    }
  }
}  /* AdjustGraphPtrs */


Static Void compacttypetable(tthwm, oldtthwm)
int *tthwm, oldtthwm;
{
  /*  Pre:  The TypeTable has been extended beyond OldTTHWM (Old Symbol
           Table High Water Mark) and the new types added have been
           smashed into the existing types and some new ones.  At this
           point, the STEquivChain of each entry of the table points to
           its equivalence class representative.  Also, the new graph
           has been walked and all pointers to symbol table entries have
           been changed to the class representatives.
     Post: The Symbol Table below OldTTHWM is scanned and all entries
           which are not equivalence class representatives are removed.
           The resulting entries are compacted and the TTHWM is reset.
  */
  int lastincompacted, current;
  stentry *rep, *s;
  int FORLIM;

  lastincompacted = oldtthwm;
  FORLIM = *tthwm;
  for (current = oldtthwm; current < FORLIM; current++) {
    s = typetable[current];
    if (s != NULL) {
      if (s->stequivchain != s) {
	/* Make sure representative has name */
	rep = s->stequivchain;
	if (rep != NULL) {
	  if (rep->stliteral.len == 0) {
	    if (s->stliteral.len != 0)
	      rep->stliteral = s->stliteral;
	  }
	}
	/* Not a class representative, remove it */
	typetable[current] = NULL;
      } else {
	/* Is a class representative */
	lastincompacted++;
	if (current + 1 > lastincompacted) {
	  /* Compact list */
	  s->stlabel = lastincompacted;
	  typetable[lastincompacted - 1] = s;
	  typetable[current] = NULL;
	}
      }
    }
  }
  *tthwm = lastincompacted;
}  /* CompactTypeTable */


Static stentry *getbasictype(base)
char base;
{
  /* returns a pointer to the type table entry corresponding to
     the basic type specified by Base.  An entry MUST exist for
     all basic types, this is required by LoadProgram.
   */
  return (typetable[base]);
}  /* GetBasicType */


Static stentry *getwildtype()
{
  /* Returns a type table entry corresponding to the Wild Card type.
     Note that this is NOT the BASIC Wild Card (IFBWild) but the
     higher level wild card (IFTWild).
   */
  stentry *wld;
  stryng name;

  mymemcpy(name.str, blankstring, sizeof(stryngar));
  name.len = 0;
  wld = searchtypetable(iftwild, NULL, NULL, name);
  if (wld != NULL) {
    return wld;
  }  /* if */
  wld = newtypealloc(iftwild);
  if (tthwm == entrymax) {
    printf("ERROR: (GetWildType) Type Table Overflow\n");
    wld = NULL;
    return wld;
  }
  tthwm++;
  wld->stlabel = tthwm;
  typetable[tthwm - 1] = wld;
  return wld;

  /* else */
}  /* GetWildType */


Static stentry *getconstructortype(kind, base)
char kind;
stentry *base;
{
  /* Requires that the Base type already be in the table - if not,
     an error message is printed and Nil is returned.
     If the base is in the table, then it searches the table for
     this constructor.  If it finds one, it returns this type, otherwise
     it returns a newly constructed type and adds it to the table.
   */
  stentry *con;
  stryng name;

  if (((1L << kind) & ((1L << iftarray) | (1L << iftstream) |
		       (1L << iftmultiple) | (1L << iftbuffer))) == 0) {
    printf("ERROR (GetConstructorType) only works with: \n");
    printf(" --->  Arrays, Streams, and Multiples\n");
    return NULL;
  } else if (base->stlabel <= 0) {
    printf("ERROR (GetConstructorType) Base type not in table\n");
    return NULL;
  } else {
    /* Base is in type table, search for Array[ Base ] or
       Stream[ Base ], or Multiple[ Base ] or Buffer[ Base ] entry */
    mymemcpy(name.str, blankstring, sizeof(stryngar));
    name.len = 0;
    con = searchtypetable(kind, base, NULL, name);
    if (con != NULL)  /* Not in table, construct one and add it */
      return con;
    con = (stentry *)Malloc(sizeof(stentry));  /* CANN: ??? */ 
    con = newtypealloc(kind);
    con->UU.stbasetype = base;
    if (tthwm == entrymax) {
      printf("ERROR: (GetConstructorType) Type Table Overflow\n");
      return con;
    }
    tthwm++;
    con->stlabel = tthwm;
    typetable[tthwm - 1] = con;
    return con;
  }

  /* else */
}  /* GetConstructorType */


Static Void addfieldtype(r, fld)
stentry *r, *fld;
{
  /* It is required that Fld already be in the type table.
     If its not, an error message is posted and nothing is done.
     If it is, a field link is constructed and added to the
     end of R's field list.
   */
  stentry *x, *y;

  if (r->stsort != iftrecord) {
    printf("ERROR: (AddFieldType) first argument is not a record\n");
    return;
  }
  if (fld->stlabel <= 0) {
    printf("ERROR: (AddFieldType) Field Component not in type table\n");
    return;
  }
  x = newtypealloc(iftfield);
  x->UU.U2.stelemtype = fld;
  y = r->UU.stbasetype;
  /* Now link X onto the end of R's list */
  if (y == NULL) {
    r->UU.stbasetype = x;   /*first in list*/
    return;
  }
  while (y->UU.U2.stnext != NULL)
    y = y->UU.U2.stnext;
  y->UU.U2.stnext = x;
}  /* AddFieldType */


Static Void addtagtype(u, tg)
stentry *u, *tg;
{
  /* It is required that Tg already be in the type table.
     If its not, an error message is posted and nothing is done.
     If it is, a field link is constructed and added to the
     end of U's field list.
   */
  stentry *x, *y;

  if (u->stsort != iftunion) {
    printf("ERROR: (AddTagType) first argument is not a union\n");
    return;
  }
  if (tg->stlabel <= 0) {
    printf("ERROR: (AddTagType) Tag Component not in type table\n");
    return;
  }
  x = newtypealloc(ifttag);
  x->UU.U2.stelemtype = tg;
  y = u->UU.stbasetype;
  /* Now link X onto the end of U's list */
  if (y == NULL) {
    u->UU.stbasetype = x;   /*first in list*/
    return;
  }
  while (y->UU.U2.stnext != NULL)
    y = y->UU.U2.stnext;
  y->UU.U2.stnext = x;
}  /* AddTagType */


Static Void putintable(typ)
stentry **typ;
{
  /* Enter Typ into the type table.
     If no room is left, send an error message
   */
  if (tthwm == entrymax) {
    printf("ERROR: (AddToTypeTable) Type Table Overflow\n");
    *typ = NULL;
    return;
  }
  tthwm++;
  (*typ)->stlabel = tthwm;
  typetable[tthwm - 1] = *typ;
}  /* PutInTable */


Local Void putstruct(tt)
stentry *tt;
{
  if (tt == NULL) {
    return;
  }  /* if TT <> nil */
  if (tt->stlabel != -1) {
    return;
  }  /* if */
  putintable(&tt);
  switch (tt->stsort) {

  case iftbasic:
  case iftwild:
    /* blank case */
    break;

  case iftfunctiontype:
    putstruct(tt->UU.U3.starg);
    putstruct(tt->UU.U3.stres);
    break;

  case ifttuple:
  case iftfield:
  case ifttag:
    if (!tt->strecurflag)
      putstruct(tt->UU.U2.stelemtype);
    putstruct(tt->UU.U2.stnext);
    break;

  case iftarray:
  case iftstream:
  case iftbuffer:
  case iftmultiple:
    if (!tt->strecurflag)
      putstruct(tt->UU.stbasetype);
    break;

  case iftrecord:
  case iftunion:
    putstruct(tt->UU.stbasetype);
    break;
  }/* case */
}  /* PutStruct */


Static stentry *addtotypetable(typ)
stentry *typ;
{
  stentry *Result, *newtyp, *elem, *nxt, *arg, *rets;
  stryng name;
  int oldtthwm;

  if (typ == NULL)
    return NULL;
  if (typ->stlabel > 0)
    return typ;
  mymemcpy(name.str, blankstring, sizeof(stryngar));
  name.len = 0;
  switch (typ->stsort) {

  case iftbasic:
    Result = searchtypetable(typ->stsort, NULL, NULL, name);
    break;

  case iftfunctiontype:
    if (typ->UU.U3.starg == NULL)
      arg = NULL;
    else
      arg = addtotypetable(typ->UU.U3.starg);
    if (typ->UU.U3.stres == NULL)
      rets = NULL;
    else
      rets = addtotypetable(typ->UU.U3.stres);
    newtyp = searchtypetable(typ->stsort, arg, rets, name);
    if (newtyp == NULL) {
      newtyp = typ;
      newtyp->UU.U3.starg = arg;
      newtyp->UU.U3.stres = rets;
      putintable(&newtyp);
    }
    Result = newtyp;
    break;

  case iftarray:
  case iftstream:
  case iftmultiple:
  case iftbuffer:
    /* this should never happen, all arrays, streams,
            and tuples should already be built into the table */
    elem = addtotypetable(typ->UU.stbasetype);
    newtyp = searchtypetable(typ->stsort, elem, NULL, name);
    if (newtyp == NULL) {  /* not in the table */
      newtyp = typ;
      newtyp->UU.stbasetype = elem;
      putintable(&newtyp);
    }
    Result = newtyp;
    break;

  case iftrecord:
    if (typ->UU.stbasetype == NULL)
      nxt = NULL;
    else
      nxt = addtotypetable(typ->UU.stbasetype);
    newtyp = searchtypetable(typ->stsort, nxt, NULL, name);
    if (newtyp == NULL) {  /* Not found in table */
      newtyp = typ;
      newtyp->UU.stbasetype = nxt;
      putintable(&newtyp);
    }
    Result = newtyp;
    break;

  case iftunion:
    if (typ->strecurflag) {
      oldtthwm = tthwm;
      putstruct(typ);
      smashtypes();
      adjustgraphptrs();
      Result = typetable[oldtthwm]->stequivchain;
      compacttypetable(&tthwm, oldtthwm);
    } else {
      if (typ->UU.stbasetype == NULL)
	nxt = NULL;
      else
	nxt = addtotypetable(typ->UU.stbasetype);
      newtyp = searchtypetable(typ->stsort, nxt, NULL, name);
      if (newtyp == NULL) {
	newtyp = typ;
	newtyp->UU.stbasetype = nxt;
	putintable(&newtyp);
      }
      Result = newtyp;
    }
    break;

  case ifttuple:
    elem = addtotypetable(typ->UU.U2.stelemtype);
    if (typ->UU.U2.stnext != NULL)
      nxt = addtotypetable(typ->UU.U2.stnext);
    else
      nxt = NULL;
    newtyp = searchtypetable(typ->stsort, elem, nxt, name);
    if (newtyp == NULL) {  /* not found in table */
      newtyp = typ;
      newtyp->UU.U2.stnext = nxt;
      newtyp->UU.U2.stelemtype = elem;
      putintable(&newtyp);
    }
    Result = newtyp;
    break;

  case ifttag:
  case iftfield:
    elem = addtotypetable(typ->UU.U2.stelemtype);
    if (typ->UU.U2.stnext != NULL)
      nxt = addtotypetable(typ->UU.U2.stnext);
    else
      nxt = NULL;
    newtyp = searchtypetable(typ->stsort, elem, nxt, typ->stliteral);
    if (newtyp == NULL) {  /* not found in table */
      newtyp = typ;
      newtyp->UU.U2.stnext = nxt;
      newtyp->UU.U2.stelemtype = elem;
      putintable(&newtyp);
    }
    Result = newtyp;
    break;
  }/* case */
  return Result;
}  /* AddToTypeTable */


Static stentry *makefuntypefromgraph(g)
node *g;
{
  /* Construct a function type definition for the given graph
     and add the definition to the global type table.
     NOTE:  Unused ports get defined as wild types.
   */
  stentry *ft, *arg, *res;
  int port_;
  port *e;
  int FORLIM;

  ft = newtypealloc(iftfunctiontype);
  FORLIM = largestinputportnumber(g);
  for (port_ = 1; port_ <= FORLIM; port_++) {
    e = getinputedge(g, port_);
    if (e == NULL)
      res = getwildtype();
    else
      res = e->pttype;
    addfunrestype(ft, res);
  }
  FORLIM = largestoutputportnumber(g);
  for (port_ = 1; port_ <= FORLIM; port_++) {
    e = getoutputedge(g, port_);
    if (e == NULL)
      arg = getwildtype();
    else
      arg = e->pttype;
    addfunargtype(ft, arg);
  }
  return (addtotypetable(ft));
}  /* MakeFunTypeFromGraph */


/* Functions that read and set pragma values */
/* GetEdgeBounds( E : EGPtr; var Lo, Hi : integer );   Assumes E <> nil */
/* macro function GetSourceLine( N: NDPtr ) : integer assumes N <> nil */
/* macro function GetWithinLine ( N : NDPtr) : integer; Assumes N <> nil */
/* macro function EdgeMark( E: EGPtr ) : EGMark */
/* macro procedure SetEdgeMark( var E: EGPtr ; M: EGMark */
/* macro function IsInlineExpandable( fungraph: NDPtr) : boolean; */
/* Assumes fungraph <> nil and fungraph is a function graph */
/* macro procedure SetInlinePragma( fungraph: NDPtr; V: boolean ); */
/* Assumes fungraph <> nil and fungraph is a function graph */
/* Functions needed specifically by the interpreter (DI) */
Static Void linkcallnodetofunction(cnode, fgraph)
node *cnode, *fgraph;
{
  /* Assumes Cnode and Fgraph <> nil */
  if (fgraph == NULL)
    cnode->UU.U1.ndlink = NULL;
  else
    cnode->UU.U1.ndlink = fgraph->UU.U1.ndlink;
}  /* LinkCallNodeToFunction */


Static node *tocalledgraph(callnode)
node *callnode;
{
  if (callnode->UU.U1.ndlink == NULL)
    return NULL;
  else
    return (callnode->UU.U1.ndlink->lkgraph);
}  /* ToCalledGraph */


/* macro function NumbFunLocals( F: NdPtr ) : integer */
/* NumbFunLocals assumes funnode is the graph node of a function */
/* macro procedure SetNumbFunLocals( F: NdPtr, n: integer ) */
/* SetNumbFunLocals assumes funnode is the graph node of a function */
/* macro function DatumAddr ( E : EGPtr) : integer; */
/* Assumes E is not nil */
/* macro procedure SetDatumAddr( E: EGPtr, n: integer ) */
/* SetDatumAddr assumes E is not nil */
/* macro function MRDatumAddr ( E : EGPtr ) : integer */
/* macro procedure SetMRDatumAddr( E : EGPtr, N : integer ) */
/* ^.NDLink^.LKARIndex :=  assumes F <> nil and F points to a function graph */
/* ^.NDLink^.LKARIndex assumes F <> nil and F points to a function graph */
/* macro function NextSimilarEdge( E : EGPtr ) : EGPtr; */
/* macro procedure LinkToSimilarEdge( List, NewLink : EGPtr ); */
/* macro function NextNodeInLine( N : NDPtr ) : NDPtr; */
/* macro procedure LinkToNextNodeInLine( List, NewLink : NDPtr ); */
/* macro procedure SetFunctionModule( F : NDPtr; Name : Stryng ); */
/* macro procedure ModuleNameOfFunction( var Name : Stryng; F : NDPtr); */
/******************************************************************/
/* These function must come last in the files due to dependencies */
Static ifgraphtype graphkind(g)
node *g;
{
  /* Assumes (G^.NDSort = NDGraph)
     Analyses a graph node to determine its IFGraphType */
  ifgraphtype Result;
  node *parent;
  graph *gptr;
  int count;

  parent = toenclosingcompound(g);
  if (parent == NULL)
    return ifgfunction;
  switch (parent->ndcode) {

  case ifnselect:
    if (g == toselectorgraph(parent))
      Result = ifgselector;
    else
      Result = ifgalternative;
    break;

  case ifntagcase:
    Result = ifgvariant;
    break;

  case ifnloopb:
    if (g == toinitgraph(parent))
      Result = ifgloopbinit;
    else if (g == totestgraph(parent))
      Result = ifgloopbtest;
    else if (g == tobodygraph(parent))
      Result = ifgloopbbody;
    else
      Result = ifgloopbreturns;
    break;

  case ifnloopa:
    if (g == toreturnsgraph(parent))
      Result = ifgloopareturns;
    else if (g == toinitgraph(parent))
      Result = ifgloopainit;
    else if (g == totestgraph(parent))
      Result = ifgloopatest;
    else
      Result = ifgloopabody;
    break;

  case ifnforall:
    gptr = parent->UU.U2.ndsubsid;
    if (g == gptr->grnode)
      Result = ifgforallgenerator;
    else if (g == gptr->grnext->grnode)
      Result = ifgforallbody;
    else
      Result = ifgforallreturns;
    break;
    /* IFNForall */

  case ifnifthenelse:
    gptr = parent->UU.U2.ndsubsid;
    count = 1;
    while (gptr->grnode != g) {
      gptr = gptr->grnext;
      count++;
    }
    if (gptr->grnext == NULL)
      Result = ifgiffalse;
    else if ((count & 1) == 0)
      Result = ifgifpredicate;
    else
      Result = ifgiftrue;
    break;

  case ifniter:
    Result = ifgiterbody;
    break;
  }/* case */
  return Result;
}  /* GraphKind */


Static char reductionop(n)
node *n;
{
  /* assumes N <> nil, N is a reduction node, and
     first input edge is a literal string of the reduction
     operators name */
  char Result;
  port *e;

  e = getinputedge(n, 1);
  switch (lowercase(e->UU.ptlitvalue.str[0])) {

  case 's':
    Result = ifrsum;
    break;

  case 'p':
    Result = ifrproduct;
    break;

  case 'l':
    Result = ifrleast;
    break;

  case 'g':
    Result = ifrgreatest;
    break;

  case 'c':
    Result = ifrcatenate;
    break;
  }/* case */
  return Result;
}  /* ReductionOp */


Static Void loopranges(c, k, l, t)
node *c;
int *k, *l, *t;
{
  /*
    Assumes C <> nil, returns:
      K = number of input ports on node C, these ports are numbered
          1 .. K.
      L = Largest Loop value port number.  Loop ports are numbered
          K + 1 .. L.
          ( NOTE:  if C is not a loop compound node then -1 is returned )
      T = Largest Temporary value port number.Temporary ports are
          numbered L+1 .. T.
          ( NOTE: -1 is returned if C is not an LoopA or Forall node )
  */
  node *g;

  *k = largestinputportnumber(c);
  if (c->ndsort != ndcompound) {
    *l = -1;
    *t = -1;
    return;
  }
  switch (c->ndcode) {

  case ifnselect:
  case ifntagcase:
  case ifnifthenelse:
    *l = -1;
    *t = -1;
    break;

  case ifnforall:
    g = toforallgeneratorgraph(c);
    *l = largestinputportnumber(g);
    if (*l == 0)
      *l = *k;
    g = toforallbodygraph(c);
    *t = largestinputportnumber(g);
    if (*t == 0)
      *t = *l;
    break;

  case ifnloopb:
  case ifnloopa:
    g = toinitgraph(c);
    *l = largestinputportnumber(g);
    if (*l == 0)
      *l = *k;
    g = tobodygraph(c);
    *t = largestinputportnumber(g);
    if (*t == 0)
      *t = *l;
    break;

  case ifniter:   /* added rky 8/87 */
    g = c->UU.U2.ndsubsid->grnode;
    *l = largestinputportnumber(g);
    *t = -1;   /* there are no Ts */
    /* difference between K and L is that K not hooked as input to Body */
    *k = g->ndilist->pttonext->pttoport - 1;
    break;
  }/* case */
}  /* LoopRanges */


Static int fanout(n, portnum)
node *n;
int portnum;
{
  /* Returns the number of edges connected to output port number
     'portnum'. A value of zero is returned if no such port exists */
  int count;
  port *eptr;

  count = 0;
  /* Find first port with this port number */
  eptr = getoutputedge(n, portnum);
  while (eptr != NULL) {
    count++;
    eptr = nextoutputedgesameport(eptr);
  }
  return count;
}  /* Fanout */


/* Graph2.m4 contains graph routines that alter the structure of the
   graph and are only used by some of the optimization routines.
   These were removed from the graph.m4 file since they are "Special"
   and not everyone needs them. */
/*dlz - 8/87 updated to use new allocation routines in graph.m4,
  some cleanup, ***still confusion/glitch (?) in CopyEdges, CopyGraph*/
Static Void disconnectedgefromsource(e)
port *e;
{
  /*in out*/
  /* ----------------------------------------------------
     DisconnectEdgeFromSource
          - Completely unlinks E from it's source node's
            output edge list
          - nills out all pointer fields having to do with
            it's source node.
     ---------------------------------------------------- */
  port *temp;

  if (e->ptsort != ptedge)
    return;
  if (e->UU.U1.ptfrnode == NULL)
    return;
  temp = e->UU.U1.ptfrnode->ndolist;
  if (temp == e)
    e->UU.U1.ptfrnode->ndolist = e->UU.U1.ptfrnext;
  else {
    while (temp->UU.U1.ptfrnext != e)
      temp = temp->UU.U1.ptfrnext;
    temp->UU.U1.ptfrnext = e->UU.U1.ptfrnext;
  }
  e->UU.U1.ptfrnode = NULL;
  e->UU.U1.ptfrnext = NULL;
  e->UU.U1.ptfrport = 0;
}  /* DisconnectEdgeFromSource */


Static Void disconnectedgefromdest(e)
port *e;
{
  /*in out*/
  /* -------------------------------------------------------
     DisconnectEdgeFromDest
         - Unlinks E from it's destination node's input list
         - nills out all pointer fields having to do with
           E's destination node
     ------------------------------------------------------- */
  port *temp;

  if (e->pttonode == NULL)
    return;
  temp = e->pttonode->ndilist;
  if (temp == e)
    e->pttonode->ndilist = e->pttonext;
  else {
    while (temp->pttonext != e)
      temp = temp->pttonext;
    temp->pttonext = e->pttonext;
  }
  e->pttonode = NULL;
  e->pttonext = NULL;
  e->pttoport = 0;
}  /* DisconnectEdgeFromDest */


Static Void removeedge(e)
port **e;
{
  /* -------------------------------------------------
     RemoveEdge
         - Removes E from graph leaving graph valid
         - nills out all pointer fields
         - returns pointer to completely unlinked edge
     ------------------------------------------------- */
  disconnectedgefromsource(*e);
  disconnectedgefromdest(*e);
  (*e)->pttype = NULL;
}  /* RemoveEdge */


Static Void removeinputedges(n)
node *n;
{
  port *e, *tmpe;

  e = n->ndilist;
  while (e != NULL) {
    tmpe = e;
    e = e->pttonext;
    removeedge(&tmpe);
  }
}  /* RemoveInputEdges */


extern Void removenode PP((node *n));

Local Void removegraph(g)
node *g;
{
  /* Walk the nodes of the graph G removing each in turn */
  node *tmpn, *nd;

  nd = g->ndnext;
  while (nd != NULL) {
    tmpn = nd;
    nd = nd->ndnext;
    removenode(tmpn);
  }
  removenode(g);
}  /* RemoveGraph */


/* macro procedure ChangeEdgeName( E : EGPtr; S : Stryng ) */
/* macro procedure ChangeLiteralValue( E : EGPtr; S : Stryng ) */
Static Void removenode(n)
node *n;
{
  /*  Remove Node :
        - if N is a compound node it removes the subgraphs first.
        - removes all input edges
        - removes all output edges
        - completely disconnects N from the graph
  */
  graph *grtmp;
  node *prev;
  port *temp;

  if (n->ndsort == ndcompound) {
    grtmp = n->UU.U2.ndsubsid;
    while (grtmp != NULL) {
      removegraph(grtmp->grnode);
      grtmp = grtmp->grnext;
    }
  }
  /* Remove input edges */
  while (n->ndilist != NULL) {
    temp = n->ndilist;
    removeedge(&temp);
  }
  /* Remove output edges */
  while (n->ndolist != NULL) {
    temp = n->ndolist;
    removeedge(&temp);
  }
  /* Remove the node from the graph */
  if (n->ndsort != ndgraph) {
    prev = n->ndparent;
    while (prev->ndnext != n)
      prev = prev->ndnext;
    prev->ndnext = n->ndnext;
  }
  n->UU.U2.ndsubsid = NULL;
  n->ndnext = NULL;
  n->ndparent = NULL;
}  /* RemoveNode */


Static node *createsimplenode(opcode)
int opcode;
{
  /* Create a Simple node with the opcode given.
     The new node will have no input or output edges and will
     not be connected to any graph.
   */
  node *newnode;

  newnode = newnodealloc(ndatomic);
  univnodecnt++;
  newnode->ndid = univnodecnt;
  newnode->ndcode = opcode;
  return newnode;
}  /* CreateSimpleNode */


Static node *createcompoundnode(opcode)
int opcode;
{
  /* Create a Compound node with the opcode given.
     The new node will have no input or output edges and will
     not be connected to any graph.
   */
  node *newnode;

  newnode = newnodealloc(ndcompound);
  univnodecnt++;
  newnode->ndid = univnodecnt;
  newnode->ndcode = opcode;
  return newnode;
}  /* CreateCompoundNode */


Static Void insertnode(parentnode, prevnode, newnode)
node *parentnode, *prevnode, *newnode;
{
  /* Inserts NewNode into graph of ParentNode after PrevNode.
     If Prevnode = nil then the NewNode is inserted as the first
     in the graph headed by ParentNode.
   */
  node *ntemp, *WITH;

  newnode->ndparent = parentnode;
  if (prevnode == NULL) {   /* with */
    newnode->ndnext = parentnode->ndnext;
    parentnode->ndnext = newnode;
    if (newnode->ndnext == NULL)
      newnode->ndlabel = 1;
    else
      newnode->ndlabel = newnode->ndnext->ndlabel;
  } else {
    newnode->ndnext = prevnode->ndnext;
    prevnode->ndnext = newnode;
    newnode->ndlabel = prevnode->ndlabel + 1;
  }
  /* Now change the Labels of all nodes following NewNode */
  ntemp = newnode->ndnext;
  while (ntemp != NULL) {
    WITH = ntemp;
    WITH->ndlabel++;
    ntemp = WITH->ndnext;
  }
}  /* InsertNode*/


Static Void changeedgedest(e, newdestnode, newdestport)
port *e;
node *newdestnode;
int newdestport;
{
  /* -----------------------------------------------------------------
     Step 1  Removes E from it's current destination node's input list
             ( if it is in one) and
     Step 2  Adds it to NewDestNode's input list at port number
             NewDestPort
     NOTE:   This operation may cause Fan-In.
     ----------------------------------------------------------------- */
  port *nedge;
  boolean found;

  disconnectedgefromdest(e);
  /* Now Link E into input edge list of NewDestNode */
  e->pttonode = newdestnode;
  e->pttoport = newdestport;
  nedge = newdestnode->ndilist;
  if (nedge == NULL) {  /* List was empty, adding first edge */
    newdestnode->ndilist = e;
    e->pttonext = NULL;
    return;
  }
  if (nedge->pttoport >= newdestport) {  /* Insert first in list */
    e->pttonext = newdestnode->ndilist;
    newdestnode->ndilist = e;
    return;
  }
  found = false;
  do {
    if (nedge->pttonext == NULL)
      found = true;
    else if (nedge->pttonext->pttoport >= newdestport)
      found = true;
    else
      nedge = nedge->pttonext;
  } while (!found);
  /* insert after Nedge */
  e->pttonext = nedge->pttonext;
  nedge->pttonext = e;
}  /* ChangeEdgeDest */


Static Void changeedgesrc(e, newsrcnode, newsrcport)
port *e;
node *newsrcnode;
int newsrcport;
{
  /* -----------------------------------------------------------------
     Step 1  Removes E from it's current source node's output list
             ( if it is in one) and
     Step 2  Adds it to NewSrcNode's input list at port number
             NewSrcPort
     ----------------------------------------------------------------- */
  port *nedge;
  boolean found;

  disconnectedgefromsource(e);
  /* Now Link E into output edge list of NewSrcNode */
  e->UU.U1.ptfrnode = newsrcnode;
  e->UU.U1.ptfrport = newsrcport;
  nedge = newsrcnode->ndolist;
  if (nedge == NULL) {  /* List was empty, adding first edge */
    newsrcnode->ndolist = e;
    e->UU.U1.ptfrnext = NULL;
    return;
  }
  if (nedge->UU.U1.ptfrport >= newsrcport) {  /* Insert first in list */
    e->UU.U1.ptfrnext = newsrcnode->ndolist;
    newsrcnode->ndolist = e;
    return;
  }
  found = false;
  do {
    if (nedge->UU.U1.ptfrnext == NULL)
      found = true;
    else if (nedge->UU.U1.ptfrnext->UU.U1.ptfrport >= newsrcport)
      found = true;
    else
      nedge = nedge->UU.U1.ptfrnext;
  } while (!found);
  /* insert after Nedge */
  e->UU.U1.ptfrnext = nedge->UU.U1.ptfrnext;
  nedge->UU.U1.ptfrnext = e;
}  /* ChangeEdgeSrc */


Static port *insertedge(fromnode, fromport, tonode, toport, etype, name)
node *fromnode;
int fromport;
node *tonode;
int toport;
stentry *etype;
stryng name;
{  /* Does NOT work for inserting Literals */
  port *newedge;

  newedge = newedgealloc(ptedge);
  univedgecnt++;
  newedge->ptid = univedgecnt;
  newedge->ptname = name;
  newedge->pttype = etype;   /* with */
  changeedgedest(newedge, tonode, toport);
  changeedgesrc(newedge, fromnode, fromport);
  return newedge;
}  /* InsertEdge */


Static port *insertliteral(tonode, toport, etype, lvalue, lname)
node *tonode;
int toport;
stentry *etype;
stryng lvalue, lname;
{
  port *newedge;

  newedge = newedgealloc(ptlit);
  univedgecnt++;
  newedge->ptid = univedgecnt;
  newedge->ptname = lname;
  newedge->UU.ptlitvalue = lvalue;
  newedge->pttype = etype;   /* with */
  changeedgedest(newedge, tonode, toport);
  return newedge;
}  /* InsertLiteral */


Static Void renumbergraph(g, recurse)
node *g;
boolean recurse;
{
  /* Walks the graph G renumbering then nodes as it comes to them.
     If Recurse = true then it recursively renumbers the
     subgraphs of the compound nodes of G otherwise, it simple
     renumbers the nodes of G.
   */
  int lab, graphnum;
  node *n;
  int FORLIM;

  n = g->ndnext;
  lab = 0;
  while (n != NULL) {
    lab++;
    n->ndlabel = lab;
    if (recurse) {
      if (n->ndsort == ndcompound) {
	FORLIM = numbofsubgraphs(n);
	for (graphnum = 0; graphnum < FORLIM; graphnum++)
	  renumbergraph(tochildgraph(n, graphnum), recurse);
      }
    }
    n = n->ndnext;
  }
}  /* RenumberGraph */


Static Void putnodebefore(n, intransit)
node *n, *intransit;
{
  /* Assumes N <> nil and not (N^.NDSort = NDGraph) */
  /* Inserts the node InTransit in front of the node N */
  node *prev;

  /* remove from old graph */
  prev = intransit->ndparent;
  while (prev->ndnext != intransit)
    prev = prev->ndnext;
  prev->ndnext = intransit->ndnext;
  /* insert into new graph */
  prev = n->ndparent;
  while (prev->ndnext != n)
    prev = prev->ndnext;
  prev->ndnext = intransit;
  intransit->ndparent = n->ndparent;
  intransit->ndnext = n;
  intransit->ndlabel = n->ndlabel;
  /* renumber labels from N on down */
  do {
    n->ndlabel++;
    n = n->ndnext;
  } while (n != NULL);
}  /* PutNodeBefore */


Static Void shiftinputports(n, startport, amount)
node *n;
int startport, amount;
{
  /* Shifts the port numbers of the input edges of N, starting from
     port StartPort and shifting by Amount.
   */
  boolean found;
  port *e;

  found = false;
  e = n->ndilist;
  while (e != NULL && !found) {
    if (e->pttoport >= startport)
      found = true;
    else
      e = e->pttonext;
  }
  if (!found)
    return;
  while (e != NULL) {
    e->pttoport += amount;
    e = e->pttonext;
  }
}  /* ShiftInputPorts */


Static Void shiftoutputports(n, startport, amount)
node *n;
int startport, amount;
{
  /* Shifts the port numbers of the output edges of N, starting from
     port StartPort and shifting by Amount.
   */
  boolean found;
  port *e;

  found = false;
  e = n->ndolist;
  while (e != NULL && !found) {
    if (e->UU.U1.ptfrport >= startport)
      found = true;
    else
      e = e->UU.U1.ptfrnext;
  }
  if (!found)
    return;
  while (e != NULL) {
    e->UU.U1.ptfrport += amount;
    e = e->UU.U1.ptfrnext;
  }
}  /* ShiftOutputPorts */


extern node *copygraph PP((node *oldg));

extern node *copycompound PP((node *oldn));


Static node *copynode(n)
node *n;
{
  /* Create and return a new node identical to N except without any
     input or output edges and detached from any surrounding graph.
     N is not changed in any way.
     If N is a graph node or a compound node, it copys the graph
     structure or the entire compound node.
   */
  node *newnode;

  if (n->ndsort == ndgraph)
    return (copygraph(n));
  else if (n->ndsort == ndcompound)
    return (copycompound(n));
  else {
    newnode = newnodealloc(ndatomic);
    univnodecnt++;
    newnode->ndid = univnodecnt;
    newnode->ndcode = n->ndcode;
    newnode->ndlabel = n->ndlabel;

    /* NEW CANN 2/92 FOR SDBX */
    newnode->ndsrcline = n->ndsrcline;

    return newnode;
  }

  /* N is a simple node */
}  /* CopyNode */


Static node *copycompound(oldn)
node *oldn;
{
  /*( OldN : NDPtr ) : NDPtr*/
  /* Returns a pointer to a newly created compound node
     with the same internal structure as OldN but no
     external input or output edges.
     Note: Recursively calls CopyGraph.
   */
  node *n;
  graph *lastgraph, *graphlink;
  assoclist *tagwalk, *taglast;
  int gnum, FORLIM;
  PSBLOCK b;

  n = newnodealloc(ndcompound);
  univnodecnt++;
  n->ndid = univnodecnt;
  n->ndcode = oldn->ndcode;
  n->ndlabel = oldn->ndlabel;
  /* Copy subgraphs of compound node */
  lastgraph = NULL;
  FORLIM = numbofsubgraphs(oldn);
  for (gnum = 0; gnum < FORLIM; gnum++) {
    graphlink = newgraphptr();
    graphlink->grnode = copygraph(tochildgraph(oldn, gnum));
    graphlink->grnode->ndparent = n;
    if (lastgraph == NULL)
      n->UU.U2.ndsubsid = graphlink;
    else
      lastgraph->grnext = graphlink;
    lastgraph = graphlink;
  }
  lastgraph->grnext = NULL;
  /* copy tag list (if it exists) */
  if (n->ndcode != ifntagcase)  /* copy tag list */
    return n;
  tagwalk = oldn->UU.U2.ndassoc;
  taglast = NULL;
  while (tagwalk != NULL) {
    if (taglast == NULL) {
      /* taglast = (assoclist *)Malloc(sizeof(assoclist)); */
      b = MySBlockAlloc(); /* CANN */
      taglast = &(b->al); /* CANN */

      taglast->next = NULL;
      n->UU.U2.ndassoc = taglast;
    } else {
      /* taglast->next = (assoclist *)Malloc(sizeof(assoclist)); */
      b = MySBlockAlloc(); /* CANN */
      taglast->next = &(b->al); /* CANN */

      taglast->next->next = NULL;
      taglast = taglast->next;
    }
    taglast->graphnum = tagwalk->graphnum;
    tagwalk = tagwalk->next;
  }
  taglast->next = NULL;
  return n;
}  /* CopyCompound */


Local Void copyedges(newg, oldg)
node *newg, *oldg;
{
  /* NewG is a skeleton structure of OldG.  It consists of
     Nodes identical to that of OldG and connected in the
     same order, but has no edges.  This procedure walks
     both NewG and OldG in tandem and makes a copy of each
     input edge of every node of OldG for each corresponding
     node of NewG.
   */
  node *oldn, *newn, *prodn;
  port *olde, *newe;
  int port_, nlab;
  PBBLOCK b;

  newn = newg;
  oldn = oldg;
  /* Walk the nodes of OldG and NewG */
  while (oldn != NULL) {
    if (oldn->ndlabel != newn->ndlabel) {
      printf("INTERNAL ERROR, COPYGRAPH: Wrong Labels: %12d%12d\n",
	     oldn->ndlabel, newn->ndlabel);
      oldn = NULL;
      break;
    }
    olde = oldn->ndilist;
    while (olde != NULL) {
      /* newe = (port *)Malloc(sizeof(port)); */
      b = MyBBlockAlloc(); /* CANN */
      newe = &(b->p); /* CANN */

      *newe = *olde;
      /* Assign a different Id to this new edge */
      univedgecnt++;
      newe->ptid = univedgecnt;
      /* dlz ***     NewE^.PTExtraInfo := nil;  temp. commented out ****/
      newe->pttonode = NULL;
      newe->pttonext = NULL;
      changeedgedest(newe, newn, olde->pttoport);
      if (olde->ptsort == ptedge) {
	newe->UU.U1.ptfrnode = NULL;
	newe->UU.U1.ptfrnext = NULL;
	port_ = producerportnumber(olde);
	nlab = olde->UU.U1.ptfrnode->ndlabel;
	prodn = getnodewithlabel(newg, nlab);
	changeedgesrc(newe, prodn, port_);
      }
      olde = olde->pttonext;
    }
    oldn = oldn->ndnext;
    newn = newn->ndnext;
  }
}  /* CopyEdges */


Static node *copygraph(oldg)
node *oldg;
{
  /* ( OldG : NDPtr ) : NDPtr */
  /* Returns a pointer to a new graph that is an exact copy
     of the old graph, except that the nodes and edges have
     new and unique ID's */
  node *g, *n, *prev, *marker;

  g = newnodealloc(oldg->ndsort);
  univnodecnt++;
  g->ndid = univnodecnt;
  g->ndline = 0;
  g->ndsrcline = -SHORT_MAX;
  g->ndwiline = -SHORT_MAX;
  g->ndxcoord = -SHORT_MAX;
  g->ndycoord = -SHORT_MAX;
  /*dlz*** NDExtraInfo := nil; **** temp commented out*/
  g->ndnext = NULL;
  g->ndparent = NULL;
  g->ndilist = NULL;
  g->ndolist = NULL;
  g->nddepilist = NULL;
  g->nddepolist = NULL;
  g->ndnextinline = NULL;
  g->ndcode = oldg->ndcode;
  g->ndlabel = oldg->ndlabel;
  g->ndsort = oldg->ndsort;
  switch (g->ndsort) {

  case ndgraph:
    g->UU.U1.ndlink = NULL;
    g->UU.U1.ndtype = oldg->UU.U1.ndtype;
    break;

  case ndcompound:
    g->UU.U2.ndsubsid = NULL;
    break;
  }/* case */
  prev = g;
  marker = oldg->ndnext;
  /* Walk the graph copying nodes as we go along */
  while (marker != NULL) {
    if (marker->ndsort == ndcompound)
      n = copycompound(marker);
    else  /* Simple Node */
      n = copynode(marker);
    n->ndparent = g;
    prev->ndnext = n;
    prev = n;
    marker = marker->ndnext;
  }
  prev->ndnext = NULL;
  copyedges(g, oldg);
  return g;
}  /* CopyGraph */


Static Void linknodeintograph(n, parent, prev)
node *n, *parent, *prev;
{
  /* This routine links the node into the graph pointed to by
     Parent after the node Prev.  It then changes the Labels
     of all the nodes from this point on to avoid having two
     nodes in the graph with the same Label.
     Note:  Assumes the graph is ordered.
     Note:  if prev = nil then adds N as the first node in the graph.
   */
  if (prev == NULL)
    prev = parent;
  n->ndnext = prev->ndnext;
  prev->ndnext = n;
  n->ndparent = parent;
  n->ndlabel = prev->ndlabel;
  while (n != NULL) {
    n->ndlabel++;
    n = n->ndnext;
  }
}  /* LinkNodeIntoGraph */


/* macro procedure ChangeEdgeType( E : EGPtr; T : STPtr ); */
/* Changes the type of the edge E to T */
/* macro procedure ChangeNextNode( CurrentNode, NextNode : NDPtr ); */
/* Assumes CurrentNode <> nil */
Static Void removefunction(fungraph)
node *fungraph;
{
  /* Disconnects the given function from the environment and
   removes all its nodes.  */
  linkrec *funlink, *prevlink;
  graph *prevgraph, *fgraph;

  /* FunGraph is connected at both the top and the bottom.  Find the
     top connection and break it first */
  prevgraph = module->UU.U2.ndsubsid;
  if (prevgraph->grnode == fungraph) {  /* first in list, remove it */
    if (prevgraph->grnext == NULL)
      firstfunction = NULL;
    else
      firstfunction = prevgraph->grnext->grnode;
    module->UU.U2.ndsubsid = prevgraph->grnext;
    prevgraph->grnext = NULL;
    prevgraph->grnode = NULL;
  } else {  /* find the previous element, then remove link to FunGraph */
    while (prevgraph->grnext->grnode != fungraph)
      prevgraph = prevgraph->grnext;
    fgraph = prevgraph->grnext;
    prevgraph->grnext = fgraph->grnext;
    fgraph->grnext = NULL;
    fgraph->grnode = NULL;
  }
  /* Now unlink at the bottom end */
  funlink = fungraph->UU.U1.ndlink;
  if (funlink == funclist) {
    funclist = funlink->lknext;
    funlink->lknext = NULL;
  } else {
    prevlink = funclist;
    while (prevlink->lknext != funlink)
      prevlink = prevlink->lknext;
    prevlink->lknext = funlink->lknext;
    funlink->lknext = NULL;
  }
  funlink->lkgraph = NULL;
  /* Now remove the graph nodes from the universe */
  fungraph->UU.U1.ndlink = NULL;
  removenode(fungraph);
}  /* RemoveFunction */


extern Void cleanupgraph PP((node *g));


Static Void cleanupnewiter(loop)
node *loop;
{
  node *bodyg;
  int lstart, lend, port_, maxloopin;
  port *e, *ine, *oute;

  bodyg = toiterbodygraph(loop);
  e = bodyg->ndilist;
  e = e->pttonext;
  lstart = e->pttoport;
  lend = largestinputportnumber(bodyg);
  maxloopin = largestinputportnumber(loop);
  /* Check for unused loop input Ports */
  for (port_ = 2; port_ <= maxloopin; port_++) {
    e = getoutputedge(bodyg, port_);
    oute = getoutputedge(loop, port_);
    if (e == NULL && oute == NULL) {
      e = getinputedge(loop, port_);
      if (e != NULL) {
	disconnectedgefromsource(e);
	disconnectedgefromdest(e);
      }
      if (getinputedge(bodyg, port_) == NULL) {
	shiftinputports(loop, port_, -1);
	shiftoutputports(bodyg, port_, -1);
	shiftinputports(bodyg, port_, -1);
	shiftoutputports(loop, port_, -1);
      }
    }  /*then*/
  }  /*for*/
  /* Check for unused L ports */
  for (port_ = lstart; port_ <= lend; port_++) {
    ine = getinputedge(bodyg, port_);
    oute = getoutputedge(bodyg, port_);
    e = getoutputedge(loop, port_);
    if (e == NULL && oute == NULL && ine != NULL) {
      disconnectedgefromsource(ine);
      disconnectedgefromdest(ine);
      shiftinputports(loop, port_, -1);
      shiftoutputports(bodyg, port_, -1);
      shiftinputports(bodyg, port_, -1);
      shiftoutputports(loop, port_, -1);
    }  /*then*/
  }  /*for*/
  cleanupgraph(bodyg);
}  /*CleanUpNewIter*/


Static Void cleanupforall(loop)
node *loop;
{
  int port_, count, k, l, t;
  port *e;
  node *geng, *bodyg, *retg;
  int FORLIM;

  geng = toforallgeneratorgraph(loop);
  bodyg = toforallbodygraph(loop);
  retg = toforallreturnsgraph(loop);
  k = largestinputportnumber(loop);
  l = largestinputportnumber(geng);
  if (l == 0)
    l = k;
  t = largestinputportnumber(bodyg);
  if (t == 0)
    t = l;
  /* walk input edges of Returns graph, removing unused edges */
  port_ = 1;
  FORLIM = largestinputportnumber(retg);
  for (count = 1; count <= FORLIM; count++) {
    if (getoutputedge(loop, port_) == NULL) {
      e = getinputedge(retg, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(retg, port_, -1);
      shiftoutputports(loop, port_, -1);
    } else
      port_++;
  }
  /* remove unused nodes of this graph */
  cleanupgraph(retg);
  /* Check for missing temporaries */
  port_ = l + 1;
  for (count = port_; count <= t; count++) {
    if (getoutputedge(retg, port_) == NULL) {
      e = getinputedge(bodyg, port_);
      if (e != NULL)
	removeedge(&e);
      shiftoutputports(retg, port_, -1);
      shiftinputports(bodyg, port_, -1);
    } else
      port_++;
  }
  /* Now remove dead code from the body graph */
  cleanupgraph(bodyg);
  /* Check for unused Loop imports */
  port_ = 1;
  for (count = port_; count <= k; count++) {
    if ((getoutputedge(geng, port_) == NULL) & (getoutputedge(bodyg, port_) ==
	  NULL) & (getoutputedge(retg, port_) == NULL)) {
      e = getinputedge(loop, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(loop, port_, -1);
      shiftoutputports(geng, port_, -1);
      shiftinputports(geng, port_, -1);
      shiftoutputports(bodyg, port_, -1);
      shiftinputports(bodyg, port_, -1);
      shiftoutputports(retg, port_, -1);
    } else
      port_++;
  }
}  /* CleanUpForall */


Static Void cleanupiterloop(loop)
node *loop;
{
  node *initg, *testg, *bodyg, *retg;
  int k, l, t, port_, count;
  port *e;
  int FORLIM;

  initg = toinitgraph(loop);
  testg = totestgraph(loop);
  bodyg = tobodygraph(loop);
  retg = toreturnsgraph(loop);
  k = largestinputportnumber(loop);
  l = largestinputportnumber(initg);
  if (l == 0)
    l = k;
  t = largestinputportnumber(bodyg);
  if (t == 0)
    t = l;
  /* walk input edges of Returns graph, removing unused edges */
  port_ = 1;
  FORLIM = largestinputportnumber(retg);
  for (count = 1; count <= FORLIM; count++) {
    if (getoutputedge(loop, port_) == NULL) {
      e = getinputedge(retg, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(retg, port_, -1);
      shiftoutputports(loop, port_, -1);
    } else
      port_++;
  }
  cleanupgraph(retg);
  cleanupgraph(testg);
  port_ = l + 1;
  for (count = port_; count <= t; count++) {
    if ((getoutputedge(retg, port_) == NULL) &
	(getoutputedge(testg, port_) == NULL)) {
      e = getinputedge(bodyg, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(bodyg, port_, -1);
      shiftoutputports(testg, port_, -1);
      shiftoutputports(retg, port_, -1);
    } else
      port_++;
  }
  cleanupgraph(bodyg);
  port_ = k + 1;
  for (count = port_; count <= l; count++) {
    if ((getoutputedge(testg, port_) == NULL) &
	(getoutputedge(bodyg, port_) == NULL) & (getinputedge(bodyg, port_) ==
	  NULL) & (getoutputedge(retg, port_) == NULL)) {
      e = getinputedge(initg, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(initg, port_, -1);
      shiftoutputports(testg, port_, -1);
      shiftoutputports(bodyg, port_, -1);
      shiftinputports(bodyg, port_, -1);
      shiftoutputports(retg, port_, -1);
    } else
      port_++;
  }
  cleanupgraph(initg);
  port_ = 1;
  for (count = port_; count <= k; count++) {
    if ((getoutputedge(initg, port_) == NULL) & (getoutputedge(testg, port_) ==
	  NULL) & (getoutputedge(bodyg, port_) == NULL) &
	(getoutputedge(retg, port_) == NULL)) {
      e = getinputedge(loop, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(loop, port_, -1);
      shiftoutputports(initg, port_, -1);
      shiftinputports(initg, port_, -1);
      shiftoutputports(testg, port_, -1);
      shiftoutputports(bodyg, port_, -1);
      shiftinputports(bodyg, port_, -1);
      shiftoutputports(retg, port_, -1);
    } else
      port_++;
  }
}  /* CleanUpIterLoop */


Static Void cleanupselect(n)
node *n;
{
  int k, r, maxr, grnum, subgr, count, port_;
  node *g;
  port *e;
  boolean empty;

  grnum = numbofsubgraphs(n) - 1;
  k = largestinputportnumber(n);
  maxr = 0;
  for (subgr = 1; subgr <= grnum; subgr++) {
    r = largestinputportnumber(tochildgraph(n, subgr));
    if (r > maxr)
      maxr = r;
  }
  port_ = 1;
  for (count = 1; count <= maxr; count++) {
    if (getoutputedge(n, port_) == NULL) {
      shiftoutputports(n, port_, -1);
      for (subgr = 1; subgr <= grnum; subgr++) {
	g = tochildgraph(n, subgr);
	e = getinputedge(g, port_);
	if (e != NULL)
	  removeedge(&e);
	shiftinputports(g, port_, -1);
      }
    } else
      port_++;
  }
  /* remove dead code from subgraphs */
  for (subgr = 0; subgr <= grnum; subgr++)
    cleanupgraph(tochildgraph(n, subgr));
  /* remove unused inputs */
  port_ = 1;
  for (count = 1; count <= k; count++) {
    empty = true;
    for (subgr = 0; subgr <= grnum; subgr++)
      empty &= (getoutputedge(tochildgraph(n, subgr), port_) == NULL);
    if (empty) {
      e = getinputedge(n, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(n, port_, -1);
      for (subgr = 0; subgr <= grnum; subgr++)
	shiftoutputports(tochildgraph(n, subgr), port_, -1);
    } else
      port_++;
  }
}  /* CleanUpSelect */


Static Void cleanuptagcase(n)
node *n;
{
  int k, r, maxr, grnum, subgr, count, port_;
  node *g;
  port *e;
  boolean empty;

  grnum = numbofsubgraphs(n) - 1;
  k = largestinputportnumber(n);
  maxr = 0;
  for (subgr = 0; subgr <= grnum; subgr++) {
    r = largestinputportnumber(tochildgraph(n, subgr));
    if (r > maxr)
      maxr = r;
  }
  /* remove unused outputs */
  port_ = 1;
  for (count = 1; count <= maxr; count++) {
    if (getoutputedge(n, port_) == NULL) {
      shiftoutputports(n, port_, -1);
      for (subgr = 0; subgr <= grnum; subgr++) {
	g = tochildgraph(n, subgr);
	e = getinputedge(g, port_);
	if (e != NULL)
	  removeedge(&e);
	shiftinputports(g, port_, -1);
      }
    } else
      port_++;
  }
  /* remove dead code in all subgraphs */
  for (subgr = 0; subgr <= grnum; subgr++)
    cleanupgraph(tochildgraph(n, subgr));
  /* remove unused inputs */
  port_ = 2;
  for (count = 2; count <= k; count++) {
    empty = true;
    for (subgr = 0; subgr <= grnum; subgr++)
      empty &= (getoutputedge(tochildgraph(n, subgr), port_) == NULL);
    if (empty) {
      e = getinputedge(n, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(n, port_, -1);
      for (subgr = 0; subgr <= grnum; subgr++)
	shiftoutputports(tochildgraph(n, subgr), port_, -1);
    } else
      port_++;
  }
}  /* CleanUpTagCase */


Static Void cleanupifthenelse(n)
node *n;
{
  int k, r, maxr, grnum, subgr, count, port_;
  node *g;
  port *e;
  boolean empty;

  grnum = numbofsubgraphs(n) - 1;
  k = largestinputportnumber(n);
  maxr = 0;
  for (subgr = 0; subgr <= grnum; subgr++) {
    if ((subgr & 1) == 0 || subgr == grnum)
    {  /* only true and false subgraphs */
      r = largestinputportnumber(tochildgraph(n, subgr));
      if (r > maxr)
	maxr = r;
    }
  }
  port_ = 1;
  for (count = 1; count <= maxr; count++) {
    if (getoutputedge(n, port_) == NULL) {
      shiftoutputports(n, port_, -1);
      for (subgr = 0; subgr <= grnum; subgr++) {
	if ((subgr & 1) == 0 || subgr == grnum)
	{  /* only true and false subgraphs */
	  g = tochildgraph(n, subgr);
	  e = getinputedge(g, port_);
	  if (e != NULL)
	    removeedge(&e);
	  shiftinputports(g, port_, -1);
	}
      }
    } else
      port_++;
  }
  /* remove dead code from subgraphs */
  for (subgr = 0; subgr <= grnum; subgr++)
    cleanupgraph(tochildgraph(n, subgr));
  /* remove unused inputs */
  /* NOTE:  Input on port 1 is control information and can't be removed */
  port_ = 2;
  for (count = 2; count <= k; count++) {
    empty = true;
    for (subgr = 0; subgr <= grnum; subgr++)
      empty &= (getoutputedge(tochildgraph(n, subgr), port_) == NULL);
    if (empty) {
      e = getinputedge(n, port_);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(n, port_, -1);
      for (subgr = 0; subgr <= grnum; subgr++)
	shiftoutputports(tochildgraph(n, subgr), port_, -1);
    } else
      port_++;
  }
}  /* CleanUpIfThenElse */


#define stackbucketsize  50


typedef struct nodebucket {
  struct nodebucket *prev, *next;
  node *elems[stackbucketsize];
} nodebucket;

typedef struct ndstack {
  int length;
  nodebucket *firstbucket, *lastbucket;
} ndstack;


Local Void nsinit(s)
ndstack *s;
{
  s->length = 0;
  s->firstbucket = NULL;
  s->lastbucket = NULL;
}

Local boolean nsempty(s)
ndstack *s;
{
  return (s->length == 0);
}

Local Void nspush(s, n)
ndstack *s;
node *n;
{
  nodebucket *b;
  int ix;

  s->length++;
  ix = (s->length - 1) % stackbucketsize + 1;
/* p2c: mono.bin.noblank.p, line 6129:
 * Note: Using % for possibly-negative arguments [317] */
  if (ix == 1) {
    b = (nodebucket *)Malloc(sizeof(nodebucket));
    b->prev = NULL;
    b->next = NULL;
    if (s->lastbucket == NULL) {  /* stack was empty */
      s->firstbucket = b;
      s->lastbucket = b;
      b->prev = NULL;
    } else {
      s->lastbucket->next = b;
      b->prev = s->lastbucket;
      s->lastbucket = b;
    }
    b->next = NULL;
  }
  /* must add a new bucket */
  /* put element in */
  s->lastbucket->elems[ix - 1] = n;   /* with */
}  /* NSPush */

Local node *nspop(s)
ndstack *s;
{
  node *Result;
  int ix;
  nodebucket *b;

  if (s->length == 0)   /* with */
    return NULL;
  ix = (s->length - 1) % stackbucketsize + 1;
/* p2c: mono.bin.noblank.p, line 6164:
 * Note: Using % for possibly-negative arguments [317] */
  Result = s->lastbucket->elems[ix - 1];
  s->length--;
  if (ix != 1)
    return Result;
  if (s->firstbucket == s->lastbucket) {
    b = s->lastbucket;
    s->lastbucket = NULL;
    s->firstbucket = NULL;
    /* dispose( b );   *jyu* */
    return Result;
  }
  b = s->lastbucket;
  s->lastbucket = b->prev;
  if (s->lastbucket != NULL)
    s->lastbucket->next = NULL;
  b->prev = NULL;
  /* dispose( b ); *jyu* */
  return Result;

  /* else */
  /* else */
}  /* NSPop */


Static Void cleanupgraph(g)
node *g;
{
  /*( G : NDPtr )*/
  ndstack stk;
  node *above, *below, *curr;

  nsinit(&stk);
  /* walk down the graph reversing pointer */
  above = g;
  curr = g->ndnext;
  while (curr != NULL) {
    below = curr->ndnext;
    curr->ndnext = above;
    above = curr;
    curr = below;
  }
  /* Walk back up, reversing the pointers again and tagging dead nodes */
  curr = above;
  above = curr->ndnext;
  below = NULL;
  while (curr != g) {
    if (curr->ndolist == NULL) {
/* CANN PEEK */ if ( curr->ndcode != ifnpeek )
if ( !(CANN_GlueMode && (curr->ndcode == ifncall)) ) { /* NEW CANN */
      removeinputedges(curr);
      nspush(&stk, curr);
} /* NEW CANN */
    } else if (curr->ndsort == ndcompound) {
      switch (curr->ndcode) {

      case ifnforall:
	cleanupforall(curr);
	break;

      case ifnloopb:
	cleanupiterloop(curr);
	break;

      case ifnloopa:
	cleanupiterloop(curr);
	break;

      case ifnselect:
	cleanupselect(curr);
	break;

      case ifntagcase:
	cleanuptagcase(curr);
	break;

      case ifnifthenelse:
	cleanupifthenelse(curr);
	break;

      case ifniter:
	cleanupnewiter(curr);
	break;
      }
    }
    curr->ndnext = below;
    below = curr;
    curr = above;
    above = above->ndnext;
  }
  /* now remove marked nodes */
  while (!nsempty(&stk)) {
    curr = nspop(&stk);
    /*if MessagesOn then
              writeln( 'Dead Code Removal:  Removing node ', Curr^.NDId:1);
    */
    removenode(curr);
  }
}  /* CleanUpGraph */

#undef stackbucketsize


Static Void removedeadcode()
{
  /* This routine walks the function graphs of a module, searching for
     and removing dead code.  Code is dead if it produces values that
     can never be used.  One example of this is a node with no output
     edges, another is a node that has output edges connected to a graph
     but for which there are no other implicit connections in other graphs.
     This second case can happen if temporaries are computed in the body
     of a loop and wired the the graph outputs but are never used in the
     test or returns graph.
     The algorithm requires that the nodes have been ordered.  It traverses
     the graph in reverse order (by walking down and reversing pointers then
     walking back up and restoring them) looking for nodes with no output
     edges.  If it finds one it removes all of its input edges and saves
     that node on a stack to be removed later (when the graph is restored).
     It cleans up compound nodes by traversing the subgraphs in the
     reverse order, taking account of impicit connections between subgraphs.
   */
  node *f;

  f = firstfunction;
  while (f != NULL) {
    cleanupgraph(f);
    f = tonextfunction(f);
  }
}  /* RemoveDeadCode */


Local Void rm(n)
node *n;
{
  port *e, *ie, *oe;
  node *srcnode, *cn;
  int srcport, port_, cp;
  stryng lvalue, name;

  /* First, change all output edges */
  ie = n->ndilist;
  while (ie != NULL) {
    port_ = ie->pttoport;
    oe = getoutputedge(n, port_);
    if (oe == NULL) {
      printf("WARNING: RemoveNoOp: No output edge on port %d", port_);
      printf(" of NoOp node ID = %d\n", n->ndid);
    } else if (producerportnumber(oe) != port_) {
      printf("WARNING: RemoveNoOp: No output edge on port %d", port_);
      printf(" of NoOp node ID = %d\n", n->ndid);
    } else if (ie->ptsort == ptlit) {
      /* make corresponding outputs Literals */
      lvalue = ie->UU.ptlitvalue;
      while (oe != NULL) {
	e = oe;
	oe = nextoutputedgesameport(oe);
	cp = e->pttoport;
	cn = e->pttonode;
	name = e->ptname;
	disconnectedgefromdest(e);
	disconnectedgefromsource(e);
	e = insertliteral(cn, cp, e->pttype, lvalue, name);
	/*  ChangeEdgeToLiteral( E, Lvalue )*/
      }
    } else {
      /* Change the source nodes of all outputs */
      srcnode = producernodeofedge(ie);
      srcport = producerportnumber(ie);
      while (oe != NULL) {
	e = oe;
	oe = nextoutputedgesameport(oe);
	changeedgesrc(e, srcnode, srcport);
      }
    }
    ie = ie->pttonext;
  }
  removenode(n);

  /* IE is an edge */
}  /* rm */


Static Void removenoop(g)
node *g;
{
  node *gn, *n;
  int grnum, FORLIM;

  n = g->ndnext;
  while (n != NULL) {
    gn = n;
    n = n->ndnext;
    if (gn->ndsort == ndcompound) {
      FORLIM = numbofsubgraphs(gn);
      for (grnum = 0; grnum < FORLIM; grnum++)
	removenoop(tochildgraph(gn, grnum));
    } else if (gn->ndcode == ifnnoop)
      rm(gn);
  }
}  /* RemoveNoOp */


Local Void pushinside(n, l)
node *n;
port *l;
{
  int port_, gr, cp;
  stryng lvalue, name;
  node *g, *cn;
  port *e, *tmpe;
  int FORLIM;

  port_ = l->pttoport;
  lvalue = l->UU.ptlitvalue;
  removeedge(&l);
  shiftinputports(n, port_, -1);
  FORLIM = numbofsubgraphs(n);
  for (gr = 0; gr < FORLIM; gr++) {
    g = tochildgraph(n, gr);
    e = getoutputedge(g, port_);
    while (e != NULL) {
      tmpe = e;
      e = nextoutputedgesameport(e);
      cn = tmpe->pttonode;
      cp = tmpe->pttoport;
      name = tmpe->ptname;
      disconnectedgefromdest(tmpe);
      disconnectedgefromsource(tmpe);
      tmpe = insertliteral(cn, cp, tmpe->pttype, lvalue, name);
      /*      ChangeEdgeToLiteral( TmpE, Lvalue )*/
    }
    shiftoutputports(g, port_, -1);
  }
  switch (n->ndcode) {

  case ifnifthenelse:
  case ifnselect:
  case ifntagcase:
    /* blank case */
    break;

  case ifnforall:
    g = toforallgeneratorgraph(n);
    shiftinputports(g, port_, -1);
    g = toforallbodygraph(n);
    shiftinputports(g, port_, -1);
    break;

  case ifnloopb:
  case ifnloopa:
    g = toinitgraph(n);
    shiftinputports(g, port_, -1);
    g = tobodygraph(n);
    shiftinputports(g, port_, -1);
    break;
  }/* case */
}  /* PushInside */


Static Void pushliteralsinside(n)
node *n;
{
  port *e, *tmpe;

  e = n->ndilist;
  while (e != NULL) {
    tmpe = e;
    e = e->pttonext;
    if (tmpe->ptsort == ptlit) {
      if (n->ndcode != ifntagcase && n->ndcode != ifnifthenelse ||
	  tmpe->pttoport != 1)
	pushinside(n, tmpe);
      /* This edge represents Control and can't be removed */
    }
  }
}  /* PushLiteralsInside */


Static Void swapinputports(n, p1, p2)
node *n;
int p1, p2;
{
  /* Swap the edges connected to ports P1 and P2 */
  port *e1, *e2;

  e1 = getinputedge(n, p1);
  e2 = getinputedge(n, p2);
  /* Note, this will cause Edge E2 to be inserted before edge E1
     at port P1 (causing Fan-in).  The next operation will rectify
     the situation by removing edge E1 from port P1 and placing
     it at port P2 */
  if (e2 != NULL)
    changeedgedest(e2, n, p1);
  if (e1 != NULL)
    changeedgedest(e1, n, p2);
}  /* SwapInputPorts */


Static Void swapoutputports(n, p1, p2)
node *n;
int p1, p2;
{
  port *etmp, *e1, *e2;

  e1 = getoutputedge(n, p1);
  e2 = getoutputedge(n, p2);
  /* Move the edges on port P1 to port P2 */
  /* NOTE: They will be moved to the FRONT of the list of all edges
          on port P2, the origional edges of port P2 will still be
          there, but at the back of the list */
  while (e1 != NULL) {
    etmp = e1;
    e1 = nextoutputedgesameport(e1);
    changeedgesrc(etmp, n, p2);
  }
  while (e2 != NULL) {
    etmp = e2;
    e2 = nextoutputedgesameport(e2);
    changeedgesrc(etmp, n, p1);
  }
}  /* SwapOutputPorts */


Local int compactinputs(c, startport, lastport)
node *c;
int startport, lastport;
{
  /* -- C is a compound node
     -- 1 <= StartPort <= LastPort <= LargestInputPortNumber( C )
     This function searches for input ports of C whose values
     are not used by any subgraphs.  If such a port is found:
       (1) the input edge corresponding to that port is removed
       (2) that port is "removed" by shifting the remaining
           ports to the left by one unit.
     -- The function returns the number of ports removed.
   */
  int p, ix, lastgraph, numberremoved, grnum;
  port *e;
  boolean portused;

  lastgraph = numbofsubgraphs(c) - 1;
  p = startport;
  numberremoved = 0;
  for (ix = p; ix <= lastport; ix++) {
    portused = false;
    grnum = 0;
    while (grnum <= lastgraph && !portused) {
      if (getoutputedge(tochildgraph(c, grnum), p) != NULL)
	portused = true;
      else
	grnum++;
    }
    if (portused)
      p++;
    else {  /* remove this port */
      numberremoved++;
      e = getinputedge(c, p);
      if (e != NULL)
	removeedge(&e);
      shiftinputports(c, p, -1);
      for (grnum = 0; grnum <= lastgraph; grnum++)
	shiftoutputports(tochildgraph(c, grnum), p, -1);
    }
  }
  return numberremoved;
}  /* CompactInputs */


Static Void compactports(c)
node *c;
{
  /* -- C is a compound node
     This routine examines the implicit port connections of C searching
     for port numbers that are no longer used by any subgraph.  If any
     are found, they are eliminated by shifting edges into this gap from
     the right.
   */
  port *tmpe, *e1, *e2, *e3;
  int k, l, t, p, count, ix;
  node *initg, *geng, *testg, *bodyg, *retg;

  k = largestinputportnumber(c);
  switch (c->ndcode) {

  case ifnforall:
    geng = toforallgeneratorgraph(c);
    bodyg = toforallbodygraph(c);
    retg = toforallreturnsgraph(c);
    l = largestinputportnumber(geng);
    if (l == 0)
      l = k;
    t = largestinputportnumber(bodyg);
    if (t == 0)
      t = l;
    /* Compact the K ports */
    count = compactinputs(c, 1, k);
    if (count > 0) {
      shiftinputports(geng, k + 1, -count);
      shiftinputports(bodyg, l + 1, -count);
      k -= count;
      l -= count;
      t -= count;
    }
    /* Compact the L ports */
    /*  P := K+1;Cannot remove and L ports, even if unused
              count := 0;
              for ix := P to L do
                begin
                  E1 := GetOutputEdge( BodyG, P );
                  E2 := GetOutputEdge( RetG, P );
                  if (E1 = nil) and (E2 = nil) then
                    begin
                      count := succ( count );
                      ShiftOutputPorts( BodyG, P, -1 );
                      ShiftInputPorts( BodyG, P, -1 );
                      ShiftOutputPorts( RetG, P, -1 );
                      TmpE := GetInputEdge( GenG, P );
                      if TmpE <> nil then
                        RemoveEdge( TmpE );
                      ShiftInputPorts( GenG, P, -1 )
                    end
                  else
                    P := succ( P )
                end;
              L := L - count;
              T := T - count;*/
    /* Compact the T ports */
    count = 0;
    p = l + 1;
    for (ix = p; ix <= t; ix++) {
      if (getoutputedge(retg, p) == NULL) {
	count++;
	shiftoutputports(retg, p, -1);
	tmpe = getinputedge(bodyg, p);
	if (tmpe != NULL)
	  removeedge(&tmpe);
	shiftinputports(bodyg, p, -1);
      } else
	p++;
    }
    t -= count;
    break;
    /* IFNForall */

  case ifnloopa:
  case ifnloopb:
    initg = toinitgraph(c);
    testg = totestgraph(c);
    bodyg = tobodygraph(c);
    retg = toreturnsgraph(c);
    l = largestinputportnumber(initg);
    if (l == 0)
      l = k;
    t = largestinputportnumber(bodyg);
    if (t == 0)
      t = l;
    /* Compact the K ports */
    count = compactinputs(c, 1, k);
    if (count > 0) {
      shiftinputports(initg, k + 1, -count);
      shiftinputports(bodyg, k + 1, -count);
      k -= count;
      l -= count;
      t -= count;
    }
    /* Compact the L ports */
    p = k + 1;
    count = 0;
    for (ix = p; ix <= l; ix++) {
      e1 = getoutputedge(testg, p);
      e2 = getoutputedge(bodyg, p);
      e3 = getoutputedge(retg, p);
      if (e1 == NULL && e2 == NULL && e3 == NULL) {
	count++;
	shiftoutputports(testg, p, -1);
	shiftoutputports(bodyg, p, -1);
	tmpe = getinputedge(bodyg, p);
	if (tmpe != NULL)
	  removeedge(&tmpe);
	shiftinputports(bodyg, p, -1);
	shiftoutputports(retg, p, -1);
	tmpe = getinputedge(initg, p);
	if (tmpe != NULL)
	  removeedge(&tmpe);
	shiftinputports(initg, p, -1);
      } else
	p++;
    }
    l -= count;
    t -= count;
    /* Compact the T ports */
    count = 0;
    p = l + 1;
    for (ix = p; ix <= t; ix++) {
      e1 = getoutputedge(testg, p);
      e2 = getoutputedge(retg, p);
      if (e1 == NULL && e2 == NULL) {
	count++;
	shiftoutputports(testg, p, -1);
	shiftoutputports(retg, p, -1);
	tmpe = getinputedge(bodyg, p);
	if (tmpe != NULL)
	  removeedge(&tmpe);
	shiftinputports(bodyg, p, -1);
      } else
	p++;
    }
    t -= count;
    break;
    /* IFNLoopA, IFNLoopB */

  case ifnselect:
    count = compactinputs(c, 1, k);
    break;
    /* IFNSelect */

  case ifntagcase:
    count = compactinputs(c, 2, k);
    break;
    /* IFNTagCase */

  case ifnifthenelse:
    count = compactinputs(c, 1, k);
    break;
    /* IFNIfThenElse */
  }/* case */
}  /* CompactPorts */


Static Void replacecallwithgraph(cnode, fgraph)
node **cnode, *fgraph;
{
  /* Preconditions:
      1) Cnode is a call node to the function whose graph is Fgraph
      2) Fgraph has already been determined to be inline expandable
    Postconditions:
      1) Fgraph is copied to a temporary location
      2) Cnode is removed from its surrounding graph and the contents of
         the copied fgraph are put in its place.
      3) The edges at the graph boundry are wired as when the call node
         existed.
  */
  node *n, *parent, *prev, *next, *newg, *srcnode, *cn;
  int port_, srcport, cp;
  port *e, *ce, *ge, *tmpe;
  stryng litvalue, name;

  newg = copynode(fgraph);
  /* Now change the input edges of Cnode */
  e = (*cnode)->ndilist;
  e = e->pttonext;   /* First Edge Is Function Name */
  while (e != NULL) {
    ce = e;
    e = e->pttonext;
    port_ = ce->pttoport - 1;
    if (ce->ptsort == ptlit) {
      litvalue = ce->UU.ptlitvalue;
      ge = getoutputedge(newg, port_);
      while (ge != NULL) {
	tmpe = ge;
	ge = nextoutputedgesameport(ge);
	cn = tmpe->pttonode;
	cp = tmpe->pttoport;
	name = tmpe->ptname;
	disconnectedgefromsource(tmpe);
	disconnectedgefromdest(tmpe);
	tmpe = insertliteral(cn, cp, tmpe->pttype, litvalue, name);
	/*ChangeEdgeToLiteral( TmpE, LitValue );*/
      }
      continue;
    }
    srcnode = producernodeofedge(ce);
    srcport = producerportnumber(ce);
    ge = getoutputedge(newg, port_);
    while (ge != NULL) {
      tmpe = ge;
      ge = nextoutputedgesameport(ge);
      changeedgesrc(tmpe, srcnode, srcport);
    }
  }
  /* Now change the output edges */
  e = newg->ndilist;
  while (e != NULL) {
    ge = e;
    e = e->pttonext;
    port_ = ge->pttoport;
    srcnode = producernodeofedge(ge);
    srcport = producerportnumber(ge);
    removeedge(&ge);
    ce = getoutputedge(*cnode, port_);
    while (ce != NULL) {
      tmpe = ce;
      ce = nextoutputedgesameport(ce);
      changeedgesrc(tmpe, srcnode, srcport);
    }
  }
  /* Now remove the call node and insert the graph contents */
  parent = (*cnode)->ndparent;
  prev = toprevnode(*cnode);
  if (prev == NULL)
    prev = parent;
  next = (*cnode)->ndnext;
  removenode(*cnode);
  n = newg->ndnext;
  prev->ndnext = n;
  while (n != NULL) {
    n->ndparent = parent;
    prev = n;
    n = n->ndnext;
    if (n == NULL)
      prev->ndnext = next;
  }
  newg->ndnext = NULL;
  removenode(newg);   /*nonrecursively*/
  /* The Graph must now be renumbered to avoid label conflicts */
  renumbergraph(parent, false);
}  /* ReplaceCallWithGraph */


Static Void replacefunctiongraph(newfg, oldfg)
node *newfg, *oldfg;
{
  /*  Replace the function represented by OldFG with NewFG.  Assumes OldFG is
      a function in the current module.   */
  graph *g;

  newfg->UU.U1.ndlink = oldfg->UU.U1.ndlink;
  newfg->UU.U1.ndlink->lkgraph = newfg;
  g = module->UU.U2.ndsubsid;
  while (g->grnode != oldfg)
    g = g->grnext;
  g->grnode = newfg;
  if (firstfunction == oldfg)
    firstfunction = newfg;
}


Local Void buildbasictype(base, name)
char base;
Char *name;
{
  /* builds a symbol table entry an IF1 basic type */
  /* BaseRange is a subrange of defined constants  */
  stentry *s;

  s = newtypealloc(iftbasic);
  s->stlabel = base + 1;
  string10(&s->stliteral, name);
  s->UU.stbasic = base;
  typetable[s->stlabel - 1] = s;
}  /* BuildBasicType */


/* dlz - reorganized and rewrote 8/87 to avoid referencing possibly
   uncompiled fields (vivek's), extracted similar sequences, etc.
   Also, rewrote pragma assignments thru new routines AssignNodePragma
   and AssignEdgePragma, now pragmas not usable in this version
   ignored and a cautionary message printed */
Static Void initmodule()
{
  /* Initializes the global variables needed for loading the program */
  int i;
  /* stentry *t; */
  node *WITH;

  univnodecnt = 0;
  univedgecnt = 0;
  univmodulecnt = 0;
  initstamps();
  funclist = NULL;
  firstfunction = NULL;
  module = newnodealloc(ndcompound);
  WITH = module;
  WITH->ndcode = ifnmodule;
  WITH->ndsort = ndcompound;
  WITH->UU.U2.ndsubsid = NULL;
  WITH->ndparent = NULL;
  WITH->ndnext = NULL;
  WITH->ndilist = NULL;
  WITH->ndolist = NULL;
  WITH->nddepilist = NULL;
  WITH->nddepolist = NULL;
  WITH->ndnextinline = NULL;
  buildbasictype(ifbboolean, "Boolean   ");
  buildbasictype(ifbcharacter, "Character ");
  buildbasictype(ifbdouble, "Double    ");
  buildbasictype(ifbinteger, "Integer   ");
  buildbasictype(ifbnull, "Null      ");
  buildbasictype(ifbreal, "Real      ");
  buildbasictype(ifbwild, "WildBasic ");
  tthwm = 7;
  for (i = tthwm; i < entrymax; i++)
    typetable[i] = NULL;
  /* Add the Wild Card Type to the Type Table */
  /* t = */ (Void)getwildtype();
}  /* InitModule */

#undef nodemax
#undef tab_
#undef hashtablesize_
#undef hashtableub

#define tab_            9


/* Local variables for dumpprogram: */
struct LOC_dumpprogram {
  FILE *diag;
} ;

extern Void dumpgraph PP((node *g, struct LOC_dumpprogram *LINK));

Local Void writetypenumber(entry_, LINK)
stentry *entry_;
struct LOC_dumpprogram *LINK;
{
  /* Print out the type number associated with this type pointer.
     If the pointer is nill the type number is zero.  */
  /* WriteTypeNumber */
  if (entry_ == NULL)
    fprintf(LINK->diag, "%c0", (Char)tab_);
  else
    fprintf(LINK->diag, "%c%d", (Char)tab_, entry_->stlabel);
}

Local Void dumpentry(eptr, LINK)
stentry *eptr;
struct LOC_dumpprogram *LINK;
{
  /* Print the type table entry for this Type pointer. */
  /* DumpEntry */
  if (eptr == NULL)   /* with */
    return;
  fprintf(LINK->diag, "T %d %d", eptr->stlabel, eptr->stsort);
  switch (eptr->stsort) {

  case iftwild:
    /* blank case */
    break;

  case iftbasic:
    fprintf(LINK->diag, " %d", eptr->UU.stbasic);
    break;

  case iftfunctiontype:
    writetypenumber(eptr->UU.U3.starg, LINK);
    writetypenumber(eptr->UU.U3.stres, LINK);
    break;

  case iftarray:
  case iftstream:
  case iftmultiple:
  case iftrecord:
  case iftunion:
  case iftbuffer:
    writetypenumber(eptr->UU.stbasetype, LINK);
    break;

  case iftfield:
  case ifttuple:
  case ifttag:
    writetypenumber(eptr->UU.U2.stelemtype, LINK);
    writetypenumber(eptr->UU.U2.stnext, LINK);
    break;
  }/* case */
  /* write type pragmas */
  if (eptr->stliteral.len != 0) {
    fprintf(LINK->diag, "    %%na=");
    writestring(LINK->diag, &eptr->stliteral);
  }
  if (eptr->stsize > 0)
    fprintf(LINK->diag, "    %%sz=%d", eptr->stsize);
  if (eptr->strecurflag)
    fprintf(LINK->diag, "    %%rt");
  putc('\n', LINK->diag);
}

Local Void writeedgepragmas(e, LINK)
port *e;
struct LOC_dumpprogram *LINK;
{
  if (e->ptname.len != 0) {
    fprintf(LINK->diag, "%c %%na=", (Char)tab_);
    writestring(LINK->diag, &e->ptname);
  }
  if (e->ptlbound != -SHORT_MAX)
    fprintf(LINK->diag, "%c%%bd=%d,%d", (Char)tab_, e->ptlbound, e->ptubound);
  if (e->ptdfaddr > 0)
    fprintf(LINK->diag, "%c%%of=%d", (Char)tab_, e->ptdfaddr);
  if (e->ptmark == byref)
    fprintf(LINK->diag, "%c%%mk=R", (Char)tab_);
  else if (e->ptmark == byval)
    fprintf(LINK->diag, "%c%%mk=V", (Char)tab_);
  else
    fprintf(LINK->diag, "%c%%mk=D", (Char)tab_);
  if (e->ptsrcline >= 0)
    fprintf(LINK->diag, "%c%%sl=%d", (Char)tab_, e->ptsrcline);
  if (e->ptwiline >= 0)
    fprintf(LINK->diag, "%c%%wl=%d", (Char)tab_, e->ptwiline);
  if (e->ptsetrc != -SHORT_MAX)
    fprintf(LINK->diag, "%c%%sr=%d", (Char)tab_, e->ptsetrc);
  if (e->ptprodmodrc != -SHORT_MAX)
    fprintf(LINK->diag, "%c%%pm=%d", (Char)tab_, e->ptprodmodrc);
  if (e->ptconmodrc != -SHORT_MAX)   /*with*/
    fprintf(LINK->diag, "%c%%cm=%d", (Char)tab_, e->ptconmodrc);
  putc('\n', LINK->diag);
}  /*WriteEdgePragmas*/

Local Void dumpinputedges(n, LINK)
node *n;
struct LOC_dumpprogram *LINK;
{
  /* Walk the input edges of the node N and print out
     each in turn.
     Print out the pragmas associated with each edge or literal.*/
  port *e, *WITH;

  e = n->nddepilist;   /* also display any dep edges */
  if (e == NULL)
    e = n->ndilist;
  while (e != NULL) {   /* while-with */
    WITH = e;
    if (e->ptsort == ptdep)
      fprintf(LINK->diag, "D%c%d%c%d\n",
	      (Char)tab_, WITH->UU.U1.ptfrnode->ndlabel, (Char)tab_,
	      WITH->pttonode->ndlabel);
    else {
      if (e->ptsort == ptlit)
	fprintf(LINK->diag, "L%c", (Char)tab_);
      else
	fprintf(LINK->diag, "E%c%d %d",
		(Char)tab_, WITH->UU.U1.ptfrnode->ndlabel,
		WITH->UU.U1.ptfrport);
      fprintf(LINK->diag, "%c%d %d",
	      (Char)tab_, WITH->pttonode->ndlabel, WITH->pttoport);
      writetypenumber(WITH->pttype, LINK);
      if (WITH->ptsort == ptlit) {
	fprintf(LINK->diag, " \"");
	writestring(LINK->diag, &WITH->UU.ptlitvalue);
	putc('"', LINK->diag);
      }
      writeedgepragmas(e, LINK);
    }  /*else*/
    e = e->pttonext;
  }
}  /* DumpInputEdges */

Local Void writenodepragmas(n, LINK)
node *n;
struct LOC_dumpprogram *LINK;
{
  if (n->ndsrcline >= 0)
    fprintf(LINK->diag, "%c%%sl=%d", (Char)tab_, n->ndsrcline);
  if (n->ndwiline >= 0)
    fprintf(LINK->diag, "%c%%wl=%d", (Char)tab_, n->ndwiline);
  if (n->ndxcoord != -SHORT_MAX)
    fprintf(LINK->diag, "%c%%xy=%d,%d", (Char)tab_, n->ndxcoord, n->ndycoord);
  if (n->ndfrequency >= 0.0)
    fprintf(LINK->diag, "%c%%fq=% .5E", (Char)tab_, n->ndfrequency);
  if (n->ndexpanded == 1)
    fprintf(LINK->diag, "%c%%ep=1", (Char)tab_);
  else if (n->ndexpanded == 0)
    fprintf(LINK->diag, "%c%%ep=0", (Char)tab_);
  putc('\n', LINK->diag);   /*with*/
}  /*WriteNodePragmas*/

Local Void dumpsimple(n, LINK)
node *n;
struct LOC_dumpprogram *LINK;
{
  /* Print out the simple node N along with its pragmas */
/* CANN PEEK */  if ( n->ndcode == ifnpeek )
/* CANN PEEK */ fprintf(LINK->diag, "N %d%c314", n->ndlabel, (Char)tab_ );
/* CANN PEEK */ else
  fprintf(LINK->diag, "N %d%c%d", n->ndlabel, (Char)tab_, n->ndcode);
  writenodepragmas(n, LINK);
}  /* DumpSimple */

Local Void dumpcompound(n, LINK)
node *n;
struct LOC_dumpprogram *LINK;
{
  /* N is a compound node.
     This will consist of :
       (1) a header line
       (2) a list of subgraphs  (recursive call to DumpGraph)
       (3) a closing line with pragmas.
     The tagcase node is handled carefully to get the subgraph-tag
     associations correct.*/
  assoclist *atemp;
  graph *gptr;
  int count, graphcount;

  /* DumpCompound */
  fprintf(LINK->diag, "{ Compound %3d %3d\n", n->ndlabel, n->ndcode);
  /* Dump the subsidiary graphs */
  graphcount = 0;
  gptr = n->UU.U2.ndsubsid;
  while (gptr != NULL) {
    graphcount++;
    dumpgraph(gptr->grnode, LINK);
    gptr = gptr->grnext;
  }
  /* Dump the pointer list */
  if (n->ndcode == ifntagcase)   /* with N^ */
  {  /* Dump association list */
    /*find length of list*/
    count = 0;
    atemp = n->UU.U2.ndassoc;
    while (atemp != NULL) {
      count++;
      atemp = atemp->next;
    }
    fprintf(LINK->diag, "} %d %d %d", n->ndlabel, n->ndcode, count);
    /* write out the assoc list */
    atemp = n->UU.U2.ndassoc;
    while (atemp != NULL) {
      fprintf(LINK->diag, " %d", atemp->graphnum);
      atemp = atemp->next;
    }
  }  /*then*/
  else {   /*if*/
    fprintf(LINK->diag, "} %d %d %d", n->ndlabel, n->ndcode, graphcount);
    for (count = 0; count < graphcount; count++)
      fprintf(LINK->diag, " %d", count);
  }
  writenodepragmas(n, LINK);
}

Local Void dumpgraph(g, LINK)
node *g;
struct LOC_dumpprogram *LINK;
{
  /*( G : NDPtr )*/
  /* Print out the appropriate IF1 line depending on the type
     of graph this is (X, I, or G).  Then walk the nodes of
     the graph and print out each node followed by all its input
     edges.*/
  node *n;
  linkrec *WITH1;

  /* Dump graph header */
  if (g->UU.U1.ndlink == NULL)   /*with*/
  {  /* Is a subgraph of a compound node */
    putc('G', LINK->diag);
    writetypenumber(g->UU.U1.ndtype, LINK);
  } else {   /* Is a Function Graph */
    WITH1 = g->UU.U1.ndlink;
    if (WITH1->lksort == lslocal)
      putc('G', LINK->diag);
    else if (WITH1->lksort == lsimported)
      putc('I', LINK->diag);
    else
      putc('X', LINK->diag);
    writetypenumber(g->UU.U1.ndtype, LINK);
    fprintf(LINK->diag, "%c\"", (Char)tab_);
    writestring(LINK->diag, &WITH1->lkname);
    putc('"', LINK->diag);
    /* write function pragmas here */
    if (WITH1->lkarsize >= 0)
      fprintf(LINK->diag, "%c%%ar=%d", (Char)tab_, WITH1->lkarsize);
    if (WITH1->lkexpand)
      fprintf(LINK->diag, "  %%ex");
  }
  /* with */
  writenodepragmas(g, LINK);
  /* CANN */ /* dumpinputedges(g, LINK); */
  n = g->ndnext;
  while (n != NULL) {
    if (n->ndsort == ndcompound)
      dumpcompound(n, LINK);
    else
      dumpsimple(n, LINK);
    dumpinputedges(n, LINK);
    n = n->ndnext;
  }
  dumpinputedges(g, LINK); /* CANN 1/92 DUMP GRAPH INPUT EDGES LAST */
}  /* DumpGraph */

Local Void dumpif1file(diag, LINK)
FILE *diag;
struct LOC_dumpprogram *LINK;
{
  /* Module = A pointer to a dummy compound node whose subsidiary
             graphs are the graphnodes of the if file being produced.*/
  int entrynum;
  Char ch;
  node *fgraph;
  int FORLIM;

  FORLIM = tthwm;

  CANN_DumpSpecialPragmas(diag);

  /* First dump the type table */
  for (entrynum = 0; entrynum < FORLIM; entrynum++)
    dumpentry(typetable[entrynum], LINK);
  /* Dump the stamps */
  for (ch = 'A'; ch <= 'Z'; ch++) {
    if (P_inset(ch, stampset)) {
      fprintf(diag, "C$  %c", ch);
      writestring(diag, &stamp[ch - 'A']);
      putc('\n', diag);
    }
  }
  fgraph = firstfunction;
  while (fgraph != NULL) {
    dumpgraph(fgraph, LINK);
    fgraph = tonextfunction(fgraph);
  }
}  /* DumpIF1File */


/* dlz, reorganized and consolidated 8/87: 3 deep procedure nesting to 1,
   conditional comp. of vivek's stuff, new procs Write(Node/Edge)Pragmas */
Static Void dumpprogram(outfile)
stryng outfile;
{
  struct LOC_dumpprogram V;

  V.diag = NULL;
  if (openoutputfile(&outfile, &V.diag))
    dumpif1file(V.diag, &V);
  else {
    printf("*** ERROR, cannot open output file: ");
    writestring(stdout, &outfile);
    printf(" ***\n");
  }
  if (V.diag != NULL)
    fclose(V.diag);
}  /* DumpProgram */

#undef tab_


Static Void dumpmodule()
{
  stryng outfilename;

  outfilename = outfile->parvalue;
  dumpprogram(outfilename);
}  /* DumpModule */


#define tab_            9
#define maxnamelen      5
#define commtablesize   80
#define commtablemax    79   /* CommTableSize - 1 */
#define intlistlen      50


typedef struct intlist {
  char length;
  int element[intlistlen];
} intlist;

typedef Char commandname[maxnamelen];
/*List all function*/
/*List all nodes*/
/*Name current function*/
/*Goto node with Id*/
/*List input edges*/
/*List commands*/
/*Move left*/
/*display loop depth*/
/*Move right*/
/*Goto node with label*/
/*Show Current Graph*/
/*List nodes with given opcode*/
/*List all nodes with opcode (recursive)*/
/*Goto next node*/
/*List output edges*/
/*Goto parent*/
/*Shop which graph ports effect node*/
/*Show current position*/
/*Quit*/
/*Goto given subgraph*/
/*Show type*/
/*Show Nodes Above*/
/*Show Nodes Below*/
/*Display node record*/
/*Display input edge record*/
/*Display output edge record*/
/*Display input dependence edge records*/
/*Display output dependence edge records*/
/*Display all input edges*/
/*Display all output edges*/
typedef enum {
  cclisallfun, cclisallnodes, ccfunction, ccnodeid, cclisinedges, cchelp,
  ccleft, cclooplevel, ccright, ccnodelabel, ccgraph, cclisnodeswop,
  ccrecfind, ccnextnode, cclisoutedges, ccparent, ccgraphports, ccposition,
  ccquit, ccsubgraph, ccdisptype, ccnodesabove, ccnodesbelow, ccdumpnode,
  ccdumpinput, ccdumpoutput, ccdumpdepinput, ccdumpdepoutput, ccdumpallinput,
  ccdumpalloutput, ccshowtable
} commandtype;   /*Display hash table*/
/* p2c: mono.bin.noblank.p, line 8579: Note:
 * Line breaker spent 1.7+0.30 seconds, 1907 tries on line 9972 [251] */

typedef struct commandrec {
  boolean empty;
  commandname name;
  commandtype commvalue;
} commandrec;


#define prime           89


/* Local variables for graphwalk: */
struct LOC_graphwalk {
  commandrec commandtable[commtablemax + 1];
  long blankchars[9];
} ;

Local char hashfunction(name, LINK)
Char *name;
struct LOC_graphwalk *LINK;
{
  int sum, pos;

  sum = (name[0] - '0') * name[1] % prime;
/* p2c: mono.bin.noblank.p, line 8597:
 * Note: Using % for possibly-negative arguments [317] */
  for (pos = 2; pos < maxnamelen; pos++)
    sum += name[pos];
  return (sum % commtablesize);
/* p2c: mono.bin.noblank.p, line 8600:
 * Note: Using % for possibly-negative arguments [317] */
}

#undef prime

/* ---------------------------------------------------------------- */
/* A collection of routines to implement lists of integers          */
Local boolean emptylist(l, LINK)
intlist *l;
struct LOC_graphwalk *LINK;
{
  return (l->length == 0);
}  /* EmptyList */

Local Void initlist(l, LINK)
intlist *l;
struct LOC_graphwalk *LINK;
{
  l->length = 0;
}  /* InitList */

Local int poplist(l, LINK)
intlist *l;
struct LOC_graphwalk *LINK;
{
  int Result;

  Result = l->element[l->length - 1];
  l->length--;
  return Result;
}  /* PopList */

Local Void addtolist(l, ele, LINK)
intlist *l;
int ele;
struct LOC_graphwalk *LINK;
{
  int i;
  boolean found;

  /* see if ele is in list yet */
  i = 1;
  found = false;
  while (i <= l->length && !found) {
    if (l->element[i - 1] == ele)
      found = true;
    else
      i++;
  }
  if (found)
    return;
  if (i > intlistlen)
    printf("INTERNAL ERROR: List Overflow!!\n");
  else {
    l->length = i;
    l->element[i - 1] = ele;
  }
}  /* AddToList */

/* ---------------------------------------------------------------- */
Local Void displaytable(LINK)
struct LOC_graphwalk *LINK;
{
  int loc;
  commandrec *WITH;

  for (loc = 0; loc <= commtablemax; loc++) {
    WITH = &LINK->commandtable[loc];
    if (!WITH->empty)
      printf("loc: %2d  %.*s\n", loc, maxnamelen, WITH->name);
  }
}  /* DisplayTable */

/* Local variables for initcommandtable: */
struct LOC_initcommandtable {
  struct LOC_graphwalk *LINK;
} ;

Local Void insert_(cname_, command, LINK)
Char *cname_;
commandtype command;
struct LOC_initcommandtable *LINK;
{
  commandname cname;
  int loc;
  commandrec *WITH;

  mymemcpy(cname, cname_, sizeof(commandname));
  loc = hashfunction(cname, LINK->LINK);
  while (!LINK->LINK->commandtable[loc].empty) {
    loc = (loc + 1) % commtablesize;
/* p2c: mono.bin.noblank.p, line 8658:
 * Note: Using % for possibly-negative arguments [317] */
  }
  WITH = &LINK->LINK->commandtable[loc];
  WITH->empty = false;
  mymemcpy(WITH->name, cname, sizeof(commandname));
  WITH->commvalue = command;
}  /* Insert */

Local Void initcommandtable(LINK)
struct LOC_graphwalk *LINK;
{
  struct LOC_initcommandtable V;
  int loc;

  V.LINK = LINK;
  for (loc = 0; loc <= commtablemax; loc++)
    LINK->commandtable[loc].empty = true;
  insert_("above", ccnodesabove, &V);
  insert_("allf ", cclisallfun, &V);
  insert_("below", ccnodesbelow, &V);
  insert_("f    ", cclisnodeswop, &V);
  insert_("fr   ", ccrecfind, &V);
  insert_("fun  ", ccfunction, &V);
  insert_("graph", ccgraph, &V);
  insert_("gport", ccgraphports, &V);
  insert_("help ", cchelp, &V);
  insert_("id   ", ccnodeid, &V);
  insert_("iedge", cclisinedges, &V);
  insert_("label", ccnodelabel, &V);
  insert_("l    ", ccleft, &V);
  insert_("loop ", cclooplevel, &V);
  insert_("n    ", ccnextnode, &V);
  insert_("nodes", cclisallnodes, &V);
  insert_("oedge", cclisoutedges, &V);
  insert_("up   ", ccparent, &V);
  insert_("pos  ", ccposition, &V);
  insert_("q    ", ccquit, &V);
  insert_("r    ", ccright, &V);
  insert_("table", ccshowtable, &V);
  insert_("down ", ccsubgraph, &V);
  insert_("type ", ccdisptype, &V);
  insert_("dn   ", ccdumpnode, &V);
  insert_("di   ", ccdumpinput, &V);
  insert_("do   ", ccdumpoutput, &V);
  insert_("ddepi", ccdumpdepinput, &V);
  insert_("ddepo", ccdumpdepoutput, &V);
  insert_("dalli", ccdumpallinput, &V);
  insert_("dallo", ccdumpalloutput, &V);
}  /* InitCommandTable */

Local Void skipblanks(LINK)
struct LOC_graphwalk *LINK;
{
  boolean finished;

  finished = false;
  while (!finished) {
    if (P_eoln(stdin)) {
      finished = true;
      break;
    }
    if (P_inset(P_peek(stdin), LINK->blankchars))
      getc(stdin);
    else
      finished = true;
  }
}  /* SkipBlanks */

Local boolean readinteger_(i, LINK)
int *i;
struct LOC_graphwalk *LINK;
{
  Char ch;

  skipblanks(LINK);
  ch = P_peek(stdin);
  if (ch >= '0' && ch <= '9') {
    scanf("%d", i);
    return true;
  } else
    return false;
}  /* ReadInteger */

/* Local variables for getvalidcommand: */
struct LOC_getvalidcommand {
  struct LOC_graphwalk *LINK;
} ;

Local Void readcommandname(cname, LINK)
Char *cname;
struct LOC_getvalidcommand *LINK;
{
  /* Read the command entered from the command line.
       -- Skip leading blanks and tabs
       -- Reads entire character string until eoln, blank or
          tab are found
       -- Smashes chars to lower case
       -- Only records first MaxNameLen characters
       -- Pads remainder of name with blanks
       -- Does not issue a readln since arguments may be forthcomming
  */
  int i, length;
  boolean found;
  Char ch;

  skipblanks(LINK->LINK);
  found = false;
  length = 0;
  while (!found) {
    if (P_eoln(stdin)) {
      found = true;
      break;
    }
    ch = P_peek(stdin);
    if (P_inset(ch, LINK->LINK->blankchars)) {
      found = true;
      break;
    }
    getc(stdin);
    if (length < maxnamelen) {
      length++;
      cname[length - 1] = lowercase(ch);
    }
  }
  /* pad word with blanks */
  for (i = length; i < maxnamelen; i++)
    cname[i] = ' ';
}  /* ReadCommandName */

Local boolean foundintable(cname, command, LINK)
Char *cname;
commandtype *command;
struct LOC_getvalidcommand *LINK;
{
  boolean Result;
  int loc;
  boolean stilllooking;
  commandrec *WITH;

  stilllooking = true;
  loc = hashfunction(cname, LINK->LINK);
  while (stilllooking) {
    WITH = &LINK->LINK->commandtable[loc];
    if (WITH->empty) {
      stilllooking = false;
      Result = false;
      continue;
    }
    if (!strncmp(WITH->name, cname, sizeof(commandname))) {
      stilllooking = false;
      Result = true;
      *command = WITH->commvalue;
    } else {
      loc = (loc + 1) % commtablesize;
/* p2c: mono.bin.noblank.p, line 8789:
 * Note: Using % for possibly-negative arguments [317] */
    }
  }
  return Result;
}  /* FoundInTable */

Local Void getvalidcommand(command, LINK)
commandtype *command;
struct LOC_graphwalk *LINK;
{
  struct LOC_getvalidcommand V;
  boolean found;
  commandname cname;

  V.LINK = LINK;
  found = false;
  while (!found) {
    printf("GW>> ");
    readcommandname(cname, &V);
    if (foundintable(cname, command, &V)) {
      found = true;
      break;
    }
    scanf("%*[^\n]");
    getchar();
    printf("Command unknown: %.*s\n", maxnamelen, cname);
  }
}  /* GetValidCommand */

/* -------------------------------------------------------------- */
/* The following procedures  the operations of the          */
/* graphwalker.            */
Local int findlooplevel(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  int level;
  boolean found;

  if (n == module)
    return -1;
  else {
    level = 0;
    found = false;
    while (!found) {
      if (n->ndsort == ndgraph) {
	switch (graphkind(n)) {

	case ifgselector:
	case ifgalternative:
	case ifgvariant:
	case ifgloopainit:
	case ifgloopbinit:
	case ifgifpredicate:
	case ifgiftrue:
	case ifgiffalse:
	  /* blank case */
	  break;

	case ifgloopabody:
	case ifgloopatest:
	case ifgloopareturns:
	case ifgloopbbody:
	case ifgloopbtest:
	case ifgloopbreturns:
	case ifgforallgenerator:
	case ifgforallbody:
	case ifgforallreturns:
	  level++;
	  break;

	case ifgfunction:
	  found = true;
	  break;
	}/* case */
      }
      n = n->ndparent;
    }
    return level;
  }
}  /* FindLoopLevel */

Local int findlexicallevel(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  /* Function graphs are considered to be at level 0
     and the level number increases each time you dive
     into a the subgraphs of a compound node
   */
  int level;

  if (n == module)
    return -1;
  else {
    level = -1;
    do {
      n = toenclosingcompound(n);
      level++;
    } while (n != NULL);
    return level;
  }
}  /* FindLexicalLevel */

Local Void displayedge(e, LINK)
port *e;
struct LOC_graphwalk *LINK;
{
  if (e == NULL) {
    printf("Error:  nil edge pointer\n");
    return;
  }
  printf("%4d", e->ptif1line);
  if (e->ptsort == ptedge)
    printf(":  E  %2d %2d", e->UU.U1.ptfrnode->ndlabel, e->UU.U1.ptfrport);
  else
    printf(":  L       ");
  printf("     %2d %2d", e->pttonode->ndlabel, e->pttoport);
  printf("  Type: %3d", e->pttype->stlabel);
  if (e->ptsort == ptlit) {
    printf("  \"");
    writestring(stdout, &e->UU.ptlitvalue);
    putchar('"');
  }
  if (e->ptname.len != 0) {
    printf("  Name: ");
    writestring(stdout, &e->ptname);
  }
  if (e->ptdfaddr > 0)
    printf("   %%of=%d", e->ptdfaddr);
  putchar('\n');
}  /* DisplayEdge */

Local Void displaynode(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  int i, pos;
  printable name;
  ifgraphtype gkind;

  if (n == NULL) {
    printf("ERROR: Nil node pointer\n");
    return;
  }
  printf("%4d:", n->ndline);
  if (n->ndsort == ndgraph) {
    printf("  G %2d   ", n->ndlabel);
    gkind = graphkind(n);
    pos = 1;
    mymemcpy(name, graphname[(long)gkind], sizeof(printable));
    while (name[pos - 1] != ' ') {
      putchar(name[pos - 1]);
      pos++;
    }
    if (gkind == ifgfunction) {
      putchar(' ');
      writestring(stdout, &n->UU.U1.ndlink->lkname);
      pos += n->UU.U1.ndlink->lkname.len + 1;
    }
    for (i = pos + 1; i <= 19; i++)
      putchar(' ');
  } else {
    printf("  N %2d   ", n->ndlabel);
    pos = 1;
    mymemcpy(name, nodename[n->ndcode], sizeof(printable));
    while (name[pos - 1] != ' ') {
      putchar(name[pos - 1]);
      pos++;
    }
    for (i = pos + 1; i <= 19; i++)
      putchar(' ');
  }
  printf("  Id: %4d   Level: %2d\n", n->ndid, findlexicallevel(n, LINK));
}  /* DisplayNode */

Local Void writetype(t)
stentry *t;
{
  if (t == NULL) {
    printf("Nil");
    return;
  }
  switch (t->stsort) {

  case iftarray:
    printf("Array[ ");
    writetype(t->UU.stbasetype);
    printf(" ] ");
    break;

  case iftbasic:
    writestring(stdout, &t->stliteral);
    break;

  case iftfunctiontype:
    printf("Function ( ");
    writetype(t->UU.U3.starg);
    printf(" RETURNS ");
    writetype(t->UU.U3.stres);
    putchar(')');
    break;

  case iftmultiple:
    printf("Multiple[ ");
    writetype(t->UU.stbasetype);
    printf(" ] ");
    break;

  case iftrecord:
    printf("Record[ ");
    writetype(t->UU.stbasetype);
    printf(" ] ");
    break;

  case iftstream:
    printf("Stream[ ");
    writetype(t->UU.stbasetype);
    printf(" ] ");
    break;

  case ifttuple:
  case iftfield:
  case ifttag:
    writetype(t->UU.U2.stelemtype);
    if (t->UU.U2.stnext != NULL) {
      printf(", ");
      writetype(t->UU.U2.stnext);
    }
    break;

  case iftunion:
    printf("Union[ ");
    writetype(t->UU.stbasetype);
    printf(" ] ");
    break;

  case iftbuffer:
    printf("Buffer[ ");
    writetype(t->UU.stbasetype);
    printf("] ");
    break;

  case 10:
    printf(" special structured Wild type ");
    break;
  }/* case */
}  /* WriteType */

Local Void displaytype(LINK)
struct LOC_graphwalk *LINK;
{
  int typenum;

  if (!readinteger_(&typenum, LINK)) {
    printf("Invalid Integer Argument\n");
    return;
  }
  if (typenum < 1 || typenum > tthwm) {
    printf("Integer argument out of range: %d\n", typenum);
    return;
  }
  printf("Type %3d = ", typenum);
  writetype(typetable[typenum - 1]);
  putchar('\n');
}  /* DisplayType */

Local Void marknode(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  n->ndid = -n->ndid;
}

Local boolean nodemarked(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  return (n->ndid < 0);
}

Local Void unmarkgraph(g, count, LINK)
node *g;
int count;
struct LOC_graphwalk *LINK;
{
  node *n;

  n = g->ndnext;
  while (count > 0) {
    if (n->ndid < 0) {
      count--;
      n->ndid = -n->ndid;
    }
    n = n->ndnext;
  }
}

Local Void gatherboundaryedges(l, n, mcount, LINK)
intlist *l;
node *n;
int *mcount;
struct LOC_graphwalk *LINK;
{
  node *pn;
  port *e;

  /* assume N is not marked */
  (*mcount)++;
  marknode(n, LINK);
  e = n->ndilist;
  while (e != NULL) {
    if (e->ptsort == ptedge) {
      pn = producernodeofedge(e);
      if (pn->ndsort == ndgraph)
	addtolist(l, producerportnumber(e), LINK);
      else if (!nodemarked(pn, LINK))
	gatherboundaryedges(l, pn, mcount, LINK);
    }
    e = e->pttonext;
  }
}  /* GatherBoundaryEdges */

Local Void listallfuns(LINK)
struct LOC_graphwalk *LINK;
{
  node *fun;

  fun = firstfunction;
  while (fun != NULL) {
    displaynode(fun, LINK);
    fun = tonextfunction(fun);
  }
}  /* ListAllFuns */

Local Void listallnodes(current, LINK)
node *current;
struct LOC_graphwalk *LINK;
{
  node *g, *n;

  if (current->ndsort == ndgraph)
    g = current;
  else
    g = current->ndparent;
  n = g->ndnext;
  if (n == NULL) {
    printf("Graph is Empty\n");
    return;
  }
  while (n != NULL) {
    displaynode(n, LINK);
    n = n->ndnext;
  }
}  /* ListAllNodes */

Local Void gotonextnode(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  node *n;

  n = (*cursor)->ndnext;
  if (n == NULL)
    printf("At end of graph\n");
  else {
    *cursor = n;
    displaynode(*cursor, LINK);
  }
}  /* GotoNextNode */

Local Void gotoparent(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  if (*cursor == module) {
    printf("How did you get here?  Module is off limits!\n");
    return;
  }
  if ((*cursor)->ndparent == module)
    printf("At top of function, you can't move up\n");
  else {
    *cursor = (*cursor)->ndparent;
    displaynode(*cursor, LINK);
  }
}  /* GotoParent */

Local Void showcurrfun(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  boolean finished;

  if (cursor == module) {
    printf("At Top Of Graph\n");
    return;
  }
  finished = false;
  while (!finished) {
    if (cursor->ndsort == ndgraph) {
      if (cursor->UU.U1.ndlink != NULL)
	finished = true;
      else
	cursor = cursor->ndparent;
    } else
      cursor = cursor->ndparent;
  }
  displaynode(cursor, LINK);
}  /* ShowCurrFun */

Local Void gotonodeid(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  node *n;
  int id;

  if (!readinteger_(&id, LINK)) {
    printf("Error: Bad node Id\n");
    return;
  }
  if (id < 1 || id > univnodecnt) {
    printf("Argument out of range: %d\n", id);
    return;
  }
  n = toenclosingfunction(*cursor);
  n = getnodewithid(n, id);
  if (n == NULL)
    printf("No such node in current function\n");
  else {
    *cursor = n;
    displaynode(*cursor, LINK);
  }
}  /* GotoNodeId */

Local Void showoutedges(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  port *e;
  int port_;

  if (readinteger_(&port_, LINK)) {
    e = getoutputedge(cursor, port_);
    if (e == NULL) {
      printf("No output edges from port %d\n", port_);
      return;
    }
    do {
      displayedge(e, LINK);
      e = nextoutputedgesameport(e);
    } while (e != NULL);
    return;
  }
  e = cursor->ndolist;
  if (e == NULL) {
    printf("No Output Edges\n");
    return;
  }
  while (e != NULL) {
    displayedge(e, LINK);
    e = e->UU.U1.ptfrnext;
  }
}  /* ShowOutEdges */

Local Void showinedges(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  port *e;
  int port_;

  if (readinteger_(&port_, LINK)) {
    e = getinputedge(cursor, port_);
    if (e == NULL)
      printf("No input edges to port %d\n", port_);
    else
      displayedge(e, LINK);
    return;
  }
  e = cursor->ndilist;
  if (e == NULL) {
    printf("No Input Edges\n");
    return;
  }
  while (e != NULL) {
    displayedge(e, LINK);
    e = e->pttonext;
  }

  /* No argument, display all edges */
}  /* ShowInEdges */

Local Void gotosubgraph(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  int graphnum;

  if ((*cursor)->ndsort != ndcompound) {
    printf("Not at a compound node\n");
    return;
  }
  if (!readinteger_(&graphnum, LINK)) {
    printf("Invalid integer argument\n");
    return;
  }
  if ((graphnum < 0) | (graphnum >= numbofsubgraphs(*cursor)))
    printf("No Subgraph numbered: %d\n", graphnum);
  else {
    *cursor = tochildgraph(*cursor, graphnum);
    displaynode(*cursor, LINK);
  }
}  /* GotoSubgraph */

Local Void gotonodelabel(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  node *g, *n;
  int lab;

  if (*cursor == module)
    g = *cursor;
  else if ((*cursor)->ndsort == ndgraph)
    g = *cursor;
  else
    g = (*cursor)->ndparent;
  if (!readinteger_(&lab, LINK)) {
    printf("Invalid Integer argument\n");
    return;
  }
  n = getnodewithlabel(g, lab);
  if (n == NULL)
    printf("No node with label: %d\n", lab);
  else {
    *cursor = n;
    displaynode(*cursor, LINK);
  }
}  /* GotoNodeLabel */

/* Local variables for findnodes: */
struct LOC_findnodes {
  struct LOC_graphwalk *LINK;
  boolean recursive;
} ;

Local Void searchgraph(g, opcode, LINK)
node *g;
int opcode;
struct LOC_findnodes *LINK;
{
  node *n;
  int graphnum, FORLIM;

  n = g->ndnext;
  while (n != NULL) {
    if (n->ndcode == opcode)
      displaynode(n, LINK->LINK);
    if (LINK->recursive && n->ndsort == ndcompound) {
      FORLIM = numbofsubgraphs(n);
      for (graphnum = 0; graphnum < FORLIM; graphnum++)
	searchgraph(tochildgraph(n, graphnum), opcode, LINK);
    }
    n = n->ndnext;
  }
}  /* SearchGraph */

Local Void findnodes(cursor, recursive_, LINK)
node *cursor;
boolean recursive_;
struct LOC_graphwalk *LINK;
{
  struct LOC_findnodes V;
  node *g;
  int opcode;

  V.LINK = LINK;
  V.recursive = recursive_;
  if (cursor == module)
    g = cursor;
  else if (cursor->ndsort == ndgraph)
    g = cursor;
  else
    g = cursor->ndparent;
  if (readinteger_(&opcode, LINK))
    searchgraph(g, opcode, &V);
  else
    printf("Invalid Integer Argument\n");
}  /* FindNodes */

Local Void moveright_(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  /* If Cursor is positioned in a subgraph of a compound
     node, move to the sibling graph to the right (if one
     exists)
   */
  node *g;
  graph *gtemp;

  if (*cursor == module) {
    printf("Not in Subgraph of Compound Node\n");
    return;
  }
  if ((*cursor)->ndsort == ndgraph)
    g = *cursor;
  else
    g = (*cursor)->ndparent;
  gtemp = g->ndparent->UU.U2.ndsubsid;
  while (gtemp->grnode != g)
    gtemp = gtemp->grnext;
  if (gtemp->grnext == NULL)
    printf("No Graph to the right\n");
  else {
    *cursor = gtemp->grnext->grnode;
    displaynode(*cursor, LINK);
  }
}  /* MoveRight */

Local Void moveleft_(cursor, LINK)
node **cursor;
struct LOC_graphwalk *LINK;
{
  /* If Cursor is positioned in a subgraph of a compound
     node, move to the sibling graph to the left (if one
     exists)
   */
  node *g;
  graph *gtemp;

  if (*cursor == module) {
    printf("Not in Subgraph of Compound Node\n");
    return;
  }
  if ((*cursor)->ndsort == ndgraph)
    g = *cursor;
  else
    g = (*cursor)->ndparent;
  gtemp = g->ndparent->UU.U2.ndsubsid;
  if (gtemp->grnode == g) {
    printf("No subgraph to the left\n");
    return;
  }
  while (gtemp->grnext->grnode != g)
    gtemp = gtemp->grnext;
  *cursor = gtemp->grnode;
  displaynode(*cursor, LINK);
}  /* MoveLeft */

Local Void shownodesabove(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  int port_;
  port *e;
  intlist l;
  node *g;

  if (readinteger_(&port_, LINK)) {
    e = getinputedge(cursor, port_);
    if (e == NULL) {
      printf("No edge connected to input port: %d\n", port_);
      return;
    }
    if (e->ptsort == ptlit)
      printf("Literal connected to port: %d\n", port_);
    else
      displaynode(producernodeofedge(e), LINK);
    return;
  }
  initlist(&l, LINK);
  if (cursor->ndsort == ndgraph)
    g = cursor;
  else
    g = cursor->ndparent;
  e = cursor->ndilist;
  while (e != NULL) {
    if (e->ptsort == ptedge)
      addtolist(&l, nodelabel(producernodeofedge(e)), LINK);
    e = e->pttonext;
  }
  if (emptylist(&l, LINK))
    printf("Not Dependant on any nodes\n");
  else {
    while (!emptylist(&l, LINK))
      displaynode(getnodewithlabel(g, poplist(&l, LINK)), LINK);
  }
}  /* ShowNodesAbove */

Local Void shownodesbelow(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  int port_;
  port *e;
  intlist l;
  node *g;

  initlist(&l, LINK);
  if (cursor->ndsort == ndgraph)
    g = cursor;
  else
    g = cursor->ndparent;
  if (readinteger_(&port_, LINK)) {
    e = getoutputedge(cursor, port_);
    if (e == NULL)
      printf("No edges connected to output port: %d\n", port_);
    else {
      while (e != NULL) {
	addtolist(&l, nodelabel(e->pttonode), LINK);
	e = nextoutputedgesameport(e);
      }
    }
  } else {
    e = cursor->ndolist;
    while (e != NULL) {
      addtolist(&l, nodelabel(e->pttonode), LINK);
      e = e->UU.U1.ptfrnext;
    }
  }
  while (!emptylist(&l, LINK))
    displaynode(getnodewithlabel(g, poplist(&l, LINK)), LINK);
}  /* ShowNodesBelow */

Local Void showgraph(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  if (cursor == module) {
    printf("Not in a graph\n");
    return;
  }
  if (cursor->ndsort == ndgraph)
    displaynode(cursor, LINK);
  else
    displaynode(cursor->ndparent, LINK);
}  /* ShowGraph */

Local Void givehelp(LINK)
struct LOC_graphwalk *LINK;
{
  printf("above [port]     : List nodes directly above current node\n");
  printf("allf             : List all function in Module\n");
  printf("below [port]     : List nodes directly below current node\n");
  printf("dn               : Dump the node record of the current node\n");
  printf("di [port]        : Dump the input edge record(s)\n");
  printf("do [port]        : Dump the output edge record(s)\n");
  printf("ddepi\t\t: Dump the input dependence edge records\n");
  printf("ddepo\t\t: Dump the output dependence edge records\n");
  printf("f <opcode>       : List all nodes in current graph with the given opcode\n");
  printf("f <opcode>       : List all nodes in function with the given opcode\n");
  printf("fun              : List function containing current node\n");
  printf("gport            : List graph ports which effect current node\n");
  printf("graph            : List graph containing current node\n");
  printf("help             : Display this HELP table\n");
  printf("id <idnum>       : Move cursor to the node with this idnum\n");
  printf("iedge [port]     : List input edges of cursor\n");
  printf("l                : Move to left sibling graph\n");
  printf("label <lnum>     : Move cursor to node in current graph with this label\n");
  printf("loop    \t\t: Show cursors loop depth\n");
  printf("n                : Move to next node in the graph\n");
  printf("nodes            : List all nodes in the current graph\n");
  printf("oedge [port]     : List output edges of cursor\n");
  printf("up               : Move to parent graph or node\n");
  printf("pos              : Show current position\n");
  printf("q                : Quit the graphwalker\n");
  printf("r                : Move to the right sibling graph\n");
  printf("table            : Show distribution of commands in hash table\n");
  printf("down <grnum>     : Move to given subgraph of compound node (zero based)\n");
  printf("type <typenum>   : Show structure of given type\n");
}  /* GiveHelp */

Local Void showgraphports(cursor, LINK)
node *cursor;
struct LOC_graphwalk *LINK;
{
  intlist l;
  int markcount;

  if (cursor == module || cursor->ndsort == ndgraph) {
    printf("At graph node\n");
    return;
  }
  initlist(&l, LINK);
  markcount = 0;
  gatherboundaryedges(&l, cursor, &markcount, LINK);
  unmarkgraph(cursor->ndparent, markcount, LINK);
  printf("Graph Ports: ");
  while (!emptylist(&l, LINK))
    printf("%3d", poplist(&l, LINK));
  putchar('\n');
}  /* ShowGraphPorts */

Local Void dumpnode(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  int count;
  graph *g;
  linkrec *WITH1;

/* p2c: mono.bin.noblank.p, line 9464:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9464:
 * Note: Element has wrong type for WRITE statement [196] */
  printf("N =         <meef>\n");
  printf("NDId =      %11d   NDLabel =  %11d\n", n->ndid, n->ndlabel);
  printf("NDCode =    %11d   NDMisc. =   %11d\n", n->ndcode, n->ndmisc.numb);
/* p2c: mono.bin.noblank.p, line 9467:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9467:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9467:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9467:
 * Note: Element has wrong type for WRITE statement [196] */
  printf("NDParent =  <meef>   NDNext =   <meef>\n");
/* p2c: mono.bin.noblank.p, line 9468:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9468:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9468:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9468:
 * Note: Element has wrong type for WRITE statement [196] */
  printf("NDIList =   <meef>   NDOList =  <meef>\n");
  printf("NDLine =    %11d\n", n->ndline);
  printf("NDSrcLine = %11d   NDWiLine = %11d\n", n->ndsrcline, n->ndwiline);
  printf("NDXCoord =  %11d   NDYCoord = %11d\n", n->ndxcoord, n->ndycoord);
  switch (n->ndsort) {

  case ndatomic:
    printf("NDSort =       NDAtomic  (Simple Node)\n");
    break;

  case ndcompound:
    printf("NDSort =     NDCompound   NDAssoc =  <meef>\n");
/* p2c: mono.bin.noblank.p, line 9478:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9478:
 * Note: Element has wrong type for WRITE statement [196] */
    printf("NDSubsid =  <meef>\n");
    count = 0;
    g = n->UU.U2.ndsubsid;
    while (g != NULL) {
      printf("  Graph %d: (G = <meef>)  GRNode = <meef>  GRNext = <meef>\n",
	     count);
      g = g->grnext;
      count++;
    }
/* p2c: mono.bin.noblank.p, line 9483:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9483:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9484:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9484:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9484:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9484:
 * Note: Element has wrong type for WRITE statement [196] */
    break;
/* p2c: mono.bin.noblank.p, line 9477:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9477:
 * Note: Element has wrong type for WRITE statement [196] */

  case ndgraph:
    printf("NDSort =        NDGraph   NDType =   <meef>");
    if (n->UU.U1.ndtype != NULL)
      printf("  (STLabel = %d)\n", n->UU.U1.ndtype->stlabel);
    else
      putchar('\n');
/* p2c: mono.bin.noblank.p, line 9496:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9496:
 * Note: Element has wrong type for WRITE statement [196] */
    printf("NDLink =    <meef>\n");
    if (n->UU.U1.ndlink != NULL) {
      WITH1 = n->UU.U1.ndlink;
      switch (WITH1->lksort) {

      case lslocal:
	printf(" LKSort =     LSLocal\n");
	break;

      case lsimported:
	printf(" LKSort =     LSImported  (Global)\n");
	break;

      case lsexported:
	printf(" LKSort =     LSExported  (Defined)\n");
	break;
      }/*case*/
      printf(" LKName =     ");
      writestring(stdout, &WITH1->lkname);
      printf("\n LKGraph =   <meef>  LKNext =    <meef>\n");
/* p2c: mono.bin.noblank.p, line 9508:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9508:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9508:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9508:
 * Note: Element has wrong type for WRITE statement [196] */
      printf(" LKARSize =  %11d  LKARIndex = %11d\n",
	     WITH1->lkarsize, WITH1->lkarindex);
      if (WITH1->lkexpand)
	printf(" LKExpand =   TRUE\n");
      else
	printf(" LKExpand =   FALSE\n");
    }
    break;
/* p2c: mono.bin.noblank.p, line 9491:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9491:
 * Note: Element has wrong type for WRITE statement [196] */
  }/* case */
}  /* DumpNode */

Local Void dumponeedge(e, LINK)
port *e;
struct LOC_graphwalk *LINK;
{
/* p2c: mono.bin.noblank.p, line 9523:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9523:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9523:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9523:
 * Note: Element has wrong type for WRITE statement [196] */
  printf("E =         <meef>   PTType =    <meef>");
  if (e->pttype != NULL)
    printf("  (STLabel = %d)\n", e->pttype->stlabel);
  else
    putchar('\n');
  printf("PTIF1Line = %11d   PTName =    ", e->ptif1line);
  writestring(stdout, &e->ptname);
  printf("\nPTToNode =  <meef>   (NodeID = %d)\n", e->pttonode->ndid);
/* p2c: mono.bin.noblank.p, line 9531:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9531:
 * Note: Element has wrong type for WRITE statement [196] */
/* p2c: mono.bin.noblank.p, line 9533:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9533:
 * Note: Element has wrong type for WRITE statement [196] */
  printf("PTToNext =  <meef>   PTToPort =  %11d\n", e->pttoport);
  printf("PTSrcLine = %11d   PTWiLine =  %11d\n", e->ptsrcline, e->ptwiline);
  printf("PTId =      %11d", e->ptid);
  if (e->ptmark == byref)
    printf("   PTMark =          ByRef\n");
  else if (e->ptmark == byval)
    printf("   PTMark =          ByVal\n");
  else
    printf("   PTMark =          ByDefault\n");
  printf("PTLBound =  %11d   PTUBound =  %11d\n", e->ptlbound, e->ptubound);
  printf("PTDFAddr =  %11d\n", e->ptdfaddr);
  if (e->ptsort == ptedge) {
    printf("PTSort =         PTEdge\n");
/* p2c: mono.bin.noblank.p, line 9547:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9547:
 * Note: Element has wrong type for WRITE statement [196] */
    printf("PTFrNode =  <meef>   (^.NDId  = %d)\n", e->UU.U1.ptfrnode->ndid);
/* p2c: mono.bin.noblank.p, line 9549:
 * Internal error in writeelement: got a char * instead of a string [214] */
/* p2c: mono.bin.noblank.p, line 9549:
 * Note: Element has wrong type for WRITE statement [196] */
    printf("PTFrNext =  <meef>   PTFrPort =  %11d\n", e->UU.U1.ptfrport);
    return;
  }
  printf("PTSort =          PTLit\n");
  printf("PTLitValue =     ");
  writestring(stdout, &e->UU.ptlitvalue);
  putchar('\n');
}  /* DumpOneEdge */

Local Void dumpdepinputedges(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  port *e;

  e = n->nddepilist;
  if (e == NULL)
    printf("This node as no input dependence edges. \n");
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = e->pttonext;
    if (e != NULL) {
      if (e->pttoport != 0)
	e = NULL;
      else
	printf("error, dep edges not connected to real edges\n");
    }
    if (e != NULL)
      printf("---------------\n");
  }
}

Local Void dumpdepoutputedges(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  port *e;

  e = n->nddepolist;
  if (e == NULL)
    printf("This node as no output dependence edges. \n");
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = nextoutputedgesameport(e);
    if (e != NULL)
      printf("---------------\n");
  }
}

Local Void dumpinputedges_(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  int port_;
  port *e;

  if (readinteger_(&port_, LINK)) {
    e = getinputedge(n, port_);
    if (e == NULL)
      printf("No input edge on port # %d\n", port_);
    else
      dumponeedge(e, LINK);
    return;
  }
  e = n->ndilist;
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = e->pttonext;
    if (e != NULL)
      printf("---------------\n");
  }

  /* Dump All Input Edges */
}

Local Void dumpallinput(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{  /* Dump All Input Edges */
  port *e;

  if (n->nddepilist == NULL)
    e = n->ndilist;
  else
    e = n->nddepilist;
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = e->pttonext;
    if (e != NULL)
      printf("---------------\n");
  }
}

Local Void dumpoutputedges(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{
  int port_;
  port *e;

  if (readinteger_(&port_, LINK)) {
    e = getoutputedge(n, port_);
    if (e == NULL) {
      printf("No output edge on port # %d\n", port_);
      return;
    }
    while (e != NULL) {
      dumponeedge(e, LINK);
      e = nextoutputedgesameport(e);
      if (e != NULL)
	printf("---------------\n");
    }
    return;
  }
  e = n->ndolist;
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = e->UU.U1.ptfrnext;
    if (e != NULL)
      printf("---------------\n");
  }

  /* Dump All Input Edges */
}  /* DumpOutputEdges */

Local Void dumpalloutput(n, LINK)
node *n;
struct LOC_graphwalk *LINK;
{  /* Dump All Output Edges */
  port *e;

  if (n->nddepolist == NULL)
    e = n->ndolist;
  else
    e = n->nddepolist;
  while (e != NULL) {
    dumponeedge(e, LINK);
    e = e->UU.U1.ptfrnext;
    if (e != NULL)
      printf("---------------\n");
  }
}


Static node *graphwalk(start)
node *start;
{
  struct LOC_graphwalk V;
  node *Result, *cursor;
  commandtype command;

  P_addset(P_expset(V.blankchars, 0L), ' ');
  P_addset(V.blankchars, (Char)tab_);
  initcommandtable(&V);
  cursor = start;
  command = ccposition;
  displaynode(cursor, &V);
  while (command != ccquit) {
    getvalidcommand(&command, &V);
    switch (command) {

    case cclisallfun:
      listallfuns(&V);
      break;

    case cclisallnodes:
      listallnodes(cursor, &V);
      break;

    case ccrecfind:
      findnodes(cursor, true, &V);
      break;

    case ccfunction:
      showcurrfun(cursor, &V);
      break;

    case cchelp:
      givehelp(&V);
      break;

    case ccnodeid:
      gotonodeid(&cursor, &V);
      break;

    case cclisinedges:
      showinedges(cursor, &V);
      break;

    case cclooplevel:
      printf("Loop Level: %d\n", findlooplevel(cursor, &V));
      break;

    case ccgraphports:
      showgraphports(cursor, &V);
      break;

    case ccnodelabel:
      gotonodelabel(&cursor, &V);
      break;

    case cclisnodeswop:
      findnodes(cursor, false, &V);
      break;

    case ccnextnode:
      gotonextnode(&cursor, &V);
      break;

    case cclisoutedges:
      showoutedges(cursor, &V);
      break;

    case ccparent:
      gotoparent(&cursor, &V);
      break;

    case ccposition:
      displaynode(cursor, &V);
      break;

    case ccquit:
      Result = cursor;
      break;

    case ccshowtable:
      displaytable(&V);
      break;

    case ccsubgraph:
      gotosubgraph(&cursor, &V);
      break;

    case ccdisptype:
      displaytype(&V);
      break;

    case ccright:
      moveright_(&cursor, &V);
      break;

    case ccleft:
      moveleft_(&cursor, &V);
      break;

    case ccnodesabove:
      shownodesabove(cursor, &V);
      break;

    case ccnodesbelow:
      shownodesbelow(cursor, &V);
      break;

    case ccgraph:
      showgraph(cursor, &V);
      break;

    case ccdumpnode:
      dumpnode(cursor, &V);
      break;

    case ccdumpinput:
      dumpinputedges_(cursor, &V);
      break;

    case ccdumpoutput:
      dumpoutputedges(cursor, &V);
      break;

    case ccdumpdepinput:
      dumpdepinputedges(cursor, &V);
      break;

    case ccdumpdepoutput:
      dumpdepoutputedges(cursor, &V);
      break;

    case ccdumpallinput:
      dumpallinput(cursor, &V);
      break;

    case ccdumpalloutput:
      dumpalloutput(cursor, &V);
      break;
    }/* case */
    scanf("%*[^\n]");
    getchar();
  }
  return Result;
}  /* GraphWalk */

#undef tab_
#undef maxnamelen
#undef commtablesize
#undef commtablemax
#undef intlistlen


/***********************NODE AND EDGE COUNT ROUTINES *******************/
extern Void createlevel PP((lexleveltype curlex));

extern Void outtype PP((FILE *fil, stentry *typ, int printname));


Static int nextnodenum()
{
  int Result;

  Result = univnodecnt;
  univnodecnt++;
  return Result;
}


Static int nextedgenum()
{
  int Result;

  Result = univedgecnt;
  univedgecnt++;
  return Result;
}


Static int nextsgnodenum(fromlevel)
treestackobj *fromlevel;
{
  int Result;
  treestackobj *level;

  level = fromlevel;
  while (level->lleveltag == let)
    level = level->lastlevel;
  Result = level->nodecounter;
  level->nodecounter++;
  return Result;
}  /* NextSGNodeNum */


Static int nextreturnport(fromlevel)
treestackobj *fromlevel;
{
  int Result;
  treestackobj *level;

  level = fromlevel;
  while (level->lleveltag == let)
    level = level->lastlevel;
  switch (level->lleveltag) {

  case forloop:
  case specfa:
    level->UU.U0.freturnportnum++;
    Result = level->UU.U0.freturnportnum;
    break;

  case initloop:
    level->UU.U5.ireturnportnum++;
    Result = level->UU.U5.ireturnportnum;
    break;
  }/*case*/
  return Result;
}


Static Void nextbodyportname(level, str)
treestackobj *level;
stryng *str;
{
  level->UU.U0.bodyoutputportnum++;
  integerstring(str, level->UU.U0.bodyoutputportnum, 10);
  stripspaces(str);
  insertchar(str, '|', 1);
}


/*------------------  MEMORY MANAGEMENT --------------------*/
Static Void freesymptr(sym)
symtblbucket *sym;
{
  /* Free(sym); */
}


Static Void freetreestack(tree)
treestackobj *tree;
{
  /* Free(tree); */
}


Static Void newsemantic(ptr, tagtype)
semanticrec **ptr;
typesemanticdata tagtype;
{
  PSBLOCK b;

  /* *ptr = (semanticrec *)Malloc(sizeof(semanticrec)); */
  b = MySBlockAlloc(); /* CANN */
  *ptr = &(b->sr); /* CANN */

  (*ptr)->next = NULL;
  (*ptr)->tag = tagtype;
  switch (tagtype) {

  case tgnamelist:
    (*ptr)->UU.namelist = NULL;
    break;

  case tgtypelist:
    (*ptr)->UU.typelist = NULL;
    break;

  case tgttptr:
    (*ptr)->UU.ttptr = NULL;
    break;

  case tgsymptr:
    (*ptr)->UU.symptr = NULL;
    break;

  case tgexplist:
    (*ptr)->UU.explist = NULL;
    break;

  case tgpreclevel:
    (*ptr)->UU.U5.preclevel = -SHORT_MAX;
    (*ptr)->UU.U5.operation = noop;
    (*ptr)->UU.U5.line = -1;
    (*ptr)->UU.U5.column = -1;
    break;

  case tgsymlist:
    (*ptr)->UU.symlist = NULL;
    break;

  case tgnodeptr:
    (*ptr)->UU.nodeptr = NULL;
    break;

  case tgsubgraphnum:
    (*ptr)->UU.subgraphnum = 0;
    break;

  case tgoldflag:
    (*ptr)->UU.oldflag = false;
    break;

  case tgreduceflag:
    (*ptr)->UU.redflag = rfnone;
    break;

  case tgrednodetype:
    (*ptr)->UU.rednodetype = 0;
    break;

  case tgtestflag:
    (*ptr)->UU.testflag = neitherloop;
    break;

  case tgtcstacklist:
    (*ptr)->UU.tcstacklist = NULL;
    break;
  }/* case */
}


Static Void freesemantic(ptr)
semanticrec **ptr;
{
  Free(*ptr);
}


Static Void newnamelist(ptr)
namelistrec **ptr;
{
  PBBLOCK b;

  /* *ptr = (namelistrec *)Malloc(sizeof(namelistrec)); */
  b = MyBBlockAlloc(); /* CANN */
  *ptr = &(b->nlr); /* CANN */

  mymemcpy((*ptr)->name.str, blankstring, sizeof(stryngar));
  (*ptr)->name.len = 0;
  (*ptr)->next = NULL;
  (*ptr)->prev = NULL;
  (*ptr)->linenum = 0;
  (*ptr)->column = 0;
}


Static Void freenamelist(ptr)
namelistrec **ptr;
{
  Free(*ptr);
}


Static Void newtypelist(ptr)
typelistrec **ptr;
{
  PSBLOCK b;

  /* *ptr = (typelistrec *)Malloc(sizeof(typelistrec)); */
  b = MySBlockAlloc(); /* CANN */
  *ptr = &(b->tlr); /* CANN */

  (*ptr)->typeptr = NULL;
  (*ptr)->next = NULL;
  (*ptr)->prev = NULL;
  (*ptr)->linenum = 0;
}


Static Void freetypelist(ptr)
typelistrec **ptr;
{
  Free(*ptr);
}


Static Void newtornlist(ptr, tag)
tornrec **ptr;
torntype tag;
{
  *ptr = (tornrec *)Malloc(sizeof(tornrec));
  (*ptr)->torntag = tag;
  switch (tag) {

  case typetag:
    (*ptr)->UU.ttptr = NULL;
    break;

  case symtag:
    (*ptr)->UU.symptr = NULL;
    break;
  }
  (*ptr)->next = NULL;
  (*ptr)->prev = NULL;
}


Static Void freetornlist(ptr)
tornrec **ptr;
{
  Free(*ptr);
}


Static Void newttptr(ptr, tagtype)
stentry **ptr;
char tagtype;
{
  PBBLOCK b;
  /* *ptr = (stentry *)Malloc(sizeof(stentry)); */
  b = MyBBlockAlloc(); /* CANN */
  *ptr = &(b->i); /* CANN */

  (*ptr)->stid = -1;
  (*ptr)->stlabel = -1;
  (*ptr)->stequivchain = *ptr;
  mymemcpy((*ptr)->stliteral.str, blankstring, sizeof(stryngar));
  (*ptr)->stliteral.len = 0;
  (*ptr)->stsize = 0;
  (*ptr)->strecurflag = false;
  (*ptr)->stsort = tagtype;
  switch (tagtype) {

  case iftwild:
    /* blank case */
    break;

  case iftbasic:
    (*ptr)->UU.stbasic = 0;
    break;

  case iftfunctiontype:
    (*ptr)->UU.U3.starg = NULL;
    (*ptr)->UU.U3.stres = NULL;
    break;

  case iftarray:
  case iftstream:
  case iftmultiple:
  case iftrecord:
  case iftunion:
    (*ptr)->UU.stbasetype = NULL;
    break;

  case iftfield:
  case ifttuple:
  case ifttag:
    (*ptr)->UU.U2.stelemtype = NULL;
    (*ptr)->UU.U2.stnext = NULL;
    break;
  }/* case */
}


Static Void freettptr(ptr)
stentry **ptr;
{
  Free(*ptr);
}


Static node *newnodeptr(ntype, level)
nodesort ntype;
treestackobj *level;
{
  node *node_;

  node_ = newnodealloc(ntype);
  node_->ndid = nextnodenum();
  node_->ndlabel = nextsgnodenum(level);   /* with */
  return node_;
}  /* NewNode */


Static Void freenodeptr(node_)
node **node_;
{
  Free(*node_);
}


Static port *newedgeptr(ptype)
portsort ptype;
{
  port *edge;

  edge = newedgealloc(ptype);
  edge->ptid = nextedgenum();
  return edge;
}


Static Void freeedgeptr(edge)
port **edge;
{
  Free(*edge);
}


Static linkrec *newlinkageptr(linkagesort)
linksort linkagesort;
{
  linkrec *lk;
  PBBLOCK b;

  /* lk = (linkrec *)Malloc(sizeof(linkrec)); */
  b = MyBBlockAlloc(); /* CANN */
  lk = &(b->lr); /* CANN */

  lk->lksort = linkagesort;
  lk->lkgraph = NULL;
  mymemcpy(lk->lkname.str, blankstring, sizeof(stryngar));
  lk->lkname.len = 0;
  mymemcpy(lk->lkmodulename.str, blankstring, sizeof(stryngar));
  lk->lkmodulename.len = 0;
  lk->lkarsize = -1;
  lk->lkexpand = false;
  lk->lkarindex = -1;
  lk->lknext = NULL;   /*with*/
  return lk;
}  /*NewLinkagePtr*/


Static Void freelinkageptr(lk)
linkrec **lk;
{
  Free(*lk);
}


Static graph *newgraphptr()
{
  graph *gr;
  PSBLOCK b;

  /* gr = (graph *)Malloc(sizeof(graph)); */
  b = MySBlockAlloc(); /* CANN */
  gr = &(b->g); /* CANN */

  gr->grnode = NULL;
  gr->grnext = NULL;
  return gr;
}


Static Void freegraphptr(gr)
graph *gr;
{
  /* Free(gr); */
}


Static edgelisthead *newheadptr()
{
  edgelisthead *head;
  PBBLOCK b;

  /* head = (edgelisthead *)Malloc(sizeof(edgelisthead)); */
  b = MyBBlockAlloc(); /* CANN */
  head = &(b->elh); /* CANN */

  mymemcpy(head->edgename.str, blankstring, sizeof(stryngar));
  head->edgename.len = 0;
  head->edges = NULL;
  head->next = NULL;
  return head;
}


Static Void freeheadptr(head)
edgelisthead **head;
{
  Free(*head);
}


Static edgelist *newedgelistptr()
{
  edgelist *edgelist_;
  PSBLOCK b;

  /* edgelist_ = (edgelist *)Malloc(sizeof(edgelist)); */
  b = MySBlockAlloc(); /* CANN */
  edgelist_ = &(b->el); /* CANN */

  edgelist_->edge = NULL;
  edgelist_->next = NULL;
  edgelist_->toorfrom = noport;
  return edgelist_;
}


Static Void freeedgelistptr(edgelist_)
edgelist **edgelist_;
{
  Free(*edgelist_);
}


Static refrec *newreferencesptr()
{
  refrec *refptr_;
  PSBLOCK b;

  /* refptr_ = (refrec *)Malloc(sizeof(refrec)); */
  b = MySBlockAlloc(); /* CANN */
  refptr_ = &(b->rr); /* CANN */

  refptr_->front = NULL;
  refptr_->back = NULL;
  return refptr_;
}


Static Void freereferencesptr(refptr_)
refrec **refptr_;
{
  Free(*refptr_);
}


Static inforecord *newinfoptr()
{
  inforecord *ptr;
  PBBLOCK b;

  /* ptr = (inforecord *)Malloc(sizeof(inforecord)); */
  b = MyBBlockAlloc(); /* CANN */
  ptr = &(b->ir);      /* CANN */

  ptr->node_ = NULL;
  ptr->port_ = -1;
  ptr->typeptr = NULL;
  mymemcpy(ptr->name.str, blankstring, sizeof(stryngar));
  ptr->name.len = 0;
  mymemcpy(ptr->litvalue.str, blankstring, sizeof(stryngar));
  ptr->litvalue.len = 0;
  ptr->onlist = NULL;
  return ptr;
}


Static Void freeinfoptr(ptr)
inforecord **ptr;
{
  Free(*ptr);
}


Static explistnode *newexplistptr()
{
  explistnode *explist;
  PSBLOCK b;

  /* explist = (explistnode *)Malloc(sizeof(explistnode)); */
  b = MySBlockAlloc(); /* CANN */
  explist = &(b->eln); /* CANN */

  explist->graphinfo = newinfoptr();
  explist->next = NULL;
  return explist;
}  /* NewExpList */


Static Void freeexplistptr(explist)
explistnode **explist;
{
  Free(*explist);
}


Static tcstacklistrec *newtcstacklist()
{
  tcstacklistrec *ptr;

  ptr = (tcstacklistrec *)Malloc(sizeof(tcstacklistrec));
  mymemcpy(ptr->tagname.str, blankstring, sizeof(stryngar));
  ptr->tagname.len = 0;
  ptr->sgnum = 0;
  ptr->typeptr = NULL;
  ptr->next = NULL;
  return ptr;
}  /* NewExpList */


Static Void freetcstacklist(ptr)
tcstacklistrec **ptr;
{
  Free(*ptr);
}


Static errorrecord *newerrorptr(etype)
errortypes etype;
{
  errorrecord *newe;

  newe = (errorrecord *)Malloc(sizeof(errorrecord));
  newe->etype = etype;
  newe->linenumber = -1;
  newe->column = -1;
  switch (etype) {

  case tnameundef:
  case vnameundef:
  case fndoubdef:
  case tagnameddef:
  case fieldnameddef:
  case defineerr:
  case doubledefarg:
  case forwardtype:
  case expimporterr:
  case forwardnotresolved:
  case undeffunct:
  case atypenameerr:
  case vnamenotloopvar:
  case undeftagub:
  case tnonubuild:
  case nameundrs:
  case ntypeonrs:
  case nameundrr:
  case typeclashrr:
  case undefntag:
  case nameundeftc:
  case ddtname:
  case notalltc:
  case vardoubledef:
  case arityonfa:
  case atinrg:
  case decbutnotdef:
  case lvarnotdef:
  case lvarusebredef:
  case loopcvmix:
  case loopconsassn:
  case redefimlvar:
  case lvarneverredef:
  case deflistnotempty:
  case typedoubdef:
  case innerfndoubdef:
    mymemcpy(newe->UU.errorstr.str, blankstring, sizeof(stryngar));
    newe->UU.errorstr.len = 0;
    break;

  case binaryoparity:
  case unaryoparity:
  case binarytypesnoteq:
    mymemcpy(newe->UU.errorstr.str, blankstring, sizeof(stryngar));
    newe->UU.errorstr.len = 0;
    newe->UU.op = noop;
    break;

  case unarytypeerr:
    newe->UU.typeptr = NULL;
    newe->UU.U31.uop = noop;
    break;

  case notypeabuild:
  case notarraytype:
  case arraytypeclash:
  case lbarity:
  case lbtype:
  case arityonerror:
  case chararity:
  case doublearity:
  case intarity:
  case arityonisu:
  case arityonrbuild:
  case typeonrbuild:
  case arityonubuild:
  case typeclashub:
  case arityonrselect:
  case etypeonrs:
  case arityonrexp:
  case arityonoexp:
  case ntyperr:
  case prednotbool:
  case predarity:
  case nothingsc:
  case typenssc:
  case typeclashsc:
  case arityontag:
  case typenutc:
  case typesnsametc:
  case othernone:
  case indexexperr:
  case arrayarity:
  case needarraytype:
  case arityareplace:
  case typeareplace:
  case replaceoverdim:
  case replaceelemtype:
  case arrayofarity:
  case valueofarity:
  case streamofarity:
  case maskingarity:
  case masknotbool:
  case longexplist:
  case longnamelist:
  case cdmix:
  case testarity:
  case testnotbool:
  case realarity:
    /* blank case */
    break;

  case charitype:
  case doubleitype:
  case intitype:
  case isutype:
  case typeonfa:
  case icnotas:
  case realitype:
    newe->UU.typeptr = NULL;
    break;

  case binarytypeerr:
    newe->UU.U18.btypeptr = NULL;
    newe->UU.U18.bop = noop;
    break;

  case typefunctarg:
  case numfunctarg:
    mymemcpy(newe->UU.U29.functionname.str, blankstring, sizeof(stryngar));
    newe->UU.U29.functionname.len = 0;
    newe->UU.U29.argnum = -1;
    break;

  case typefunctres:
  case numfunctres:
    mymemcpy(newe->UU.U19.functname.str, blankstring, sizeof(stryngar));
    newe->UU.U19.functname.len = 0;
    newe->UU.U19.resnum = -1;
    break;

  case asstypeclash:
    newe->UU.U21.exptype = NULL;
    newe->UU.U21.nametype = NULL;
    break;

  case vardoubledecl:
    mymemcpy(newe->UU.U25.varname.str, blankstring, sizeof(stryngar));
    newe->UU.U25.varname.len = 0;
    newe->UU.U25.exp1type = NULL;
    newe->UU.U25.exp2type = NULL;
    break;

  case abuildelemtype:
    newe->UU.elemnum = -1;
    break;

  case ifreturnstypes:
  case tagreturnstypes:
  case numifreturns:
  case expsnstypesc:
  case numtagreturns:
    newe->UU.expnum = 0;
    break;

  case reducetype:
    newe->UU.U91.redflag = rfnone;
    newe->UU.U91.redtype = NULL;
    break;
  }/* case */
  return newe;
}  /* NewErrorPtr */


Static Void freeerrorptr(errorrec)
errorrecord **errorrec;
{
  Free(*errorrec);
}


Static symlistnode *newsymlistptr()
{
  symlistnode *slptr;
  PSBLOCK b;

  /* slptr = (symlistnode *)Malloc(sizeof(symlistnode)); */
  b = MySBlockAlloc(); /* CANN */
  slptr = &(b->sln); /* CANN */

  slptr->sym = NULL;
  slptr->next = NULL;
  return slptr;
}  /* NewSymListPtr */


Static Void freesymlistptr(slptr)
symlistnode **slptr;
{
  Free(*slptr);
}  /* FreeSymListPtr */


/* ******************************************************************* */
/*  This cleans the TYPE table after a smash that was not supposed to occur */
Static Void cleantable(oldtthwm)
int oldtthwm;
{
  int i, FORLIM;

  FORLIM = tthwm;
  for (i = oldtthwm; i < FORLIM; i++)
    freettptr(&typetable[i]);
  tthwm = oldtthwm;
}  /* Cleantable */


Static Char *errortypes_NAMES[] = {
  "fndoubdef", "tnameundef", "fieldnameddef", "tagnameddef", "defineerr",
  "doubledefarg", "forwardtype", "innerfndoubdef", "binaryoparity",
  "chararity", "doublearity", "intarity", "realarity", "binarytypesnoteq",
  "charitype", "doubleitype", "intitype", "realitype", "binarytypeerr",
  "typefunctres", "numfunctres", "asstypeclash", "vardoubledef",
  "longexplist", "longnamelist", "vardoubledecl", "vnameundef", "undeffunct",
  "typefunctarg", "numfunctarg", "unaryoparity", "unarytypeerr",
  "vnamenotloopvar", "arityonerror", "arityonisu", "isutype", "expimporterr",
  "arityonrbuild", "typeonrbuild", "forwardnotresolved", "arityonubuild",
  "tnonubuild", "undeftagub", "typeclashub", "lbarity", "lbtype",
  "notypeabuild", "atypenameerr", "notarraytype", "arraytypeclash",
  "abuildelemtype", "arityonrselect", "etypeonrs", "nameundrs", "ntypeonrs",
  "arityonrexp", "arityonoexp", "ntyperr", "nameundrr", "typeclashrr",
  "ifreturnstypes", "tagreturnstypes", "numifreturns", "numtagreturns",
  "prednotbool", "predarity", "nothingsc", "typenssc", "expsnstypesc",
  "typeclashsc", "undefntag", "arityontag", "typenutc", "nameundeftc",
  "ddtname", "typesnsametc", "othernone", "notalltc", "arrayarity",
  "needarraytype", "indexexperr", "arityareplace", "typeareplace",
  "replaceoverdim", "replaceelemtype", "valueofarity", "maskingarity",
  "masknotbool", "arityonfa", "typeonfa", "atinrg", "reducetype",
  "arrayofarity", "streamofarity", "icnotas", "cdmix", "decbutnotdef",
  "lvarnotdef", "testarity", "testnotbool", "lvarusebredef", "loopcvmix",
  "loopconsassn", "redefimlvar", "lvarneverredef", "deflistnotempty",
  "typedoubdef"
} ;


Local Void printtypesort(filename, typeptr)
FILE *filename;
stentry *typeptr;
{
  switch (typeptr->stsort) {

  case iftarray:
    fprintf(filename, "array ");
    break;

  case iftstream:
    fprintf(filename, "stream ");
    break;

  case iftrecord:
    fprintf(filename, "record ");
    break;

  case iftunion:
    fprintf(filename, "union ");
    break;

  case iftfield:
    fprintf(filename, "field ");
    break;

  case iftfunctiontype:
    fprintf(filename, "function type ");
    break;

  case iftmultiple:
    fprintf(filename, "multiple ");
    break;

  case ifttag:
    fprintf(filename, "tag ");
    break;

  case ifttuple:
    fprintf(filename, "tuple ");
    break;

  case iftwild:
    fprintf(filename, "wild ");
    break;

  case iftbasic:
    switch (typeptr->UU.stbasic) {

    case ifbboolean:
      fprintf(filename, "boolean ");
      break;

    case ifbcharacter:
      fprintf(filename, "character ");
      break;

    case ifbdouble:
      fprintf(filename, "double ");
      break;

    case ifbinteger:
      fprintf(filename, "integer ");
      break;

    case ifbnull:
      fprintf(filename, "null ");
      break;

    case ifbreal:
      fprintf(filename, "real ");
      break;

    case ifbwild:
      fprintf(filename, "wild ");
      break;
    }/* case */
    break;
  }/*case*/
}  /*PrintTypeSort*/

Local Void printoperation(filename, op)
FILE *filename;
opertype op;
{
  switch (op) {

  case orop:
    fprintf(filename, "| ");
    break;

  case andop:
    fprintf(filename, "& ");
    break;

  case mult:
    fprintf(filename, "* ");
    break;

  case divd:
    fprintf(filename, "/ ");
    break;

  case plus:
  case uplus:
    fprintf(filename, "+ ");
    break;

  case minus:
  case uminus:
    fprintf(filename, "- ");
    break;

  case concat:
    fprintf(filename, "|| ");
    break;

  case lt:
    fprintf(filename, "< ");
    break;

  case le:
    fprintf(filename, "<= ");
    break;

  case gt:
    fprintf(filename, "> ");
    break;

  case ge:
    fprintf(filename, ">= ");
    break;

  case equal:
    fprintf(filename, "= ");
    break;

  case notequal:
    fprintf(filename, "~= ");
    break;

  case unot:
    fprintf(filename, "~ ");
    break;
  }/*case*/
}  /*PrintOperation*/

Local Void printerror(filename, errorrec)
FILE *filename;
errorrecord *errorrec;
{
  fprintf(filename, "****ERROR ");
  switch (errorrec->etype) {

  case fndoubdef:
    fprintf(filename, "Function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined in this scope.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case tnameundef:
    fprintf(filename, "type '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' used but never defined. Line %4d\n",
	    errorrec->linenumber);
    break;

  case vnameundef:
    fprintf(filename, "Value name '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' undefined. Line %4d\n", errorrec->linenumber);
    break;

  case fieldnameddef:
    fprintf(filename, "Field '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined in this scope. Line %4d\n",
	    errorrec->linenumber);
    break;

  case tagnameddef:
    fprintf(filename, "Tag '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined in this scope. Line %4d\n",
	    errorrec->linenumber);
    break;

  case defineerr:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, " already listed in DEFINE statement. Line %4d\n",
	    errorrec->linenumber);
    break;

  case doubledefarg:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined as an argument. Line %4d\n",
	    errorrec->linenumber);
    break;

  case forwardtype:
    fprintf(filename, " Forward header not the same as function ");
    fprintf(filename, "header for function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case innerfndoubdef:
    fprintf(filename, "Function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined in an outer scope. Line %4d\n",
	    errorrec->linenumber);
    break;

  case expimporterr:
    fprintf(filename, "Function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' can not be both imported and exported. Line %4d\n",
	    errorrec->linenumber);
    break;

  case forwardnotresolved:
    fprintf(filename, "Forward function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename,
	    "' never had a local function defined for it. Line %4d\n",
	    errorrec->linenumber);
    break;

  case lbarity:
    fprintf(filename,
	    "Lower Bound expression has arity greater that one. Line %4d\n",
	    errorrec->linenumber);
    break;

  case lbtype:
    fprintf(filename,
	    "Type of lower bound expression must be integer. Line %4d\n",
	    errorrec->linenumber);
    break;

  case notypeabuild:
    fprintf(filename,
      "A type name must be given when creating an empty array. Line%12d\n",
      errorrec->linenumber);
    break;

  case atypenameerr:
    fprintf(filename, "Type '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename,
      "' does not match the type of the array element given. Line %12d\n",
      errorrec->linenumber);
    break;

  case notarraytype:
    fprintf(filename,
	    "Type name it not an array type in Array Build.Line %4d\n",
	    errorrec->linenumber);
    break;

  case arraytypeclash:
    fprintf(filename,
      "Base type of type name and type of element in  Abuild must be the same.  Line %4d\n",
      errorrec->linenumber);
    break;

  case abuildelemtype:
    fprintf(filename,
      "Type of the array element number %3din array build is incorrect.  Line %4d\n",
      errorrec->UU.elemnum, errorrec->linenumber);
    break;

  case unarytypeerr:
    fprintf(filename, "Operand of type ");
    printtypesort(filename, errorrec->UU.U31.utypeptr);
    fprintf(filename, "incorrect for operation ");
    printoperation(filename, errorrec->UU.U31.uop);
    fprintf(filename, "Line %4d\n", errorrec->linenumber);
    break;

  case unaryoparity:
    fprintf(filename, "Arity of operand for unary operation ");
    printoperation(filename, errorrec->UU.op);
    fprintf(filename, " is greater than 1. Line %4d\n", errorrec->linenumber);
    break;

  case binaryoparity:
    fprintf(filename, "Arity of operand for binary operation ");
    printoperation(filename, errorrec->UU.op);
    fprintf(filename, " is greater than 1. Line %4d\n", errorrec->linenumber);
    break;

  case binarytypesnoteq:
    fprintf(filename, "Operand types not equal for binary operation ");
    printoperation(filename, errorrec->UU.op);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case binarytypeerr:
    fprintf(filename, "Operand of type ");
    printtypesort(filename, errorrec->UU.U18.btypeptr);
    fprintf(filename, "incorrect for operation ");
    printoperation(filename, errorrec->UU.U18.bop);
    fprintf(filename, "Line %4d\n", errorrec->linenumber);
    break;

  case chararity:
  case doublearity:
  case intarity:
  case realarity:
    fprintf(filename, " Expression argument to prefix operator ");
/* p2c: mono.bin.noblank.p, line 10411: Note:
 * Line breaker spent 0.0+4.30 seconds, 5000 tries on line 12255 [251] */
    fprintf(filename, "%s must be of arity one. Line %4d\n",
	    errortypes_NAMES[(long)errorrec->etype], errorrec->linenumber);
    break;

  case charitype:
    fprintf(filename, "Argument type to prefix operator Character");
    fprintf(filename, " must be integer, not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, ". Line %4d\n", errorrec->linenumber);
    break;

  case doubleitype:
    fprintf(filename, "Argument type to prefix operator Double_Real");
    fprintf(filename, " must be integer or real, not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, ". Line %4d\n", errorrec->linenumber);
    break;

  case intitype:
    fprintf(filename, "Argument type to prefix operator Integer ");
    fprintf(filename, " must be real, double, or character, not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, ". Line %4d\n", errorrec->linenumber);
    break;

  case realitype:
    fprintf(filename, "Argument type to prefix operator Real");
    fprintf(filename, " must be integer, or double, not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, ". Line %4d\n", errorrec->linenumber);
    break;

  case typefunctarg:
    fprintf(filename, "Type of argument number %3d incorrect in function ",
	    errorrec->UU.U29.argnum);
    writestring(filename, &errorrec->UU.U29.functionname);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case numfunctarg:
    if (errorrec->UU.U29.argnum < 0)   /*Not enough arguments*/
      fprintf(filename, "Not enough argument for function ");
    else
      fprintf(filename, "Too many arguments for function ");
    writestring(filename, &errorrec->UU.U29.functionname);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case undeffunct:
    fprintf(filename, "Function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' is undefined.  Line %4d\n", errorrec->linenumber);
    break;

  case typefunctres:
    fprintf(filename, "Type of result number %3d incorrect in function ",
	    errorrec->UU.U19.resnum);
    writestring(filename, &errorrec->UU.U19.functname);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case numfunctres:
    if (errorrec->UU.U19.resnum < 0)   /*Not enough results*/
      fprintf(filename, "Not enough result expressions for function ");
    else
      fprintf(filename, "Too many result expressions for function ");
    writestring(filename, &errorrec->UU.U19.functname);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case asstypeclash:
    fprintf(filename, "Type clash on assignment. expression type ");
    printtypesort(filename, errorrec->UU.U21.exptype);
    fprintf(filename, " name type ");
    printtypesort(filename, errorrec->UU.U21.nametype);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case vardoubledef:
    fprintf(filename, "Value name already defined in this block, '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case longexplist:
    fprintf(filename, " Expression list longer than name list ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case longnamelist:
    fprintf(filename, " Name list longer than expression list");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case vardoubledecl:
    writestring(filename, &errorrec->UU.U25.varname);
    fprintf(filename, " declared twice,  type ");
    printtypesort(filename, errorrec->UU.U25.exp1type);
    fprintf(filename, " and type ");
    printtypesort(filename, errorrec->UU.U25.exp2type);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case vnamenotloopvar:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not a loop name, can't be used with old operator.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonerror:
    fprintf(filename, "input to IS ERROR must be of arity one ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonisu:
    fprintf(filename, "Arity on Is Union must be one. ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case isutype:
    fprintf(filename, "Argument to Is Union must be of type union not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonrbuild:
    fprintf(filename, "Expression in field of record build must be");
    fprintf(filename, " of arity one.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typeonrbuild:
    fprintf(filename, "Type name must match record being built");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonubuild:
    fprintf(filename, "Expression must be of arity one on union create");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case tnonubuild:
    fprintf(filename, "Type name on union create must be a union type");
    fprintf(filename, ". Name was '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case undeftagub:
    fprintf(filename, "Tag '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not defined in this union");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typeclashub:
    fprintf(filename, "Type of expression does not match tag type");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonrselect:
    fprintf(filename, "Expression on record select must be arity one.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case etypeonrs:
    fprintf(filename, "Expression type on record select must be record");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case nameundrs:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not a field in this record. ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case ntypeonrs:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not of type record. ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonrexp:
    fprintf(filename, "Replacement expression must be of arity one in ");
    fprintf(filename, "record replace");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityonoexp:
    fprintf(filename, "Expression preceeding record replace must");
    fprintf(filename, "of arity one ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case ntyperr:
    fprintf(filename, "Expression and fields must be of type record");
    fprintf(filename, " in a record replace.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case nameundrr:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not a field in this record.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typeclashrr:
    fprintf(filename, "Replacement expression not the same type as ");
    fprintf(filename, "field '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case ifreturnstypes:
    fprintf(filename,
      "Type of expression number %4d incorrect in arm of IF. Line %4d\n",
      errorrec->UU.expnum, errorrec->linenumber);
    break;

  case tagreturnstypes:
    fprintf(filename,
      "Type of expression number %4d incorrect in arm of TagCase.  Line %4d\n",
      errorrec->UU.expnum, errorrec->linenumber);
    break;

  case numifreturns:
    if (errorrec->UU.expnum < 0)
      fprintf(filename,
	      "Not enough expressions returned by arm  of IF.  Line %4d\n",
	      errorrec->linenumber);
    else
      fprintf(filename,
	      "Too many expressions returned by arm  of IF.  Line %4d\n",
	      errorrec->linenumber);
    break;

  case numtagreturns:
    if (errorrec->UU.expnum < 0)
      fprintf(filename,
	"Not enough expressions returned by arm  of TagCase.  Line %4d\n",
	errorrec->linenumber);
    else
      fprintf(filename,
	      "Too many expressions returned by arm  of TagCase.  Line %4d\n",
	      errorrec->linenumber);
    break;

  case prednotbool:
    fprintf(filename,
      "The predicate expression of an IF statement  must be of type boolean.  Line %4d\n",
      errorrec->linenumber);
    break;

  case predarity:
    fprintf(filename,
      "The predicate expression of an IF statement  must have arity 1. Line %4d\n",
      errorrec->linenumber);
    break;

  case nothingsc:
    fprintf(filename, "A stream create must have a typename or an ");
    fprintf(filename, "expression");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typenssc:
    fprintf(filename, "The typename in a stream build must be of ");
    fprintf(filename, "type stream");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typeclashsc:
    fprintf(filename, "Expressions in stream build must be the same");
    fprintf(filename, " type as the type name's elements");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case expsnstypesc:
    fprintf(filename, "All expressions must be the same type, error");
    fprintf(filename, " on exp %6d", errorrec->UU.expnum);
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case undefntag:
    fprintf(filename, " Name '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "'undefined.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arityontag:
    fprintf(filename, "Expression in Tagcase head must be of arity one.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typenutc:
    fprintf(filename, "Expression in Tagcase header must ");
    fprintf(filename, "be of type union.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case nameundeftc:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not a tag of this union.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case ddtname:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already associated with an arm in this tagcase");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typesnsametc:
    fprintf(filename, "Types of fields in tag list must be of the same");
    fprintf(filename, " type. ");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case othernone:
    fprintf(filename, "All tags have been specified, otherwise");
    fprintf(filename, " clause not allowed.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case notalltc:
    fprintf(filename, "All tags must be specified in a tagcase,'");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' not specified.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case arrayarity:
    fprintf(filename,
	    "Arity of expression for array indexing must be 1.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case needarraytype:
    fprintf(filename,
      "Type of expression for array indexing must be an array.  Line %4d\n",
      errorrec->linenumber);
    break;

  case indexexperr:
    fprintf(filename,
      "Type of expression for array indexing must be integer. Line%4d\n",
      errorrec->linenumber);
    break;

  case arityareplace:
    fprintf(filename,
      "Array Expression for an AReplace must have an arity of 1. Line %4d\n",
      errorrec->linenumber);
    break;

  case typeareplace:
    fprintf(filename,
	    "Expression for an AREplace must be of type array.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case replaceoverdim:
    fprintf(filename,
      "Too many dimensions specified in AReplace for the size of the input array. Line %4d\n",
      errorrec->linenumber);
    break;

  case replaceelemtype:
    fprintf(filename, "Replace element not the correct type. Line %4d\n",
	    errorrec->linenumber);
    break;

  case valueofarity:
    fprintf(filename,
	    "Arity of expression in Value Of must be 1.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case streamofarity:
    fprintf(filename,
	    "Arity of expression in Stream Of must be 1.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case arrayofarity:
    fprintf(filename,
	    "Arity of expression in Array Of must be 1.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case maskingarity:
    fprintf(filename, "Arity of masking expression must be 1.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case masknotbool:
    fprintf(filename,
	    "Type of masking expression must be boolean.  Line %4d\n",
	    errorrec->linenumber);
    break;

  case arityonfa:
    fprintf(filename, "Forall header must be one array/stream expression");
    fprintf(filename, " or two integer expressions.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typeonfa:
    fprintf(filename, "Type of expression in forall header must be");
    fprintf(filename, " integer, array, or stream");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case atinrg:
    fprintf(filename, "At index list only allowed with array/stream");
    fprintf(filename, " forall expressions.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case reducetype:
    fprintf(filename, "Type ");
    printtypesort(filename, errorrec->UU.U91.redtype);
    fprintf(filename, " not legal input type to ");
    switch (errorrec->UU.U91.redflag) {

    case rfsum:
      fprintf(filename, "Sum ");
      break;

    case rfproduct:
      fprintf(filename, "Product ");
      break;

    case rfleast:
      fprintf(filename, "Least ");
      break;

    case rfgreatest:
      fprintf(filename, "Greatest ");
      break;

    case rfcatenate:
      fprintf(filename, "Catenate ");
      break;
    }/*case*/
    fprintf(filename, " reduction. Line %4d\n", errorrec->linenumber);
    break;

  case icnotas:
    fprintf(filename, "Index names may only be used on stream or array");
    fprintf(filename, " types not ");
    printtypesort(filename, errorrec->UU.typeptr);
    fprintf(filename, " in a for.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case cdmix:
    fprintf(filename, "Cross and Dot may not be mixed in a for loop.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case decbutnotdef:
    fprintf(filename, "Definition must precede use of value name '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case lvarnotdef:
    fprintf(filename, "Definition must precede use of loop name :'");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case testarity:
    fprintf(filename, "Arity of test expression must be 1. Line %4d\n",
	    errorrec->linenumber);
    break;

  case testnotbool:
    fprintf(filename, "Type of test expression must be boolean. Line %4d\n",
	    errorrec->linenumber);
    break;

  case lvarusebredef:
    fprintf(filename, "Loop variables must be redefined before use: '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' Line %4d\n", errorrec->linenumber);
    break;

  case loopcvmix:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' is a loop constant, ");
    fprintf(filename, "can not be used with old.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case loopconsassn:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "'already used as a loop constant, can not be");
    fprintf(filename, " used on the left hand side on assignment.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case redefimlvar:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' is an imported loop name and can not");
    fprintf(filename, " be assigned to in this loop.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case lvarneverredef:
    putc('\'', filename);
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' used as a loop name but never redefined");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case deflistnotempty:
    fprintf(filename, "Function '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' is listed in Define Stmt, but never defined.");
    fprintf(filename, " Line %4d\n", errorrec->linenumber);
    break;

  case typedoubdef:
    fprintf(filename, "Type name '");
    writestring(filename, &errorrec->UU.errorstr);
    fprintf(filename, "' already defined.  Line %4d\n", errorrec->linenumber);
    break;
  }/* case */
}  /* semerror */


/*------------------- ERROR ROUTINES -------------------------*/
Static Void semerror(errorrec)
errorrecord *errorrec;
{  /*semerr*/
  semerrcnt++;
  printerror(stdout, errorrec);
  if (listing)
    printerror(listfile, errorrec);
  freeerrorptr(&errorrec);
}


/*------------------- GRAPH ROUTINES -----------------------*/
Static node *findparent(level)
treestackobj *level;
{
  node *Result;

  switch (level->lleveltag) {

  case forloop:
  case specfa:
  case initloop:
  case tagcase:
  case nestedifcomp:
  case ifcomp:
    Result = level->currentsubn0;
    break;

  case funct:
    Result = level->UU.lfunctname->graphinfo->node_;
    break;
  }/*case*/
  return Result;
}


Static Void linkparent(nd, level)
node *nd;
treestackobj *level;
{
  while (level->lleveltag == let)
    level = level->lastlevel;
  level->prevnode->ndnext = nd;
  level->prevnode = nd;
  nd->ndparent = findparent(level);
}  /*LinkParent*/


/*------------------- SEMANTIC STACK -------------------*/
Static Void pushsemantic(stackitem)
semanticrec *stackitem;
{
  stackitem->next = topsemstk;
  topsemstk = stackitem;
}


Static semanticrec *popsemantic()
{
  semanticrec *Result;

  Result = topsemstk;
  if (topsemstk != NULL)
    topsemstk = topsemstk->next;
  else
    printf("INTERNAL ERROR -- semantic stack underflow\n");
  return Result;
}


/*--------------- LIST ROUTINES ---------------------------*/
Static Void linkname(name, line, col, list)
stryng name;
int line, col;
namelistrec **list;
{
  namelistrec *listptr;

  if (*list == NULL) {
    newnamelist(list);
    (*list)->name = name;
    (*list)->linenum = linenumber;
    return;
  }
  listptr = *list;
  while (listptr->next != NULL)
    listptr = listptr->next;
  newnamelist(&listptr->next);
  listptr->next->name = name;
  listptr->next->column = col;
  listptr->next->linenum = line;
  listptr->next->prev = listptr;
}  /* linkname */


Static tornrec *linktorntype(elem, list)
tornrec *elem, *list;
{
  if (list != NULL)
    list->prev = elem->next;
  elem->next = list;
  elem->prev = NULL;
  return elem;
}


Static Void linktype(typeptr, list)
stentry *typeptr;
typelistrec **list;
{
  typelistrec *listptr;

  if (*list == NULL) {
    newtypelist(list);
    (*list)->typeptr = typeptr;
    return;
  }
  listptr = *list;
  while (listptr->next != NULL)
    listptr = listptr->next;
  newtypelist(&listptr->next);
  listptr->next->typeptr = typeptr;
  listptr->next->prev = listptr;
}  /* linktype */


Static Void addtonamelist(inputtoken, line, col)
stryng inputtoken;
int line, col;
{
  /*action 3*/
  /*stackpops - tgnamelist
                  pushes - tgnamelist*/
  /*description
          Pop a name list from the semstack and add the inputtoken stryng
          to the list and push it again*/
  semanticrec *semrec;

  if (semtrace) {
    printf("begin AddToNameList, name is ");
    writestring(stdout, &inputtoken);
    putchar('\n');
  }
  semrec = popsemantic();
  stringlowercase(&inputtoken);
  linkname(inputtoken, line, col, &semrec->UU.namelist);
  pushsemantic(semrec);
}  /* addtonamelist */


Static Void startnamelist()
{
  /*action 2*/
  /*stackpops - nothing
                  pushes - tgnamelist (empty)*/
  /*description
          Obtain a namelist record and initialize it to empty.  Push it
          on the semantic stack.*/
  semanticrec *semrec;

  if (semtrace)
    printf("begin StartNameList \n");
  newsemantic(&semrec, tgnamelist);
  pushsemantic(semrec);
}  /* startnamelist */


Static Void addtoexplist(list, exp)
explistnode **list, *exp;
{
  /* Add the graph info pointed to by exp to the end of the list pointed
     to by list.*/
  explistnode *listptr;

  listptr = *list;
  if (listptr == NULL) {
    *list = exp;
    return;
  }
  while (listptr->next != NULL)
    listptr = listptr->next;
  listptr->next = exp;

  /*else*/
}  /*AddToExpList*/


Static Void pushemptyexplist()
{
  /*action 107 PushEmptyExpList*/
  /*stackpops - nothing
                  pushes - tgexplist*/
  /*description
          Save an empty explist on the stack*/
  semanticrec *semexplist;

  if (semtrace)
    printf("begin PushEmptyExpList \n");
  newsemantic(&semexplist, tgexplist);
  pushsemantic(semexplist);
}


Static inforecord *getalit1()
{
  inforecord *info;

  info = newinfoptr();
  info->typeptr = getbasictype(ifbinteger);
  string10(&info->litvalue, "1         ");
  return info;
}


Static int assignports(list, portnum)
edgelisthead *list;
int portnum;
{
  edgelist *edgelist_, *tempedge;
  edgelisthead *headlist, *temphead;
  int port_;

  headlist = list;
  port_ = portnum;
  while (headlist != NULL) {
    edgelist_ = headlist->edges;
    while (edgelist_ != NULL) {
      if (edgelist_->toorfrom == toport)
	edgelist_->edge->pttoport = port_;
      else if (edgelist_->toorfrom == fromport)
	edgelist_->edge->UU.U1.ptfrport = port_;
      else {
	printf("COMPILER ERROR PORT FLAG = NoPort, name = ");
	writestring(stdout, &headlist->edgename);
	putchar('\n');
      }
      tempedge = edgelist_;
      edgelist_ = edgelist_->next;
      freeedgelistptr(&tempedge);
    }  /*while*/
    port_++;
    temphead = headlist;
    headlist = headlist->next;
    freeheadptr(&temphead);
  }  /*while*/
  return port_;
}
