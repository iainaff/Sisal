#include "frontend.h"

Static Void createlevel();
Static Void outtype();

Static node *buildcompound_(leveltype)
lexleveltype leveltype;
{
  node *compnd;

  compnd = newnodeptr(ndcompound, currentlevel);
  linkparent(compnd, currentlevel);
  createlevel(leveltype);
  currentlevel->prevnode = compnd;
  compnd->ndsrcline = linenumber;
  switch (leveltype) {

  case nestedifcomp:
  case ifcomp:
    currentlevel->UU.U8.ifnode = compnd;
    compnd->ndcode = ifnifthenelse;
    break;

  case tagcase:
    currentlevel->UU.U4.tagcasenode = compnd;
    compnd->ndcode = ifntagcase;
    break;

  case forloop:
  case specfa:
    currentlevel->UU.U0.forallnode = compnd;
    compnd->ndcode = ifnforall;
    break;

  case initloop:
    currentlevel->UU.U5.initnode = compnd;
    compnd->ndcode = -1;
    break;
  }/*case*/
  return compnd;
}


Local Void addgrptr(gr, comp)
graph *gr;
node *comp;
{
  graph *curgrptr;

  curgrptr = comp->UU.U2.ndsubsid;
  if (curgrptr == NULL) {
    comp->UU.U2.ndsubsid = gr;
    return;
  }
  while (curgrptr->grnext != NULL)
    curgrptr = curgrptr->grnext;
  curgrptr->grnext = gr;

  /*else*/
}  /*AddGRptr*/


Static node *buildsubgraph(comp)
node *comp;
{
  node *subgr;
  graph *gr;

  currentlevel->nodecounter = 0;
  subgr = newnodeptr(ndgraph, currentlevel);
  gr = newgraphptr();
  gr->grnode = subgr;
  addgrptr(gr, comp);
  subgr->ndcode = ifngraph;
  subgr->ndparent = comp;
  subgr->ndsrcline = linenumber;
  currentlevel->prevnode = subgr;
  currentlevel->currentsubn0 = subgr;
  return subgr;
}


Static comparetype stringcompare(str1, str2, mpt)
stryng *str1;
stryng *str2;
int     mpt;
{
  /*  The strutil routines have not been called here because this procedure
      will eventually end up in strutl.m4.*/
  comparetype Result;
  int stop, index1, index2;
  Char ch1, ch2;

  index1 = 1;
  index2 = 1;
  if (str1->len < str2->len)
    stop = str2->len + 1;
  else
    stop = str1->len + 1;
  while (index1 < stop) {
    ch1 = str1->str[index1 - 1];
    if (mpt) {
      if (isupper(ch1))
	ch1 = z_tolower(ch1);
    }
    ch2 = str2->str[index2 - 1];
    if (mpt) {
      if (isupper(ch2))
	ch2 = z_tolower(ch2);
    }
    if (ch1 < ch2) {
      Result = compareless;
      goto _L999;
    }
    if (ch1 > ch2) {
      Result = comparegreater;
      goto _L999;
    }
    index1++;
    index2++;
  }
  /* if we reach here, strings look equal;  check lengths */
  if (str1->len == str2->len)
    Result = compareequal;
  else if (str1->len < str2->len)
    Result = compareless;
  else
    Result = comparegreater;
_L999:
  return Result;
}  /*stringcompare*/


Local Void insertat(place)
int place;
{
  int i, temp;

  temp = idsorted[place - 1];
  idsorted[place - 1] = idsorted[idxrefmax - 1];
  for (i = idxrefmax; i >= place + 2; i--)
    idsorted[i - 1] = idsorted[i - 2];
  idsorted[place] = temp;
}

Local Void addtolist_(low, high)
int low, high;
{
  if (high == 0)
    return;
  if (high - low == 1 || high == 1) {
    if (stringcompare(&(idunsorted[idsorted[idxrefmax - 1] - 1].name),
		      &(idunsorted[idsorted[high - 1] - 1].name),
		      true) == comparegreater) {
      insertat(high + 1);
      return;
    }
    if (stringcompare(&(idunsorted[idsorted[idxrefmax - 1] - 1].name),
		      &(idunsorted[idsorted[low - 1] - 1].name),
		      true) == compareless)
      insertat(low);
    else
      insertat(high);
    return;
  }
  if (stringcompare(&(idunsorted[idsorted[idxrefmax - 1] - 1].name),
		    &(idunsorted[idsorted[(high + low) / 2 - 1] - 1].name),
		    true) == comparegreater) {
    addtolist_((high + low) / 2, high);
    return;
  }
  if (stringcompare(&(idunsorted[idsorted[idxrefmax - 1] - 1].name),
		    &(idunsorted[idsorted[(high + low) / 2 - 1] - 1].name),
		    true) == compareless)
    addtolist_(low, (high + low) / 2);
  else
    insertat((high + low) / 2);
}


Static Void addtoxref(buck)
symtblbucket *buck;
{
  /*AddBuck to the sorted XRef array.*/
  idxrefmax++;
  idunsorted[idxrefmax - 1].name = buck->name;
  idunsorted[idxrefmax - 1].refs = buck->references->front;
  idunsorted[idxrefmax - 1].defs = buck->defloc;
  idunsorted[idxrefmax - 1].typeptr = buck->typeptr;
  idsorted[idxrefmax - 1] = idxrefmax;
  addtolist_(1, idxrefmax - 1);
}


Local Void printxref()
{
  int i;
  xrefrec xrefbuck;
  reflist *reflist_;
  int FORLIM;

  FORLIM = idxrefmax;
  for (i = 0; i < FORLIM; i++) {
    xrefbuck = idunsorted[idsorted[i] - 1];
    writestring(listfile, &xrefbuck.name);
    fprintf(listfile, " : ");
    if (xrefbuck.typeptr->stliteral.len != 0)
      writestring(listfile, &xrefbuck.typeptr->stliteral);
    else
      outtype(listfile, xrefbuck.typeptr, true);
    putc('\n', listfile);
    if (xrefbuck.defs->fline != -1) {
      fprintf(listfile, "  *%d", xrefbuck.defs->fline);
      if (xrefbuck.defs->sline != -1)
	fprintf(listfile, "  *%d", xrefbuck.defs->sline);
    }
    reflist_ = xrefbuck.refs;
    while (reflist_ != NULL) {
      fprintf(listfile, "  %d", reflist_->line);
      reflist_ = reflist_->nextref;
    }
    fprintf(listfile, "\n\n");
  }
}


Static Void endlevel()
{
  /*If XRef, add the entrys in the symbol table to the sorted Xref array.
          If XRef and we are ending a function, print the XRef and then clear
          the array.*/
  treestackobj *dumpt;
  symtblbucket *tbucket, *dump;
  int i;

  for (i = 0; i <= maxhashtable; i++) {
    tbucket = currentlevel->hashtable[i];
    while (tbucket != NULL) {
      if (xref && !tbucket->importedflag)
	addtoxref(tbucket);
      dump = tbucket;
      tbucket = tbucket->nextbucket;
      freesymptr(dump);
    }  /* while */
  }  /* for */
  if (currentlevel->lleveltag == funct && xref) {
    fprintf(listfile,
      "\n------------------------------------------------------------------------- \n");
    fprintf(listfile, "Cross Reference for Function  ");
    writestring(listfile, &currentlevel->UU.lfunctname->name);
    fprintf(listfile, "\n\n");
    printxref();
    idxrefmax = 0;
    fprintf(listfile,
      "------------------------------------------------------------------------- \n\n");
  }
  dumpt = currentlevel;
  currentlevel = currentlevel->lastlevel;
  currentlevel->nextlevel = NULL;
  freetreestack(dumpt);
}  /* EndLevel */


Static Void endcompound()
{
  /*stackpops - nothing
                  pushes - tgexplist*/
  explistnode *explist;
  semanticrec *semexplist;
  typelistrec *typelist;
  int portnum;
  node *compnode;

  newsemantic(&semexplist, tgexplist);
  portnum = 1;
  switch (currentlevel->lleveltag) {

  case tagcase:
  case nestedifcomp:
  case ifcomp:
    if (currentlevel->lleveltag == ifcomp ||
	currentlevel->lleveltag == nestedifcomp) {
      typelist = currentlevel->UU.U4.tarmreturnstype;
      compnode = currentlevel->UU.U8.ifnode;
    } else {
      typelist = currentlevel->UU.U8.iarmreturnstype;
      compnode = currentlevel->UU.U4.tagcasenode;
    }  /*else*/
    semexplist->UU.explist = newexplistptr();
    explist = semexplist->UU.explist;
    explist->graphinfo->port_ = portnum;
    portnum++;
    explist->graphinfo->node_ = compnode;
    explist->graphinfo->typeptr = typelist->typeptr;
    typelist = typelist->next;
    while (typelist != NULL) {
      explist->next = newexplistptr();
      explist = explist->next;
      explist->graphinfo->port_ = portnum;
      portnum++;
      explist->graphinfo->node_ = compnode;
      explist->graphinfo->typeptr = typelist->typeptr;
      typelist = typelist->next;
    }  /*while*/
    break;
    /*IfComp, NestedIfComp,  TagCase*/
  }/*case*/
  endlevel();
  pushsemantic(semexplist);
}  /*EndCompound*/


Static Void addtoassolist(node_, asso)
node *node_;
int asso;
{
  assoclist *newa, *assoclist_;
  PSBLOCK b;

  /* newa = (assoclist *)Malloc(sizeof(assoclist)); */
  b = MySBlockAlloc(); /* CANN */
  newa = &(b->al); /* CANN */

  newa->graphnum = asso;
  newa->next = NULL;
  assoclist_ = node_->UU.U2.ndassoc;
  if (assoclist_ == NULL) {
    node_->UU.U2.ndassoc = newa;
    return;
  }
  while (assoclist_->next != NULL)
    assoclist_ = assoclist_->next;
  assoclist_->next = newa;

  /* else */
}  /* AddToAssoList */


Static port *orderolist(list)
port *list;
{
  int num, j, pass;
  boolean interch;
  port *lead, *middle, *trail;
  int FORLIM;

  num = 0;
  lead = list;
  while (lead != NULL) {
    num++;
    lead = lead->UU.U1.ptfrnext;
  }
  interch = true;
  pass = 1;
  while (pass < num && interch) {
    interch = false;
    middle = list;
    lead = middle->UU.U1.ptfrnext;
    trail = NULL;
    FORLIM = num - pass;
    for (j = 1; j <= FORLIM; j++) {
      if (middle->UU.U1.ptfrport > lead->UU.U1.ptfrport) {
	interch = true;
	middle->UU.U1.ptfrnext = lead->UU.U1.ptfrnext;
	lead->UU.U1.ptfrnext = middle;
	if (trail == NULL) {
	  list = lead;
	  trail = list;
	} else {
	  trail->UU.U1.ptfrnext = lead;
	  trail = lead;
	}
	lead = middle->UU.U1.ptfrnext;
      } else {
	trail = middle;
	middle = lead;
	lead = lead->UU.U1.ptfrnext;
      }
    }
    pass++;
  }
  return list;
}


Static port *orderilist(list)
port *list;
{
  int num, j, pass;
  boolean interch;
  port *lead, *middle, *trail;
  int FORLIM;

  num = 0;
  lead = list;
  while (lead != NULL) {
    num++;
    lead = lead->pttonext;
  }
  interch = true;
  pass = 1;
  while (pass < num && interch) {
    interch = false;
    middle = list;
    lead = middle->pttonext;
    trail = NULL;
    FORLIM = num - pass;
    for (j = 1; j <= FORLIM; j++) {
      if (middle->pttoport > lead->pttoport) {
	interch = true;
	middle->pttonext = lead->pttonext;
	lead->pttonext = middle;
	if (trail == NULL) {
	  list = lead;
	  trail = list;
	} else {
	  trail->pttonext = lead;
	  trail = lead;
	}
	lead = middle->pttonext;
      } else {
	trail = middle;
	middle = lead;
	lead = lead->pttonext;
      }
    }
    pass++;
  }
  return list;
}


Static boolean openlistfile(listfilename, listfile)
stryng listfilename;
FILE **listfile;
{
  stryng dev, name, ext;

  splitname(&listfilename, &dev, &name, &ext);
  defaultext(&listfilename, &name, "lst       ");
  if (canwrite(&listfilename))
    return (openwrite(listfile, &listfilename));
  else {
    printf("\n*****Could not open listing file:  ");
    writestring(stdout, &listfilename);
    printf(" *****\n");
    return false;
  }
}


Static Void createlevel(curlex)
lexleveltype curlex;
{
  /* Create a new Lex Level of type CurLex and add it to the LL stack.*/
  treestackobj *newelement;
  int i;

  newelement = (treestackobj *)Malloc(sizeof(treestackobj));
  newelement->lleveltag = curlex;
  for (i = 0; i <= maxhashtable; i++)
    newelement->hashtable[i] = NULL;
  newelement->prevnode = NULL;
  newelement->currentsubn0 = NULL;
  newelement->nodecounter = 0;
  newelement->lastlevel = currentlevel;
  newelement->nextlevel = NULL;
  switch (curlex) {   /* with */

  case forloop:
  case specfa:
    newelement->UU.U0.kflist = NULL;
    newelement->UU.U0.mflist = NULL;
    newelement->UU.U0.tflist = NULL;
    newelement->UU.U0.forallnode = NULL;
    newelement->UU.U0.lowerbound = NULL;
    newelement->UU.U0.crossdotflag = 0;
    newelement->UU.U0.crossdoterror = false;
    newelement->UU.U0.freturnportnum = 0;
    newelement->UU.U0.returnn0 = NULL;
    newelement->UU.U0.bodyn0 = NULL;
    newelement->UU.U0.returnnodecount = 1;
    newelement->UU.U0.bodynodecount = 1;
    newelement->UU.U0.bodyoutputportnum = 0;
    newelement->UU.U0.bodyprevnode = NULL;
    newelement->UU.U0.returnprevnode = NULL;
    break;

  case initloop:
    newelement->UU.U5.kilist = NULL;
    newelement->UU.U5.lilist = NULL;
    newelement->UU.U5.tilist = NULL;
    newelement->UU.U5.initnode = NULL;
    newelement->UU.U5.ireturnportnum = 0;
    break;

  case tagcase:
    newelement->UU.U4.ktport = 1;
    newelement->UU.U4.tagcasenode = NULL;
    newelement->UU.U4.tarmreturnstype = NULL;
    break;

  case ifcomp:
  case nestedifcomp:
    newelement->UU.U8.kiport = 1;
    newelement->UU.U8.ifnode = NULL;
    newelement->UU.U8.iarmreturnstype = NULL;
    break;

  case funct:
    newelement->UU.lfunctname = NULL;
    break;

  case let:
  case root:
    /* blank case */
    break;
  }/* case */
  if (currentlevel != NULL)
    currentlevel->nextlevel = newelement;
  currentlevel = newelement;
}


Static Void endblock()
{
  /*Pop a lex level off the LL stack.*/
  if (currentlevel->lleveltag != root)
    currentlevel = currentlevel->lastlevel;
}


Static symtblbucket *createbucket(bucktype)
identtype bucktype;
{
  /*Create a symbol table entry of type BuckType and returns its pointer.  */
  symtblbucket *newbuck;
  PBBLOCK b;
  PSBLOCK sb;

  /* newbuck = (symtblbucket *)Malloc(sizeof(symtblbucket)); */
  b = MyBBlockAlloc(); /* CANN */
  newbuck = &(b->sb); /* CANN */

  /* newbuck->graphinfo = (inforecord *)Malloc(sizeof(inforecord)); */
  b = MyBBlockAlloc(); /* CANN */
  newbuck->graphinfo = &(b->ir); /* CANN */

  /* newbuck->defloc = (defrec *)Malloc(sizeof(defrec)); */
  sb = MySBlockAlloc(); /* CANN */
  newbuck->defloc = &(sb->dr); /* CANN */

  mymemcpy(newbuck->name.str, blankstring, sizeof(stryngar));
  newbuck->name.len = 0;
  mymemcpy(newbuck->graphinfo->name.str, blankstring, sizeof(stryngar));
  newbuck->graphinfo->name.len = 0;
  newbuck->identtag = bucktype;
  newbuck->defloc->fline = -1;
  newbuck->defloc->fcolumn = -1;
  newbuck->defloc->sline = -1;
  newbuck->defloc->scolumn = -1;
  newbuck->references = newreferencesptr();
  newbuck->typeptr = NULL;
  newbuck->graphinfo->node_ = NULL;
  newbuck->graphinfo->port_ = -1;
  newbuck->graphinfo->typeptr = NULL;
  newbuck->graphinfo->onlist = NULL;
  newbuck->importedflag = false;
  switch (bucktype) {

  case ttype:
    newbuck->UU.undeftypeptrs = NULL;
    break;

  case tvariable:
    newbuck->UU.U4.loopflag = (loopvarflags *)Malloc(sizeof(loopvarflags));
    *newbuck->UU.U4.loopflag = lnone;
    newbuck->UU.U4.tflag = false;

    /* newbuck->UU.U4.oldfrom = (inforecord *)Malloc(sizeof(inforecord)); */
    b = MyBBlockAlloc(); /* CANN */
    newbuck->UU.U4.oldfrom = &(b->ir); /* CANN */

    newbuck->UU.U4.oldfrom->node_ = NULL;
    newbuck->UU.U4.oldfrom->port_ = -1;
    newbuck->UU.U4.oldfrom->typeptr = NULL;
    newbuck->UU.U4.oldfrom->onlist = NULL;
    newbuck->UU.U4.redefflag = false;
    break;

  case tglobal:
  case tforward:
  case tfunction:
  case tparam:
    /* blank case */
    break;

  case tpredefined:
    newbuck->UU.predefined = nopredef;
    break;
  }/* case */
  newbuck->nextbucket = NULL;   /* with */
  return newbuck;
}


Static Void setname(inbucket, ident)
symtblbucket *inbucket;
stryng ident;
{
  inbucket->name = ident;
  inbucket->graphinfo->name = ident;
}


Static Void setdefloc(inbucket, line, column)
symtblbucket *inbucket;
int line, column;
{
  if (inbucket->defloc->fline < 0) {
    inbucket->defloc->fline = line;
    inbucket->defloc->fcolumn = column;
  } else {
    inbucket->defloc->sline = line;
    inbucket->defloc->scolumn = column;
  }
}


Static Void addref(inbucket, line, column)
symtblbucket *inbucket;
int line, column;
{
  reflist *ref;
  PSBLOCK b;

  /* ref = (reflist *)Malloc(sizeof(reflist)); */
  b = MySBlockAlloc(); /* CANN */
  ref = &(b->rl); /* CANN */

  ref->line = line;
  ref->column = column;
  ref->nextref = NULL;
  if (inbucket->references->front == NULL)
    inbucket->references->front = ref;
  else
    inbucket->references->back->nextref = ref;
  inbucket->references->back = ref;
}


Static Void setlowerbound(level, lower)
treestackobj *level;
inforecord *lower;
{
  /*Set the lower bound info in the lex level passed in.*/
  if (level->UU.U0.lowerbound == NULL)
    level->UU.U0.lowerbound = newinfoptr();
  level->UU.U0.lowerbound->node_ = lower->node_;
  level->UU.U0.lowerbound->port_ = lower->port_;
  level->UU.U0.lowerbound->typeptr = lower->typeptr;
  level->UU.U0.lowerbound->name = lower->name;
  level->UU.U0.lowerbound->litvalue = lower->litvalue;
  level->UU.U0.lowerbound->onlist = lower->onlist;
}


/*macro procedure SetTypePtr(InBucket : SymTblPtr; TypeTb : STPtr); */
/*macro procedure SetLoopFlag(InBucket : SymTblPtr; Flag : LoopVarFlag); */
/*macro procedure SetFromOld(InBucket : SymTblPtr; FromInfo : InfoPtr)*/
/*macro procedure SetLFunctName(Name : SymTblPtr); */
/*macro procedure SetPrevNode(Node : NDPtr); */
/*macro procedure SetCurrentSubN0(Node : NDPtr); */
/*macro procedure SetNodeGraphInfo(InBucket : SymTblPtr; From : NDPtr)*/
/*macro procedure SetNodeOldFrom(InBucket : SymTblPtr; From : NDPtr)*/
/*macro procedure SetPortGraphInfo(InBucket : SymTblPtr; Port : integer) */
/*macro procedure SetPortOldFrom(InBucket : SymTblPtr; Port : integer) */
/*macro procedure SetTypePtrGraphInfo(InBucket : SymTblPtr; FType : TypeTPtr) */
/*macro procedure SetNameGraphInfo(InBucket : SymTblPtr; Name : stryng)*/
/*macro procedure SetOnListOldFrom(InBucket : SymTblPtr; List : EdgeHeadPtr) */
/*macro procedure SetOnListGraphInfo(InBucket : SymTblPtr; List : EdgeHeadPtr) */
/*macro function GetLowerBound(Level : TreeStackPtr): InfoPtr; */
/*macro function GetIdentTag(InBucket : SymTblPtr): IdentType; */
/*macro function GetName(InBucket : SymTblPtr) : Stryng); */
Static int getdefline(inbucket, time)
symtblbucket *inbucket;
int time;
{
  if (time == 1)
    return (inbucket->defloc->fline);
  else
    return (inbucket->defloc->sline);
}


Static int getdefcolumn(inbucket, time)
symtblbucket *inbucket;
int time;
{
  if (time == 1)
    return (inbucket->defloc->fcolumn);
  else
    return (inbucket->defloc->scolumn);
}


/*macro function GetType(InBucket: SymTblPtr) : STPtr; */
/*macro function GetUndefTypePtrs(InBucket : SymTblPtr) : pttypelistrec; */
Static loopvarflags getloopflag(inbucket)
symtblbucket *inbucket;
{
  if (inbucket->identtag == tvariable && inbucket->UU.U4.loopflag != NULL)
    return (*inbucket->UU.U4.loopflag);
  else
    return lnone;
}


/*macro function GetOldFrom(InBucket : SymTblPtr ) : InfoPtr; */
/*macro function GetLLevelTag(Level : TreeStackptr) : LexLevelType; */
/*macro function GetPrevNode(Level : TreeStackptr) : NDPtr; */
/*macro function GetCurrentSubN0(Level : TreeStackptr) : NDPtr; */
/*macro function GetLFunctName(Level : TreeStackPtr) : SymTblPtr; */
/*macro function GetNodeGraphInfo(InBucket : SymTblPtr) : NDPtr;*/
/*macro function GetNodeOldFrom(InBucket : SymTblPtr) : NDPtr;*/
/*macro function GetPortGraphInfo(InBucket : SymTblPtr) : integer; */
/*macro function GetTypePtrGraphInfo(InBucket : SymTblPtr) : TypeTPtr;*/
/*macro function GetNameGraphInfo(InBucket : SymTblPtr) : stryng); */
/*macro function GetOnListGraphInfo(InBucket : SymTblPtr) : EdgeHeadPtr; */
Static int hash(firstch, lastch)
Char firstch, lastch;
{
  return ((firstch + lastch) % hashtablesize);
/* p2c: mono.bin.noblank.p, line 11587:
 * Note: Using % for possibly-negative arguments [317] */
}


Static Void addtotable(bucket, level)
symtblbucket *bucket;
treestackobj *level;
{
  /*Hash the bucket in the symbol table for Level.*/
  int position;

  stringlowercase(&bucket->name);
  position = hash(stringchar(&bucket->name, 1),
		  stringchar(&bucket->name, bucket->name.len));
  bucket->nextbucket = level->hashtable[position];
  level->hashtable[position] = bucket;
}


Static symtblbucket *incurrentlevel(ident, itype)
stryng ident;
identtype itype;
{
  /*Search current symbol table for an entry with name = Ident and
          IdentTag = IType.  Return its pointer or nil if not found.*/
  symtblbucket *Result, *chain;
  boolean found;
  int position;

  Result = NULL;
  found = false;
  stringlowercase(&ident);
  position = hash(stringchar(&ident, 1), stringchar(&ident, ident.len));
  chain = currentlevel->hashtable[position];
  while (chain != NULL && !found) {
    if (equalstrings(&chain->name, &ident)) {
      switch (itype) {

      case ttype:
	if (chain->identtag == ttype) {
	  found = true;
	  Result = chain;
	}  /* if */
	break;

      case tforward:
      case tglobal:
      case tpredefined:
      case tfunction:
	if (chain->identtag == tfunction || chain->identtag == tforward ||
	    chain->identtag == tglobal || chain->identtag == tpredefined) {
	  found = true;
	  Result = chain;
	}  /* if */
	break;

      case tparam:
      case tvariable:
	if (chain->identtag == tvariable || chain->identtag == tparam) {
	  found = true;
	  Result = chain;
	}  /* if */
	break;
      }/* case */
    }
    chain = chain->nextbucket;
  }  /* while */
  return Result;
}


Static Void findname(ident, itype, level, place)
stryng ident;
identtype itype;
treestackobj **level;
symtblbucket **place;
{
  /*Search all symbol tables back to  a function boundary for an
          entry with name = Ident and IdentTag = IType.*/
  treestackobj *looklevel;
  symtblbucket *chain;
  boolean found;
  int position;

  stringlowercase(&ident);
  looklevel = currentlevel;
  found = false;
  position = hash(stringchar(&ident, 1), stringchar(&ident, ident.len));
  *place = NULL;
  *level = NULL;
  do {
    chain = looklevel->hashtable[position];
    while (chain != NULL && !found) {
      if (equalstrings(&chain->name, &ident)) {
	switch (itype) {

	case ttype:
	  if (chain->identtag == ttype) {
	    found = true;
	    *level = looklevel;
	    *place = chain;
	  }  /* if */
	  break;

	case tforward:
	case tglobal:
	case tpredefined:
	case tfunction:
	  if (chain->identtag == tfunction || chain->identtag == tforward ||
	      chain->identtag == tglobal || chain->identtag == tpredefined) {
	    found = true;
	    *level = looklevel;
	    *place = chain;
	  }  /* if */
	  break;

	case tparam:
	case tvariable:
	  if (chain->identtag == tvariable || chain->identtag == tparam) {
	    found = true;
	    *level = looklevel;
	    *place = chain;
	  }  /* if */
	  break;
	}/* case */
      }
      chain = chain->nextbucket;
    }  /* while */
    if (looklevel->lleveltag == funct && itype != tfunction &&
	itype != tglobal && itype != tpredefined && itype != ttype)
      found = true;
    else if (looklevel->lastlevel == NULL)
      found = true;
    else
      looklevel = looklevel->lastlevel;
  } while (!found);
}


Local Void addpredefined(name, tag)
stryng name;
predeftype tag;
{
  symtblbucket *bucket;

  bucket = createbucket(tpredefined);
  setname(bucket, name);
  bucket->UU.predefined = tag;
  addtotable(bucket, currentlevel);
}


Static Void initsymtable()
{
  /*Initialize the root symbol table to contain all the predefined
          functions.*/
  stryng predefined;

  currentlevel = NULL;
  createlevel(root);
  string10(&predefined, "Abs       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgabs);
  string10(&predefined, "Array_AddH");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayaddh);
  string10(&predefined, "Array_AddL");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayaddl);
  string20(&predefined, "Array_Adjust        ");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayadjust);
  string10(&predefined, "Array_Fill");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayfill);
  string10(&predefined, "Array_LimH");
  stripspaces(&predefined);
  addpredefined(predefined, tgarraylimh);
  string10(&predefined, "Array_LimL");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayliml);
  string20(&predefined, "Array_PrefixSize    ");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayprefixsize);
  string10(&predefined, "Array_RemH");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayremh);
  string10(&predefined, "Array_RemL");
  stripspaces(&predefined);
  addpredefined(predefined, tgarrayreml);
  string10(&predefined, "Array_SetL");
  stripspaces(&predefined);
  addpredefined(predefined, tgarraysetl);
  string10(&predefined, "Array_Size");
  stripspaces(&predefined);
  addpredefined(predefined, tgarraysize);
  string10(&predefined, "Exp       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgexp);
  string10(&predefined, "Floor     ");
  stripspaces(&predefined);
  addpredefined(predefined, tgfloor);
  string10(&predefined, "Max       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgmax);
  string10(&predefined, "Min       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgmin);
  string10(&predefined, "Mod       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgmod);

/* CANN PEEK */  string10(&predefined, "Peek      ");
/* CANN PEEK */stripspaces(&predefined);
/* CANN PEEK */addpredefined(predefined, tgpeek);

  string20(&predefined, "Stream_Append       ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamappend);
  string20(&predefined, "Stream_Empty        ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamempty);
  string20(&predefined, "Stream_First        ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamfirst);
  string20(&predefined, "Stream_PrefixSize   ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamprefixsize);
  string20(&predefined, "Stream_Rest         ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamrest);
  string20(&predefined, "Stream_Size         ");
  stripspaces(&predefined);
  addpredefined(predefined, tgstreamsize);
  string10(&predefined, "Trunc     ");
  stripspaces(&predefined);
  addpredefined(predefined, tgtrunc);
}


Static Void outtype(fil, typ, printname)
FILE *fil;
stentry *typ;
boolean printname;
{
  /*Output to the file = Fil the type = Typ.  If PrintName is true
          and the type has a name, just print the name.  Otherwise print
          the structure of the type.  This routine calls itself recursively.  */
  if (typ == NULL) {
    return;
  }  /* if */
  if (typ->stliteral.len != 0 && printname) {
    writestring(fil, &typ->stliteral);
    return;
  }
  switch (typ->stsort) {

  case iftwild:
    fprintf(fil, " wild ");
    break;

  case iftbasic:
    switch (typ->UU.stbasic) {

    case 0:
      fprintf(fil, " Boolean ");
      break;

    case 1:
      fprintf(fil, " Character ");
      break;

    case 2:
      fprintf(fil, " Double ");
      break;

    case 3:
      fprintf(fil, " Integer ");
      break;

    case 4:
      fprintf(fil, " Null ");
      break;

    case 5:
      fprintf(fil, " Real ");
      break;

    case 6:
      fprintf(fil, " Wild ");
      break;
    }
    break;

  case iftfunctiontype:
    fprintf(fil, " Function ");
    break;

  case iftarray:
    fprintf(fil, " Array[ ");
    if (!typ->strecurflag)
      outtype(fil, typ->UU.stbasetype, true);
    else
      writestring(fil, &typ->UU.stbasetype->stliteral);
    fprintf(fil, " ]");
    break;

  case iftstream:
    fprintf(fil, " Stream( ");
    if (!typ->strecurflag)
      outtype(fil, typ->UU.stbasetype, true);
    else
      writestring(fil, &typ->UU.stbasetype->stliteral);
    fprintf(fil, " )");
    break;

  case iftmultiple:
    fprintf(fil, " Multiple( ");
    if (!typ->strecurflag)
      outtype(fil, typ->UU.stbasetype, true);
    else
      writestring(fil, &typ->UU.stbasetype->stliteral);
    fprintf(fil, " )");
    break;

  case iftrecord:
    fprintf(fil, " Record[ ");
    outtype(fil, typ->UU.stbasetype, false);
    fprintf(fil, " ]");
    break;

  case iftunion:
    fprintf(fil, " Union[ ");
    outtype(fil, typ->UU.stbasetype, false);
    fprintf(fil, " ]");
    break;

  case iftfield:
    writestring(fil, &typ->stliteral);
    fprintf(fil, " : ");
    if (typ->UU.U2.stelemtype != NULL) {
      if (typ->UU.U2.stelemtype->stliteral.len == 0)
	outtype(fil, typ->UU.U2.stelemtype, false);
      else
	writestring(fil, &typ->UU.U2.stelemtype->stliteral);
    } else
      printf(" Undef Type ");
    if (typ->UU.U2.stnext != NULL)
      fprintf(fil, "; ");
    outtype(fil, typ->UU.U2.stnext, false);
    break;

  case ifttuple:
    writestring(fil, &typ->stliteral);
    fprintf(fil, " : ");
    if (typ->UU.U2.stelemtype->stliteral.len == 0)
      outtype(fil, typ->UU.U2.stelemtype, false);
    else
      writestring(fil, &typ->UU.U2.stelemtype->stliteral);
    putc('\n', fil);
    outtype(fil, typ->UU.U2.stnext, false);
    break;

  case ifttag:
    writestring(fil, &typ->stliteral);
    fprintf(fil, " : ");
    if (typ->UU.U2.stelemtype != NULL) {
      if (typ->UU.U2.stelemtype->stliteral.len == 0)
	outtype(fil, typ->UU.U2.stelemtype, false);
      else
	writestring(fil, &typ->UU.U2.stelemtype->stliteral);
    } else
      printf(" Undef Type ");
    if (typ->UU.U2.stnext != NULL)
      fprintf(fil, "; ");
    outtype(fil, typ->UU.U2.stnext, false);
    break;
  }/* case */
}


Static Char *identtype_NAMES[] = {
  "ttype", "tglobal", "tfunction", "tforward", "tvariable", "tparam",
  "tpredefined", "none"
} ;


Static Char *loopvarflags_NAMES[] = {
  "lnone", "lvar", "lconst", "linit", "lbinit"
} ;


Static Void printbucket(symbucket)
symtblbucket *symbucket;
{
  /*Print the contents of the symbol table entry.*/
  reflist *ref;

  printf("%s  ", identtype_NAMES[(long)symbucket->identtag]);
  writestring(stdout, &symbucket->name);
  if (symbucket->identtag == tpredefined) {
    return;
  }  /* if */
  printf("  Defs %7d", symbucket->defloc->fline);
  if (symbucket->defloc->sline != -1)
    printf(" , %12d", symbucket->defloc->sline);
  putchar('\n');
  if (symbucket->references != NULL) {
    ref = symbucket->references->front;
    while (ref != NULL) {
      printf("%12d ", ref->line);
      ref = ref->nextref;
    }
    putchar('\n');
  } else
    printf("References pointer is nil\n");
  if (symbucket->typeptr != NULL) {
    if (symbucket->typeptr->stsort == iftbasic)
      outtype(stdout, symbucket->typeptr, true);
    writestring(stdout, &symbucket->typeptr->stliteral);
    putchar('\n');
  }
  if (symbucket->graphinfo != NULL) {
    printf("GraphInfo\n");
    if (symbucket->graphinfo->node_ != NULL)
      printf("Node = %4d", symbucket->graphinfo->node_->ndlabel);
    else
      printf("node is nil");
    printf("Port = %4d", symbucket->graphinfo->port_);
    writestring(stdout, &symbucket->graphinfo->name);
    printf("  ");
    writestring(stdout, &symbucket->graphinfo->litvalue);
    putchar('\n');
  } else
    printf("GraphInfo is nil\n");
  if (symbucket->identtag != tvariable) {
    return;
  }  /* if */
  printf("  LoopFlag is : %s",
	 loopvarflags_NAMES[(long)(*symbucket->UU.U4.loopflag)]);
  printf(" TFlag is : %s", symbucket->UU.U4.tflag ? " TRUE" : "FALSE");
  printf(" RedefFlag is : %s\n",
	 symbucket->UU.U4.redefflag ? " TRUE" : "FALSE");
  if (symbucket->UU.U4.oldfrom == NULL)
    return;
  printf("Old GraphInfo\n");
  if (symbucket->UU.U4.oldfrom->node_ != NULL)
    printf("Node = %4d", symbucket->UU.U4.oldfrom->node_->ndlabel);
  else
    printf("node is nil ");
  printf("Port = %4d", symbucket->UU.U4.oldfrom->port_);
  writestring(stdout, &symbucket->UU.U4.oldfrom->name);
  printf("  ");
  writestring(stdout, &symbucket->UU.U4.oldfrom->litvalue);
  putchar('\n');
}  /* PrintBucket */


typedef struct symlistobj {
  symtblbucket *entry_;
  struct symlistobj *next;
} symlistobj;


/* Local variables for printtable: */
struct LOC_printtable {
  symlistobj *frontl, *backl;
} ;

Local Void newsymlist(sym, LINK)
symlistobj **sym;
struct LOC_printtable *LINK;
{
  *sym = (symlistobj *)Malloc(sizeof(symlistobj));
  (*sym)->entry_ = NULL;
  (*sym)->next = NULL;
}  /* NewSymList */

Local Void freesymlist(sym, LINK)
symlistobj *sym;
struct LOC_printtable *LINK;
{
  /* Free(sym); */
}  /* FreeSymList */

Local Void addtofunctlist(sym, LINK)
symtblbucket *sym;
struct LOC_printtable *LINK;
{
  symlistobj *syml;

  newsymlist(&syml, LINK);
  syml->entry_ = sym;
  if (LINK->frontl == NULL)
    LINK->frontl = syml;
  else
    LINK->backl->next = syml;
  LINK->backl = syml;
}  /* addtofunctlist */


Static Void printtable(fil, node_)
FILE *fil;
treestackobj *node_;
{
  /*Print the symbol table node to the file.*/
  struct LOC_printtable V;
  int i;
  symtblbucket *chain;
  symlistobj *dump;

  V.frontl = NULL;
  V.backl = NULL;
  /*writeln(Fil,'  Name                    Class      Defined    Referenced      Type   ');*/
  putc('\n', fil);
  for (i = 0; i <= maxhashtable; i++) {
    chain = node_->hashtable[i];
    while (chain != NULL) {
      if (chain->identtag != tfunction && chain->identtag != tglobal &&
	  chain->identtag != tforward && chain->identtag != tpredefined)
	printbucket(chain);
      else
	addtofunctlist(chain, &V);
      chain = chain->nextbucket;
    }  /* while */
  }  /* for */
  while (V.frontl != NULL) {
    chain = V.frontl->entry_;
    fprintf(fil, "Function ");
    writestring(fil, &chain->name);
    fprintf(fil, "  %s\n", identtype_NAMES[(long)chain->identtag]);
    if (chain->typeptr != NULL) {
      fprintf(fil, " arguments ");
      outtype(fil, chain->typeptr->UU.U3.starg, true);
      fprintf(fil, "\n returns ");
      outtype(fil, chain->typeptr->UU.U3.stres, true);
      putc('\n', fil);
    }  /* if */
    dump = V.frontl;
    V.frontl = V.frontl->next;
    freesymlist(dump, &V);
  }  /* while */
}


Static Void printtypes(fil)
FILE *fil;
{
  /*Print out the IF1 type table.*/
  int i;

  for (i = 0; i < entrymax; i++) {
    if (typetable[i] != NULL) {
      if (typetable[i]->stid == 1) {
	writestring(fil, &typetable[i]->stliteral);
	fprintf(fil, " :\n");
	fprintf(fil, "           ");
	outtype(fil, typetable[i], false);
	putc('\n', fil);
	typetable[i]->stid = -1;
      }  /* if */
    }
  }  /* for */
}


Static Char *lexleveltype_NAMES[] = {
  "forloop", "specfa", "let", "funct", "tagcase", "initloop", "ifcomp",
  "root", "nestedifcomp"
} ;


Static Void printtree(fil, current)
FILE *fil;
treestackobj *current;
{
  /*Print all the symbol tables from current down to root.*/
  symtblbucket *fun;

  while (current != NULL) {
    putc('\n', fil);
    if (current->lleveltag == funct) {
      fun = current->UU.lfunctname;
      fprintf(fil, "Function ");
      if (fun == NULL)
	printf(" LFunctName is nil\n");
      else {
	writestring(fil, &fun->name);
	putc('\n', fil);
	if (fun->typeptr != NULL) {
	  fprintf(fil, " arguments\n");
	  outtype(fil, fun->typeptr->UU.U3.starg, true);
	  fprintf(fil, "\n returns\n");
	  outtype(fil, fun->typeptr->UU.U3.stres, true);
	  putc('\n', fil);
	}  /* if */
	else
	  printf("LFunctName type pointer is nil\n");
      }  /* else */
    }  /* if */
    else
      fprintf(fil, "%s\n", lexleveltype_NAMES[(long)current->lleveltag]);
    printtable(fil, current);
    current = current->nextlevel;
  }  /* while */
}  /* PrintTree */


Static Void crossref(fil, level)
FILE *fil;
treestackobj *level;
{
  /*Print out sym table at Level, used only by debugger.*/
  if (level == NULL) {
    return;
  }  /* if */
  printtree(fil, level);
  fprintf(fil,
    "\n --------------------------------------------------------------------\n\n");
  printtypes(fil);
}


Static symtblbucket *getsymtabentry(id, bucktype)
stryng id;
identtype bucktype;
{
  /*Get a symbol table entry of type Bucktype and set its name to
          id.  Returns its pointer.*/
  symtblbucket *symentry;

  symentry = createbucket(bucktype);
  setname(symentry, id);
  return symentry;
}  /* getsymtabentry */


Static edgelisthead *addnametolist(list, name)
edgelisthead **list;
stryng name;
{
  edgelisthead *newhead, *inlist;

  newhead = newheadptr();
  newhead->edgename = name;
  if (*list == NULL) {
    *list = newhead;
    return newhead;
  }  /* if */
  inlist = *list;
  while (inlist->next != NULL)
    inlist = inlist->next;
  inlist->next = newhead;
  return newhead;
}


Static edgelisthead *addtoklist(name, edge, list)
stryng name;
port *edge;
edgelisthead *list;
{
  edgelisthead *Result;
  edgelist *newedgerec;
  edgelisthead *newhead;
  boolean done, found;

  done = false;
  found = false;
  while (!done && !found) {
    if (!equalstrings(&list->edgename, &name)) {
      if (list->next != NULL)
	list = list->next;
      else
	done = true;
      continue;
    }  /* if */
    newedgerec = newedgelistptr();
    newedgerec->edge = edge;
    newedgerec->next = list->edges;
    newedgerec->toorfrom = toport;
    list->edges = newedgerec;
    Result = list;
    found = true;
  }  /* while */
  if (!done) {
    return Result;
  }  /* if */
  newhead = newheadptr();
  newedgerec = newedgelistptr();
  newedgerec->next = NULL;
  newhead->edgename = name;
  newhead->edges = newedgerec;
  newedgerec->edge = edge;
  newedgerec->toorfrom = toport;
  list->next = newhead;
  return newhead;
/* p2c: mono.bin.noblank.p: Note: Deleting unreachable code [255] */
}  /* AddToKList */


Static edgelisthead *addtoforklist(name, edge, level)
stryng name;
port *edge;
treestackobj *level;
{
  edgelisthead *klist;
  edgelist *newedgerec;
  edgelisthead *newhead;

  if (level->lleveltag == forloop || level->lleveltag == specfa) {
    klist = level->UU.U0.kflist;
    if (klist != NULL)
      return (addtoklist(name, edge, klist));
    else {
      newhead = newheadptr();
      newedgerec = newedgelistptr();
      newedgerec->next = NULL;
      newhead->edgename = name;
      newhead->edges = newedgerec;
      newedgerec->edge = edge;
      newedgerec->toorfrom = toport;
      level->UU.U0.kflist = newhead;
      return newhead;
    }  /* else */
  }  /* if */
  printf(" tag not Forloop or SpecFA in AddToForKList\n");
  return NULL;
}  /* AddToForKList */


Static edgelisthead *addtoiklist(name, edge, level)
stryng name;
port *edge;
treestackobj *level;
{
  edgelisthead *klist;
  edgelist *newedgerec;
  edgelisthead *newhead;

  if (level->lleveltag == initloop) {
    klist = level->UU.U5.kilist;
    if (klist != NULL)
      return (addtoklist(name, edge, klist));
    else {
      newhead = newheadptr();
      newedgerec = newedgelistptr();
      newedgerec->next = NULL;
      newhead->edgename = name;
      newhead->edges = newedgerec;
      newedgerec->edge = edge;
      newedgerec->toorfrom = toport;
      level->UU.U5.kilist = newhead;
      return newhead;
    }  /* else */
  }  /* if */
  printf(" tag not InitLoop in AddToIKList\n");
  return NULL;
}  /* AddToIKList */


Static Void addtoolist(edge, node_)
port *edge;
node *node_;
{
  /* Add the edge to the output port list of the node suppiled.*/
  port *trail, *olist;
  boolean found;
  stryng name1, name2;

  if (edge->UU.U1.ptfrport != -1)   /* if */
  {  /* if a port num has been assigned */
    olist = node_->ndolist;
    if (olist == NULL) {
      node_->ndolist = edge;
      return;
    }  /* if olist <> nil */
    trail = NULL;
    found = false;
    while (!found) {  /* list is ordered by port num */
      /* if (olist->UU.U1.ptfrport > edge->UU.U1.ptfrport) */
      if (olist->UU.U1.ptfrport >= edge->UU.U1.ptfrport)  /* CANN FASTER!!! */
	found = true;
      else {
	trail = olist;
	olist = olist->UU.U1.ptfrnext;
      }
      if (olist == NULL)
	found = true;
    }  /* while */
    edge->UU.U1.ptfrnext = olist;
    if (trail == NULL)
      node_->ndolist = edge;
    else
      trail->UU.U1.ptfrnext = edge;
    return;
  }
  /* else no port num has been assigned */
  olist = node_->ndolist;
  if (olist == NULL) {
    node_->ndolist = edge;
    return;
  }  /* if */
  trail = NULL;
  found = false;
  name2 = edge->ptname;
  stringlowercase(&name2);
  while (!found) {  /* Search for others with the same name */
    name1 = olist->ptname;
    stringlowercase(&name1);
    if (equalstrings(&name1, &name2)) {
      found = true;
      break;
    }
    trail = olist;
    olist = olist->UU.U1.ptfrnext;
    if (olist == NULL)
      found = true;
  }  /* while */
  edge->UU.U1.ptfrnext = olist;
  if (trail == NULL)
    node_->ndolist = edge;
  else
    trail->UU.U1.ptfrnext = edge;

  /* else */
}  /* AddToOList */


Static Void addtoilist(edge, node_)
port *edge;
node *node_;
{
  /*  Add the edge to the input port list of the supplied node*/
  port *trail, *ilist;
  boolean found;

  if (edge->pttoport != -1) {  /* if no port num is assigned */
    ilist = node_->ndilist;
    if (ilist == NULL) {
      node_->ndilist = edge;
      return;
    }  /* if olist <> nil */
    trail = NULL;
    found = false;
    while (!found) {  /* list is ordered by port num */
      if (ilist->pttoport > edge->pttoport)
	found = true;
      else {
	trail = ilist;
	ilist = ilist->pttonext;
      }
      if (ilist == NULL)
	found = true;
    }  /* while */
    edge->pttonext = ilist;
    if (trail == NULL)
      node_->ndilist = edge;
    else
      trail->pttonext = edge;
    return;
  }  /* if */
  ilist = node_->ndilist;
  if (ilist == NULL) {
    node_->ndilist = edge;
    return;
  }  /* if */
  trail = NULL;
  found = false;
  while (!found) {  /* look for others with same name */
    if (equalstrings(&ilist->ptname, &edge->ptname)) {
      found = true;
      break;
    }
    trail = ilist;
    ilist = ilist->pttonext;
    if (ilist == NULL)
      found = true;
  }  /* while */
  edge->pttonext = ilist;
  if (trail == NULL)
    node_->ndilist = edge;
  else
    trail->pttonext = edge;

  /* else no port num is assigned */
  /* else */
}  /* AddToIList */


Static symtblbucket *copysym(old)
symtblbucket *old;
{
  /*  Build a new sym table bucket with the same info as the one passed in */
  symtblbucket *new_;

  new_ = createbucket(old->identtag);
  new_->name = old->name;
  new_->graphinfo->name = old->name;
  new_->defloc = old->defloc;
  new_->references = old->references;
  new_->typeptr = old->typeptr;
  new_->importedflag = true;
  switch (old->identtag) {

  case tvariable:
    new_->UU.U4.loopflag = old->UU.U4.loopflag;
    new_->UU.U4.tflag = old->UU.U4.tflag;
    new_->UU.U4.redefflag = old->UU.U4.redefflag;
    new_->UU.U4.oldfrom->name = old->name;
    insertchar(&new_->UU.U4.oldfrom->name, '-', 1);
    insertchar(&new_->UU.U4.oldfrom->name, 'd', 1);
    insertchar(&new_->UU.U4.oldfrom->name, 'l', 1);
    insertchar(&new_->UU.U4.oldfrom->name, 'o', 1);
    break;

  case tglobal:
  case tforward:
  case tfunction:
    printf("Pulling function !!\n");
    break;

  case tparam:
    /* blank case */
    break;

  case ttype:
    printf(" Pulling type!!\n");
    break;
  }/* case */
  return new_;
}  /* CopySym */


Static port *emptyoldedge(sym)
symtblbucket *sym;
{
  /* Return an edge that comes from the OldFrom graph info in Sym*/
  port *newedge;

  newedge = newedgeptr(ptedge);
  newedge->UU.U1.ptfrnode = sym->UU.U4.oldfrom->node_;
  newedge->ptname = sym->name;
  insertchar(&newedge->ptname, '-', 1);
  insertchar(&newedge->ptname, 'd', 1);
  insertchar(&newedge->ptname, 'l', 1);
  insertchar(&newedge->ptname, 'o', 1);
  newedge->pttype = sym->typeptr;
  if (sym->UU.U4.oldfrom->onlist == NULL)
    newedge->UU.U1.ptfrport = sym->UU.U4.oldfrom->port_;
  return newedge;
}


Static port *emptyedge(sym)
symtblbucket *sym;
{
  /* Return an edge that comes from the graph info in Sym*/
  port *newedge;

  newedge = newedgeptr(ptedge);
  newedge->UU.U1.ptfrnode = sym->graphinfo->node_;
  newedge->ptname = sym->name;
  newedge->pttype = sym->typeptr;
  if (sym->graphinfo->onlist == NULL)
    newedge->UU.U1.ptfrport = sym->graphinfo->port_;
  return newedge;
}


Static int nextkport(level)
treestackobj *level;
{
  if (level->lleveltag == ifcomp || level->lleveltag == nestedifcomp) {
    level->UU.U8.kiport++;
    return (level->UU.U8.kiport);
  } else {
    level->UU.U4.ktport++;
    return (level->UU.U4.ktport);
  }
}  /* NexrKPort */


Static symtblbucket *pullname(symname, level, line)
symtblbucket *symname;
treestackobj *level;
int line;
{
  /* This routine connects the edges required to bring the value represented
     by SymName in Lex Level Level into the current lex level.  If SymName is
     a loop name it will also pull in the OldFrom value is it is defined.
     A symbol table entry containing the graphinfo for the value in the
     current lex level is returned.*/
  symtblbucket *newsym;
  edgelisthead *list;
  edgelist *newel;
  treestackobj *fromlevel;
  port *edge, *dupedge;
  boolean oldpull, graphinfopull;

  if (level == currentlevel) {  /* if not at the inner level */
    return symname;
  }  /* if */
  graphinfopull = true;
  oldpull = false;
  if (symname->identtag == tvariable) {
    if (*symname->UU.U4.loopflag == lvar) {  /* if pulling a loop name */
      oldpull = true;   /* and it had OldFrom info */
      if (!symname->UU.U4.redefflag)   /* then pull that along too */
	graphinfopull = false;
    }
  }
  while (level != currentlevel) {
    /* go until we reach the inner-
                                                  most level*/
    newsym = copysym(symname);
    level = level->nextlevel;
    if (level->lleveltag != let) {
      if (graphinfopull) {
	if (symname->graphinfo->node_ == NULL) {  /* It's a lit */
	  edge = newedgeptr(ptlit);
	  edge->ptname = symname->name;
	  edge->UU.ptlitvalue = symname->graphinfo->litvalue;
	  edge->pttype = symname->typeptr;
	  edge->ptsrcline = line;
	} else {
	  edge = emptyedge(symname);
	  edge->ptsrcline = line;
	  list = symname->graphinfo->onlist;
	  if (list == NULL && level->lleveltag != funct)
		/* not on a list, so it's easy */
		  addtoolist(edge, symname->graphinfo->node_);
	  else {  /* else need to add to a list */
	    newel = newedgelistptr();
	    newel->edge = edge;
	    newel->next = list->edges;
	    newel->toorfrom = fromport;
	    list->edges = newel;
	    fromlevel = level->lastlevel;   /* find the current SG Node */
	    while (fromlevel->lleveltag == let)   /* add to the SG Olist */
	      fromlevel = fromlevel->lastlevel;
	    addtoolist(edge, fromlevel->currentsubn0);
	  }
	}  /* else */
      }  /* if */
      if (oldpull) {  /* do the same thing for the old value */
	if (symname->UU.U4.oldfrom->node_ == NULL) {
	  dupedge = newedgeptr(ptlit);
	  dupedge->ptname = symname->name;
	  insertchar(&dupedge->ptname, '-', 1);
	  insertchar(&dupedge->ptname, 'd', 1);
	  insertchar(&dupedge->ptname, 'l', 1);
	  insertchar(&dupedge->ptname, 'o', 1);
	  dupedge->UU.ptlitvalue = symname->UU.U4.oldfrom->litvalue;
	  dupedge->pttype = symname->typeptr;
	  dupedge->ptsrcline = line;
	} else {
	  dupedge = emptyoldedge(symname);
	  dupedge->ptsrcline = line;
	  list = symname->UU.U4.oldfrom->onlist;
	  if (list == NULL && level->lleveltag != funct)
	    addtoolist(dupedge, symname->UU.U4.oldfrom->node_);
	  else {
	    newel = newedgelistptr();
	    newel->edge = dupedge;
	    newel->next = list->edges;
	    newel->toorfrom = fromport;
	    list->edges = newel;
	    fromlevel = level->lastlevel;
	    while (fromlevel->lleveltag == let)
	      fromlevel = fromlevel->lastlevel;
	    addtoolist(dupedge, fromlevel->currentsubn0);
	  }
	}  /* else */
      }  /* if */
    }  /* if */
    /* Now have to process the TO end of the edge.  Add the edge to the
       input list of the node, set the TO node in the edge and set
       the Node Graphinfo to the current subgraph in that lex level.  */
    switch (level->lleveltag) {

    case forloop:
    case specfa:
      if (graphinfopull) {
	newsym->graphinfo->onlist = addtoforklist(symname->name, edge, level);
	edge->pttonode = level->UU.U0.forallnode;
	addtoilist(edge, level->UU.U0.forallnode);
	newsym->graphinfo->node_ = level->currentsubn0;
	newsym->graphinfo->typeptr = symname->typeptr;
      }  /* if */
      if (oldpull) {
	newsym->UU.U4.oldfrom->onlist = addtoforklist(dupedge->ptname,
						      dupedge, level);
	dupedge->pttonode = level->UU.U0.forallnode;
	addtoilist(dupedge, level->UU.U0.forallnode);
	newsym->UU.U4.oldfrom->node_ = level->currentsubn0;
	newsym->UU.U4.oldfrom->typeptr = symname->typeptr;
      }  /* then */
      addtotable(newsym, level);
      break;

    case initloop:
      if (graphinfopull) {
	newsym->graphinfo->onlist = addtoiklist(symname->name, edge, level);
	edge->pttonode = level->UU.U5.initnode;
	addtoilist(edge, level->UU.U5.initnode);
	newsym->graphinfo->node_ = level->currentsubn0;
	newsym->graphinfo->typeptr = symname->typeptr;
      }  /* if */
      if (oldpull) {
	newsym->UU.U4.oldfrom->onlist = addtoiklist(dupedge->ptname, dupedge,
						    level);
	dupedge->pttonode = level->UU.U5.initnode;
	addtoilist(dupedge, level->UU.U5.initnode);
	newsym->UU.U4.oldfrom->node_ = level->currentsubn0;
	newsym->UU.U4.oldfrom->typeptr = symname->typeptr;
      }  /* if */
      addtotable(newsym, level);
      break;

    case tagcase:
      if (graphinfopull) {
	newsym->graphinfo->onlist = NULL;
	edge->pttonode = level->UU.U4.tagcasenode;
	edge->pttoport = nextkport(level);
	addtoilist(edge, level->UU.U4.tagcasenode);
	newsym->graphinfo->node_ = level->currentsubn0;
	newsym->graphinfo->port_ = edge->pttoport;
	newsym->graphinfo->typeptr = edge->pttype;
      }  /* if */
      if (oldpull) {
	newsym->UU.U4.oldfrom->onlist = NULL;
	dupedge->pttonode = level->UU.U4.tagcasenode;
	dupedge->pttoport = nextkport(level);
	addtoilist(dupedge, level->UU.U4.tagcasenode);
	newsym->UU.U4.oldfrom->node_ = level->currentsubn0;
	newsym->UU.U4.oldfrom->port_ = dupedge->pttoport;
	newsym->UU.U4.oldfrom->typeptr = dupedge->pttype;
      }
      addtotable(newsym, level);
      break;

    case ifcomp:
    case nestedifcomp:
      if (graphinfopull) {
	newsym->graphinfo->onlist = NULL;
	edge->pttonode = level->UU.U8.ifnode;
	edge->pttoport = nextkport(level);
	addtoilist(edge, level->UU.U8.ifnode);
	newsym->graphinfo->node_ = level->currentsubn0;
	newsym->graphinfo->port_ = edge->pttoport;
	newsym->graphinfo->typeptr = edge->pttype;
      }  /* if */
      if (oldpull) {
	newsym->UU.U4.oldfrom->onlist = NULL;
	dupedge->pttonode = level->UU.U8.ifnode;
	dupedge->pttoport = nextkport(level);
	addtoilist(dupedge, level->UU.U8.ifnode);
	newsym->UU.U4.oldfrom->node_ = level->currentsubn0;
	newsym->UU.U4.oldfrom->port_ = dupedge->pttoport;
	newsym->UU.U4.oldfrom->typeptr = dupedge->pttype;
      }
      addtotable(newsym, level);
      break;

    case funct:
      if (graphinfopull)
	newsym->graphinfo = symname->graphinfo;
      if (oldpull)
	newsym->UU.U4.oldfrom = symname->UU.U4.oldfrom;
      addtotable(newsym, level);
      break;

    case let:
    case root:
      if (graphinfopull)
	newsym->graphinfo = symname->graphinfo;
      if (oldpull)
	newsym->UU.U4.oldfrom = symname->UU.U4.oldfrom;
      addtotable(newsym, level);
      break;
    }/* case */
    symname = newsym;
  }  /* while */
  return symname;
}  /* PullName */


Static Void addedge(from, tonode)
inforecord *from, *tonode;
{
  /* This routine do everything necessary to create and edge between the
     From graphinfo and the To graphinfo.  */
  port *edge;
  edgelist *edgeel;
  int line;
  symtblbucket *sym;
  stryng str, oldstr;
  /* boolean oldflag; */

  sym = NULL;
  line = -1;
  /* oldflag = false; */
  if (from->name.len != 0) {
    if (tonode->node_->ndcode == ifncall && tonode->port_ == 1) {
      /*name is a function*/
      sym = incurrentlevel(from->name, tfunction);
    } else {
      substring(&str, &from->name, 1, 4);
      string10(&oldstr, "old-      ");
      stripspaces(&oldstr);
      if (equalstrings(&str, &oldstr)) {
	str = from->name;
	deletestring(&str, 1, 4);
	/* oldflag = true; */
      } else
	str = from->name;
      sym = incurrentlevel(str, tvariable);
    }
    if (sym != NULL) {
      if (sym->references->back != NULL)
	line = sym->references->back->line;
      else if (getdefline(sym, 2) > 0)
	line = getdefline(sym, 2);
      else
	line = getdefline(sym, 1);
    }  /*then*/
  }  /*then*/
  if (from->node_ == NULL) {  /* from is a literal */
    edge = newedgeptr(ptlit);
    edge->pttype = from->typeptr;
    edge->ptname = from->name;
    edge->UU.ptlitvalue = from->litvalue;
    edge->ptsrcline = line;
  } else {
    edge = newedgeptr(ptedge);
    edge->UU.U1.ptfrnode = from->node_;
    edge->pttype = from->typeptr;
    edge->ptname = from->name;
    edge->ptsrcline = line;
    if (from->onlist == NULL) {  /* If port is assigned */
      edge->UU.U1.ptfrport = from->port_;
      addtoolist(edge, from->node_);
    }  /* if Onlist = nil */
    else {  /* else place on the list */
      edgeel = newedgelistptr();
      edgeel->edge = edge;
      edgeel->toorfrom = fromport;
      edgeel->next = from->onlist->edges;
      from->onlist->edges = edgeel;
      addtoolist(edge, from->node_);
    }  /* else Onlist <> nil */
  }  /* from not a literal */
  edge->pttonode = tonode->node_;   /* process the To Node */
  if (tonode->onlist == NULL) {  /* if port is assigned */
    edge->pttoport = tonode->port_;
    addtoilist(edge, tonode->node_);
    return;
  }  /* if Onlist = nil */
  edgeel = newedgelistptr();
  edgeel->edge = edge;
  edgeel->toorfrom = toport;
  edgeel->next = tonode->onlist->edges;
  tonode->onlist->edges = edgeel;
  addtoilist(edge, tonode->node_);

  /* else put on the list */
}  /* AddEdge */


Static Void pushnilttptr()
{
  /*action 93 PushNilTTPtr*/
  /*stack              pops - nothing
                       pushes - TGTTptr*/
  /*description
            Create a new type table semantic record and push it on the
            stack.*/
  semanticrec *sem;

  if (semtrace)
    printf("In PushNilTTPtr\n");
  newsemantic(&sem, tgttptr);
  pushsemantic(sem);
}  /* PushNilTTPtr*/


Static Void pushbasic(token)
int token;
{
  /*action 10 PushBasic*/
  /*stack              pops - nothing
                       pushes - TGTTptr*/
  /*description
           Get a type table pointer of the correct basic type.
           Create a new type table semantic record and save the basic
           type pointer in it.  Push the semantic record*/
  semanticrec *semrec;
  stentry *typeptr;

  if (semtrace)
    printf("In PushBasic\n");
  switch (token) {

  case nullkw:
    typeptr = getbasictype(ifbnull);
    break;

  case boolkw:
    typeptr = getbasictype(ifbboolean);
    break;

  case intgkw:
    typeptr = getbasictype(ifbinteger);
    break;

  case realkw:
    typeptr = getbasictype(ifbreal);
    break;

  case doubkw:
    typeptr = getbasictype(ifbdouble);
    break;

  case charkw:
    typeptr = getbasictype(ifbcharacter);
    break;
  }/* case */
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = typeptr;
  pushsemantic(semrec);
}  /* PushBasic */


Static Void pushtypename(inputtoken, line, column)
stryng inputtoken;
int line, column;
{
  /*action 11 PushTypeName*/
  /*stack              pops - nothing
                       pushes - TGSymList*/
  /*description
          Search the current SymTab entry for type name of inputtoken.  If
          one is found its typeptr should be empty.  Otherwise this is a
          second defition of this type name so declare and error.  If the
          name is found with a nil typeptr, this means it was referenced
          before it was defined which is ok.  If no entry is found, create
          a new one.  Save the symtabptr on the stack.*/
  symtblbucket *symname;
  semanticrec *symptr;
  errorrecord *errorrec;

  if (semtrace)
    printf("In PushTypeName\n");
  symname = incurrentlevel(inputtoken, ttype);
  if (symname == NULL) {
    symname = getsymtabentry(inputtoken, ttype);
    symname->typeptr = NULL;
    setdefloc(symname, line, column);
    addtotable(symname, currentlevel);
  } else {
    if (symname->typeptr != NULL) {
      errorrec = newerrorptr(typedoubdef);
      errorrec->UU.errorstr = inputtoken;
      errorrec->linenumber = line;
      semerror(errorrec);
    }
  }
  newsemantic(&symptr, tgsymptr);
  symptr->UU.symptr = symname;
  pushsemantic(symptr);
}  /* PushTypeName */


Static Void pusharray(flgaddtotable)
boolean flgaddtotable;
{
  /*action 12 PushArray*/
  /*stack              pops - nothing
                       pushes - TGTTPtr*/
  /*description
           Get an array type table pointer.  Create a semantic record
           and save the type pointer in it. Push the semantic record.*/
  stentry *parray;
  semanticrec *semrec;

  if (semtrace)
    printf("In PushArray\n");
  newttptr(&parray, iftarray);
  if (flgaddtotable)
    putintable(&parray);
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = parray;
  pushsemantic(semrec);
}  /* PushArray */


Static Void pushstream(flgaddtotable)
boolean flgaddtotable;
{
  /*action 16 PushStream*/
  /*stack              pops - nothing
                       pushes - TGTTPtr*/
  /*description
           Get a stream type table pointer.  Create a semantic record
           and save the type pointer in it. Push the semantic record.*/
  stentry *pstream;
  semanticrec *semrec;

  if (semtrace)
    printf("In PushStream\n");
  newttptr(&pstream, iftstream);
  if (flgaddtotable)
    putintable(&pstream);
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = pstream;
  pushsemantic(semrec);
}  /* PushStream */


Static Void pushrecord(flgaddtotable)
boolean flgaddtotable;
{
  /*action 17 PushRecord*/
  /*stack              pops - nothing
                       pushes - TGTTPtr*/
  /*description
           Get a record type table pointer.  Create a semantic record
           and save the type pointer in it. Push the semantic record.*/
  stentry *precord;
  semanticrec *semrec;

  if (semtrace) {
    printf("In PushRecord");
    if (flgaddtotable)
      printf(" and put in table\n");
    else
      printf(" and no put in table\n");
  }
  newttptr(&precord, iftrecord);
  if (flgaddtotable)
    putintable(&precord);
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = precord;
  pushsemantic(semrec);
}  /* PushRecord */


Static Void pushunion(flgaddtotable)
boolean flgaddtotable;
{
  /*action 19 PushUnion*/
  /*stack              pops - nothing
                       pushes - TGTTPtr*/
  /*description
           Get a union type table pointer.  Create a semantic record
           and save the type pointer in it. Push the semantic record.*/
  stentry *punion;
  semanticrec *semrec;

  if (semtrace)
    printf("In PushUnion\n");
  newttptr(&punion, iftunion);
  if (flgaddtotable)
    putintable(&punion);
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = punion;
  pushsemantic(semrec);
}  /* PushUnion */


Static Void pushnulltype()
{
  /*action 21 PushNullType*/
  /*stack              pops - nothing
                       pushes - TGTTPtr*/
  /*description
           Get a null type table pointer.  Create a semantic record
           and save the type pointer in it. Push the semantic record.*/
  stentry *pbasic;
  semanticrec *semrec;

  if (semtrace)
    printf("In PushNullType\n");
  newttptr(&pbasic, iftbasic);
  pbasic = getbasictype(ifbnull);
  newsemantic(&semrec, tgttptr);
  semrec->UU.ttptr = pbasic;
  pushsemantic(semrec);
}  /* PushNullType */


Static Void checktypename(inputtoken, line, column)
stryng inputtoken;
int line, column;
{
  /*action 14 CheckTypeName*/
  /*stack              pops - nothing
                       pushes - TGTTPtr or TGSymList*/
  /*description
          Search for type name in the symbol type.  If not found and
          we are type parsing the type def section, make an entry and
          put it on the undef type list.  If it is not found, but we
          are done with the type defs, then declare an error.  If the
          SymTab entries typeptr is nil, push the SymTabPtr else
          push the typeptr.*/
  semanticrec *semrec;
  symtblbucket *syname;
  treestackobj *level;
  errorrecord *errorrec;
  boolean undef;

  if (semtrace) {
    printf("In CheckTypeName, name = ");
    writestring(stdout, &inputtoken);
    putchar('\n');
  }
  undef = false;
  findname(inputtoken, ttype, &level, &syname);
  if (syname == NULL) {
    undef = true;
    syname = getsymtabentry(inputtoken, ttype);
    addref(syname, line, column);
    addtotable(syname, currentlevel);
  }
  if (typedefsfinished) {
    newsemantic(&semrec, tgttptr);
    if (undef) {
      errorrec = newerrorptr(tnameundef);
      errorrec->UU.errorstr = inputtoken;
      errorrec->linenumber = line;
      semerror(errorrec);
      syname->typeptr = getbasictype(ifbwild);
      semrec->UU.ttptr = syname->typeptr;
    } else if (syname->typeptr == NULL) {
      printf("***** COMPILER ERROR*****");
      writestring(stdout, &syname->name);
      printf("  type ptr is nil\n");
    } else {
      semrec->UU.ttptr = syname->typeptr;
      addref(syname, line, column);
    }
  } else {
    if (undef) {
      newsemantic(&semrec, tgsymptr);
      semrec->UU.symptr = syname;
    } else if (syname->typeptr == NULL) {
      addref(syname, line, column);
      newsemantic(&semrec, tgsymptr);
      semrec->UU.symptr = syname;
    } else {
      addref(syname, line, column);
      newsemantic(&semrec, tgttptr);
      semrec->UU.ttptr = syname->typeptr;
    }
  }  /* else */
  pushsemantic(semrec);

  /* then */
  /*else*/
}  /* CheckTypeName */


Static Void finishtypelinks()
{
  /*action 13 FinishTypeLinks*/
  /*stack              pops - TGTTPtr or TGSymTblPtr
                              TGSymTblPtr
                       pushes - Nothing*/
  /*description
          Since this routine can be called recursively, check that the
          Type Ptr for SemName^.symptr is nil.  Then if the top stack
          entry is a typeptr we now have a type def for SemName^.symptr.
          Fill in its typeptr and search its UndefTypeptr list.  The
          items on the list can be TypePtrs or SymPtrs, for TypePtrs,
          just fill in the TypePtr with the new structure; for SymPtrs
          call this routine recursively after pushing the info back on
          the stack.  Otherwise the top item on the stack was a SymPtr.
          In this case, add this SymPtr to the UndefTypeList of the
          "lower" SymPtr.  This was when the "lower" SymPtr has its
          type definded we will also fill in the other SymPtr's typeptr.*/
  semanticrec *rsemtorn, *rsemname, *semtorn, *semname;
  stentry *tptr;
  symtblbucket *sptr;
  tornrec *undeftypelist, *old, *tornelem;

  if (semtrace)
    printf("In FinishTypeLinks\n");
  semtorn = popsemantic();
  semname = popsemantic();
  if (semname->UU.symptr->typeptr == NULL)
  {   /*if Double Def, might not be nil*/
    if (semtorn->tag == tgttptr) {
      semname->UU.symptr->typeptr = semtorn->UU.ttptr;
      semname->UU.symptr->typeptr->stliteral = semname->UU.symptr->name;
      undeftypelist = semname->UU.symptr->UU.undeftypeptrs;
      if (undeftypelist != NULL) {
	semname->UU.symptr->UU.undeftypeptrs = NULL;
	while (undeftypelist != NULL) {
	  switch (undeftypelist->torntag) {

	  case typetag:
	    tptr = undeftypelist->UU.ttptr;
	    switch (tptr->stsort) {

	    case iftmultiple:
	    case iftarray:
	    case iftstream:
	      tptr->UU.stbasetype = semtorn->UU.ttptr;
	      break;

	    case iftfield:
	    case ifttag:
	    case ifttuple:
	      tptr->UU.U2.stelemtype = semtorn->UU.ttptr;
	      break;

	    case iftbasic:
	    case iftwild:
	    case iftrecord:
	    case iftfunctiontype:
	    case iftunion:   /* This should never occur */
	      break;
	    }/* case */
	    break;

	  case symtag:
	    sptr = undeftypelist->UU.symptr;
	    newsemantic(&rsemtorn, tgttptr);
	    newsemantic(&rsemname, tgsymptr);
	    rsemtorn->UU.ttptr = semtorn->UU.ttptr;
	    rsemname->UU.symptr = sptr;
	    pushsemantic(rsemname);
	    pushsemantic(rsemtorn);
	    finishtypelinks();
	    break;
	  }/*case*/
	  old = undeftypelist;
	  undeftypelist = undeftypelist->next;
	  freetornlist(&old);
	}  /* while */
      }  /*then*/
    }  /*then*/
    else {  /*have 2 names*/
      newtornlist(&tornelem, symtag);
      tornelem->UU.symptr = semname->UU.symptr;
      semtorn->UU.symptr->UU.undeftypeptrs = linktorntype(tornelem,
	  semtorn->UU.symptr->UU.undeftypeptrs);
    }  /*else*/
  }  /*then*/
  freesemantic(&semtorn);
  freesemantic(&semname);
}  /* FinishTypeLinks */


Static Void linkbase()
{
  /*action 15 LinkBase*/
  /*stack              pops - TGTTPtr or TGSymPtr
                              TGTTPtr
                       pushes - TGTTPtr*/
  /*description
          If the first semantic record is a symbol table pointer then
          add the second type table structure to the Undef list
          else set the base type of the second type
          table pointer to be the first type table pointer. Push the
          remaining type table structure.*/
  semanticrec *tpsemrec, *nxtsemrec;
  tornrec *torn;

  if (semtrace)
    printf("In LinkBase\n");
  tpsemrec = popsemantic();
  nxtsemrec = popsemantic();
  switch (tpsemrec->tag) {

  case tgsymptr:
    newtornlist(&torn, typetag);
    torn->UU.ttptr = nxtsemrec->UU.ttptr;
    tpsemrec->UU.symptr->UU.undeftypeptrs = linktorntype(torn,
	tpsemrec->UU.symptr->UU.undeftypeptrs);
    break;

  case tgttptr:
    nxtsemrec->UU.ttptr->UU.stbasetype = tpsemrec->UU.ttptr;
    break;
  }/* case */
  pushsemantic(nxtsemrec);
  freesemantic(&tpsemrec);
}  /* LinkBase */


Static Void buildfort(ifttype, flgaddtotable)
char ifttype;
boolean flgaddtotable;
{
  semanticrec *typef, *namelist, *struct_;
  namelistrec *dump, *names;
  stentry *fort, *oldnames;
  boolean done;
  tornrec *torn;
  errorrecord *errorrec;

  typef = popsemantic();
  namelist = popsemantic();
  struct_ = popsemantic();
  names = namelist->UU.namelist;
  while (names != NULL) {
    newttptr(&fort, ifttype);
    if (flgaddtotable)
      putintable(&fort);
    fort->stliteral = names->name;
    switch (typef->tag) {

    case tgsymptr:
      newtornlist(&torn, typetag);
      torn->UU.ttptr = fort;
      typef->UU.symptr->UU.undeftypeptrs = linktorntype(torn,
	  typef->UU.symptr->UU.undeftypeptrs);
      break;

    case tgttptr:
      fort->UU.U2.stelemtype = typef->UU.ttptr;
      break;
    }/* case */
    oldnames = struct_->UU.ttptr->UU.stbasetype;
    if (oldnames != NULL) {
      done = false;
      while (!done) {
	if (equalstrings(&names->name, &oldnames->stliteral)) {
	  if (ifttype == iftfield) {
	    errorrec = newerrorptr(fieldnameddef);
	    errorrec->UU.errorstr = names->name;
	    semerror(errorrec);
	  } else {
	    errorrec = newerrorptr(tagnameddef);
	    errorrec->UU.errorstr = names->name;
	    semerror(errorrec);
	  }
	}  /* if */
	if (oldnames->UU.U2.stnext != NULL)
	  oldnames = oldnames->UU.U2.stnext;
	else
	  done = true;
      }  /* while */
      oldnames->UU.U2.stnext = fort;
    }  /* if */
    else
      struct_->UU.ttptr->UU.stbasetype = fort;
    dump = names;
    names = names->next;
    freenamelist(&dump);
  }  /* while */
  pushsemantic(struct_);
}  /* BuildForT */


Static Void buildfield(flgaddtotable)
boolean flgaddtotable;
{
  /*action 18 BuildField*/
  /*stack              pops - TGTTPtr or TGSymptr
                              TGNameList
                              TGTTPtr
                       pushes - TGTTPtr*/
  /*description
          Create a field with name on TGNameList and type of the
          first type table pointer.  If this type has not been defined
          yet, its SymPtr will be on the stack.  Then a pointer to the
          field's typeptr will be saved in the SymPtr entry to be filled
          in later.  Be sure name is distinct from all other names in
          this record. Push resulting record.*/
  if (semtrace) {
    printf("In BuildField");
    if (flgaddtotable)
      printf(" and add to table\n");
    else
      printf(" do not add to table\n");
  }
  buildfort(iftfield, flgaddtotable);
}


Static Void buildtag(flgaddtotable)
boolean flgaddtotable;
{
  /*action 20 BuildTag*/
  /*stack              pops - TGTTPtr or TGSymPtr
                              TGNameList
                              TGTTPtr
                       pushes - TGTTPtr*/
  /*description
          Create a tag with name on TGNameList and type of the
          first type table pointer.  If this type has not been defined
          yet, its SymPtr will be on the stack.  Then a pointer to the
          field's typeptr will be saved in the SymPtr entry to be filled
          in later.  Be sure name is distinct from all other names in
          this union. Push resulting union.*/
  if (semtrace)
    printf("In BuildTag\n");
  buildfort(ifttag, flgaddtotable);
}


Static Void starttypedefs()
{
  /*action 152  StartTypeDefs*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Init variables for type definition section.*/
  typedefsfinished = false;
}


Local Void printtypelist()
{
  int i;
  xrefrec xrefbuck;
  int FORLIM;

  fprintf(listfile,
    "-----------------------------------------------------------------------\n");
  fprintf(listfile, " These are the global types\n\n");
  FORLIM = idxrefmax;
  for (i = 0; i < FORLIM; i++) {
    xrefbuck = idunsorted[idsorted[i] - 1];
    writestring(listfile, &xrefbuck.name);
    fprintf(listfile, " : ");
    outtype(listfile, xrefbuck.typeptr, false);
    fprintf(listfile, "\n\n");
  }
  fprintf(listfile,
    "-----------------------------------------------------------------------\n");
}


Static Void endtypedefs()
{
  /*action 145  EndTypeDefs*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symtbl for any type names whose type ptrs are
          still nil.  These types are undefined.  Make them type
          wild, declare and error and fill in all the typeptrs
          on their UndefTypeptrs list with type wild.  The symptrs
          on the list can be ignored because they will be found in
          the sym tab search.  Do a global type smash of the type
          table.  If the XRef flag is set, print out each type name
          and its structure.*/
  int i;
  symtblbucket *buck;
  errorrecord *errorrec;
  tornrec *tlist;

  if (semtrace)
    printf("In EndTypeDefs\n");
  for (i = 0; i <= maxhashtable; i++) {
    buck = currentlevel->hashtable[i];
    while (buck != NULL) {
      if (buck->identtag == ttype) {
	if (buck->UU.undeftypeptrs != NULL || buck->typeptr == NULL) {
	  errorrec = newerrorptr(tnameundef);
	  errorrec->UU.errorstr = buck->name;
	  if (buck->references->front != NULL)
	    errorrec->linenumber = buck->references->front->line;
	  else if (buck->defloc->fline != -1)
	    errorrec->linenumber = buck->defloc->fline;
	  else
	    errorrec->linenumber = linenumber;
	  semerror(errorrec);
	  buck->typeptr = getbasictype(ifbwild);
	  tlist = buck->UU.undeftypeptrs;
	  while (tlist != NULL) {
	    if (tlist->torntag == typetag) {
	      switch (tlist->UU.ttptr->stsort) {

	      case iftmultiple:
	      case iftarray:
	      case iftstream:
		tlist->UU.ttptr->UU.stbasetype = getbasictype(ifbwild);
		break;

	      case iftfield:
	      case ifttag:
	      case ifttuple:
		tlist->UU.ttptr->UU.U2.stelemtype = getbasictype(ifbwild);
		break;

	      case iftbasic:
	      case iftwild:
	      case iftrecord:
	      case iftfunctiontype:
	      case iftunion:   /* This should never occur */
		break;
	      }/* case */
	    }  /*then*/
	    tlist = tlist->next;
	  }  /*while*/
	}  /*then*/
      }  /*then*/
      buck = buck->nextbucket;
    }  /*while*/
  }  /*for*/
  smashtypes();
  adjustgraphptrs();
  for (i = 0; i <= maxhashtable; i++) {
    buck = currentlevel->hashtable[i];
    while (buck != NULL) {
      if (buck->identtag == ttype) {
	buck->typeptr = buck->typeptr->stequivchain;
	if (xref)
	  addtoxref(buck);
      }
      buck = buck->nextbucket;
    }  /*while*/
  }  /*for*/
  if (xref) {
    /* printtypelist(); */
    idxrefmax = 0;
  }
  compacttypetable(&tthwm, 7);
  typedefsfinished = true;
}


Static Void smashintotable()
{
  /*action  8  SmashIntoTable*/
  /*stackpops - TgTTPtr
                  pushes - TgTTPtr*/
  /*description
          Pop the type table pointer off the stack and smash it into the
          type table.  Save the resulting type table pointer on the stack. */
  semanticrec *semttptr;
  stentry *tempptr;

  if (semtrace)
    printf("In SmashIntoTable\n");
  semttptr = popsemantic();
  tempptr = addtotypetable(semttptr->UU.ttptr);
  semttptr->UU.ttptr = tempptr;
  pushsemantic(semttptr);
}


Static Void startabuild()
{
  /*action 99 StartABuild*/
  /*stackpops - nothing
                  pushes - tgexplist
                           tgTTptr*/
  /*description
          Get an ABuild node and store it in an explist record and push the
          record on the stack.*/
  semanticrec *semexplist;

  if (semtrace)
    printf("begin StartABuild\n");
  newsemantic(&semexplist, tgexplist);
  semexplist->UU.explist = newexplistptr();
  semexplist->UU.explist->graphinfo->node_ = newnodeptr(ndatomic, currentlevel);
  semexplist->UU.explist->graphinfo->node_->ndsrcline = linenumber;
  semexplist->UU.explist->graphinfo->node_->ndcode = ifnabuild;
  linkparent(semexplist->UU.explist->graphinfo->node_, currentlevel);
  pushsemantic(semexplist);
}


Static Void dolowerbound()
{
  /*action 101 DoLowerBound*/
  /*stackpops - tgexplist  (for lower bound)
                         tgexplist  (for ABuild node)
                  pushes - tgexplist  (for ABuild node)*/
  /*description
          Check that arity of lower bound exp = 1 and that it's type is
          integer.  Connect the lower bound exp to port 1 of the ABuild
          node.  If the lower bound expression is nil then wire in a
          literal 1 on port 1.*/
  semanticrec *semexplb, *semexpabuild;
  errorrecord *errorrec;
  inforecord *lbexp;

  if (semtrace)
    printf("begin DoLowerBound\n");
  semexplb = popsemantic();
  semexpabuild = popsemantic();
  if (semexplb->UU.explist == NULL) {
    lbexp = newinfoptr();   /* set the LB to 1 */
    lbexp->typeptr = getbasictype(ifbinteger);
    string10(&lbexp->litvalue, "1         ");
  } else {
    lbexp = semexplb->UU.explist->graphinfo;
    if (semexplb->UU.explist->next != NULL) {
      errorrec = newerrorptr(lbarity);
      errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->ndsrcline;
      semerror(errorrec);
    }
    if (lbexp->typeptr->stlabel != intlabel) {
      errorrec = newerrorptr(lbtype);
      errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->ndsrcline;
      semerror(errorrec);
      lbexp = newinfoptr();
      lbexp->typeptr = getbasictype(ifbinteger);
      string10(&lbexp->litvalue, "1         ");
    }
  }  /*else*/
  semexpabuild->UU.explist->graphinfo->port_ = 1;
  addedge(lbexp, semexpabuild->UU.explist->graphinfo);
  pushsemantic(semexpabuild);
  if (semexplb->UU.explist == NULL) {
    /* push the empty list back so we have
                                       an empty list for the array elem. */
    pushsemantic(semexplb);
  } else
    freesemantic(&semexplb);
}


Static Void endabuild()
{
  /*action 102 EndABuild*/
  /*stackpops - tgexplist  (array elem)
                         tgexplist  (Abuild node)
                         tgTTptr    (type name)
                  pushes - tgexplist (Abuild node output)*/
  /*description
          If not array elem is given than a type name must be given.  Check
          that the type name is for an array.  If both a name and an array
          elem are given make sure the types are compatible.  If array elems
          are given, connect then to the ABuild node starting with port 2.
          Push the graph info for the output of the Abuild onto the stack.  */
  semanticrec *semexpaelem, *semexpabuild, *semttname, *semttatype;
  errorrecord *errorrec;
  stentry *atype;
  explistnode *element;
  int portnum;

  if (semtrace)
    printf("begin EndABuild\n");
  semexpaelem = popsemantic();
  semexpabuild = popsemantic();
  semttname = popsemantic();
  if (semexpaelem->UU.explist == NULL) {   /* no element is supplied */
    if (semttname->UU.ttptr == NULL) {   /* no type name is supplied */
      errorrec = newerrorptr(notypeabuild);
      errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->ndsrcline;
      semerror(errorrec);
      pusharray(noaddtottable);
      semttatype = popsemantic();
      atype = semttatype->UU.ttptr;
      atype->UU.stbasetype = getbasictype(ifbwild);
      atype = addtotypetable(atype);
	  /* we've made a new type, so smash it */
    }  /*then*/
    else {  /* type name is supplied */
      if (semttname->UU.ttptr->stsort != iftarray)
      {   /* name is not an array type */
	errorrec = newerrorptr(notarraytype);
	errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->
			       ndsrcline;
	semerror(errorrec);
	pusharray(noaddtottable);
	semttatype = popsemantic();
	atype = semttatype->UU.ttptr;
	atype->UU.stbasetype = getbasictype(ifbwild);
	atype = addtotypetable(atype);
	    /* we've made a new type, so smash it */
      } else  /* have OK name, no element */
	atype = semttname->UU.ttptr;
    }  /*else*/
  }  /*then*/
  else {  /* an element is supplied */
    if (semttname->UU.ttptr == NULL) {   /* no type name supplied */
      pusharray(noaddtottable);
      semttatype = popsemantic();
      atype = semttatype->UU.ttptr;
      atype->UU.stbasetype = semexpaelem->UU.explist->graphinfo->typeptr;
      atype = addtotypetable(atype);
	  /* we've made a new type, so smash it */
    } else {  /* have a name and an element */
      if (semttname->UU.ttptr->stsort != iftarray)
      {   /* type name isn't an array */
	errorrec = newerrorptr(notarraytype);
	errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->
			       ndsrcline;
	semerror(errorrec);
	pusharray(noaddtottable);
	semttatype = popsemantic();
	atype = semttatype->UU.ttptr;
	atype->UU.stbasetype = getbasictype(ifbwild);
	atype = addtotypetable(atype);
	    /* we've made a new type, so smash it */
      } else {  /* Check name and elem types are the same */
	if (semttname->UU.ttptr->UU.stbasetype !=
	    semexpaelem->UU.explist->graphinfo->typeptr) {
	  errorrec = newerrorptr(arraytypeclash);
	  errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->
				 ndsrcline;
	  semerror(errorrec);
	  pusharray(noaddtottable);
	  semttatype = popsemantic();
	  atype = semttatype->UU.ttptr;
	  atype->UU.stbasetype = semexpaelem->UU.explist->graphinfo->typeptr;
	  if (atype == NULL)
	    printf("atype is NIL inside\n");
	  atype = addtotypetable(atype);
	      /*we've made a new type,so smash it*/
	}  /*then*/
	else
	  atype = semttname->UU.ttptr;
      }  /*else*/
    }  /*else*/
    /* now AType hold the array type, we have an element so connect it */
    element = semexpaelem->UU.explist;
    portnum = 2;
    while (element != NULL) {
      semexpabuild->UU.explist->graphinfo->port_ = portnum;
      if (element->graphinfo->typeptr != atype->UU.stbasetype) {
	errorrec = newerrorptr(abuildelemtype);
	errorrec->UU.elemnum = portnum - 1;
	errorrec->linenumber = semexpabuild->UU.explist->graphinfo->node_->
			       ndsrcline;
	semerror(errorrec);
      }
      addedge(element->graphinfo, semexpabuild->UU.explist->graphinfo);
      element = element->next;
      portnum++;
    }  /*while*/
  }  /*else*/
  /* push Graph Info for output of ABuild */
  semexpabuild->UU.explist->graphinfo->port_ = 1;
  semexpabuild->UU.explist->graphinfo->typeptr = atype;
  pushsemantic(semexpabuild);
  freesemantic(&semexpaelem);
  freesemantic(&semttname);
}


Static Void doarrayindex()
{
  /*action 109 DoArrayIndex*/
  /*stackpops - tgexplist  (for indexing expression)
                         tgexplist  (for primary)
                  pushes - tgexplist  (for output of AElements)*/
  /*description
          Check that the type of <PRIMARY> is array and has arity = 1.
          Get an AElements and connect <PRIMARY> to it.  For each expression
          in the expression list, do the following:
          - Check that type of the expression is integer.
          - Connect the expression to the next input port in the AElements node.
          - Set the type of the output port to the base type of the last
            dimension indexed.
          - Save the graph info for the output port on the stack. */
  semanticrec *semindexexp, *semarray, *semttatype;
  errorrecord *errorrec;
  inforecord *agraphinfo;
  stentry *atype;
  inforecord *aelemgi;
  int portnum;
  explistnode *indexexp;

  if (semtrace)
    printf("begin DoArrayIndex\n");
  semindexexp = popsemantic();
  semarray = popsemantic();
  if (semarray->UU.explist->next != NULL) {
    errorrec = newerrorptr(arrayarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    semarray->UU.explist->next = NULL;
  }
  if (semarray->UU.explist->graphinfo->typeptr == NULL) {
    pusharray(noaddtottable);
    semttatype = popsemantic();
    atype = semttatype->UU.ttptr;
    atype->UU.stbasetype = getbasictype(ifbwild);
    atype = addtotypetable(atype);   /* we've made a new type, so smash it */
    semarray->UU.explist->graphinfo->typeptr = atype;
  }
  if (semarray->UU.explist->graphinfo->typeptr->stsort != iftarray) {
    errorrec = newerrorptr(needarraytype);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    pusharray(noaddtottable);
    semttatype = popsemantic();
    atype = semttatype->UU.ttptr;
    atype->UU.stbasetype = getbasictype(ifbwild);
    atype = addtotypetable(atype);   /* we've made a new type, so smash it */
    semarray->UU.explist->graphinfo->typeptr = atype;
  } else
    atype = semarray->UU.explist->graphinfo->typeptr;
  agraphinfo = semarray->UU.explist->graphinfo;
  indexexp = semindexexp->UU.explist;
  portnum = 1;
  aelemgi = newinfoptr();   /*connect the array to port 1*/
  aelemgi->node_ = newnodeptr(ndatomic, currentlevel);
  aelemgi->node_->ndcode = ifnaelement;
  aelemgi->node_->ndsrcline = linenumber;
  linkparent(aelemgi->node_, currentlevel);
  aelemgi->port_ = 1;
  addedge(agraphinfo, aelemgi);
  while (indexexp != NULL) {
    portnum++;
    if (indexexp->graphinfo->typeptr == NULL)
      indexexp->graphinfo->typeptr = getbasictype(ifbwild);
    if (indexexp->graphinfo->typeptr->stlabel != intlabel) {
      errorrec = newerrorptr(indexexperr);
      errorrec->linenumber = linenumber;
      semerror(errorrec);
    }
    if (atype->stsort != iftarray) {
      errorrec = newerrorptr(needarraytype);
      errorrec->linenumber = linenumber;
      semerror(errorrec);
      indexexp->next = NULL;   /*End the while loop*/
    } else
      atype = atype->UU.stbasetype;
    aelemgi->port_ = portnum;
    addedge(indexexp->graphinfo, aelemgi);
    indexexp = indexexp->next;
    aelemgi->typeptr = atype;
  }  /*while*/
  aelemgi->port_ = 1;
  semarray->UU.explist->graphinfo = aelemgi;
  pushsemantic(semarray);
  freesemantic(&semindexexp);
}


Static Void doarrayreplace()
{
  /*action 110 DoArrayReplace*/
  /*stackpops - tgexplist  (element to replace)
                         tgexplist  (indices to replace at)
                         tgexplist  (array to replace them in)
                  pushes - tgexplist  (for replaced array)*/
  /*description
          Check that the graph info for array really has the type of array
          and it's arity should be 1.  Get an AReplaceN node for the first
          expression on list1.  Connect the ports as follows; input port 1
          = array, input port 2 = # of expressions in list1 (check that
          each's type is integer).  Connect each expression on list1 to the
          next input port on the AReplaceN.  Next check that the type of all
          the expressions on list2 are the same as the base type of the array.
          Connect them to the next available input ports.  Save the graphinfo
          for the output of the AReplaceN node on the stack.  The type should
          be the same as the type coming in on port 1.*/
  semanticrec *semlist1, *semlist2, *semarray, *semttatype;
  errorrecord *errorrec;
  inforecord *replacegi, *numindiceslit;
  int portnum;
  stentry *atype;
  explistnode *explist;

  if (semtrace)
    printf("Begin DoArrayReplace\n");
  semlist2 = popsemantic();
  semlist1 = popsemantic();
  semarray = popsemantic();
  if (semarray->UU.explist->next != NULL) {
    errorrec = newerrorptr(arityareplace);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    semarray->UU.explist->next = NULL;
  }  /*then*/
  if (semarray->UU.explist->graphinfo->typeptr == NULL) {
    pusharray(noaddtottable);
    semttatype = popsemantic();
    semttatype->UU.ttptr->UU.stbasetype = getbasictype(ifbwild);
    semarray->UU.explist->graphinfo->typeptr = addtotypetable(semttatype->UU.ttptr);
  }  /*then*/
  if (semarray->UU.explist->graphinfo->typeptr->stsort != iftarray) {
    errorrec = newerrorptr(typeareplace);
    errorrec->linenumber = linenumber;
    pusharray(noaddtottable);
    semttatype = popsemantic();
    semttatype->UU.ttptr->UU.stbasetype = getbasictype(ifbwild);
    semarray->UU.explist->graphinfo->typeptr = addtotypetable(semttatype->UU.ttptr);
  }  /*then*/
  replacegi = newinfoptr();
  replacegi->node_ = newnodeptr(ndatomic, currentlevel);
  replacegi->node_->ndsrcline = linenumber;
  replacegi->node_->ndcode = ifnareplacen;
  linkparent(replacegi->node_, currentlevel);
  replacegi->port_ = 1;
  replacegi->typeptr = semarray->UU.explist->graphinfo->typeptr;
  addedge(semarray->UU.explist->graphinfo, replacegi);
  portnum = 3;
  atype = semarray->UU.explist->graphinfo->typeptr;
  explist = semlist1->UU.explist;
  while (explist != NULL) {
    if (atype->stsort != iftarray) {
      errorrec = newerrorptr(replaceoverdim);
      errorrec->linenumber = linenumber;
      semerror(errorrec);
      explist = NULL;   /*end the loop*/
      continue;
    }
    atype = atype->UU.stbasetype;
    if (explist->graphinfo->typeptr == NULL)
      explist->graphinfo->typeptr = getbasictype(ifbwild);
    if (explist->graphinfo->typeptr->stlabel != intlabel) {
      errorrec = newerrorptr(indexexperr);
      errorrec->linenumber = linenumber;
      semerror(errorrec);
      explist->graphinfo = getalit1();
    }  /*then*/
    replacegi->port_ = portnum;
    addedge(explist->graphinfo, replacegi);
    portnum++;
    explist = explist->next;
  }  /*while*/
  numindiceslit = newinfoptr();
  numindiceslit->typeptr = getbasictype(ifbinteger);
  integerstring(&numindiceslit->litvalue, portnum - 3, 4);
  while (stringchar(&numindiceslit->litvalue, 1) == ' ')
    deletestring(&numindiceslit->litvalue, 1, 1);
  replacegi->port_ = 2;
  addedge(numindiceslit, replacegi);
  explist = semlist2->UU.explist;
  while (explist != NULL) {
    if (explist->graphinfo->typeptr != atype) {
      errorrec = newerrorptr(replaceelemtype);
      errorrec->linenumber = linenumber;
      semerror(errorrec);
    }
    replacegi->port_ = portnum;
    addedge(explist->graphinfo, replacegi);
    portnum++;
    explist = explist->next;
  }  /*while*/
  replacegi->port_ = 1;
  semarray->UU.explist->graphinfo = replacegi;
  pushsemantic(semarray);
  freesemantic(&semlist1);
  freesemantic(&semlist2);

  /*else*/
}


Static Void startsbuild()
{
  /*action 100 StartSBuild                                                     */
  /*stack           pops - nothing
                    pushes - TGNodePtr                                         */
  /* description
            Create an ABuild node. Wire a '1' to input port one.  push node on
           stack.                                                              */
  node *abnode;
  inforecord *lit, *toinfo;
  semanticrec *nsem;

  if (semtrace)
    printf("In StartSBuild \n");
  abnode = newnodeptr(ndatomic, currentlevel);
  abnode->ndcode = ifnabuild;
  abnode->ndsrcline = linenumber;
  linkparent(abnode, currentlevel);
  lit = newinfoptr();
  string10(&lit->litvalue, "1         ");
  stripspaces(&lit->litvalue);
  lit->typeptr = getbasictype(ifbinteger);
  toinfo = newinfoptr();
  toinfo->node_ = abnode;
  toinfo->port_ = 1;
  addedge(lit, toinfo);
  newsemantic(&nsem, tgnodeptr);
  nsem->UU.nodeptr = abnode;
  pushsemantic(nsem);
}


Static inforecord *createerr(stype)
stentry **stype;
{
  inforecord *info;

  info = newinfoptr();
  string10(&info->litvalue, "error     ");
  (*stype)->UU.stbasetype = getbasictype(ifbwild);
  *stype = addtotypetable(*stype);
  info->typeptr = *stype;
  return info;
}  /* CreateErr */


Static Void endsbuild(line, col)
int line, col;
{
  /*action  103 EndSBuild                                                     */
  /*stack           pops - TGExpList
                            TGNodePtr
                            TGTTPtr
                            TGTTPtr          */
  /*description
            Make sure that either the type name is not nil or the explist is
           not nil. If they are both not nil then make sure the base type of
           the stream is the same as the type of the expressions. Wire the
           expressions on consecutive input ports of the ABuild. Push the
           output graph info of the ABuild onto the stack.                     */
  semanticrec *expl, *abnodes, *stypes, *tnamep;
  explistnode *exp;
  node *abnode;
  stentry *stype, *ntype;
  inforecord *info, *toinfo;
  int port_;
  errorrecord *errorrec;

  if (semtrace)
    printf("In EndSBuild\n");
  expl = popsemantic();
  abnodes = popsemantic();
  stypes = popsemantic();
  tnamep = popsemantic();
  exp = expl->UU.explist;
  abnode = abnodes->UU.nodeptr;
  stype = stypes->UU.ttptr;
  if (exp == NULL) {
    if (tnamep->UU.ttptr == NULL) {
      errorrec = newerrorptr(nothingsc);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      info = createerr(&stype);
    }  /* if */
    else {
      if (tnamep->UU.ttptr->stsort != iftstream) {
	errorrec = newerrorptr(typenssc);
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	info = createerr(&stype);
      }  /* if */
      else {
	info = newinfoptr();
	info->node_ = abnode;
	info->port_ = 1;
	info->typeptr = tnamep->UU.ttptr;
      }  /* else */
    }  /* else */
  }  /* if exp = nil */
  else {
    toinfo = newinfoptr();
    toinfo->node_ = abnode;
    port_ = 2;
    ntype = NULL;
    while (exp != NULL) {
      if (ntype == NULL)
	ntype = exp->graphinfo->typeptr;
      if (ntype != exp->graphinfo->typeptr) {
	errorrec = newerrorptr(expsnstypesc);
	errorrec->UU.expnum = port_ - 1;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	info = createerr(&stype);
      }  /* if */
      else {
	toinfo->port_ = port_;
	addedge(exp->graphinfo, toinfo);
      }  /* else */
      port_++;
      exp = exp->next;
    }  /* while */
    stype->UU.stbasetype = ntype;
    stype = addtotypetable(stype);
    if (tnamep->UU.ttptr != NULL) {
      if (tnamep->UU.ttptr != stype) {
	errorrec = newerrorptr(typeclashsc);
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	stype = tnamep->UU.ttptr;
      }  /* if */
    }
    info = newinfoptr();
    info->node_ = abnode;
    info->port_ = 1;
    info->typeptr = stype;
  }  /* else exp <> nil */
  expl->UU.explist = newexplistptr();
  expl->UU.explist->graphinfo = info;
  pushsemantic(expl);
}


Static Void beforeexp()
{
  /*action 86 BeforeExp*/
  /*stackpops - nothing
                  pushes - tgpreclevel
                           tgexplist*/
  /*description
          Push an empty expression list (used to collect all the expressions
          because the arity may be greater than 1).  Also push a precedence
          level rec with preclevel = -1, this is used to force total reduction
          of the expression, -1 is lower than the prec level of any operator. */
  semanticrec *semexplist, *sempreclevel;

  if (semtrace)
    printf("Begin BeforeExp\n");
  newsemantic(&semexplist, tgexplist);
  newsemantic(&sempreclevel, tgpreclevel);
  sempreclevel->UU.U5.preclevel = -1;
  pushsemantic(semexplist);
  pushsemantic(sempreclevel);
}


Static Void incboostfac()
{
  /*action 87 IncBoostFac*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          We have seen a ( so increase the Boost Factor by 10.*/
  if (semtrace)
    printf("Begin IncBoostFac\n");
  boostfac += 10;
}


Static Void decboostfac()
{
  /*action 88 DecBoostFac*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          We have seen a rt paran so decrement the BoostFactor by 10.*/
  if (semtrace)
    printf("Begin DecBoostFac\n");
  boostfac -= 10;
}


Local Void buildunarynode(exp1list, oper, line)
explistnode *exp1list;
opertype oper;
int line;
{
  explistnode *expresult;
  semanticrec *semexplist;
  errorrecord *errorrec;
  inforecord *exp;

  if (semtrace)
    printf("Begin BuildUnaryNode\n");
  if (exp1list->next != NULL) {   /* check that arity is 1 */
    errorrec = newerrorptr(unaryoparity);
    errorrec->UU.op = oper;
    errorrec->linenumber = line;
    semerror(errorrec);
  }
  exp = exp1list->graphinfo;
  expresult = newexplistptr();
  /*  Check that the type of expression is valid for this operation  */
  switch (oper) {

  case uplus:
    if ((unsigned)exp->typeptr->stlabel >= 32 ||
	((1L << exp->typeptr->stlabel) & arithtypes) == 0) {
      errorrec = newerrorptr(unarytypeerr);
      errorrec->UU.U31.uop = oper;
      errorrec->UU.U31.utypeptr = exp->typeptr;
      semerror(errorrec);
    }  /*then*/
    break;

  case uminus:
    if ((unsigned)exp->typeptr->stlabel >= 32 ||
	((1L << exp->typeptr->stlabel) & arithtypes) == 0) {
      errorrec = newerrorptr(unarytypeerr);
      errorrec->UU.U31.uop = oper;
      errorrec->UU.U31.utypeptr = exp->typeptr;
      semerror(errorrec);
    }  /*then*/
    expresult->graphinfo->node_ = newnodeptr(ndatomic, currentlevel);
    expresult->graphinfo->port_ = 1;
    expresult->graphinfo->node_->ndsrcline = line;
    linkparent(expresult->graphinfo->node_, currentlevel);
    expresult->graphinfo->node_->ndcode = ifnneg;
    expresult->graphinfo->typeptr = exp->typeptr;
    break;

  case unot:
    if (exp->typeptr->stlabel != boollabel) {
      errorrec = newerrorptr(unarytypeerr);
      errorrec->UU.U31.uop = oper;
      errorrec->UU.U31.utypeptr = exp->typeptr;
      semerror(errorrec);
    }  /*then*/
    expresult->graphinfo->node_ = newnodeptr(ndatomic, currentlevel);
    expresult->graphinfo->port_ = 1;
    expresult->graphinfo->node_->ndsrcline = line;
    linkparent(expresult->graphinfo->node_, currentlevel);
    expresult->graphinfo->node_->ndcode = ifnnot;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;
  }/*case*/
  newsemantic(&semexplist, tgexplist);
  if (oper != uplus) {
    addedge(exp, expresult->graphinfo);
    semexplist->UU.explist = expresult;
  }  /*then*/
  else
    semexplist->UU.explist = exp1list;
  /*uplus doesn't generate a node
                                             so just push the input info */
  pushsemantic(semexplist);
}  /*BuildUnaryNode*/

Local Void buildbinarynode(exp1list, exp2list, oper, line)
explistnode *exp1list, *exp2list;
opertype oper;
int line;
{
  inforecord *exp1, *exp2;
  explistnode *expresult;
  semanticrec *semexplist;
  errorrecord *errorrec;

  if (semtrace)
    printf("begin BuildBinaryNode\n");
  /* Check arity of exps are 1 */
  if (exp1list->next != NULL || exp2list->next != NULL) {
    errorrec = newerrorptr(binaryoparity);
    errorrec->UU.op = oper;
    errorrec->linenumber = line;
    semerror(errorrec);
  }
  exp1 = exp1list->graphinfo;
  exp2 = exp2list->graphinfo;
  if (exp1->typeptr != exp2->typeptr) {   /* types much be equal */
    errorrec = newerrorptr(binarytypesnoteq);
    errorrec->UU.op = oper;
    errorrec->linenumber = line;
    semerror(errorrec);
  }
  /* Check that the types of the expressions are valid for the operation */
  switch (oper) {

  case plus:
  case minus:
  case mult:
  case divd:
    if ((unsigned)exp1->typeptr->stlabel >= 32 ||
	((1L << exp1->typeptr->stlabel) & arithtypes) == 0 ||
	(unsigned)exp2->typeptr->stlabel >= 32 ||
	((1L << exp2->typeptr->stlabel) & arithtypes) == 0) {
      errorrec = newerrorptr(binarytypeerr);
      errorrec->UU.U18.bop = oper;
      errorrec->UU.U18.btypeptr = exp1->typeptr;
      errorrec->linenumber = line;
      semerror(errorrec);
    }  /*then*/
    break;

  case concat:
    if (((1L << exp1->typeptr->stsort) & streamorarray) == 0 ||
	((1L << exp2->typeptr->stsort) & streamorarray) == 0) {
      errorrec = newerrorptr(binarytypeerr);
      errorrec->UU.U18.bop = oper;
      errorrec->UU.U18.btypeptr = exp1->typeptr;
      errorrec->linenumber = line;
      semerror(errorrec);
    }  /*then*/
    break;

  case lt:
  case le:
  case gt:
  case ge:
    if ((unsigned)exp1->typeptr->stlabel >= 32 ||
	((1L << exp1->typeptr->stlabel) & (arithtypes | (1L << charlabel))) == 0 ||
	(unsigned)exp2->typeptr->stlabel >= 32 ||
	((1L << exp2->typeptr->stlabel) & (arithtypes | (1L << charlabel))) == 0) {
      errorrec = newerrorptr(binarytypeerr);
      errorrec->UU.U18.bop = oper;
      errorrec->UU.U18.btypeptr = exp1->typeptr;
      errorrec->linenumber = line;
      semerror(errorrec);
    }  /*then*/
    break;

  case orop:
  case andop:
    if (exp1->typeptr->stlabel != boollabel ||
	exp2->typeptr->stlabel != boollabel) {
      errorrec = newerrorptr(binarytypeerr);
      errorrec->UU.U18.bop = oper;
      errorrec->UU.U18.btypeptr = exp1->typeptr;
      errorrec->linenumber = line;
      semerror(errorrec);
    }  /*then*/
    break;

  case equal:
  case notequal:
    if ((unsigned)exp1->typeptr->stlabel >= 32 ||
	((1L << exp1->typeptr->stlabel) & (algebtypes | (1L << charlabel))) == 0 ||
	(unsigned)exp2->typeptr->stlabel >= 32 ||
	((1L << exp2->typeptr->stlabel) & (algebtypes | (1L << charlabel))) == 0) {
      errorrec = newerrorptr(binarytypeerr);
      errorrec->UU.U18.bop = oper;
      errorrec->UU.U18.btypeptr = exp1->typeptr;
      errorrec->linenumber = line;
      semerror(errorrec);
    }
    break;
  }/*case*/
  expresult = newexplistptr();
  expresult->graphinfo->node_ = newnodeptr(ndatomic, currentlevel);
  expresult->graphinfo->port_ = 1;
  expresult->graphinfo->node_->ndsrcline = line;
  linkparent(expresult->graphinfo->node_, currentlevel);
  switch (oper) {

  case plus:
    expresult->graphinfo->node_->ndcode = ifnplus;
    expresult->graphinfo->typeptr = exp1->typeptr;
    break;

  case minus:
    expresult->graphinfo->node_->ndcode = ifnminus;
    expresult->graphinfo->typeptr = exp1->typeptr;
    break;

  case mult:
    expresult->graphinfo->node_->ndcode = ifntimes;
    expresult->graphinfo->typeptr = exp1->typeptr;
    break;

  case divd:
    expresult->graphinfo->node_->ndcode = ifndiv;
    expresult->graphinfo->typeptr = exp1->typeptr;
    break;

  case concat:
    expresult->graphinfo->node_->ndcode = ifnacatenate;
    expresult->graphinfo->typeptr = exp1->typeptr;
    break;

  case lt:
    expresult->graphinfo->node_->ndcode = ifnless;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case le:
    expresult->graphinfo->node_->ndcode = ifnlessequal;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case gt:
    expresult->graphinfo->node_->ndcode = ifnlessequal;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case ge:
    expresult->graphinfo->node_->ndcode = ifnless;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case andop:
    expresult->graphinfo->node_->ndcode = ifntimes;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case orop:
    expresult->graphinfo->node_->ndcode = ifnplus;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case equal:
    expresult->graphinfo->node_->ndcode = ifnequal;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;

  case notequal:
    expresult->graphinfo->node_->ndcode = ifnnotequal;
    expresult->graphinfo->typeptr = getbasictype(ifbboolean);
    break;
  }/*case*/
  addedge(exp2, expresult->graphinfo);
  expresult->graphinfo->port_ = 2;
  addedge(exp1, expresult->graphinfo);
  expresult->graphinfo->port_ = 1;   /*now work with output port 1*/
  /* > and >= need a NOT node since they are built with  <= and < */
  if (oper == gt || oper == ge) {
    buildunarynode(expresult, unot, line);
    return;
  }
  newsemantic(&semexplist, tgexplist);
  semexplist->UU.explist = expresult;
  pushsemantic(semexplist);

  /*else*/
}  /*BuildBinaryNode*/


Static boolean Reduce()
{
  /* This function checks the precedence of the top 2 operations on the
     stack.  If the one lower on the stack has higher or equal precedence,
     then its node it output, otherwise nothing happens.  The boolean value
     returned tells whether a reduction occured or not.  If one did occur
     others operation nodes may also need to be built.  False is returned
     to denote that the expression may not be fully reduced.*/
  boolean Result;
  semanticrec *oper1, *exp1, *oper2, *exp2;

  if (semtrace)
    printf("begin Reduce\n");
  oper1 = popsemantic();
  exp1 = popsemantic();
  oper2 = popsemantic();
  /*  If operation lower on stack has lower precedence then do nothing and return
      true otherwise output that operation.*/
  if (oper2->UU.U5.preclevel < oper1->UU.U5.preclevel) {
    Result = true;
    pushsemantic(oper2);
    pushsemantic(exp1);
    pushsemantic(oper1);
    return Result;
  }  /*then*/
  else {
    if (exp1->UU.explist->graphinfo->typeptr == NULL)
      exp1->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
    if (((1L << ((long)oper2->UU.U5.operation)) & binaryops) != 0) {
      exp2 = popsemantic();
      if (exp2->UU.explist->graphinfo->typeptr == NULL)
	exp2->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
      buildbinarynode(exp1->UU.explist, exp2->UU.explist,
		      oper2->UU.U5.operation, oper2->UU.U5.line);
      pushsemantic(oper1);
      freesemantic(&exp1);
      freesemantic(&oper2);
      freesemantic(&exp2);
      return false;
    }  /*then*/
    buildunarynode(exp1->UU.explist, oper2->UU.U5.operation,
		   oper2->UU.U5.line);
    pushsemantic(oper1);
    freesemantic(&oper2);
    freesemantic(&exp1);
    return false;
  }  /*else*/

}  /*Reduce*/


Static Void reduceexp()
{
  /*action 89 ReduceExp*/
  /*stackpops - All tgexplist and tgpreclevel down to tgpreclevel
                         with preclevel < current boost factor.
                  pushes - tgexplist*/
  /*description
          Push the current boost factor on the stack and call Reduce.  This
          will cause all operator with precedence levels > the pushed
          boost factor to be generated.  Then pop off the tgprcelevel
          on the top of the stack.*/
  boolean reduced;
  semanticrec *semboostfac;

  if (semtrace)
    printf("Begin ReduceExp\n");
  newsemantic(&semboostfac, tgpreclevel);
  semboostfac->UU.U5.preclevel = boostfac;
  pushsemantic(semboostfac);
  reduced = false;
  while (!reduced)
    reduced = Reduce();
  semboostfac = popsemantic();
  freesemantic(&semboostfac);
}


Static Void afterexp()
{
  /*action 91 AfterExp*/
  /*stackpops - tgexplist (single expression)
                         tgpreclevel
                         tgexplist (expression list)
                  pushes - tgpreclevel
                           tgexplist*/
  /*description
          Add the single expression's graphinfo to the end of the expression
          list.  Push the new list and the preclevel back on the stack.  This
          is how expressions of arity > 1 are collected.*/
  semanticrec *semexp, *sempreclevel, *semexplist;

  if (semtrace)
    printf("Begin AfterExp\n");
  semexp = popsemantic();
  sempreclevel = popsemantic();
  semexplist = popsemantic();
  addtoexplist(&semexplist->UU.explist, semexp->UU.explist);
  freesemantic(&semexp);
  pushsemantic(semexplist);
  pushsemantic(sempreclevel);
}


Static Void aftersimpexp()
{
  /*action 144 AfterSimpExp*/
  /*stackpops - tgexplist (single expression)
                         tgpreclevel
                         tgexplist (expression list)
                  pushes - tgexplist*/
  /*description
          Add the single expression's graphinfo to the end of the expression
          list.  Same as AfterExp accept the preclevel isn't pushed back
          on to the stack.  This is becasue a simple expression will only
          have arity one, so we know the whole expression is done at this
          point.  The preclevel is needed on the stack to separate expressions
          that are separated by commas.*/
  semanticrec *semexp, *sempreclevel, *semexplist;

  if (semtrace)
    printf("Begin AfterSimpExp\n");
  semexp = popsemantic();
  sempreclevel = popsemantic();
  semexplist = popsemantic();
  addtoexplist(&semexplist->UU.explist, semexp->UU.explist);
  freesemantic(&semexp);
  freesemantic(&sempreclevel);
  pushsemantic(semexplist);
}


Static Void dobinaryop(token, line, col)
int token, line, col;
{
  /*action 112 DoBinaryOp*/
  /*stackpops - nothing
                  pushes - tgpreclevel*/
  /*description
          Push the operator and it's precedence level on the stack.*/
  semanticrec *sempreclevel;
  boolean reduced;

  if (semtrace)
    printf("begin DoBinaryOp\n");
  reduced = false;
  newsemantic(&sempreclevel, tgpreclevel);
  sempreclevel->UU.U5.line = line;
  sempreclevel->UU.U5.column = col;
  switch (token) {

  case 1:
    sempreclevel->UU.U5.operation = andop;
    sempreclevel->UU.U5.preclevel = boostfac + 2;
    break;

  case 4:
    sempreclevel->UU.U5.operation = mult;
    sempreclevel->UU.U5.preclevel = boostfac + 7;
    break;

  case 5:
    sempreclevel->UU.U5.operation = plus;
    sempreclevel->UU.U5.preclevel = boostfac + 6;
    break;

  case 7:
    sempreclevel->UU.U5.operation = minus;
    sempreclevel->UU.U5.preclevel = boostfac + 6;
    break;

  case 9:
    sempreclevel->UU.U5.operation = divd;
    sempreclevel->UU.U5.preclevel = boostfac + 7;
    break;

  case 18:
    sempreclevel->UU.U5.operation = equal;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 74:
    sempreclevel->UU.U5.operation = ge;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 75:
    sempreclevel->UU.U5.operation = gt;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 76:
    sempreclevel->UU.U5.operation = le;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 77:
    sempreclevel->UU.U5.operation = lt;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 79:
    sempreclevel->UU.U5.operation = notequal;
    sempreclevel->UU.U5.preclevel = boostfac + 4;
    break;

  case 80:
    sempreclevel->UU.U5.operation = orop;
    sempreclevel->UU.U5.preclevel = boostfac + 1;
    break;

  case 81:
    sempreclevel->UU.U5.operation = concat;
    sempreclevel->UU.U5.preclevel = boostfac + 5;
    break;
  }/*case*/
  pushsemantic(sempreclevel);
  while (!reduced)
    reduced = Reduce();
}


Static Void poppreclevel()
{
  /*action 90 PopPrecLevel*/
  /*stackpops tgpreclevel
                  pushes - nothing*/
  /*description
          Pops a tgpreclevel off the stack.  This is the one pushed by
          BeforeExp.*/
  semanticrec *sempreclevel;

  sempreclevel = popsemantic();
  freesemantic(&sempreclevel);
}


Static Void findfieldname(name, fields, found, place)
stryng name;
stentry **fields;
boolean *found;
int *place;
{
  stryng name1, name2;

  name1 = name;
  stringlowercase(&name1);
  *found = false;
  *place = 1;
  while (!*found && *fields != NULL) {
    (*place)++;
    name2 = (*fields)->stliteral;
    stringlowercase(&name2);
    if (equalstrings(&name1, &name2))
      *found = true;
    else
      *fields = (*fields)->UU.U2.stnext;
  }  /* while */
}  /* FindFieldName */


Static inforecord *createerrlit()
{
  inforecord *info;

  info = newinfoptr();
  string10(&info->litvalue, "error     ");
  info->typeptr = getbasictype(ifbwild);
  return info;
}  /* CreateErrLit */


Static Void dofieldlist(line, col)
int line, col;
{
  /*action 92 DoFieldList                                                     */
  /*stack             pops - TGNameList
                             TGExpList
                      pushes - TGExpList                                      */
  /* description
          For each name on the list make sure it is a field of the current
         record. Create a relements node. Select the output corresponding
         to the name.  Push the output info for the last name.               */
  semanticrec *namel, *expl;
  namelistrec *name;
  explistnode *exp;
  inforecord *info, *toinfo;
  boolean found;
  int place;
  stentry *fields;
  node *renode;
  errorrecord *errorrec;

  if (semtrace)
    printf("In DoFieldList \n");
  namel = popsemantic();
  expl = popsemantic();
  name = namel->UU.namelist;
  exp = expl->UU.explist;
  if (exp->next != NULL) {
    exp->next = NULL;
    errorrec = newerrorptr(arityonrselect);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftrecord) {
    info = createerrlit();
    errorrec = newerrorptr(etypeonrs);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  } else {
    info = exp->graphinfo;
    while (name != NULL) {
      fields = info->typeptr->UU.stbasetype;
      findfieldname(name->name, &fields, &found, &place);
      if (!found) {
	info = createerrlit();
	errorrec = newerrorptr(nameundrs);
	errorrec->UU.errorstr = name->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	name = NULL;
	continue;
      }  /* if */
      if (name->next != NULL && fields->UU.U2.stelemtype->stsort != iftrecord) {
	info = createerrlit();
	errorrec = newerrorptr(ntypeonrs);
	errorrec->UU.errorstr = name->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	name = NULL;
	continue;
      }  /* else if */
      renode = newnodeptr(ndatomic, currentlevel);
      renode->ndcode = ifnrelements;
      renode->ndsrcline = line;
      linkparent(renode, currentlevel);
      toinfo = newinfoptr();
      toinfo->node_ = renode;
      toinfo->port_ = 1;
      addedge(info, toinfo);
      info = newinfoptr();
      info->node_ = renode;
      info->port_ = place - 1;
      info->typeptr = fields->UU.stbasetype;
      name = name->next;
    }  /* while */
  }  /* else */
  exp->graphinfo = info;
  pushsemantic(expl);

  /* else */
}  /* DoFieldList */


Static Void startrbuild()
{
  /*action 95 StartRBuild                                                     */
  /*stack             pops - nothing
                      pushes - TGExpList                                      */
  /*description
           Create a RBuild node. Push it's graph information onto the stack.  */
  node *rbuildnode;
  semanticrec *sem;
  inforecord *info;
  explistnode *nexp;

  if (semtrace)
    printf("In StartRBuild \n");
  rbuildnode = newnodeptr(ndatomic, currentlevel);
  rbuildnode->ndcode = ifnrbuild;
  rbuildnode->ndsrcline = linenumber;
  linkparent(rbuildnode, currentlevel);
  info = newinfoptr();
  info->node_ = rbuildnode;
  info->port_ = 1;
  nexp = newexplistptr();
  nexp->graphinfo = info;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = nexp;
  pushsemantic(sem);
}


Static Void buildfieldlist(line, col)
int line, col;
{
  /*action 97 BuildFieldList                                                  */
  /*stack             pops - TGExpList
                             TGNameList
                             TGExpList
                             TGTTPtr
                      pushes - TGExpList                                      */
  /* description
           Create a new field of the record with type from explist and name
          from namelist.  Make sure the name is unique in this record. Connec
          expression to the next input port of the RBuild. Push the RBuild
          graph inof onto the stack.                                          */
  semanticrec *expl, *namel, *rnode, *ttstruct, *tsem;
  errorrecord *errorrec;

  if (semtrace)
    printf("In BuildFieldList \n");
  expl = popsemantic();
  namel = popsemantic();
  rnode = popsemantic();
  ttstruct = popsemantic();
  if (expl->UU.explist->next != NULL) {
    expl->UU.explist->next = NULL;
    errorrec = newerrorptr(arityonrbuild);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if*/
  newsemantic(&tsem, tgttptr);
  tsem->UU.ttptr = expl->UU.explist->graphinfo->typeptr;
  pushsemantic(ttstruct);
  pushsemantic(namel);
  pushsemantic(tsem);
  buildfield(noaddtottable);
  addedge(expl->UU.explist->graphinfo, rnode->UU.explist->graphinfo);
  rnode->UU.explist->graphinfo->port_++;
  pushsemantic(rnode);
}


Static Void endrbuild(line, col)
int line, col;
{
  /*action 98 EndRBuild                                                       */
  /*stack             pops - TGExpList
                             TGTTPtr
                             TGTTPtr
                      pushes - TGExpList                                      */
  /* description
           Smash the record stucture into the type table. If the type name
          pointer is not nil then the pointer returned from the samsh must
          be equal to the type name pointer else error.  Save the output graph
          info of the RBuild node on the stack.                               */
  semanticrec *rnode, *ttstruct, *tnamel;
  errorrecord *errorrec;
  stentry *intable;

  if (semtrace)
    printf("In EndRBuild \n");
  rnode = popsemantic();
  ttstruct = popsemantic();
  tnamel = popsemantic();
  intable = addtotypetable(ttstruct->UU.ttptr);
  if (tnamel->UU.ttptr != NULL) {
    if (tnamel->UU.ttptr != intable) {
      errorrec = newerrorptr(typeonrbuild);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
  }
  rnode->UU.explist->graphinfo->typeptr = intable;
  rnode->UU.explist->graphinfo->port_ = 1;
  pushsemantic(rnode);
}


Static Void pushniltag()
{
  /*action  105  PushNilTag                                                    */
  /*stack            pops - nothing
                    pushes - TGExpList                                        */
  /*description
          Create a nil tag type stucture and push it on the stack.            */
  semanticrec *sem;
  explistnode *exp;
  inforecord *info;

  if (semtrace)
    printf("In PushNilTag \n");
  exp = newexplistptr();
  info = newinfoptr();
  info->typeptr = getbasictype(ifbnull);
  string10(&info->litvalue, "nil       ");
  exp->graphinfo = info;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}


Local Void builderrunion(expl, namel, tnt)
semanticrec *expl, *namel, **tnt;
{
  semanticrec *ntsem;

  pushunion(noaddtottable);
  pushsemantic(namel);
  newsemantic(&ntsem, tgttptr);
  ntsem->UU.ttptr = expl->UU.explist->graphinfo->typeptr;
  pushsemantic(ntsem);
  buildtag(noaddtottable);
  ntsem = popsemantic();
  (*tnt)->UU.ttptr = addtotypetable(ntsem->UU.ttptr);
}  /* BuildErrUnion */


Static Void endubuild(line, col)
int line, col;
{
  /*action  106  EndUBuild                                                    */
  /*stack              pops - TGExpList
                              TGNameList
                              TGTTPtr
                       pushes - TGExpList                                     */
  /* description
           Make sure the type is a union. Search for the name on the tag list
          of the union.  If it is not there then error.  Check that found tag
          has the same type as expression. Connect the expression to the input
          port of the RBuild node corresponding to the correct tag. Push the
          output graph info of the RBuild node.                               */
  semanticrec *expl, *namel, *tnt;
  stentry *tags;
  boolean found;
  int place;
  stryng name1, name2;
  inforecord *rinfo, *ininfo, *toinfo;
  node *ubuildnode;
  errorrecord *errorrec;

  if (semtrace)
    printf(" In EndUBuild\n");
  expl = popsemantic();
  namel = popsemantic();
  tnt = popsemantic();
  if (expl->UU.explist->next != NULL) {
    expl->UU.explist->next = NULL;
    errorrec = newerrorptr(arityonubuild);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  if (tnt->UU.ttptr == getbasictype(ifbwild))
    builderrunion(expl, namel, &tnt);
  if (tnt->UU.ttptr->stsort != iftunion) {
    errorrec = newerrorptr(tnonubuild);
    errorrec->UU.errorstr = namel->UU.namelist->name;
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    builderrunion(expl, namel, &tnt);
  }  /* if */
  name1 = namel->UU.namelist->name;
  stringlowercase(&name1);
  tags = tnt->UU.ttptr->UU.stbasetype;
  found = false;
  place = 0;
  while (!found) {
    place++;
    name2 = tags->stliteral;
    stringlowercase(&name2);
    if (equalstrings(&name1, &name2)) {
      found = true;
      break;
    }
    tags = tags->UU.U2.stnext;
    if (tags == NULL) {
      found = true;
      place = 0;
    }  /* if */
  }  /* while */
  if (place == 0) {
    rinfo = newinfoptr();
    string10(&rinfo->litvalue, "error     ");
    rinfo->typeptr = tnt->UU.ttptr;
    errorrec = newerrorptr(undeftagub);
    errorrec->UU.errorstr = namel->UU.namelist->name;
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  else {
    ubuildnode = newnodeptr(ndatomic, currentlevel);
    ubuildnode->ndcode = ifnrbuild;
    linkparent(ubuildnode, currentlevel);
    ininfo = newinfoptr();
    if (expl->UU.explist->graphinfo->typeptr != tags->UU.U2.stelemtype) {
      string10(&ininfo->litvalue, "error     ");
      ininfo->typeptr = tags->UU.U2.stelemtype;
      errorrec = newerrorptr(typeclashub);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
    else
      ininfo = expl->UU.explist->graphinfo;
    toinfo = newinfoptr();
    toinfo->node_ = ubuildnode;
    toinfo->port_ = place;
    addedge(ininfo, toinfo);
    rinfo = newinfoptr();
    rinfo->node_ = ubuildnode;
    rinfo->port_ = 1;
    rinfo->typeptr = tnt->UU.ttptr;
  }  /* else */
  expl->UU.explist->graphinfo = rinfo;
  pushsemantic(expl);

  /* else */
}  /* EndUBuild */


typedef struct ilistrec {
  inforecord *graphinfo;
  struct ilistrec *next;
} ilistrec;

typedef struct plistrec {
  int position;
  struct plistrec *next;
} plistrec;


Local Void pushinfolist(infol, newinfo)
ilistrec **infol;
inforecord *newinfo;
{
  ilistrec *newl;

  newl = (ilistrec *)Malloc(sizeof(ilistrec));
  newl->graphinfo = newinfo;
  newl->next = *infol;
  *infol = newl;
}  /* PushInfoList */

Local inforecord *popinfolist(infol)
ilistrec **infol;
{
  inforecord *Result;

  Result = (*infol)->graphinfo;
  *infol = (*infol)->next;
  return Result;
}  /* PopInfoList */

Local Void pushpositionlist(positionl, newpos)
plistrec **positionl;
int newpos;
{
  plistrec *newl;

  newl = (plistrec *)Malloc(sizeof(plistrec));
  newl->position = newpos;
  newl->next = *positionl;
  *positionl = newl;
}  /* PushPositionList */

Local int poppositionlist(positionl)
plistrec **positionl;
{
  int Result;

  Result = (*positionl)->position;
  *positionl = (*positionl)->next;
  return Result;
}  /* PopPositionList */


Static Void buildreplace(line, col)
int line, col;
{  /* Build Replace */
  /*action  111  BuildReplace                                                 */
  /*stack              pops - TGExpList
                              TGNameList
                              TGExpList
                       pushes - TGExpList                                     */
  /* description
           For each name on the name list except for last one:
                - make sure the current type is a record
                - search for name as a field of that record
                - create a RElements node, record as input to port one
                - extract name from RElements node, make this the current type
                - save RElements graphinfo and location of name in record
           For last name
                - make sure the current type is a record
                - search for name as a field of that record
                - check that expression is of the same type as field
                - create a RReplace node, express input to corresponding port
           While saved lists are not nil
                - Build an RReplace node
                - connect graphinfo to node at port location
           push last output info on to stack */
  semanticrec *rexpl, *namel, *oexpl;
  explistnode *rexp, *oexp;
  namelistrec *names;
  inforecord *cinfo, *toinfo, *frinfo;
  stentry *fields, *save;
  boolean found, error;
  node *renode, *rrnode;
  int place;
  plistrec *positionl;
  ilistrec *infol;
  errorrecord *errorrec;

  if (semtrace)
    printf("In BuildReplace \n");
  positionl = NULL;
  infol = NULL;
  rexpl = popsemantic();
  namel = popsemantic();
  oexpl = popsemantic();
  rexp = rexpl->UU.explist;
  names = namel->UU.namelist;
  oexp = oexpl->UU.explist;
  if (rexp->next != NULL) {
    rexp->next = NULL;
    errorrec = newerrorptr(arityonrexp);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  if (oexp->next != NULL) {
    oexp->next = NULL;
    errorrec = newerrorptr(arityonoexp);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  cinfo = oexp->graphinfo;
  error = false;
  if (cinfo->typeptr == NULL)
    cinfo->typeptr = getbasictype(ifbwild);
  while (names->next != NULL && !error) {
    pushinfolist(&infol, cinfo);
    if (cinfo->typeptr->stsort != iftrecord) {
      cinfo = createerrlit();
      errorrec = newerrorptr(ntyperr);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      error = true;
      continue;
    }  /* if */
    fields = cinfo->typeptr->UU.stbasetype;
    findfieldname(names->name, &fields, &found, &place);
    if (found == false) {
      cinfo = createerrlit();
      errorrec = newerrorptr(nameundrr);
      errorrec->UU.errorstr = names->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      error = true;
      continue;
    }  /* if */
    pushpositionlist(&positionl, place);
    renode = newnodeptr(ndatomic, currentlevel);
    renode->ndcode = ifnrelements;
    linkparent(renode, currentlevel);
    toinfo = newinfoptr();
    toinfo->node_ = renode;
    toinfo->port_ = 1;
    addedge(cinfo, toinfo);
    cinfo = toinfo;
    cinfo->port_ = place - 1;
    cinfo->typeptr = fields->UU.U2.stelemtype;
    names = names->next;
  }  /* while */
  if (cinfo->typeptr->stsort != iftrecord) {
    cinfo = createerrlit();
    errorrec = newerrorptr(ntyperr);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    error = true;
  }  /* if */
  if (!error) {
    fields = cinfo->typeptr->UU.stbasetype;
    findfieldname(names->name, &fields, &found, &place);
    if (!found) {
      cinfo = createerrlit();
      errorrec = newerrorptr(nameundrr);
      errorrec->UU.errorstr = names->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
    else {
      rrnode = newnodeptr(ndatomic, currentlevel);
      rrnode->ndcode = ifnrreplace;
      linkparent(rrnode, currentlevel);
      toinfo = newinfoptr();
      toinfo->node_ = rrnode;
      toinfo->port_ = 1;
      addedge(cinfo, toinfo);
      toinfo->port_ = place;
      if (rexp->graphinfo->typeptr != fields->UU.U2.stelemtype) {
	errorrec = newerrorptr(typeclashrr);
	errorrec->UU.errorstr = names->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
	rexp->graphinfo = newinfoptr();
	string10(&rexp->graphinfo->litvalue, "error     ");
	rexp->graphinfo->typeptr = fields->UU.U2.stelemtype;
      }  /* if */
      addedge(rexp->graphinfo, toinfo);
      save = cinfo->typeptr;
      cinfo = newinfoptr();
      cinfo->node_ = rrnode;
      cinfo->port_ = 1;
      cinfo->typeptr = save;
      freeinfoptr(&toinfo);
      while (infol != NULL) {
	rrnode = newnodeptr(ndatomic, currentlevel);
	rrnode->ndcode = ifnrreplace;
	linkparent(rrnode, currentlevel);
	toinfo = newinfoptr();
	toinfo->node_ = rrnode;
	toinfo->port_ = poppositionlist(&positionl);
	addedge(cinfo, toinfo);
	frinfo = popinfolist(&infol);
	toinfo->port_ = 1;
	addedge(frinfo, toinfo);
	cinfo = toinfo;
	cinfo->typeptr = frinfo->typeptr;
      }  /* while */
    }  /* else */
  }  /* if */
  rexp->graphinfo = cinfo;
  pushsemantic(rexpl);

  /* else */
  /* else */
}


Static Void popsymlist()
{
  /*action 113 PopSymList                                                   */
  /*stack          pops - TGSymList
                   pushes - nothing                                         */
  /* description
          Pop the symble table list off the stack                           */
  semanticrec *syml;

  if (semtrace)
    printf("In PopSymList\n");
  syml = popsemantic();
  freesemantic(&syml);
}  /* PopSymList */


Static Void startsymlist()
{
  /*action 32 StartSymList                                                    */
  /*stack          pops - nothing
                   pushes - TGSymList                                         */
  /* description
           Create a new semantic record of type symlist and push it onto the
           stack.                                                              */
  semanticrec *sem;

  if (semtrace)
    printf("In StartSymList\n");
  newsemantic(&sem, tgsymlist);
  pushsemantic(sem);
}  /* StartSymList */


Static Void checkatypes(line, col)
int line, col;
{
  /*action 33 CheckATypes                                                     */
  /*stack          pops - TGExpList
                          TGSymList
                   pushes - nothing                                           */
  /* description
           Follow the expression and symbol table pointer list pairwise and
          for each pair :
                 - make sure the types are the same
                 - insert definition line number in symbol table, if already
                   defined then error except for loop names which get
                   special treatment (see code)
                 - set the graph info of the symbol table entry to that of
                   expression
           make sure the lists are both the same length                       */
  semanticrec *exps, *syms;
  explistnode *expl;
  symlistnode *syml;
  symtblbucket *sym;
  loopvarflags vartype;
  errorrecord *errorrec;

  if (semtrace)
    printf("In CheckATypes\n");
  exps = popsemantic();
  syms = popsemantic();
  expl = exps->UU.explist;
  syml = syms->UU.symlist;
  while (syml != NULL && expl != NULL) {
    sym = syml->sym;
    /* Types must be equal  */
    if (sym->typeptr != expl->graphinfo->typeptr) {
      errorrec = newerrorptr(asstypeclash);
      errorrec->UU.U21.exptype = expl->graphinfo->typeptr;
      errorrec->UU.U21.nametype = sym->typeptr;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
    /* find out if this variable is "involved" with a forinit loop  */
    if (sym->identtag == tvariable)
      vartype = getloopflag(sym);
    else
      vartype = lnone;
    /* If it is a loop constant then error, can't be reassigned  */
    if (vartype == lconst) {
      errorrec = newerrorptr(loopconsassn);
      errorrec->UU.errorstr = sym->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
    else if (vartype == linit) {
      *sym->UU.U4.loopflag = lvar;
      vartype = lvar;
      sym->UU.U4.redefflag = true;
      sym->UU.U4.oldfrom = sym->graphinfo;
      insertchar(&sym->UU.U4.oldfrom->name, '-', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'd', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'l', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'o', 1);
    } else if (vartype == lvar) {
      if (sym->importedflag) {
	errorrec = newerrorptr(redefimlvar);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
      }  /* if imported */
      else {
	if (!sym->UU.U4.redefflag) {
	  sym->UU.U4.redefflag = true;
	}  /* if */
      }  /* else */
      /* Else it is a loop var and we are at it's outer scope.  So set the
         redef flag if it's not already set.  We check for double
         redefinitions below.*/
    }
    /* Else if it was defined in the initpart of the forloop, it is now
       becoming a loop var since it is being redefined.  Copy the graphinfo
       to the OldFrom.  GraphInfo holds the new value and OldFrom holds the
       old value.*/
    /*  Check for a redefintion of a non loop var or a third definition of
        a loop var.*/
    if (getdefline(sym, 1) != -1) {
      if (getdefline(sym, 2) != -1 && vartype == lvar || vartype != lvar) {
	errorrec = newerrorptr(vardoubledef);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
      }  /* if */
      else  /* else assign the new definition location */
	setdefloc(sym, linenumber, -1);
    } else  /* else assign the new definition location */
      setdefloc(sym, linenumber, -1);
    /*  Fill in the new graphinfo  */
    sym->graphinfo = newinfoptr();
    sym->graphinfo->node_ = expl->graphinfo->node_;
    sym->graphinfo->port_ = expl->graphinfo->port_;
    sym->graphinfo->typeptr = expl->graphinfo->typeptr;
    sym->graphinfo->litvalue = expl->graphinfo->litvalue;
    sym->graphinfo->onlist = expl->graphinfo->onlist;
    sym->graphinfo->name = sym->name;
    expl = expl->next;
    syml = syml->next;
  }  /* while */
  if (expl != NULL) {  /* Check that there aren't extra exps */
    errorrec = newerrorptr(longexplist);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  /* If there are extra names, assign them error values  */
  if (syml == NULL)
    return;
  errorrec = newerrorptr(longnamelist);
  semerror(errorrec);
  while (syml != NULL) {
    sym = syml->sym;
    /* find out if this variable is "involved" with a forinit loop  */
    if (sym->identtag == tvariable)
      vartype = getloopflag(sym);
    else
      vartype = lnone;
    /* If it is a loop constant then error, can't be reassigned  */
    if (vartype == lconst) {
      errorrec = newerrorptr(loopconsassn);
      errorrec->UU.errorstr = sym->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    }  /* if */
    else if (vartype == linit) {
      *sym->UU.U4.loopflag = lvar;
      vartype = lvar;
      sym->UU.U4.redefflag = true;
      sym->UU.U4.oldfrom = sym->graphinfo;
      insertchar(&sym->UU.U4.oldfrom->name, '-', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'd', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'l', 1);
      insertchar(&sym->UU.U4.oldfrom->name, 'o', 1);
    } else if (vartype == lvar) {
      if (sym->importedflag) {
	errorrec = newerrorptr(redefimlvar);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
      }  /* if imported */
      else {
	if (!sym->UU.U4.redefflag) {
	  sym->UU.U4.redefflag = true;
	}  /* if */
      }  /* else */
      /* Else it is a loop var and we are at it's outer scope.  So set the
         redef flag if it's not already set.  We check for double
         redefinitions below.*/
    }
    /* Else if it was defined in the initpart of the forloop, it is now
       becoming a loop var since it is being redefined.  Copy the graphinfo
       to the OldFrom.  GraphInfo holds the new value and OldFrom holds the
       old value.*/
    /*  Check for a redefintion of a non loop var or a third definition of
        a loop var.*/
    if (getdefline(sym, 1) != -1) {
      if (getdefline(sym, 2) != -1 && vartype == lvar || vartype != lvar) {
	errorrec = newerrorptr(vardoubledef);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
      }  /* if */
      else  /* else assign the new definition location */
	setdefloc(sym, linenumber, -1);
    } else  /* else assign the new definition location */
      setdefloc(sym, linenumber, -1);
    /*  Fill in the new graphinfo  */
    sym->graphinfo = newinfoptr();
    sym->graphinfo->typeptr = sym->typeptr;
    string10(&sym->graphinfo->litvalue, "error     ");
    sym->graphinfo->name = sym->name;
    syml = syml->next;
  }  /* while */

  /* else if */
  /* Else if it is already a loop var and was pulled in from an outer
     scope, then error.  You can not redefine a loop var in an internal
     scope.  Only at the outer scope.*/
  /* else if */
  /* else if */
  /* Else if it is already a loop var and was pulled in from an outer
     scope, then error.  You can not redefine a loop var in an internal
     scope.  Only at the outer scope.*/
  /* else if */
}  /* CheckATypes */


Static Void checkarity()
{
  /*action 34 CheckArity                                                      */
  /*stack          pops - TGExpList
                          TGNameList
                          TGSymList
                   pushes - nothing                                           */
  /* description
           Follow the expression and name list pairwise and
          for each pair :
                 - look up name in symbol table, if it exists then
                   make sure the types are the same, else create a new symbol
                   table entry for name of type the expression.
                 - insert definition line number in symbol table, if already
                   defined then error except for loop names which get special
                   treatment (see code)
                 - set the graph info of the symbol table entry to that of
                   expression
           make sure the lists are both the same length                       */
  semanticrec *exps, *names /* , *syml*/;
  explistnode *expl;
  namelistrec *namel;
  symtblbucket *sym;
  loopvarflags vartype;
  errorrecord *errorrec;

  if (semtrace)
    printf("In CheckArity\n");
  exps = popsemantic();
  names = popsemantic();
  /* syml = */ (Void)popsemantic();
  expl = exps->UU.explist;
  namel = names->UU.namelist;
  while (namel != NULL && expl != NULL) {
    /* Find the name in the symbol table  */
    sym = incurrentlevel(namel->name, tvariable);
    if (sym != NULL) {
      /* Find out if name is involved with a for init loop  */
      if (sym->identtag == tvariable)
	vartype = getloopflag(sym);
      else
	vartype = lnone;
      /* If a loop constant then error, can't be reassigned  */
      if (vartype == lconst) {
	errorrec = newerrorptr(loopconsassn);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = namel->linenum;
	semerror(errorrec);
      }  /* if */
      else if (vartype == linit) {
	*sym->UU.U4.loopflag = lvar;
	vartype = lvar;
	sym->UU.U4.redefflag = true;
	sym->UU.U4.oldfrom = sym->graphinfo;
	insertchar(&sym->UU.U4.oldfrom->name, '-', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'd', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'l', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'o', 1);
      } else if (vartype == lvar) {
	if (sym->importedflag) {
	  errorrec = newerrorptr(redefimlvar);
	  errorrec->UU.errorstr = sym->name;
	  errorrec->linenumber = namel->linenum;
	  semerror(errorrec);
	}  /* if imported */
	else {
	  if (!sym->UU.U4.redefflag) {
	    sym->UU.U4.redefflag = true;
	  }  /* if */
	}  /* else */
	/*  Else it is a loop var and we are at the outer scope.  So set the
	    redef flag if it's not already set.  We check for double
	    redefinitions below.*/
      }
      /* if  in the initpart of a for loop make it a loop var now.
         Copy the graphinfo to the OldFrom.  Graphinfo hold the new value
         and OldFrom holds the old value.*/
      /* Check that the type of the name and the exp are the same */
      if (sym->typeptr != expl->graphinfo->typeptr) {
	errorrec = newerrorptr(asstypeclash);
	errorrec->UU.U21.exptype = expl->graphinfo->typeptr;
	errorrec->UU.U21.nametype = sym->typeptr;
	errorrec->linenumber = namel->linenum;
	semerror(errorrec);
      }  /* if */
    }  /* if */
    else {
      sym = getsymtabentry(namel->name, tvariable);
      sym->typeptr = expl->graphinfo->typeptr;
      addtotable(sym, currentlevel);
    }  /* else */
    /*  Else get a new symbol table entry for the name and set it's
        type to that of the expressionand put it in the sym tab.*/
    /*  Check for a redefinition of a non loop var or a third def of a
        loop var.*/
    if (getdefline(sym, 1) != -1) {
      if (getdefline(sym, 2) != -1 && vartype == lvar || vartype != lvar) {
	errorrec = newerrorptr(vardoubledef);
	errorrec->UU.errorstr = namel->name;
	errorrec->linenumber = namel->linenum;
	semerror(errorrec);
      }  /* if */
      else
	setdefloc(sym, namel->linenum, -1);
    } else
      setdefloc(sym, namel->linenum, -1);
    /* Fill in the new graph info  */
    sym->graphinfo = newinfoptr();
    sym->graphinfo->node_ = expl->graphinfo->node_;
    sym->graphinfo->port_ = expl->graphinfo->port_;
    sym->graphinfo->typeptr = expl->graphinfo->typeptr;
    sym->graphinfo->litvalue = expl->graphinfo->litvalue;
    sym->graphinfo->onlist = expl->graphinfo->onlist;
    sym->graphinfo->name = namel->name;
    namel = namel->next;
    expl = expl->next;
  }  /* while */
  if (expl != NULL) {  /* Check for extra expressions */
    errorrec = newerrorptr(longexplist);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }  /* if */
  if (namel == NULL) {  /* Check for extra names */
    return;
  }  /* if */
  errorrec = newerrorptr(longnamelist);
  errorrec->linenumber = linenumber;
  semerror(errorrec);
  while (namel != NULL) {
    sym = incurrentlevel(namel->name, tvariable);
    if (sym != NULL) {
      /* Find out if name is involved with a for init loop  */
      if (sym->identtag == tvariable)
	vartype = getloopflag(sym);
      else
	vartype = lnone;
      /* If a loop constant then error, can't be reassigned  */
      if (vartype == lconst) {
	errorrec = newerrorptr(loopconsassn);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = namel->linenum;
	semerror(errorrec);
      }  /* if */
      else if (vartype == linit) {
	*sym->UU.U4.loopflag = lvar;
	vartype = lvar;
	sym->UU.U4.redefflag = true;
	sym->UU.U4.oldfrom = sym->graphinfo;
	insertchar(&sym->UU.U4.oldfrom->name, '-', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'd', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'l', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'o', 1);
      } else if (vartype == lvar) {
	if (sym->importedflag) {
	  errorrec = newerrorptr(redefimlvar);
	  errorrec->UU.errorstr = sym->name;
	  errorrec->linenumber = namel->linenum;
	  semerror(errorrec);
	}  /* if imported */
	else {
	  if (!sym->UU.U4.redefflag) {
	    sym->UU.U4.redefflag = true;
	  }  /* if */
	}  /* else */
	/*  Else it is a loop var and we are at the outer scope.  So set the
	    redef flag if it's not already set.  We check for double
	    redefinitions below.*/
      }
      /* if  in the initpart of a for loop make it a loop var now.
         Copy the graphinfo to the OldFrom.  Graphinfo hold the new value
         and OldFrom holds the old value.*/
    }  /* if */
    else {
      sym = getsymtabentry(namel->name, tvariable);
      sym->typeptr = getbasictype(ifbwild);
      addtotable(sym, currentlevel);
    }  /* else */
    /*  Else get a new symbol table entry for the name and set it's
        type to that of the expressionand put it in the sym tab.*/
    /*  Check for a redefinition of a non loop var or a third def of a
        loop var.*/
    if (getdefline(sym, 1) != -1) {
      if (getdefline(sym, 2) != -1 && vartype == lvar || vartype != lvar) {
	errorrec = newerrorptr(vardoubledef);
	errorrec->UU.errorstr = namel->name;
	errorrec->linenumber = namel->linenum;
	semerror(errorrec);
      }  /* if */
      else
	setdefloc(sym, namel->linenum, -1);
    } else
      setdefloc(sym, namel->linenum, -1);
    /* Fill in the new graph info  */
    sym->graphinfo->typeptr = sym->typeptr;
    string10(&sym->graphinfo->litvalue, "error     ");
    sym->graphinfo->name = namel->name;
    setdefloc(sym, namel->linenum, -1);
    namel = namel->next;
  }  /* while */

  /* else if */
  /* Else if it is already a loop var and was pulled in from an outer
     scope, then error.  You can not redefine a loop var in an inner
     scope.  Only at the outer scope.*/
  /* else if */
  /* else if */
  /* Else if it is already a loop var and was pulled in from an outer
     scope, then error.  You can not redefine a loop var in an inner
     scope.  Only at the outer scope.*/
  /* else if */
}  /* CheckArity */


Static Void assoctypes()
{
  /*action 35 AssocTypes                                                      */
  /*stack          pops - TGTTPtr
                          TGNameList
                          TGSymList
                   pushes - TGSymList                                         */
  /*description
           Follow the name list and for each name :
                 - look up name in symbol table, if it exists then
                   make sure the type is the same as the TTPtr,
                   else create a new symbol table entry for name of type TTPtr
           make sure the lists are both the same length                       */
  semanticrec *atype, *names, *syml;
  namelistrec *namel;
  errorrecord *errorrec;
  symtblbucket *sym;
  symlistnode *symentry, *findend;

  if (semtrace)
    printf("In AssocTypes\n");
  atype = popsemantic();
  names = popsemantic();
  syml = popsemantic();
  namel = names->UU.namelist;
  while (namel != NULL) {
    sym = incurrentlevel(namel->name, tvariable);
    if (sym != NULL) {
      if (sym->typeptr != atype->UU.ttptr) {
	errorrec = newerrorptr(vardoubledecl);
	errorrec->UU.U25.varname = sym->name;
	errorrec->UU.U25.exp1type = atype->UU.ttptr;
	errorrec->UU.U25.exp2type = sym->typeptr;
	errorrec->linenumber = linenumber;
	semerror(errorrec);
      }  /* if */
    }  /* if */
    else {
      sym = getsymtabentry(namel->name, tvariable);
      sym->typeptr = atype->UU.ttptr;
      addtotable(sym, currentlevel);
    }  /* else */
    symentry = newsymlistptr();
    symentry->sym = sym;
    if (syml->UU.symlist != NULL) {
      findend = syml->UU.symlist;
      while (findend->next != NULL)
	findend = findend->next;
      findend->next = symentry;
    } else
      syml->UU.symlist = symentry;
    namel = namel->next;
  }  /* while */
  pushsemantic(syml);
}


Static Void errindexnames(indexnames, col)
namelistrec **indexnames;
int col;
{
  symtblbucket *sym;

  while (*indexnames != NULL) {
    sym = getsymtabentry((*indexnames)->name, tvariable);
    setdefloc(sym, (*indexnames)->linenum, col);
    sym->graphinfo = getalit1();
    sym->typeptr = getbasictype(ifbinteger);
    sym->graphinfo->name = (*indexnames)->name;
    addtotable(sym, currentlevel);
    *indexnames = (*indexnames)->next;
  }  /* while */
}  /* ErrIndexNames */


Static Void startbody()
{
  /*action 39  StartBody*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Create the body subgraph.  Save it in the CurrentSubN0.  Also
          set BodyN0 and BodyPrevNode to the new subgraph.  These are
          used so we can switch between the body and returns subgraphs
          when processing the returns parts.  Search the current symbol
          tables looking for values whose node is the generator subgraph.
          Change their node to the body subgraph since that's where they
          will now come from. If the value is a loop var also check its
          OldFrom node.  It may need to be changed too.*/
  node *genn0, *bodyn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("In StartBody\n");
  genn0 = currentlevel->currentsubn0;
  bodyn0 = buildsubgraph(currentlevel->UU.U0.forallnode);
  currentlevel->UU.U0.bodyprevnode = bodyn0;
  currentlevel->UU.U0.bodyn0 = bodyn0;
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (genn0 == chain->graphinfo->node_) {
	chain->graphinfo->node_ = bodyn0;
	if (chain->typeptr == NULL)
	  chain->typeptr = getbasictype(ifbwild);
	if (chain->typeptr->stsort == iftmultiple)
	  chain->typeptr = chain->typeptr->UU.stbasetype;
      }  /* if */
      if (chain->identtag == tvariable) {
	if (chain->UU.U4.oldfrom->node_ == genn0)
	  chain->UU.U4.oldfrom->node_ = bodyn0;
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* StartBody */


Static Void checkcross()
{
  /*action 37  CheckCross*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          This routine checks to see if a cross product is legal at
          this point.  If we have already done a dot product we can
          not also do a cross product.  CrossDotError is set if there
          is a problem.  StartBody is called to create the body of the
          new forall created by the cross product.*/
  if (semtrace)
    printf("In CheckCross\n");
  if (currentlevel->UU.U0.crossdotflag == dotflag ||
      currentlevel->UU.U0.crossdoterror)
    currentlevel->UU.U0.crossdoterror = true;
  else {
    currentlevel->UU.U0.crossdotflag = crossflag;
    startbody();
  }  /* else */
}  /* CheckCross */


Static Void preparefordot()
{
  /*action 153 PrepareForDot*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Set CurrentLexLevel to next outer level.  This is to insure
          that all nodes created while building the dot expression are
          placed outside the forall instead of in the generator graph.
          The routine IncreaseGenerator will restore the current lex level.*/
  currentlevel = currentlevel->lastlevel;
}


Static Void endbody()
{
  /*action 77  EndBody*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for values that are coming for the
          body subgraph node.  Change then to come from the returns
          subgraph node.  Also search for values that were created in the
          body and create and edge that connects them to the body subgraph.
          These values will not be on a list.  Copy the current subgraph
          values into the body values in the lex level and move the
          returns values in the current values.*/
  node *bodyn0, *returnn0;
  int hashindex;
  symtblbucket *chain;
  inforecord *toinfo;

  if (semtrace)
    printf("In EndBody\n");
  bodyn0 = currentlevel->currentsubn0;
  returnn0 = currentlevel->UU.U0.returnn0;
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (bodyn0 == chain->graphinfo->node_)
	chain->graphinfo->node_ = returnn0;
      if (chain->identtag == tvariable) {
	if (chain->UU.U4.oldfrom->node_ == bodyn0)
	  chain->UU.U4.oldfrom->node_ = returnn0;
      }
      if (chain->graphinfo->onlist == NULL && chain->graphinfo->node_ != NULL) {
	toinfo = newinfoptr();
	toinfo->node_ = bodyn0;
	toinfo->onlist = addnametolist(&currentlevel->UU.U0.tflist,
				       chain->name);
	addedge(chain->graphinfo, toinfo);
	chain->graphinfo->node_ = returnn0;
	chain->graphinfo->onlist = toinfo->onlist;
	chain->graphinfo->port_ = -1;
      }  /* if */
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
  currentlevel->UU.U0.bodyn0 = currentlevel->currentsubn0;
  currentlevel->UU.U0.bodyprevnode = currentlevel->prevnode;
  currentlevel->UU.U0.bodynodecount = currentlevel->nodecounter;
  currentlevel->currentsubn0 = currentlevel->UU.U0.returnn0;
  currentlevel->prevnode = currentlevel->UU.U0.returnn0;
  currentlevel->nodecounter = 1;
}  /* EndBody */


Local explistnode *createerrint()
{
  explistnode *expr;

  expr = newexplistptr();
  expr->graphinfo->typeptr = getbasictype(ifbinteger);
  string10(&expr->graphinfo->litvalue, "error     ");
  return expr;
}  /* CreateErrInt */


Static Void checkexptype(exp, line, col)
explistnode **exp;
int line, col;
{
  errorrecord *errorrec;

  if (*exp == NULL) {
    errorrec = newerrorptr(arityonfa);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    *exp = createerrint();
    return;
  }  /* if */
  if ((*exp)->graphinfo->typeptr->stlabel == intlabel) {
    return;
  }  /* else  if */
  errorrec = newerrorptr(typeonfa);
  errorrec->UU.typeptr = (*exp)->graphinfo->typeptr;
  errorrec->linenumber = line;
  errorrec->column = col;
  semerror(errorrec);
  *exp = createerrint();
}  /* CheckExpType */


Static Void makearrayfor(indexnames, exp, name, line, column)
namelistrec *indexnames;
explistnode *exp;
namelistrec *name;
int line, column;
{
  /* This routine builds the generator for a array or stream driven forall.
     It calls itself recursively to build nested foralls when implicit
     cross products are to be created.*/
  symtblbucket *sym;
  inforecord *toinfo, *asinfo;
  node *asnode, *alnode, *sub, *sfallnode, *saven0;
  stentry *mult;
  treestackobj *level;
  boolean implicitcross;
  explistnode *currentexp;
  stryng compname, oldname, newname;
  errorrecord *errorrec;
  node *genn0;
  int hashindex;
  symtblbucket *chain;

  if (indexnames == NULL)
    implicitcross = false;
  else if (indexnames->next == NULL)
    implicitcross = false;
  else
    implicitcross = true;
  if (exp->graphinfo->node_ != NULL) {
    if (exp->graphinfo->name.len != 0) {
      string10(&oldname, "old-      ");
      stripspaces(&oldname);
      /* if exp is an old value of a loop var we need to find the value in
         the outer scope and pull it in.  We can't just pull in the name
         in the expression because it isn't hashed in with the word old
         in it.*/
      if (findstring(&oldname, &exp->graphinfo->name, 1) == 1) {
	substring(&newname, &exp->graphinfo->name, 5,
		  exp->graphinfo->name.len);
	findname(newname, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp->graphinfo = sym->UU.U4.oldfrom;
      }  /* if */
      else {  /* else just find the value and pull in */
	findname(exp->graphinfo->name, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp->graphinfo = sym->graphinfo;
      }  /* else */
    }  /* else if */
    else {  /* else edge isn't named, give it a dummy name and */
      string10(&compname, "#1        ");   /* place it on the KList */
      stripspaces(&compname);
      string10(&exp->graphinfo->name, "#1        ");
      stripspaces(&exp->graphinfo->name);
      toinfo = newinfoptr();
      toinfo->node_ = currentlevel->UU.U0.forallnode;
      toinfo->onlist = addnametolist(&currentlevel->UU.U0.kflist, compname);
      addedge(exp->graphinfo, toinfo);
      exp->graphinfo->port_ = 1;
      exp->graphinfo->node_ = currentlevel->currentsubn0;
      exp->graphinfo->onlist = toinfo->onlist;
    }  /* else */
  }
  /* literal so do nothing */
  /* else if the edge has a name */
  asnode = newnodeptr(ndatomic, currentlevel);   /* create AScatter node */
  asnode->ndcode = ifnascatter;
  asnode->ndsrcline = line;
  linkparent(asnode, currentlevel);
  asinfo = newinfoptr();
  asinfo->node_ = asnode;
  asinfo->port_ = 1;
  addedge(exp->graphinfo, asinfo);
  newttptr(&mult, iftmultiple);
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  mult->UU.stbasetype = exp->graphinfo->typeptr->UU.stbasetype;
  asinfo->typeptr = addtotypetable(mult);
  toinfo = newinfoptr();
  toinfo->node_ = currentlevel->currentsubn0;
  if (implicitcross) {  /* then value out of AScatter doesn't */
    string10(&asinfo->name, "*1        ");
	/*have a name,give it one and place*/
    stripspaces(&asinfo->name);
    string10(&compname, "*1        ");   /* on the M List.     */
    stripspaces(&compname);
    toinfo->onlist = addnametolist(&currentlevel->UU.U0.mflist, compname);
  }  /* if */
  else {  /* else we have a name for the value */
    asinfo->name = name->name;
    toinfo->onlist = addnametolist(&currentlevel->UU.U0.mflist, name->name);
  }  /* else */
  addedge(asinfo, toinfo);   /* connect value to SG boundary */
  if (!implicitcross) {
    sym = getsymtabentry(name->name, tvariable);   /* add name to Sym Tab */
    setdefloc(sym, name->linenum, column);
    sym->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    sym->graphinfo->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    sym->graphinfo->name = name->name;
    sym->graphinfo->onlist = toinfo->onlist;
    sym->graphinfo->node_ = currentlevel->currentsubn0;
    addtotable(sym, currentlevel);
  }
  asinfo->port_ = 2;   /* output port 2 of AScatter to */
  newttptr(&mult, iftmultiple);   /* SG Boundary, make it a Multiple */
  mult->UU.stbasetype = getbasictype(ifbinteger);
  asinfo->typeptr = addtotypetable(mult);
  toinfo = newinfoptr();
  toinfo->node_ = currentlevel->currentsubn0;
  if (indexnames == NULL) {
    string10(&compname, "*1        ");   /*no index name,give it one and */
    stripspaces(&compname);   /*place it on the M List.*/
    mymemcpy(asinfo->name.str, blankstring, sizeof(stryngar));
    asinfo->name.len = 0;
    toinfo->onlist = addnametolist(&currentlevel->UU.U0.mflist, compname);
  }  /*then*/
  else {
    asinfo->name = indexnames->name;
    toinfo->onlist = addnametolist(&currentlevel->UU.U0.mflist,
				   indexnames->name);
    sym = getsymtabentry(indexnames->name, tvariable);
	/* Add name to SymTab */
    setdefloc(sym, indexnames->linenum, column);
    sym->typeptr = getbasictype(ifbinteger);
    sym->graphinfo->typeptr = sym->typeptr;
    sym->graphinfo->name = indexnames->name;
    sym->graphinfo->onlist = toinfo->onlist;
    sym->graphinfo->node_ = currentlevel->currentsubn0;
    addtotable(sym, currentlevel);
    indexnames = indexnames->next;   /* move to next name */
  }
  addedge(asinfo, toinfo);
  if (indexnames == NULL) {
    return;
  }  /* if */
  /* if more names, then need an
                                                array or stream, else error */
  if (exp->graphinfo->typeptr->UU.stbasetype->stsort != iftarray &&
      exp->graphinfo->typeptr->UU.stbasetype->stsort != iftstream) {
    errorrec = newerrorptr(icnotas);
    errorrec->UU.typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    errorrec->linenumber = line;
    errorrec->column = column;
    semerror(errorrec);
    errindexnames(&indexnames, column);
    return;
  }  /* if */
  /* more names means we have a cross product, make sure it's legal */
  if (currentlevel->UU.U0.crossdoterror ||
      currentlevel->UU.U0.crossdotflag == dotflag) {
    currentlevel->UU.U0.crossdoterror = true;
    errorrec = newerrorptr(cdmix);
    errorrec->linenumber = line;
    errorrec->column = column;
    semerror(errorrec);
    errindexnames(&indexnames, column);
    sym = getsymtabentry(name->name, tvariable);
    setdefloc(sym, name->linenum, column);
    sym->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    sym->graphinfo->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    sym->graphinfo->name = name->name;
    sym->graphinfo->onlist = toinfo->onlist;
    sym->graphinfo->node_ = currentlevel->currentsubn0;
    addtotable(sym, currentlevel);
    return;
  }
  currentlevel->UU.U0.crossdotflag = crossflag;
  genn0 = currentlevel->currentsubn0;
  sub = buildsubgraph(currentlevel->UU.U0.forallnode);
  currentlevel->UU.U0.bodyn0 = sub;
  currentlevel->UU.U0.bodyprevnode = sub;
  currentexp = newexplistptr();
  currentexp->graphinfo->node_ = currentlevel->currentsubn0;
  currentexp->graphinfo->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
  currentexp->graphinfo->onlist = currentlevel->UU.U0.mflist;
  /*make the node GI of the things created in the generator the new
    body N0 */
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (chain->graphinfo->node_ == genn0)
	chain->graphinfo->node_ = currentlevel->UU.U0.bodyn0;
      if (chain->identtag == tvariable) {
	if (chain->UU.U4.oldfrom->node_ == genn0)
	  chain->UU.U4.oldfrom->node_ = currentlevel->UU.U0.bodyn0;
      }
      if (chain->typeptr->stsort == iftmultiple)
	chain->typeptr = chain->typeptr->UU.stbasetype;
      chain = chain->nextbucket;
    }  /*while*/
    hashindex++;
  }  /*while*/
  /*  extract lower bound in case we return an array */
  alnode = newnodeptr(ndatomic, currentlevel);
  alnode->ndcode = ifnaliml;
  alnode->ndsrcline = line;
  linkparent(alnode, currentlevel);
  toinfo = newinfoptr();
  toinfo->node_ = alnode;
  toinfo->port_ = 1;
  addedge(currentexp->graphinfo, toinfo);
  toinfo->typeptr = getbasictype(ifbinteger);
  /* build a "special" forall in this body */
  sfallnode = buildcompound_(specfa);
  setlowerbound(currentlevel, toinfo);
  addtoassolist(sfallnode, 0);
  addtoassolist(sfallnode, 1);
  addtoassolist(sfallnode, 2);
  currentlevel->UU.U0.crossdotflag = crossflag;
  saven0 = buildsubgraph(sfallnode);
  currentlevel->UU.U0.returnn0 = buildsubgraph(sfallnode);
  currentlevel->UU.U0.returnprevnode = currentlevel->UU.U0.returnn0;
  currentlevel->currentsubn0 = saven0;
  currentlevel->prevnode = saven0;
  /* build the generator of this new forall */
  makearrayfor(indexnames, currentexp, name, line, column);

  /* It's legal. build the body of this forall */
  /* else */
}  /* MakeArrayFor */


Static Void makerangefor(exp, exp2, name, line, col)
explistnode *exp, *exp2;
namelistrec *name;
int line, col;
{
  /* This routine builds the generator for a range driven forall. */
  symtblbucket *sym;
  inforecord *toinfo, *rginfo;
  node *rgnode;
  stentry *mult;
  stryng compname, oldname, newname;
  treestackobj *level;


  /* Connect the range values to the Forall node.  If they are named they
     need to be pulled into this scope.  If they are "old" values, you
     must pull them by their non-old name. If they are not named, give
     them dummy names.  Add these inputs to the K List.*/
  if (exp->graphinfo->node_ != NULL) {
    if (exp->graphinfo->name.len != 0) {
      string10(&oldname, "old-      ");
      stripspaces(&oldname);
      if (findstring(&oldname, &exp->graphinfo->name, 1) == 1) {
	substring(&newname, &exp->graphinfo->name, 5,
		  exp->graphinfo->name.len);
	findname(newname, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp->graphinfo = sym->UU.U4.oldfrom;
      }  /* if */
      else {
	findname(exp->graphinfo->name, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp->graphinfo = sym->graphinfo;
      }  /* else */
    }  /* else if */
    else {
      string10(&compname, "#1        ");
      stripspaces(&compname);
      string10(&exp->graphinfo->name, "#1        ");
      stripspaces(&exp->graphinfo->name);
      toinfo = newinfoptr();
      toinfo->node_ = currentlevel->UU.U0.forallnode;
      toinfo->onlist = addnametolist(&currentlevel->UU.U0.kflist, compname);
      addedge(exp->graphinfo, toinfo);
      exp->graphinfo->port_ = 1;
      exp->graphinfo->node_ = currentlevel->currentsubn0;
      exp->graphinfo->onlist = toinfo->onlist;
    }  /* else */
  }
  /* literal so do nothing */
  if (exp2->graphinfo->node_ != NULL) {
    if (exp2->graphinfo->name.len != 0) {
      string10(&oldname, "old-      ");
      stripspaces(&oldname);
      if (findstring(&oldname, &exp2->graphinfo->name, 1) == 1) {
	substring(&newname, &exp2->graphinfo->name, 5,
		  exp2->graphinfo->name.len);
	findname(newname, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp2->graphinfo = sym->UU.U4.oldfrom;
      }  /* if */
      else {
	findname(exp2->graphinfo->name, tvariable, &level, &sym);
	sym = pullname(sym, level, line);
	exp2->graphinfo = sym->graphinfo;
      }  /* else */
    }  /* else if */
    else {
      string10(&compname, "#2        ");
      stripspaces(&compname);
      string10(&exp2->graphinfo->name, "#2        ");
      stripspaces(&exp2->graphinfo->name);
      toinfo = newinfoptr();
      toinfo->node_ = currentlevel->UU.U0.forallnode;
      toinfo->onlist = addnametolist(&currentlevel->UU.U0.kflist, compname);
      addedge(exp2->graphinfo, toinfo);
      exp2->graphinfo->port_ = 1;
      exp2->graphinfo->node_ = currentlevel->currentsubn0;
      exp2->graphinfo->onlist = toinfo->onlist;
    }  /* else */
  }
  /* literal so do nothing */
  rgnode = newnodeptr(ndatomic, currentlevel);
      /* Create the Range Gen node */
  rgnode->ndcode = ifnrangegenerate;
  rgnode->ndsrcline = line;
  linkparent(rgnode, currentlevel);
  rginfo = newinfoptr();
  rginfo->node_ = rgnode;
  rginfo->port_ = 1;
  addedge(exp->graphinfo, rginfo);
  rginfo->port_ = 2;
  addedge(exp2->graphinfo, rginfo);
  rginfo->port_ = 1;
  newttptr(&mult, iftmultiple);
  mult->UU.stbasetype = getbasictype(ifbinteger);
  rginfo->typeptr = addtotypetable(mult);
  rginfo->name = name->name;
  toinfo = newinfoptr();
  toinfo->node_ = currentlevel->currentsubn0;
  toinfo->onlist = addnametolist(&currentlevel->UU.U0.mflist, name->name);
  addedge(rginfo, toinfo);   /* connect the output to SG */
  sym = getsymtabentry(name->name, tvariable);   /* add name to SymTab */
  setdefloc(sym, name->linenum, col);
  sym->typeptr = exp->graphinfo->typeptr;
  sym->graphinfo->typeptr = exp->graphinfo->typeptr;
  sym->graphinfo->name = name->name;
  sym->graphinfo->onlist = toinfo->onlist;
  sym->graphinfo->node_ = currentlevel->currentsubn0;
  addtotable(sym, currentlevel);
}  /* MakeRangeFor */


Static Void makeforall(line, col)
int line, col;
{
  /*action 41  MakeForAll*/
  /*stackpops - tgnamelist  (index names)
                         tgexplist   (array/stream or range exp)
                         tgnamelist  (loop name)*/
  /*description
          Check the type of the exp.  If it is an array or stream,
          then it's arity must be 1.  Otherwise the arity must be 2
          and the types of both expressions must be integer.  If
          array/stream then output a ALimL node to extract the lower
          bound.  Save the value in the new LexLevel (for a range for just
          save the info, no node is created).  Create a ForAll node and
          a new lexlevel.  Build the association list and create the
          returns subgraph, saving its infomation in the rerturns part
          of the lexlevel.  The body will be created later.  If we
          are building an array/stream forall then call MakeArrayFor
          else call MakeRangeFor.  Also if this is a range forall
          the index name list must be empty.*/
  semanticrec *indexnamesl, *expl, *namel;
  namelistrec *indexnames, *name;
  explistnode *exp, *exp2;
  boolean isarray;
  node *alnode, *fallnode, *saven0;
  inforecord *toinfo;
  errorrecord *errorrec;

  if (semtrace)
    printf("In MakeForall\n");
  indexnamesl = popsemantic();
  expl = popsemantic();
  namel = popsemantic();
  indexnames = indexnamesl->UU.namelist;
  exp = expl->UU.explist;
  name = namel->UU.namelist;
  isarray = false;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort == iftarray ||
      exp->graphinfo->typeptr->stsort == iftstream)
  {   /* array/stream forall */
    if (exp->next != NULL) {
      errorrec = newerrorptr(arityonfa);   /* arity must be 1 */
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      exp->next = NULL;
    }  /* if */
    alnode = newnodeptr(ndatomic, currentlevel);
    alnode->ndcode = ifnaliml;   /* save lower bound to set it on the */
    alnode->ndsrcline = line;   /* output if we return an array */
    linkparent(alnode, currentlevel);
    toinfo = newinfoptr();
    toinfo->node_ = alnode;
    toinfo->port_ = 1;
    addedge(exp->graphinfo, toinfo);
    toinfo->typeptr = getbasictype(ifbinteger);
    isarray = true;
  }  /* if */
  else {  /* range forall */
    exp2 = exp->next;   /* exp must have arity 2 and be integers */
    checkexptype(&exp, line, col);
    checkexptype(&exp2, line, col);
    toinfo = newinfoptr();   /* save lower value of range to set */
    toinfo->node_ = exp->graphinfo->node_;
	/* lower bound if we return an array */
    toinfo->port_ = exp->graphinfo->port_;
    toinfo->typeptr = exp->graphinfo->typeptr;
    toinfo->name = exp->graphinfo->name;
    toinfo->onlist = exp->graphinfo->onlist;
    toinfo->litvalue = exp->graphinfo->litvalue;
  }  /* else */
  fallnode = buildcompound_(forloop);   /* create the forall node */
  setlowerbound(currentlevel, toinfo);
  addtoassolist(fallnode, 0);
  addtoassolist(fallnode, 1);
  addtoassolist(fallnode, 2);
  saven0 = buildsubgraph(fallnode);
  currentlevel->UU.U0.returnn0 = buildsubgraph(fallnode);
  currentlevel->UU.U0.returnprevnode = currentlevel->UU.U0.returnn0;
  currentlevel->currentsubn0 = saven0;
  currentlevel->prevnode = saven0;
  if (isarray) {   /* build Array/stream generator */
    makearrayfor(indexnames, exp, name, line, col);
    return;
  }
  makerangefor(exp, exp2, name, line, col);   /* build the range generator */
  if (indexnames == NULL) {  /* index name list must be */
    return;
  }  /* if */
  errorrec = newerrorptr(atinrg);   /* empty for a range forall */
  errorrec->linenumber = line;
  errorrec->column = col;
  semerror(errorrec);
  errindexnames(&indexnamesl->UU.namelist, col);

  /* else */
}  /* MakeForall */


Static Void increasegenerator(line, col)
int line, col;
{
  /*action 36  IncreaseGenerator*/
  /*stackpops - tgnamelist  (index names)
                         tgexplist   (array/stream/range exp)
                         tgnamelist  (loop value name)
                  pushes - nothing*/
  /*description
          This routine implements a dot product forall.  Restore the lex level
          for the forall node.  We were popped back to the outer level by
          PrepareForDot.  If we are building an array/stream dot prod, check
          that the arity of the expression is 1.  Else we are doing an range
          dot prod, the arity should be 2 and the types should both be integers.
          Check that a dot product is ok at this point.  Dots and crosses can
          not be combined.  Then call MakeArrayFor or MakeRangeFor to fill in
          the generator subgraph.*/
  semanticrec *indexnamesl, *expl, *namel;
  explistnode *exp, *exp2;
  boolean isarray;
  symtblbucket *sym;
  errorrecord *errorrec;

  if (semtrace)
    printf("In IncreaseGenerator\n");
  indexnamesl = popsemantic();
  expl = popsemantic();
  namel = popsemantic();
  currentlevel = currentlevel->nextlevel;
  /*restore the lexlevel for the
                                               forall node.*/
  isarray = false;
  exp = expl->UU.explist;   /* check exp arity and type */
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort == iftarray ||
      exp->graphinfo->typeptr->stsort == iftstream) {
    if (exp->next != NULL) {
      errorrec = newerrorptr(arityonfa);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      exp->next = NULL;
    }  /* if */
    isarray = true;
  }  /* if */
  else {
    exp2 = exp->next;
    checkexptype(&exp, line, col);
    checkexptype(&exp2, line, col);
  }  /* else */
  if (currentlevel->UU.U0.crossdoterror ||
      currentlevel->UU.U0.crossdotflag == crossflag)
  {   /* check if dot is legal */
    currentlevel->UU.U0.crossdoterror = true;
    errorrec = newerrorptr(cdmix);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    sym = getsymtabentry(namel->UU.namelist->name, tvariable);
    setdefloc(sym, namel->UU.namelist->linenum, col);
    if (isarray)
      sym->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    else
      sym->typeptr = getbasictype(ifbinteger);
    sym->graphinfo->typeptr = sym->typeptr;
    string10(&sym->graphinfo->litvalue, "error     ");
    addtotable(sym, currentlevel);
    errindexnames(&indexnamesl->UU.namelist, col);
    return;
  }  /* if */
  currentlevel->UU.U0.crossdotflag = dotflag;
  if (isarray) {
    makearrayfor(indexnamesl->UU.namelist, expl->UU.explist,
		 namel->UU.namelist, line, column);
    return;
  }
  makerangefor(exp, exp2, namel->UU.namelist, line, column);
  if (indexnamesl->UU.namelist == NULL) {
    return;
  }  /* if */
  errorrec = newerrorptr(atinrg);
  errorrec->linenumber = line;
  errorrec->column = col;
  semerror(errorrec);
  errindexnames(&indexnamesl->UU.namelist, col);

  /* else */
  /* else */
}  /* IncreaseGenerator */


Static Void explicitcross(line, col)
int line, col;
{
  /*action 38  ExplicitCross*/
  /*stackpops - tgnamelist  (index names)
                         tgexplist   (array/stream/range exp)
                         tgnamelist  (loop value name)
                  pushes - nothing*/
  /*description
          This routine implements a cross product forall.  If we are building
          an array/stream cross prod, check that the arity of the expression is
          1.  Else we are doing an range cross prod, the arity should be 2 and
          the types should both be integers.  Check that a cross product is
          ok at this point.  Dots and crosses can not be combined.  Extract
          the lower bound of the array or range in case an array is returned.
          Build a new forall node and then call MakeArrayFor or MakeRangeFor
          to fill in the generator subgraph.*/
  semanticrec *indexnamesl, *expl, *namel;
  explistnode *exp, *exp2;
  boolean isarray;
  symtblbucket *sym;
  node *alnode, *sfallnode, *saven0;
  inforecord *toinfo;
  errorrecord *errorrec;

  if (semtrace)
    printf("In ExplicitCross\n");
  indexnamesl = popsemantic();
  expl = popsemantic();
  namel = popsemantic();
  isarray = false;
  exp = expl->UU.explist;   /* check exp arity and type */
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort == iftarray ||
      exp->graphinfo->typeptr->stsort == iftstream) {
    if (exp->next != NULL) {
      errorrec = newerrorptr(arityonfa);
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      exp->next = NULL;
    }  /* if */
    isarray = true;
  }  /* if */
  else {
    exp2 = exp->next;
    checkexptype(&exp, line, col);
    checkexptype(&exp2, line, col);
    toinfo = exp->graphinfo;
  }  /* else */
  if (currentlevel->UU.U0.crossdoterror) {
    errorrec = newerrorptr(cdmix);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    sym = getsymtabentry(namel->UU.namelist->name, tvariable);
    setdefloc(sym, namel->UU.namelist->linenum, col);
    if (isarray)
      sym->typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    else
      sym->typeptr = getbasictype(ifbinteger);
    sym->graphinfo->typeptr = sym->typeptr;
    string10(&sym->graphinfo->litvalue, "error     ");
    addtotable(sym, currentlevel);
    errindexnames(&indexnamesl->UU.namelist, col);
    return;
  }  /* if */
  if (isarray) {
    alnode = newnodeptr(ndatomic, currentlevel);
    alnode->ndcode = ifnaliml;
    alnode->ndsrcline = line;
    linkparent(alnode, currentlevel);
    toinfo = newinfoptr();
    toinfo->node_ = alnode;
    toinfo->port_ = 1;
    addedge(exp->graphinfo, toinfo);
    toinfo->typeptr = getbasictype(ifbinteger);
  }  /* if */
  sfallnode = buildcompound_(specfa);
  setlowerbound(currentlevel, toinfo);
  addtoassolist(sfallnode, 0);
  addtoassolist(sfallnode, 1);
  addtoassolist(sfallnode, 2);
  saven0 = buildsubgraph(sfallnode);
  currentlevel->UU.U0.returnn0 = buildsubgraph(sfallnode);
  currentlevel->UU.U0.returnprevnode = currentlevel->UU.U0.returnn0;
  currentlevel->currentsubn0 = saven0;
  currentlevel->prevnode = saven0;
  if (isarray) {
    makearrayfor(indexnamesl->UU.namelist, exp, namel->UU.namelist, line, col);
    return;
  }
  makerangefor(exp, exp2, namel->UU.namelist, line, col);
  if (indexnamesl->UU.namelist == NULL) {
    return;
  }  /* if */
  errorrec = newerrorptr(atinrg);
  errorrec->linenumber = line;
  errorrec->column = col;
  semerror(errorrec);
  errindexnames(&indexnamesl->UU.namelist, col);

  /* else */
  /* else */
}  /* ExplicitCross */


Static Void endforall()
{
  /*action 140  EndForAll*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          While the current lex level is not a real forall (versus a forall
          created by a cross product) do the following.
                  - swap the 2nd and 3rd graphs on the grlist of the
                    forall node.  This is required because the returns
                    SG is created before the body.
                  - Assign port number to the inputs of the forall
                  - order the edges on the input and output lists of
                    each subgraph.
                  - End the level*/
  int portnum;
  graph *gr;
  boolean done;

  if (semtrace)
    printf("begin EndForAll \n");
  done = false;   /*fix up the ass list*/
  while (!done) {
    gr = currentlevel->UU.U0.forallnode->UU.U2.ndsubsid->grnext;
    currentlevel->UU.U0.forallnode->UU.U2.ndsubsid->grnext = gr->grnext;
    gr->grnext->grnext = gr;
    gr->grnext = NULL;
    portnum = assignports(currentlevel->UU.U0.kflist, 1);
    portnum = assignports(currentlevel->UU.U0.mflist, portnum);
    portnum = assignports(currentlevel->UU.U0.tflist, portnum);
    currentlevel->UU.U0.forallnode->ndilist = orderilist(
	currentlevel->UU.U0.forallnode->ndilist);
    gr = currentlevel->UU.U0.forallnode->UU.U2.ndsubsid;
    gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
    /* order input ports
                                                                of generator*/
    gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
    /*order output ports
                                                                of generator */
    gr = gr->grnext;
    gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
    /* order input ports
                                                                 of body*/
    gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
    /*order output ports
                                                                 of body */
    gr = gr->grnext;
    gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
    /* order input ports
                                                                of returns */
    gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
    /*order output ports
                                                                of returns */
    if (currentlevel->lleveltag == forloop)
      done = true;
    endlevel();
  }  /*while*/
}


Static edgelisthead *findnameonlist(name, list)
stryng name;
edgelisthead *list;
{
  /* returns nil if name is not found */
  boolean found;

  found = false;
  while (list != NULL && !found) {
    if (equalstrings(&name, &list->edgename))
      found = true;
    else
      list = list->next;
  }
  return list;
}  /* FindNameOnList */


Static Void endinitpart()
{
  /*action 50  EndInitPart*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for all values defined in the Init
          part of the loop.  These will be variables that do not
          have their imported flag set.  Set their loop flag to Linit
          and create an edge from them to the SG boundary.  The edge it
          put on the L list.*/
  int hashindex;
  symtblbucket *chain;
  inforecord *toinfo;

  if (semtrace)
    printf("In EndInitPart\n");
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (chain->identtag == tvariable) {
	if (!chain->importedflag &&
	    (chain->graphinfo->node_ != NULL ||
	     chain->graphinfo->litvalue.len != 0)) {
	  *chain->UU.U4.loopflag = linit;
	  toinfo = newinfoptr();
	  toinfo->node_ = currentlevel->currentsubn0;
	  toinfo->onlist = addnametolist(&currentlevel->UU.U5.lilist,
					 chain->name);
	  addedge(chain->graphinfo, toinfo);
	  chain->graphinfo->onlist = toinfo->onlist;
	  chain->graphinfo->node_ = currentlevel->currentsubn0;
	  chain->graphinfo->port_ = -1;
	}  /* if */
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* EndInitPart */


Static Void endabody(line, col)
int line, col;
{
  /*action 40  EndABody*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table looking for the following
                  - loop temporaries
                          These will have their LoopFlag set to LNone
                          and not be imported.  Create an edge from
                          them to the SG boundary and place the edge
                          on the T list.
                  - loop vars that were redefined
                          These will have their LoopFlag set to LVar,
                          will have their 2nd definition location filled
                          in and will not be imported.  Create an edge
                          from the value to the SG boundary.  This value
                          will already have an entry on the L list.
                          Also since the old values may be used in the test
                          we have to wire them thru the body like a temporary.
                          Put in a straight thru edge in the body and put the
                          value on a T Port.
                  - loop constants
                          These will have their LoopFlag set to LConst
                          and will not be imported.  Do the same thing
                          that was done for loop vars.
                  - loop vars that were not redefined
                          These will have their LoopFlag set to LVar
                          and will not be imported, but DO NOT have their
                          second definition location filled in.  Declare an
                          error.  We do connect an edge from the value to
                          to the SG anyway.*/
  symtblbucket *chain;
  int hashindex;
  inforecord *toinfo;
  errorrecord *errorrec;

  if (semtrace)
    printf("In EndABody\n");
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (chain->identtag == tvariable && !chain->importedflag) {
	if (chain->graphinfo->onlist == NULL &&
	    *chain->UU.U4.loopflag == lnone) {
	  chain->UU.U4.tflag = true;
	  toinfo = newinfoptr();
	  toinfo->node_ = currentlevel->currentsubn0;
	  toinfo->onlist = addnametolist(&currentlevel->UU.U5.tilist,
					 chain->name);
	  addedge(chain->graphinfo, toinfo);
	  chain->graphinfo->node_ = currentlevel->currentsubn0;
	  chain->graphinfo->onlist = toinfo->onlist;
	  chain->graphinfo->port_ = -1;
	}  /* if */
	else if ((((*chain->UU.U4.loopflag == lvar) & (getdefline(chain, 2) != -1)) ||
		  *chain->UU.U4.loopflag == lconst) &&
		 chain->importedflag == false) {
	  toinfo = newinfoptr();
	  toinfo->node_ = currentlevel->currentsubn0;
	  toinfo->onlist = findnameonlist(chain->name,
					  currentlevel->UU.U5.lilist);
	  addedge(chain->graphinfo, toinfo);
	  chain->graphinfo->node_ = currentlevel->currentsubn0;
	  chain->graphinfo->onlist = toinfo->onlist;
	  chain->graphinfo->port_ = -1;
	  if (*chain->UU.U4.loopflag == lvar) {
	    toinfo = newinfoptr();
	    toinfo->node_ = currentlevel->currentsubn0;
	    toinfo->onlist = addnametolist(&currentlevel->UU.U5.tilist,
					   chain->name);
	    addedge(chain->UU.U4.oldfrom, toinfo);
	    chain->UU.U4.oldfrom->node_ = currentlevel->currentsubn0;
	    chain->UU.U4.oldfrom->onlist = toinfo->onlist;
	    chain->UU.U4.oldfrom->port_ = -1;
	  } else if (getdefline(chain, 2) == -1 &&
		     *chain->UU.U4.loopflag == lvar && !chain->importedflag) {
	    errorrec = newerrorptr(lvarneverredef);
	    errorrec->UU.errorstr = chain->name;
	    errorrec->linenumber = line;
	    errorrec->column = col;
	    semerror(errorrec);
	    toinfo = newinfoptr();
	    toinfo->node_ = currentlevel->currentsubn0;
	    toinfo->onlist = findnameonlist(chain->name,
					    currentlevel->UU.U5.lilist);
	    addedge(chain->UU.U4.oldfrom, toinfo);
	    chain->UU.U4.oldfrom->node_ = currentlevel->currentsubn0;
	    chain->UU.U4.oldfrom->onlist = toinfo->onlist;
	    chain->UU.U4.oldfrom->port_ = -1;
	  }
	}
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */

  /*then*/
  /* else if */
}  /* EndABody */


Static Void endbbody(line, col)
int line, col;
{
  /*action 143  EndBBody*/
  /*stackpops - nothing
                  pushed - nothing*/
  /*description
          Search the symbol table looking for the following (all of these
          types of symbol table entries will not have their imported flag
          set.
                   - loop temporaries
                          They will have a LoopFlag of LNone.  Remove these
                          entries from the table.  In LoopB's loop temporaries
                          are not allowed in the returns part.
                  - loop var and loop constants
                          These are entries with the LoopFlag set to LoopVar
                          and have their second definition location filled in
                          (loop var) or have their LoopFlag set to LConst or LInit
                          (loop constants).  Create an edge from these values
                          to the SG boundary.  They will already have an entry
                          on the L list.
                  - loop var that weren't redefined
                          These are entries with their LoopFlag set to LoopVar
                          but their second definition location is not filled
                          in.  Declare and error.  We do connect and edge from
                          the value to the SG boundary anyway.*/
  symtblbucket *chain, *prev, *dump;
  int hashindex;
  inforecord *toinfo;
  errorrecord *errorrec;

  if (semtrace)
    printf("In EndBBody\n");
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    prev = NULL;
    while (chain != NULL) {
      if (chain->identtag != tvariable || chain->importedflag) {
	prev = chain;
	chain = chain->nextbucket;
	continue;
      }  /* then */
      if (chain->graphinfo->onlist == NULL && *chain->UU.U4.loopflag == lnone) {
	if (prev == NULL)
	  currentlevel->hashtable[hashindex] = chain->nextbucket;
	else
	  prev->nextbucket = chain->nextbucket;
	dump = chain;
	chain = chain->nextbucket;
	freesymptr(dump);
	continue;
      }  /* then */
      if ((((*chain->UU.U4.loopflag == lvar) & (getdefline(chain, 2) != -1)) ||
	   *chain->UU.U4.loopflag == lconst ||
	   *chain->UU.U4.loopflag == linit) && !chain->importedflag) {
	toinfo = newinfoptr();
	toinfo->node_ = currentlevel->currentsubn0;
	toinfo->onlist = findnameonlist(chain->name, currentlevel->UU.U5.lilist);
	addedge(chain->graphinfo, toinfo);
	chain->graphinfo->node_ = currentlevel->currentsubn0;
	chain->graphinfo->onlist = toinfo->onlist;
	chain->graphinfo->port_ = -1;
	prev = chain;
	chain = chain->nextbucket;
	continue;
      }
      if (getdefline(chain, 2) != -1 || *chain->UU.U4.loopflag != lvar ||
	  chain->importedflag) {
	prev = chain;
	chain = chain->nextbucket;
	continue;
      }  /* elseif */
      errorrec = newerrorptr(lvarneverredef);
      errorrec->UU.errorstr = chain->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
      toinfo = newinfoptr();
      toinfo->node_ = currentlevel->currentsubn0;
      toinfo->onlist = findnameonlist(chain->name, currentlevel->UU.U5.lilist);
      addedge(chain->UU.U4.oldfrom, toinfo);
      chain->UU.U4.oldfrom->node_ = currentlevel->currentsubn0;
      chain->UU.U4.oldfrom->onlist = toinfo->onlist;
      chain->UU.U4.oldfrom->port_ = -1;
      prev = chain;
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */

  /*else*/
  /* else */
}  /* EndBBody */


Static Void startbtest()
{
  /*action 80  StartBTest*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for values whose graphinfo node is the
          init SG of this for loop.  Change their graphinfo node and their
          oldfrom node info if they are a loop var (of an outer loop) to
          the test SG.  If the value was created in the init SG (LoopFlag =
          LInit) change their LoopFlag to LBInit.  This is required because
          when we enter the body we must be able to tell what values were
          defined in the init part.  If we just labeled them LInit, they
          could be confused with an LInit value that was pulled into this
          scope for use in the test subgraph.*/
  node *oldn0, *testn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("In StartBTest\n");
  oldn0 = currentlevel->currentsubn0;
  testn0 = buildsubgraph(currentlevel->UU.U5.initnode);
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (oldn0 == chain->graphinfo->node_)
	chain->graphinfo->node_ = testn0;
      if (chain->identtag == tvariable) {
	if (oldn0 == chain->UU.U4.oldfrom->node_)
	  chain->UU.U4.oldfrom->node_ = testn0;
	if (getloopflag(chain) == linit && !chain->importedflag)
	  *chain->UU.U4.loopflag = lbinit;
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* StartBTest */


Static Void startatest()
{
  /*action 53  StartATest*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for values whose graphinfo node is the
          body SG of this for loop.  Change their graphinfo node and their
          oldfrom node info if they are a loop var to the test SG.*/
  node *oldn0, *testn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("In StartATest\n");
  oldn0 = currentlevel->currentsubn0;
  testn0 = buildsubgraph(currentlevel->UU.U5.initnode);
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (oldn0 == chain->graphinfo->node_)
	chain->graphinfo->node_ = testn0;
      if (chain->identtag == tvariable) {
	if (oldn0 == chain->UU.U4.oldfrom->node_)
	  chain->UU.U4.oldfrom->node_ = testn0;
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* StartATest */


Static Void startbinitbody()
{
  /*action 84  StartBInitBody*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table looking for values whose graphinfo
          node or OldFrom node is the test SG.  Change it to be the
          Body SG.  If its LoopFlag is LBInit, set it to LInit. */
  node *oldn0, *bodyn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("In StartBInitBody\n");
  oldn0 = currentlevel->currentsubn0;
  bodyn0 = buildsubgraph(currentlevel->UU.U5.initnode);
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (oldn0 == chain->graphinfo->node_)
	chain->graphinfo->node_ = bodyn0;
      if (chain->identtag == tvariable) {
	if (oldn0 == chain->UU.U4.oldfrom->node_)
	  chain->UU.U4.oldfrom->node_ = bodyn0;
	if (getloopflag(chain) == lbinit && !chain->importedflag)
	  *chain->UU.U4.loopflag = linit;
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* StartBInitBody */


Static Void startainitbody()
{
  /*action 76  StartAInitBody*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for values whose GraphInfo node or
          OldFrom node is the Init SG of this for loop.  Change the
          node to be the Body SG.*/
  node *oldn0, *bodyn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("In StartAInitBody\n");
  oldn0 = currentlevel->currentsubn0;
  bodyn0 = buildsubgraph(currentlevel->UU.U5.initnode);
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (oldn0 == chain->graphinfo->node_)
	chain->graphinfo->node_ = bodyn0;
      if (chain->identtag == tvariable) {
	if (oldn0 == chain->UU.U4.oldfrom->node_)
	  chain->UU.U4.oldfrom->node_ = bodyn0;
      }
      chain = chain->nextbucket;
    }  /* while */
    hashindex++;
  }  /* while */
}  /* StartAInitBody */


Static Void startforinit()
{
  /*action 49  StartForInit*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Build the compound node for the ForInit loop.  Start a new
          lex level and create the init subgraph.*/
  node *compnode /* , *subgr */;

  if (semtrace)
    printf("Begin StartForInit \n");
  compnode = buildcompound_(initloop);
  /* subgr = */ (Void)buildsubgraph(compnode);
}


Static Void pushwhile()
{
  /*action 51  PushWhile*/
  /*stackpops - nothing
                  pushes - tgTestFlag*/
  /*description
          Push a flag that notes that the test of this loop is a while
          statement.*/
  semanticrec *semrec;

  if (semtrace)
    printf("Begin PushWhile \n");
  newsemantic(&semrec, tgtestflag);
  semrec->UU.testflag = whileloop;
  pushsemantic(semrec);
}


Static Void pushuntil()
{
  /*action 52  PushUntil*/
  /*stackpops - nothing
                  pushes - tgTestFlag*/
  /*description
          Push a flag that notes that the test of this loop is an until
          statement.*/
  semanticrec *semrec;

  if (semtrace)
    printf("Begin PushUntil \n");
  newsemantic(&semrec, tgtestflag);
  semrec->UU.testflag = untilloop;
  pushsemantic(semrec);
}


Static Void endtest()
{
  /*action 54  EndTest*/
  /*stackpops - tgExpList
                         tgTestFlag
                  pushes - nothing*/
  /*description
          Check that the arity of the expression is 1 and that its type
          is boolean.  If the TestFlag says that this is an until test
          then output a Not node, connect the expression to its input.
          Connent the result (if until) or just the exp (if while) to
          the SG boundary, port 1.*/
  semanticrec *semexplist, *semflag;
  inforecord *testgi, *subgi, *errorgi;
  errorrecord *errorrec;

  if (semtrace)
    printf("Begin EndTest \n");
  semexplist = popsemantic();
  semflag = popsemantic();
  if (semexplist->UU.explist->next != NULL) {
    errorrec = newerrorptr(testarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    semexplist->next = NULL;
  }
  if (semexplist->UU.explist->graphinfo->typeptr->stlabel != boollabel) {
    errorrec = newerrorptr(testnotbool);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    errorgi = newinfoptr();
    string10(&errorgi->litvalue, "error     ");
    errorgi->typeptr = getbasictype(ifbboolean);
    semexplist->UU.explist->graphinfo = errorgi;
  }
  testgi = newinfoptr();
  if (semflag->UU.testflag == untilloop) {
    testgi->node_ = newnodeptr(ndatomic, currentlevel);
    testgi->node_->ndcode = ifnnot;
    testgi->node_->ndsrcline = linenumber;
    linkparent(testgi->node_, currentlevel);
    testgi->port_ = 1;
    addedge(semexplist->UU.explist->graphinfo, testgi);
    testgi->typeptr = getbasictype(ifbboolean);
  } else
    testgi = semexplist->UU.explist->graphinfo;
  subgi = newinfoptr();
  subgi->node_ = currentlevel->currentsubn0;
  subgi->port_ = 1;
  addedge(testgi, subgi);
  freesemantic(&semflag);
  freesemantic(&semexplist);
}  /* EndTest */


Static assoclist *buildforinitasslist(initnode)
node *initnode;
{
  assoclist *Result;
  int count;
  assoclist *al;
  PSBLOCK b;

  /* initnode->UU.U2.ndassoc = (assoclist *)Malloc(sizeof(assoclist)); */
  b = MySBlockAlloc(); /* CANN */
  initnode->UU.U2.ndassoc = &(b->al); /* CANN */

  initnode->UU.U2.ndassoc->next = NULL;
  al = initnode->UU.U2.ndassoc;
  Result = al;
  for (count = 1; count <= 3; count++) {
    /* al->next = (assoclist *)Malloc(sizeof(assoclist)); */
    b = MySBlockAlloc(); /* CANN */
    al->next = &(b->al); /* CANN */

    al->next->next = NULL;
    al = al->next;
  }
  return Result;
}


Static Void loopbassoclist()
{
  /*action 55  LoopBAssocList*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Set the NDCode of the ForInit to LoopB.  Build the association
          list.*/
  int grnum;
  assoclist *asslistptr;

  if (semtrace)
    printf("Begin LoopBAssoclist \n");
  currentlevel->UU.U5.initnode->ndcode = ifnloopb;
  asslistptr = buildforinitasslist(currentlevel->UU.U5.initnode);
  for (grnum = 0; grnum <= 3; grnum++) {
    asslistptr->graphnum = grnum;
    asslistptr = asslistptr->next;
  }  /*for*/
}


Static Void loopaassoclist()
{
  /*action 56  LoopAAssocList*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Set the NDCode of the ForInit to LoopA.  Build the association
          list.*/
  int grnum;
  assoclist *asslistptr;

  if (semtrace)
    printf("Begin LoopAAssocList \n");
  currentlevel->UU.U5.initnode->ndcode = ifnloopa;
  asslistptr = buildforinitasslist(currentlevel->UU.U5.initnode);
  for (grnum = 0; grnum <= 3; grnum++) {
    asslistptr->graphnum = grnum;
    asslistptr = asslistptr->next;
  }  /*for*/
}


Static Void pushold()
{
  /*action 57 PushOld*/
  /*stackpops - nothing
                  pushes - tgoldflag*/
  /*description
          Push a true old flag onto the stack.*/
  semanticrec *semold;

  if (semtrace)
    printf("begin PushOld\n");
  newsemantic(&semold, tgoldflag);
  semold->UU.oldflag = true;
  pushsemantic(semold);
}


Static Void pushnoold()
{
  /*action 58 PushNoOld*/
  /*stackpops - nothing
                  pushes - tgoldflag*/
  /*description
          Push a false old flag onto the stack.*/
  semanticrec *semold;

  if (semtrace)
    printf("begin PushNoOld\n");
  newsemantic(&semold, tgoldflag);
  semold->UU.oldflag = false;
  pushsemantic(semold);
}


Static Void doreduce(action)
int action;
{
  /*action 59 DoRedRight
           60 DoRedLeft
           61 DoRedTree
           62 DoReduce*/
  /*stackpops - nothing
                  pushes - IFN Code for the appropriate reduce node*/
  /*decscription
          Push IFN Code for the type of reduce node specified.*/
  semanticrec *semrednodetype;

  if (semtrace)
    printf("begin DoReduce\n");
  newsemantic(&semrednodetype, tgrednodetype);
  switch (action) {

  case lbdoredright:
    semrednodetype->UU.rednodetype = ifnredright;
    break;

  case lbdoredleft:
    semrednodetype->UU.rednodetype = ifnredleft;
    break;

  case lbdoredtree:
    semrednodetype->UU.rednodetype = ifnredtree;
    break;

  case lbdoreduce:
    semrednodetype->UU.rednodetype = ifnreduce;
    break;
  }/*case*/
  pushsemantic(semrednodetype);
}


Static Void dosum()
{
  /*action 63 DoSum*/
  /*stackpops - nothing
                  pushes - tgReduceFlag*/
  /*description
          Push a Reduce flag equal to sum.*/
  semanticrec *semredflag;

  if (semtrace)
    printf("begin DoSum\n");
  newsemantic(&semredflag, tgreduceflag);
  semredflag->UU.redflag = rfsum;
  pushsemantic(semredflag);
}


Static Void doproduct()
{
  /*action 64 DoProduct*/
  /*stackpops - nothing
                  pushes - tgReduceFlag*/
  /*description
          Push a Reduce flag equal to RFProduct.*/
  semanticrec *semredflag;

  if (semtrace)
    printf("begin DoProduct\n");
  newsemantic(&semredflag, tgreduceflag);
  semredflag->UU.redflag = rfproduct;
  pushsemantic(semredflag);
}


Static Void dogreatest()
{
  /*action 65 DoGreatest*/
  /*stackpops - nothing
                  pushes - tgReduceFlag*/
  /*description
          Push a Reduce flag equal to RFGreatest.*/
  semanticrec *semredflag;

  if (semtrace)
    printf("begin DoGreatest\n");
  newsemantic(&semredflag, tgreduceflag);
  semredflag->UU.redflag = rfgreatest;
  pushsemantic(semredflag);
}


Static Void doleast()
{
  /*action 66 DoLeast*/
  /*stackpops - nothing
                  pushes - tgReduceFlag*/
  /*description
          Push a Reduce flag equal to RFLeast.*/
  semanticrec *semredflag;

  if (semtrace)
    printf("begin DoLeast\n");
  newsemantic(&semredflag, tgreduceflag);
  semredflag->UU.redflag = rfleast;
  pushsemantic(semredflag);
}


Static Void docatenate()
{
  /*action 67 DoCatenate*/
  /*stackpops - nothing
                  pushes - tgReduceFlag*/
  /*description
          Push a Reduce flag equal to RFCatenate.*/
  semanticrec *semredflag;

  if (semtrace)
    printf("begin DoCatenate\n");
  newsemantic(&semredflag, tgreduceflag);
  semredflag->UU.redflag = rfcatenate;
  pushsemantic(semredflag);
}


Static Void noredop()
{
  /*action 68  NoRedOp*/
  /*stackpops - nothing
                  pushes - tgexplist  empty for no reduce node
                           tgReduceFlag  flag set to none*/
  /*description
          Push an empty explist to signify no reduction is specified and
          a reduceflag set to none.*/
  semanticrec *semrec;

  if (semtrace)
    printf("begin NoRedOp\n");
  newsemantic(&semrec, tgrednodetype);
  semrec->UU.rednodetype = 0;
  pushsemantic(semrec);
  newsemantic(&semrec, tgreduceflag);
  semrec->UU.redflag = rfnone;
  pushsemantic(semrec);
}


Static Void checkmask(explist)
explistnode *explist;
{
  errorrecord *errorrec;

  if (explist->next != NULL) {
    errorrec = newerrorptr(maskingarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    explist->next = NULL;
  }
  if (explist->graphinfo->typeptr == getbasictype(ifbboolean)) {
    return;
  }  /*then*/
  errorrec = newerrorptr(masknotbool);
  errorrec->linenumber = linenumber;
  semerror(errorrec);
  explist = NULL;   /*just remove the mask*/
}


Static Void dounlessexp()
{
  /*action 69  DoUnlessExp*/
  /*stackpops - tgexplist
                  pushes - tgexplist*/
  /*description
          Check that the masking exp has arity 1 and type boolean.
          Connect a negate node to the expression and save the graphinfo
          for the output of the negate node on the stack.*/
  semanticrec *semexplist;
  inforecord *gi;

  if (semtrace)
    printf("begin DoUnlessExp\n");
  semexplist = popsemantic();
  checkmask(semexplist->UU.explist);
  if (semexplist->UU.explist != NULL) {
    gi = newinfoptr();
    gi->node_ = newnodeptr(ndatomic, currentlevel);
    gi->node_->ndcode = ifnnot;
    gi->node_->ndsrcline = linenumber;
    linkparent(gi->node_, currentlevel);
    gi->port_ = 1;
    addedge(semexplist->UU.explist->graphinfo, gi);
    gi->typeptr = getbasictype(ifbboolean);
    semexplist->UU.explist->graphinfo = gi;
  }
  pushsemantic(semexplist);
}


Static Void dowhenexp()
{
  /*action 70 DoWhenExp*/
  /*stackpops - tgexplist
                  pushes - tgexplist*/
  /*description
          Check that masking exp had arity 1 and type bool.*/
  semanticrec *semexplist;

  if (semtrace)
    printf("begin DoWhenExp\n");
  semexplist = popsemantic();
  checkmask(semexplist->UU.explist);
  pushsemantic(semexplist);
}


Static Void nomaskingexp()
{
  /*action 71 NoMaskingExp*/
  /*stackpops - nothing
                  pushes - tgexplist  (empty)*/
  /*description
          Push an empty explist on the stack.*/
  semanticrec *semrec;

  if (semtrace)
    printf("begin NoMaskingExp\n");
  newsemantic(&semrec, tgexplist);
  pushsemantic(semrec);
}


Static inforecord *removelast(inputgi, level)
inforecord *inputgi;
treestackobj *level;
{
  /* Attach an AllButLastValue node to the expression supplied and return
     graph info for the output of the ABLV node.*/
  inforecord *outputgi;

  outputgi = newinfoptr();
  outputgi->node_ = newnodeptr(ndatomic, level);
  outputgi->node_->ndcode = ifnallbutlastvalue;
  outputgi->node_->ndsrcline = linenumber;
  linkparent(outputgi->node_, level);
  outputgi->port_ = 1;
  addedge(inputgi, outputgi);
  outputgi->typeptr = inputgi->typeptr;
  return outputgi;
}


Static stentry *buildredtype(intypeptr)
stentry *intypeptr;
{
  stentry *functtype, *tuple1type, *tuple2type;

  newttptr(&functtype, iftfunctiontype);
  newttptr(&tuple1type, ifttuple);
  newttptr(&tuple2type, ifttuple);
  tuple1type->UU.U2.stelemtype = intypeptr;
  tuple1type->UU.U2.stnext = tuple2type;
  tuple2type->UU.U2.stelemtype = intypeptr;
  functtype->UU.U3.starg = tuple1type;
  newttptr(&tuple1type, ifttuple);
  tuple1type->UU.U2.stelemtype = intypeptr;
  tuple1type->UU.U2.stelemtype = intypeptr;
  functtype->UU.U3.stres = tuple1type;
  return (addtotypetable(functtype));
}


Static Void switchtoreturngraph(level)
treestackobj *level;
{
  level->UU.U0.bodynodecount = level->nodecounter;
  level->nodecounter = level->UU.U0.returnnodecount;
  level->UU.U0.bodyprevnode = level->prevnode;
  level->prevnode = level->UU.U0.returnprevnode;
  level->currentsubn0 = level->UU.U0.returnn0;
}


Static Void switchtobodygraph(level)
treestackobj *level;
{
  level->UU.U0.returnnodecount = level->nodecounter;
  level->nodecounter = level->UU.U0.bodynodecount;
  level->UU.U0.returnprevnode = level->prevnode;
  level->prevnode = level->UU.U0.bodyprevnode;
  level->currentsubn0 = level->UU.U0.bodyn0;
}


Static Void dovalueof()
{
  /*action 72  DoValueOf*/
  /*stackpops - tgExpList     (masking exp)
                         tgExpList     (return value)
                         tgRedFlag     (reduction operation type, Sum, Least, etc)
                         tgRedNodeType (type of reduce node, RedLeft,RedTree, etc)
                         tgOldFlag     (returning old or not)
                         tgExpList     (list of results already created)
                  pushes - tgExpList   (list of results)*/
  /*description
          This procedure constructs the returns graphs for "value of"
          returns.  It takes care of masking expressions, "old value of",
          and reduction nodes.  Also if we have nested for all nodes
          due to cross products, we have to build those returns graphs
          also.  See the code for a more complete description.*/
  semanticrec *semmaskexp, *semvalueexp, *semredflag, *semrednodetype,
	      *semoldflag, *semresults;
  errorrecord *errorrec;
  inforecord *gi;
  explistnode *resexp;
  boolean done;
  inforecord *bodyportgi;
  stryng bodyportname;
  treestackobj *level;
  inforecord *abinfo, *tyinfo;
  node *abnode;
  inforecord *rednodegi;

  if (semtrace)
    printf("begin DoValueOf\n");
  semmaskexp = popsemantic();
  semvalueexp = popsemantic();
  semredflag = popsemantic();
  semrednodetype = popsemantic();
  semoldflag = popsemantic();
  semresults = popsemantic();
  if (semvalueexp->UU.explist->graphinfo->typeptr == NULL)
    semvalueexp->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
  if (semvalueexp->UU.explist->next != NULL)   /* check arity of value exp */
  {  /* should be 1 */
    errorrec = newerrorptr(valueofarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    semvalueexp->UU.explist->next = NULL;
  }  /*then*/
  done = false;
  level = currentlevel;
  /* if we have nested foralls due to cross products, we will have to
     execute this code for each of them.*/
  while (!done) {
    if (semoldflag->UU.oldflag)   /* if "old value of" then */
    {  /* output AllButLastValue node */
      semvalueexp->UU.explist->graphinfo = removelast(
	  semvalueexp->UU.explist->graphinfo, level);
      if (semmaskexp->UU.explist != NULL)
	    /* do the same for mask exp if it exists */
	      semmaskexp->UU.explist->graphinfo = removelast(
		  semmaskexp->UU.explist->graphinfo, level);
    }
    if (semrednodetype->UU.rednodetype == 0)   /* if no reduction operation */
    {  /* output a final value node */
      gi = newinfoptr();
      gi->node_ = newnodeptr(ndatomic, level);
      gi->node_->ndcode = ifnfinalvalue;
      gi->node_->ndsrcline = linenumber;
      linkparent(gi->node_, level);
      gi->port_ = 1;
      addedge(semvalueexp->UU.explist->graphinfo, gi);
      if (semmaskexp->UU.explist != NULL) {   /* if mask exists, connect it */
	gi->port_ = 2;
	addedge(semmaskexp->UU.explist->graphinfo, gi);
      }
      gi->port_ = 1;   /*set up for the output graph info*/
      gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
    }  /*then*/
    else {  /* else get a reduce node */
      rednodegi = newinfoptr();
      rednodegi->node_ = newnodeptr(ndatomic, level);
      rednodegi->node_->ndcode = semrednodetype->UU.rednodetype;
      rednodegi->node_->ndsrcline = linenumber;
      linkparent(rednodegi->node_, level);
      /* case on the type of reduction.  Connect the operation to port 1.
         Check that the value exp is a legal type for this reduction.
         Connect the correct unit value to port 2.*/
      switch (semredflag->UU.redflag) {

      case rfsum:
	gi = newinfoptr();
	string10(&gi->litvalue, "SUM       ");
	gi->typeptr = buildredtype(semvalueexp->UU.explist->graphinfo->typeptr);
	rednodegi->port_ = 1;
	addedge(gi, rednodegi);
	if ((unsigned)semvalueexp->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	    ((1L << semvalueexp->UU.explist->graphinfo->typeptr->stlabel) &
	     algebtypes) == 0) {
/* p2c: mono.bin.noblank.p, line 17218: Note:
 * Line breaker spent 0.7+0.36 seconds, 440 tries on line 20103 [251] */
	  errorrec = newerrorptr(reducetype);
	  errorrec->UU.U91.redtype = semvalueexp->UU.explist->graphinfo->typeptr;
	  errorrec->UU.U91.redflag = semredflag->UU.redflag;
	  errorrec->linenumber = linenumber;
	  semerror(errorrec);
	} else {
	  switch (semvalueexp->UU.explist->graphinfo->typeptr->stlabel) {

	  case boollabel:
	    string10(&gi->litvalue, "false     ");
	    break;

	  case doublabel:
	    string10(&gi->litvalue, "0.0D0     ");
	    break;

	  case intlabel:
	    string10(&gi->litvalue, "0         ");
	    break;

	  case reallabel:
	    string10(&gi->litvalue, "0.0       ");
	    break;
	  }/*case*/
	}  /*else*/
	gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
	rednodegi->port_ = 2;
	addedge(gi, rednodegi);
	break;
	/*RFSum*/

      case rfproduct:
	gi = newinfoptr();
	string10(&gi->litvalue, "PRODUCT   ");
	gi->typeptr = buildredtype(semvalueexp->UU.explist->graphinfo->typeptr);
	rednodegi->port_ = 1;
	addedge(gi, rednodegi);
	if ((unsigned)semvalueexp->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	    ((1L << semvalueexp->UU.explist->graphinfo->typeptr->stlabel) &
	     algebtypes) == 0) {
	  errorrec = newerrorptr(reducetype);
	  errorrec->UU.U91.redtype = semvalueexp->UU.explist->graphinfo->typeptr;
	  errorrec->UU.U91.redflag = semredflag->UU.redflag;
	  errorrec->linenumber = linenumber;
	  semerror(errorrec);
	} else {
	  switch (semvalueexp->UU.explist->graphinfo->typeptr->stlabel) {

	  case boollabel:
	    string10(&gi->litvalue, "true      ");
	    break;

	  case doublabel:
	    string10(&gi->litvalue, "1.0D0     ");
	    break;

	  case intlabel:
	    string10(&gi->litvalue, "1         ");
	    break;

	  case reallabel:
	    string10(&gi->litvalue, "1.0       ");
	    break;
	  }/*case*/
	}  /*else*/
	gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
	rednodegi->port_ = 2;
	addedge(gi, rednodegi);
	break;
	/*RFProduct*/

      case rfleast:
	if ((unsigned)semvalueexp->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	    ((1L << semvalueexp->UU.explist->graphinfo->typeptr->stlabel) &
	     algebtypes) == 0) {
	  errorrec = newerrorptr(reducetype);
	  errorrec->UU.U91.redtype = semvalueexp->UU.explist->graphinfo->typeptr;
	  errorrec->UU.U91.redflag = semredflag->UU.redflag;
	  errorrec->linenumber = linenumber;
	  semerror(errorrec);
	}
	gi = newinfoptr();
	string10(&gi->litvalue, "LEAST     ");
	gi->typeptr = buildredtype(semvalueexp->UU.explist->graphinfo->typeptr);
	rednodegi->port_ = 1;
	addedge(gi, rednodegi);
	string10(&gi->litvalue, "max       ");
	gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
	rednodegi->port_ = 2;
	addedge(gi, rednodegi);
	break;
	/*RFLeast*/

      case rfgreatest:
	if ((unsigned)semvalueexp->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	    ((1L << semvalueexp->UU.explist->graphinfo->typeptr->stlabel) &
	     algebtypes) == 0) {
	  errorrec = newerrorptr(reducetype);
	  errorrec->UU.U91.redtype = semvalueexp->UU.explist->graphinfo->typeptr;
	  errorrec->UU.U91.redflag = semredflag->UU.redflag;
	  errorrec->linenumber = linenumber;
	  semerror(errorrec);
	}
	gi = newinfoptr();
	string10(&gi->litvalue, "GREATEST  ");
	gi->typeptr = buildredtype(semvalueexp->UU.explist->graphinfo->typeptr);
	rednodegi->port_ = 1;
	addedge(gi, rednodegi);
	string10(&gi->litvalue, "min       ");
	gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
	rednodegi->port_ = 2;
	addedge(gi, rednodegi);
	break;
	/*RFGreatest*/

      case rfcatenate:
	if (semvalueexp->UU.explist->graphinfo->typeptr->stsort != iftarray &&
	    semvalueexp->UU.explist->graphinfo->typeptr->stsort != iftstream) {
	  errorrec = newerrorptr(reducetype);
	  errorrec->UU.U91.redtype = semvalueexp->UU.explist->graphinfo->typeptr;
	  errorrec->UU.U91.redflag = semredflag->UU.redflag;
	  errorrec->linenumber = linenumber;
	  semerror(errorrec);
	}
	gi = newinfoptr();
	string10(&gi->litvalue, "CATENATE  ");
	gi->typeptr = buildredtype(semvalueexp->UU.explist->graphinfo->typeptr);
	rednodegi->port_ = 1;
	addedge(gi, rednodegi);
	abnode = newnodeptr(ndatomic, level);
	abnode->ndcode = ifnabuild;
	linkparent(abnode, level);
	abinfo = newinfoptr();
	abinfo->node_ = abnode;
	abinfo->port_ = 1;
	tyinfo = newinfoptr();
	tyinfo->typeptr = getbasictype(ifbinteger);
	string10(&tyinfo->litvalue, "1         ");
	addedge(tyinfo, abinfo);
	abinfo->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
	rednodegi->port_ = 2;
	addedge(abinfo, rednodegi);
	break;
	/*RFCatenate*/
      }/*case*/
      /*connect the values to reduce to port 3*/
      rednodegi->port_ = 3;
      addedge(semvalueexp->UU.explist->graphinfo, rednodegi);
      /*connect the mask to port 4 if one exists*/
      if (semmaskexp->UU.explist != NULL) {
	rednodegi->port_ = 4;
	addedge(semmaskexp->UU.explist->graphinfo, rednodegi);
      }
      gi = rednodegi;   /* set up the output value */
      gi->port_ = 1;
      gi->typeptr = semvalueexp->UU.explist->graphinfo->typeptr;
    }  /*else*/
    resexp = newexplistptr();   /* connect the value to the SG */
    resexp->graphinfo->node_ = level->currentsubn0;
    resexp->graphinfo->port_ = nextreturnport(level);
    resexp->graphinfo->typeptr = gi->typeptr;
    addedge(gi, resexp->graphinfo);
    switch (level->lleveltag) {

    case forloop:
    case specfa:
      resexp->graphinfo->node_ = level->UU.U0.forallnode;
      break;

    case initloop:
      resexp->graphinfo->node_ = level->UU.U5.initnode;
      break;
    }/*case*/
    /*  Determine if we have to process some outer foralls due to
        cross products.*/
    if (level->lleveltag == initloop || level->lleveltag == forloop)
    {  /* if not, add this value to return exp list */
      addtoexplist(&semresults->UU.explist, resexp);
      pushsemantic(semresults);   /* and push it */
      done = true;
      continue;
    }  /*then*/
    level = level->lastlevel;   /* levels body subgraph.   */
    if (level->currentsubn0 == level->UU.U0.returnn0)
      switchtobodygraph(level);
    bodyportgi = newinfoptr();   /* Connect the value to the body SG */
    bodyportgi->node_ = level->currentsubn0;
    bodyportgi->typeptr = resexp->graphinfo->typeptr;
    nextbodyportname(level, &bodyportname);
    bodyportgi->onlist = addnametolist(&level->UU.U0.tflist, bodyportname);
    addedge(resexp->graphinfo, bodyportgi);
    if (level->currentsubn0 == level->UU.U0.bodyn0)
	  /* switch to the returns graph */
	    switchtoreturngraph(level);
    /* Change the node of the returns value to the returns SG.  Copy it
       to the place where the looping code expects it and continue the loop. */
    bodyportgi->node_ = level->currentsubn0;
    semvalueexp->UU.explist->graphinfo = bodyportgi;
    semmaskexp->UU.explist = NULL;
    /*masking only occurs in the inner
                                         most forall node.*/
  }  /*while*/
  /* Set the context back to the body SGs of all the foralls except
     for the inner most, so that pull name will connect the edges
     correctly.*/
  while (level->nextlevel != NULL) {
    if (level->currentsubn0 == level->UU.U0.returnn0)
      switchtobodygraph(level);
    level = level->nextlevel;
  }
  freesemantic(&semmaskexp);
  freesemantic(&semvalueexp);
  freesemantic(&semrednodetype);
  freesemantic(&semoldflag);

  /* else pop back a level and switch to that */
  /*else*/
}


Static Void doarrayof(action)
int action;
{
  /*action 73 DoArrayOf  74 DoStreamOf*/
  /*stackpops tgexplist  (masking exp)
                       tgexplist  (array/stream expression)
                       tgoldflag
                       tgexplist  (results list)
                  pushes tgexplist*/
  /*description
          This procedure constructs the returns graphs for "array of" or
          "stream of" returns.  It takes care of masking, "old array/
          stream of" and setting the lower bound of the arrays in the
          "array of" returns.  Also if we have nested for all nodes due
          to cross products, we have to build those returns graphs also.
          See the code for a more complete description.*/
  semanticrec *semmaskexp, *semarrayexp, *semoldflag, *semresults, *sematype;
  inforecord *lit1, *agathergi;
  explistnode *resexp;
  errorrecord *errorrec;
  boolean done;
  treestackobj *level;
  inforecord *bodyportgi, *asetlgi;
  stryng bodyportname;

  if (semtrace) {
    if (action == lbdoarrayof)
      printf("begin DoArrayOf \n");
    else
      printf("begin DoStreamOf \n");
  }
  semmaskexp = popsemantic();
  semarrayexp = popsemantic();
  semoldflag = popsemantic();
  semresults = popsemantic();
  if (semarrayexp->UU.explist->next != NULL)   /*check arity of array/stream*/
  {  /*expression, should be 1 */
    if (action == lbdoarrayof)
      errorrec = newerrorptr(arrayofarity);
    else
      errorrec = newerrorptr(streamofarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    semarrayexp->UU.explist->next = NULL;
  }  /*then*/
  done = false;
  level = currentlevel;
  /* If we have nested foralls due to cross products, we will have
     to execute this code for each of them.*/
  while (!done) {
    if (semoldflag->UU.oldflag)   /* if "old array/stream of" then */
    {  /* output AllButLastValue node */
      semarrayexp->UU.explist->graphinfo = removelast(
	  semarrayexp->UU.explist->graphinfo, level);
      if (semmaskexp->UU.explist != NULL)   /* do the same for the mask exp */
	semmaskexp->UU.explist->graphinfo = removelast(
	    semmaskexp->UU.explist->graphinfo, level);
    }
    agathergi = newinfoptr();   /* output an AGather node */
    agathergi->node_ = newnodeptr(ndatomic, level);
    agathergi->node_->ndcode = ifnagather;
    agathergi->node_->ndsrcline = linenumber;
    linkparent(agathergi->node_, level);
    agathergi->port_ = 1;
    lit1 = getalit1();   /* Set lower bound to 1 */
    addedge(lit1, agathergi);
    agathergi->port_ = 2;
    addedge(semarrayexp->UU.explist->graphinfo, agathergi);
    if (semmaskexp->UU.explist != NULL) {   /* connect mask if it exists */
      agathergi->port_ = 3;
      addedge(semmaskexp->UU.explist->graphinfo, agathergi);
    }
    resexp = newexplistptr();   /* set up the output info */
    resexp->graphinfo->node_ = level->currentsubn0;
    if (action == lbdoarrayof)
      pusharray(noaddtottable);
    else
      pushstream(noaddtottable);
    sematype = popsemantic();   /* type = array[input type] */
    sematype->UU.ttptr->UU.stbasetype = semarrayexp->UU.explist->graphinfo->
					typeptr;
    agathergi->typeptr = addtotypetable(sematype->UU.ttptr);
    agathergi->port_ = 1;
    resexp->graphinfo->port_ = nextreturnport(level);
    addedge(agathergi, resexp->graphinfo);   /* connect to SG boundary */
    resexp->graphinfo->typeptr = agathergi->typeptr;
    switch (level->lleveltag) {

    case forloop:
    case specfa:
      resexp->graphinfo->node_ = level->UU.U0.forallnode;
      break;

    case initloop:
      resexp->graphinfo->node_ = level->UU.U5.initnode;
      break;
    }/*case*/
    /* If we have a for initial loop, or a forall loop with a
       returns stream of, add the value to the returns list
       and push the list.*/
    if (level->lleveltag == initloop ||
	level->lleveltag == forloop && action == lbdostreamof) {
      addtoexplist(&semresults->UU.explist, resexp);
      pushsemantic(semresults);
      done = true;
      continue;
    }  /*then*/
    /* If we have a forall loop with a returns array of, we need
       to set the lower bound on the array.Output an ASetL node
       outside the forall node.  Connect the lower bound info
       found in the lex level to the node along with the output
       value.  Add the output of the ASetL node to the result exp
       list and push the list.*/
    if (level->lleveltag == forloop && action == lbdoarrayof) {
      asetlgi = newinfoptr();
      asetlgi->node_ = newnodeptr(ndatomic, level->lastlevel);
      asetlgi->node_->ndcode = ifnasetl;
      asetlgi->node_->ndsrcline = linenumber;
      linkparent(asetlgi->node_, level->lastlevel);
      asetlgi->port_ = 1;
      addedge(resexp->graphinfo, asetlgi);
      asetlgi->typeptr = resexp->graphinfo->typeptr;
      asetlgi->port_ = 2;
      addedge(level->UU.U0.lowerbound, asetlgi);
      asetlgi->port_ = 1;
      resexp->graphinfo = asetlgi;
      addtoexplist(&semresults->UU.explist, resexp);
      pushsemantic(semresults);
      done = true;
      continue;
    }  /*then*/
    /* Else we have a special forall node generated due to a cross
       product.  We need to process the outer forall's returns
       graph.*/
    level = level->lastlevel;   /* pop back a level */
    if (level->currentsubn0 == level->UU.U0.returnn0)
	  /* switch to it's body SG */
	    switchtobodygraph(level);
    bodyportgi = newinfoptr();   /* set up to connect to */
    bodyportgi->node_ = level->currentsubn0;   /* the body SG */
    bodyportgi->typeptr = resexp->graphinfo->typeptr;
    nextbodyportname(level, &bodyportname);
    bodyportgi->onlist = addnametolist(&level->UU.U0.tflist, bodyportname);
    /* If we are returning an array, output an ASetL node.  Connect
       the lower bound info found in the inner lex level to the node
       along with the output value.  Then connect the output of the
       ASetL node to the Body SG boundary.*/
    if (action == lbdoarrayof) {
      asetlgi = newinfoptr();
      asetlgi->node_ = newnodeptr(ndatomic, level);
      asetlgi->node_->ndcode = ifnasetl;
      asetlgi->node_->ndsrcline = linenumber;
      linkparent(asetlgi->node_, level);
      asetlgi->port_ = 1;
      addedge(resexp->graphinfo, asetlgi);
      asetlgi->typeptr = resexp->graphinfo->typeptr;
      asetlgi->port_ = 2;
      addedge(level->nextlevel->UU.U0.lowerbound, asetlgi);
      asetlgi->port_ = 1;
      addedge(asetlgi, bodyportgi);
    } else  /* otherwise, just connect the value to the SG boundary */
      addedge(resexp->graphinfo, bodyportgi);
    if (level->currentsubn0 == level->UU.U0.bodyn0)
	  /* Switch to the returns SG */
	    switchtoreturngraph(level);
    /* Change the node of the returns calue to the returns SG.  Copy it
       to the place where the looping code expects it and continue the loop. */
    bodyportgi->node_ = level->currentsubn0;
    semarrayexp->UU.explist->graphinfo = bodyportgi;
    semmaskexp->UU.explist = NULL;
    /*masking only occurs in the inner
                                          most loop */
  }  /*while*/
  /* Set the context back to the body SGs of all the foralls except
     for the inner most, so that pullname will connect the edges
     correctly.*/
  while (level->nextlevel != NULL) {
    if (level->currentsubn0 == level->UU.U0.returnn0)
      switchtobodygraph(level);
    level = level->nextlevel;
  }
  freesemantic(&semmaskexp);
  freesemantic(&semarrayexp);
  freesemantic(&semoldflag);
  freesemantic(&sematype);

  /*else*/
}


Static Void startreturns()
{
  /*action 75  StartReturns*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the symbol table for the following
                  - loop temporaries  ( not imported )
                          These have their TFlag set.  Remove these
                          from the table, they can not appear in the
                          returns part.
                  - loop vars  ( not imported )
                          These have their LoopFlag set to LVar.  Change
                          it to LNone.  These will then not allow the
                          word old to appear in the returns with these
                          values.
                  - all values that have their graphinfo node or oldfrom
                    node equal to the previous subgraph (this may be the
                    body in a LoopB or the test in a LoopA).  Change the
                    node into to the returns SG.*/
  symtblbucket *buck, *lastbuck, *dump;
  node *oldn0, *returnn0;
  int hashvalue;

  if (semtrace)
    printf("begin StartReturns \n");
  oldn0 = currentlevel->currentsubn0;
  returnn0 = buildsubgraph(currentlevel->UU.U5.initnode);
  for (hashvalue = 0; hashvalue <= maxhashtable; hashvalue++) {
    buck = currentlevel->hashtable[hashvalue];
    lastbuck = NULL;
    while (buck != NULL) {
      if (buck->identtag != tvariable && buck->identtag != tparam) {
	buck = buck->nextbucket;
	continue;
      }  /*if*/
      if (buck->UU.U4.tflag && !buck->importedflag) {
	if (lastbuck == NULL)
	  currentlevel->hashtable[hashvalue] = buck->nextbucket;
	else
	  lastbuck->nextbucket = buck->nextbucket;
	dump = buck;
	buck = buck->nextbucket;
	freesymptr(dump);
	continue;
      }  /*then*/
      if (buck->identtag == tvariable) {   /*TParams don't have LoopFlags*/
	if (*buck->UU.U4.loopflag == lvar && !buck->importedflag)
	  *buck->UU.U4.loopflag = lnone;
      }
      /*don't allow old in Returns*/
      if (oldn0 == buck->graphinfo->node_)
	buck->graphinfo->node_ = returnn0;
      if (buck->identtag == tvariable) {   /*TParams don't have OldFrom*/
	if (oldn0 == buck->UU.U4.oldfrom->node_)
	  buck->UU.U4.oldfrom->node_ = returnn0;
      }
      lastbuck = buck;
      buck = buck->nextbucket;
    }  /*while*/
  }  /*for*/

  /*else*/
}


Static Void endforinit()
{
  /*action 78  EndForInit*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          If this is a LoopA forinit, swap the test and body subgraphs.
          Assign the port number to the edges on the lists held in the
          lex level.  Order the output lists and input lists for each of
          the subgraphs.  End the lexlevel.*/
  int portnum;
  graph *gr, *bodygr, *testgr;

  if (semtrace)
    printf("begin EndForinit \n");
  if (currentlevel->UU.U5.initnode->ndcode == ifnloopa) {
    bodygr = currentlevel->UU.U5.initnode->UU.U2.ndsubsid->grnext;
    testgr = bodygr->grnext;
    currentlevel->UU.U5.initnode->UU.U2.ndsubsid->grnext = testgr;
    bodygr->grnext = testgr->grnext;
    testgr->grnext = bodygr;
  }
  portnum = assignports(currentlevel->UU.U5.kilist, 1);
  portnum = assignports(currentlevel->UU.U5.lilist, portnum);
  portnum = assignports(currentlevel->UU.U5.tilist, portnum);
  gr = currentlevel->UU.U5.initnode->UU.U2.ndsubsid;
  gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
  gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
  gr = gr->grnext;
  gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
  gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
  gr = gr->grnext;
  gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
  gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
  gr = gr->grnext;
  gr->grnode->ndilist = orderilist(gr->grnode->ndilist);
  gr->grnode->ndolist = orderolist(gr->grnode->ndolist);
  endlevel();
}


/* Local variables for dopredefined: */
struct LOC_dopredefined {
  int line;
} ;

Local Void notenoughargs(functname, LINK)
stryng functname;
struct LOC_dopredefined *LINK;
{
  errorrecord *errorrec;

  errorrec = newerrorptr(numfunctarg);
  errorrec->UU.U29.argnum = -1;
  errorrec->UU.U29.functionname = functname;
  errorrec->linenumber = LINK->line;
  semerror(errorrec);
}

Local Void toomanyargs(functname, LINK)
stryng functname;
struct LOC_dopredefined *LINK;
{
  errorrecord *errorrec;

  errorrec = newerrorptr(numfunctarg);
  errorrec->UU.U29.argnum = 1;
  errorrec->UU.U29.functionname = functname;
  errorrec->linenumber = LINK->line;
  semerror(errorrec);
}

Local Void wrongargtype(argnum, functname, LINK)
int argnum;
stryng functname;
struct LOC_dopredefined *LINK;
{
  errorrecord *errorrec;

  errorrec = newerrorptr(typefunctarg);
  errorrec->UU.U29.argnum = argnum;
  errorrec->UU.U29.functionname = functname;
  errorrec->linenumber = LINK->line;
  semerror(errorrec);
}

Local inforecord *getalit1_(LINK)
struct LOC_dopredefined *LINK;
{
  inforecord *info;

  info = newinfoptr();
  info->typeptr = getbasictype(ifbinteger);
  string10(&info->litvalue, "1         ");
  return info;
}

Local Void checknumberofargs(expl, num, functname, LINK)
semanticrec *expl;
int num;
stryng functname;
struct LOC_dopredefined *LINK;
{
  int cnum;
  explistnode *listptr;

  listptr = expl->UU.explist;
  if (listptr == NULL)
    cnum = 0;
  else {
    cnum = 1;
    while (listptr->next != NULL) {
      listptr = listptr->next;
      cnum++;
    }  /* while */
  }  /* else */
  if (cnum < num) {
    notenoughargs(functname, LINK);
    while (cnum < num) {
      if (cnum == 0) {
	expl->UU.explist = newexplistptr();
	expl->UU.explist->graphinfo = getalit1_(LINK);
	listptr = expl->UU.explist;
      }  /* if */
      else {
	listptr->next = newexplistptr();
	listptr = listptr->next;
	listptr->graphinfo = getalit1_(LINK);
      }  /* else */
      cnum++;
    }  /* while */
  }  /* if */
  if (cnum <= num) {
    return;
  }  /* if */
  toomanyargs(functname, LINK);
  if (num == 0) {
    expl->UU.explist = NULL;
    return;
  }
  listptr = expl->UU.explist;
  cnum = 1;
  while (cnum != num) {
    listptr = listptr->next;
    cnum++;
  }  /* while */
  listptr->next = NULL;

  /* else */
}  /* CheckNumberOfArgs */

Local inforecord *addintnode(gi, LINK)
inforecord *gi;
struct LOC_dopredefined *LINK;
{
  /* Since the IF1 exp node does not output integer values we need to
     add an integer node after the exp node if the inputs were integers. */
  node *intnode;
  inforecord *intgi;

  intnode = newnodeptr(ndatomic, currentlevel);
  intnode->ndcode = ifnint;
  intnode->ndsrcline = linenumber;
  linkparent(intnode, currentlevel);
  intgi = newinfoptr();
  intgi->node_ = intnode;
  intgi->port_ = 1;
  addedge(gi, intgi);
  intgi->typeptr = getbasictype(ifbinteger);
  return intgi;
}

Local inforecord *buildpredefnode(nodecode, line, LINK)
int nodecode, line;
struct LOC_dopredefined *LINK;
{
  inforecord *predefgraphinfo;

  predefgraphinfo = newinfoptr();
  predefgraphinfo->node_ = newnodeptr(ndatomic, currentlevel);
  predefgraphinfo->node_->ndcode = nodecode;
  predefgraphinfo->node_->ndsrcline = line;
  linkparent(predefgraphinfo->node_, currentlevel);
  return predefgraphinfo;
}

Local inforecord *errtype_(iftype, base, LINK)
char iftype;
stentry *base;
struct LOC_dopredefined *LINK;
{
  stentry *ptype;
  inforecord *info;

  newttptr(&ptype, iftype);
  ptype->UU.stbasetype = base;
  ptype = addtotypetable(ptype);
  info = newinfoptr();
  string10(&info->litvalue, "error     ");
  info->typeptr = ptype;
  return info;
}  /* ErrType */

Local inforecord *maxmin(ifntype, expl, line, functname, LINK)
unchar ifntype;
semanticrec *expl;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  explistnode *exp1, *exp2;
  inforecord *preinfo;

  checknumberofargs(expl, 2, functname, LINK);
  exp1 = expl->UU.explist;
  exp2 = expl->UU.explist->next;
  if (exp1->graphinfo->typeptr == NULL)
    exp1->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp2->graphinfo->typeptr == NULL)
    exp2->graphinfo->typeptr = getbasictype(ifbwild);
  if ((unsigned)exp1->graphinfo->typeptr->stlabel >= 32 ||
      ((1L << exp1->graphinfo->typeptr->stlabel) & arithtypes) == 0) {
    exp1->graphinfo = getalit1_(LINK);
    wrongargtype(1, functname, LINK);
  }  /* if */
  if ((unsigned)exp2->graphinfo->typeptr->stlabel >= 32 ||
      ((1L << exp2->graphinfo->typeptr->stlabel) & arithtypes) == 0) {
    exp2->graphinfo = getalit1_(LINK);
    wrongargtype(2, functname, LINK);
  }  /* if */
  if (exp1->graphinfo->typeptr != exp2->graphinfo->typeptr) {
    exp2->graphinfo = newinfoptr();
    string10(&exp2->graphinfo->litvalue, "error     ");
    exp2->graphinfo->typeptr = exp1->graphinfo->typeptr;
    wrongargtype(2, functname, LINK);
  }  /* if */
  preinfo = buildpredefnode(ifntype, line, LINK);
  preinfo->port_ = 1;
  addedge(exp1->graphinfo, preinfo);
  preinfo->port_ = 2;
  addedge(exp2->graphinfo, preinfo);
  preinfo->port_ = 1;
  preinfo->typeptr = exp1->graphinfo->typeptr;
  return preinfo;
}  /* MaxMin */

Local inforecord *addhlarrayorstream(iftype, expl, ifntype, line, functname,
				     LINK)
char iftype;
semanticrec *expl;
unchar ifntype;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  explistnode *exp;
  inforecord *predefgraphinfo;

  checknumberofargs(expl, 2, functname, LINK);
  exp = expl->UU.explist;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftype) {
    wrongargtype(1, functname, LINK);
    exp->graphinfo = errtype_(iftype, exp->next->graphinfo->typeptr, LINK);
  }  /* if */
  if (exp->graphinfo->typeptr->UU.stbasetype != exp->next->graphinfo->typeptr)
    wrongargtype(2, functname, LINK);
  predefgraphinfo = buildpredefnode(ifntype, line, LINK);
  predefgraphinfo->port_ = 1;
  addedge(exp->graphinfo, predefgraphinfo);
  predefgraphinfo->port_ = 2;
  addedge(exp->next->graphinfo, predefgraphinfo);
  predefgraphinfo->port_ = 1;
  predefgraphinfo->typeptr = exp->graphinfo->typeptr;
  return predefgraphinfo;
}  /* AddHLArrayOrStream */

Local inforecord *sizearrayorstream(predefkind, iftype, expl, line, functname,
				    LINK)
predeftype predefkind;
char iftype;
semanticrec *expl;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  explistnode *exp;
  inforecord *info;

  checknumberofargs(expl, 1, functname, LINK);
  exp = expl->UU.explist;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftype) {
    info = newinfoptr();
    string10(&info->litvalue, "error     ");
    wrongargtype(1, functname, LINK);
  }  /* if */
  else {
    if (predefkind == tgarraysize || predefkind == tgstreamsize)
      info = buildpredefnode(ifnasize, line, LINK);
    else
      info = buildpredefnode(ifnaprefixsize, line, LINK);
    info->port_ = 1;
    addedge(exp->graphinfo, info);
  }  /* else */
  info->typeptr = getbasictype(ifbinteger);
  return info;
}  /* SizeArrayOrStream */

Local inforecord *remhlarrayorstream(iftype, expl, ifntype, line, functname,
				     LINK)
char iftype;
semanticrec *expl;
unchar ifntype;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  stentry *bwild;
  inforecord *info;
  explistnode *exp;

  checknumberofargs(expl, 1, functname, LINK);
  exp = expl->UU.explist;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftype) {
    bwild = getbasictype(ifbwild);
    exp->graphinfo = errtype_(iftype, bwild, LINK);
    wrongargtype(1, functname, LINK);
  }  /* if */
  info = buildpredefnode(ifntype, line, LINK);
  info->port_ = 1;
  addedge(exp->graphinfo, info);
  info->typeptr = exp->graphinfo->typeptr;
  return info;
}  /* RemHLArrayOrStream */

Local inforecord *arraylimhorl(ntype, semexplist, line, functname, LINK)
unchar ntype;
semanticrec *semexplist;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  stentry *bwild;
  inforecord *predefgraphinfo;
  explistnode *exp;

  checknumberofargs(semexplist, 1, functname, LINK);
  exp = semexplist->UU.explist;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftarray) {
    wrongargtype(1, functname, LINK);
    bwild = getbasictype(ifbwild);
    exp->graphinfo = errtype_(iftarray, bwild, LINK);
  }
  predefgraphinfo = buildpredefnode(ntype, line, LINK);
  predefgraphinfo->port_ = 1;
  addedge(exp->graphinfo, predefgraphinfo);
  predefgraphinfo->typeptr = getbasictype(ifbinteger);
  return predefgraphinfo;
}  /* ArrayLimHOrL*/

Local inforecord *floorortrunc(ntype, semexplist, line, functname, LINK)
unchar ntype;
semanticrec *semexplist;
int line;
stryng functname;
struct LOC_dopredefined *LINK;
{
  explistnode *exp;
  inforecord *predefgraphinfo;

  checknumberofargs(semexplist, 1, functname, LINK);
  exp = semexplist->UU.explist;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stlabel != reallabel &&
      exp->graphinfo->typeptr->stlabel != doublabel) {
    exp->graphinfo = newinfoptr();
    string10(&exp->graphinfo->litvalue, "error     ");
    exp->graphinfo->typeptr = getbasictype(ifbreal);
    wrongargtype(1, functname, LINK);
  }  /* if */
  predefgraphinfo = buildpredefnode(ntype, line, LINK);
  predefgraphinfo->port_ = 1;
  addedge(exp->graphinfo, predefgraphinfo);
  predefgraphinfo->typeptr = getbasictype(ifbinteger);
  return predefgraphinfo;
}  /* FloorOrTrunc */


Static Void dopredefined(symptr, semexplist, line_)
symtblbucket *symptr;
semanticrec *semexplist;
int line_;
{
  /*  This procedure builds a call node for any of the predefined SISAL
      functions.  It checks the types and arity of the arguments and
      pushes an explist containing the graphinfo for the output of the
      call node.*/
  struct LOC_dopredefined V;
  inforecord *predefgraphinfo, *index;
  stentry *bwild, *newt;
  explistnode *exp, *exp2, *low, *high;

  V.line = line_;
  if (semtrace)
    printf("begin DoPreDefined\n");
  if (semexplist->UU.explist == NULL) {
    notenoughargs(symptr->name, &V);
    semexplist->UU.explist = newexplistptr();
    semexplist->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
    string10(&semexplist->UU.explist->graphinfo->name, "ERROR     ");
    stripspaces(&semexplist->UU.explist->graphinfo->name);
    string10(&semexplist->UU.explist->graphinfo->litvalue, "ERROR     ");
  } else {
    switch (symptr->UU.predefined) {

    case tgfloor:
      semexplist->UU.explist->graphinfo = floorortrunc(ifnfloor, semexplist,
	  V.line, symptr->name, &V);
      break;

    case tgtrunc:
      semexplist->UU.explist->graphinfo = floorortrunc(ifntrunc, semexplist,
	  V.line, symptr->name, &V);
      break;

    case tgmax:
      semexplist->UU.explist->graphinfo = maxmin(ifnmax, semexplist, V.line,
						 symptr->name, &V);
      break;

    case tgmin:
      semexplist->UU.explist->graphinfo = maxmin(ifnmin, semexplist, V.line,
						 symptr->name, &V);
      break;

    case tgmod:
      checknumberofargs(semexplist, 2, symptr->name, &V);
      exp = semexplist->UU.explist;
      if (exp->graphinfo->typeptr == NULL)
	exp->graphinfo->typeptr = getbasictype(ifbwild);
      exp2 = exp->next;
      if (exp2->graphinfo->typeptr == NULL)
	exp2->graphinfo->typeptr = getbasictype(ifbwild);
      if (exp->graphinfo->typeptr->stlabel != intlabel) {
	exp->graphinfo = getalit1_(&V);
	wrongargtype(1, symptr->name, &V);
      }  /* if */
      if (exp2->graphinfo->typeptr->stlabel != intlabel) {
	exp2->graphinfo = getalit1_(&V);
	wrongargtype(2, symptr->name, &V);
      }  /* if */
      predefgraphinfo = buildpredefnode(ifnmod, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(exp->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 2;
      addedge(exp2->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      predefgraphinfo->typeptr = exp->graphinfo->typeptr;
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

    case tgabs:
      checknumberofargs(semexplist, 1, symptr->name, &V);
      if (semexplist->UU.explist->graphinfo->typeptr == NULL)
	semexplist->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
      if ((unsigned)semexplist->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	  ((1L << semexplist->UU.explist->graphinfo->typeptr->stlabel) &
	   arithtypes) == 0)
	wrongargtype(1, symptr->name, &V);
      predefgraphinfo = buildpredefnode(ifnabs, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(semexplist->UU.explist->graphinfo, predefgraphinfo);
      predefgraphinfo->typeptr = semexplist->UU.explist->graphinfo->typeptr;
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

    case tgexp:
      predefgraphinfo = buildpredefnode(ifnexp, V.line, &V);
      checknumberofargs(semexplist, 2, symptr->name, &V);
      if (semexplist->UU.explist->graphinfo->typeptr == NULL)
	semexplist->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
      if ((unsigned)semexplist->UU.explist->graphinfo->typeptr->stlabel >= 32 ||
	  ((1L << semexplist->UU.explist->graphinfo->typeptr->stlabel) &
	   arithtypes) == 0)
	wrongargtype(1, symptr->name, &V);
      else {
	predefgraphinfo->typeptr = semexplist->UU.explist->graphinfo->typeptr;
	switch (semexplist->UU.explist->graphinfo->typeptr->stlabel) {

	case intlabel:
	  if (semexplist->UU.explist->next->graphinfo->typeptr->stlabel !=
	      intlabel)
	    wrongargtype(2, symptr->name, &V);
	  break;

	case reallabel:
	  if (semexplist->UU.explist->next->graphinfo->typeptr->stlabel !=
	      intlabel && semexplist->UU.explist->next->graphinfo->typeptr->
			  stlabel != reallabel)
	    wrongargtype(2, symptr->name, &V);
	  break;

	case doublabel:
	  if (semexplist->UU.explist->next->graphinfo->typeptr->stlabel !=
	      intlabel && semexplist->UU.explist->next->graphinfo->typeptr->
			  stlabel != doublabel)
	    wrongargtype(2, symptr->name, &V);
	  break;
	}/*case*/
      }  /*else*/
      predefgraphinfo->port_ = 1;
      addedge(semexplist->UU.explist->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 2;
      addedge(semexplist->UU.explist->next->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      if (predefgraphinfo->typeptr == getbasictype(ifbinteger)) {
	predefgraphinfo->typeptr = getbasictype(ifbreal);
	predefgraphinfo = addintnode(predefgraphinfo, &V);
      }
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

    case tgarrayaddh:
      semexplist->UU.explist->graphinfo = addhlarrayorstream(iftarray,
	  semexplist, ifnaaddh, V.line, symptr->name, &V);
      break;

    case tgarrayaddl:
      semexplist->UU.explist->graphinfo = addhlarrayorstream(iftarray,
	  semexplist, ifnaaddl, V.line, symptr->name, &V);
      break;

    case tgarraysize:
    case tgarrayprefixsize:
      semexplist->UU.explist->graphinfo = sizearrayorstream(
	  symptr->UU.predefined, iftarray, semexplist, V.line, symptr->name,
	  &V);
      break;

    case tgarrayreml:
      semexplist->UU.explist->graphinfo = remhlarrayorstream(iftarray,
	  semexplist, ifnareml, V.line, symptr->name, &V);
      break;

    case tgarrayremh:
      semexplist->UU.explist->graphinfo = remhlarrayorstream(iftarray,
	  semexplist, ifnaremh, V.line, symptr->name, &V);
      break;

    case tgarraylimh:
      semexplist->UU.explist->graphinfo = arraylimhorl(ifnalimh, semexplist,
	  V.line, symptr->name, &V);
      break;

    case tgarrayliml:
      semexplist->UU.explist->graphinfo = arraylimhorl(ifnaliml, semexplist,
	  V.line, symptr->name, &V);
      break;

    case tgarrayadjust:
      checknumberofargs(semexplist, 3, symptr->name, &V);
      exp = semexplist->UU.explist;
      if (exp->graphinfo->typeptr == NULL)
	exp->graphinfo->typeptr = getbasictype(ifbwild);
      if (exp->graphinfo->typeptr->stsort != iftarray) {
	bwild = getbasictype(ifbwild);
	exp->graphinfo = errtype_(iftarray, bwild, &V);
	wrongargtype(1, symptr->name, &V);
      }  /* if */
      low = exp->next;
      if (low->graphinfo->typeptr == NULL)
	low->graphinfo->typeptr = getbasictype(ifbwild);
      if (low->graphinfo->typeptr->stlabel != intlabel) {
	low->graphinfo = getalit1_(&V);
	wrongargtype(2, symptr->name, &V);
      }  /* if */
      high = low->next;
      if (high->graphinfo->typeptr == NULL)
	high->graphinfo->typeptr = getbasictype(ifbwild);
      if (high->graphinfo->typeptr->stlabel != intlabel) {
	high->graphinfo = getalit1_(&V);
	wrongargtype(3, symptr->name, &V);
      }  /* if */
      predefgraphinfo = buildpredefnode(ifnaextract, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(exp->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 2;
      addedge(low->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 3;
      addedge(high->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      predefgraphinfo->typeptr = exp->graphinfo->typeptr;
      exp->graphinfo = predefgraphinfo;
      break;

    case tgarrayfill:
      checknumberofargs(semexplist, 3, symptr->name, &V);
      low = semexplist->UU.explist;
      if (low->graphinfo->typeptr == NULL)
	low->graphinfo->typeptr = getbasictype(ifbwild);
      if (low->graphinfo->typeptr->stlabel != intlabel) {
	low->graphinfo = getalit1_(&V);
	wrongargtype(1, symptr->name, &V);
      }  /* if */
      high = low->next;
      if (high->graphinfo->typeptr == NULL)
	high->graphinfo->typeptr = getbasictype(ifbwild);
      if (high->graphinfo->typeptr->stlabel != intlabel) {
	high->graphinfo = getalit1_(&V);
	wrongargtype(2, symptr->name, &V);
      }  /* if */
      exp = high->next;
      predefgraphinfo = buildpredefnode(ifnafill, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(low->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 2;
      addedge(high->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 3;
      addedge(exp->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      newttptr(&newt, iftarray);
      newt->UU.stbasetype = exp->graphinfo->typeptr;
      predefgraphinfo->typeptr = addtotypetable(newt);
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

    case tgarraysetl:
      checknumberofargs(semexplist, 2, symptr->name, &V);
      exp = semexplist->UU.explist;
      if (exp->graphinfo->typeptr == NULL)
	exp->graphinfo->typeptr = getbasictype(ifbwild);
      if (exp->graphinfo->typeptr->stsort != iftarray) {
	bwild = getbasictype(ifbwild);
	exp->graphinfo = errtype_(iftarray, bwild, &V);
	wrongargtype(1, symptr->name, &V);
      }  /* if */
      low = exp->next;
      if (low->graphinfo->typeptr == NULL)
	low->graphinfo->typeptr = getbasictype(ifbwild);
      if (low->graphinfo->typeptr->stlabel != intlabel) {
	low->graphinfo = getalit1_(&V);
	wrongargtype(2, symptr->name, &V);
      }  /* if */
      predefgraphinfo = buildpredefnode(ifnasetl, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(exp->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 2;
      addedge(low->graphinfo, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      predefgraphinfo->typeptr = exp->graphinfo->typeptr;
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

/* CANN PEEK */ case tgpeek:
/* CANN PEEK */ predefgraphinfo = buildpredefnode(ifnpeek, V.line, &V);
/* CANN PEEK */ {
/* CANN PEEK */   int c;
/* CANN PEEK */   for ( c=1, exp = semexplist->UU.explist; exp != NULL; exp = exp->next, c++ ) {
/* CANN PEEK */     if (exp->graphinfo->typeptr == NULL)
/* CANN PEEK */       exp->graphinfo->typeptr = getbasictype(ifbwild);
/* CANN PEEK */     predefgraphinfo->port_ = c;
/* CANN PEEK */     addedge(exp->graphinfo, predefgraphinfo);
/* CANN PEEK */     }
/* CANN PEEK */ }
/* CANN PEEK */ predefgraphinfo->port_ = 1;
/* CANN PEEK */ predefgraphinfo->typeptr = getbasictype(ifbinteger);
/* CANN PEEK */ semexplist->UU.explist->graphinfo = predefgraphinfo;
/* CANN PEEK */ break;

    case tgstreamempty:
      checknumberofargs(semexplist, 1, symptr->name, &V);
      exp = semexplist->UU.explist;
      if (exp->graphinfo->typeptr == NULL)
	exp->graphinfo->typeptr = getbasictype(ifbwild);
      if (exp->graphinfo->typeptr->stsort != iftstream) {
	bwild = getbasictype(ifbwild);
	exp->graphinfo = errtype_(iftstream, bwild, &V);
	wrongargtype(1, symptr->name, &V);
      }
      predefgraphinfo = buildpredefnode(ifnaisempty, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(exp->graphinfo, predefgraphinfo);
      predefgraphinfo->typeptr = getbasictype(ifbboolean);
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;

    case tgstreamrest:
      semexplist->UU.explist->graphinfo = remhlarrayorstream(iftstream,
	  semexplist, ifnareml, V.line, symptr->name, &V);
      break;

    case tgstreamsize:
    case tgstreamprefixsize:
      semexplist->UU.explist->graphinfo = sizearrayorstream(
	  symptr->UU.predefined, iftstream, semexplist, V.line, symptr->name,
	  &V);
      break;

    case tgstreamappend:
      semexplist->UU.explist->graphinfo = addhlarrayorstream(iftstream,
	  semexplist, ifnaaddh, V.line, symptr->name, &V);
      break;

    case tgstreamfirst:
      checknumberofargs(semexplist, 1, symptr->name, &V);
      if (semexplist->UU.explist->graphinfo->typeptr == NULL)
	semexplist->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
      if (semexplist->UU.explist->graphinfo->typeptr->stsort != iftstream) {
	wrongargtype(1, symptr->name, &V);
	bwild = getbasictype(ifbwild);
	semexplist->UU.explist->graphinfo = errtype_(iftstream, bwild, &V);
      }  /* if */
      predefgraphinfo = buildpredefnode(ifnaelement, V.line, &V);
      predefgraphinfo->port_ = 1;
      addedge(semexplist->UU.explist->graphinfo, predefgraphinfo);
      index = newinfoptr();
      string10(&index->litvalue, "1         ");
      index->typeptr = getbasictype(ifbinteger);
      predefgraphinfo->port_ = 2;
      addedge(index, predefgraphinfo);
      predefgraphinfo->port_ = 1;
      predefgraphinfo->typeptr = semexplist->UU.explist->graphinfo->typeptr->
				 UU.stbasetype;
      semexplist->UU.explist->graphinfo = predefgraphinfo;
      break;
      /* TGStreamFirst */
    }/*case*/
  }  /*else*/
  semexplist->UU.explist->next = NULL;
  pushsemantic(semexplist);
}  /*DoPreDefined*/


Static Void initsem()
{
  /*action 9 initsem*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Initialize the symbol table, semantic stack, the if1 graph
          for the program and misc variables.*/
  if (semtrace)
    printf("begin initsem\n");
  initsymtable();
  initnames();
  initmodule();
  P_expset(stampset, 0L);
  topsemstk = NULL;
  topincludestk = NULL;
  univnodecnt = 1;
  binaryops = (1L << ((long)orop)) | (1L << ((long)andop)) |
      (1L << ((long)mult)) | (1L << ((long)divd)) | (1L << ((long)plus)) |
      (1L << ((long)minus)) | (1L << ((long)concat)) | (1L << ((long)lt)) |
      (1L << ((long)le)) | (1L << ((long)gt)) | (1L << ((long)ge)) |
      (1L << ((long)equal)) | (1L << ((long)notequal));
/* p2c: mono.bin.noblank.p, line 18087: Note:
 * Line breaker spent 1.9+0.96 seconds, 1390 tries on line 21328 [251] */
  arithtypes = (1L << intlabel) | (1L << doublabel) | (1L << reallabel);
  algebtypes = arithtypes | (1L << boollabel);
  streamorarray = (1L << iftarray) | (1L << iftstream);
  boostfac = 0;
  idxrefmax = 0;
}


Local boolean linkdefinename(name, list)
stryng name;
namelistrec **list;
{
  /* Check to see if name is already on list, if not return false and
     add it to the end of the list, else return true.*/
  namelistrec *listptr;
  boolean found, error;
  stryng name1, name2;

  found = false;
  error = false;
  if (*list == NULL) {
    newnamelist(list);
    (*list)->name = name;
    (*list)->linenum = linenumber;
    return error;
  }
  name1 = name;
  stringuppercase(&name1);
  listptr = *list;
  while (listptr->next != NULL) {
    name2 = listptr->name;
    stringuppercase(&name2);
    if (equalstrings(&name1, &name2))
      found = true;
    listptr = listptr->next;
  }  /*while*/
  name2 = listptr->name;
  stringuppercase(&name2);
  if (equalstrings(&name1, &name2) || found) {
    error = true;
    return error;
  }
  newnamelist(&listptr->next);
  listptr->next->name = name;
  listptr->next->linenum = linenumber;
  listptr->next->prev = listptr;
  return error;

  /*else*/
  /*else*/
}  /*LinkDefineName*/


Static Void addtodefinelist(inputtoken)
stryng inputtoken;
{
  /*action 23*/
  /*stackpops - tgnamelist
                  pushes - tgnamelist*/
  /*description
    Pop the namelist off the stack.  Check that <NAME> (input token) isn't
    already on the list, if so declare a warning else add it to the list
    and save the list back on the stack.*/
  semanticrec *semrec;
  boolean error;
  errorrecord *errorrec;

  if (semtrace)
    printf("begin AddToDefineList\n");
  semrec = popsemantic();
  error = linkdefinename(inputtoken, &semrec->UU.namelist);
  if (error) {
    errorrec = newerrorptr(defineerr);
    errorrec->UU.errorstr = inputtoken;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  pushsemantic(semrec);
}  /* addtodefinelist */


Static Void addtotypelist()
{
  /*action 5 AddToTypeList*/
  /*stackpops - tgTTptr
                         tgtypelist
                  pushes - tgtypelist*/
  /*description
          Add the TTptr to the end of the typelist.*/
  semanticrec *semttptr, *semtypelist;
  typelistrec *curtypelist, *entry_;

  if (semtrace)
    printf("Begin addtotypelist\n");
  semttptr = popsemantic();
  semtypelist = popsemantic();
  newtypelist(&entry_);
  entry_->typeptr = semttptr->UU.ttptr;
  entry_->linenum = linenumber;
  curtypelist = semtypelist->UU.typelist;
  if (curtypelist == NULL)
    semtypelist->UU.typelist = entry_;
  else {
    while (curtypelist->next != NULL)
      curtypelist = curtypelist->next;
    curtypelist->next = entry_;
    entry_->prev = curtypelist;
  }  /*else*/
  freesemantic(&semttptr);
  pushsemantic(semtypelist);
}  /* addtotypelist */


Static Void starttypelist()
{
  /*action 4 StartTypeList*/
  /*stackpops - nothing
                  pushes - tgtypelist (empty)*/
  /*description
          Places an empty typelist on the stack*/
  semanticrec *semrec;

  if (semtrace)
    printf("Begin StartTypeList \n");
  newsemantic(&semrec, tgtypelist);
  pushsemantic(semrec);
}  /* starttypelist */


Static stryng buildfunctname(name)
stryng name;
{
  /* Build the IF1 function name by prepending all the outer function names
     to the SISAL function name.  This is required because IF1 doesn't have
     a Lex Level structure.*/
  stryng newname;
  treestackobj *level;
  symtblbucket *symptrlevelname;
  stryng levelname, dot;

  mymemcpy(dot.str, blankstring, sizeof(stryngar));
  dot.len = 0;
  insertchar(&dot, '.', 1);
  newname = name;
  level = currentlevel;
  while (level->lleveltag != root) {
    if (level->lleveltag != funct) {
      continue;
    }  /*then*/
    symptrlevelname = level->UU.lfunctname;
    levelname = symptrlevelname->name;
    insertstring(&newname, &dot, 1);
    insertstring(&newname, &levelname, 1);
    level = level->lastlevel;
  }  /*while*/
  return newname;
}  /*BuildFunctName*/


Local Void addlkptr(lk)
linkrec *lk;
{
  linkrec *curlkptr;

  curlkptr = funclist;   /* global pointer to linkage rec list */
  if (curlkptr == NULL) {   /*  the list is empty */
    funclist = lk;
    return;
  }
  while (curlkptr->lknext != NULL)
    curlkptr = curlkptr->lknext;
  curlkptr->lknext = lk;

  /* else */
}  /*AddLKptr*/

Local Void addgrptr_(gr)
graph *gr;
{
  graph *curgrptr;

  curgrptr = module->UU.U2.ndsubsid;
  if (curgrptr == NULL) {   /* the list is empty */
    module->UU.U2.ndsubsid = gr;
    firstfunction = gr->grnode;
    return;
  }  /*then*/
  while (curgrptr->grnext != NULL)
    curgrptr = curgrptr->grnext;
  curgrptr->grnext = gr;

  /*else*/
}  /*AddGRptr*/

Local Void buildfunctgraph(symptr, functname, action, exported)
symtblbucket *symptr;
stryng functname;
int action;
boolean exported;
{
  /* Build the framework for a new function and hook it into the
     IF1 graph we are creating.*/
  graph *gr;
  linkrec *lk;
  node *nd;
  int defloc2;

  nd = newnodeptr(ndgraph, currentlevel);
  if (action == 1 && exported)
    lk = newlinkageptr(lsexported);
  else if (action == 1)
    lk = newlinkageptr(lslocal);
  else
    lk = newlinkageptr(lsimported);
  gr = newgraphptr();
  gr->grnode = nd;
  lk->lkgraph = nd;
  lk->lkname = functname;
  addlkptr(lk);
  addgrptr_(gr);
  nd->ndcode = ifngraph;
  nd->ndparent = module;
  nd->UU.U1.ndlink = lk;
  nd->UU.U1.ndtype = symptr->typeptr;
  defloc2 = getdefline(symptr, 2);
  if (defloc2 == -1)   /* If 2nd Def Loc = -1 there was no forward dec.*/
    nd->ndsrcline = getdefline(symptr, 1);
  else
    nd->ndsrcline = defloc2;
  /*use 2nd, cause 1st is for the forward*/
  currentlevel->prevnode = nd;
  symptr->graphinfo->node_ = nd;
}  /*BuildFunctGraph*/

Local boolean checkdefinelist(functname, action)
stryng functname;
int action;
{
  /*Check the DefineList for FunctName, if found remove it.*/
  namelistrec *nameptr;
  boolean found;
  namelistrec *nextnameptr;
  stryng lowerfunctname, lowername;

  found = false;
  nameptr = definelist;
  lowerfunctname = functname;
  stringlowercase(&lowerfunctname);
  while (nameptr != NULL && !found) {
    lowername = nameptr->name;
    stringlowercase(&lowername);
    if (!equalstrings(&lowername, &lowerfunctname)) {
      nameptr = nameptr->next;
      continue;
    }  /*then*/
    found = true;
    if (action != 1) {   /* if not a global function also */
      continue;
    }  /*then*/
    if (nameptr->next != NULL)
      nameptr->next->prev = nameptr->prev;
    if (nameptr->prev != NULL)
      nameptr->prev->next = nameptr->next;
    nextnameptr = nameptr->next;
    if (nameptr == definelist)
      definelist = nextnameptr;
    freenamelist(&nameptr);
    nameptr = nextnameptr;
  }  /*while*/
  return found;
}  /*CheckDefineList*/


Static Void startfunction(inputtoken, action, line, col)
stryng inputtoken;
int action, line, col;
{
  /*action 1 StartFunction and 25 StartGlobalFunct*/
  /*stackpops - nothing
                  pushes - tgsymptr*/
  /*description
          If we are at the outer most level, then check to see if the
          funct name is on the DefineList.  If so, remove it.  If this
          is not a Global function def, get a SymTblPtr and set it's type
          to TFunction.  If this is a global function def, make sure
          that the name did not appear on the DefineList.  Build the
          if1 function name by prepending any outer function names to
          this function name.  This is needed because if1 had a flat
          scope.  Start a new lexlevel of type Funct if this is a local
          function.  Build the function graph structure.  Save the
          SymTblPtr on the stack.*/
  semanticrec *semrec;
  stentry *ttptr;
  stryng functname;
  boolean exported;
  errorrecord *errorrec;

  if (semtrace) {
    if (action == 1)
      printf("Begin StartFunction\n");
    else
      printf("Begin StartGlobalFunct\n");
  }
  newsemantic(&semrec, tgsymptr);
  if (currentlevel->lleveltag == root)   /*if at outer level, check for */
    exported = checkdefinelist(inputtoken, action);   /*name on DefineList*/
  else
    exported = false;
  if (action == 1)
    semrec->UU.symptr = createbucket(tfunction);
  else {
    semrec->UU.symptr = createbucket(tglobal);
    if (exported) {   /* can't be both global and exported */
      errorrec = newerrorptr(expimporterr);
      errorrec->UU.errorstr = inputtoken;
      errorrec->linenumber = line;
      semerror(errorrec);
    }  /*then*/
  }
  functname = buildfunctname(inputtoken);
  setname(semrec->UU.symptr, inputtoken);
  setdefloc(semrec->UU.symptr, line, col);
  semrec->UU.symptr->graphinfo->port_ = 1;
  semrec->UU.symptr->graphinfo->litvalue = functname;
  newttptr(&ttptr, iftfunctiontype);
  semrec->UU.symptr->typeptr = ttptr;
  if (action == 1) {   /* if this is a local function */
    createlevel(funct);
    currentlevel->UU.lfunctname = semrec->UU.symptr;
  }
  buildfunctgraph(semrec->UU.symptr, functname, action, exported);
  pushsemantic(semrec);
}  /* startfunction */


Static Void enddefinelist()
{
  /*action 22 EndDefineList*/
  /*stackpops - tgnamelist
                  pushes - nothing*/
  /*description
          Save the namelist found on the stack in the variable called
          DefineList.  As functions at the outer most level are ,
          if their name occurs in the DefineList, it is removed.  At the
          end of compiling a modlue, no functions names should remain
          on the DefineList.*/
  semanticrec *semrec;

  if (semtrace)
    printf("Begin EndDefineList\n");
  semrec = popsemantic();
  definelist = semrec->UU.namelist;
  freesemantic(&semrec);
}


Static Void startforwardfunct(inputtoken, line, col)
stryng inputtoken;
int line, col;
{
  /*action 24 StartForwardFunct*/
  /*stackpops - nothing
                  pushes - tgsymptr*/
  /*description
          Get a symbol table pointer and set its tag to TForward.  Get a
          type table pointer for a function and connect it to the symptr.
          Save the symptr on the stack.*/
  semanticrec *semrec;
  stentry *ttptr;
  stryng functname;

  if (semtrace)
    printf("begin StartForwardFunct\n");
  newsemantic(&semrec, tgsymptr);
  semrec->UU.symptr = createbucket(tforward);
  setname(semrec->UU.symptr, inputtoken);
  setdefloc(semrec->UU.symptr, line, col);
  newttptr(&ttptr, iftfunctiontype);
  semrec->UU.symptr->typeptr = ttptr;
  functname = buildfunctname(inputtoken);
  semrec->UU.symptr->graphinfo->litvalue = functname;
  pushsemantic(semrec);
}


Local Void addfunctargtype(functttptr, argttptr)
stentry *functttptr, *argttptr;
{
  /* Add the type ArgTTptr to the argument type list of function type
     functTTptr.*/
  stentry *tupptr, *curtupptr;

  newttptr(&tupptr, ifttuple);
  tupptr->UU.U2.stelemtype = argttptr;
  curtupptr = functttptr->UU.U3.starg;
  if (curtupptr == NULL) {
    functttptr->UU.U3.starg = tupptr;
    return;
  }
  while (curtupptr->UU.U2.stnext != NULL)
    curtupptr = curtupptr->UU.U2.stnext;
  curtupptr->UU.U2.stnext = tupptr;

  /*else*/
}  /*AddFunctArgType*/


Static Void processparam()
{
  /*action 27 ProcessParam*/
  /*stackpops - tgTTPtr
                       - tgnamelist
                       - tgsymptr
                  pushes - tgsymptr*/
  /*description
          The tgsymptr is for a function name.  If this function isn't
          global or forward, get Stptr's for each name on the namelist
          and hash them in.  Get TypeTPtrs for each arg and link them
          together on the functions type table entry's arg list.
          Check to make sure all the arg names are distinct.  Push
          the STptr for the function back on the stack.*/
  semanticrec *semttptr, *semnamelist, *semsymptr;
  boolean realfunct;
  symtblbucket *symptr;
  namelistrec *param;
  int portnum;
  errorrecord *errorrec;

  if (semtrace)
    printf("Begin ProcessParam\n");
  semttptr = popsemantic();
  semnamelist = popsemantic();
  semsymptr = popsemantic();
  if (semsymptr->UU.symptr->identtag == tfunction)
    realfunct = true;
  else
    realfunct = false;
  param = semnamelist->UU.namelist;
  portnum = semsymptr->UU.symptr->graphinfo->port_;
  while (param != NULL) {
    if (realfunct) {
      if (incurrentlevel(param->name, tparam) != NULL) {
	errorrec = newerrorptr(doubledefarg);
	errorrec->UU.errorstr = param->name;
	errorrec->linenumber = param->linenum;
	semerror(errorrec);
      }
      symptr = createbucket(tparam);
      symptr->typeptr = semttptr->UU.ttptr;
      setname(symptr, param->name);
      setdefloc(symptr, param->linenum, param->column);
      addtotable(symptr, currentlevel);
      symptr->graphinfo->port_ = portnum;
      symptr->graphinfo->node_ = semsymptr->UU.symptr->graphinfo->node_;
      symptr->graphinfo->typeptr = semttptr->UU.ttptr;
    }  /*then*/
    addfunctargtype(semsymptr->UU.symptr->typeptr, semttptr->UU.ttptr);
    if (param->next != NULL) {
      param = param->next;
      freenamelist(&param->prev);
    }  /*then*/
    else {
      freenamelist(&param);
      param = NULL;
    }  /*else*/
    portnum++;
  }  /*while*/
  semsymptr->UU.symptr->graphinfo->port_ = portnum;
  freesemantic(&semnamelist);
  freesemantic(&semttptr);
  pushsemantic(semsymptr);
}


Local Void addfunctrestype(functttptr, resttptr)
stentry *functttptr, *resttptr;
{
  /* Add the type ResTTptr to the result type list of function type
     functTTptr.*/
  stentry *tupptr, *curtupptr;

  newttptr(&tupptr, ifttuple);
  tupptr->UU.U2.stelemtype = resttptr;
  curtupptr = functttptr->UU.U3.stres;
  if (curtupptr == NULL) {
    functttptr->UU.U3.stres = tupptr;
    return;
  }
  while (curtupptr->UU.U2.stnext != NULL)
    curtupptr = curtupptr->UU.U2.stnext;
  curtupptr->UU.U2.stnext = tupptr;

  /*else*/
}  /*AddFunctResType*/


Static Void doresultlist(typelist, symptr)
typelistrec *typelist;
symtblbucket *symptr;
{
  /* Build the result list for the function.*/
  typelistrec *curtypelist;
  stentry *functttptr;

  curtypelist = typelist;
  functttptr = symptr->typeptr;
  while (curtypelist != NULL) {
    addfunctrestype(functttptr, curtypelist->typeptr);
    if (curtypelist->next != NULL) {
      curtypelist = curtypelist->next;
      freetypelist(&curtypelist->prev);
    }  /*then*/
    else {
      freetypelist(&curtypelist);
      curtypelist = NULL;
    }  /*else*/
  }  /*while*/
}  /*DoResultList*/


Static boolean checklastlevel(name, itype, bucket)
stryng name;
identtype itype;
symtblbucket **bucket;
{
  /* Search level one back from current for a function of type Itype and
     name name.*/
  boolean found;
  int hashvalue;

  found = false;
  stringlowercase(&name);
  hashvalue = hash(stringchar(&name, 1), stringchar(&name, name.len));
  *bucket = currentlevel->lastlevel->hashtable[hashvalue];
  while (*bucket != NULL && !found) {
    if (equalstrings(&(*bucket)->name, &name)) {
      switch (itype) {

      case tforward:
	if ((*bucket)->identtag == tforward)
	  found = true;
	break;

      case tfunction:
	if ((*bucket)->identtag == tfunction)
	  found = true;
	break;
      }/*case*/
    }
    if (!found)
      *bucket = (*bucket)->nextbucket;
  }  /*while*/
  return found;
}  /*CheckLastLevel*/


Local Void freelist(list)
stentry *list;
{
  stentry *nexttypeptr, *freetypeptr;

  freetypeptr = list;
  if (freetypeptr == NULL) {
    return;
  }  /*then*/
  nexttypeptr = freetypeptr->UU.U2.stnext;
  while (nexttypeptr != NULL) {
    freettptr(&freetypeptr);
    freetypeptr = nexttypeptr;
    nexttypeptr = freetypeptr->UU.U2.stnext;
  }  /*while*/
  freettptr(&freetypeptr);
}  /*FreeList*/


Static Void freefuncttype(functtypeptr)
stentry *functtypeptr;
{  /*FreeTypeList*/
  freelist(functtypeptr->UU.U3.starg);
  freelist(functtypeptr->UU.U3.stres);
  freettptr(&functtypeptr);
}  /*FreeTypeList*/


Static Void endfunctionheader()
{
  /*action 29 EndFunctionHeader*/
  /*stackpops - tgtypelist
                         tgsymptr
                  pushes - tgsymptr*/
  /*description
          The symptr is for a function.  The typelist contains the types
          that the function returns.  Link the types on the list to the
          functions reslist in the type table.  Check for a function with
          the came name one level back.  If one exist declare an error.
          Check for a forward function one level back.  If one exists,
          check that it's type and the type of the one we are processing
          are the same.  If not, declare an error, else change the forward
          function to a real function.  Check that no other function has
          been defined in this scope with this name.  The manual says
          this is illegal, although most people believe it should be
          ok.  Smach the function into the type table and save it's
          symbol table entry on the stack.*/
  semanticrec *semtypelist, *semsymptr;
  stryng functname;
  treestackobj *levelfoundat;
  symtblbucket *bucketfoundin;
  stentry *smashedtypeptr, *functtypeptr, *forwardtypeptr;
  int linenum, colnum;
  node *fnode;
  errorrecord *errorrec;

  if (semtrace)
    printf("Begin EndFunctionHeader\n");
  semtypelist = popsemantic();
  semsymptr = popsemantic();
  doresultlist(semtypelist->UU.typelist, semsymptr->UU.symptr);
  functname = semsymptr->UU.symptr->name;
  if (checklastlevel(functname, tfunction, &bucketfoundin)) {
    errorrec = newerrorptr(fndoubdef);
    errorrec->UU.errorstr = functname;
    errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
    semerror(errorrec);
  }
  functtypeptr = semsymptr->UU.symptr->typeptr;
  if (checklastlevel(functname, tforward, &bucketfoundin))
      /*There is a forward*/
      {  /*function with this name*/
    forwardtypeptr = bucketfoundin->typeptr;
    smashedtypeptr = addtotypetable(functtypeptr);
    if (smashedtypeptr != forwardtypeptr) {
      errorrec = newerrorptr(forwardtype);
      errorrec->UU.errorstr = functname;   /*Forward funct type not the */
      errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
      semerror(errorrec);   /*same as this one, change it.*/
      semsymptr->UU.symptr->typeptr = smashedtypeptr;
      fnode = semsymptr->UU.symptr->graphinfo->node_;
      fnode->UU.U1.ndtype = smashedtypeptr;
    }  /*then*/
    else {  /*else fix the forward to a normal function*/
      linenum = getdefline(semsymptr->UU.symptr, 1);
      colnum = getdefcolumn(semsymptr->UU.symptr, 2);
      setdefloc(bucketfoundin, linenum, colnum);
      bucketfoundin->identtag = tfunction;
      bucketfoundin->graphinfo = semsymptr->UU.symptr->graphinfo;
      fnode = semsymptr->UU.symptr->graphinfo->node_;
      fnode->UU.U1.ndtype = smashedtypeptr;
      semsymptr->UU.symptr = bucketfoundin;
      freefuncttype(functtypeptr);
    }  /*else*/
  }  /*then*/
  else {
    /*not declared as forward, check for another
                                   function with this name.*/
    findname(functname, tfunction, &levelfoundat, &bucketfoundin);
    if (bucketfoundin != NULL) {
      if (bucketfoundin->identtag == tfunction) {  /* versus TGlobal*/
	errorrec = newerrorptr(innerfndoubdef);
	errorrec->UU.errorstr = functname;
	errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
	semerror(errorrec);
      }
    }
    smashedtypeptr = addtotypetable(functtypeptr);
    if (smashedtypeptr != functtypeptr) {
      semsymptr->UU.symptr->typeptr = smashedtypeptr;
      fnode = semsymptr->UU.symptr->graphinfo->node_;
      fnode->UU.U1.ndtype = smashedtypeptr;
      freefuncttype(functtypeptr);
    }
    addtotable(semsymptr->UU.symptr, currentlevel->lastlevel);
  }  /*else*/
  freesemantic(&semtypelist);
  pushsemantic(semsymptr);
}


Static Void endforwardglobheader()
{
  /*action 7 EndForwardGlobHeader*/
  /*stackpops - tgtypelist
                         tgsymptr
                  pushes - nothing*/
  /*description
          Finish building the function type by building the results list.
          Check for other functions in the current level with this name.
          Declare an error if one exists.  Hash the function name and
          smash its typeptr into the typetable.*/
  semanticrec *semtypelist, *semsymptr;
  stryng functname;
  errorrecord *errorrec;
  stentry *typeptr, *smashedtypeptr;
  node *fnode;

  if (semtrace)
    printf("begin EndForwardGlobHeader \n");
  semtypelist = popsemantic();
  semsymptr = popsemantic();
  doresultlist(semtypelist->UU.typelist, semsymptr->UU.symptr);
  functname = semsymptr->UU.symptr->name;
  if (incurrentlevel(functname, tfunction) != NULL) {
    errorrec = newerrorptr(fndoubdef);
    errorrec->UU.errorstr = functname;
    errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
    semerror(errorrec);
  }
  addtotable(semsymptr->UU.symptr, currentlevel);
  typeptr = semsymptr->UU.symptr->typeptr;
  smashedtypeptr = addtotypetable(typeptr);
  if (smashedtypeptr != typeptr) {
    semsymptr->UU.symptr->typeptr = smashedtypeptr;
    if (semsymptr->UU.symptr->identtag == tglobal) {
      fnode = semsymptr->UU.symptr->graphinfo->node_;
      fnode->UU.U1.ndtype = smashedtypeptr;
    }  /*then*/
    freefuncttype(typeptr);
  }  /*then*/
  freesemantic(&semsymptr);
  freesemantic(&semtypelist);
}


Static Void endfunctionlist()
{
  /*action 30 EndFunctionList*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Search the current level in the symbol table and look for forward
          functions that have not had a local function defined for them.  They
          will still have IdentTag = TForward.  For each one found declare
          an error.*/
  int hashvalue;
  symtblbucket *bucket;
  errorrecord *errorrec;

  if (semtrace)
    printf("begin EndFunctionList\n");
  for (hashvalue = 0; hashvalue <= maxhashtable; hashvalue++) {
    bucket = currentlevel->hashtable[hashvalue];
    while (bucket != NULL) {
      if (bucket->identtag == tforward) {
	errorrec = newerrorptr(forwardnotresolved);
	errorrec->UU.errorstr = bucket->name;
	errorrec->linenumber = getdefline(bucket, 1);
	semerror(errorrec);
      }  /*then*/
      bucket = bucket->nextbucket;
    }  /*while*/
  }  /*for*/
}


Static Void endfunction()
{
  /*action 31 EndFunction*/
  /*stackpops - tgexplist
                         tgsymptr
                  pushes - nothing*/
  /*description
          Check that the expression list matches the returns type for
          the function.  This means checking the types and the arity
          of the expression.  Connect the outputs of the expression to
          N0 of the function, they go on the functions I-List.  Call
          EndLevel, which will do a xref if it was requested and free
          up the lex level structures.*/
  semanticrec *semexplist, *semsymptr;
  explistnode *exp;
  stentry *typeptr;
  errorrecord *errorrec;
  int rescnt;
  inforecord *functgraphinfo;

  if (semtrace)
    printf("Begin EndFunction\n");
  semexplist = popsemantic();
  semsymptr = popsemantic();
  functgraphinfo = semsymptr->UU.symptr->graphinfo;
  rescnt = 1;
  exp = semexplist->UU.explist;
  typeptr = semsymptr->UU.symptr->typeptr->UU.U3.stres;
  do {
    if (exp->graphinfo->typeptr != typeptr->UU.U2.stelemtype) {
      errorrec = newerrorptr(typefunctres);
      errorrec->UU.U19.functname = semsymptr->UU.symptr->name;
      errorrec->UU.U19.resnum = rescnt;
      if (getdefline(semsymptr->UU.symptr, 2) < 1)
	errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
      else
	errorrec->linenumber = getdefline(semsymptr->UU.symptr, 2);
      semerror(errorrec);
    }
    functgraphinfo->port_ = rescnt;
    addedge(exp->graphinfo, functgraphinfo);
    exp = exp->next;
    typeptr = typeptr->UU.U2.stnext;
    rescnt++;
  } while (exp != NULL && typeptr != NULL);
  if (exp != NULL || typeptr != NULL) {
    errorrec = newerrorptr(numfunctres);
    errorrec->UU.U19.functname = semsymptr->UU.symptr->name;
    if (getdefline(semsymptr->UU.symptr, 2) < 1)
      errorrec->linenumber = getdefline(semsymptr->UU.symptr, 1);
    else
      errorrec->linenumber = getdefline(semsymptr->UU.symptr, 2);
    if (exp == NULL)   /*too many result expressions */
      errorrec->UU.U19.resnum = -1;   /*not enough result expressions*/
    else
      errorrec->UU.U19.resnum = 1;
    semerror(errorrec);
  }  /*then*/
  endlevel();
}


Static Void dofunctcall()
{
  /*action 108 DoFunctionCall*/
  /*stackpops - tgexplist
                         tgnamelist
                  pushes - tgexplist*/
  /*description
          Get a call node.  Find the name in the symbol table as a function
          or global.  If it isn't there, declare an error.  Connect the <NAME>
          on the list to the 1st input port of the call node.  Step thru
          the expression list and the arg list for the function (in the
          type table).  Check that the type of the exp matches the type of
          the arg.  Connect the exp to the next input port of the call node.
          Make sure there are the correct number of args.  If not, declare an
          error.  Build an expression list for the results of the expression
          and push it on the stack.*/
  semanticrec *semexplist, *semnamelist;
  stryng functname;
  treestackobj *level;
  int argcnt, rescnt;
  errorrecord *errorrec;
  inforecord *callgraphinfo, *paramgraphinfo;
  explistnode *exp;
  stentry *typeptr;
  symtblbucket *symptr;
  node *nd;

  semexplist = popsemantic();
  semnamelist = popsemantic();
  functname = semnamelist->UU.namelist->name;
  findname(functname, tfunction, &level, &symptr);
  if (symptr == NULL) {
    errorrec = newerrorptr(undeffunct);
    errorrec->UU.errorstr = functname;
    errorrec->linenumber = semnamelist->UU.namelist->linenum;
    semerror(errorrec);
    /*the function that is to be called is undefined.  Make up an explist
      that holds a literal "error" of type wild.*/
    semexplist->UU.explist = newexplistptr();
    string10(&semexplist->UU.explist->graphinfo->litvalue, "error     ");
    semexplist->UU.explist->graphinfo->typeptr = getbasictype(ifbwild);
    symptr = createbucket(tfunction);
    setname(symptr, functname);
    setdefloc(symptr, semnamelist->UU.namelist->linenum,
	      semnamelist->UU.namelist->column);
    typeptr = getbasictype(ifbwild);
    symptr->typeptr = typeptr;
    symptr->graphinfo = semexplist->UU.explist->graphinfo;
    addtotable(symptr, currentlevel);
    pushsemantic(semexplist);
    freesemantic(&semnamelist);
    return;
  }
  if (symptr->identtag == tpredefined) {
    dopredefined(symptr, semexplist, semnamelist->UU.namelist->linenum);
    return;
  }
  if (symptr->graphinfo->node_ == NULL && symptr->identtag != tforward)
      /* this funct is undef, but was */
      {  /* called before. */
    addref(symptr, semnamelist->UU.namelist->linenum,
	   semnamelist->UU.namelist->column);
    semexplist->UU.explist = newexplistptr();
    semexplist->UU.explist->graphinfo = symptr->graphinfo;
    pushsemantic(semexplist);
    freesemantic(&semnamelist);
    return;
  }  /*then*/
  addref(symptr, semnamelist->UU.namelist->linenum,
	 semnamelist->UU.namelist->column);
  nd = newnodeptr(ndatomic, currentlevel);
  nd->ndcode = ifncall;
  nd->ndsrcline = semnamelist->UU.namelist->linenum;
  linkparent(nd, currentlevel);
  paramgraphinfo = newinfoptr();   /*make a literal for funct name*/
  paramgraphinfo->litvalue = symptr->graphinfo->litvalue;
  paramgraphinfo->typeptr = symptr->typeptr;
  callgraphinfo = newinfoptr();
  callgraphinfo->node_ = nd;
  callgraphinfo->port_ = 1;
  addedge(paramgraphinfo, callgraphinfo);
  exp = semexplist->UU.explist;
  typeptr = symptr->typeptr->UU.U3.starg;
  argcnt = 1;
  while (exp != NULL && typeptr != NULL) {
    argcnt++;
    if (exp->graphinfo->typeptr != typeptr->UU.U2.stelemtype) {
      errorrec = newerrorptr(typefunctarg);
      errorrec->UU.U29.functionname = functname;
      errorrec->UU.U29.argnum = argcnt - 1;
      errorrec->linenumber = semnamelist->UU.namelist->linenum;
      semerror(errorrec);
    }  /*then*/
    callgraphinfo->port_ = argcnt;
    addedge(exp->graphinfo, callgraphinfo);
    exp = exp->next;
    typeptr = typeptr->UU.U2.stnext;
  }  /*while*/
  if (exp != NULL || typeptr != NULL) {
    errorrec = newerrorptr(numfunctarg);
    errorrec->UU.U29.functionname = functname;
    errorrec->linenumber = semnamelist->UU.namelist->linenum;
    if (exp == NULL)   /*Too many args*/
      errorrec->UU.U29.argnum = -1;   /*not enough args*/
    else
      errorrec->UU.U29.argnum = 1;
    semerror(errorrec);
  }  /*then*/
  /* push an expression list to represent the results  */
  rescnt = 1;
  typeptr = symptr->typeptr->UU.U3.stres;
  semexplist->UU.explist = newexplistptr();
  semexplist->UU.explist->graphinfo->node_ = nd;
  semexplist->UU.explist->graphinfo->port_ = rescnt;
  semexplist->UU.explist->graphinfo->typeptr = typeptr->UU.U2.stelemtype;
  exp = semexplist->UU.explist;
  typeptr = typeptr->UU.U2.stnext;
  while (typeptr != NULL) {
    rescnt++;
    exp->next = newexplistptr();
    exp->next->graphinfo->node_ = nd;
    exp->next->graphinfo->port_ = rescnt;
    exp->next->graphinfo->typeptr = typeptr->UU.U2.stelemtype;
    typeptr = typeptr->UU.U2.stnext;
    exp = exp->next;
  }  /*while*/
  pushsemantic(semexplist);
  freesemantic(&semnamelist);

  /*else*/
}


Static Void endprogram()
{
  /*action 141  EndProgram*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Check that all functions listed in the  statement have
          been declared at the outer most level.  This means that the
          DefineList should be empty.  Declare and error for each name
          on the list.*/
  errorrecord *errorrec;

  while (definelist != NULL) {
    errorrec = newerrorptr(deflistnotempty);
    errorrec->UU.errorstr = definelist->name;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    definelist = definelist->next;
  }  /*while*/
}


Static Void startif(iftype)
lexleveltype iftype;
{
  /*action 79 StartIf*/
  /*stackpops - tgexplist
                  pushes - tgSubGraphNum*/
  /*description
          Check that the explist on the stack has arity one and type
          boolean.  Start an IF compound node and a subgraph for the
          then part.  Put an entry on the association list for the
          then part.  Connect the expression to port 1 of the compound
          node.  Save a 1 on the stack to signify that the IF has one
          subgraph so far.*/
  errorrecord *errorrec;
  semanticrec *semexplist;
  inforecord *graphinfo, *compnodegi;
  semanticrec *semsubgraphnum;
  /* node *subgraph; */

  if (semtrace)
    printf("begin StartIf\n");
  semexplist = popsemantic();
  compnodegi = newinfoptr();
  compnodegi->node_ = buildcompound_(iftype);
  compnodegi->port_ = 1;
  if (semexplist->UU.explist->next != NULL) {
    errorrec = newerrorptr(predarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  if (semexplist->UU.explist->graphinfo->typeptr != getbasictype(ifbboolean)) {
    errorrec = newerrorptr(prednotbool);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
    graphinfo = newinfoptr();
    graphinfo->typeptr = getbasictype(ifbboolean);
    string10(&graphinfo->litvalue, "error     ");
    addedge(graphinfo, compnodegi);
  } else
    addedge(semexplist->UU.explist->graphinfo, compnodegi);
  addtoassolist(compnodegi->node_, 1);
  /* subgraph = */ (Void)buildsubgraph(currentlevel->UU.U8.ifnode);
  freesemantic(&semexplist);
  newsemantic(&semsubgraphnum, tgsubgraphnum);
  semsubgraphnum->UU.subgraphnum = 1;
  pushsemantic(semsubgraphnum);
}


Static Void startifsubgraph()
{
  /*action 81 StartIfSubGraph*/
  /*stackpops - tgSubGraphNum
                  pushes - tgSubGraphNum*/
  /*description
          Call BuildSubGraph and add an entry to the association list
          for the next subgraph.  Search the symbol table for entries
          whose graphinfo node or oldfrom node is the old SG.  Change
          it to the new SG.  Increment the SubGraphNum and save it on
          the stack.*/
  semanticrec *semsubgraphnum;
  node *newsubgraph, *oldsubgraph;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf("Begin StartIfSubgraph \n");
  semsubgraphnum = popsemantic();
  oldsubgraph = currentlevel->currentsubn0;
  newsubgraph = buildsubgraph(currentlevel->UU.U8.ifnode);
  hashindex = 0;
  while (hashindex <= maxhashtable) {
    chain = currentlevel->hashtable[hashindex];
    while (chain != NULL) {
      if (oldsubgraph == chain->graphinfo->node_)
	chain->graphinfo->node_ = newsubgraph;
      if (chain->identtag == tvariable) {
	if (oldsubgraph == chain->UU.U4.oldfrom->node_)
	  chain->UU.U4.oldfrom->node_ = newsubgraph;
      }
      chain = chain->nextbucket;
    }  /*while*/
    hashindex++;
  }  /*while*/
  semsubgraphnum->UU.subgraphnum++;
  addtoassolist(currentlevel->UU.U8.ifnode, semsubgraphnum->UU.subgraphnum);
  pushsemantic(semsubgraphnum);
}


Static Void endiftagsubgraph(predflag)
boolean predflag;
{
  /*stackpops - tgexplist
                  pushes - nothing*/
  semanticrec *semexplist;
  typelistrec *typelist, *tp1, *tp2;
  int portnum;
  explistnode *explist;
  inforecord *subgraphgi;
  errorrecord *errorrec;

  semexplist = popsemantic();
  typelist = NULL;
  portnum = 1;
  explist = semexplist->UU.explist;
  subgraphgi = newinfoptr();
  subgraphgi->node_ = currentlevel->currentsubn0;
  while (explist != NULL) {
    subgraphgi->port_ = portnum;
    addedge(explist->graphinfo, subgraphgi);
    portnum++;
    linktype(explist->graphinfo->typeptr, &typelist);
    explist = explist->next;
  }  /*while*/
  portnum = 1;
  switch (currentlevel->lleveltag) {

  case nestedifcomp:
  case ifcomp:
    if (predflag) {
      if (semexplist->UU.explist->next != NULL) {
	errorrec = newerrorptr(predarity);
	errorrec->linenumber = linenumber;
	semerror(errorrec);
      }
      if (semexplist->UU.explist->graphinfo->typeptr !=
	  getbasictype(ifbboolean)) {
	errorrec = newerrorptr(prednotbool);
	errorrec->linenumber = linenumber;
	semerror(errorrec);
      }
    }  /*then*/
    else if (currentlevel->UU.U8.iarmreturnstype == NULL)
      currentlevel->UU.U8.iarmreturnstype = typelist;
    else {
      tp1 = typelist;
      tp2 = currentlevel->UU.U8.iarmreturnstype;
      while (tp1 != NULL && tp2 != NULL) {
	if (tp1->typeptr != tp2->typeptr) {
	  errorrec = newerrorptr(ifreturnstypes);
	  errorrec->linenumber = linenumber;
	  errorrec->UU.expnum = portnum;
	  semerror(errorrec);
	}
	portnum++;
	tp1 = tp1->next;
	tp2 = tp2->next;
      }  /*while*/
      if (tp1 != tp2) {
	errorrec = newerrorptr(numifreturns);
	errorrec->linenumber = linenumber;
	if (tp1 == NULL)
	  errorrec->UU.expnum = -1;
	else
	  errorrec->UU.expnum = 1;
	semerror(errorrec);
      }  /*then*/
    }
    break;

  case tagcase:
    if (currentlevel->UU.U4.tarmreturnstype == NULL)
      currentlevel->UU.U4.tarmreturnstype = typelist;
    else {
      tp1 = typelist;
      tp2 = currentlevel->UU.U4.tarmreturnstype;
      while (tp1 != NULL && tp2 != NULL) {
	if (tp1->typeptr != tp2->typeptr) {
	  errorrec = newerrorptr(tagreturnstypes);
	  errorrec->linenumber = linenumber;
	  errorrec->UU.expnum = portnum;
	  semerror(errorrec);
	}
	portnum++;
	tp1 = tp1->next;
	tp2 = tp2->next;
      }  /*while*/
      if (tp1 != tp2) {
	errorrec = newerrorptr(numtagreturns);
	errorrec->linenumber = linenumber;
	if (tp1 == NULL)
	  errorrec->UU.expnum = -1;
	else
	  errorrec->UU.expnum = 1;
	semerror(errorrec);
      }  /*then*/
    }  /*else*/
    break;
  }/* case */

  /*else*/
}  /*EndIfTagSubGraph*/


Static Void endifsubgraph()
{
  /*action 82 EndIFSubGraph*/
  /*stackpops - nothing
                  pushes - nothing*/
  /*description
          Just call EndIfTagSubGraph with the predflag set to false.*/
  if (semtrace)
    printf("begin EndIFSubGraph \n");
  endiftagsubgraph(false);
}


/*procedure EndPredSubGraph;
(*action 82 EndPredSubGraph*)
(*stackpops - nothing
                pushes - nothing*)
(*description
        Just call EndIfTagSubGraph with the predflag set to true.*)
begin
  if SemTrace then
    writeln('begin EndPredSubGraph ');
  EndIfTagSubGraph(true);
end;
*/
Static Void endif()
{
  /*action 85  EndIf*/
  /*stackpops - tgSubGraphNum
                  pushes - tgexplist  (for IF outputs)*/
  /*description
          Pop the SubGraphNum off the stack and call EndCompound.*/
  semanticrec *semrec;

  if (semtrace)
    printf("begin EndIf \n");
  /* end all the nested ones */
  while (currentlevel->lleveltag != ifcomp) {
    semrec = popsemantic();
    freesemantic(&semrec);
    endcompound();
    endiftagsubgraph(false);
  }  /*while*/
  semrec = popsemantic();
  freesemantic(&semrec);
  endcompound();
}


Static Void findandpushname()
{
  /*action 137 FindAndPushName*/
  /*stack                pops - TGNameList
                         pushes - TGExpList   */
  /*description
          Look for the name is the symbol table. If it is not found then
          create a new symbol table entry of type wild.  Check that the
          name has been defined (Def loc 1 is not empty for a non loop var
          or def loc 2 is not empty for a loop var).  If the loop flag is
          LInit, set it to LConst.  This means the name was used with out
          old in the body before it was redefined, which forces it to be
          a loop constant.  Call pull name to bring the value into this
          scope and add a reference to its reference list.  Push the graph
          info of the symbol table entry.*/
  semanticrec *namel, *esem;
  treestackobj *level;
  symtblbucket *sym;
  stentry *wtype;
  errorrecord *errorrec;
  explistnode *expl;
  inforecord *outinfo;

  if (semtrace)
    printf("InFindAndPushName\n");
  namel = popsemantic();
  findname(namel->UU.namelist->name, tvariable, &level, &sym);
  if (sym == NULL) {
    errorrec = newerrorptr(vnameundef);
    errorrec->UU.errorstr = namel->UU.namelist->name;
    errorrec->linenumber = namel->UU.namelist->linenum;
    semerror(errorrec);
    sym = getsymtabentry(namel->UU.namelist->name, tvariable);
    wtype = getbasictype(ifbwild);
    sym->typeptr = wtype;
    sym->graphinfo->typeptr = wtype;
    setdefloc(sym, namel->UU.namelist->linenum, namel->UU.namelist->column);
    sym->graphinfo->name = namel->UU.namelist->name;
    string10(&sym->graphinfo->litvalue, "error     ");
    stripspaces(&sym->graphinfo->name);
    sym->graphinfo->typeptr = wtype;
    addtotable(sym, currentlevel);
    outinfo = newinfoptr();
    outinfo->node_ = sym->graphinfo->node_;
    outinfo->port_ = sym->graphinfo->port_;
    outinfo->typeptr = sym->graphinfo->typeptr;
    outinfo->name = sym->graphinfo->name;
    outinfo->litvalue = sym->graphinfo->litvalue;
    outinfo->onlist = sym->graphinfo->onlist;
  }  /* if */
  else {
    if (getdefline(sym, 1) == -1) {
      if (getloopflag(sym) == lvar)
	errorrec = newerrorptr(lvarnotdef);
      else
	errorrec = newerrorptr(decbutnotdef);
      errorrec->UU.errorstr = sym->name;
      errorrec->linenumber = namel->UU.namelist->linenum;
      semerror(errorrec);
      outinfo = newinfoptr();
      string10(&outinfo->litvalue, "error     ");
      outinfo->name = sym->name;
      outinfo->typeptr = sym->typeptr;
    }  /* if */
    else if (sym->identtag == tvariable) {
      if ((getloopflag(sym) == lvar) & (getdefline(sym, 2) == -1)) {
	errorrec = newerrorptr(lvarusebredef);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = namel->UU.namelist->linenum;
	semerror(errorrec);
	outinfo = newinfoptr();
	string10(&outinfo->litvalue, "error     ");
	outinfo->name = sym->name;
	outinfo->typeptr = sym->typeptr;
      }  /* else if */
      else {
	if (getloopflag(sym) == linit)
	  *sym->UU.U4.loopflag = lconst;
	sym = pullname(sym, level, namel->UU.namelist->linenum);
	addref(sym, namel->UU.namelist->linenum, namel->UU.namelist->column);
	outinfo = newinfoptr();
	outinfo->node_ = sym->graphinfo->node_;
	outinfo->port_ = sym->graphinfo->port_;
	outinfo->typeptr = sym->graphinfo->typeptr;
	outinfo->name = sym->graphinfo->name;
	outinfo->litvalue = sym->graphinfo->litvalue;
	outinfo->onlist = sym->graphinfo->onlist;
      }  /* else */
    } else {
      sym = pullname(sym, level, namel->UU.namelist->linenum);
      addref(sym, namel->UU.namelist->linenum, namel->UU.namelist->column);
      outinfo = newinfoptr();
      outinfo->node_ = sym->graphinfo->node_;
      outinfo->port_ = sym->graphinfo->port_;
      outinfo->typeptr = sym->graphinfo->typeptr;
      outinfo->name = sym->graphinfo->name;
      outinfo->litvalue = sym->graphinfo->litvalue;
      outinfo->onlist = sym->graphinfo->onlist;
    }
  }  /* else */
  expl = newexplistptr();
  expl->graphinfo = outinfo;
  newsemantic(&esem, tgexplist);
  esem->UU.explist = expl;
  pushsemantic(esem);

  /* else if */
  /* else */
}


Static Void pushnilcons()
{
  /*action 129 PushNilCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type null and name inputtoken. Push
          this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushNilCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbnull);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushNilCons */


Static Void pushtruecons()
{
  /*action 130 PushTrueCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type boolean and name inputtoken.
           Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushTrueCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbboolean);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushTrueCons */


Static Void pushfalsecons()
{
  /*action 131 PushFalseCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type boolean and name inputtoken.
           Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushFalseCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbboolean);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushFalseCons */


Static Void pushintcons()
{
  /*action 132 PushIntCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type integer and name inputtoken.
           Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushIntCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbinteger);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushIntCons */


Static Void pushrealcons()
{
  /*action 133 PushRealCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type real and name inputtoken.
           Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushRealCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbreal);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushRealCons */


Static Void pushdoubcons()
{
  /*action 142 PushDoubCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
           Create an expression of type double_real and name inputtoken.
           Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushDoubCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbdouble);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushDoubCons */


Static Void pushcharcons()
{
  /*action 134 PushCharCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
          Create an expression of type character and name inputtoken.
          Push this expression.*/
  explistnode *exp;
  semanticrec *sem;

  if (semtrace)
    printf("PushCharCons\n");
  exp = newexplistptr();
  exp->graphinfo->typeptr = getbasictype(ifbcharacter);
  if (stringchar(&inputtoken, 2) == '"')
    insertchar(&inputtoken, '\\', 2);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushCharCons */


Static Void pushstringcons()
{
  /*action 135 PushStringCons*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
          Create an expression of type array of character and name
          inputtoken. Push this expression.*/
  explistnode *exp;
  semanticrec *sem;
  stentry *parray;

  if (semtrace)
    printf("PushCharCons\n");
  exp = newexplistptr();
  newttptr(&parray, iftarray);
  parray->UU.stbasetype = getbasictype(ifbcharacter);
  exp->graphinfo->typeptr = addtotypetable(parray);
  exp->graphinfo->litvalue = inputtoken;
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushStringCons */


Static Void pusherrorcons()
{
  /*action 136 PushErrorCons*/
  /*stack        pops - TGTTPtr
                 pushes - TGExpList*/
  /*description
          Create an expression with type of type table pointer and name
          inputtoken. Push this expression.*/
  explistnode *exp;
  semanticrec *sem, *semtype;

  if (semtrace)
    printf("PushErrorCons\n");
  semtype = popsemantic();
  exp = newexplistptr();
  exp->graphinfo->typeptr = addtotypetable(semtype->UU.ttptr);
  string10(&exp->graphinfo->litvalue, "error     ");
  newsemantic(&sem, tgexplist);
  sem->UU.explist = exp;
  pushsemantic(sem);
}  /* PushErrorCons */


Static Void buildchar()
{
  /*action 114 BuildChar*/
  /*stack        pops - TGExpList
                 pushes - TGExpList*/
  /*description
          Check that expression is of type integer.  Build a character
          type casting node. Connect the expression to port one of this
          node. Push the output graph info of the character node.*/
  semanticrec *sem;
  errorrecord *errorrec;
  boolean terr;
  node *charnode;
  inforecord *graph_, *info;

  if (semtrace)
    printf("BuildChar\n");
  sem = popsemantic();
  if (sem->UU.explist->next != NULL) {
    errorrec = newerrorptr(chararity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  terr = false;
  graph_ = sem->UU.explist->graphinfo;
  if (graph_->typeptr == NULL)
    graph_->typeptr = getbasictype(ifbwild);
  if (graph_->typeptr->stsort != iftbasic)
    terr = true;
  else {
    if (graph_->typeptr->UU.stbasic != ifbinteger)
      terr = true;
  }
  if (terr) {
    errorrec = newerrorptr(charitype);
    errorrec->UU.typeptr = graph_->typeptr;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  charnode = newnodeptr(ndatomic, currentlevel);
  charnode->ndcode = ifnchar;
  charnode->ndsrcline = linenumber;
  linkparent(charnode, currentlevel);
  info = newinfoptr();
  info->node_ = charnode;
  info->port_ = 1;
  addedge(graph_, info);
  info->typeptr = getbasictype(ifbcharacter);
  sem->UU.explist->graphinfo = info;
  sem->UU.explist->next = NULL;
  pushsemantic(sem);
}  /* BuildChar */


Static Void builddouble()
{
  /*action 116 BuildDouble*/
  /*stack        pops - TGExpList
                 pushes - TGExpList*/
  /*description
          Check that expression is of type integer or real.  Build a
          double type casting node. Connect the expression to port one of
          this node. Push the output graph info of the double node.*/
  semanticrec *sem;
  errorrecord *errorrec;
  boolean terr;
  node *doublenode;
  inforecord *graph_, *info;

  if (semtrace)
    printf("BuildDouble\n");
  sem = popsemantic();
  if (sem->UU.explist->next != NULL) {
    errorrec = newerrorptr(doublearity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  terr = false;
  graph_ = sem->UU.explist->graphinfo;
  if (graph_->typeptr == NULL)
    graph_->typeptr = getbasictype(ifbwild);
  if (graph_->typeptr->stsort != iftbasic)
    terr = true;
  else {
    if (graph_->typeptr->UU.stbasic != ifbinteger &&
	graph_->typeptr->UU.stbasic != ifbreal)
      terr = true;
  }
  if (terr) {
    errorrec = newerrorptr(doubleitype);
    errorrec->UU.typeptr = graph_->typeptr;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  doublenode = newnodeptr(ndatomic, currentlevel);
  doublenode->ndcode = ifndouble;
  doublenode->ndsrcline = linenumber;
  linkparent(doublenode, currentlevel);
  info = newinfoptr();
  info->node_ = doublenode;
  info->port_ = 1;
  addedge(graph_, info);
  info->typeptr = getbasictype(ifbdouble);
  sem->UU.explist->graphinfo = info;
  sem->UU.explist->next = NULL;
  pushsemantic(sem);
}  /* BuildDouble */


Static Void buildint()
{
  /*action 118 BuildInt*/
  /*stack        pops - TGExpList
                 pushes - TGExpList*/
  /*description
          Check that expression is of type double, character or real.
          Build a integer type casting node. Connect the expression to
          port one of this node. Push the output graph info of the
          integer node.*/
  semanticrec *sem;
  errorrecord *errorrec;
  boolean terr;
  node *intnode;
  inforecord *graph_, *info;

  if (semtrace)
    printf("BuildInt\n");
  sem = popsemantic();
  if (sem->UU.explist->next != NULL) {
    errorrec = newerrorptr(intarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  terr = false;
  graph_ = sem->UU.explist->graphinfo;
  if (graph_->typeptr == NULL)
    graph_->typeptr = getbasictype(ifbwild);
  if (graph_->typeptr->stsort != iftbasic)
    terr = true;
  else {
    if (graph_->typeptr->UU.stbasic != ifbreal &&
	graph_->typeptr->UU.stbasic != ifbdouble &&
	graph_->typeptr->UU.stbasic != ifbcharacter)
      terr = true;
  }
  if (terr) {
    errorrec = newerrorptr(intitype);
    errorrec->UU.typeptr = graph_->typeptr;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  intnode = newnodeptr(ndatomic, currentlevel);
  intnode->ndcode = ifnint;
  intnode->ndsrcline = linenumber;
  linkparent(intnode, currentlevel);
  info = newinfoptr();
  info->node_ = intnode;
  info->port_ = 1;
  addedge(graph_, info);
  info->typeptr = getbasictype(ifbinteger);
  sem->UU.explist->graphinfo = info;
  sem->UU.explist->next = NULL;
  pushsemantic(sem);
}  /* BuildInt */


Static Void buildreal()
{
  /*action 120 BuildReal*/
  /*stack        pops - TGExpList
                 pushes - TGExpList*/
  /*description
          Check that expression is of type integer or real.
          Build a real type casting node. Connect the expression to
          port one of this node. Push the output graph info of the
          real node.*/
  semanticrec *sem;
  errorrecord *errorrec;
  boolean terr;
  node *realnode;
  inforecord *graph_, *info;

  if (semtrace)
    printf("BuildReal\n");
  sem = popsemantic();
  if (sem->UU.explist->next != NULL) {
    errorrec = newerrorptr(realarity);
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  terr = false;
  graph_ = sem->UU.explist->graphinfo;
  if (graph_->typeptr == NULL)
    graph_->typeptr = getbasictype(ifbwild);
  if (graph_->typeptr->stsort != iftbasic)
    terr = true;
  else {
    if (graph_->typeptr->UU.stbasic != ifbinteger &&
	graph_->typeptr->UU.stbasic != ifbdouble)
      terr = true;
  }
  if (terr) {
    errorrec = newerrorptr(realitype);
    errorrec->UU.typeptr = graph_->typeptr;
    errorrec->linenumber = linenumber;
    semerror(errorrec);
  }
  realnode = newnodeptr(ndatomic, currentlevel);
  realnode->ndcode = ifnsingle;
  realnode->ndsrcline = linenumber;
  linkparent(realnode, currentlevel);
  info = newinfoptr();
  info->node_ = realnode;
  info->port_ = 1;
  addedge(graph_, info);
  info->typeptr = getbasictype(ifbreal);
  sem->UU.explist->graphinfo = info;
  sem->UU.explist->next = NULL;
  pushsemantic(sem);
}  /* BuildReal */


Local Void createsub(tagcasenode, tagvalue)
node *tagcasenode;
tagflag tagvalue;
{
  node *subn0;
  inforecord *lit, *nod;

  subn0 = buildsubgraph(tagcasenode);
  lit = newinfoptr();
  lit->typeptr = getbasictype(ifbboolean);
  if (tagvalue == truetag)
    string10(&lit->litvalue, "true      ");
  else if (tagvalue == falsetag)
    string10(&lit->litvalue, "false     ");
  else
    string10(&lit->litvalue, "error     ");
  nod = newinfoptr();
  nod->node_ = subn0;
  nod->port_ = 1;
  addedge(lit, nod);
}  /* CreateSub */

Local Void changeto2(assolist)
assoclist *assolist;
{
  while (assolist != NULL) {
    assolist->graphnum = 2;
    assolist = assolist->next;
  }  /* while */
}  /* ChangeTo2 */


Static Void createisunion(line, col)
int line, col;
{
  /*action 121 CreateUsUnion*/
  /*stack        pops - TGExpList
                        TGNameList
                 pushes - TGExpList*/
  /*description
          Create a tagcae node the three subgraphs false, true and error.
          Look for the name as a tag of the union. While searching the
          tag list make entries into the association list such that every
          tag whose name does not match the name asseciates with the false
          subgraph and the tag with the name that matches with the true
          subgraph.  If the name is not found in the tag list then change
          the association list to associate to the error subgraph in all
          cases. Push the union expression back onto the stack.*/
  semanticrec *expl, *namel;
  explistnode *exp;
  namelistrec *name;
  errorrecord *errorrec;
  node *tagcasenode;
  boolean found, badtype;
  stentry *tags;
  inforecord *info;
  stryng nname, tname;
  treestackobj *level;

  if (semtrace)
    printf("CreateIsUnion\n");
  expl = popsemantic();
  namel = popsemantic();
  exp = expl->UU.explist;
  name = namel->UU.namelist;
  if (exp->next != NULL) {
    exp->next = NULL;
    errorrec = newerrorptr(arityonisu);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  badtype = false;
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftunion) {
    badtype = true;
    errorrec = newerrorptr(isutype);
    errorrec->UU.typeptr = exp->graphinfo->typeptr;
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  tagcasenode = buildcompound_(tagcase);
  createsub(tagcasenode, falsetag);
  createsub(tagcasenode, truetag);
  createsub(tagcasenode, errortag);
  if (!badtype) {
    tags = exp->graphinfo->typeptr->UU.stbasetype;
    found = false;
    while (tags != NULL) {
      tname = tags->stliteral;
      nname = name->name;
      stringlowercase(&tname);
      stringlowercase(&nname);
      if (equalstrings(&nname, &tname)) {
	addtoassolist(tagcasenode, 1);
	found = true;
      }  /* if */
      else
	addtoassolist(tagcasenode, 0);
      tags = tags->UU.U2.stnext;
    }  /* while */
    if (!found)
      changeto2(tagcasenode->UU.U2.ndassoc);
  }  /* if */
  else
    addtoassolist(tagcasenode, 2);
  info = newinfoptr();
  info->node_ = tagcasenode;
  info->port_ = 1;
  addedge(exp->graphinfo, info);
  info->typeptr = getbasictype(ifbboolean);
  expl->UU.explist->graphinfo = info;
  level = currentlevel;
  currentlevel = currentlevel->lastlevel;
  currentlevel->nextlevel = NULL;
  freetreestack(level);
  pushsemantic(expl);
}  /* CreateIsUnion */


Static Void checkold(inputtoken, line, column)
stryng inputtoken;
int line, column;
{
  /*action 122 CheckOld*/
  /*stack        pops - nothing
                 pushes - TGExpList*/
  /*description
          Find inputtoken in the symbol table. If it doesn't exist then
          declare and error and put it in the symbol table with type wild.
          If it is a LInit, then make it an LVar.  Copy the graphinfo into
          the OldFrom info.  I fit is not an LInit or Lvar then error,
          can not use old with this name.  Push the graph info in the
          symbol table onto the stack.*/
  semanticrec *esem;
  treestackobj *level;
  symtblbucket *sym;
  stentry *wtype;
  errorrecord *errorrec;
  explistnode *expl;
  loopvarflags vartype;

  if (semtrace)
    printf("CheckOld\n");
  findname(inputtoken, tvariable, &level, &sym);
  expl = newexplistptr();
  if (sym == NULL) {
    errorrec = newerrorptr(vnameundef);
    errorrec->UU.errorstr = inputtoken;
    errorrec->linenumber = line;
    errorrec->column = column;
    semerror(errorrec);
    sym = getsymtabentry(inputtoken, tvariable);
    wtype = getbasictype(ifbwild);
    sym->typeptr = wtype;
    sym->graphinfo->typeptr = wtype;
    setdefloc(sym, line, column);
    addtotable(sym, currentlevel);
    expl->graphinfo = sym->graphinfo;
  }  /* if */
  else {
    addref(sym, line, column);
    if (getdefline(sym, 1) == -1) {
      if (getloopflag(sym) == lvar)
	errorrec = newerrorptr(lvarnotdef);
      else
	errorrec = newerrorptr(decbutnotdef);
      errorrec->UU.errorstr = sym->name;
      errorrec->linenumber = line;
      semerror(errorrec);
      expl->graphinfo = newinfoptr();
      string10(&expl->graphinfo->litvalue, "error     ");
      expl->graphinfo->name = sym->name;
      expl->graphinfo->typeptr = sym->typeptr;
    }  /* if undefined */
    else {
      vartype = getloopflag(sym);
      if (vartype == linit) {
	*sym->UU.U4.loopflag = lvar;
	sym->UU.U4.oldfrom = sym->graphinfo;
	insertchar(&sym->UU.U4.oldfrom->name, '-', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'd', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'l', 1);
	insertchar(&sym->UU.U4.oldfrom->name, 'o', 1);
	sym->graphinfo = newinfoptr();
	sym->graphinfo->node_ = NULL;
	sym->graphinfo->typeptr = NULL;
	sym->graphinfo->onlist = NULL;
	sym = pullname(sym, level, line);
	expl->graphinfo = sym->UU.U4.oldfrom;
      }  /* else if */
      else if (vartype == lvar) {
	sym = pullname(sym, level, line);
	expl->graphinfo = sym->UU.U4.oldfrom;
      } else if (vartype == lconst) {
	errorrec = newerrorptr(loopcvmix);
	errorrec->UU.errorstr = sym->name;
	errorrec->linenumber = line;
	errorrec->column = column;
	sym = pullname(sym, level, line);
	expl->graphinfo = sym->graphinfo;
      } else {
	errorrec = newerrorptr(vnamenotloopvar);
	errorrec->UU.errorstr = inputtoken;
	errorrec->linenumber = line;
	errorrec->column = column;
	semerror(errorrec);
	sym = pullname(sym, level, line);
	expl->graphinfo = sym->graphinfo;
      }
    }  /* else */
  }  /* else */
  newsemantic(&esem, tgexplist);
  esem->UU.explist = expl;
  pushsemantic(esem);

  /* else if */
  /* else if */
  /* else */
}  /* CheckOld */


Static Void pushuplus(line, column)
int line, column;
{
  /*action 123 PushUPlus*/
  /*stack        pops - nothing
                 pushes - TGPrecLevel*/
  /*description
          Push a unary plus operation onto the stack.*/
  semanticrec *sem;

  if (semtrace)
    printf("PushUPlus\n");
  newsemantic(&sem, tgpreclevel);
  sem->UU.U5.preclevel = boostfac + 8;
  sem->UU.U5.operation = uplus;
  sem->UU.U5.line = line;
  sem->UU.U5.column = column;
  pushsemantic(sem);
}  /* PushUPLus */


Static Void pushuminus(line, column)
int line, column;
{
  /*action 124 PushUMinus*/
  /*stack        pops - nothing
                 pushes - TGPrecLevel */
  /*description
           Push a unary minus operation onto the stack.*/
  semanticrec *sem;

  if (semtrace)
    printf("PushUMinus\n");
  newsemantic(&sem, tgpreclevel);
  sem->UU.U5.preclevel = boostfac + 8;
  sem->UU.U5.operation = uminus;
  sem->UU.U5.line = line;
  sem->UU.U5.column = column;
  pushsemantic(sem);
}  /* PushUMinus */


Static Void pushunot(line, column)
int line, column;
{
  /*action 125 PushUNot*/
  /*stack        pops - nothing
                 pushes - TGPrecLevel*/
  /*description
          Push a unary not operation onto the stack.*/
  semanticrec *sem;

  if (semtrace)
    printf("PushUNot\n");
  newsemantic(&sem, tgpreclevel);
  sem->UU.U5.preclevel = boostfac + 3;
  sem->UU.U5.operation = unot;
  sem->UU.U5.line = line;
  sem->UU.U5.column = column;
  pushsemantic(sem);
}  /* PushUNot */


Static Void createiserror(line, column)
int line, column;
{
  /*action 127 CreateIsError*/
  /*stack        pops - TGExpList
                 pushes - TGExpList*/
  /* description
          Create an iserror node. Link the literal 'error' to it's first
          input port and expression to it's second.  Push the output graph
          info of the iserror node onto the stack.*/
  semanticrec *expl;
  node *iserrornode;
  inforecord *lit, *nod;
  errorrecord *errorrec;

  if (semtrace)
    printf("CreateIsError\n");
  expl = popsemantic();
  if (expl->UU.explist->next != NULL) {
    errorrec = newerrorptr(arityonerror);
    errorrec->linenumber = line;
    errorrec->column = column;
    semerror(errorrec);
  }
  iserrornode = newnodeptr(ndatomic, currentlevel);
  iserrornode->ndcode = ifniserror;
  iserrornode->ndsrcline = line;
  linkparent(iserrornode, currentlevel);
  lit = newinfoptr();
  lit->typeptr = expl->UU.explist->graphinfo->typeptr;
  string10(&lit->litvalue, "error     ");
  nod = newinfoptr();
  nod->node_ = iserrornode;
  nod->port_ = 1;
  addedge(lit, nod);
  nod->port_ = 2;
  addedge(expl->UU.explist->graphinfo, nod);
  nod->port_ = 1;
  nod->typeptr = getbasictype(ifbboolean);
  expl->UU.explist->graphinfo = nod;
  expl->UU.explist->next = NULL;
  pushsemantic(expl);
}  /* CreateIsError */


Static Void fixforfunct()
{
  /*action 42 FixForFunct*/
  /*stack        pops - TGNameList
                 pushes - TGSymPtr
                          TGNameList*/
  /*description
           Place a null namelist under the current name list.  this
          leaves the semantic stack in the correct form for a function
          call inside a tagcase header.*/
  semanticrec *namelist, *symptr;

  if (semtrace)
    printf(" In FixForFunct\n");
  namelist = popsemantic();
  newsemantic(&symptr, tgsymptr);
  pushsemantic(symptr);
  pushsemantic(namelist);
}  /* FixForFunct */


Static stentry *errorstruct()
{
  semanticrec *nsem, *tsem, *errstr;

  pushunion(noaddtottable);
  newsemantic(&nsem, tgnamelist);
  newnamelist(&nsem->UU.namelist);
  string10(&nsem->UU.namelist->name, "error     ");
  stripspaces(&nsem->UU.namelist->name);
  pushsemantic(nsem);
  newsemantic(&tsem, tgttptr);
  tsem->UU.ttptr = getbasictype(ifbwild);
  pushsemantic(tsem);
  buildtag(noaddtottable);
  errstr = popsemantic();
  return (addtotypetable(errstr->UU.ttptr));
}  /* ErrorStruct */


Static Void fixfortag(line, col)
int line, col;
{
  /*action 115 FixForTag*/
  /*stack        pops - TGNameList
                 pushes - TGExpList*/
  /* description
           Looks for name in symbol table and pushes it's graph info onto
           the stack.  If the name is not in the symbol table then an
           error union is built.*/
  semanticrec *namel, *expl;
  namelistrec *name;
  treestackobj *level;
  symtblbucket *sym;
  errorrecord *errorrec;

  namel = popsemantic();
  name = namel->UU.namelist;
  newsemantic(&expl, tgexplist);
  expl->UU.explist = newexplistptr();
  findname(name->name, tvariable, &level, &sym);
  if (sym == NULL) {
    errorrec = newerrorptr(undefntag);
    errorrec->UU.errorstr = name->name;
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    string10(&expl->UU.explist->graphinfo->litvalue, "error     ");
    string10(&expl->UU.explist->graphinfo->name, "error     ");
    stripspaces(&expl->UU.explist->graphinfo->name);
    expl->UU.explist->graphinfo->typeptr = errorstruct();
  }  /* if */
  else {
    sym = pullname(sym, level, line);
    expl->UU.explist->graphinfo = sym->graphinfo;
  }  /* else */
  pushsemantic(expl);
}  /* FixForTag */


Static Void processtaghead(line, col)
int line, col;
{
  /*action 43 ProcessTaghead*/
  /*stack        pops - TGExpList
                        TGNameList
                 pushes - TGSubGraphNum
                          TGTCStackList
                          TGSymPtr*/
  /*description
          Check that expression is of arity one and type union.
          Build a tagcase node, connect the expression to port one of the
          tagcase node. Create a new subgraph number semantic record.
          Push the ExpList and the subgraph number.*/
  semanticrec *expl, *subnum, *semnamelist, *semsymptr, *semtclist;
  explistnode *exp;
  errorrecord *errorrec;
  inforecord *toinfo;
  node *tagnode;
  symtblbucket *symptr;
  tcstacklistrec *tclist, *tclistend;
  stentry *typeptr;

  if (semtrace)
    printf(" In ProcessTagHead\n");
  expl = popsemantic();
  semnamelist = popsemantic();
  newsemantic(&semtclist, tgtcstacklist);
  semtclist->UU.tcstacklist = NULL;
  exp = expl->UU.explist;
  if (exp->next != NULL) {
    exp->next = NULL;
    errorrec = newerrorptr(arityontag);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  if (exp->graphinfo->typeptr == NULL)
    exp->graphinfo->typeptr = getbasictype(ifbwild);
  if (exp->graphinfo->typeptr->stsort != iftunion) {
    errorrec = newerrorptr(typenutc);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
    exp->graphinfo = newinfoptr();
    string10(&exp->graphinfo->litvalue, "error     ");
    exp->graphinfo->typeptr = errorstruct();
  }  /* if */
  else {
    typeptr = exp->graphinfo->typeptr->UU.stbasetype;
    while (typeptr != NULL) {
      tclist = newtcstacklist();
      tclist->typeptr = typeptr;
      if (semtclist->UU.tcstacklist == NULL)
	semtclist->UU.tcstacklist = tclist;
      else
	tclistend->next = tclist;
      tclistend = tclist;
      typeptr = typeptr->UU.U2.stnext;
    }  /*while*/
  }  /*else*/
  tagnode = buildcompound_(tagcase);
  toinfo = newinfoptr();
  toinfo->node_ = tagnode;
  toinfo->port_ = 1;
  addedge(exp->graphinfo, toinfo);
  newsemantic(&subnum, tgsubgraphnum);
  subnum->UU.subgraphnum = 0;
  newsemantic(&semsymptr, tgsymptr);
  if (semnamelist->UU.namelist != NULL) {
    symptr = getsymtabentry(semnamelist->UU.namelist->name, tvariable);
    symptr->graphinfo->port_ = 1;   /*comes into all SGs on port 1*/
    setdefloc(symptr, semnamelist->UU.namelist->linenum, -1);
    addtotable(symptr, currentlevel);
    newsemantic(&semsymptr, tgsymptr);
    semsymptr->UU.symptr = symptr;
  }
  pushsemantic(semsymptr);
  pushsemantic(semtclist);
  pushsemantic(subnum);
  freesemantic(&expl);
}  /* ProcessTagHead */


Static Void createtagsubgraph()
{
  /*action 44 CreateTagSubgraph*/
  /*stack        pops - TGSubGrahpNum
                 pushes - TGSubGraphNum*/
  /*description
          Build a tagcase subgraph. Search the symbol table for values which
          have the old SG as their graphinfo node or oldfrom node.  Change
          these to be the new SG node.  Increment the subgraph number.
          Push the subgraph number.*/
  semanticrec *subgrn;
  node *newn0, *oldn0;
  int hashindex;
  symtblbucket *chain;

  if (semtrace)
    printf(" In CreateTagSubgraph\n");
  subgrn = popsemantic();
  subgrn->UU.subgraphnum++;
  oldn0 = currentlevel->currentsubn0;
  newn0 = buildsubgraph(currentlevel->UU.U4.tagcasenode);
  if (subgrn->UU.subgraphnum > 1) {
    hashindex = 0;
    while (hashindex <= maxhashtable) {
      chain = currentlevel->hashtable[hashindex];
      while (chain != NULL) {
	if (oldn0 == chain->graphinfo->node_)
	  chain->graphinfo->node_ = newn0;
	if (chain->identtag == tvariable) {
	  if (oldn0 == chain->UU.U4.oldfrom->node_)
	    chain->UU.U4.oldfrom->node_ = newn0;
	}
	chain = chain->nextbucket;
      }
      hashindex++;
    }  /*while*/
  }  /*then*/
  pushsemantic(subgrn);
}  /* CreateTagSubgraph */


Static tcstacklistrec *findtag(name, struct_)
stryng name;
tcstacklistrec *struct_;
{
  boolean found;
  stryng name2;

  if (struct_ == NULL) {
    return struct_;
  }  /*then*/
  stringlowercase(&name);
  found = false;
  while (!found) {
    name2 = struct_->typeptr->stliteral;
    stringlowercase(&name2);
    if (equalstrings(&name, &name2))
      found = true;
    else
      struct_ = struct_->next;
    if (struct_ == NULL)
      found = true;
  }  /* while */
  return struct_;
}  /* FindTag */


Static Void checktagnames(line, col)
int line, col;
{
  /*action 45 CheckTagNames*/
  /*stack        pops - TGNameList
                        TGSubGraphNum
                        TGTCStackList
                        TGsymptr
                 pushes - TGSubGraphNum
                         TGTCStackList
                         TGsymptr */
  /*description
         Check that each name on the first name list is a tag of the
         union on the explist, is of the same type, and has not already
         been listed in this tagcase.  Mark each name with the subgraph
         number.  If the second namelist is not nil then add it to the
         symbol table with the same type as the names. Push the second
         name list, the union expression, and the subgraph number.*/
  semanticrec *namel, *subgrn, *semtclist, *tsymptr;
  stentry *ttype;
  namelistrec *names;
  symtblbucket *sym;
  errorrecord *errorrec;
  tcstacklistrec *tclist;

  if (semtrace)
    printf(" In CheckTagNames\n");
  namel = popsemantic();
  subgrn = popsemantic();
  semtclist = popsemantic();
  tsymptr = popsemantic();
  names = namel->UU.namelist;
  tclist = semtclist->UU.tcstacklist;
  ttype = NULL;
  while (names != NULL) {
    tclist = findtag(names->name, tclist);
    if (tclist == NULL) {
      errorrec = newerrorptr(nameundeftc);
      errorrec->UU.errorstr = names->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      ttype = getbasictype(ifbwild);   /*Set to wild type for undef name */
      semerror(errorrec);
    }  /* if */
    else if (tclist->sgnum != 0) {
      errorrec = newerrorptr(ddtname);
      errorrec->UU.errorstr = names->name;
      errorrec->linenumber = line;
      errorrec->column = col;
      semerror(errorrec);
    } else {
      if (ttype == NULL)
	ttype = tclist->typeptr;
      if (ttype->UU.U2.stelemtype != tclist->typeptr->UU.U2.stelemtype) {
	errorrec = newerrorptr(typesnsametc);
	errorrec->linenumber = line;
	errorrec->column = col;
	semerror(errorrec);
      }  /* if */
      else
	tclist->sgnum = subgrn->UU.subgraphnum;
    }
    names = names->next;
  }  /* while */
  if (tsymptr->UU.symptr != NULL) {
    sym = tsymptr->UU.symptr;
    sym->typeptr = ttype->UU.U2.stelemtype;
    sym->graphinfo->typeptr = ttype->UU.U2.stelemtype;
    sym->graphinfo->node_ = currentlevel->currentsubn0;
  }  /* if */
  pushsemantic(tsymptr);
  pushsemantic(semtclist);
  pushsemantic(subgrn);

  /* else if */
  /* else */
}  /* CheckTagNames */


Static Void assocexprs()
{
  /*action 46 AssocExprs*/
  /*stack        pops - nothing
                pushes - Nothing*/
  /*description
         Closes off a tagcase subgraph associating the output
         expressions with the tagcase node.*/
  if (semtrace)
    printf(" In AssocExprs\n");
  endiftagsubgraph(false);
}  /* AssocExprs */


Static Void endtagcase()
{
  /*action 47 EndTagcase*/
  /*stack        pops - TGSubGraphNum
                        TGTCStackList
                        TGSymPtr
                pushes - Nothing*/
  /*description
         Create the association list for the union in the explist.
         Make sure every tag was mentioned in the tagcase.*/
  /* semanticrec *subgrn, *semtclist, *tsymptr; */
  semanticrec *semtclist;
  tcstacklistrec *struct_;
  node *tnode;
  errorrecord *errorrec;

  if (semtrace)
    printf(" In EndTagcase\n");
  /* subgrn = */ (Void)popsemantic();
  semtclist = popsemantic();
  /* tsymptr = */ (Void)popsemantic();
  struct_ = semtclist->UU.tcstacklist;
  tnode = currentlevel->UU.U4.tagcasenode;
  while (struct_ != NULL) {
    if (struct_->sgnum == 0) {
      errorrec = newerrorptr(notalltc);
      errorrec->UU.errorstr = struct_->typeptr->stliteral;
      errorrec->linenumber = linenumber;
      semerror(errorrec);
    }  /* if */
    else
      addtoassolist(tnode, struct_->sgnum - 1);
    struct_ = struct_->next;
  }  /* while */
  endcompound();
}  /* EndTagcase */


Static Void otherwisetag(line, col)
int line, col;
{
  /*action 48 OtherwiseTag*/
  /*stack        pops - TGSubGraphNum
                        TGTCStackList
                 pushes - TGSubGraphNum
                          TGTCStackList */
  /*description
         Mark all unmarked tags with current subgraph number. If none
         are found then declare an error. Push explist, subgraph number*/
  semanticrec *subgrn, *semtclist;
  tcstacklistrec *struct_;
  int subnum;
  boolean found;
  errorrecord *errorrec;

  if (semtrace)
    printf(" In OtherwiseTag\n");
  subgrn = popsemantic();
  semtclist = popsemantic();
  struct_ = semtclist->UU.tcstacklist;
  subnum = subgrn->UU.subgraphnum;
  found = false;
  while (struct_ != NULL) {
    if (struct_->sgnum == 0) {
      found = true;
      struct_->sgnum = subnum;
    }  /* if */
    struct_ = struct_->next;
  }
  if (!found) {
    errorrec = newerrorptr(othernone);
    errorrec->linenumber = line;
    errorrec->column = col;
    semerror(errorrec);
  }  /* if */
  pushsemantic(semtclist);
  pushsemantic(subgrn);
}  /* OtherwiseTag */


Static Char *typesemanticdata_NAMES[] = {
  "tgnamelist", "tgtypelist", "tgttptr", "tgsymptr", "tgexplist",
  "tgpreclevel", "tgsymlist", "tgnodeptr", "tgsubgraphnum", "tgoldflag",
  "tgreduceflag", "tgrednodetype", "tgtestflag", "tgtcstacklist"
} ;


Static Char *opertype_NAMES[] = {
  "noop", "orop", "andop", "mult", "divd", "plus", "minus", "concat", "lt",
  "le", "gt", "ge", "equal", "notequal", "uplus", "uminus", "unot"
} ;


Static Char *reduceflag_NAMES[] = {
  "rfnone", "rfsum", "rfproduct", "rfgreatest", "rfleast", "rfcatenate"
} ;


Local Void setstep()
{
  printf("Please enter step count: ");
  scanf("%d%*[^\n]", &stepcount);
  getchar();
  currentstep = stepcount;
}

Local Void showbrkpts()
{
  int i, j;

  printf("The Breakpoints are:\n");
  j = 0;
  for (i = 1; i <= maxsemanticnumber; i++) {   /****************/
    if (P_inset(i, breakpointset)) {
      printf("%5d", i);
      j += 5;
      if (j > 75) {
	putchar('\n');
	j = 0;
      }  /* if */
    }  /* if */
  }
  if (j != 0)
    putchar('\n');
}  /* ShowBrkPts */

Local Void removebrkpts()
{
  unchar rembrkpt;   /**************/
  int TEMP;
  /* long SET[maxsemanticnumber / 32 + 2]; */

  showbrkpts();
  printf("Enter the breakpoints to be removed:\n");
  while (!P_eoln(stdin)) {
    scanf("%d", &TEMP);
    rembrkpt = TEMP;
    P_remset(breakpointset, rembrkpt);
  }  /* while */
  scanf("%*[^\n]");
  getchar();
  showbrkpts();
}  /* RemoveBrkPts */

Local Void addbrkpts()
{
  unchar newbrkpt;   /************/
  int TEMP;
  /* long SET[maxsemanticnumber / 32 + 2]; */

  showbrkpts();
  printf("Enter the breakpoints to be added:\n");
  while (!P_eoln(stdin)) {
    scanf("%d", &TEMP);
    newbrkpt = TEMP;
    P_addset(breakpointset, newbrkpt);
  }  /* while */
  scanf("%*[^\n]");
  getchar();
  showbrkpts();
}  /* AddBrkPts */

Local Void printnames(namelist)
namelistrec *namelist;
{
  if (namelist == NULL)
    printf("namelist is nil\n");
  else {
    while (namelist != NULL) {
      writestring(stdout, &namelist->name);
      printf(" , ");
      namelist = namelist->next;
    }  /* while */
  }
  putchar('\n');
}  /* PrintNames */

Local Void printtypes_(typelist)
typelistrec *typelist;
{
  if (typelist == NULL) {
    printf("typelist is nil\n");
    return;
  }
  while (typelist != NULL) {
    if (typelist->typeptr != NULL) {
      outtype(stdout, typelist->typeptr, false);
      putchar('\n');
      typelist = typelist->next;
    } else
      printf(" nil type ptr \n");
  }  /* while */
}  /* PrintTypes */

Local Void printgraphinfo(info)
inforecord *info;
{
  if (info->node_ != NULL)
    printf(" ^.NDId  %5d ", info->node_->ndid);
  else
    printf("node is nil ");
  printf("Port %5d ", info->port_);
  if (info->typeptr != NULL)
    outtype(stdout, info->typeptr, true);
  else
    printf("type ptr is nil ");
  printf("   ");
  writestring(stdout, &info->name);
  putchar(' ');
  if (info->onlist != NULL)
    printf("Onlist is not nil ");
  else
    printf("OnList is nil ");
  printf("LitValue ");
  writestring(stdout, &info->litvalue);
  putchar('\n');
}  /* PrintGraphInfo */

Local Void printsymlist(symlist)
symlistnode *symlist;
{
  while (symlist != NULL) {
    printbucket(symlist->sym);
    putchar('\n');
    symlist = symlist->next;
  }  /* while */
}  /* PrintSymList */

Local Void printexplist(explist)
explistnode *explist;
{
  while (explist != NULL) {
    printgraphinfo(explist->graphinfo);
    putchar('\n');
    explist = explist->next;
  }  /* while */
}  /* PrintExpList */

Local Void printtclist(tclist)
tcstacklistrec *tclist;
{
  while (tclist != NULL) {
    writestring(stdout, &tclist->tagname);
    printf("  SGNum = %3d  ", tclist->sgnum);
    outtype(stdout, tclist->typeptr, true);
    putchar('\n');
    tclist = tclist->next;
  }
}

Local Void printstack()
{
  int i;
  semanticrec *semstack;

  printf("Please enter the number of stack items to be printed:\n");
  scanf("%d%*[^\n]", &i);
  getchar();
  semstack = topsemstk;
  while (semstack != NULL && i != 0) {
    printf("** Object is of type : %s\n",
	   typesemanticdata_NAMES[(long)semstack->tag]);
    switch (semstack->tag) {

    case tgnamelist:
      printnames(semstack->UU.namelist);
      break;

    case tgtypelist:
      printtypes_(semstack->UU.typelist);
      break;

    case tgttptr:
      if (semstack->UU.ttptr != NULL) {
	outtype(stdout, semstack->UU.ttptr, false);
	putchar('\n');
      } else
	printf("semstack^.ttptr is nil\n");
      break;

    case tgsymptr:
      if (semstack->UU.symptr != NULL) {
	printf("%s  ", identtype_NAMES[(long)semstack->UU.symptr->identtag]);
	writestring(stdout, &semstack->UU.symptr->name);
	putchar('\n');
      } else
	printf("symptr is nil\n");
      break;

    case tgexplist:
      if (semstack->UU.explist == NULL)
	printf("semstack^.explist is nil\n");
      else
	printexplist(semstack->UU.explist);
      break;

    case tgsymlist:
      if (semstack->UU.symlist == NULL)
	printf("semstack^.SymList is nil\n");
      else
	printsymlist(semstack->UU.symlist);
      break;

    case tgpreclevel:
      printf("prec level %12d", semstack->UU.U5.preclevel);
      printf("operation = %s\n",
	     opertype_NAMES[(long)semstack->UU.U5.operation]);
      break;

    case tgnodeptr:
      if (semstack->UU.nodeptr == NULL)
	printf(" semstack^.nodeptr is nil\n");
      else {
	printf("Node id is %12d", semstack->UU.nodeptr->ndid);
	printf("Node code is %12d\n", semstack->UU.nodeptr->ndcode);
      }
      break;

    case tgsubgraphnum:
      printf(" Num of subgraphs = %4d\n", semstack->UU.subgraphnum);
      break;

    case tgoldflag:
      printf("Old Flag = %s\n", semstack->UU.oldflag ? " TRUE" : "FALSE");
      break;

    case tgreduceflag:
      printf("Reduce Flag = %s\n",
	     reduceflag_NAMES[(long)semstack->UU.redflag]);
      break;

    case tgrednodetype:
      printf("Reduce Type = %12d\n", semstack->UU.rednodetype);
      break;

    case tgtcstacklist:
      if (semstack->UU.tcstacklist == NULL)
	printf("TC List is nil\n");
      else
	printtclist(semstack->UU.tcstacklist);
      break;
    }/*  case */
    semstack = semstack->next;
    i--;
  }  /* while */
  if (i != 0)
    printf("Not that many items on stack\n");
}  /* PrintStack */


Static boolean debugger(action, token)
int action, token;
{
  boolean Result;
  int i;
  boolean run;
  Char ch;
  treestackobj *temp;
  /* node *node_; */

  if (token != -1) {
    printf("action number is : %12d", action);
    switch (action) {

    case lbinitsem:
      printf(" initsem\n");
      break;

    case lbstartfunction:
      printf(" startfunction(inputtoken)\n");
      break;

    case lbaddtonamelist:
      printf(" addtonamelist(inputtoken)\n");
      break;

    case lbstartnamelist:
      printf(" startnamelist\n");
      break;

    case lbstarttypelist:
      printf(" starttypelist\n");
      break;

    case lbaddtotypelist:
      printf(" addtotypelist\n");
      break;

    case lbendglobalheader:
      printf(" EndGlobalHeader\n");
      break;

    case lbendforwardglobheader:
      printf(" EndForwardGlobHeader\n");
      break;

    case lbsmashintotable:
      printf(" SmashIntoTable\n");
      break;

    case lbpushbasic:
      printf(" PushBasic(token)\n");
      break;

    case lbpushtypename:
      printf(" PushTypeName(inputtoken)\n");
      break;

    case lbpusharray:
      printf(" PushArray\n");
      break;

    case lbpushdefarray:
      printf(" PushDefArray\n");
      break;

    case lbpushstream:
      printf(" PushStream\n");
      break;

    case lbpushdefstream:
      printf(" PushDefStream\n");
      break;

    case lbpushrecord:
      printf(" PushRecord\n");
      break;

    case lbpushdefrecord:
      printf(" PushDefRecord\n");
      break;

    case lbpushdefunion:
      printf(" PushUnion\n");
      break;

    case lbpushunion:
      printf(" PushDefUnion\n");
      break;

    case lbpushnulltype:
      printf(" PushNullType\n");
      break;

    case lbfinishtypelinks:
      printf(" FinishTypeLinks\n");
      break;

    case lbchecktypename:
      printf(" CheckTypeName(inputtoken)\n");
      break;

    case lblinkbase:
      printf(" LinkBase\n");
      break;

    case lbbuildfield:
      printf(" BuildField\n");
      break;

    case lbbuilddeffield:
      printf(" BuildDefField\n");
      break;

    case lbbuildtag:
      printf(" BuildTag\n");
      break;

    case lbbuilddeftag:
      printf(" BuildDefTag\n");
      break;

    case lbenddefinelist:
      printf(" EndDefineList\n");
      break;

    case lbaddtodefinelist:
      printf(" AddToDefineList(inputtoken)\n");
      break;

    case lbstartforwardfunct:
      printf(" StartForwardFunct\n");
      break;

    case lbstartglobalfunct:
      printf(" StartGlobalFunct\n");
      break;

    case lbprocessparam:
      printf(" ProcessParam\n");
      break;

    case lbendfunctionheader:
      printf(" EndFunctionHeader\n");
      break;

    case lbendfunctionlist:
      printf(" EndFunctionList\n");
      break;

    case lbendfunction:
      printf(" EndFunction\n");
      break;

    case lbstartsymlist:
      printf(" StartSymList\n");
      break;

    case lbcheckatypes:
      printf(" CheckATypes\n");
      break;

    case lbcheckarity:
      printf(" CheckArity\n");
      break;

    case lbassoctypes:
      printf(" AssocTypes\n");
      break;

    case lbincreasegenerator:
      printf(" IncreaseGenerator\n");
      break;

    case lbcheckcross:
      printf(" CheckCross\n");
      break;

    case lbexplicitcross:
      printf(" ExplicitCross\n");
      break;

    case lbstartbody:
      printf(" StartBody\n");
      break;

    case lbendabody:
      printf(" EndABody\n");
      break;

    case lbendbbody:
      printf(" EndBBody\n");
      break;

    case lbmakeforall:
      printf(" MakeForall\n");
      break;

    case lbfixforfunct:
      printf(" FixForFunct\n");
      break;

    case lbfixfortag:
      printf(" FixForTag\n");
      break;

    case lbprocesstaghead:
      printf(" ProcessTagHead\n");
      break;

    case lbchecktagnames:
      printf(" CheckTagNames\n");
      break;

    case lbassocexprs:
      printf(" AssocExprs\n");
      break;

    case lbendtagcase:
      printf(" EndTagcase\n");
      break;

    case lbotherwisetag:
      printf(" OtherwiseTag\n");
      break;

    case lbcreatetagsubgraph:
      printf(" CreateTagSubgraph\n");
      break;

    case lbstartforinit:
      printf(" StartForinit\n");
      break;

    case lbendinitpart:
      printf(" EndInitPart\n");
      break;

    case lbpushwhile:
      printf(" PushWhile\n");
      break;

    case lbpushuntil:
      printf(" PushUntil\n");
      break;

    case lbstartatest:
      printf(" StartATest\n");
      break;

    case lbendtest:
      printf(" EndTest\n");
      break;

    case lbloopbassoclist:
      printf(" LoopBAssocList\n");
      break;

    case lbloopaassoclist:
      printf(" LoopBAssocList\n");
      break;

    case lbpushold:
      printf(" PushOld\n");
      break;

    case lbpushnoold:
      printf(" PushNoOld\n");
      break;

    case lbdoredright:
      printf(" DoRedRight\n");
      break;

    case lbdoredleft:
      printf(" DoRedLeft\n");
      break;

    case lbdoredtree:
      printf(" DoRedTree\n");
      break;

    case lbdoreduce:
      printf(" DoReduce\n");
      break;

    case lbdosum:
      printf(" DoSum\n");
      break;

    case lbdoproduct:
      printf(" DoProduct\n");
      break;

    case lbdogreatest:
      printf(" DoGreatest\n");
      break;

    case lbdoleast:
      printf(" DoLeast\n");
      break;

    case lbdocatenate:
      printf(" DoCatenate\n");
      break;

    case lbnoredop:
      printf(" NoRedOp\n");
      break;

    case lbdounlessexp:
      printf(" DoUnlessExp\n");
      break;

    case lbdowhenexp:
      printf(" DoWhenExp\n");
      break;

    case lbnomaskingexp:
      printf(" NoMaskingExp\n");
      break;

    case lbdovalueof:
      printf(" DoValueOf\n");
      break;

    case lbdoarrayof:
      printf(" DoArrayOf\n");
      break;

    case lbdostreamof:
      printf(" DoStreamOf\n");
      break;

    case lbstartreturns:
      printf(" StartReturns\n");
      break;

    case lbstartainitbody:
      printf(" StartAInitBody\n");
      break;

    case lbendbody:
      printf(" EndBody\n");
      break;

    case lbendforinit:
      printf(" EndForinit\n");
      break;

    case lbstartif:
      printf(" StartIf\n");
      break;

    case lbstartifsubgraph:
      printf(" StartIfSubgraph\n");
      break;

    case lbendifsubgraph:
      printf(" EndIfSubgraph\n");
      break;

    case lbendpredsubgraph:
      printf(" EndPredSubGraph\n");
      break;

    case lbendif:
      printf(" EndIf\n");
      break;

    case lbbeforeexp:
      printf(" BeforeExp\n");
      break;

    case lbincboostfac:
      printf(" IncBoostFac\n");
      break;

    case lbdecboostfac:
      printf(" DecBoostFac\n");
      break;

    case lbreduceexp:
      printf(" ReduceExp\n");
      break;

    case lbpoppreclevel:
      printf(" PopPrecLevel\n");
      break;

    case lbafterexp:
      printf(" AfterExp\n");
      break;

    case lbdofieldlist:
      printf(" DoFieldList\n");
      break;

    case lbpushnilttptr:
      printf(" PushNILTTPtr\n");
      break;

    case lbstartrbuild:
      printf(" StartRBuild\n");
      break;

    case lbbuildfieldlist:
      printf(" BuildFieldList\n");
      break;

    case lbendrbuild:
      printf(" EndRBuild\n");
      break;

    case lbstartabuild:
      printf(" StartABuild\n");
      break;

    case lbstartsbuild:
      printf(" StartSBuild\n");
      break;

    case lbdolowerbound:
      printf(" DoLowerBound\n");
      break;

    case lbendabuild:
      printf(" EndABuild\n");
      break;

    case lbendsbuild:
      printf(" EndSBuild\n");
      break;

    case lbpushniltag:
      printf(" PushNILTag\n");
      break;

    case lbendubuild:
      printf(" EndUBuild\n");
      break;

    case lbpushemptyexplist:
      printf(" PushEmptyExpList\n");
      break;

    case lbdofunctcall:
      printf(" DoFunctCall\n");
      break;

    case lbdoarrayindex:
      printf(" DoArrayIndex\n");
      break;

    case lbdoarrayreplace:
      printf(" DoArrayReplace\n");
      break;

    case lbbuildreplace:
      printf(" BuildReplace\n");
      break;

    case lbdobinaryop:
      printf(" DoBinaryOp(token)\n");
      break;

    case lbpushnilcons:
      printf(" PushNilCons\n");
      break;

    case lbpushtruecons:
      printf(" PushTrueCons\n");
      break;

    case lbpushfalsecons:
      printf(" PushFalseCons\n");
      break;

    case lbpushintcons:
      printf(" PushIntCons\n");
      break;

    case lbpushrealcons:
      printf(" PushRealCons\n");
      break;

    case lbpushcharcons:
      printf(" PushCharCons\n");
      break;

    case lbpushstringcons:
      printf(" PushStringCons\n");
      break;

    case lbpusherrorcons:
      printf(" PushErrorCons\n");
      break;

    case lbfindandpushname:
      printf(" FindAndPushName\n");
      break;

    case lbbuildchar:
      printf(" BuildChar\n");
      break;

    case lbbuilddouble:
      printf(" BuildDouble\n");
      break;

    case lbbuildint:
      printf(" BuildInt\n");
      break;

    case lbbuildreal:
      printf(" BuildReal\n");
      break;

    case lbcreateisunion:
      printf(" CreateIsUnion\n");
      break;

    case lbcheckold:
      printf(" CheckOld\n");
      break;

    case lbpushuplus:
      printf(" PushUPlus\n");
      break;

    case lbpushuminus:
      printf(" PushUMinus\n");
      break;

    case lbpushunot:
      printf(" PushUNot\n");
      break;

    case lbcreateiserror:
      printf(" CreateIsError\n");
      break;

    case lbpopsymlist:
      printf(" PopSymList\n");
      break;

    case lbopenllevel:
      printf(" OpenLLevel\n");
      break;

    case lbendlevel:
      printf(" EndLevel\n");
      break;

    case lbendforall:
      printf(" EndForAll\n");
      break;

    case lbendprogram:
      printf(" EndProgram\n");
      break;

    case lbpushdoubcons:
      printf(" PushDoubCons\n");
      break;

    case lbstartbtest:
      printf(" StartBTest\n");
      break;

    case lbstartbinitbody:
      printf(" StartBInitBody\n");
      break;

    case lbaftersimpexp:
      printf(" AfterSimpExp\n");
      break;

    case lbendtypedefs:
      printf(" EndTypeDefs\n");
      break;

    case lbstarttypedefs:
      printf(" StarTypeDefs\n");
      break;

    case lbpreparefordot:
      printf(" PrepareForDot\n");
      break;
    }/* case */
    printf("Token is: %6d  ", token);
    writestring(stdout, &inputtoken);
    putchar('\n');
  }  /* if */
  else {
    currentstep = 0;
    stepcount = 0;
    P_expset(breakpointset, 0L);
  }
  run = false;
  Result = true;
  while (!run) {
    if (currentstep == 0)
      currentstep = stepcount;
    printf("  Please type: \n");
    printf(" 0 : to add break points.      1 : to remove break points.\n");
    printf(" 2 : turn on parse trace       3 : to set step count.\n");
    printf(" 4 : continue running          5 : symbol table, Current\n");
    printf(" 6 : symbol table,Root.        7 : semantic stack.\n");
    printf(" 8 : turn sem trace on.        9 : turn trace off.\n");
    printf("10 : enter graphwalker        11 : try to run dump.\n");
    printf("12 : Quit.\n");
    scanf("%d%*[^\n]", &i);
    getchar();
    switch (i) {

    case 0:
      addbrkpts();
      break;

    case 1:
      removebrkpts();
      break;

    case 2:
      debug = true;
      cortrace = true;
      break;

    case 3:
      setstep();
      break;

    case 4:
      run = true;
      break;

    case 5:
      if (currentlevel != NULL)
	crossref(stdout, currentlevel);
      else
	printf("Symbol table not initialized yet\n");
      ch = getchar();
      if (ch == '\n')
	ch = ' ';
      break;

    case 6:
      if (currentlevel != NULL) {
	temp = currentlevel;
	while (temp->lleveltag != root)
	  temp = temp->lastlevel;
	crossref(stdout, temp);
      }  /* if */
      else
	printf("Symbol table not initialized yet\n");
      ch = getchar();
      if (ch == '\n')
	ch = ' ';
      break;

    case 7:
      printstack();
      ch = getchar();
      if (ch == '\n')
	ch = ' ';
      break;

    case 8:
      semtrace = true;
      break;

    case 9:
      semtrace = false;
      cortrace = false;
      debug = false;
      break;

    case 10:
      /* node_ = */ (Void)graphwalk(module);
      break;

    case 11:
      dumpmodule();
      break;

    case 12:
      run = true;
      Result = false;
      break;
    }/* case */
  }  /* while */
  return Result;
}  /* Debugger */


Static boolean announceaction(action, token, line, col, inputtoken)
int action, token, line, col;
stryng inputtoken;
{
  /*--------------------------------------------------------------------
    call the semantic routine
    put a case action of
             ...
    here to call your personal routines.
   --------------------------------------------------------------------*/
  boolean Result;

  Result = true;
  currentstep--;
  if (P_inset(action, breakpointset) || currentstep == 0)
    Result = debugger(action, token);
  switch (action) {

  case lbinitsem:
    initsem();
    break;

  case lbstartfunction:
    startfunction(inputtoken, action, line, col);
    break;

  case lbstartglobalfunct:
    startfunction(inputtoken, action, line, col);
    break;

  case lbaddtonamelist:
    addtonamelist(inputtoken, line, col);
    break;

  case lbstartnamelist:
    startnamelist();
    break;

  case lbstarttypelist:
    starttypelist();
    break;

  case lbaddtotypelist:
    addtotypelist();
    break;

  case lbendforwardglobheader:
    endforwardglobheader();
    break;

  case lbsmashintotable:
    smashintotable();
    break;

  case lbpushbasic:
    pushbasic(token);
    break;

  case lbpushtypename:
    pushtypename(inputtoken, line, col);
    break;

  case lbpusharray:
    pusharray(noaddtottable);
    break;

  case lbpushdefarray:
    pusharray(addtottable);
    break;

  case lbpushstream:
    pushstream(noaddtottable);
    break;

  case lbpushdefstream:
    pushstream(addtottable);
    break;

  case lbpushrecord:
    pushrecord(noaddtottable);
    break;

  case lbpushdefrecord:
    pushrecord(addtottable);
    break;

  case lbpushunion:
    pushunion(noaddtottable);
    break;

  case lbpushdefunion:
    pushunion(addtottable);
    break;

  case lbpushnulltype:
    pushnulltype();
    break;

  case lbfinishtypelinks:
    finishtypelinks();
    break;

  case lbchecktypename:
    checktypename(inputtoken, line, col);
    break;

  case lblinkbase:
    linkbase();
    break;

  case lbbuildfield:
    buildfield(noaddtottable);
    break;

  case lbbuilddeffield:
    buildfield(addtottable);
    break;

  case lbbuildtag:
    buildtag(noaddtottable);
    break;

  case lbbuilddeftag:
    buildtag(addtottable);
    break;

  case lbenddefinelist:
    enddefinelist();
    break;

  case lbaddtodefinelist:
    addtodefinelist(inputtoken);
    break;

  case lbstartforwardfunct:
    startforwardfunct(inputtoken, line, col);
    break;

  case lbprocessparam:
    processparam();
    break;

  case lbendfunctionheader:
    endfunctionheader();
    break;

  case lbendfunctionlist:
    endfunctionlist();
    break;

  case lbendfunction:
    endfunction();
    break;

  case lbstartsymlist:
    startsymlist();
    break;

  case lbcheckatypes:
    checkatypes(line, col);
    break;

  case lbcheckarity:
    checkarity();
    break;

  case lbassoctypes:
    assoctypes();
    break;

  case lbincreasegenerator:
    increasegenerator(line, col);
    break;

  case lbcheckcross:
    checkcross();
    break;

  case lbexplicitcross:
    explicitcross(line, col);
    break;

  case lbstartbody:
    startbody();
    break;

  case lbendabody:
    endabody(line, col);
    break;

  case lbendbbody:
    endbbody(line, col);
    break;

  case lbmakeforall:
    makeforall(line, col);
    break;

  case lbfixforfunct:
    fixforfunct();
    break;

  case lbfixfortag:
    fixfortag(line, col);
    break;

  case lbprocesstaghead:
    processtaghead(line, col);
    break;

  case lbchecktagnames:
    checktagnames(line, col);
    break;

  case lbassocexprs:
    assocexprs();
    break;

  case lbendtagcase:
    endtagcase();
    break;

  case lbotherwisetag:
    otherwisetag(line, col);
    break;

  case lbcreatetagsubgraph:
    createtagsubgraph();
    break;

  case lbstartforinit:
    startforinit();
    break;

  case lbendinitpart:
    endinitpart();
    break;

  case lbpushwhile:
    pushwhile();
    break;

  case lbpushuntil:
    pushuntil();
    break;

  case lbstartatest:
    startatest();
    break;

  case lbendtest:
    endtest();
    break;

  case lbloopbassoclist:
    loopbassoclist();
    break;

  case lbloopaassoclist:
    loopaassoclist();
    break;

  case lbpushold:
    pushold();
    break;

  case lbpushnoold:
    pushnoold();
    break;

  case lbdoredright:
    doreduce(action);
    break;

  case lbdoredleft:
    doreduce(action);
    break;

  case lbdoredtree:
    doreduce(action);
    break;

  case lbdoreduce:
    doreduce(action);
    break;

  case lbdosum:
    dosum();
    break;

  case lbdoproduct:
    doproduct();
    break;

  case lbdogreatest:
    dogreatest();
    break;

  case lbdoleast:
    doleast();
    break;

  case lbdocatenate:
    docatenate();
    break;

  case lbnoredop:
    noredop();
    break;

  case lbdounlessexp:
    dounlessexp();
    break;

  case lbdowhenexp:
    dowhenexp();
    break;

  case lbnomaskingexp:
    nomaskingexp();
    break;

  case lbdovalueof:
    dovalueof();
    break;

  case lbdoarrayof:
    doarrayof(action);
    break;

  case lbdostreamof:
    doarrayof(action);
    break;

  case lbstartreturns:
    startreturns();
    break;

  case lbstartainitbody:
    startainitbody();
    break;

  case lbendbody:
    endbody();
    break;

  case lbendforinit:
    endforinit();
    break;

  case lbstartif:
    startif(ifcomp);
    break;

  case lbstartnestedif:
    startif(nestedifcomp);
    break;

  case lbstartifsubgraph:
    startifsubgraph();
    break;

  case lbendifsubgraph:
    endifsubgraph();
    break;

  case lbendpredsubgraph:   /*EndPredSubGraph*/
    printf("INTERNAL ERROR\n");
    break;

  case lbendif:
    endif();
    break;

  case lbbeforeexp:
    beforeexp();
    break;

  case lbincboostfac:
    incboostfac();
    break;

  case lbdecboostfac:
    decboostfac();
    break;

  case lbreduceexp:
    reduceexp();
    break;

  case lbpoppreclevel:
    poppreclevel();
    break;

  case lbafterexp:
    afterexp();
    break;

  case lbdofieldlist:
    dofieldlist(line, col);
    break;

  case lbpushnilttptr:
    pushnilttptr();
    break;

  case lbstartrbuild:
    pushrecord(noaddtottable);
    startrbuild();
    break;

  case lbbuildfieldlist:
    buildfieldlist(line, col);
    break;

  case lbendrbuild:
    endrbuild(line, col);
    break;

  case lbstartabuild:
    startabuild();
    break;

  case lbstartsbuild:
    pushstream(noaddtottable);
    startsbuild();
    break;

  case lbdolowerbound:
    dolowerbound();
    break;

  case lbendabuild:
    endabuild();
    break;

  case lbendsbuild:
    endsbuild(line, col);
    break;

  case lbpushniltag:
    pushniltag();
    break;

  case lbendubuild:
    endubuild(line, col);
    break;

  case lbpushemptyexplist:
    pushemptyexplist();
    break;

  case lbdofunctcall:
    dofunctcall();
    break;

  case lbdoarrayindex:
    doarrayindex();
    break;

  case lbdoarrayreplace:
    doarrayreplace();
    break;

  case lbbuildreplace:
    buildreplace(line, col);
    break;

  case lbdobinaryop:
    dobinaryop(token, line, col);
    break;

  case lbpushnilcons:
    pushnilcons();
    break;

  case lbpushtruecons:
    pushtruecons();
    break;

  case lbpushfalsecons:
    pushfalsecons();
    break;

  case lbpushintcons:
    pushintcons();
    break;

  case lbpushrealcons:
    pushrealcons();
    break;

  case lbpushcharcons:
    pushcharcons();
    break;

  case lbpushstringcons:
    pushstringcons();
    break;

  case lbpusherrorcons:
    pusherrorcons();
    break;

  case lbfindandpushname:
    findandpushname();
    break;

  case lbbuildchar:
    buildchar();
    break;

  case lbbuilddouble:
    builddouble();
    break;

  case lbbuildint:
    buildint();
    break;

  case lbbuildreal:
    buildreal();
    break;

  case lbcreateisunion:
    createisunion(line, col);
    break;

  case lbcheckold:
    checkold(inputtoken, line, col);
    break;

  case lbpushuplus:
    pushuplus(line, col);
    break;

  case lbpushuminus:
    pushuminus(line, col);
    break;

  case lbpushunot:
    pushunot(line, col);
    break;

  case lbcreateiserror:
    createiserror(line, col);
    break;

  case lbpopsymlist:
    popsymlist();
    break;

  case lbopenllevel:
    createlevel(let);
    break;

  case lbendlevel:
    endlevel();
    break;

  case lbendforall:
    endforall();
    break;

  case lbendprogram:
    endprogram();
    break;

  case lbpushdoubcons:
    pushdoubcons();
    break;

  case lbstartbtest:
    startbtest();
    break;

  case lbstartbinitbody:
    startbinitbody();
    break;

  case lbaftersimpexp:
    aftersimpexp();
    break;

  case lbendtypedefs:
    endtypedefs();
    break;

  case lbstarttypedefs:
    starttypedefs();
    break;

  case lbpreparefordot:
    preparefordot();
    break;
  }/* case */
  return Result;
}  /* callsemantics*/
