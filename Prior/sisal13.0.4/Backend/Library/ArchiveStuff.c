#include "world.h"

/* ------------------------------------------------------------ */
/* Add a name to a namelink list				*/
/* ------------------------------------------------------------ */
void AddName(list,nm)
     namelink	**list;
     char	*nm;
{
  namelink	*work;

  work = (namelink*)(malloc(sizeof(namelink))); 
  work->name = nm; 
  work->next = *list; 
  *list = work; 
}

/* ------------------------------------------------------------ */
/* Build the export list for the current IF1/2 internal tree	*/
/* ------------------------------------------------------------ */
void BuildHaveList(HaveP)
     namelink	**HaveP;
{
  PNODE		F;

  for(F = glstop->gsucc; F; F = F->gsucc) {
    switch ( F->type ) {
    case IFXGraph:
      AddName(HaveP,F->G_NAME);
      break;
    default:
      ;
    }
  }
}

/* ------------------------------------------------------------ */
/* Dump out a namelink list to stdout				*/
/* ------------------------------------------------------------ */
void DumpList(p)
     namelink	*p;
{
  for(; p; p = p->next) {
    if ( *p->name ) printf("%s ",p->name);
  }
  putchar('\n');
}

/* ------------------------------------------------------------ */
/* Search for a name in a namelink list				*/
/* ------------------------------------------------------------ */
int InNameList(name,list)
     char	*name;
     namelink	*list;
{
  namelink	*nl;
  char		lowername[MAX_PATH],*p;

  for(strcpy(lowername,name), p=lowername; *p; p++) {
    if (isupper(*p)) *p = tolower(*p);
  }
  for(nl=list; nl; nl = nl->next) {
    if ( strcmp(nl->name,lowername) == 0 ) return 1;
  }

  return 0;
}

/* $Log$
 * Revision 1.1  1993/06/15  20:47:00  miller
 * Library support.
 * */
