/* if2modules.c,v
 * Revision 12.7  1992/11/04  22:05:08  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:35  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

#define DBNONE       0
#define M_DATABASE   1
#define M_NODATABASE 2
#define P_DATABASE   3
#define P_NODATABASE 4

char *mdbfile = NULL;                       /* MODULE DATA BASE FILE NAME */
static FILE *mdbfd = NULL;            /* MODULE DATA BASE FILE DESCRIPTOR */

static int cmode = DBNONE;                            /* COMPILATION MODE */

#define MAX_FUNCTIONS 1000

static PMDATA cetable[MAX_FUNCTIONS];     /* CALLEE TABLE AND TOP POINTER */
static int cetop = -1;


/**************************************************************************/
/* LOCAL  **************    DriveParallelMarks     ************************/
/**************************************************************************/
/* PURPOSE: FUNCTION g IS CALLED IN PARALLEL, SO MARK IT'S CALLEES        */
/*          SIMILARLY.                                                    */
/**************************************************************************/

static int DriveParallelMarks( g, Pmark )
PNODE g;
int   Pmark;
{
  register PNODE n;
  register PNODE sg;
  register PNODE f;
  register int   change;

  for ( change = FALSE, n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	change = change || DriveParallelMarks( sg, Pmark || n->smark );
      }

    if ( !IsCall( n ) )
      continue;

    f = FindFunction( n->imp->CoNsT );

    if ( !(f->Pmark) )
      if ( Pmark ) {
        f->Pmark = TRUE;
        change = TRUE;
        }
    }

  return( change );
}


/**************************************************************************/
/* GLOBAL **************   MarkParallelFunctions   ************************/
/**************************************************************************/
/* PURPOSE: ESTABLISH EXECUTION MODES FOR ALL THE FUNCTIONS. THE DEFAULT  */
/*          IS SEQUENTIAL. THIS ROUTINE ASSUMES THAT CONTROL LEAVING      */
/*          SISAL WILL NOT REENTER THROUGH A SUBSEQUENT CALL TO A SISAL   */
/*          ENTRY POINT!                                                  */
/**************************************************************************/

void MarkParallelFunctions()
{
  register PNODE f;
  register int   change = TRUE;

  while ( change ) {
    change = FALSE;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
      switch( f->type ) {
	case IFIGraph:
	  break;

	case IFLGraph:
	case IFXGraph:
	  change = change || DriveParallelMarks( f, (int)f->Pmark );
	  break;

	default:
	  Error2( "MarkParallelFunctions", "ILLEGAL GRAPH NODE" );
	}
    }
}


/**************************************************************************/
/* STATIC **************          MDataAlloc       ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A MODULE DATA BASE NODE.     */
/**************************************************************************/

static PMDATA MDataAlloc()
{
  register PMDATA m;

  m = (PMDATA) MyAlloc( (int)sizeof(MDATA) );

  m->name = NULL;

  m->kind = MDB_HELP;

  m->cnum  = 0;
  m->level = 1;
  m->pbusy = 1;

  return( m );
}


/**************************************************************************/
/* GLOBAL **************        LookupCallee       ************************/
/**************************************************************************/
/* PURPOSE: SEE IF name IS IN THE CALLEE TABLE. RETURN NULL IF IT ISN'T,  */
/*          ELSE RETURN THE ENTRY.                                        */
/**************************************************************************/

PMDATA LookupCallee( name )
char *name;
{
  register int    i;
  register PMDATA ce;

  for ( i = 0; i <= cetop; i++ ) {
    ce = cetable[i];

    if ( strcmp( ce->name, name ) == 0 )
      return( ce );
    }

  return( NULL );
}


/**************************************************************************/
/* GLOBAL **************     ReadModuleDataBase    ************************/
/**************************************************************************/
/* PURPOSE: READ THE MODULE DATA BASE FILE AND BUILD THE INTERNAL DATA    */
/*          BASE FOR FUTURE PROCESSING.                                   */
/**************************************************************************/

static char name[100];
static char kind;

void ReadModuleDataBase()
{
  register PNODE  f;
  register PMDATA ce;
  int      cnum, pbusy, dblevel;

  /* DETERMINE COMPILATION TYPE */ 
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    if ( f->mark == 's' ) {
      if ( mdbfile == NULL ) {
	f->Pmark = TRUE;            /* SORRY, MUST ASSUME THE WORST!!! */
        cmode = M_NODATABASE;
	}
      else
        cmode = M_DATABASE;

      break;
      }

    if ( f->mark == 'e' || f->mark == 'c' || f->mark == 'f' ) {
      if ( mdbfile == NULL )
	cmode = P_NODATABASE;
      else
	cmode = P_DATABASE;

      break;
      }
    }

  if ( mdbfile == NULL )
    return;

  mdbfd = fopen( mdbfile, "r" );

  if ( mdbfd != NULL ) {
    while ( fscanf( mdbfd, "%s %c %d %d %d\n", 
		    name, &kind, &cnum, &dblevel, &pbusy ) != EOF ) {

      if ( (ce = LookupCallee( name )) == NULL ) {
        ce = MDataAlloc();
	ce->name  = CopyString( name );
	ce->kind  = kind;
	ce->cnum  = cnum;
	ce->pbusy = pbusy;
	ce->level = dblevel;

	if ( ++cetop >= MAX_FUNCTIONS )
	  Error2( "ReadModuleDataBase", "cetable OVERFLOW!" );

	cetable[cetop] = ce;
	}
      else
	Error2( "ReadModuleDataBase", "DUPLICATE NAMES IN DATA BASE" );
      }

    (void)fclose( mdbfd );
    }

  /* INITIALIZE 's' XGraphs */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    switch ( f->type ) {
      case IFXGraph:
        if ( (ce = LookupCallee( f->G_NAME )) == NULL )
	  break;

	switch ( ce->kind ) {
	  case MDB_PAR:
	  case MDB_PARX:
	    f->Pmark = TRUE;
	    f->cnum  = ce->cnum;
	    f->level = ce->level;
	    f->pbusy = ce->pbusy;
	    break;

	  case MDB_SEQ:
	  case MDB_SEQX:
	    break;

	  default:
	    Error2( "ReadModuleDataBase", "ILLEGAL KIND FIELD" );
          }

	break;

      default:
	break;
      }
}


