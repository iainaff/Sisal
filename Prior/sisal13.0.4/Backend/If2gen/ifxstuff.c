#include "world.h"

int ExpandedEqual	= TRUE;  /* Use expanded def'n */
int FixPortsToo		= FALSE; /* Don't fix up Loop ports */
int AllowVMarks		= TRUE; /* Allow %mk=V input (means vector here) */
int StreamsOK		= FALSE; /* Don't allow stream types */
PNODE fhead		= NULL;	/* No modified function chain */
int InitialNodeLevel	= -1;	/* We don't use node level here */
int DMarkProblem	= TRUE; /* Problem with %mk=d nodes */
int FullyOrdered	= TRUE; /* Don't use Artif. Dep. Edges */
int streams		= FALSE; /* Has a stream been found? */
int sgnok		= TRUE;	/* Allow negative constants */
int			echange = 0;
int			nchange = 0;
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
  (x)->pmark = pragmas.pmark;	\
  (x)->rmark1 = pragmas.rmark1;	\
  (x)->omark1 = pragmas.omark1;	\
  (x)->rmark2 = pragmas.rmark2;	\
  (x)->omark2 = pragmas.omark2;	\
  (x)->dmark = pragmas.dmark;	\
  (x)->mark = pragmas.mark;	\
  (x)->lmark = pragmas.lmark;	\
  (x)->Pmark = pragmas.Pmark;	\
  (x)->Cmark = pragmas.Cmark;	\
  (x)->xmark = pragmas.xmark;	\
  (x)->emark = pragmas.emark;	\
  (x)->cmark = pragmas.cmark;	\
  (x)->nmark = pragmas.nmark;	\
  (x)->umark = pragmas.umark;	\
  (x)->bmark = pragmas.bmark;	\
  (x)->flp = pragmas.flp;	\
  (x)->wmark = pragmas.wmark;	\
  (x)->rstable = pragmas.rstable;	\
  (x)->ostable = pragmas.ostable;	\
  (x)->sr = pragmas.sr;	\
  (x)->pm = pragmas.pm;	\
  (x)->pl = pragmas.pl;	\
  (x)->cm = pragmas.cm;	\
  (x)->vmark = pragmas.vmark;	\
  (x)->fmark = pragmas.fmark;	\
  (x)->Fmark = pragmas.Fmark;	\
  (x)->smark = pragmas.smark;	\
  (x)->ccost = pragmas.ccost;	\
  (x)->MinSlice= pragmas.MinSlice; \
  (x)->ID      = pragmas.ID;	\
  (x)->ThinCopy= pragmas.ThinCopy; \
  (x)->Style   = pragmas.Style; \
  (x)->LoopSlice= pragmas.LoopSlice; \
\
  (x)->name  = pragmas.name;  	\
  (x)->line  = pragmas.line;  	\
  (x)->file  = ( pragmas.file )?(pragmas.file):sfile;	\
  (x)->funct = ( pragmas.funct )?(pragmas.funct):(	\
		     (cfunct == NULL)? "" : cfunct->G_NAME);	\
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
#define DoInitPragmas(x) StandardPragmas(x)

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
