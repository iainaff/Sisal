#include "world.h"

int ExpandedEqual	= TRUE;  /* Use expanded def'n */
int FixPortsToo		= FALSE; /* Don't fix up Loop ports */
int AllowVMarks		= FALSE; /* Don't bother with %mk=V input */
int StreamsOK		= FALSE; /* Don't allow stream types */
int InitialNodeLevel	= -1;	/* n->level not used */
int streams		= FALSE; /* No streams found yet */
int recursive		= FALSE; /* No %mk=B functions found yet */
int dbl			= FALSE; /* Don't convert double types */
int flt			= FALSE; /* Don't convert float  types */
int DMarkProblem	= FALSE; /* Don't mess with with %mk=d nodes */
int FullyOrdered	= FALSE; /* Do use Artif. Dep. Edges */
int echange		= 0;
int nchange		= 0;
int CheckForBadEdges    = FALSE; /* Turn edge checking on/off */

double mcosts[12]	= { 0.0,0.0,0.0,0.0,0.0,0.0, /* Don't care about */
			    0.0,0.0,0.0,0.0,0.0,0.0 }; /* costs yet */

void PlaceInEntryTable(c) char *c; {}
void PlaceInFortranTable(c) char *c; {}
void PlaceInCTable(c) char *c; {}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define DoAssignPragmas(x) \
{	\
  (x)->pmark   = pragmas.pmark;	\
  (x)->rmark1  = pragmas.rmark1;\
  (x)->omark1  = pragmas.omark1;\
  (x)->rmark2  = pragmas.rmark2;\
  (x)->omark2  = pragmas.omark2;\
  (x)->dmark   = pragmas.dmark;	\
  (x)->lmark   = pragmas.lmark;	\
  (x)->emark   = pragmas.emark;	\
  (x)->imark   = pragmas.imark;	\
  (x)->mark    = pragmas.mark;	\
  (x)->cmark   = pragmas.cmark;	\
  (x)->fmark   = pragmas.fmark;	\
  (x)->nmark   = pragmas.nmark;	\
  (x)->bmark   = pragmas.bmark;	\
  (x)->wmark   = pragmas.wmark;	\
  (x)->umark   = pragmas.umark;	\
  (x)->rstable = pragmas.rstable;\
  (x)->ostable = pragmas.ostable;\
  (x)->sr      = pragmas.sr;	\
  (x)->pm      = pragmas.pm;	\
  (x)->pl      = pragmas.pl;	\
  (x)->cm      = pragmas.cm;	\
  (x)->smark   = pragmas.smark;	\
  (x)->ccost   = pragmas.ccost;	\
  (x)->ID      = pragmas.ID;	\
  (x)->ThinCopy = pragmas.ThinCopy; \
\
  (x)->name    = pragmas.name;	\
  (x)->line    = pragmas.line;	\
  (x)->file  = ( pragmas.file )?(pragmas.file):sfile;		\
  (x)->funct = ( pragmas.funct )?(pragmas.funct):(		\
	          (cfunct == NULL)? "?" : cfunct->G_NAME);	\
}
                                                      
void TypeAssignPragmas(p)
     PINFO	p;
{
  DoAssignPragmas(p);
}
void EdgeAssignPragmas(p)
     PEDGE	p;
{
  DoAssignPragmas(p);
}
void NodeAssignPragmas(p)
     PNODE	p;
{
  DoAssignPragmas(p);
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define DoInitPragmas(x) \
{ \
  StandardPragmas(x);	\
  (x)->sr = -2;		\
  (x)->pm = -2;		\
  (x)->pl = -2;		\
  (x)->cm =  0;		\
}

void PragInitPragmas(p)
     PRAGS	*p;
{
  DoInitPragmas(p);
}
void NodeInitPragmas(p)
     PNODE	p;
{
  DoInitPragmas(p);
}
void TypeInitPragmas(p)
     PINFO	p;
{
  DoInitPragmas(p);
}
void EdgeInitPragmas(p)
     PEDGE	p;
{
  DoInitPragmas(p);
}

