#include "world.h"

/* ------------------------------------------------------------ */
int Trace_TWINE		= 0;
int Trace_INTERACTIVE	= 0;
int Trace_APPEND	= 1;
int Trace_STEP		= 1;
int Trace_BREAK		= 0;
int Trace_FILE		= 0;

/* ------------------------------------------------------------ */
#define TraceSize	(100)
typedef struct {
  char		*FName;
  int		Alive;
  int		ToFile;
  int		Append;
  int		Mod;
  int		Count;
  int		TCount;
  int		Written;
} TraceTableEntry;

TraceTableEntry TraceTable[TraceSize];
static int 		IDPool = 0;

/**************************************************************************/
/* GLOBAL **************     ParseTracerCommand    ************************/
/**************************************************************************/
/* PURPOSE:  Parse commands for the tracer function			  */
/**************************************************************************/
void ParseTracerCommand(p)
     char	*p;
{
  int  len = strlen(p);

  if ( len == 0 ) goto OptionError; /* Must have an arg! */

  /* ------------------------------------------------------------ */
  if ( strncmp(p,"twine",len) == 0 ) {
    Trace_TWINE = 1;
    return;
  }
  if ( strncmp(p,"~twine",len) == 0 ) {
    Trace_TWINE = 0;
    return;
  }

  /* ------------------------------------------------------------ */
  if ( strncmp(p,"interactive",len) == 0 ) {
    Trace_INTERACTIVE = 1;
    return;
  }
  if ( strncmp(p,"~interactive",len) == 0 ) {
    Trace_INTERACTIVE = 0;
    return;
  }

  /* ------------------------------------------------------------ */
  if ( strncmp(p,"append",len) == 0 ) {
    Trace_APPEND = 1;
    return;
  }
  if ( strncmp(p,"~append",len) == 0 ) {
    Trace_APPEND = 0;
    return;
  }
  if ( strncmp(p,"overwrite",len) == 0 ) {
    Trace_APPEND = 0;
    return;
  }
  if ( strncmp(p,"~overwrite",len) == 0 ) {
    Trace_APPEND = 1;
    return;
  }

  /* ------------------------------------------------------------ */
  if ( isdigit(*p) ) {
    Trace_STEP = atoi(p);
    if ( Trace_STEP <= 0 ) goto OptionError;
    return;
  }

  /* ------------------------------------------------------------ */
  if ( strncmp(p,"quit",len) == 0 ) {
    Trace_BREAK = 1;
    return;
  }
  if ( strncmp(p,"~quit",len) == 0 ) {
    Trace_BREAK = 0;
    return;
  }
  if ( strncmp(p,"go",len) == 0 ) {
    Trace_BREAK = 0;
    return;
  }
  if ( strncmp(p,"~go",len) == 0 ) {
    Trace_BREAK = 1;
    return;
  }

  /* ------------------------------------------------------------ */
  if ( strncmp(p,"file",len) == 0 ) {
    Trace_FILE = 1;
    return;
  }
  if ( strncmp(p,"~file",len) == 0 ) {
    Trace_FILE = 0;
    return;
  }

  return;

 OptionError:
  SisalError("INVALID TRACER ARGUMENT",p);
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
static void
TraceDisplay(Entry,args,writer,f,ID)
     TraceTableEntry	*Entry;
     POINTER		args;
     void		((*writer)());
     char		*f;
     int		*ID;
{
  FILE			*HoldFD;

  /* ------------------------------------------------------------ */
  FPRINTF(stderr,"# %d] Break %s.%d\n",*ID,f,Entry->TCount);
  Entry->Count = 0;		/* Count since last displayed */

  /* ------------------------------------------------------------ */
  HoldFD = FibreOutFd;

  if ( Entry->ToFile ) {
    char			buf[100];
    SPRINTF(buf,"./%s.trace",Entry->FName);
    FibreOutFd = fopen(buf,(Entry->Append && Entry->Written)?"a":"w");
    if ( !FibreOutFd ) {
      FPRINTF(stderr,"Warning: cannot open %s\n",buf);
    }
    Entry->Written = 1;		/* Allow things to append from now on */
  } else {
    FibreOutFd = stderr;
  }

  if ( FibreOutFd ) {
    if ( FibreOutFd != stderr ) FPRINTF(FibreOutFd,"# %d] Break %s.%d\n",
					*ID,f,Entry->TCount);

    if ( Trace_TWINE ) {
      FPRINTF(FibreOutFd,"break function %s;\n",Entry->FName);
      FPRINTF(FibreOutFd,"set entry %s;\n",Entry->FName);
      FPRINTF(FibreOutFd,"set arguments \n");
    }
    writer(args);
    if ( Trace_TWINE ) FPRINTF(FibreOutFd,";\n");
  }

  /* Close the file (if opened) */
  if ( FibreOutFd && FibreOutFd != stderr ) FCLOSE(FibreOutFd);

  FibreOutFd = HoldFD;
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
static void
TraceInteractive(Entry,args,writer,f,ID)
     TraceTableEntry	*Entry;
     POINTER		args;
     void		((*writer)());
     char		*f;
     int		*ID;
{
  TraceTableEntry	*ThisEntry;
  char			com[256];
  char			*cp;
  FILE			*TTY;
  int			NOT;
  int			Save;
  int			i,j,len;
  int			Step;
  int			OptID;

  /* ------------------------------------------------------------ */
  /* Note that this breakpoint has been touched */
  Entry->Count = 0;

  /* ------------------------------------------------------------ */
  /* Get commands from /dev/tty */
  TTY = fopen("/dev/tty","r");
  if ( !TTY ) {
    FPRINTF(stderr,"Cannot trace interactively\n");
    Trace_INTERACTIVE = 0;
    return;
  }

  /* ------------------------------------------------------------ */
  /* Command loop */
  FPRINTF(stderr,"[%d] %s.%d> ",*ID,f,Entry->TCount);
  while ( fgets(com,SIZEOF(com)-1,TTY) ) {
    ThisEntry = Entry;

    /* Check for optional ID */
    for(cp=com; *cp && isspace(*cp); cp++);
    if ( isdigit(*cp) ) {
      OptID = atoi(cp);
      for(; *cp && isdigit(*cp); cp++);
      if ( OptID <= 0 || OptID > IDPool ) {
	FPRINTF(stderr,"No such entry %d\n",OptID);
	goto LoopEnd;
      }
      ThisEntry = TraceTable+OptID;
    }

    /* Check for negation */
    for(; *cp && isspace(*cp); cp++);
    if ( *cp == '~' ) {
      NOT = 1;
      cp++;
    } else {
      NOT = 0;
    }

    /* Execute the command */
    for(; *cp && isspace(*cp); cp++);
    switch ( *cp ) {
     case 'a': case 'A':
      ThisEntry->Append = NOT?(0):(1);
      FPRINTF(stderr,"%s mode\n",(ThisEntry->Append)?"APPEND":"OVERWRITE");
      break;

     case 'c': case 'C':
      return;

     case 'd': case 'D':
      TraceDisplay(Entry,args,writer,f,ID);
      break;

     case 'f': case 'F':
      ThisEntry->ToFile = NOT?(0):(1);
      FPRINTF(stderr,"%s mode\n",(ThisEntry->ToFile)?"FILE":"SCREEN");
      break;

     case 'i': case 'I':
      Trace_INTERACTIVE = NOT?(0):(1);
      FPRINTF(stderr,"%s mode\n",(Trace_INTERACTIVE)?"INTERACTIVE":"BATCH");
      break;

     case 'l': case 'L':
      FPRINTF(stderr,
	      "ID] Function         TCount Count Step Alive File Append\n");
      for(i=1;i<=IDPool;i++) {
	FPRINTF(stderr,"%2d] %s",i,TraceTable[i].FName);
	len = strlen(TraceTable[i].FName);
	for(j=len;j<16;j++) FPUTC(' ',stderr);
	FPRINTF(stderr," %6d %5d %4d %5d %4d %6d\n",
		TraceTable[i].TCount,
		TraceTable[i].Count,
		TraceTable[i].Mod,
		TraceTable[i].Alive,
		TraceTable[i].ToFile,
		TraceTable[i].Append);
      }
      break;

     case 'k': case 'K':
      ThisEntry->Alive = NOT?(1):(0);
      FPRINTF(stderr,"%s\n",(ThisEntry->Alive)?"ALIVE":"KILLED");
      break;

     case 'o': case 'O':
      ThisEntry->Append = NOT?(1):(0);
      FPRINTF(stderr,"%s mode\n",(ThisEntry->Append)?"APPEND":"OVERWRITE");
      break;

     case 'p': case 'P':
      Save = Entry->ToFile;
      Entry->ToFile = 0;
      TraceDisplay(Entry,args,writer,f,ID);
      Entry->ToFile = Save;
      break;

     case 'q': case 'Q':
      if ( UsingSdbx ) SdbxMonitor( SDBX_ERR );
      AbortParallel();

     case 's': case 'S':
      /* First skip the step command */
      for(;*cp && isalpha(*cp);cp++);
      Step = atoi(cp);
      if ( Step <= 0 ) {
	FPRINTF(stderr,"Step must be greater than zero\n");
      } else {
	ThisEntry->Mod = Step;
      }
      break;

     case 't': case 'T':
      Trace_TWINE = NOT?(0):(1);
      FPRINTF(stderr,"%s mode\n",(Trace_TWINE)?"TWINE":"FIBRE");
      break;

     case 'h': case 'H': case '?':
     default:
      FPRINTF(stderr,"Single letter commands with optional ID and ~ (not)\n");
      FPUTC('\n',stderr);
      FPRINTF(stderr,
	      "[id] [~]Append        : Append (Overwrite) trace to file\n");
      FPRINTF(stderr,
	      "        Continue      : Continue execution\n");
      FPRINTF(stderr,
	      "        Display       : Display trace (to screen or file)\n");
      FPRINTF(stderr,
	      "[id] [~]File          : Set file (screen) trace mode\n");
      FPRINTF(stderr,
	      "     [~]Interactive   : Change interactive setting\n");
      FPRINTF(stderr,
	      "        List          : List all breakpoints that have been hit\n");
      FPRINTF(stderr,
	      "[id] [~]Kill          : Kill (Resurrect) the breakpoint\n");
      FPRINTF(stderr,
	      "[id] [~]Overwrite     : Overwrite (Append) trace to file\n");
      FPRINTF(stderr,
	      "        Peek          : Display trace to screen\n");
      FPRINTF(stderr,
	      "        Quit          : Quit s.out\n");
      FPRINTF(stderr,
	      "[id]    Step n        : Set step count for breakpoint\n");
      FPRINTF(stderr,
	      "     [~]Twine         : Output in TWINE (Fibre) format\n");
    }

   LoopEnd:
    FPRINTF(stderr,"[%d] %s.%d> ",*ID,f,Entry->TCount);
  }

}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
void
_Tracer_(args,writer,f,ID)
     POINTER	args;
     void	((*writer)());
     char	*f;
     int	*ID;
{
  TraceTableEntry	*Entry;

  /* ------------------------------------------------------------ */
  /* Better get a lock or bad things will happen! */
  MY_SLOCK( SUtilityLock );

  /* ------------------------------------------------------------ */
  /* First, initialize */
  if ( !*ID ) {
    *ID = ++IDPool;
    Entry = TraceTable+(*ID);

    Entry->FName	= f;
    Entry->Alive	= 1;
    Entry->ToFile	= Trace_FILE;
    Entry->Append	= Trace_APPEND;
    Entry->Mod		= Trace_STEP;
    Entry->Count	= 0;
    Entry->TCount	= 0;
    Entry->Written	= 0;
  } else {
    Entry = TraceTable+(*ID);
  }

  /* ------------------------------------------------------------ */
  /* If dead, just return */
  if ( !Entry->Alive ) goto Finish;

  /* ------------------------------------------------------------ */
  /* Update Counts */
  Entry->Count++;
  Entry->TCount++;

  /* ------------------------------------------------------------ */
  if ( Entry->Count % Entry->Mod == 0 ) {
    if ( Trace_INTERACTIVE ) {
      TraceInteractive(Entry,args,writer,f,ID);
    } else {
      TraceDisplay(Entry,args,writer,f,ID);
    }
  }

  /* ------------------------------------------------------------ */
  if ( Trace_BREAK ) {
    if ( UsingSdbx ) SdbxMonitor( SDBX_ERR );
    AbortParallel();
  }

  /* ------------------------------------------------------------ */
  /* Better unlock or else! */
 Finish:
  FLUSH(Entry,Entry+sizeof(*Entry));
  MY_SUNLOCK( SUtilityLock );
}
