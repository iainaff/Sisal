#include "frontend.h"

extern Void printsym PP((int sym));

extern Void scan PP((short *token, int *tokenlineno, inputbuffer **buffer));

extern Void myabort PV();

extern Void getline PP((inputbuffer **buffer));

extern Void peek PP((short *sym));

extern Void deletetokens PP((int pighowmany));

extern Void inserttokens PP((stagerec fix));

extern Void printline PP((inputbuffer *buffer));

extern Void flushlines PV();

FILE *CANN_source = NULL;
char  CANN_source_file[1024];
char *CANN_file_name = NULL;

/*======================================================
     do the FMQ correction
=======================================================*/
Static Void storehisto(deltried, deldone, inserts)
int deltried, deldone, inserts;
{
  /*-----------------------------------------------------------------
    store info on distibution of deletions considered and
    insertions made
  ------------------------------------------------------------------*/
  deltriedhisto[deltried]++;
  delmadehisto[deldone]++;
  inshisto[inserts]++;
}  /* storehisto */


Static Void readcosts()
{
  /*-----------------------------------------------------------------
    read the correction costs for the terminals
   ------------------------------------------------------------------*/
  char term;
  int c, d;
  char FORLIM;
  insertstringrec *WITH;

  FORLIM = numterms;
  for (term = 1; term <= FORLIM; term++) {
    WITH = &costtable[term + 2];
    fread(&c, sizeof(int), 1, etableout);
    fread(&d, sizeof(int), 1, etableout);
    deletecosts[term] = d;
    WITH->cost = c;
    insertptr++;
    WITH->first = insertptr;
    WITH->last = insertptr;
    insertspace[insertptr] = term;
  }
}  /* readcosts */


Static Void reads()
{
  /*-----------------------------------------------------------------
    read in the S table
  ------------------------------------------------------------------*/
  short sym;
  int length, i, c;
  short FORLIM;
  insertstringrec *WITH;

  FORLIM = numsymbols + 2;
  for (sym = numterms + 3; sym <= FORLIM; sym++) {   /* for */
    WITH = &costtable[sym];
    fread(&c, sizeof(int), 1, etableout);
    fread(&length, sizeof(int), 1, etableout);
    WITH->cost = c;
    if (insertptr + length > maxinsertspace) {
      printf(" increase size of insertspace\n");
      myabort();
    }
    WITH->first = insertptr + 1;
    for (i = 1; i <= length; i++) {
      insertptr++;
      fread(&c, sizeof(int), 1, etableout);
      insertspace[insertptr] = c;
    }
    WITH->last = insertptr;
  }
}  /* readS */


Static Void checkinsertsize(l)
int l;
{
  /*-------------------------------------------------------------------
    check to see if a string of length l will fit into insertspace
  --------------------------------------------------------------------*/
  if (insertptr + l > maxinsertspace) {
    printf(" constant exceeded, increase MaxInsertSpace\n");
    myabort();
  }
}  /* checkinsertsize */


Static Void get1prefix()
{
  /*-------------------------------------------------------------------
      read in all the prefix entries for this terminal symbol
       this will have to be changed slightly for direct access,
        put into prefixtable instead of full prefixtable
  --------------------------------------------------------------------*/
  int sym, insym, inscost, length, i;
  insertstringrec *WITH;

  fread(&sym, sizeof(int), 1, etableout);
  fread(&inscost, sizeof(int), 1, etableout);
  fread(&length, sizeof(int), 1, etableout);
  while (sym != 0) {
    checkinsertsize(length);
    WITH = &prefixtable[sym + 2];   /* with */
    WITH->first = insertptr + 1;
    WITH->cost = inscost;
    for (i = 1; i <= length; i++) {
      fread(&insym, sizeof(int), 1, etableout);
      if ((unsigned)insym > numsymbols)
	printf(" !! insym ?? %12d\n", insym);
      insertptr++;
      insertspace[insertptr] = insym;
    }  /* for */
    WITH->last = insertptr;
    fread(&sym, sizeof(int), 1, etableout);
    fread(&inscost, sizeof(int), 1, etableout);
    fread(&length, sizeof(int), 1, etableout);
  }  /* while */
}  /* get1prefix */


Static Void skipcosts()
{
  /*-----------------------------------------------------------------
    same as readcosts, but ignore the info
  ------------------------------------------------------------------*/
  char term;
  int c, deletecost;
  char FORLIM;

  FORLIM = numterms;
  for (term = 1; term <= FORLIM; term++) {
    fread(&c, sizeof(int), 1, etableout);
    fread(&deletecost, sizeof(int), 1, etableout);
  }
}  /* skipcosts */


Static Void skips()
{
  /*-----------------------------------------------------------------
     same as readS, but ignore the info
  ------------------------------------------------------------------*/
  short sym;
  int i, c, length;
  short FORLIM;

  FORLIM = numsymbols;
  for (sym = numterms + 1; sym <= FORLIM; sym++) {
    fread(&c, sizeof(int), 1, etableout);
    fread(&length, sizeof(int), 1, etableout);
    for (i = 1; i <= length; i++)
      fread(&c, sizeof(int), 1, etableout);
  }  /* for */
}  /* skipS */


Static Void skip1prefix()
{
  /*------------------------------------------------------------------
         same as get1prefix, but ignore the info
       this will have to be changed slightly for direct access,
        put into prefixtable instead of full prefixtable
  -------------------------------------------------------------------*/
  int sym, insym, inscost, length, i;

  fread(&sym, sizeof(int), 1, etableout);
  fread(&inscost, sizeof(int), 1, etableout);
  fread(&length, sizeof(int), 1, etableout);
  while (sym != 0) {
    for (i = 1; i <= length; i++) {
      fread(&insym, sizeof(int), 1, etableout);
      if ((unsigned)insym > numsymbols)
	printf(" !! insym ?? %12d\n", insym);
    }  /* for */
    fread(&sym, sizeof(int), 1, etableout);
    fread(&inscost, sizeof(int), 1, etableout);
    fread(&length, sizeof(int), 1, etableout);
  }  /* while */
}  /* skip1prefix */


Static Void seeke(term, directoryname)
char term;
stryng directoryname;
{
  /*------------------------------------------------------------------
       position the file pointer to the proper place to
       read the prefixes for term
  --------------------------------------------------------------------*/
  int sym, t, cost, length;
  boolean successfulopen;
  stryng etablename, etablesuffix;

  string10(&etablesuffix, "/etabl.bin");
  stripspaces(&etablesuffix);
  etablename = directoryname;
  if (etablename.len < maxstringchars)
    insertstring(&etablename, &etablesuffix, etablename.len + 1);
  successfulopen = openintread(&etableout, &etablename);
  /*    reset(etableout);  sks*/
  if (!successfulopen) {
    printf("Cannot find error correction tables in ");
    writestring(stdout, &etablename);
    printf(".\n");
    myabort();
  }
  fread(&t, sizeof(int), 1, etableout);
  fread(&sym, sizeof(int), 1, etableout);
  fread(&myinfinity, sizeof(int), 1, etableout);
  if (sym != numsymbols)
    printf(" symbols don't match parse tables %12d%12d\n", sym, numsymbols);
  if (t != numterms)
    printf(" terms don't match parse tables %12d%12d\n", t, numterms);
  skipcosts();
  skips();
  fread(&sym, sizeof(int), 1, etableout);
  fread(&cost, sizeof(int), 1, etableout);
  fread(&length, sizeof(int), 1, etableout);
  if (sym != 0 || cost != 1 || length != 0) {
    printf(" error in reading prefix tables\n");
    myabort();
  }  /* if */
  for (sym = 1; sym < term; sym++)
    skip1prefix();
}  /* seekE */


Static Void getprefixinfo(term, directoryname)
char term;
stryng directoryname;
{
  /*------------------------------------------------------------------
    get the prefixtable entries for term;
    similar to getstateinfo
  -------------------------------------------------------------------*/
  short sym, FORLIM;
  insertstringrec *WITH;

  FORLIM = numsymbols + 2;
  for (sym = 3; sym <= FORLIM; sym++) {
    WITH = &prefixtable[sym];
    WITH->cost = myinfinity;
    WITH->first = 2;
    WITH->last = 1;
  }
  WITH = &prefixtable[term + 2];
  WITH->cost = 0;
  if (cortrace) {
    printf(" get info for ");
    printsym(term);
    putchar('\n');
  }
  insertptr = savinsertptr;
  /*this code uses random file access*/
  /*this code simulates random file access*/
  seeke(term, directoryname);
  get1prefix();
  curerrsym = term;
  prefixtable[term + 2].cost = 0;
}  /* getprefixinfo */


Static Void readerrtables(directoryname)
stryng *directoryname;
{
  /*----------------------------------------------------------------
    read in all the error table info
  -=---------------------------------------------------------------*/
  int sym, term;
  boolean successfulopen;
  stryng etablename, etablesuffix;

  if (!errortables)
    return;
  string10(&etablesuffix, "/etabl.bin");
  stripspaces(&etablesuffix);
  etablename = *directoryname;
  if (etablename.len < maxstringchars)
    insertstring(&etablename, &etablesuffix, etablename.len + 1);
  successfulopen = openintread(&etableout, &etablename);
  /*     reset(etableout);  sks */
  if (!successfulopen) {
    printf("Cannot find error correction tables in ");
    writestring(stdout, &etablename);
    printf(".\n");
    myabort();
  }
  fread(&term, sizeof(int), 1, etableout);
  fread(&sym, sizeof(int), 1, etableout);
  fread(&myinfinity, sizeof(int), 1, etableout);
  if (sym != numsymbols)
    printf(" symbols don't match parse tables %12d%12d\n", sym, numsymbols);
  if (term != numterms)
    printf(" terms don't match parse tables %12d%12d\n", term, numterms);
  readcosts();
  reads();
  savinsertptr = insertptr;
}  /* readerrtables */


Static Void copye(source, dest)
short source;
stagerec *dest;
{
  /*-------------------------------------------------------------------
     copy source to dest, converting type of string
  --------------------------------------------------------------------*/
  short i;
  insertstringrec *WITH;
  short FORLIM;

  WITH = &prefixtable[source + 2];
  if (dest->length - WITH->first + WITH->last + 1 > maximuminsert) {
    printf(" insertion found is too long\n");
    myabort();
  }
  FORLIM = WITH->last;
  for (i = WITH->first; i <= FORLIM; i++) {
    dest->length++;
    dest->string[dest->length] = insertspace[i];
  }
  dest->cost += WITH->cost;
}  /* copyE */


Static Void copys(source, dest)
short source;
stagerec *dest;
{
  /*-------------------------------------------------------------------
     copy source to dest, converting type of string
  --------------------------------------------------------------------*/
  short i;
  insertstringrec *WITH1;
  short FORLIM;

  WITH1 = &costtable[source + 2];
  if (dest->length - WITH1->first + WITH1->last + 1 > maximuminsert) {
    printf(" insertion found is too long\n");
    myabort();
  }
  FORLIM = WITH1->last;
  /*writeln('copystring ',first:3,' ',last:3,' ',cost:3);*/
  for (i = WITH1->first; i <= FORLIM; i++) {
    dest->length++;
    dest->string[dest->length] = insertspace[i];
  }
  dest->cost += WITH1->cost;
}  /* copyS */


Static Void printinsertion(ins)
stagerec ins;
{
  /*---------------------------------------------------------------
    print out the insertion string ins.
   ----------------------------------------------------------------*/
  char i;

  for (i = 1; i <= ins.length; i++) {
    printsym(ins.string[i]);
    putchar(' ');
  }
  printf(" {%ld}", ins.cost);
}  /*printinsertion*/


Static Void expandinsertion(minpos, insert)
short minpos;
stagerec *insert;
{
  /*----------------------------------------------------------------
    expand out the insertion determined by the position in
    the stack and the error symbol
  -----------------------------------------------------------------*/
  short pos;
  int sym;

  if (debug)
    printf("start expandinsertion\n");
  insert->cost = 0;
  insert->length = 0;
  for (pos = stackptr; pos > minpos; pos--) {
    sym = parsestack[pos];
    if (sym > 0)
      copys(sym, insert);
  }
  sym = parsestack[minpos];
  copye(sym, insert);
  if (debug)
    printf("end expandinsertion\n");
}  /* expandinsertion */


Static Void llinsert(errsym, insertion, directoryname)
char errsym;
stagerec *insertion;
stryng *directoryname;
{
  /*---------------------------------------------------------------
    compute the least-cost insertion to make errsym legal given the
        current parse stack.
   ----------------------------------------------------------------*/
  int stackloc, runningcost, mincost, thiscost;
  short minpos;
  int sym;

  if (debug)
    printf("start LLInsert\n");
  if (curerrsym != errsym)
    getprefixinfo(errsym, *directoryname);
  stackloc = stackptr;
  runningcost = 0;
  mincost = myinfinity;
  do {
    sym = parsestack[stackloc];
    if (sym > 0) {
      thiscost = runningcost + prefixtable[sym + 2].cost;
      if (thiscost < mincost) {
	mincost = thiscost;
	minpos = stackloc;
      }
      runningcost += costtable[sym + 2].cost;
    }
    stackloc--;
  } while (runningcost < mincost && stackloc >= 0);
  if (mincost < myinfinity)
    expandinsertion(minpos, insertion);
  else
    insertion->cost = myinfinity;
  if (debug)
    printf("end LLInsert\n");
}  /* LLInsert */


Static Void inoverflow()
{
  /*---------------------------------------------------------------
    input buffer is full.
    give up
  ----------------------------------------------------------------*/
  if (debug)
    printf("end inoverflow\n");
  flushlines();
  printf(" ***%d*** line too long for correction\n", linenumber);
  myabort();
  if (debug)
    printf("end inoverflow\n");
}  /* inoverflow */


Static Void llcorrector(token, directoryname)
short *token;
stryng *directoryname;
{
  /*-------------------------------------------------------------------
    find the least cost correction to get us out of this error situation
   --------------------------------------------------------------------*/
  stagerec insertion;
  short nextsym;
  int delcost, mincost, delcount, deletepoint;
  stagerec savinsert;

  if (debug)
    printf("start LLcorrector\n");
  errorcount++;
  llinsert(*token, &savinsert, directoryname);
  if (cortrace) {
    printf(" first correction found costs %3ld\n", savinsert.cost);
    printf(" insertion is: ");
    column = 15;
    printinsertion(savinsert);
    putchar('\n');
  }
  delcost = deletecosts[*token];
  mincost = savinsert.cost;
  deletepoint = 0;
  delcount = 0;
  nextsym = *token;

  while (delcost < mincost)
  {  /* consider deletions until accumulated cost is too high */
    numdeltried++;
    if (cortrace) {
      printf(" try deleting ");
      printsym(nextsym);
      putchar('\n');
    }
    peek(&nextsym);

    if (nextsym == commenttoken || nextsym == incltoken) {
      continue;
    }  /*then*/
    delcount++;
    llinsert(nextsym, &insertion, directoryname);
	/*find insert necessary if deletion made*/
    if (delcost + insertion.cost < mincost)
    {  /* a better correction is found remember it */
      deletepoint = delcount;
      savinsert = insertion;
      mincost = delcost + insertion.cost;
      if (cortrace) {
	printf(" better correction found:\n");
	printinsertion(savinsert);
	printf(" total cost is %2d\n\n", mincost);
      }
    }
    delcost += deletecosts[nextsym];
  }
  if (mincost >= myinfinity) {
    printf(" no correction is possible \n");
    myabort();
  }
  if (cortrace) {
    printf(" final correction: delete %d symbols, and insert\n", deletepoint);
    printf("   ");
    printinsertion(savinsert);
    putchar('\n');
  }
  if (deletepoint > 0)
    deletetokens(deletepoint);
  inserttokens(savinsert);
  tokinserted += savinsert.length;
  tokdeleted += deletepoint;
  if (debug)
    printf("end LLcorrector\n");
}  /* LLcorrector */


Static Void myabort()
{
  printf("************************ A B O R T *************************\n\n");

  /* if ( source != NULL ) {
    fclose(source);
    sprintf(dummystring, "rm %s", dummyfilename);
    system(dummystring);
    } */

  _Escape(-1);
}


Static Void printfraction(num, divider)
int num, divider;
{
  /*----------------------------------------------------------------------
    print the fraction num/divider
    (with a leading zero if < 1)
    ----------------------------------------------------------------------*/
  int whole, fraction;
  double realfrac;
  int digit;

  if (debug)
    printf("start printfraction\n");
  whole = num / divider;
  fraction = num % divider;
/* p2c: mono.bin.noblank.p, line 21489:
 * Note: Using % for possibly-negative arguments [317] */
  if (whole > 0)
    printf("%5d.", whole);
  else
    printf("    0.");
  realfrac = (double)fraction / divider;
  do {
    realfrac *= 10;
    digit = (long)realfrac;
    realfrac -= digit;
    divider /= 10;
    printf("%d", digit);
  } while (divider >= 10);
  if (debug)
    printf("end printfraction\n");
}  /* printfraction */


Static Void initptab()
{
  /*------------------------------------------------------------------
    initialize the table to all nils
   -------------------------------------------------------------------*/
  short sym;

  if (debug)
    printf("start initPtab\n");
  for (sym = 3; sym <= maxsym + 2; sym++)
    table[sym] = NULL;
  if (debug)
    printf("end initPtab\n");
}  /* initPtab */


Static short findlltab(sym, term)
short sym;
char term;
{
  /*-----------------------------------------------------------------
      lookup  the parse action for sym, term
  ------------------------------------------------------------------*/
  short Result;
  ptabrec *p;

  if (debug)
    printf("start findLLtab\n");
  p = table[sym + 2];
  Result = 0;
  while (p != NULL) {
    if (SEXT(p->term, 10) != term) {
      p = p->next;
      continue;
    }
    /* code to compat with ntfmq */
    if (p->pact > 2000)
      Result = p->pact - 2000;
    else if (p->pact > 1000)
      Result = p->pact - 1000;
    else
      Result = p->pact;
    p = NULL;
  }
  if (debug)
    printf("end findLLtab\n");
  return Result;
}  /* findLLtab */


/*this procedure is not suited for use with text/binary option
  but is left here just in case you want to put in such a feature */
Static Void printstring(string)
stryng string;
{
  /*----------------------------------------------------------------
    print out a string.
   -----------------------------------------------------------------*/
  short i, j;
  Char STR2[256];
  short FORLIM;

  if (column + string.len > maxlinelength - 3) {
    printf("\n      ");
    column = 5;
  }
  FORLIM = string.len;
  for (i = 1; i <= FORLIM; i++) {
    j = i;
    sprintf(STR2, "%c", stringchar(&string, j));
    if (strcmp(STR2, blank) < 0)
      printf("\\%d", stringchar(&string, j));
    else
      fputc(stringchar(&string, j), stdout);
  }
  column += string.len;
}  /* printstring */


Static Void printsym(sym)
int sym;
{
  /*-----------------------------------------------------------------
    print out symbol number sym
   ------------------------------------------------------------------*/
  if (sym < 0)
    printf("#%d", -sym);
  else
    printstring(p.resword[sym + 2]);
}  /* printsym */