/**************************************************************************/
/* GLOBAL **************    WriteModuleDataBase    ************************/
/**************************************************************************/
/* PURPOSE: WRITE THE INTERNAL DATA BASE TO THE MODULE DATA BASE FILE.    */
/**************************************************************************/

void WriteModuleDataBase()
{
  register int     i;
  register PMDATA  ce;
  register PNODE   f;

  MarkParallelFunctions();

  if ( mdbfile == NULL )
    return;

  /* UPDATE DATA BASE FOR XGraphs */
  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    switch ( f->type ) {
      case IFXGraph:
        ce = LookupCallee( f->G_NAME );

	if ( ce == NULL ) {
          ce = MDataAlloc();

	  if ( ++cetop >= MAX_FUNCTIONS )
	    Error2( "WriteModuleDataBase", "cetable OVERFLOW!" );

	  cetable[cetop] = ce;

	  ce->name = CopyString( f->G_NAME );
	  ce->cnum  = f->cnum;
	  ce->level = f->level;
	  ce->pbusy = f->pbusy;

	  if ( f->Pmark )
	    ce->kind  = MDB_PARX;
	  else
	    ce->kind  = MDB_SEQX;

	  break;
	  }

	/* IS f CALLED IN PARALLEL? */
	if ( f->Pmark ) {
	  switch ( ce->kind ) {
	    case MDB_PAR:
	    case MDB_PARX:
	    case MDB_SEQ:
	    case MDB_SEQX:
	      ce->kind  = MDB_PARX;
	      ce->cnum  = f->cnum;
	      ce->level = f->level;
	      ce->pbusy = f->pbusy;
	      break;

	    default:
	      Error2( "WriteModuleDataBase", "ILLEGAL KIND FIELD" );
	    }

          break;
	  }

	/* f IS CALLED SEQUENTIALLY */

	switch ( ce->kind ) {
	  case MDB_PAR:
	    ce->kind = MDB_PARX;
	    break;

	  case MDB_SEQ:
	    ce->kind = MDB_SEQX;
	    break;

	  case MDB_PARX:
	  case MDB_SEQX:
	    break;

	  default:
	    Error2( "WriteModuleDataBase", "ILLEGAL KIND FIELD SEQ" );
	  }

	break;

      default:
	break;
      }

  /* PROCESS IGraphs */

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    switch ( f->type ) {
      case IFIGraph:
	if ( f->mark != 's' )
	  break;

        ce = LookupCallee( f->G_NAME );

	if ( ce == NULL ) {
          ce = MDataAlloc();

	  if ( ++cetop >= MAX_FUNCTIONS )
	    Error2( "WriteModuleDataBase", "cetable OVERFLOW!" );

	  cetable[cetop] = ce;

	  ce->name = CopyString( f->G_NAME );
	  ce->cnum  = f->cnum;
	  ce->level = f->level;
	  ce->pbusy = f->pbusy;

	  if ( f->Pmark )
	    ce->kind  = MDB_PAR;
	  else
	    ce->kind  = MDB_SEQ;

	  break;
	  }

	/* IS f CALLED IN PARALLEL? */
	if ( f->Pmark ) {
	  ce->cnum  = f->cnum;
	  ce->level = f->level;
	  ce->pbusy = f->pbusy;

	  switch ( ce->kind ) {
	    case MDB_SEQ:
	      ce->kind = MDB_PAR;
	      break;

	    case MDB_SEQX:
	      ce->kind = MDB_PAR;
	      break;

	    case MDB_PARX:
	    case MDB_PAR:
	      break;

	    default:
	      Error2( "WriteModuleDataBase", "ILLEGAL KIND FIELD" );
	    }

          break;
	  }

	break;

      default:
	break;
      }


  if ( (mdbfd = fopen( mdbfile, "w" )) == NULL )
    Error2( "CAN'T OPEN", mdbfile );

  /* WRITE THE STUFF */
  for ( i = 0; i <= cetop; i++ ) {
    ce = cetable[i];

    FPRINTF( mdbfd, "%s %c %d %d %d\n", ce->name, ce->kind,
		    ce->cnum, ce->level, ce->pbusy       );
    }

  (void)fclose( mdbfd );
}


/**************************************************************************/
/* GLOBAL **************       AddModuleStamp      ************************/
/**************************************************************************/
/* PURPOSE: ADD THE APPROPRIATE MODULE STAMP TO THE STAMP SYSTEM.         */
/**************************************************************************/

void AddModuleStamp()
{
  switch( cmode ) {
    case M_DATABASE:
      AddStamp( MDBASE,  "     CANN MODULES: ModuleDataBase" );
      break;

    case M_NODATABASE:
      AddStamp( MNODBASE,  "     CANN MODULES: ModuleNoDataBase" );
      break;

    case P_DATABASE:
      AddStamp( PDBASE,  "     CANN MODULES: ProgramDataBase" );
      break;

    case P_NODATABASE:
      AddStamp( PNODBASE,  "     CANN MODULES: ProgramNoDataBase" );
      break;

    default:
      Error2( "AddModuleStamp", "ILLEGAL cmode" );
    }
}
