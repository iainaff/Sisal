#include "world.h"

int FixPortsToo		= TRUE;	/* Fix up Loop ports */
int AllowVMarks		= TRUE;	/* Allow %mk=V input */
int StreamsOK		= TRUE;	/* Allow stream types */
int InitialNodeLevel	= -1;	/* n->level not used */
int streams		= FALSE; /* No streams found yet */
int recursive		= FALSE; /* No %mk=B functions found yet */
int dbl			= FALSE; /* Don't convert double types */
int flt			= FALSE; /* Don't convert float  types */
int CheckForBadEdges    = FALSE;  /* Turn edge checking on/off */

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define DoAssignPragmas(x) \
{	\
  (x)->print = pragmas.print;	\
  (x)->mark  = pragmas.mark;  	\
\
  (x)->name  = pragmas.name;  	\
  (x)->line  = pragmas.line;  	\
  (x)->file  = ( pragmas.file )?(pragmas.file):sfile;	\
  (x)->funct = ( pragmas.funct )?(pragmas.funct):sfunct;	\
}

void TypeAssignPragmas(i)
     PINFO	i;
{
  DoAssignPragmas(i);
}
void NodeAssignPragmas(n)
     PNODE	n;
{
  DoAssignPragmas(n);
  if (n->type==IFXGraph && IsReductionInterface(n->CoNsT)) {
      n->mark = 'd';
  }
}
void EdgeAssignPragmas(e)
     PEDGE	e;
{
  DoAssignPragmas(e);
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define DoInitPragmas(x) \
{	\
  StandardPragmas(x); \
  (x)->print = TRUE; \
}

void PragInitPragmas(p)
     PRAGS	*p;
{
  DoInitPragmas(p);
}
void TypeInitPragmas(i)
     PINFO	i;
{
  DoInitPragmas(i);
}
void NodeInitPragmas(n)
     PNODE	n;
{
  DoInitPragmas(n);
}
void EdgeInitPragmas(e)
     PEDGE	e;
{
  DoInitPragmas(e);
}