/*======================================================================
    routines to read and print source lines for LLParse
     Source input is read a line at a time into a buffer.
     In order to allow unlimited lookahead in case of an error,
     the buffers are allocated by 'new', and pointers are passed.
     The main entry point in normal use is 'readchar' which returns
     the next character in the buffer, moving to the next line when
     necessary.
     'Unreadchar' is the opposite of readchar, and puts a character back,
     allowing lookahead in the scanner. unreadchar cannot back up
     across lines.
     In normal use, characters are read from the buffer pointed to by
     'linebuf'.  During lookahead for error correction, new buffers are
     allocated.  Linebuf moves ahead with the new buffers, while 'oldline'
     points to the first buffer in the list.
     In order to effect corrections, two procedure are called:
     displaydeletions and DisplayInsertion. Each moves the linebuf
     pointer so that the next character read by 'readchar(linebuf)' will be
     the first character AFTER the changes. (displaydeletions calls 'scan'
     to accomplish this)
======================================================================*/
Static Void printline(buffer)
inputbuffer *buffer;
{
  /* (buffer: InBufPtr);forward */
  /*-------------------------------------------------------------------
       print out source line with corrections
  ------------------------------------------------------------------*/
  unchar i, FORLIM;

  if (buffer->lineno < 0)   /* if */
    return;

  if ( listing )                       /* CANN 2/92 */
    if ( buffer->buf[1] == '%' )       /* CANN 2/92 */
      if ( buffer->buf[2] == '$' ) {   /* CANN 2/92 */
	fprintf( listfile, "\n ------------ [" );
        FORLIM = buffer->inputfile.len;
        for (i = 0; i < FORLIM; i++)
          putc(buffer->inputfile.str[i],listfile);
	fprintf( listfile, "]\n\n" );
	return;                        /* CANN 2/92 */
	}                              /* CANN 2/92 */

  if (buffer->modified) {
    if (echoed) {
      printf("------ corrected to:\n");
      echoed = false;
    }
    printf(" **%3d* ", buffer->lineno);
    if (listing)
      fprintf(listfile, " **%3d* ", buffer->lineno);
  } else if (buffer->lineno > 0 && listing)
    fprintf(listfile, " %5d: ", buffer->lineno);
  else if (listing)
    fprintf(listfile, "        ");
  if (buffer->modified) {
    FORLIM = buffer->length;
    for (i = 1; i <= FORLIM; i++)
      putchar(buffer->buf[i]);
    putchar('\n');
  }
  if (listing) {
    FORLIM = buffer->length;
    for (i = 1; i <= FORLIM; i++)
      putc(buffer->buf[i], listfile);
    putc('\n', listfile);
  }
  if (cortrace) {
    printf("%6c", ' ');
    FORLIM = buffer->pointer;
    for (i = 1; i <= FORLIM; i++)
      putchar('-');
    printf("^\n");
  }
  if (!buffer->modified)
    return;
  printf(" error  ");
  if (listing)
    fprintf(listfile, " error  ");
  if (buffer->underline != NULL) {
    FORLIM = buffer->length;
    for (i = 1; i <= FORLIM; i++)
      putchar(buffer->underline[i]);
    putchar('\n');
    if (listing) {
      FORLIM = buffer->length;
      for (i = 1; i <= FORLIM; i++)
	putc(buffer->underline[i], listfile);
      putc('\n', listfile);
    }
    MyFree(buffer->underline); /* CANN */
    buffer->underline = NULL;
    return;
  }
  if (buffer->modified)
    putchar('\n');
  if (listing)
    putc('\n', listfile);
}  /* printline */


Static Void modify(buffer)
inputbuffer *buffer;
{
  /*-------------------------------------------------------------
    set the modified' flag for the buffer.
     If desired, echo the unchanged line first
   --------------------------------------------------------------*/
  if (debug)
    printf("start modify\n");
  if (!buffer->modified) {
    if (echooldline) {
      if (!echoed)
	printf(" ** syntax error **\n");
      printline(buffer);
      echoed = true;
    }
    buffer->modified = true;
  }
  if (debug)
    printf("end modify\n");
}  /* modify */


Static Void flushlines()
{
  /*-----------------------------------------------------------------
      after the program has been accepted,
       print any corrected lines that remain.
  ------------------------------------------------------------------*/
  if (debug)
    printf("start flushlines\n");
  while (linebuf != NULL) {
    if (linebuf->modified)
      printline(linebuf);
    linebuf = linebuf->next;
  }
  if (debug)
    printf("end flushlines\n");
}  /* flushline */


Static Void readline(buffer)
inputbuffer *buffer;
{
  /*--------------------------------------------------------------------
  actually read the line into buffer
  ---------------------------------------------------------------------*/
  Char ch;
  int i, pointer;
  stryng pragname, uninclstring, linenumstr, txt;

  done = false;
  while (!done) {
    if (P_eof(source)) {
      done = true;
      break;
    }
    if (!P_eoln(source)) {
      done = true;
      break;
    }
    linenumber++;
    if (listing)
      fprintf(listfile, " %5d: \n", linenumber);
    fscanf(source, "%*[^\n]");
    getc(source);
  }  /*while*/
  buffer->length = 1;
  buffer->pointer = 1;
  linenumber++;
  buffer->lineno = linenumber;
  buffer->modified = false;
  buffer->underline = NULL;
  buffer->next = NULL;
  if (P_eof(source)) {   /* with */
    if (debug)
      printf("end of file found\n");
    endfile = true;
    buffer->buf[buffer->pointer] = ' ';
    buffer->lastline = true;
    return;
  }
  buffer->lastline = false;
  do {
    ch = getc(source);
    if (ch == '\n')
      ch = ' ';
    buffer->buf[buffer->length] = ch;
    if (buffer->length == maxlinelength) {
      printf(" ***%d*** line too long\n", linenumber);
      myabort();
    }
    buffer->length++;
  } while (!P_eoln(source));

  fscanf(source, "%*[^\n]");
  getc(source);

  if ( buffer->buf[1] == '%' ) {
      txt.len = 0;
      for ( i = 1; i < buffer->length; i++ )
	concatchar(&txt, buffer->buf[i] );
      string20(&uninclstring, "%$UNINCLUDE         ");
      stripspaces(&uninclstring);
      substring(&pragname, &txt, 1, 11);
      if (equalstrings(&pragname, &uninclstring)) {
	mymemcpy(newfname.str, blankstring, sizeof(stryngar));
	newfname.len = 0;
	pointer = 13;
	while (stringchar(&txt, pointer) != ',') {
	  concatchar(&newfname, stringchar(&txt, pointer));
	  pointer++;
	  }
	pointer++;
	mymemcpy(linenumstr.str, blankstring, sizeof(stryngar));
	linenumstr.len = 0;
	while (stringchar(&txt, pointer) != ')') {
	  concatchar(&linenumstr, stringchar(&txt, pointer));
	  pointer++;
	  }
	pointer = 1;
	newlinenum = stringnumber(&linenumstr, &pointer, 10);

	linenumber = newlinenum;
        buffer->lineno = linenumber;
	buffer->length = 3;
	buffer->buf[2] = '$'; /* SO NOT PRINTED BY printline */
	buffer->inputfile = curinputfile = newfname;

	/* printstring(newfname); */
	/* printf( "DEBUG READLINE linenumber=%d newlinenum=%d\n", linenumber, newlinenum ); */
        }
    }
  /* XXX PROCESS %$UNINCLUDE(file,line) XXX */

  buffer->buf[buffer->length] = ' ';

  /* else  */
}  /* readline */


Static Void getline(buffer)
inputbuffer **buffer;
{
  /*(var buffer: InBufPtr); forward*/
  /*-----------------------------------------------------------------
    read one line of input or move to next line if one is buffered
     print previous line, if appropriate
   ------------------------------------------------------------------*/
  inputbuffer *temp;

  if (!peeking && (listing || (*buffer)->modified))
    printline(*buffer);
  if ((*buffer)->next != NULL) {
    if (peeking)
      *buffer = (*buffer)->next;
    else {
      temp = *buffer;
      *buffer = (*buffer)->next;
      MyFree(temp); /* HERE HERE HERE CANN */
    }
    (*buffer)->pointer = 1;
    return;
  }
  if (peeking) {
    (*buffer)->next = (inputbuffer *)Malloc(sizeof(inputbuffer));
    (*buffer)->next->next = NULL;
    *buffer = (*buffer)->next;
    (*buffer)->underline = NULL;
  }
  readline(*buffer);

  /* else */
}  /* getline */


Static Void readchar(ch, buffer)
Char *ch;
inputbuffer **buffer;
{
  /*---------------------------------------------------------------
    read in one character from the buffer.
    get the next line if necessary
   ----------------------------------------------------------------*/
  inputbuffer *WITH;

  if ((*buffer)->pointer > (*buffer)->length) {   /* at the end of line */
    if ((*buffer)->lastline)
      endfile = true;
    else
      getline(buffer);
  }
  WITH = *buffer;   /* with */
  *ch = WITH->buf[WITH->pointer];
  WITH->pointer++;
}  /* readchar */


Static Void unreadchar(ch, buff)
Char ch;
inputbuffer *buff;
{
  /*----------------------------------------------------------------
    put a character back into the buffer
   -----------------------------------------------------------------*/
  if (debug)
    printf("start unreadchar\n");
  buff->pointer--;
  if (buff->buf[buff->pointer] != ch) {   /* with */
    printf(" incorrect unreadchar \"%c\" %d\n", ch, buff->pointer);
    printf(" char was \"%c\"\n", buff->buf[buff->pointer]);
    printline(buff);
    myabort();
  }
  if (debug)
    printf("end unreadchar\n");
}  /* unreadchar */


Static Void insertcharbuf(ch, loc, buffer)
Char ch;
unchar loc;
inputbuffer *buffer;
{
  /*-------------------------------------------------------------------
      stick ch into the line held in buffer, at location loc
  --------------------------------------------------------------------*/
  unchar i;

  if (debug)
    printf("start insertcharbuf\n");
  if (buffer->length == maxlinelength)
    inoverflow();
  buffer->length++;
  for (i = buffer->length; i > loc; i--)
    buffer->buf[i] = buffer->buf[i - 1];
  buffer->buf[loc] = ch;
  if (debug)
    printf("end insertcharbuf\n");
}  /* insertcharbuf */


Static Void displaydeletions(pighowmany)
int pighowmany;
{
  /*----------------------------------------------------------------
    show that pighowmany tokens have been deleted
    (calls scan)
    be careful to leave the pointer pointing to the right place
    which is the next character after the change.
    ----------------------------------------------------------------*/
  int i;
  short sym;
  inputbuffer *savbuf;

  if (debug)
    printf("start displaydeletions\n");
  modify(oldline);
  insertcharbuf('{', tokenstart - 1, oldline);   /* put in a bracket */
  oldline->pointer++;
  savbuf = oldline;
  oldline->pointer = tokenstart;
  for (i = 1; i <= pighowmany; i++) {  /* move past pighowmany tokens */
    modify(oldline);
    scan(&sym, &tokenlineno, &oldline);
  }
  modify(oldline);
  insertcharbuf('}', oldline->pointer, oldline);   /*put in a bracket*/
  insertcharbuf(' ', oldline->pointer + 1, oldline);   /* and a blank */
  oldline->pointer += 2;
  tokenstart = oldline->pointer;
  peeking = false;
  while (oldline != savbuf)   /* print the skipped lines */
    getline(&savbuf);
  linebuf = oldline;   /* next char will be first AFTER deletions */
  linebuf->pointer = tokenstart;
  if (debug)
    printf("end displaydeletions\n");
}  /* displaydeletions */


Static Void displayinsertion(token)
short token;
{
  /*-------------------------------------------------------------
    show that token has been inserted
    put it before 'tokenstart'
    leave the pointer pointing after the change.
    -------------------------------------------------------------*/
  /* TWO TYPE CHANGES FOR LARGER LINE BUFFERS 1/92 CANN */
  /* unchar */ unsigned int oldindex;   /* index into old part of line */
  /* unchar */ unsigned int newindex;   /* index into new part of line */
  short strindex;   /* index into the character rep for token */
  inputbuffer *WITH;
  unchar FORLIM;

  if (debug)
    printf("start displayinsertions\n");
  WITH = oldline;   /* with */
  modify(oldline);
  if (WITH->underline == NULL) {  /* create the underline line */
    WITH->underline = (Char *)Malloc(sizeof(linebuffer));
    for (oldindex = 1; oldindex <= maxlinelength; oldindex++) {
      if (oldindex <= WITH->length) {
	if (WITH->buf[oldindex] == tabord)
	  WITH->underline[oldindex] = (Char)tabord;
	else
	  WITH->underline[oldindex] = ' ';
      } else
	WITH->underline[oldindex] = ' ';
    }
  }
  if (WITH->length + p.resword[token + 2].len + 1 > maxlinelength)
    inoverflow();
  newindex = p.resword[token + 2].len + WITH->length + 1;
  FORLIM = tokenstart - 1;
  for (oldindex = WITH->length; oldindex >= FORLIM; oldindex--)
  {  /* make room in the line */
    WITH->buf[newindex] = WITH->buf[oldindex];
    newindex--;
  }
  WITH->length += p.resword[token + 2].len + 1;
  strindex = 1;
  FORLIM = tokenstart + p.resword[token + 2].len;
  for (newindex = tokenstart - 1; newindex < FORLIM; newindex++)
  {  /* insert the characters */
    WITH->buf[newindex] = stringchar(&p.resword[token + 2], strindex);
	/*stringspace[strindex];*/
    WITH->underline[newindex] = '*';
    strindex++;
  }
  WITH->pointer = tokenstart + p.resword[token + 2].len;
  tokenstart = WITH->pointer + 1;
  /*next char is first after insertions */
  WITH->buf[WITH->pointer - 1] = ' ';
  WITH->underline[WITH->pointer - 1] = ' ';
  linebuf = oldline;
  if (debug)
    printf("end displayinsertion\n");
}  /* DisplayInsertion */


/*===================SCANNER======================================*/
/*================================================================*/
/*----------------------------------------------------------------*/
Static Void scanerror()
{
  /*----------------------------------------------------------------*/
  if (listing) {
    fprintf(listfile, "ILLEGAL CHARACTER \" ");
    writestring(listfile, &inputtoken);
    fprintf(listfile, "\" found in input, was ignored! Line %d\n", linenumber);
  }
  printf("ILLEGAL CHARACTER \"");
  writestring(stdout, &inputtoken);
  printf("\" found in input, was ignored! Line %d\n", linenumber);
}


/*------------------------------------------------------------------*/
Static short searchsymboltable(low, high, s, default_)
int low, high;
stryng *s;
short default_;
{
  /*------------------------------------------------------------------*/
  int guess;
  boolean notfound;

  /* Searchsymboltable */
  notfound = true;
  while (low <= high && notfound) {   /* while */
    guess = (low + high) / 2;
    if (P_getbits_UB(debugflags, '1', 0, 3))
      printf("Guess = %d\n", guess);
    switch (stringcompare(s, &(p.resword[guess + 2]), true)) {

    case compareless:
      high = guess - 1;
      break;

    case compareequal:
      notfound = false;
      break;

    case comparegreater:
      low = guess + 1;
      break;
    }/* case */
  }
  if (notfound)
    return default_;
  else
    return guess;
}


#define backspace_      8
#define tabord_         9
#define formfeed        12
/* p2c: mono.bin.noblank.p, line 22102: 
 * Note: Changed "* /" to "% /" in comment [140] */


/* Local variables for scan: */
struct LOC_scan {
  Char ch;
  boolean done;
} ;

/*----------------------------------------------------------*/
Local short lookupterm(term, default_, LINK)
stryng *term;
short default_;
struct LOC_scan *LINK;
{
  /*----------------------------------------------------------*/
  short result;

  result = searchsymboltable(firstkeyword, lastkeyword, term, default_);
  if (P_getbits_UB(debugflags, '1', 0, 3))
    printf("LookupTerm returned %d\n", result);
  return result;
}  /*LookupTerm*/

/*-------------------------------------------------------------------*/
Local Void ctrlchtooctal(c, s, LINK)
Char c;
stryng *s;
struct LOC_scan *LINK;
{
  /*---------------------------------------------------------------*/
  int i, d, m;

  if ((unchar)c < 32 &&
      ((1L << c) & ((1L << backspace_) | (1L << tabord_) | (1L << newline) |
		    (1L << formfeed))) != 0) {
    switch (c) {

    case backspace_:
      string10(s, "\\b        ");
      break;

    case tabord_:
      string10(s, "\\t        ");
      break;

    case newline:
      string10(s, "\\n        ");
      break;

    case formfeed:
      string10(s, "\\f        ");
      break;
    }/* case */
    return;
  }
  d = c;
  string10(s, "\\         ");
  for (i = 4; i >= 2; i--) {
    m = d & 7;
    d /= 8;
    insertchar(s, m + '0', i);
  }  /* for */

  /* else */
}  /* CtrlChToOctal */

#undef backspace_
#undef tabord_
#undef formfeed

/*---------------------------------------------------------------------*/
Local int branchonfirstchar(LINK)
struct LOC_scan *LINK;
{
  /*---------------------------------------------------------------------*/
  int Result;

  /* BranchOnFirstChar */
  if (P_getbits_UB(debugflags, '1', 0, 3))
    printf("entering BranchOnFirstChar, ord(ch) = %12d\n", LINK->ch);
  /*REA      if (ch in P.CmdChars) then
          begin
            Done := true;
            BranchOnFirstChar := 23
          endREA*/
  if (LINK->ch == blankord || LINK->ch == newline || LINK->ch == tabord ||
      LINK->ch == fford) {
    Result = 0;
    firsttime = true;
    deletestring(&inputtoken, inputtoken.len, inputtoken.len);
    return Result;
  }
  /*  if ((LINK->ch < ' ' || LINK->ch > '~') | P_inset(LINK->ch, badcharset))
      return 24;  Using the default in the switch below instead */
  switch (LINK->ch) {   /* case ch */

  case '+':
  case '-':
  case '*':
  case '/':
  case '&':
  case '(':
  case ')':
  case '[':
  case ']':
  case ',':
  case ';':
  case '.':
  case '=':
    LINK->done = true;
    Result = 1;
    break;

  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
    Result = 2;
    break;

  case '\'':
    Result = 25;
    break;

  case '"':
    Result = 30;
    break;

  case '#':
  case '%':
    Result = 5;
    break;

  case ':':
    Result = 6;
    break;

  case '<':
    Result = 8;
    break;

  case '>':
    Result = 12;
    break;

  case '~':
    Result = 14;
    break;

  case '|':
    Result = 16;
    break;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    Result = 18;
    break;

  default:
    Result = 24;
  }

  return Result;
}

Local Void scannererr(err, LINK)
errtype err;
struct LOC_scan *LINK;
{
  switch (err) {

  case charerr:
    printf("SCANNER ERROR ==>  invalid character constant\n");
    break;
  }/*case*/
}

Local Void unappend(string, buffer, LINK)
stryng *string;
inputbuffer **buffer;
struct LOC_scan *LINK;
{
  /*---------------------------------------------------------------
         remove last char from string
  ----------------------------------------------------------------*/
  unreadchar(stringchar(string, string->len), *buffer);
  deletestring(string, string->len, string->len);
}  /* unappend */


