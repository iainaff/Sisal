#include "world.h"

int FixPortsToo		= TRUE;	/* Fix up Loop ports */
int AllowVMarks		= TRUE;	/* Allow %mk=V input */
int StreamsOK		= FALSE; /* Don't allow stream types */
PNODE fhead		= NULL;	/* No extra chain */
int InitialNodeLevel	= -1;	/* n->level not used */
int streams		= FALSE; /* No streams found yet */
int recursive		= FALSE; /* No %mk=B functions found yet */
int dbl			= FALSE; /* Don't convert double types */
int flt			= FALSE; /* Don't convert float  types */
int DMarkProblem	= FALSE; /* Don't mess with with %mk=d nodes */
int ExpandedEqual	= FALSE; /* Use the old def'n */
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
  (x)->print = pragmas.print;	\
  (x)->mark  = pragmas.mark;  	\
  (x)->umark = pragmas.umark;  	\
  (x)->ID    = pragmas.ID;	\
  (x)->ThinCopy = pragmas.ThinCopy; \
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
#define DoInitPragmas(x) \
{ \
  StandardPragmas(x); \
  (x)->line  = -1; \
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