/*-----------------------------------------------------------*/
Static Void scan(token, tokenlineno, buffer)
short *token;
int *tokenlineno;
inputbuffer **buffer;
{  /* Scanner */
  /* (var token : SymIndex ; var buffer: InBufPtr );forward */
  /* written by sks 83/2/2          */
  /*  rev 1  by sks 83/8/8  fixup on < had wrong state*/
  /* modifies my rea 85/6   for use with fmq error correcting parser       */
  /* imports: varsBadCharSet, idcharset, digitset, AltFFlag, EscapeChar*/
  /*P.numsymbols, P.CmdChars             */
  /*  const IdToken, EscapeToken, RealToken, IntegerToken, CatToken*/
  /*StringToken, AssignToken, LeToken, EqToken, GrToken*/
  /* exports: inputtoken, globalchar, inttokenval*/
  /* results: "token" gets token number or EscapeToken*/
  /*   if the latter, first char of inputtoken is either*/
  /*     space, backspace or escape*/
  /*"inputtoken" character string of the token*/
  /*"globalchar" character that caused scanning error*/
  /*"inttokenval" assigned when token=IntegerToken*/
  /*-----------------------------------------------------------*/
  struct LOC_scan V;
  int state;
  long octalchset[9];
  /*------------------------------------------------------
       TRANSITION DIAGRAM FOR THE VAL SCANNER
  (0)---{+-%/()[].,:;}--->((1))
   |
   |  .-------------.
   |  V             |
   +---letter---> ((2))---idchar--'
   |
   |
   |
   +---single quote---> (25) ---{not \}-------------> (28) ---quote---> ((29))
   |        |                            ^                  ^
   |                      |                            `----------.       |
   |        `---{\}---> (26) ---quote or nonoctal---'       |
   |                                   ||
   |                                   `---octal---(27)---quote-----------'
   |                                                |^
   |  |`----------.
   |  `---octal---'
   |
   |                     .---{not \ and not "}---.
   |                     |.----------------------'
   |                     |V
   +---double quote---> (30) ---double quote--------------------------> ((33))
   |                    |^^^
   |                    ||`--------------nonoctal---.|
   |                    |`---{" or nonoctal}---.    ||
   |      |             .--------'    ||
   |      |             V             ||
   |                    `---{\}---> (31)            ||
   |                                 `---octal---> (32) ---double quote---'
   |  |^
   |  |`----------.
   |  `---octal---'
   |
   |       .-------------------.
   |       V   |
   +---{%}---> ((5)) ---not newline--'
   |
   +---{:}---> ((6)) ---{=}---> ((7))
   |
   +---{<}---> ((8)) ---{=}---> ((9))
   |       |
   |       `--->{nonblank}--->(10)------------------+--{>}--->((11))
   |   ||
   |   `<---{not newline}---'
   |
   +---{>}---> ((12)) ---{=}---> ((13))
   |
   +---{tilde}---> ((14)) ---{=}---> ((15))
   |
   |---{|}---> ((16)) ---{|}---> ((17))
   |
   |  .-digits-.
   |  V    |
   +---digits---> ((18)) ----+   .-digits-.
   |  |            V        |
   |  `--{.}---> ((19)) ----+
   |       |
   |       `--{eE}---> (20) ---{+-}---> (21)
   |    |                |
   |                                          |              digits
   |    |                |
   |---{blank backspace escape}---> ((23))    |                +-digits-.
   |    |                |        |
   |                                          |                V        |
   `---badchar---> ((24))                     `---digits---> ((22)) ----'
  -----------------------------------------------------------------*/
  /*----------------- S c a n n e r ---------------------------------*/
  stryng newch;
  int i, pointer;
  stryng pragname, uninclstring, linenumstr;

  if (debug)
    printf("start scan\n");
  P_addsetr(P_expset(octalchset, 0L), '0', '7');
  mymemcpy(inputtoken.str, blankstring, sizeof(stryngar));
  inputtoken.len = 0;
  state = 0;
  statehistory[0] = state;
  V.done = false;
  if (P_getbits_UB(debugflags, '1', 0, 3))
    printf("Called Scanner.\n");
  firsttime = true;   /* DO THIS A BETTER WAY */
  do {
    readchar(&V.ch, buffer);
    if (!peeking && firsttime) {
      *tokenlineno = (*buffer)->lineno;
      tokenstart = (*buffer)->pointer;
      firsttime = false;
    }
    if (endfile) {
      V.done = true;
      *token = eoftoken;   /* end of file */
    } else {
      concatchar(&inputtoken, V.ch);
      switch (state) {   /* case State */

      case 0:
	state = branchonfirstchar(&V);
	if (debug)
	  printf("branch on first char = %12d\n", state);
	break;

      /* Should never reach here */
      case 1:
	printf("Scanner error (state 1 reached) -- not your fault\n");
	break;

      /* Building an id... */
      case 2:
	V.done = !P_inset(V.ch, p.idcharset);
	break;

      /* Building a comment */
      case 5:
	if ((*buffer)->pointer > (*buffer)->length) {  /* at eoln */
	  deletestring(&inputtoken, inputtoken.len, inputtoken.len);
	  V.done = true;
	}
	break;

      /* Colon or Assign? */
      case 6:
	V.done = true;
	if (V.ch == '=')
	  state = 7;
	else
	  unappend(&inputtoken, buffer, &V);
	break;

      /*   Got a <, look for =, space, NewLine, etc */
      case 8:
	V.done = true;
	if (V.ch == '=')
	  state = 9;
	else
	  unappend(&inputtoken, buffer, &V);
	break;

      /* Got a >, check for >=  */
      case 12:
	V.done = true;
	if (V.ch == '=')
	  state = 13;
	else
	  unappend(&inputtoken, buffer, &V);
	break;

      /*  Got a tilde, check for tilde=  */
      case 14:
	V.done = true;
	if (V.ch == '=')
	  state = 15;
	else
	  unappend(&inputtoken, buffer, &V);
	break;

      /*  Got a |, check for '||' */
      case 16:
	V.done = true;
	if (V.ch == '|')
	  state = 17;
	else
	  unappend(&inputtoken, buffer, &V);
	break;

      /* Build an integer */
      case 18:
	if (P_inset(V.ch, p.digits)) {
	  /* nothing */
	  state = 18;
	} else if (V.ch == '.')
	  state = 19;
	else if (V.ch == 'E' || V.ch == 'e')
	  state = 20;
	else if (V.ch == 'D' || V.ch == 'd')
	  state = 35;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Get fraction */
      case 19:
	if (P_inset(V.ch, p.digits))
	  state = 19;
	else if (V.ch == 'E' || V.ch == 'e')
	  state = 20;
	else if (V.ch == 'D' || V.ch == 'd')
	  state = 35;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Get exponent */
      case 20:
	if (V.ch == '+' || V.ch == '-')
	  state = 21;
	else if (P_inset(V.ch, p.digits))
	  state = 22;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Saw an exponent sign, better get a digit */
      case 21:
	if (P_inset(V.ch, p.digits))
	  state = 22;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Get rest of exponent */
      case 22:
	if (!P_inset(V.ch, p.digits)) {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      case 24:  /* found an illegal char declare error and ignore */
	scanerror();
	mymemcpy(inputtoken.str, blankstring, sizeof(stryngar));
	inputtoken.len = 0;
	state = 0;
	break;

      case 25:
	if (V.ch == '\\')
	  state = 26;
	else {
	  state = 28;
	  if (V.ch < ' ') {
	    deletestring(&inputtoken, inputtoken.len, inputtoken.len);
	    printf("calling Ctrltoct  ch = %c  ordch = %4d\n", V.ch, V.ch);
	    ctrlchtooctal(V.ch, &newch, &V);
	    i = 1;
	    while (stringchar(&newch, i) != ' ') {
	      fputc(stringchar(&newch, i), stdout);
	      concatchar(&inputtoken, stringchar(&newch, i));
	      /*REAEchoChar (NewCh[I]);      REA*/
	      i++;
	    }  /* while */
	    putchar('\n');
	  }  /* if */
	}  /* else */
	break;

      case 26:
	if (P_inset(V.ch, octalchset))
	  state = 27;
	else
	  state = 28;
	break;

      case 27:
	if (P_inset(V.ch, octalchset))
	  state = 27;
	else if (V.ch == '\'') {
	  state = 29;
	  V.done = true;
	} else if (V.ch == newline) {
	  state = 34;
	  V.done = true;
	} else {
	  state = 27;
	  V.done = true;
	}
	break;

      case 28:
	if (V.ch != '\'')
	  state = 27;
	V.done = true;
	break;

      case 30:
	if (V.ch == '"') {
	  state = 33;
	  V.done = true;
	} else if (V.ch == '\\')
	  state = 31;
	else if (V.ch >= ' ')
	  state = 30;
	else {
	  deletestring(&inputtoken, inputtoken.len, inputtoken.len);
	  ctrlchtooctal(V.ch, &newch, &V);
	  i = 1;
	  while (stringchar(&newch, i) != ' ') {
	    concatchar(&inputtoken, stringchar(&newch, i));
	    /*REA    EchoChar (NewCh[I]);REA*/
	    i++;
	  }  /* while */
	}
	break;

      case 31:
	if (P_inset(V.ch, octalchset))
	  state = 32;
	else
	  state = 30;
	break;

      case 32:
	if (V.ch == '"') {
	  state = 33;
	  V.done = true;
	} else if (P_inset(V.ch, octalchset))
	  state = 32;
	else
	  state = 30;
	break;

      /* Get exponent */
      case 35:
	if (V.ch == '+' || V.ch == '-')
	  state = 36;
	else if (P_inset(V.ch, p.digits))
	  state = 37;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Saw an exponent sign, better get a digit */
      case 36:
	if (P_inset(V.ch, p.digits))
	  state = 37;
	else {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;

      /* Get rest of exponent */
      case 37:
	if (!P_inset(V.ch, p.digits)) {
	  V.done = true;
	  unappend(&inputtoken, buffer, &V);
	}
	break;
      }
      statehistory[inputtoken.len] = state;
      /*REA  end;  REA*/
      if (P_getbits_UB(debugflags, '1', 0, 3))
	printf("State and length are %4d%4d\n", state, inputtoken.len);
    }  /*else*/
  } while (!V.done);
  /* Now we have found a token string; it's time to assign the token */
  /* value and do any fixup (correct improper numbers, non-terminals  */
  if (*token != eoftoken) {   /* with P */
    switch (state) {   /* case State */

    /* done -- should be escape or bad character */
    case 0:
      break;

    /* single-character tokens.  ch has char of interest */
    case 1:
      *token = singlechar[V.ch - ' '];
      break;

    case 2:
      unappend(&inputtoken, buffer, &V);
      *token = lookupterm(&inputtoken, idtoken, &V);
      break;

    case 5:
      *token = commenttoken;
      commentflag = true;
      string20(&uninclstring, "%$UNINCLUDE         ");
      stripspaces(&uninclstring);
      substring(&pragname, &inputtoken, 1, 11);
      if (equalstrings(&pragname, &uninclstring)) {
	*token = incltoken;
	mymemcpy(newfname.str, blankstring, sizeof(stryngar));
	newfname.len = 0;
	pointer = 13;
	while (stringchar(&inputtoken, pointer) != ',') {
	  concatchar(&newfname, stringchar(&inputtoken, pointer));
	  pointer++;
	}
	pointer++;
	mymemcpy(linenumstr.str, blankstring, sizeof(stryngar));
	linenumstr.len = 0;
	while (stringchar(&inputtoken, pointer) != ')') {
	  concatchar(&linenumstr, stringchar(&inputtoken, pointer));
	  pointer++;
	}
	pointer = 1;
	newlinenum = stringnumber(&linenumstr, &pointer, 10);
      }
      break;

    case 6:
      *token = singlechar[':' - ' '];
      break;

    case 7:
      *token = assigntoken;
      break;

    case 8:
      *token = lttoken;
      break;

    case 9:
      *token = letoken;
      break;

    case 10:
      *token = singlechar['<' - ' '];
      break;

    case 11:
      *token = singlechar['<' - ' '];
      break;

    case 12:
      *token = gttoken;
      break;

    case 13:
      *token = getoken;
      break;

    case 14:
      *token = singlechar['~' - ' '];
      break;

    case 15:
      *token = netoken;
      break;

    case 16:
      *token = singlechar['|' - ' '];
      break;

    case 17:
      *token = cattoken;
      break;

    case 18:
      *token = integertoken;
      rea = 1;
      /* CHANGE THIS NEEDS TO BE PASSED
                                            A VARIABLE    */
      inttokenval = stringnumber(&inputtoken, &rea, 10);
      break;

    case 19:
    case 22:
      *token = realtoken;
      break;

    case 20:
    case 21:
      concatchar(&inputtoken, '0');
      *token = realtoken;
      break;

    case 23:
      *token = escapetoken;
      break;

    /*    24: begin
              globalchar := ch;
              scanerror;
              token := EscapeToken
            end;
    */
    case 25:
    case 26:
      concatchar(&inputtoken, '\'');
      *token = chartoken;
      break;

    case 27:
      unappend(&inputtoken, buffer, &V);
      concatchar(&inputtoken, '\'');
      scannererr(charerr, &V);
      *token = chartoken;
      break;

    case 28:
      *token = chartoken;
      break;

    case 29:
      if (inputtoken.len != 6)
	scannererr(charerr, &V);
      *token = chartoken;
      break;

    case 34:
      deletestring(&inputtoken, inputtoken.len, inputtoken.len);
      concatchar(&inputtoken, '\'');
      scannererr(charerr, &V);
      *token = chartoken;
      break;

    case 30:
    case 31:
      concatchar(&inputtoken, '"');
      *token = stringtoken;
      break;

    case 32:
      deletestring(&inputtoken, inputtoken.len, inputtoken.len);
      concatchar(&inputtoken, '"');
      *token = stringtoken;
      break;

    case 33:
      *token = stringtoken;
      break;

    case 35:
    case 36:
      concatchar(&inputtoken, '0');
      *token = doubtoken;
      break;

    case 37:
      *token = doubtoken;
      break;
    }
    if (P_getbits_UB(debugflags, '1', 0, 3)) {
      printf("   token(%2d) : \"", *token);
      writestring(stdout, &inputtoken);
      printf("\"\n");
    }
    tokenposition = 0;
  }  /*then*/
  if (debug)
    printf("end scan\n");

  /* else */
}  /* Scanner */


/*===============================================================
     scanner routines for llparse
     The scanner provided is almost certainly inadequate for
     use in a real-life situation, and will have to be replaced.
     It should be possible to replace only procedure 'scan'.
     If this is done, scan should take the same parameters,
     and get its input from 'readchar'.  Since scan is called from
     the corrector (actually from peek) extra side-effects, such
     as symbol-table manipulations, should be avoided, at least
     during correction. The side-effects should be placed in 'gettok'
     which is only called by the parser.
     Most of the rest of this discussion applies when more procedures
     are being replaced.
     The scanner 'module' interfaces with the parser, the corrector
     and the 'module' which reads and prints lines, hereafter referred
     to as 'getline'.
     procedure 'initscanner' is called by the parser to allow any
     initialization to be done
     The parser gets symbols from the scanner by calling 'gettok',
     which returns the next symbol to be parsed.  For purposes of
     possible error correction, gettok keeps track of the starting
     position of the token just sent (oldline and tokenstart;)
     The corrector looks ahead at upcoming symbols through 'peek'.
     Peek does not change the value of the 'starting postion' kept by
     gettok.  Symbols peeked at must be available for normal use by
     gettok at a later time. To this end, they may be kept in a
     buffer, or they may be physically rescanned a second time.
     The current implementation uses the rescanning. If symbols are kept
     in a buffer, some provision must be made to keep semantic information
     (such as the characters in an identifier) around until the symbol
     is actually used by the parser.  Side-effects of the scanner,
     such as symbol-table manipulations, should be avoided during peeking.
     A global boolean, 'peeking', is used to indicate this. Peeking is
     set by peek, cleared by gettok, and may also be used in 'getline'.
     The corrector effects corrections by two procedures: 'deletetokens'
     and 'inserttokens'. Deletetokens has one argument, giving the number
     of (previously scanned) symbols to be deleted. This count begins
     at the point of error, the starting point kept by gettok.
     (notice that in the current implemention, this is handled by 'getline').
     Inserttokens has an argument of record type 'StageRec', which contains
     an array of symbols to be inserted. These symbols are to be inserted
     before the first non-deleted symbol in the input. If any symbols are
     to be deleted, deletetokens will be called before inserttokens.
     It is the responsibility of deletetokens and inserttokens to
     call the appropriate routines (in getline) to display the corrections
     made.
     After a correction has been made, all the inserted symbols plus
     at least one symbol in the input will be comsumed by the parser.
     If this is not the case, an internal error has occurred, most likely
     in the corrector. procedure checkerrorok checks this situation.
     The distinction between the scanner module and the getline module is
     fairly fuzzy.  In the current implementation, the scanner depends
     on getline to reposition the scanning pointer after corrections are made.
     In order to do this, getline calls 'scan' to skip past one token.
     End of file is reporsted through the global boolean 'endfile'.  No attempt
     to read will be made while endfile is true.  Since a correction may cause
     input to be rescanned, endfile must be cleared by the correction routines.
     When the end is again reached, endfile will be set again.
================================================================*/
Static Void initscanner()
{
  /*---------------------------------------------------------------
    do any initialization needed
   ----------------------------------------------------------------*/
  int tokennumber;

  if (debug)
    printf("start initscanner\n");
  P_addsetr(P_expset(p.digits, 0L), '0', '9');
  P_addsetr(P_expset(p.idcharset, 0L), 'A', 'Z');
  P_addsetr(p.idcharset, 'a', 'z');
  P_addsetr(p.idcharset, '0', '9');
  P_addset(p.idcharset, '_');
  /* initialize the SingleCharacter array by looking at the reserved*/
  /*  word list that are one character long*/
  for (tokennumber = numterms; tokennumber >= 1; tokennumber--) {   /* with */
    if (p.resword[tokennumber + 2].len == 1)
      singlechar[stringchar(&p.resword[tokennumber + 2], 1) - ' '] = tokennumber;
  }
  ordzero = '0';
  savetoken = eoftoken;
  newcommand = ' ';
  P_addset(P_expset(badcharset, 0L), '!');
  P_addset(badcharset, '#');
  P_addset(badcharset, '$');
  P_addset(badcharset, '^');
  P_addset(badcharset, '\\');
  P_addset(badcharset, '{');
  P_addset(badcharset, '}');
  P_addset(badcharset, '`');
  P_addset(badcharset, '_');
  P_addset(badcharset, '@');
  reuseindex = 0;
  readaheadbuf = NULL;
  commentflag = false;
  if (debug)
    printf("end initscanner\n");
}  /* initscanner */


Static Void peek(sym)
short *sym;
{
  /*(var sym: SymIndex); forward */
  /*----------------------------------------------------------------
    get the next token
     leave old line pointer where it is,
     move new line pointer.
     set peeking
   -----------------------------------------------------------------*/
  if (debug)
    printf("start peek\n");
  peeking = true;
  scan(sym, &tokenlineno, &linebuf);
  if (debug)
    printf("end peek\n");
}  /* peek */


Static Void gettok(tok, tokenlineno)
short *tok;
int *tokenlineno;
{
  /*-----------------------------------------------------------------
    return the next token:
             if buffer empty then
                  read new token and return it
                  leaves the token read on the buffer
             else pop one off of buffer
    positions the old line buffer same as current buffer
   -----------------------------------------------------------------*/
  /* rabrec *ptr; */
  stryng tempstr, tempstr2;
  int i, FORLIM;

  if (debug)
    printf("start gettok\n");
  peeking = false;
  if (readaheadbuf != NULL) {
    *tok = readaheadbuf->token;
    if (cortrace) {
      printf(" from buffer:");
      printsym(*tok);
      putchar('\n');
    }
    if (*tok == idtoken) {
      string10(&inputtoken, "<NAME>    ");
      stripspaces(&inputtoken);
    }
    /* ptr = readaheadbuf; */
    readaheadbuf = readaheadbuf->next;
    /* Free(ptr); */
  } else {
    scan(tok, tokenlineno, &linebuf);
    if (cortrace) {
      printf(" from scan:(%d) ", linebuf->lineno);
      printsym(*tok);
      putchar('\n');
    }
    if (*tok == incltoken) {
      linenumber = newlinenum;
      curinputfile = newfname;
      if (newlinenum == 0)
	string20(&tempstr, "%---- entering file ");
      else
	string20(&tempstr, "%---- resuming file ");
      insertstring(&tempstr, &curinputfile, 21);
      string10(&tempstr2, "----      ");
      stripspaces(&tempstr2);
      if (tempstr.len < maxstringchars)
	insertstring(&tempstr, &tempstr2, tempstr.len + 1);
      FORLIM = tempstr.len;
      for (i = 1; i <= FORLIM; i++)
	linebuf->buf[i] = stringchar(&tempstr, i);
      linebuf->length = tempstr.len + 1;
      linebuf->buf[linebuf->length] = ' ';
      linebuf->pointer = linebuf->length + 1;
      /* We have make the line longer and must place the pointer at
         the end of the line  */
      linebuf->lineno = 0;
    }
    oldline = linebuf;
  }
  oldtoken = *tok;
  if (toconsume >= 0)
    toconsume--;
  if (debug)
    printf("end gettok\n");
}  /* gettok */


Static Void deletetokens(pighowmany)
int pighowmany;
{
  /* (pighowmany: integer);forward*/
  /*----------------------------------------------------------
    remove pighowmany tokens from buffer,
    and update display image to show tokens deleted
    ----------------------------------------------------------*/
  if (debug)
    printf("start deletokens\n");
  endfile = false;
  toconsume = 1;
  displaydeletions(pighowmany);
  if (debug)
    printf("end deletetokens\n");
}  /* deletetokens */


Static Void inserttokens(fix)
stagerec fix;
{
  /* (fix: StageRec);forward*/
  /*---------------------------------------------------------------
    push tokens listed in fix into buffer.
   also displays the print image of the token
   ----------------------------------------------------------------*/
  rabrec *ptr, *qtr;
  int iindex;

  if (debug)
    printf("start inserttokens\n");
  toconsume = fix.length + 1;
  if (fix.length >= 1) {
    ptr = (rabrec *)Malloc(sizeof(rabrec));
    ptr->token = fix.string[1];
    ptr->next = readaheadbuf;
    displayinsertion(ptr->token);
    readaheadbuf = ptr;
    qtr = ptr;
    for (iindex = 2; iindex <= fix.length; iindex++) {
      ptr = (rabrec *)Malloc(sizeof(rabrec));
      ptr->token = fix.string[iindex];
      ptr->next = qtr->next;
      qtr->next = ptr;
      qtr = ptr;
      displayinsertion(ptr->token);
    }
  }  /* if */
  endfile = false;
  if (debug)
    printf("end inserttiken\n");
}  /* inserttoken */


Static Void checkerrorok()
{
  /*--------------------------------------------------------------
    check that an error is 'acceptable' in this situation.
    that is, all the corrections from the last error have been consumed.
   ---------------------------------------------------------------*/
  if (debug)
    printf("start checkerrorok\n");
  if (toconsume >= 0) {
    printf("******** CORRECTOR UNABLE TO PROCEED ****************\n");
    printf("*** parse error in inserted symbols ****\n");
    printf("****** %4d symbols left       *********\n", toconsume);
    myabort();
  }
  if (readaheadbuf != NULL) {
    printf("******** CORRECTOR UNABLE TO PROCEED ****************\n");
    printf("*** parse error in inserted symbols ****\n");
    myabort();
  }
  if (debug)
    printf("end checkerrorok\n");
}  /* checkerrorok */


Static Void readtables(directoryname)
stryng directoryname;
{
  /*------------------------------------------------------------
    read in the various tables.
   -------------------------------------------------------------*/
  short sym;
  int termsym, paction;
  ptabrec *ptr;
  int prod, i, j, len, epcount;
  boolean successfulopen;
  stryng ptablename, ptablesuffix;
  int oneletter;   /* added by jyu to read the ptableout by chars */
  int FORLIM;
  prodrec *WITH;
  int FORLIM1;
  short FORLIM2;
  stringrec *WITH1;

  if (debug)
    printf("start readtables\n");
  string10(&ptablesuffix, "/ptabl.bin");
  stripspaces(&ptablesuffix);
  ptablename = directoryname;
  if (ptablename.len < maxstringchars)
    insertstring(&ptablename, &ptablesuffix, ptablename.len + 1);
  successfulopen = openintread(&ptableout, &ptablename);
  /*    reset(ptableout);    sks*/
  if (!successfulopen) {
    printf("Cannot find parsing tables in ");
    writestring(stdout, &ptablename);
    printf(".\n");
    myabort();
  }
  fread(&numterms, sizeof(int), 1, ptableout);
  fread(&numsymbols, sizeof(int), 1, ptableout);
  fread(&numprods, sizeof(int), 1, ptableout);
  fread(&stringptr, sizeof(int), 1, ptableout);
  fread(&oneletter, sizeof(int), 1, ptableout);
  /* need to readln, because a newline
                                                                      might be considered a character - jyu*/

  errortables = (oneletter == 'T');

  if (numsymbols > maxsym) {
    printf(" maximum is %12ld\n", (long)maxsym);
    myabort();
  }
  if (numterms > maxterm) {
    printf(" number of terminals is too big: %d\n", numterms);
    myabort();
  }
  if (numprods > maxprod) {
    printf(" number of productions is too big: %d\n", numprods);
    myabort();
  }
  if (stringptr > maxstring) {
    printf(" symbol string too big: %d\n", stringptr);
    myabort();
  }
  FORLIM = numprods;
  for (prod = 1; prod <= FORLIM; prod++) {
    WITH = &productions[prod];
    fread(&len, sizeof(int), 1, ptableout);
    WITH->length = len;
    WITH->start = prodspceptr + 1;
    FORLIM1 = WITH->length;
    for (i = 1; i <= FORLIM1; i++) {
      fread(&j, sizeof(int), 1, ptableout);
      prodspceptr++;
      prodspace[prodspceptr] = j;
    }
  }
  initptab();
  fread(&termsym, sizeof(int), 1, ptableout);
  fread(&paction, sizeof(int), 1, ptableout);
  FORLIM2 = numsymbols;
  for (sym = numterms + 1; sym <= FORLIM2; sym++) {
    if (termsym != 0 || paction != sym) {
      printf(" error in reading parse table%12d%12d%12d\n",
	     sym, termsym, paction);
      myabort();
    }
    fread(&termsym, sizeof(int), 1, ptableout);
    fread(&paction, sizeof(int), 1, ptableout);
    while (termsym != 0) {
      ptr = (ptabrec *)Malloc(sizeof(ptabrec));
      ptr->term = termsym;
      ptr->pact = paction;
      ptr->next = table[sym + 2];
      table[sym + 2] = ptr;
      fread(&termsym, sizeof(int), 1, ptableout);
      fread(&paction, sizeof(int), 1, ptableout);
    }
  }
  fread(&epcount, sizeof(int), 1, ptableout);
  FORLIM = numprods;
  for (prod = 1; prod <= FORLIM; prod++)
    epsprod[prod] = false;
  for (i = 1; i <= epcount; i++) {
    fread(&prod, sizeof(int), 1, ptableout);
    epsprod[prod] = true;
  }
  FORLIM2 = numsymbols + 2;
  for (sym = 3; sym <= FORLIM2; sym++) {
    WITH1 = &symkluge[sym];
    fread(&i, sizeof(int), 1, ptableout);
    fread(&j, sizeof(int), 1, ptableout);
    WITH1->start = i;
    WITH1->length = j;
  }
  FORLIM = stringptr;
  for (i = 1; i <= FORLIM; i++) {
    fread(&oneletter, sizeof(int), 1, ptableout);
    stringspace[i] = oneletter;
  }
  FORLIM = numsymbols + 2;
  for (i = 3; i <= FORLIM; i++) {
    FORLIM1 = symkluge[i].start + symkluge[i].length;
    for (j = symkluge[i].start; j < FORLIM1; j++)
      concatchar(&p.resword[i], stringspace[j]);
  }
  if (debug)
    printf("end readtables\n");
}  /* readtables */


Static Void push(prod)
short prod;
{
  /*----------------------------------------------------------
    push action onto parse stack
   -----------------------------------------------------------*/
  int i;
  prodrec *WITH;
  int FORLIM;

  if (debug)
    printf("start push, prod = %3d\n", prod);
  WITH = &productions[prod];
  if (stackptr + WITH->length > maxstack) {
    printf(" parse stack overflow\n");
    myabort();
  } else {
    FORLIM = WITH->start + WITH->length;
    for (i = WITH->start; i < FORLIM; i++) {
      stackptr++;
      parsestack[stackptr] = prodspace[i];
      if (debug)
	printf("pushing %4d\n", prodspace[i]);
    }
  }
  if (debug)
    printf("end push\n");
}  /* push */


Static int tos()
{
  /*--------------------------------------------------------------
    return the value at the top of the parse stack
   ---------------------------------------------------------------*/
  return (parsestack[stackptr]);
}  /* tos */


Static Void pop()
{
  /*------------------------------------------------------------
    pop pighowmany items from the stack.
   -------------------------------------------------------------*/
  if (debug)
    printf("start pop\n");
  if (stackptr < 1) {
    printf(" parse stack underflow\n");
    myabort();
  } else
    stackptr--;
  if (debug)
    printf("end pop\n");
}  /* pop */


Static Void trace()
{
  /*--------------------------------------------------------------
    print out a parse trace
   ---------------------------------------------------------------*/
  short i, FORLIM;

  if (debug)
    printf("start trace\n");
  printf(" Parse stack:");
  column = 10;
  FORLIM = stackptr;
  for (i = 1; i <= FORLIM; i++) {
    column++;
    putchar(' ');
    printsym(parsestack[i]);
  }
  putchar('\n');
  if (debug)
    printf("end trace\n");
}  /* trace */


Static boolean checkepsilonok(term)
char term;
{
  /*--------------------------------------------------------------
    check whether an epsilon production would be OK
  a la Fischer Tai Milton.
  ---------------------------------------------------------------*/
  boolean Result;
  short ptr;
  int sym, pact;
  boolean nomore;

  if (debug)
    printf("start checkepsilonok\n");
  Result = true;
  ptr = stackptr;
  do {
    sym = parsestack[ptr];
    if (sym < 0) {
      ptr--;
      nomore = (ptr == 0);
    } else if (sym <= numterms) {
      if (sym == term)
	nomore = true;
      else {
	nomore = true;
	Result = false;
      }
    } else {
      pact = findlltab(sym, term);
      if (pact == 0) {
	nomore = true;
	Result = false;
      } else if (epsprod[pact]) {
	ptr--;
	nomore = (ptr == 0);
      } else
	nomore = true;
    }
  } while (!nomore);
  if (debug)
    printf("end checkepsilonok\n");
  return Result;
}  /* checkepsilonok */


Static Void parseerror(token, directoryname)
short token;
stryng *directoryname;
{
  /*---------------------------------------------------------------
    Handle the syntax error.
    call LLcorrector to find the fix
   ----------------------------------------------------------------*/
  /*var  pos: integer;*/
  if (debug)
    printf("start parseerror\n");
  /*old version: puts a marker under each error*/
  /*
  if LISTING then begin
  write(blank:5);
  for pos := 1 to tokenstart do write('-');
  writeln('^');
  write(' ***** error line ');
  write(linebuf^.lineno:3);
  write(' on "');
  if oldtoken=numterms then
       write('end-of-file')
  else printstring(inputtoken);
  write('"');
  writeln;
  end; */
  checkerrorok();
  if (errortables) {
    if (cortrace) {
      printf("  parse stack :\n");
      trace();
    }

    llcorrector(&token, directoryname);

  } else {
    printf(" ** %d ** syntax error on \"", linenumber);
    printstring(inputtoken);
    printf("\"\n");
    printf(" no correction tables \n");
    myabort();
  }
  if (debug)
    printf("end parseerror\n");
}  /*parseerror*/


Static Void printstats()
{
  /*---------------------------------------------------------------
    print assorted statistics about whats going on
  ----------------------------------------------------------------*/
  printf(" %4d lines in program\n", linenumber - 1);
  printf(" %4d errors ( calls to corrector)\n", errorcount);
  printf(" %4d tokens inserted; %4d tokens deleted.\n",
	 tokinserted, tokdeleted);
  printf(" %4d semantic errors\n", semerrcnt);
  if (listing) {
    fprintf(listfile, " %4d lines in program\n", linenumber - 1);
    fprintf(listfile, " %4d errors ( calls to corrector)\n", errorcount);
    fprintf(listfile, " %4d tokens inserted; %4d tokens deleted.\n",
	    tokinserted, tokdeleted);
    fprintf(listfile, " %4d semantic errors\n", semerrcnt);
  }
  if (!gorydetail) {
    return;
  }  /* if gory detail */
  if (errorcount != 0)
    printf(" %4d deletions were considered, %3.2f per correction.\n",
	   numdeltried, (double)numdeltried / errorcount);
  putchar(' ');
}  /* PrintStats */


Static Void init()
{
  /*-=-----------------------------------------------------------
    initialize anything and everything except scanner, need to read tables first
   --------------------------------------------------------------*/
  inputbuffer *WITH;

  if (debug)
    printf("start init\n");
  stringptr = 0;
  errorcount = 0;
  numdeltried = 0;
  linenumber = 0;
  tokinserted = 0;
  tokdeleted = 0;
  curerrsym = 0;

  linebuf = (inputbuffer *)Malloc(sizeof(inputbuffer));

  WITH = linebuf;
  WITH->length = 0;
  WITH->pointer = 1;
  WITH->lineno = 0;
  WITH->modified = false;
  WITH->underline = NULL;
  WITH->next = NULL;
  oldline = linebuf;
  if (debug)
    printf("end init\n");
}  /* init */


Static Void parse(directoryname)
stryng *directoryname;
{
  /* ---------------------------------------------------------
   parse
  -----------------------------------------------------------*/
  int paction, sym;
  short token;
  boolean more;

  debug = false;
  cortrace = false;
  debugcg = false;
  /* init parser and scanner, read in tables */
  init();
  readtables(*directoryname);
  readerrtables(directoryname);
  initscanner();
  printf("version %s%s\n", version, compiled);
  /* begin parsing */
  stackptr = 0;
  push(numprods);
  if (tracing)
    trace();
  more = true;
  if (debugparam->parvalue.len > 0)
    more = debugger(-1, -1);
  gettok(&token, &tokenlineno);
  while (more) {
    if (commentflag) {
      commentflag = false;
      gettok(&token, &tokenlineno);
      continue;
    }  /* then */
    if (debug)
      printf("comment flag not set\n");
    sym = tos();
    if (sym < 0) {
      more = announceaction(-sym, token, linenumber, tokenstart, inputtoken);
      pop();
      continue;
    }
    if (sym <= numterms) {
      if (sym == token) {
	pop();
	if (token == eoftoken)
	  more = false;
	else
	  gettok(&token, &tokenlineno);
      } else {
	if (debug)
	  printf("parse error 1, sym,token = %3d%3d\n", sym, token);
	parseerror(token, directoryname);
	gettok(&token, &tokenlineno);
      }
    } else {  /*B*/
      paction = findlltab(sym, token);
      if (paction == 0) {
	if (debug)
	  printf("parse error 2, sym,token = %3d%3d\n", sym, token);
	parseerror(token, directoryname);
	gettok(&token, &tokenlineno);
      } else if (iedp && epsprod[paction]) {
	if (checkepsilonok(token)) {
	  pop();
	  push(paction);
	} else {
	  if (debug)
	    printf("parse error 3\n");
	  parseerror(token, directoryname);
	  gettok(&token, &tokenlineno);
	}
      } else {
	pop();
	push(paction);
      }
    }  /*B*/
    if (tracing)
      trace();
  }  /* while */
  flushlines();
  if (debugparam->parvalue.len > 0)
    more = debugger(-1, -1);
  printf("\n accepted\n");
  if (listing)
    fprintf(listfile, "\n accepted\n");
  printstats();

  /*A*/
  /*A*/
}  /* parse */


/* Local variables for pass1: */
struct LOC_pass1 {
  stryng fname;
  FILE *outfile;
} ;

Local Void readline_(linenum, linebuf, infile, LINK)
int linenum;
inputbuffer *linebuf;
FILE *infile;
struct LOC_pass1 *LINK;
{
  Char ch;
  int length;

  length = 0;
  do {
    length++;
    ch = getc(infile);
    if (ch == '\n')
      ch = ' ';
    linebuf->buf[length] = ch;
    if (length == maxlinelength - 1) {
      printf("***%d*** line too long, %d characters is maximum\n", linenum, maxlinelength);
      myabort();
    }
  } while (!P_eoln(infile));
  fscanf(infile, "%*[^\n]");
  getc(infile);
  linebuf->length = length;
}

Local boolean recursiveincl(fname, LINK)
stryng fname;
struct LOC_pass1 *LINK;
{
  namelistrec *listptr;
  boolean found;
  stryng lowername;

  listptr = includelist;
  found = false;
  stringlowercase(&fname);
  while (listptr != NULL) {
    lowername = listptr->name;
    stringlowercase(&lowername);
    if (equalstrings(&lowername, &fname)) {
      found = true;
      listptr = NULL;
    } else
      listptr = listptr->next;
  }  /*while*/
  return found;
}

Local Void pushinclstack(fname, linenum, LINK)
stryng fname;
int linenum;
struct LOC_pass1 *LINK;
{
  namelistrec *listptr;

  newnamelist(&listptr);
  listptr->name = fname;
  listptr->linenum = linenum;
  if (includelist == NULL)
    includelist = listptr;
  else {
    listptr->next = includelist;
    includelist = listptr;
  }
}

Local Void popinclstack(LINK)
struct LOC_pass1 *LINK;
{
  includelist = includelist->next;
}

Local Void dumpline(linebuf, LINK)
inputbuffer *linebuf;
struct LOC_pass1 *LINK;
{
  int pointer;

  pointer = 1;
  while (pointer <= linebuf->length) {
    putc(linebuf->buf[pointer], LINK->outfile);
    pointer++;
  }
  putc('\n', LINK->outfile);
}

#define MAX_NAMES 200

static char *EnTrY[MAX_NAMES];
static char *FoRtRaN[MAX_NAMES];
static char *CcC[MAX_NAMES];

int ENum = 0;
int FNum = 0;
int CNum = 0;

static char *CANN_StringCopy(length,buf)
int   length;
char *buf;
{
  register char *s;
  register char *r;
  register int   i;

  r = Malloc( length+1 * sizeof(char) );
  s = r;

  for ( i = 0; i < length; i++ )
    s[i] = buf[i];

  s[length] = '\0';
  return( r );
}

static int CANN_PreparePragmaName( s )
char *s;
{
  register int   e;
  register char *p;

  for ( e = 0, p = s; *p != '\0'; p++, e++ ) {
    if ( *p == '=' )
      return( e );

    if ( (*p >= 'a') && (*p <= 'z') )
      *p = (*p - 'a') + 'A';
    }

  return( -1 );
}

static int CANN_IsEntryPragma(b)
char *b;
{
  register int e;

  e = CANN_PreparePragmaName( b );

  if ( *b == 'E' )
    if ( *(b+1) == 'N' )
      if ( *(b+2) == 'T' )
        if ( *(b+3) == 'R' )
          if ( *(b+4) == 'Y' )
            if ( *(b+5) == '=' )
              return( e );

  return( -1 );
}

static int CANN_IsFortranPragma(b)
char *b;
{
  register int e;

  e = CANN_PreparePragmaName( b );

  if ( *b == 'F' )
    if ( *(b+1) == 'O' )
      if ( *(b+2) == 'R' )
        if ( *(b+3) == 'T' )
          if ( *(b+4) == 'R' )
            if ( *(b+5) == 'A' )
              if ( *(b+6) == 'N' )
                if ( *(b+7) == '=' )
                  return( e );

  return( -1 );
}

static int CANN_IsCPragma(b)
char *b;
{
  register int e;

  e = CANN_PreparePragmaName( b );

  if ( *b == 'C' )
    if ( *(b+1) == '=' )
      return( e );

  return( -1 );
}

static void CANN_ProcessPragmaFields(e,b,dbase,num)
int   e;
char *b;
char **dbase;
int  *num;
{
  register char *r;
  register char *p;
  register char *f;

  p = r = &(b[e+1]);

  while ( *p != '\0' ) {
    if ( *p == ',' )
      *p = '\0';

    p++;
    }

  f = p+1;
  p = r;

  while ( p != f ) {
    if ( *num >= MAX_NAMES ) {
      fprintf( stderr, "CANN_ProcessPragmaFields: dbase overflow!\n" );
      CANN_exit(1);
      }

    *dbase = p;
    dbase++;
    (*num)++;

    while ( *p != '\0' ) 
      p++;

    p++;
    }
}

static int CANN_DissectSpecialPragmas(pointer,length,buf)
int   pointer;
int   length;
char *buf;
{
  register char *b;
  register char *s;
  register int   e;

  b = &buf[pointer];
  s = CANN_StringCopy(length-2,b);

  if ( (e = CANN_IsEntryPragma( s )) > 0 )
    CANN_ProcessPragmaFields( e, s, &EnTrY[ENum], &ENum );
  else if ( (e = CANN_IsFortranPragma( s )) > 0 )
    CANN_ProcessPragmaFields( e, s, &FoRtRaN[FNum], &FNum );
  else if ( (e = CANN_IsCPragma( s )) > 0 )
    CANN_ProcessPragmaFields( e, s, &CcC[CNum], &CNum );
  else {
    /* free( s ); */ /* PROFILE */
    return( 0 );
    }

  return( 1 );
}

char *CANN_StripWhites( s )
char *s;
{
  register char *b;

  while ( *s == ' ' || *s == '\t' ) s++;

  if ( *s == '\0' )
    return(NULL);

  b = s;

  for ( s++; *s != ' ' && *s != '\t' && *s != '\0'; s++ ) /* NOTHING */ ;

  *s = '\0';

  return( b );
}

void CANN_DumpSpecialPragmas(fd)
FILE *fd;
{
  register int i;
  register char *s;

  for ( i = 0; i < ENum; i++ )
    if ( EnTrY[i][0] != '\0' ) {
      if ( (s = CANN_StripWhites(EnTrY[i])) == NULL ) 
	continue;

      fprintf( fd, "CE$%s\n", s );
      }

  for ( i = 0; i < FNum; i++ )
    if ( FoRtRaN[i][0] != '\0' ) {
      if ( (s = CANN_StripWhites(FoRtRaN[i])) == NULL ) 
	continue;

      fprintf( fd, "CF$%s\n", s );
      }

  for ( i = 0; i < CNum; i++ )
    if ( CcC[i][0] != '\0' ) {
      if ( (s = CANN_StripWhites(CcC[i])) == NULL ) 
	continue;

      fprintf( fd, "CC$%s\n", s );
      }
}

/* Local variables for dissectpragma: */
struct LOC_dissectpragma {
  struct LOC_pass1 *LINK;
  int linenum;
  pragmaname *pname;
} ;

Local Void error(LINK)
struct LOC_dissectpragma *LINK;
{
  printf("*** malformed pragma in file ");
  writestring(stdout, &LINK->LINK->fname);
  printf(", line %d ***\n", LINK->linenum);
  *LINK->pname = pother;
  myabort();
}

Local Void dissectpragma(linebuf, linenum_, pragmastart, pname_, pragmaparam,
			 LINK)
inputbuffer *linebuf;
int linenum_, pragmastart;
pragmaname *pname_;
stryng *pragmaparam;
struct LOC_pass1 *LINK;
{
  struct LOC_dissectpragma V;
  int namestart, nameend, paramstart, paramend, length, pointer;
  stryng name, includestr;
  long whitespace[9];

  V.LINK = LINK;
  V.linenum = linenum_;
  V.pname = pname_;
  length = linebuf->length;
  pointer = pragmastart;

  if ( CANN_DissectSpecialPragmas(pointer,length,linebuf->buf) ) {
    *V.pname = pother;
    return;
    }

  string10(&includestr, "include   ");
  stripspaces(&includestr);
  P_addset(P_expset(whitespace, 0L), blankord);
  P_addset(whitespace, tabord);
  while ((pointer <= length) & P_inset(linebuf->buf[pointer], whitespace))
    pointer++;
  if (pointer > length) {
    error(&V);
    return;
  }
  namestart = pointer;
  while (pointer <= length && linebuf->buf[pointer] != '(')
    pointer++;
  if (pointer > length) {
    error(&V);
    return;
  }
  nameend = pointer - 1;
  paramstart = pointer + 1;
  while (pointer <= length && linebuf->buf[pointer] != ')')
    pointer++;
  if (pointer > length) {
    error(&V);
    return;
  }
  paramend = pointer - 1;
  mymemcpy(name.str, blankstring, sizeof(stryngar));
  name.len = 0;
  for (pointer = namestart; pointer <= nameend; pointer++)
    concatchar(&name, linebuf->buf[pointer]);
  stripspaces(&name);
  stringlowercase(&name);
  if (!equalstrings(&name, &includestr)) {
    *V.pname = pother;
    return;
  }  /*then*/

/* CANN 2/92 */
fprintf( stderr, "\nERROR - %%$include IS OBSOLETE, USE THE C PREPROCESSOR\n\n" );
myabort();
/* CANN */

  *V.pname = pinclude;
  mymemcpy(pragmaparam->str, blankstring, sizeof(stryngar));
  pragmaparam->len = 0;
  for (pointer = paramstart; pointer <= paramend; pointer++)
    concatchar(pragmaparam, linebuf->buf[pointer]);
  stripspaces(pragmaparam);
  while (stringchar(pragmaparam, 1) == ' ')
    deletestring(pragmaparam, 1, 1);
  if ((stringchar(pragmaparam, 1) != '"') |
      (stringchar(pragmaparam, pragmaparam->len) != '"')) {
    error(&V);
    *V.pname = pother;
  }  /*then*/
  else {
    deletestring(pragmaparam, 1, 1);
    deletestring(pragmaparam, pragmaparam->len, pragmaparam->len);
  }  /*else*/

  /*else*/
  /*else*/
  /*else*/
}  /*DissectPragma*/


Static boolean pass1(txtp,fname_p, outfile_)
stryng *txtp;
stryng *fname_p;
FILE *outfile_;
{
  /* This pass will expand the  pragmas.  An internal pragma
     (UNINCLUDE) is used to pass infomation about the actual name of
     the input file and the line number within that file to the rest of
     the passes of the compiler.*/
  stryng txt,fname_;		/* PJM to fix Vax CC problem */
  struct LOC_pass1 V;
  boolean Result;
  int linenum;
  FILE *infile;
  boolean error;
  inputbuffer *linebuf;
  boolean pragma;
  pragmaname pname;
  stryng pragmaparam, inclstring;
  boolean blankline;
  int pointer, len;
  boolean endoffile /*, temperror*/;

  txt = *txtp;
  fname_ = *fname_p;

  V.fname = txt;
  V.outfile = outfile_;
  infile = NULL;
  if (!recursiveincl(fname_, &V)) {
    linenum = 0;
    error = false;
    string10(&inclstring, "include   ");
    stripspaces(&inclstring);

    linebuf = (inputbuffer *)Malloc(sizeof(inputbuffer));

    linebuf->underline = NULL;
    linebuf->next = NULL;
    if (!openread(&infile, &fname_)) {
      if (includelist == NULL) {
	concatchar(&V.fname, '.');
	concatchar(&V.fname, 's');
	concatchar(&V.fname, 'i');
	concatchar(&V.fname, 's');
	if (!openread(&infile, &fname_)) {
	  error = true;
	  printf("***ERROR File ");
	  writestring(stdout, &fname_);
	  printf("  not found\n");
	}
      } else {
	error = true;
	printf("***ERROR in pass1, File ");
	writestring(stdout, &fname_);
	printf("  not found\n");
      }
    }
    if (!error) {
      fprintf(V.outfile, "%%$UNINCLUDE(");
      writestring(V.outfile, &V.fname);
      fprintf(V.outfile, ",%d)\n", linenum);
      endoffile = false;
      while (!endoffile) {
	blankline = true;
	while (blankline) {
	  if (P_eof(infile)) {
	    endoffile = true;
	    blankline = false;
	    break;
	  }
	  if (!P_eoln(infile)) {
	    blankline = false;
	    break;
	  }
	  linenum++;
	  fscanf(infile, "%*[^\n]");
	  getc(infile);
	  putc('\n', V.outfile);
	}  /*while*/
	if (endoffile) {
	  break;
	}  /*then*/
	readline_(linenum, linebuf, infile, &V);
	linenum++;
	pointer = 1;
	len = linebuf->length;
	while (pointer <= len && linebuf->buf[pointer] != '%')
	  pointer++;
	pragma = true;
	if (linebuf->buf[pointer] != '%' || pointer + 1 >= len) {
	  dumpline(linebuf, &V);
	  continue;
	}  /*then*/
	pointer++;
	if (linebuf->buf[pointer] == '$')
	  dissectpragma(linebuf, linenum, pointer + 1, &pname, &pragmaparam,
			&V);
	else {
	  pragma = false;
	  dumpline(linebuf, &V);
	}
	if (!pragma) {
	  continue;
	}  /*then*/
	switch (pname) {

	case pinclude:
	  linebuf->length = pointer - 2;
	  if (linebuf->length > 0)
	    dumpline(linebuf, &V);
	  pushinclstack(V.fname, linenum, &V);
	  /* temperror = */ (Void)pass1(&pragmaparam,&pragmaparam, V.outfile);
	  popinclstack(&V);
	  fprintf(V.outfile, "%%$UNINCLUDE(");
	  writestring(V.outfile, &V.fname);
	  fprintf(V.outfile, ",%d)\n", linenum);
	  break;

	case pother:
	  dumpline(linebuf, &V);
	  break;
	}/*case*/
      }  /*while*/
    }  /*then*/
  }  /*then*/
  else {
    printf("*** recursive include pragma in file ");
    writestring(stdout, &includelist->name);
    printf(", line %12d ***\n", includelist->linenum);
  }  /*else*/
  Result = error;
  if (infile != NULL)
    fclose(infile);
  return Result;

  /*else*/
}  /*pass1*/


Local Void splitaelement(n)
node *n;
{
  /* - N is an AElement node with more than one index inputs on ports 2, 3,...
     - A new AElement node is constructed and placed in front of this node.
     - The first two inputs are moved to this node as its inputs.
     - An output is constructed from the new node to N
     - The ports of N are shifted left by 1
   */
  node *newn;
  port *arraye, *indexe /*, *newe*/;
  stentry *etype, *edgebasetype;
  stryng ename;

  newn = createsimplenode(ifnaelement);
  insertnode(n->ndparent, toprevnode(n), newn);
  arraye = getinputedge(n, 1);
  indexe = getinputedge(n, 2);
  changeedgedest(arraye, newn, 1);
  changeedgedest(indexe, newn, 2);
  shiftinputports(n, 1, -1);
  etype = arraye->pttype;
  if (etype->stsort == iftarray)
    edgebasetype = etype->UU.stbasetype;
  else
    edgebasetype = etype->UU.stbasetype;
  mymemcpy(ename.str, blankstring, sizeof(stryngar));
  ename.len = 0;
  /* newe = */ (Void)insertedge(newn, 1, n, 1, edgebasetype, ename);
}  /* SplitAElement */

Local Void convertareplacen(n)
node *n;
{
  /* - N is an AReplaceN node with the number of levels on port 2
     - Iteratively convert this node to a sequence of three nodes by:
       (1) adding an AElement node above N
       (2) converting N to an AReplaceN node of one lower level
       (3) adding an AReplace Node below N
     - Change the Remaining AReplaceN node to its equivalent AReplace node.
   */
  node *newaelem, *parent, *aelem, *arepl, *pn;
  port *levele, *arraye, /* *newe, */ *indexe, *e, *tmpe;
  stryng litvalue, levelstring, ename;
  int startpos, level, pp;
  stentry *arraytype, *indextype, *arraybasetype;

  parent = n->ndparent;
  aelem = toprevnode(n);
  levele = getinputedge(n, 2);
  /* NOTE:  LevelE MUST be a Literal edge */
  levelstring = levele->UU.ptlitvalue;
  startpos = 1;
  level = stringnumber(&levelstring, &startpos, 10);
  while (level > 1) {
    newaelem = createsimplenode(ifnaelement);
    insertnode(parent, aelem, newaelem);
    aelem = newaelem;
    arepl = createsimplenode(ifnareplace);
    insertnode(parent, n, arepl);
    /* Construct input edges for new AReplace node */
    arraye = getinputedge(n, 1);
    arraytype = arraye->pttype;
    ename = arraye->ptname;
    if (arraye->ptsort == ptlit) {
      litvalue = arraye->UU.ptlitvalue;
      /* newe = */ (Void)insertliteral(arepl, 1, arraytype, litvalue, ename);
    } else {
      pn = producernodeofedge(arraye);
      pp = producerportnumber(arraye);
      /* newe = */ (Void)insertedge(pn, pp, arepl, 1, arraytype, ename);
    }
    indexe = getinputedge(n, 3);
    indextype = indexe->pttype;
    ename = indexe->ptname;
    if (indexe->ptsort == ptlit) {
      litvalue = indexe->UU.ptlitvalue;
      /* newe = */ (Void)insertliteral(arepl, 2, indextype, litvalue, ename);
    } else {
      pn = producernodeofedge(indexe);
      pp = producerportnumber(indexe);
      /* newe = */ (Void)insertedge(pn, pp, arepl, 2, indextype, ename);
    }
    /* Move Output Edges from node N to ARepl */
    e = n->ndolist;
    while (e != NULL) {
      tmpe = e;
      e = e->UU.U1.ptfrnext;
      tmpe->pttype = arraytype;
      changeedgesrc(tmpe, arepl, 1);
    }
    /* Change Input and Output edges of node N */
    changeedgedest(arraye, aelem, 1);
    changeedgedest(indexe, aelem, 2);
    shiftinputports(n, 3, -1);
    arraybasetype = arraytype->UU.stbasetype;
    mymemcpy(ename.str, blankstring, sizeof(stryngar));
    ename.len = 0;
    /* newe = */ (Void)insertedge(aelem, 1, n, 1, arraybasetype, ename);
    /* newe = */ (Void)insertedge(n, 1, arepl, 3, arraybasetype, ename);
    level--;
  }
  /* convert to AReplace node */
  removeedge(&levele);
  shiftinputports(n, 2, -1);
  n->ndcode = ifnareplace;
}  /* ConvertAReplaceN */

Local Void removeunusedports(seln)
node *seln;
{
  /* - SelN is a select node with K inputs and no input gaps.
     - This routine removed any input edges not used
       by any of the subgraphs of SelN.
     - Port Compaction is also done.
   */
  int lastgraph, grnum, count, eport;
  boolean used;
  port *e;
  int FORLIM;

  lastgraph = numbofsubgraphs(seln) - 1;
  eport = 1;
  FORLIM = largestinputportnumber(seln);
  for (count = 1; count <= FORLIM; count++) {
    e = getinputedge(seln, eport);
    used = false;
    for (grnum = 0; grnum <= lastgraph; grnum++) {
      if (getoutputedge(tochildgraph(seln, grnum), eport) != NULL)
	used = true;
    }
    if (!used) {
      removeedge(&e);
      shiftinputports(seln, eport, -1);
      for (grnum = 0; grnum <= lastgraph; grnum++)
	shiftoutputports(tochildgraph(seln, grnum), eport, -1);
    } else
      eport++;
  }
}  /* RemoveUnusedPorts */

Local node *createselectorgraph(n)
node *n;
{
  node *g;
  /* port *e;*/
  stryng ename;

  g = newnodealloc(ndgraph);
  univnodecnt++;
  g->ndid = univnodecnt;
  g->ndparent = n;
  g->ndcode = ifngraph;
  mymemcpy(ename.str, blankstring, sizeof(stryngar));
  ename.len = 0;
  /* e = */ (Void)insertedge(g, 1, g, 1, getbasictype(ifbinteger), ename);
  return g;
}  /* CreateSelectorGraph */

Local Void convertiftoselect(n)
node *n;
{
  /* Preconditions
        - N is an IfThenElse node with two subgraphs
        - Subgraph 0 is the True branch
        - Subgraph 1 is the False branch
    PostConditions
        - N is changed to a Select Node
        - An INT node has been added to change the predicate into an integer
        - A selector graph has been constructed with a direct edge.
        - The False branch becomes Subgraph 1
        - The True branch becomes Subgraph 2
  */
  port *pe /*, *e*/;
  stentry *ity, *pt;
  node *pn, *intn;
  int pp;
  stryng litvalue, pname;
  graph *gr;

  /* Insert INT node */
  intn = createsimplenode(ifnint);
  insertnode(n->ndparent, toprevnode(n), intn);
  /* Construct input edge to INT node */
  pe = getinputedge(n, 1);
  pname = pe->ptname;
  pt = pe->pttype;
  if (pe->ptsort == ptlit) {
    litvalue = pe->UU.ptlitvalue;
    /* e = */ (Void)insertliteral(intn, 1, pt, litvalue, pname);
  } else {
    pn = producernodeofedge(pe);
    pp = producerportnumber(pe);
    /* e = */ (Void)insertedge(pn, pp, intn, 1, pt, pname);
  }
  /* Shift input edges RIGHT one port */
  shiftinputports(n, 1, 1);
  shiftoutputports(tochildgraph(n, 0), 1, 1);
  shiftoutputports(tochildgraph(n, 1), 1, 1);
  /* Add output edge to INT node */
  ity = getbasictype(ifbinteger);
  mymemcpy(pname.str, blankstring, sizeof(stryngar));
  pname.len = 0;
  /* e = */ (Void)insertedge(intn, 1, n, 1, ity, pname);
  /* Change N to a SELECT node */
  n->ndcode = ifnselect;
  /* Swap True and False graphs */
  gr = n->UU.U2.ndsubsid;
  n->UU.U2.ndsubsid = gr->grnext;
  gr->grnext = NULL;
  n->UU.U2.ndsubsid->grnext = gr;
  /* Insert Selector Graph */
  gr = newgraphptr();
  gr->grnode = createselectorgraph(n);
  gr->grnext = n->UU.U2.ndsubsid;
  n->UU.U2.ndsubsid = gr;
  removeunusedports(n);
}  /* ConvertIfToSelect */

Local Void convertelseiftoselect(n)
node *n;
{
  /* In this case, N is an IfThenElse node with more than two subgraphs,
     the last three are (1) a predicate graph, (2) its true branch, and
     (3) the false branch.
     What we do here is convert the predicate graph into the false branch
     of the preceeding predicate by adding a select node to this graph
     and attaching the True and False branches to this new select node.
     Note that a selector graph must be added and an INT node added to
     convert the boolean predicate into an integer selector.  Note also
     that the True and False branches must be swapped in the ordering so
     that the Flase branch corresponds to the zero selector value.
   */
  int count, eport;
  stryng ename;
  stentry *etype;
  port *e /*, *newe*/;
  node *predicate, *intn, *seln;
  graph *gr, *gr1, *gr2, *pgr;
  int FORLIM;

  /* Find the last three subgraphs */
  count = numbofsubgraphs(n) - 2;
  pgr = n->UU.U2.ndsubsid;
  while (count > 1) {
    pgr = pgr->grnext;
    count--;
  }
  predicate = pgr->grnode;
  /* create the INT and SELECT nodes and wire them */
  intn = createsimplenode(ifnint);
  insertnode(predicate, tolastnodeingraph(predicate), intn);
  seln = createcompoundnode(ifnselect);
  insertnode(predicate, intn, seln);
  e = getinputedge(predicate, 1);
  changeedgedest(e, intn, 1);
  mymemcpy(ename.str, blankstring, sizeof(stryngar));
  ename.len = 0;
  /* newe = */ (Void)insertedge(intn, 1, seln, 1, getbasictype(ifbinteger), ename);
  e = n->ndilist;
  while (e != NULL) {
    etype = e->pttype;
    ename = e->ptname;
    eport = e->pttoport;
    /* newe = */ (Void)insertedge(predicate, eport, seln, eport + 1, etype, ename);
    e = e->pttonext;
  }
  FORLIM = largestoutputportnumber(n);
  for (eport = 1; eport <= FORLIM; eport++) {
    e = getoutputedge(n, eport);
    if (e != NULL) {
      etype = e->pttype;
      ename = e->ptname;
      /* newe = */ (Void)insertedge(seln, eport, predicate, eport, etype, ename);
    }
  }
  /* Add subgraphs to Select Node */
  seln->ndsort = ndcompound;
  gr = newgraphptr();
  gr->grnode = NULL;
  gr->grnext = NULL;
  seln->UU.U2.ndsubsid = gr;
  gr->grnode = createselectorgraph(seln);
  gr2 = pgr->grnext;   /* Last subgraph */
  gr1 = gr2->grnext;   /* Next to last subgraph */
  gr->grnext = gr1;
  gr1->grnext = gr2;
  gr2->grnext = NULL;
  pgr->grnext = NULL;
  gr1->grnode->ndparent = seln;
  shiftoutputports(gr1->grnode, 1, 1);
  gr2->grnode->ndparent = seln;
  shiftoutputports(gr2->grnode, 1, 1);
  removeunusedports(seln);
}  /* ConvertElseIfToSelect */

Local Void searchgraph_(g)
node *g;
{
  node *n;
  int grnum, FORLIM;

  n = g->ndnext;
  while (n != NULL) {
    /* depth first search */
    if (n->ndsort == ndcompound) {
      FORLIM = numbofsubgraphs(n);
      for (grnum = 0; grnum < FORLIM; grnum++)
	searchgraph_(tochildgraph(n, grnum));
      if (n->ndcode == ifnifthenelse) {
	/* Conver from right to left */
	while (numbofsubgraphs(n) > 2)
	  convertelseiftoselect(n);
	/* Now only two subgraphs exist */
	convertiftoselect(n);
      }
    } else {
      if (n->ndcode == ifnaelement) {
	while (largestinputportnumber(n) > 2)
	  splitaelement(n);
      } else if (n->ndcode == ifnareplacen)
	convertareplacen(n);
    }
    n = n->ndnext;
  }
}  /* SearchGraph */


Static Void converttoversion1()
{
  /* This routine converts IF1 version 1.1 to version 1.0 by
      - Translating IfThenElse nodes to nested Select nodes.
      - Translating multilevel AElement nodes to a string of single level
        AElement nodes.
      - Translating multilivel AReplaceN nodes to an equivalent combination
        of AElement - AReplace - AReplace nodes.
  */
  node *f;

  f = firstfunction;
  while (f != NULL) {
    searchgraph_(f);
    f = tonextfunction(f);
  }
}  /* ConvertToVersion1 */


typedef struct trec {
  node *node_;
  struct trec *next;
} trec;


/* Local variables for removegraphcse: */
struct LOC_removegraphcse {
  trec *table[ifmaxnode - firstatom + 1];
} ;

Local boolean nodesareequivalent(n1, n2, LINK)
node *n1, *n2;
struct LOC_removegraphcse *LINK;
{
  /* Pre:  (N1^.NDSort = NDAtomic) and (N2^.NDSort = NDAtomic) and OpCode(N1) = OpCode(N2)
     Post: NodesAreEquivalent := (Input edges of N1 and N2 are equivalent)
                     AND (corresponding output edges have the same types )
   */
  port *e1, *e2;
  stryng lit1, lit2;
  boolean sametypes, same;
  int port_, maxport;

  maxport = largestinputportnumber(n1);
  same = (maxport == largestinputportnumber(n2));
  port_ = 1;
  while (same && port_ <= maxport) {
    e1 = getinputedge(n1, port_);
    e2 = getinputedge(n2, port_);
    /* Check that the edges are equivalent */
    if (e1 != NULL || e2 != NULL) {
      if (e1 != NULL && e2 != NULL) {
	/* Both edges are not nil, check that they are equivalent */
	if (e1->ptsort == ptlit && e2->ptsort == ptlit) {
	  lit1 = e1->UU.ptlitvalue;
	  lit2 = e2->UU.ptlitvalue;
	  same = (e1->pttype == e2->pttype) & equalstrings(&lit1, &lit2);
	} else if (e1->ptsort == ptedge && e2->ptsort == ptedge)
	  same = (producernodeofedge(e1) == producernodeofedge(e2)) &
		 (producerportnumber(e1) == producerportnumber(e2));
	else
	  same = false;
      } else  /* One edge is nil and the other is not */
	same = false;
    }
    /* do nothing, go on to the next port */
    port_++;
  }  /* while same ... */
  if (same) {
    /* The inputs of the two nodes are identical, now we must
       check that the corresponding types of the output edges
       are the same */
    sametypes = true;
    port_ = largestoutputportnumber(n1);
    maxport = largestoutputportnumber(n2);
    if (port_ > maxport)
      maxport = port_;
    port_ = 1;
    while (port_ <= maxport && sametypes) {
      e1 = getoutputedge(n1, port_);
      e2 = getoutputedge(n2, port_);
      if (e1 != NULL && e2 != NULL)
	sametypes = (e1->pttype == e2->pttype);
      port_++;
    }
    return sametypes;
  } else
    return false;

  /* one is an edge and the other is a literal */
}  /* NodesAreEquivalent */

Local Void inittable(LINK)
struct LOC_removegraphcse *LINK;
{
  /* Think of the Table as a collection of sets, one for each
     simple opcode.  InitTable initiallizes all these sets to
     be empty.
   */
  unchar index;

  for (index = firstatom; index <= ifmaxnode; index++)
    LINK->table[index - firstatom] = NULL;
}  /* InitTable */

Local Void addtotable_(key, n, LINK)
unchar key;
node *n;
struct LOC_removegraphcse *LINK;
{
  /* Add node N to the set identified by Key */
  trec *t;

  t = (trec *)Malloc(sizeof(trec));
  t->node_ = n;
  t->next = LINK->table[key - firstatom];
  LINK->table[key - firstatom] = t;
}  /* AddToTable */

Local node *getequivnode(opcode, n, LINK)
unchar opcode;
node *n;
struct LOC_removegraphcse *LINK;
{
  /* Search the table for a node with the given OpCode and
     Matches N's input edges
     Return this equivalent node if one exists otherwise return nil.
   */
  boolean found;
  trec *t;
  node *checknode;

  found = false;
  t = LINK->table[opcode - firstatom];
  while (!found && t != NULL) {
    checknode = t->node_;
    if (nodesareequivalent(checknode, n, LINK))
      found = true;
    else
      t = t->next;
  }
  if (found)
    return checknode;
  else
    return NULL;
}  /* GetEquivNode */

Local Void movealloutputedges(fromnode, tonode, LINK)
node *fromnode, *tonode;
struct LOC_removegraphcse *LINK;
{
  /* move all the output edges of FromNode to corresponding
     output edges of ToNode.
     NOTE:  ChangeEdgeSrc( E, .. ) changes the "environment"
            of E and so we must save E in temp, then advance E
            and finally modify temp
   */
  port *e, *temp;
  int port_;

  e = fromnode->ndolist;
  while (e != NULL) {
    temp = e;
    e = e->UU.U1.ptfrnext;
    port_ = producerportnumber(temp);
    changeedgesrc(temp, tonode, port_);
  }
}  /* MoveAllOutputEdges */

Local Void moveoutputedges(n, fromport, toport)
node *n;
int fromport, toport;
{
  /* detach the output edges from port 'fromport' and reattach
     then to port 'toport'
   */
  port *e, *tmpe;

  e = getoutputedge(n, fromport);
  while (e != NULL) {
    /* NOTE: we must move off the edge we are moving
            BEFORE we move it! */
    tmpe = e;
    e = nextoutputedgesameport(e);
    changeedgesrc(tmpe, n, toport);
  }
}  /* MoveOutputEdges */

Local Void pushfanoutout(n, LINK)
node *n;
struct LOC_removegraphcse *LINK;
{
  /* If fanout was created from nodes inside the returns
     graph to the boundry of that edge, we wnat to push the
     fanout out of the subgraph and into the outside scope */
  /* The assumptions are:
     (1) N is a loop node (Forall, LoopA or LoopB)
     (2) cse has been run on it
  */
  node *retg, *srcnode, *cn;
  stryng litvalue, name;
  int litport, eport, srcport, removeport, cp;
  port *e, *lite, *ne, *srce, *removee;

  if (n->ndcode == ifnforall)
    retg = toforallreturnsgraph(n);
  else
    retg = toreturnsgraph(n);
  e = retg->ndilist;
  while (e != NULL) {
    if (e->ptsort == ptlit) {
      /* Push this Literal edge out of the returns
         graph and into the graph containing the
         compound node
       */
      lite = e;
      e = e->pttonext;
      litport = lite->pttoport;
      litvalue = lite->UU.ptlitvalue;
      ne = getoutputedge(n, litport);
      while (ne != NULL) {
	srce = ne;
	ne = nextoutputedgesameport(ne);
	cn = srce->pttonode;
	cp = srce->pttoport;
	name = srce->ptname;
	disconnectedgefromdest(srce);
	disconnectedgefromsource(srce);
	srce = insertliteral(cn, cp, srce->pttype, litvalue, name);
	/*  ChangeEdgeToLiteral( SrcE, LitValue );*/
      }
      removeedge(&lite);
      shiftinputports(retg, litport, -1);
      shiftoutputports(n, litport, -1);
      continue;
    }
    /* Replace fanout within the returns graph with fanout
       on the surrounding compound node.
       In this algorithm, all activity takes place to the RIGHT
       of edge E.  Edge E itself is never removed.
     */
    eport = e->pttoport;
    srcnode = producernodeofedge(e);
    srcport = producerportnumber(e);
    srce = getoutputedge(srcnode, srcport);
    while (srce != NULL) {
      if (srce == e || srce->pttonode != retg) {
	srce = nextoutputedgesameport(srce);
	continue;
      }
      removee = srce;
      srce = nextoutputedgesameport(srce);
      removeport = removee->pttoport;
      moveoutputedges(n, removeport, eport);
      removeedge(&removee);
      shiftinputports(retg, removeport, -1);
      shiftoutputports(n, removeport, -1);
    }
    e = e->pttonext;
  }
}  /* PushFanoutOut */


Static int removegraphcse(g, recurse)
node *g;
boolean recurse;
{
  /* Search this graph removing all common subexpressions involving
     simple nodes.
     NOTE:  Will not equate equivalent compound nodes.
            This can be done by checking for:
                1) Equivalent Opcodes
                2) Equivalent inputs
                3) Equivalent subgraphs.  We can assume subgraphs
                   have common subexpressions already merged and
                   hence all surviving nodes are in fact unique.
                   Walk the two graphs in the given order and
                   check that corresponding nodes are equivalent.
                   This will only find IDENTICAL graphs and not
                   equivalent graphs.  Finding EQUIVALENT graphs
                   is a bit more complicated.  It involves constructing
                   parallel lists, one for each opcode in each graph.
                   With each list, pull one node off a list at a time
                   and pair it with an equivalent node from its
                   parallel list.  The two subgraphs are equivalent
                   if the operation terminates normally.
     NOTE:  Will not equate node with identical but reversed
            inputs on a commutative operation node.
            The only commutative nodes are:  Plus, Times, Min, Max,
                Equal, NotEqual.
     NOTE:  Common subexpressions are not moved across graph boundaries.
   */
  struct LOC_removegraphcse V;
  node *tmpn, *eqnode, *n;
  boolean remove;
  int count, subgr;
  unchar opcode;
  int FORLIM;

  count = 0;
  inittable(&V);   /* Initialize all sets to be empty */
  n = g->ndnext;
  while (n != NULL) {   /* Walk the graph */
    remove = false;
    if (n->ndsort == ndatomic) {
      opcode = n->ndcode;
      eqnode = getequivnode(opcode, n, &V);
      if (eqnode == NULL) {
	/* No equivalent nodes found, add it to the set */
	addtotable_(opcode, n, &V);
      } else {
	count++;
	/* An equivalent node was found, flag this one to be removed */
	remove = true;
      }
    } else {  /* Recursively walk subgraphs of compound node */
      if (recurse) {
	FORLIM = numbofsubgraphs(n);
	for (subgr = 0; subgr < FORLIM; subgr++)
	  count += removegraphcse(tochildgraph(n, subgr), recurse);
      }
      if ((unsigned)n->ndcode < 32 &&
	  ((1L << n->ndcode) &
	   ((1L << ifnforall) | (1L << ifnloopb) | (1L << ifnloopa))) != 0)
	pushfanoutout(n, &V);
    }
    tmpn = n;
    n = n->ndnext;
    if (remove) {
      movealloutputedges(tmpn, eqnode, &V);
      /* Note, RemoveNode also removes all remaining input
              and output edges */
      removenode(tmpn);
    }
  }  /* while */
  return count;
}  /* RemoveGraphCSE */


Static boolean csemodule(module, verbose)
node *module;
boolean verbose;
{
  /* returns true iff the operation was successful */
  boolean Result;
  int count, funnum;
  node *fungraph;
  stryng csestring, funname;
  int FORLIM;

  /* Check Stamps Here */
  if (stampismissing('C')) {
    printf("ERROR: Not all input files have been Structure Checked\n");
    return false;
  }
  if (stampismissing('D')) {
    printf("ERROR: Not all input files have been Data Flow Ordered\n");
    return false;
  }
  FORLIM = numbofsubgraphs(module);
  for (funnum = 0; funnum < FORLIM; funnum++) {
    fungraph = tochildgraph(module, funnum);
    count = removegraphcse(fungraph, true);
    if (verbose) {
      funname = fungraph->UU.U1.ndlink->lkname;
      printf("In function ");
      writestring(stdout, &funname);
      printf(", CSE removed %d simple nodes.\n", count);
    }
  }
  Result = true;
  /* Add Stamps Here */
  string20(&csestring, " Common Subs Removed");
  addstamp('E', csestring);
  removestamp('O');
  return Result;
}  /* CSEModule */


typedef struct ndptrelmlist {
  node *elem;
  struct ndptrelmlist *next;   /*NDPtrelmlist*/
} ndptrelmlist;

typedef struct ndptrreclist {
  int length;
  ndptrelmlist *listhd, *listtl;
} ndptrreclist;


Local ndptrreclist *ndptrnewlist()
{
  ndptrreclist *templis;

  templis = (ndptrreclist *)Malloc(sizeof(ndptrreclist));
  templis->length = 0;
  templis->listhd = NULL;
  templis->listtl = NULL;
  return templis;
}

Local boolean ndptrisnull(lisptr)
ndptrreclist *lisptr;
{
  return (lisptr->length == 0);
}

Local node *ndptrrmhead(lisptr)
ndptrreclist *lisptr;
{
  node *Result;

  if (lisptr->listhd == NULL)
    return NULL;
  Result = lisptr->listhd->elem;
  lisptr->length--;
  if (lisptr->listhd == lisptr->listtl) {
    lisptr->listhd = NULL;
    lisptr->listtl = NULL;
  } else
    lisptr->listhd = lisptr->listhd->next;
  return Result;
}

Local int ndptrlength(lisptr)
ndptrreclist *lisptr;
{
  return (lisptr->length);
}

Local ndptrreclist *ndptrappend(lisptr, element)
ndptrreclist *lisptr;
node *element;
{
  ndptrelmlist *elmptr;

  elmptr = (ndptrelmlist *)Malloc(sizeof(ndptrelmlist));
  elmptr->elem = element;
  elmptr->next = NULL;
  if (lisptr->length == 0)
    lisptr->listhd = elmptr;
  else
    lisptr->listtl->next = elmptr;
  lisptr->listtl = elmptr;
  lisptr->length++;
  return lisptr;
}

Local Void imposegraphordering(neworder, g)
ndptrreclist *neworder;
node *g;
{
  /* ImposeGraphOrdering*/
  /*   Rearrange the links on the nodes in G to reflect the*/
  /*   ordering given in NewOrder*/
  /* Precondition:  length( NewOrder ) = length( G )   */
  /* written 84/3/8 by sks*/
  /* modified 85/1/13 by mlw*/
  int newlabel;
  node *thisnode, *previousnode;

  /* ImposeGraphOrdering */
  newlabel = 0;
  previousnode = g;
  while (!ndptrisnull(neworder)) {
    thisnode = ndptrrmhead(neworder);
    newlabel++;
    thisnode->ndlabel = newlabel;
    previousnode->ndnext = thisnode;
    previousnode = thisnode;
  }
  previousnode->ndnext = NULL;
}

Local int numancestors(n)
node *n;
{
  /* NumAncestors*/
  /*   Return the number of direct ancestor nodes*/
  /* written 84/9/11 by sks*/
  port *e;
  int result;
  node *prodn;

  /* NumAncestors */
  result = 0;
  e = n->nddepilist;
  if (e == NULL)
    e = n->ndilist;
  while (e != NULL) {
    if (e->ptsort != ptlit) {
      prodn = producernodeofedge(e);
      if (prodn->ndsort != ndgraph)
	result++;
    }
    e = e->pttonext;
  }
  return result;
}

/* Store the reference count of a node in the Misc field of the node */
Local Void setrefcount(n, count)
node *n;
int count;
{
  n->ndmisc.numb = count;
}  /* SetRefCount */

Local int refcount(n)
node *n;
{
  return (n->ndmisc.numb);
}  /* RefCount */

Local boolean dfordergraph(g)
node *g;
{
  /* DFOrderGraph*/
  /*   Rearrange nodes in a graph so that if node A depends*/
  /*   on outputs of node B, A will follow B*/
  /* written 84/2/22 by sks*/
  /* changed 84/9/11 by sks to use linear time algorithm*/
  /* modified 85/1/13 by mlw to check for cycles in graph*/
  /* modified 86/2/24 by mlw: Changed to a function to return a    */
  /*   status code (true if it worked ok, false if a problem).     */
  /*   Also, changed for seperate compilation of */
  ndptrreclist *newordering, *scheduled;
  int i, neworderlength, graphlength;
  node *n, *current, *consumer;
  boolean noerror;
  port *e;
  int FORLIM;

  /*DFOrderGraph*/
  noerror = true;
  if (g->ndnext == NULL) {
    return noerror;
  }  /* not an empty graph */
  scheduled = ndptrnewlist();
  newordering = ndptrnewlist();
  /* Set the reference count for each node to be the number*/
  /* of ancestors.  If none, place node on 'scheduled' list*/
  /* if we see a compound node, schedule within it now!*/
  current = g->ndnext;
  while (current != NULL) {
    if (current->ndsort == ndcompound) {
      FORLIM = numbofsubgraphs(current);
      for (i = 0; i < FORLIM; i++)
	noerror &= dfordergraph(tochildgraph(current, i));
    }
    setrefcount(current, numancestors(current));
    if (refcount(current) == 0)
      scheduled = ndptrappend(scheduled, current);
    current = current->ndnext;
  }
  /* Decrement reference counts of direct descendants of each node*/
  /* that has been scheduled.  When reference count reaches zero,*/
  /* move a node to the scheduled list.*/
  current = ndptrrmhead(scheduled);
  while (current != NULL) {
    newordering = ndptrappend(newordering, current);
    e = current->nddepolist;
    if (e == NULL)
      e = current->ndolist;
    while (e != NULL) {
      consumer = e->pttonode;
      if (consumer->ndsort != ndgraph) {
	setrefcount(consumer, refcount(consumer) - 1);
	if (refcount(consumer) == 0)
	  scheduled = ndptrappend(scheduled, consumer);
      }
      e = e->UU.U1.ptfrnext;
    }
    current = ndptrrmhead(scheduled);
  }
  neworderlength = ndptrlength(newordering);
  graphlength = numbnodesingraph(g);
  if (neworderlength < graphlength) {
    noerror = false;
    printf("ERROR: Not all graph nodes scheduled\n");
    printf("  ---  Check for cycles in graph\n");
    printf("  ---  Here is a list of unscheduled nodes:\n");
    n = g->ndnext;
    while (n != NULL) {
      if (refcount(n) != 0)
	printf("  -----      %.16sID = %5d    (Label = %d)\n",
	       nodename[n->ndcode], n->ndid, n->ndlabel);
      n = n->ndnext;
    }
    return noerror;
  }
  if (neworderlength <= graphlength) {
    imposegraphordering(newordering, g);
    return noerror;
  }
  noerror = false;
  printf("ERROR: More nodes scheduled than in graph\n");
  printf("  ---  Some nodes may be scheduled twice.\n");
  return noerror;
}

/* Local variables for fixreturnsgraph: */
struct LOC_fixreturnsgraph {
  node *loop, *geng, *bodyg, *retg, *initg, *testg, *loopparent;
  int k, l, t, port_;
  port *e;
  boolean isforall;
} ;

Local boolean allconstantinputs(n, LINK)
node *n;
struct LOC_fixreturnsgraph *LINK;
{
  /* Returns true if all inputs of N are either literals or come from
     K ports.
     -- N is a node in the returns graph of Loop which does not
        depend on any other node in that graph.
   */
  boolean constant;
  port *e;

  /* K : integer   -- Imported from outer scope */
  constant = true;
  e = n->ndilist;
  while (constant && e != NULL) {
    if (e->ptsort == ptedge) {
      if (producerportnumber(e) > LINK->k)
	constant = false;
    }
    e = e->pttonext;
  }
  return constant;
}  /* AllConstantInputs */

Local Void createnewkport(LINK)
struct LOC_fixreturnsgraph *LINK;
{
  /* Creates ONE new K port on the compound node Loop.
     -- Imports the following variables:
     -- IsForall : boolean;
     -- Loop, InitG, TestG, BodyG, RetG, GenG : NDPtr;
     -- var  K, L, T : integer
   */
  LINK->k++;
  LINK->l++;
  LINK->t++;
  if (LINK->isforall) {
    shiftinputports(LINK->geng, LINK->k, 1);
    shiftoutputports(LINK->bodyg, LINK->k, 1);
    shiftinputports(LINK->bodyg, LINK->k, 1);
    shiftoutputports(LINK->retg, LINK->k, 1);
    return;
  }
  shiftinputports(LINK->initg, LINK->k, 1);
  shiftoutputports(LINK->bodyg, LINK->k, 1);
  shiftinputports(LINK->bodyg, LINK->k, 1);
  shiftoutputports(LINK->testg, LINK->k, 1);
  shiftoutputports(LINK->retg, LINK->k, 1);
}  /* CreateNewKPort */

Local Void createnewlport(LINK)
struct LOC_fixreturnsgraph *LINK;
{
  /* Creates ONE new L port on the compound node Loop.
     -- Imports the following variables:
     -- IsForall : boolean;
     -- Loop, InitG, TestG, BodyG, RetG, GenG : NDPtr;
     -- var  L, T : integer
   */
  LINK->l++;
  LINK->t++;
  if (LINK->isforall) {
    shiftinputports(LINK->bodyg, LINK->l, 1);
    shiftoutputports(LINK->retg, LINK->l, 1);
    return;
  }
  shiftinputports(LINK->bodyg, LINK->l, 1);
  shiftoutputports(LINK->testg, LINK->l, 1);
  shiftoutputports(LINK->retg, LINK->l, 1);
}  /* CreateNewLPort */

Local Void insureedgeisnotconstant(n, p, LINK)
node *n;
int p;
struct LOC_fixreturnsgraph *LINK;
{
  /* The edge on input port P of node N is not supposed to be
     a literal or from a K port.  If it is, a new L or T port is
     added and this edge is redirected to that new port.
   */
  port *e /*, *newe*/;
  stryng ename;
  stentry *etype;
  int pp;
  node *pn;

  e = getinputedge(n, p);
  if (e == NULL) {
    return;
  }  /* E <> nil */
  mymemcpy(ename.str, blankstring, sizeof(stryngar));
  ename.len = 0;
  etype = e->pttype;
  if (etype->stsort == iftmultiple)
    etype = etype->UU.stbasetype;
  if (e->ptsort == ptlit) {
    if (LINK->isforall) {
      LINK->t++;
      changeedgedest(e, LINK->bodyg, LINK->t);
      /* newe = */ (Void)insertedge(LINK->retg, LINK->t, n, p, etype, ename);
      return;
    }
    createnewlport(LINK);
    changeedgedest(e, LINK->initg, LINK->l);
    /* newe = */ (Void)insertedge(LINK->bodyg, LINK->l, LINK->bodyg, LINK->l, etype,
		      ename);
    /* newe = */ (Void)insertedge(LINK->retg, LINK->l, n, p, etype, ename);
    return;
  }
  pn = producernodeofedge(e);
  pp = producerportnumber(e);
  if (pn != LINK->retg || pp > LINK->k)
    return;
  if (LINK->isforall) {  /* Should Come from a T port in the Body */
    LINK->t++;
    /* newe = */ (Void)insertedge(LINK->bodyg, pp, LINK->bodyg, LINK->t, etype, ename);
    changeedgesrc(e, LINK->retg, LINK->t);
    return;
  }
  createnewlport(LINK);
  /* newe = */ (Void)insertedge(LINK->initg, pp, LINK->initg, LINK->l, etype, ename);
  /* newe = */ (Void)insertedge(LINK->bodyg, LINK->l, LINK->bodyg, LINK->l, etype, ename);
  changeedgesrc(e, LINK->retg, LINK->l);

  /* E is an edge */
  /* Should come from an L port */
}  /* InsureEdgeIsNotConstant */

Local Void moveoutsideofloop(n, LINK)
node *n;
struct LOC_fixreturnsgraph *LINK;
{
  /* -- N is a node in the returns graph that is to be moved outside
       the loop.  Its inputs are either literals for edges directly from
       K ports.  A copy of the node is make outside and a new K port
       and edges are created for each wired output port of N
  */
  port *le, *e, /* *newe,*/ *tmpe;
  node *newn, *pn;
  int pp, cp;
  stryng ename, litvalue;
  stentry *etype;
  int FORLIM;

  /* Imported from outside scope . . .
     var K : integer;
     LoopParent, Loop, RetG : NDPtr;
   */
  newn = copynode(n);
  insertnode(LINK->loopparent, toprevnode(LINK->loop), newn);
  /* Add input edges */
  e = n->ndilist;
  while (e != NULL) {
    cp = e->pttoport;
    if (e->ptsort == ptlit) {
      ename = e->ptname;
      litvalue = e->UU.ptlitvalue;
      etype = e->pttype;
      if (etype->stsort == iftmultiple)
	etype = etype->UU.stbasetype;
      /* newe = */(Void)insertliteral(newn, cp, etype, litvalue, ename);
    } else {
      pp = producerportnumber(e);
      le = getinputedge(LINK->loop, pp);
      ename = le->ptname;
      etype = le->pttype;
      if (etype->stsort == iftmultiple)
	etype = etype->UU.stbasetype;
      if (le->ptsort == ptlit) {
	litvalue = le->UU.ptlitvalue;
	/* newe = */(Void)insertliteral(newn, cp, etype, litvalue, ename);
      } else {
	pn = producernodeofedge(le);
	pp = producerportnumber(le);
	/* newe = */(Void)insertedge(pn, pp, newn, cp, etype, ename);
      }
    }
    e = e->pttonext;
  }  /* Add input edges to NewN */
  FORLIM = largestoutputportnumber(n);
  /* Create a new K port for each used output port of node N */
  for (pp = 1; pp <= FORLIM; pp++) {
    e = getoutputedge(n, pp);
    if (e != NULL) {
      createnewkport(LINK);
      etype = e->pttype;
      if (etype->stsort == iftmultiple)
	etype = etype->UU.stbasetype;
      ename = e->ptname;
      /* newe = */(Void)insertedge(newn, pp, LINK->loop, LINK->k, etype, ename);
      do {
	tmpe = e;
	e = nextoutputedgesameport(e);
	changeedgesrc(tmpe, LINK->retg, LINK->k);
      } while (e != NULL);
    }
  }
}  /* MoveOutsideOfLoop */

Local Void copynodetograph(n, g, startport, LINK)
node *n, *g;
int startport;
struct LOC_fixreturnsgraph *LINK;
{
  /* -- N is a node in the returns graph.
     -- All input edges of N are Literals or from K or L ports.
     -- Make a copy of N and install it as the last node in graph G.
     -- Graph ports, starting at number StartPort, have been allocated
        and reserved for the outputs of the copy of N.  These edges
        are installed here, but the output edges of node N itself
        are left unchanged.
   */
  node *prev, *newn, *pn;
  int pp, cp;
  port *ge, *e /*, *newe*/;
  stentry *etype;
  stryng ename, litvalue;
  int FORLIM;

  prev = tolastnodeingraph(g);
  newn = copynode(n);
  insertnode(g, prev, newn);
  /* Add input edges */
  e = n->ndilist;
  while (e != NULL) {
    cp = e->pttoport;
    etype = e->pttype;
    if (etype->stsort == iftmultiple)
      etype = etype->UU.stbasetype;
    ename = e->ptname;
    if (e->ptsort == ptlit) {
      litvalue = e->UU.ptlitvalue;
      /* newe = */ (Void)insertliteral(newn, cp, etype, litvalue, ename);
    } else {  /* E is an edge, trace it back to graph G */
      pp = producerportnumber(e);
      ge = getinputedge(g, pp);
      if (ge == NULL)   /* create one */
	/* newe = */ (Void)insertedge(g, pp, newn, cp, etype, ename);
      else if (ge->ptsort == ptedge) {
	pp = producerportnumber(ge);
	pn = producernodeofedge(ge);
	/* newe = */ (Void)insertedge(pn, pp, newn, cp, etype, ename);
      } else {
	litvalue = ge->UU.ptlitvalue;
	/* newe = */ (Void)insertliteral(newn, cp, etype, litvalue, ename);
      }
    }
    e = e->pttonext;
  }
  FORLIM = largestoutputportnumber(n);
  /* Add Output Edges, assume ports have already been created */
  for (LINK->port_ = 1; LINK->port_ <= FORLIM; LINK->port_++) {
    e = getoutputedge(n, LINK->port_);
    if (e != NULL) {
      ename = e->ptname;
      etype = e->pttype;
      if (etype->stsort == iftmultiple)
	etype = etype->UU.stbasetype;
      /* newe = */ (Void)insertedge(newn, LINK->port_, g, startport, etype, ename);
      startport++;
    }
  }

  /* GE is a literal edge */
}  /* CopyNodeToGraph */

Local Void movenodeafterloop(node_, fvnode, loop, LINK)
node *node_, *fvnode, *loop;
struct LOC_fixreturnsgraph *LINK;
{
  /* -- Node is a node in the returns graph.
     -- FVNode is the final value node that an input edge at port 1 which
        which comes from the above Node.
     -- Loop is the for loop we are working in.
     Make a copy of Node and place it below the loop.  Connect each of its
     input edges as follows:
       if the edge is a literal
         then just connect it
       else if it comes from a KPort in the returns graph
         then make a connection from the value outside the loop
       else
         create a Final Value node with in the returns graph
         connect the value that used to go into Node to port 1 of the
         Final Value node.  If FVNode has a masking value, connect it to
         the new Final Value node.  Create an edge from the Final Value to
         the returns subgraph.  Create an edge from the loop to the copied
         node outside the loop.
      Finally create an edge(s) from the output of the newly created node
      to where ever the output of FVNode used to go (this edge will come
      out of the loop node);*/
  node *newn, *newfv;
  port *nextedgetomove, *nexte, /* *newe,*/ *tempe;
  stentry *etype;
  stryng valuestr, namestr;
  int eoldinputport;

  newn = copynode(node_);
  /* Place node after loop */
  insertnode(loop->ndparent, loop, newn);
  /* Connect input edges */
  LINK->e = node_->ndilist;
  while (LINK->e != NULL) {
    eoldinputport = LINK->e->pttoport;
    nextedgetomove = LINK->e->pttonext;
    if (LINK->e->pttype->stsort == iftmultiple)
      etype = LINK->e->pttype->UU.stbasetype;
    else
      etype = LINK->e->pttype;
    if (((producernodeofedge(LINK->e) == LINK->retg) &
	 (producerportnumber(LINK->e) <= LINK->k)) ||
	LINK->e->ptsort == ptlit)
    {  /*connect edge from outside of loop*/
      if (LINK->e->ptsort == ptlit) {
	valuestr = LINK->e->UU.ptlitvalue;
	namestr = LINK->e->ptname;
	/* newe = */ (Void)insertliteral(newn, LINK->e->pttoport, etype, valuestr,
			     namestr);
      } else {  /*else its coming from a K Port */
	tempe = getinputedge(loop, producerportnumber(LINK->e));
	namestr = tempe->ptname;
	if (tempe->ptsort == ptlit) {
	  valuestr = tempe->UU.ptlitvalue;
	  /* newe = */ (Void)insertliteral(newn, LINK->e->pttoport, etype, valuestr,
			       namestr);
	}  /*then*/
	else
	  /* newe = */ (Void)insertedge(producernodeofedge(tempe),
			    producerportnumber(tempe), newn,
			    LINK->e->pttoport, etype, namestr);
      }  /*else*/
    }  /*then*/
    else {  /* else input isn't a literal or imported into loop */
      newfv = createsimplenode(ifnfinalvalue);
      insertnode(LINK->retg, tolastnodeingraph(LINK->retg), newfv);
      changeedgedest(LINK->e, newfv, 1);
      tempe = getinputedge(fvnode, 2);
      namestr = LINK->e->ptname;
      if (tempe != NULL)   /*FV has a mask, must add it to newFV */
	tempe = insertedge(producernodeofedge(tempe),
			   producerportnumber(tempe), newfv, 2, tempe->pttype,
			   namestr);
      /* Connect NewFV to RetG */
      tempe = insertedge(newfv, 1, LINK->retg,
			 largestinputportnumber(LINK->retg) + 1, etype,
			 namestr);
      /* Connect output edge to NewN */
      mymemcpy(namestr.str, blankstring, sizeof(stryngar));
      namestr.len = 0;
      tempe = insertedge(loop, largestinputportnumber(LINK->retg), newn,
			 eoldinputport, etype, namestr);
    }  /*else*/
    LINK->e = nextedgetomove;
  }  /*while*/
  tempe = getoutputedge(fvnode, 1);
  if (tempe == NULL)
    printf("Error in fix returns graph!\n");
  else {
    tempe = getoutputedge(loop, tempe->pttoport);
    while (tempe != NULL) {
      nexte = nextoutputedgesameport(tempe);
      changeedgesrc(tempe, newn, producerportnumber(getinputedge(fvnode, 1)));
      tempe = nexte;
    }  /*while*/
  }  /*else*/
  /* chop off connection from Node to FVNode */
  tempe = getinputedge(fvnode, 1);
  removeedge(&tempe);
}  /* MoveNodeAfterLoop */

Local Void removefinalvalue(n, loop, LINK)
node *n, *loop;
struct LOC_fixreturnsgraph *LINK;
{
  int pp, foroutport;
  port *fvinedge, *fvoutedge, *foroutedge, *loopinedge, /* *newe,*/ *tempe;
  boolean literal;
  stryng ename, eval;

  fvinedge = getinputedge(n, 1);
  pp = producerportnumber(fvinedge);
  literal = (fvinedge->ptsort == ptlit);
  if (!literal)
    loopinedge = getinputedge(loop, pp);
  fvoutedge = n->ndolist;
  while (fvoutedge != NULL) {
    foroutport = fvoutedge->pttoport;
    foroutedge = getoutputedge(loop, foroutport);
    while (foroutedge != NULL) {
      if (literal) {
	ename = fvinedge->ptname;
	eval = fvinedge->UU.ptlitvalue;
	/* newe = */ (Void)insertliteral(foroutedge->pttonode, foroutedge->pttoport,
			     fvinedge->pttype, eval, ename);
      } else {
	ename = loopinedge->ptname;
	/* newe = */ (Void)insertedge(producernodeofedge(loopinedge),
			  producerportnumber(loopinedge),
			  foroutedge->pttonode, foroutedge->pttoport,
			  loopinedge->pttype, ename);
      }
      disconnectedgefromdest(foroutedge);
      tempe = foroutedge;
      foroutedge = nextoutputedgesameport(tempe);
      disconnectedgefromsource(tempe);
    }  /*while*/
    disconnectedgefromdest(fvoutedge);
    tempe = fvoutedge;
    fvoutedge = nextoutputedgesameport(tempe);
    disconnectedgefromsource(tempe);
  }  /*while*/
}  /*RemoveFinalValue*/

Local Void fixreturnsgraph(loop_)
node *loop_;
{
  /*  -- Loop is either a FORALL, LOOPA or LOOPB compound node.
      -- All the subgraphs of Loop must be complete and must have
         been ordered by data dependence.
      -- Loop is contained in a graph G that must be complete above this
         compound node (Parent pointers are set, Next fields are set,
         input edges of Loop are defined).
      This routine will walk the returns graph, transplanting nodes that
      really don't belong there to either the outer graph G or the other
      subgraphs.  After the walk:
      -- The returns graph will only contain nodes dealing with multiple
         values.
      -- The port numbers of the subgraphs may be dramatically altered
         but will be consistant with the new graph organization.
   */
  struct LOC_fixreturnsgraph V;
  node *n, *pn, *rn;
  int oports, startport /* , cse */;
  port *tmpe;
  stentry *etype;
  boolean ok;
  long legalreturnsnodes[ifmaxnode / 32 + 2];
  int FORLIM;

  V.loop = loop_;
  P_addset(P_expset(legalreturnsnodes, 0L), ifnfirstvalue);
  P_addset(legalreturnsnodes, ifnfinalvalue);
  P_addset(legalreturnsnodes, ifnagather);
  P_addset(legalreturnsnodes, ifnallbutlastvalue);
  P_addset(legalreturnsnodes, ifnreduce);
  P_addset(legalreturnsnodes, ifnredleft);
  P_addset(legalreturnsnodes, ifnredright);
  P_addset(legalreturnsnodes, ifnredtree);
  P_addset(legalreturnsnodes, ifnagatherat);
  P_addset(legalreturnsnodes, ifnreduceat);
  P_addset(legalreturnsnodes, ifnfinalvalueat);
  V.loopparent = V.loop->ndparent;
  V.k = largestinputportnumber(V.loop);
  V.isforall = (V.loop->ndcode == ifnforall);
  if (V.isforall) {
    V.geng = toforallgeneratorgraph(V.loop);
    V.bodyg = toforallbodygraph(V.loop);
    V.retg = toforallreturnsgraph(V.loop);
    V.l = largestinputportnumber(V.geng);
    if (V.l == 0)
      V.l = V.k;
  } else {
    V.initg = toinitgraph(V.loop);
    V.testg = totestgraph(V.loop);
    V.bodyg = tobodygraph(V.loop);
    V.retg = toreturnsgraph(V.loop);
    V.l = largestinputportnumber(V.initg);
    if (V.l == 0)
      V.l = V.k;
  }
  V.t = largestinputportnumber(V.bodyg);
  if (V.t == 0)
    V.t = V.l;
  /* cse = */ (Void)removegraphcse(V.retg, false);
  rn = V.retg->ndnext;
  while (rn != NULL) {
    n = rn;
    if (n->ndcode == ifnfinalvalue) {
      V.e = getinputedge(n, 1);
      if (V.e->ptsort != ptlit) {
	pn = producernodeofedge(V.e);
	tmpe = pn->ndolist;
	if (tmpe->UU.U1.ptfrnext == NULL && pn != V.retg &&
	    pn->ndcode != ifnallbutlastvalue)
	  movenodeafterloop(pn, n, V.loop, &V);
      }  /*then*/
    }  /*then*/
    rn = rn->ndnext;
  }  /*while*/
  cleanupgraph(V.loop->ndparent);
  if (V.loop->ndparent == NULL)
  {   /* make sure loop was not removed by CleanUpGraph*/
    return;
  }  /*then*/
  V.k = largestinputportnumber(V.loop);
  if (V.isforall) {
    V.l = largestinputportnumber(V.geng);
    if (V.l == 0)
      V.l = V.k;
  } else {
    V.l = largestinputportnumber(V.initg);
    if (V.l == 0)
      V.l = V.k;
  }
  V.t = largestinputportnumber(V.bodyg);
  if (V.t == 0)
    V.t = V.l;
  rn = V.retg->ndnext;
  while (rn != NULL) {
    n = rn;
    rn = rn->ndnext;
    if (P_inset(n->ndcode, legalreturnsnodes)) {
      switch (n->ndcode) {

      case ifnfinalvalue:
	pn = producernodeofedge(getinputedge(n, 1));
	if (pn == NULL)
	  ok = true;
	else if (pn->ndsort == ndgraph)
	  ok = true;
	else
	  ok = false;
	if ((allconstantinputs(n, &V) & (getinputedge(n, 2) == NULL)) && ok)
	  removefinalvalue(n, V.loop, &V);
	else {
	  insureedgeisnotconstant(n, 1, &V);
	  insureedgeisnotconstant(n, 2, &V);
	}
	break;

      case ifnfirstvalue:
      case ifnallbutlastvalue:
      case ifnfinalvalueat:
	insureedgeisnotconstant(n, 1, &V);
	insureedgeisnotconstant(n, 2, &V);
	break;
	/* FirstValue */

      case ifnagather:
      case ifnagatherat:
	insureedgeisnotconstant(n, 2, &V);
	insureedgeisnotconstant(n, 3, &V);
	break;
	/* AGather */

      case ifnreduce:
      case ifnredleft:
      case ifnredright:
      case ifnredtree:
      case ifnreduceat:
	insureedgeisnotconstant(n, 3, &V);
	insureedgeisnotconstant(n, 4, &V);
	break;
	/* Reduce */
      }/* case */
      continue;
    }
    if (allconstantinputs(n, &V))
      moveoutsideofloop(n, &V);
    else {
      oports = numberofwiredoutputports(n);
      if (V.isforall) {
	/* Create ports for output edges of N */
	startport = V.t + 1;
	V.t += oports;
	copynodetograph(n, V.bodyg, startport, &V);
      } else {  /* Must be an iterative loop */
	/* Create ports for output edges of N */
	startport = V.l + 1;
	shiftoutputports(V.testg, startport, oports);
	shiftinputports(V.bodyg, startport, oports);
	V.l += oports;
	V.t += oports;
	copynodetograph(n, V.bodyg, startport, &V);
	copynodetograph(n, V.initg, startport, &V);
      }
      FORLIM = largestoutputportnumber(n);
      /* Move output ports of N to RetG */
      for (V.port_ = 1; V.port_ <= FORLIM; V.port_++) {
	V.e = getoutputedge(n, V.port_);
	if (V.e != NULL) {
	  do {
	    tmpe = V.e;
	    V.e = nextoutputedgesameport(V.e);
	    changeedgesrc(tmpe, V.retg, startport);
	  } while (V.e != NULL);
	  startport++;
	}
      }
    }
    removenode(n);
  }  /* while RN */
  compactports(V.loop);
  /* Make sure all L and T port edges in RetG are multiples */
  V.k = largestinputportnumber(V.loop);
  V.e = outputedgegeport(V.retg, V.k + 1);
  while (V.e != NULL) {
    etype = V.e->pttype;
    if (etype->stsort != iftmultiple && etype->stsort != iftbuffer) {
      etype = getconstructortype(iftmultiple, etype);
      V.e->pttype = etype;
    }
    V.e = V.e->UU.U1.ptfrnext;
  }
  /* Change the type of the output edges of AllButLastValue
     nodes to a Multiple */
  n = V.retg->ndnext;
  while (n != NULL) {
    if (n->ndcode == ifnallbutlastvalue) {
      V.e = n->ndolist;
      while (V.e != NULL) {
	etype = V.e->pttype;
	if (etype->stsort != iftmultiple) {
	  etype = getconstructortype(iftmultiple, etype);
	  V.e->pttype = etype;
	}
	V.e = V.e->UU.U1.ptfrnext;
      }
    }
    n = n->ndnext;
  }

  /* Must move N out of RetG */
  /* Move N out of RetG */
}  /* FixReturnsGraph */

Local Void findloopnodes(g)
node *g;
{
  node *n;
  int grnum, FORLIM;

  n = g->ndnext;
  while (n != NULL) {
    if (n->ndsort == ndcompound) {
      FORLIM = numbofsubgraphs(n);
      for (grnum = 0; grnum < FORLIM; grnum++)
	findloopnodes(tochildgraph(n, grnum));
      switch (n->ndcode) {

      case ifnforall:
      case ifnloopa:
      case ifnloopb:
	fixreturnsgraph(n);
	break;

      case ifnselect:
      case ifntagcase:
      case ifnifthenelse:
      case ifniter:
	/* blank case */
	break;
      }/* case */
    }
    n = n->ndnext;
  }
}  /* FindLoopNodes */


Static boolean dforder(module)
node *module;
{
  boolean Result;
  node *g;
  int i;
  stryng orderstring;
  boolean ok, noerror;
  int FORLIM;

  /* DFOrder */
  noerror = true;
  FORLIM = numbofsubgraphs(module);
  for (i = 0; i < FORLIM; i++) {
    g = tochildgraph(module, i);
    ok = dfordergraph(g);
    if (ok)
      findloopnodes(g);
    noerror = (noerror && ok);
  }  /* for */
  removedeadcode();
  Result = noerror;
  if (!noerror)
    return Result;
  string20(&orderstring, " Nodes are DFOrdered");
  addstamp('D', orderstring);
  /* Remove the offsetter stamp */
  removestamp('O');
  return Result;
}


/* Local variables for findloopconstants: */
struct LOC_findloopconstants {
  node *n, *initg, *testg, *bodyg, *retg;
  int port_;
  port *inite;
} ;

Local Void turnintoliteral(LINK)
struct LOC_findloopconstants *LINK;
{
  /* VALUES IMPORTED FROM OUTSIDE SCOPE:
        port : integer;
        InitE, InitG, TestG, BodyG, RetG : NDPtr;
    PRECONDITIONS:
       - InitE is a literal on an L port in the init graph.
       - Edges corresponding to "port" are coded as loop values but
         are never given redefinitions.
       - The value associated with this kport is NOT used in the
         returns graph.
       - The Origional definition of this loop value is a Literal.
    POSTCONDITIONS:
       - The Literal defining this value in the INIT graph is removed.
       - All uses of this loop value are turned into Literals.
       - Port numbers are shifted left to implement the removal of
         a loop value.
  */
  stryng litvalue, name;
  int gr, cp;
  port *e, *tmpe;
  node *g, *cn;

  litvalue = LINK->inite->UU.ptlitvalue;
  removeedge(&LINK->inite);
  shiftinputports(LINK->initg, LINK->port_, -1);
  for (gr = 1; gr <= 3; gr++) {
    g = tochildgraph(LINK->n, gr);
    e = getoutputedge(g, LINK->port_);
    while (e != NULL) {
      tmpe = e;
      e = nextoutputedgesameport(e);
      cn = tmpe->pttonode;
      cp = tmpe->pttoport;
      name = tmpe->ptname;
      disconnectedgefromdest(tmpe);
      disconnectedgefromsource(tmpe);
      tmpe = insertliteral(cn, cp, tmpe->pttype, litvalue, name);
      /*      ChangeEdgeToLiteral( TmpE, LitValue )*/
    }
  }
  shiftoutputports(LINK->testg, LINK->port_, -1);
  shiftoutputports(LINK->bodyg, LINK->port_, -1);
  shiftinputports(LINK->bodyg, LINK->port_, -1);
  shiftoutputports(LINK->retg, LINK->port_, -1);
}  /* TurnIntoLiteral */

Local Void moveoutputports(n, fromport, toport, LINK)
node *n;
int fromport, toport;
struct LOC_findloopconstants *LINK;
{
  /* Move all output edges on port FromPort to port ToPort */
  port *e, *tmpe;

  e = getoutputedge(n, fromport);
  while (e != NULL) {
    tmpe = e;
    e = nextoutputedgesameport(e);
    changeedgesrc(tmpe, n, toport);
  }
}  /* MoveOutputPorts */

Local Void removefinalvalue_(n, loop, initg, LINK)
node *n, *loop, *initg;
struct LOC_findloopconstants *LINK;
{
  int pp, foroutport;
  port *fvoutedge, *foroutedge, /* *newe,*/ *tempe, *retvale;
  boolean literal;
  stryng ename, eval;

  literal = false;
  tempe = getinputedge(n, 1);
  pp = producerportnumber(tempe);
  if (tempe->ptsort == ptlit) {
    retvale = tempe;
    literal = true;
  }  /*then*/
  if (!literal) {
    tempe = getinputedge(initg, pp);
    if (tempe->ptsort == ptlit) {
      retvale = tempe;
      literal = true;
    }  /*then*/
    else {
      pp = producerportnumber(tempe);
      retvale = getinputedge(loop, pp);
    }  /*else*/
  }  /*then*/
  fvoutedge = n->ndolist;
  while (fvoutedge != NULL) {
    foroutport = fvoutedge->pttoport;
    foroutedge = getoutputedge(loop, foroutport);
    ename = retvale->ptname;
    while (foroutedge != NULL) {
      if (literal) {
	eval = retvale->UU.ptlitvalue;
	/* newe = */ (Void)insertliteral(foroutedge->pttonode, foroutedge->pttoport,
			     retvale->pttype, eval, ename);
      } else
	/* newe = */ (Void)insertedge(producernodeofedge(retvale),
			  producerportnumber(retvale), foroutedge->pttonode,
			  foroutedge->pttoport, retvale->pttype, ename);
      disconnectedgefromdest(foroutedge);
      tempe = foroutedge;
      foroutedge = nextoutputedgesameport(tempe);
      disconnectedgefromsource(tempe);
    }  /*while*/
    disconnectedgefromdest(fvoutedge);
    tempe = fvoutedge;
    fvoutedge = nextoutputedgesameport(tempe);
    disconnectedgefromsource(tempe);
  }  /*while*/
}  /*RemoveFinalValue*/


Static Void findloopconstants(n_)
node *n_;
{
  /* ASSUMPTIONS:
      - N is an IFNLoopB or IFNLoopA node
      - Loop Invariants have been removed (including from the INIT graph)
      - NoOp nodes have been removed
      - Expressions have been moved out of the RETURNS graph
    A LOOP CONSTANT IS:
      - coded as a loop value (coming from an L port)
      - Not given a new value in the BODY graph
      - Not used in the RETURNS graph
    THIS ROUTINE WILL:
      - Find all loop constants as defined above
      - If one is found, and is a Literal, then all edges representing
        this value will be converted to Literals
      - If one is found and is an edge (given a definition in the INIT graph,
        which is a direct edge from a K port) then that loop value will
        be removed and all references to it will be replaced by the
        corresponding K port from which it got its value.
  */
  struct LOC_findloopconstants V;
  stryng ename;
  stentry *etype;
  boolean konst;
  node *connode;
  int kport, k, l;
  port *e, *rete;

  V.n = n_;
  V.initg = toinitgraph(V.n);
  V.testg = totestgraph(V.n);
  V.bodyg = tobodygraph(V.n);
  V.retg = toreturnsgraph(V.n);
  k = largestinputportnumber(V.n);
  l = largestinputportnumber(V.initg);
  if (l == 0)
    l = k;
  V.port_ = k + 1;
  while (V.port_ <= l) {
    e = getinputedge(V.bodyg, V.port_);
    V.inite = getinputedge(V.initg, V.port_);
    if (e == NULL) {
      /* there should be a direct edge here */
      etype = V.inite->pttype;
      ename = V.inite->ptname;
      e = insertedge(V.bodyg, V.port_, V.bodyg, V.port_, etype, ename);
    }
    konst = false;
    if (e->ptsort == ptedge) {
      if ((getoutputedge(V.retg, V.port_) == NULL) & (producernodeofedge(e) ==
	    V.bodyg) & (producerportnumber(e) == V.port_))
	konst = true;
      else {
	rete = getoutputedge(V.retg, V.port_);
	if (rete != NULL) {
	  connode = rete->pttonode;
	  if ((connode->ndcode == ifnfinalvalue) &
	      (getinputedge(connode, 3) == NULL) & (producernodeofedge(e) ==
		V.bodyg) & (producerportnumber(e) == V.port_))
	    removefinalvalue_(connode, V.n, V.initg, &V);
	}  /*then*/
      }  /*else*/
    }
    if (!konst) {   /*move onto next l-port*/
      V.port_++;
      continue;
    }
    /* the edges corresponding to "port" represent
       loop constants, but are coded as loop values */
    removeedge(&e);
    V.inite = getinputedge(V.initg, V.port_);
    if (V.inite->ptsort == ptlit) {
      turnintoliteral(&V);
      l--;
      continue;
    }
    /* Turn this Loop Value into a Constant */
    kport = producerportnumber(V.inite);
    removeedge(&V.inite);
    shiftinputports(V.initg, V.port_, -1);
    moveoutputports(V.testg, V.port_, kport, &V);
    shiftoutputports(V.testg, V.port_, -1);
    moveoutputports(V.bodyg, V.port_, kport, &V);
    shiftoutputports(V.bodyg, V.port_, -1);
    shiftinputports(V.bodyg, V.port_, -1);
    moveoutputports(V.retg, V.port_, kport, &V);
    shiftoutputports(V.retg, V.port_, -1);
    l--;
    /*this moves us onto the next l*/
  }
}  /* FindLoopConstants */


/*  This code calls the routines that do the processing that used to be done
    separatly in if1prepare.*/
Static boolean backend()
{  /* Back End */
  boolean ok;
  stryng checkstring;

  ok = true;
  /* Add Stamp for IF1check */
  string20(&checkstring, " Faked IF1CHECK     ");
  addstamp('C', checkstring);
  if (!dforder(module)) {
    ok = false;
    printf("******** graph couldn't be ordered ********\n");
  }
  converttoversion1();
  removedeadcode();
  return ok;
}  /* Back End */


main(argc, argv)
int argc;
Char *argv[];
{
  register int i;                                /* NEW CANN */
  stryng txt;
  for ( i = 0; i < argc; i++ ) {                 /* NEW CANN */
    if ( strcmp( argv[i], "-SISglue" ) == 0 )   /* NEW CANN */
      CANN_GlueMode = 1;                        /* NEW CANN */
    else if ( argv[i][1] == 'F' ) /* NEW CANN */
      CANN_file_name = &(argv[i][2]); /* NEW CANN */
    }                                            /* NEW CANN */

  PASCAL_MAIN(argc, argv);
  /*llparse*/
  listfile = NULL;
  reservedwords = NULL;
  inputfile = NULL;
  etableout = NULL;
  ptableout = NULL;
  diag = NULL;
  source = NULL;
  printf(" LL Parse");
  printf(", using binary files\n");
  paramlist = NULL;
  xrefparam = defineparameter(&paramlist, "XREF      ", "X         ", flagpar,
			      13, 1);
  directoryparam = defineparameter(&paramlist, "DIRECTORY ", "DIR       ",
				   strpar, 14, 1);
  debugparam = defineparameter(&paramlist, "DEBUG     ", "D         ",
			       flagpar, 15, 1);
  listingparam = defineparameter(&paramlist, "LISTING   ", "L         ",
				 flagpar, 16, 1);
  pass1param = defineparameter(&paramlist, "PREPRO    ", "PREPRO    ",
			       flagpar, 17, 1);
  listfparam = defineparameter(&paramlist, "ListFile  ", "LF        ", strpar,
			       18, 1);
  readcommandline(infile, &outfile, &paramlist);
  listing = (listingparam->parvalue.len > 0 || listfparam->parvalue.len > 0);
  string20(&defaultdirectoryname, "/usr/local/sisal    ");
  askordefault(directoryparam, &defaultdirectoryname, false,
	       "                    ");
  directoryname = directoryparam->parvalue;
  xref = (xrefparam->parvalue.len > 0);
  if (xref)
    listing = true;
  if (listing) {
    if (listfparam->parvalue.len > 0)
      listfilename = listfparam->parvalue;
    else
      listfilename = outfile->parvalue;
    if (!openlistfile(listfilename, &listfile)) {
      listing = false;
      if (xref) {
	printf("No Cross Reference will be generated, can not open listing file.\n");
	xref = false;
      }
    }
  }
  if (infile[0]->parvalue.len > 0) {

{ /* NEW CANN 2/92 */
  char *Ptr;
  int   Idx;

  if ( CANN_file_name != NULL ) {
    txt.len = 0;
    for ( Idx = 1, Ptr = CANN_file_name; *Ptr != '\0'; Ptr++, Idx++ ) {
      concatchar(&txt, *Ptr );
      }
    }
} /* NEW CANN 2/92 */

    filename = infile[0]->parvalue;

    if ( CANN_file_name == NULL ) /* NEW CANN 2/92 */
      txt = filename;             /* NEW CANN 2/92 */

    printf("* Reading file: ");
    writestring(stdout, &txt);
    printf("...\n\n");
    if (listing) {
      fprintf(listfile, "* Reading file: ");
      writestring(listfile, &txt);
      fprintf(listfile, "...\n\n");
    }
  }
  if (pass1param->parvalue.len > 0) {
    passerr = pass1(&txt,&filename, stdout);
    if (passerr) {
      printf("Compilation terminated, input file did not exist\n");
      if (listing)
	fprintf(listfile,
		"Compilation terminated, input file did not exist\n");
      CANN_exit(1);
    }
  } else {   /*else*/
    /* if (source != NULL)
      rewind(source);
    else */

    /* source = tmpfile(); */
    source = fopen( CANN_source_file, "w+" );
    CANN_source = source;

    if (source == NULL)
      _EscIO(FileNotFound);
    passerr = pass1(&txt,&filename, source);
    if (!passerr) {
      rewind(source);
      parse(&directoryname);
      string20(&stampstr, " Livermore Frontend ");
      string10(&versionstr, "   Version");
      string10(&versionnum, version);
      if (stampstr.len < maxstringchars)
	insertstring(&stampstr, &versionstr, stampstr.len + 1);
      if (stampstr.len < maxstringchars)
	insertstring(&stampstr, &versionnum, stampstr.len + 1);
      addstamp('F', stampstr);
      if (semerrcnt == 0)
	passerr = backend();
      else {
	printf("Due to semantic errors the optimization and offset ");
	printf("passes were skipped.\n");
	if (listing) {
	  fprintf(listfile, "Due to semantic errors the optimization and ");
	  fprintf(listfile, "offset passes were skipped.\n");
	}
      }
      dumpmodule();
      if (!passerr)   /* Backend returns true if all is well */
	CANN_exit(1);
    } else {
      printf("Compilation terminated, input file did not exist\n");
      if (listing)
	fprintf(listfile,
		"Compilation terminated, input file did not exist\n");
      CANN_exit(1);
    }
  }
  /* if (source != NULL)
    fclose(source); */
  /* FIXED BY CANN_exit 1/92 */
  /* if ( source != NULL ) { 
    fclose(source);
    sprintf(dummystring, "rm %s", dummyfilename);
    system(dummystring);
    } */

  if (diag != NULL)
    fclose(diag);
  if (ptableout != NULL)
    fclose(ptableout);
  if (etableout != NULL)
    fclose(etableout);
  if (inputfile != NULL)
    fclose(inputfile);
  if (reservedwords != NULL)
    fclose(reservedwords);
  if (listfile != NULL)
    fclose(listfile);
  CANN_exit(0);

/* p2c: mono.bin.noblank.p, line 25405:
 * Note: REWRITE does not specify a name [181] */
}



/* End. */
