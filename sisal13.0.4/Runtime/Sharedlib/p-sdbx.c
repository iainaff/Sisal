#include "world.h"

#define T_NOTUSED   1
#define T_BREAK     2
#define T_CONT      3
#define T_HELP      4
#define T_LIST      5
#define T_PRINT     6
#define T_QUIT      7
#define T_STEP      8
#define T_WHERE     9
#define T_RUN       10
#define T_DISPLAY   11
#define T_FUNCTIONS 12
#define T_BOUNDS    13

#define IsDigit(x)  (((x) >= '0') && ((x) <= '9'))

static FILE *CurrentOpenFile = NULL;
static char *CurrentOpenFileName = "\0";
static int   CurrentLineNumber = 0;
static int   Number1 = -1;
static int   Number2 = -1;

#define OK        0
#define ERROR     1

#define A_NONE    0
#define A_STEP    1
#define A_BREAKF  2
#define A_BREAKR  3
#define A_BREAKL  4
#define A_BREAKE  5

struct SdbxInfo SdbxState  = { A_NONE, NULL, "", 0, 0, 0, NULL, NULL };
struct SdbxInfo SdbxAction = { A_NONE, NULL, "", 0, 0, 0, NULL, NULL };

#define MAX_WHERE  1000
#define MAX_SCOPES 1000
#define MAX_WORD    200
#define MAX_LINE    202

static char Line[MAX_LINE];
static char Command[MAX_WORD];
static char Arg1[MAX_WORD];
static char Arg2[MAX_WORD];

static int SDBX_Token;

static int WhereTop = -1;
static struct SdbxInfo WhereStack[MAX_WHERE];

struct SdbxValue *SdbxScope     = NULL;
int               SdbxScopeSize = 0;

struct SdbxValue *SdbxCurrentScope = NULL;
char             **SdbxCurrentFunctionList = NULL;

static int FirstSdbxCall = TRUE;
static int SdbxMonitorCode = SDBX_NONE;


static char *CopyString( String ) 
char *String;
{
  extern char *Alloc();

  return( (char *) strcpy( Alloc((int)strlen(String)+1), String ) );
}


static char *LowerCase( String )
char *String;
{
  register char *Base;
  register char *Ptr;

  for ( Ptr = Base = String; *Ptr != '\0'; Ptr++ )
    if ( (*Ptr >= 'A') && (*Ptr <= 'Z') )
      *Ptr = 'a' + (*Ptr - 'A'); 

  return( Base );
}


static void CloseFile()
{
  if ( CurrentOpenFile != NULL )
    FCLOSE( CurrentOpenFile );

  CurrentOpenFile = NULL;
  CurrentOpenFileName = "\0";
  CurrentLineNumber = 0;
}


static int OpenFile( FileName )
char *FileName;
{
   if ( FileName == NULL )
     return( ERROR );

   if ( strcmp( FileName, CurrentOpenFileName ) == 0 )
     if ( CurrentOpenFile != NULL )
       return( OK );

   if ( CurrentOpenFile != NULL ) {
     CloseFile( );
     CurrentOpenFile = NULL;
     }

   CurrentOpenFileName = FileName;
   CurrentLineNumber   = 0;

   if ( (CurrentOpenFile = fopen( FileName, "r" )) == NULL ) {
     PRINTF( " cannot open [%s]\n", FileName );
     return( ERROR );
     }

  return( OK );
}

static int ReadLine()
{
  register int  Ch;
  register int  Idx;

  for ( Idx = 0; Idx < MAX_LINE-2 ; /* NOTHING */ ) {
    Ch = getc( CurrentOpenFile );

    if ( Ch == EOF ) {
      if (Idx > 0)
        Line[Idx++] = '\n';

      Line[Idx++] = '\0';
      return( EOF );
      }

    if ( Idx >= MAX_LINE-2 ) {
      Line[Idx++] = '\n';
      break;
      }

    Line[Idx++] = Ch;

    if ( Ch == '\n' )
      break;
    }

  Line[Idx] = '\0';
  return( ~EOF );
}


static void GetLine( LineNumber )
int LineNumber;
{
  register int Idx;

  if ( LineNumber == CurrentLineNumber )
    return;

  if ( LineNumber <= 0 ) {
    Line[0] = '\0';
    return;
    }

  if ( LineNumber < CurrentLineNumber ) {
    rewind( CurrentOpenFile );
    CurrentLineNumber = 0;
    }

  for ( Idx = CurrentLineNumber+1; Idx <= LineNumber; Idx++ ) {
    if ( ReadLine() == EOF ) {
      Line[0] = '\0';
      return;
      }
    }

  CurrentLineNumber = LineNumber;
}


static void PrintSdbxPrompt( Message )
char *Message;
{
  if ( Message != NULL ) {
    PRINTF( " [%s]\n", Message );

    if ( !UsingSdbx ) {
      PRINTF( " [commands: bounds break cont functs help" );
      PRINTF( " list names print quit run step where]\n" );
      }
    }

  PRINTF( "sdbx-> " );
}


static void PrintSdbxHelp()
{
  PRINTF( " bounds    NAME        [print the bounds of array NAME]\n" );
  PRINTF( " break                 [continue and break just inside the next function]\n" );
  PRINTF( " break     NAME        [continue and break just inside function NAME]\n" );
  PRINTF( " break     return      [continue and break just before next function return]\n" );
  PRINTF( " break     end         [continue and break at next scope end]\n" );
  PRINTF( " break     LINE        [continue and break on completion of line LINE]\n" );
  PRINTF( " cont                  [continue execution]\n" );
  PRINTF( " functs                [list the name of each function in the module]\n" );
  PRINTF( " help                  [this command]\n" );
  PRINTF( " list                  [list the previously executed line]\n" );
  PRINTF( " list      LINE        [list line LINE ]\n" );
  PRINTF( " list      LINE1 LINE2 [list lines LINE1 through LINE2 inclusive]\n" );
  PRINTF( " names                 [list all currently defined NAMES]\n" );
  PRINTF( " print     NAME        [print the value of NAME]\n" );
  PRINTF( " print     NAME  FILE  [append the value of NAME to FILE]\n" );
  PRINTF( " quit                  [abort execution]\n" );
  PRINTF( " run                   [identical to cont]\n" );
  PRINTF( " step                  [execute the next line and return to sdbx]\n" );
  PRINTF( " where                 [list all the active functions]\n" );
  PRINTF( "*special names:        [$NUM, scope or function result]\n" ); 
  PRINTF( "                       [#1, array of previous dereference]\n" ); 
  PRINTF( "                       [%%1, denominator of previous division]\n" ); 
}


static void GetWord( Word )
char *Word;
{
  register int  Ch;
  register int  Count;

  for (;;) {
    Ch = getc(stdin);

    if ( Ch == EOF )
      AbortParallel();

    if ( Ch == '\n' ) {
      UNGETC( Ch, stdin );
      *Word = '\0';
      return;
      }

    if ( Ch == ' ' )
      continue;

    break;
    }

  Count = 1;

  *Word = Ch;
  Word++;

  while ( (Ch = getc(stdin)) != ' ' ) {
    if ( Count > MAX_WORD )
      SisalError( "GetWord", "Word OVERFLOW!!!" );

    *Word = Ch;
    Word++;

    if ( Ch == '\n' ) {
      UNGETC( Ch, stdin );
      Word--;
      break;
      }

    if ( Ch == EOF )
      AbortParallel();
    }

  *Word = '\0';
}


static void GetCommand()
{
  register int Ch;

  Command[0] = '\0';
  Arg1[0] = '\0';
  Arg2[0] = '\0';

StartAgain:

  GetWord( Command );

  switch( Ch = Command[0] ) {
    case '\0':
      if ( (Ch = getc(stdin)) != '\n' )
        SisalError( "GetNextCommand", "NEWLINE EXPECTED" );

      goto DoItAgain;

    case '\n':
      SisalError( "GetNextCommand", "NEWLINE ENCOUTERED!!!" );

    case 'b':
      if ( Command[1] == 'o' ) { /* bounds name */
        SDBX_Token = T_BOUNDS;

        GetWord( Arg1 );

        if ( (Ch = Arg1[0]) == '\0' )
	  goto SyntaxError;

	break;
	}

      SDBX_Token = T_BREAK;    /* break name OR break line */

      GetWord( Arg1 );

      if ( Arg1[0] == '\0' ) {
        Arg1[0] = '*';
        Arg1[1] = '\0';
        Number1 = -1;
        break;
        }

      Ch = Arg1[0];

      if ( IsDigit(Ch) ) {
        if ( (Number1 = atoi( Arg1 )) <= 0 )
          goto SyntaxError;
        }
      else
        Number1 = -1;

      break;

    case 'r':
      SDBX_Token = T_RUN;
      break;

    case 's':
      SDBX_Token = T_STEP;
      break;

    case 'f':
      SDBX_Token = T_FUNCTIONS;
      break;

    case 'l':
      SDBX_Token = T_DISPLAY;

      GetWord( Arg1 );

      if ( (Ch = Arg1[0]) == '\0' ) {
        Number1 = -1;
        break;
        }

      if ( !IsDigit( Ch ) )
        goto SyntaxError;

      Number1 = atoi( &(Arg1[0]) );

      if ( Number1 < 1 )
        goto SyntaxError;

      GetWord( Arg2 );

      if ( (Ch = Arg2[0]) == '\0' ) {
        Number2 = Number1;
        break;
        }

      Number2 = atoi( Arg2 );

      if ( Number2 < 1 )
        goto SyntaxError;

      if ( Number2 < Number1 )
        goto SyntaxError;

      break;

    case 'c':
      SDBX_Token = T_CONT;
      break; 

    case 'h':
      SDBX_Token = T_HELP;
      break; 

    case 'n':
      SDBX_Token = T_LIST;
      break; 

    case 'p':             /* print name [file] */
      SDBX_Token = T_PRINT;

      GetWord( Arg1 );

      if ( Arg1[0] == '\0' )
        goto SyntaxError;

      LowerCase( Arg1 );
      GetWord( Arg2 );
      break; 

    case 'q':
      SDBX_Token = T_QUIT;
      break; 

    case 'w':
      SDBX_Token = T_WHERE;
      break; 

    default:

SyntaxError:
      if ( Ch == '\0' )
        PRINTF( " usage error [try help]\n" );
      else
        PRINTF( " syntax error at or near [%c]\n", Ch );

      while ( (Ch = getc(stdin)) != '\n' )
        if ( Ch == EOF )
          AbortParallel();

DoItAgain:
      PRINTF( "sdbx-> " );
      goto StartAgain;
    }

  while ( (Ch = getc(stdin)) != '\n' )
    if ( Ch == EOF )
      AbortParallel();
}


static void DisplayFunctions()
{
  register char **Ptr;
  register int    Count;

  Ptr = SdbxCurrentFunctionList;

  for ( Count = 0; (*Ptr)[0] != '\0'; Ptr++ ) {
    PRINTF( " %-10s", *Ptr );

    if ( Count % 7 == 6 )
      PRINTF( "\n" );

    Count++;
    }

  if ( Count % 7 != 0 )
    PRINTF( "\n" );
}


static void DisplayText()
{
  register int Lo;
  register int Hi;

  if ( OpenFile( SdbxState.File ) == ERROR )
    return;

  if ( Number1 == -1 ) {
    Lo = SdbxState.Line;
    Hi = SdbxState.Line;
  } else {
    Lo = Number1;
    Hi = Number2;
    }

  for ( /* NOTHING */; Lo <= Hi; Lo++ ) {
    GetLine( Lo );
  
    if ( Line[0] != '\0' )
      PRINTF( "%s%4d: %s", (Lo == SdbxState.Line)? "*" : " ", Lo, Line ); 
    }
}


static void ProcessCommands()
{
  register int      Count;
  register int      Idx;
  register struct   SdbxValue *Scope;
  register FILE    *BackupFd;
  register FILE    *TmpFd;

  for ( ;; ) {
    GetCommand();

    switch( SDBX_Token ) {
      case T_BREAK:
        if ( Number1 <= 0 ) {
          if ( strcmp( Arg1, "return" ) == 0 ) {
            SdbxAction.Action   = A_BREAKR;
            SdbxAction.Function = "";
            SdbxAction.Line     = 0;

            return;
            }

          if ( strcmp( Arg1, "end" ) == 0 ) {
            SdbxAction.Action   = A_BREAKE;
            SdbxAction.Function = "";
            SdbxAction.Line     = 0;

            return;
            }

          SdbxAction.Function = LowerCase( CopyString(Arg1) );
          SdbxAction.Action   = A_BREAKF;
          SdbxAction.Line     = 0;
        } else {
          SdbxAction.Action   = A_BREAKL;
          SdbxAction.Function = "";
          SdbxAction.Line     = Number1;
          }

        return;

      case T_RUN:
      case T_CONT:
        return;

      case T_HELP:
        PrintSdbxHelp();
        break;

      case T_LIST:
        if ( (Scope = SdbxCurrentScope) == NULL )
          break;

        if ( Scope[0].Name == NULL )
          break;

        for ( Count = 0, Idx = 0; /* NO TEST */ ; Idx++ ) {
          if ( Scope[Idx].Name == NULL )
            break;

          if ( !(Scope[Idx].Active) )
            continue;

          PRINTF( " %-10s", Scope[Idx].Name );

          if ( Count % 7 == 6 )
            PRINTF( "\n" );

          Count++;
          }

        if ( Count % 7 != 0 )
          PRINTF( "\n" );

        break;

      case T_BOUNDS:
        if ( (Scope = SdbxCurrentScope) == NULL ) {
          PRINTF( " [%s not defined]\n", Arg1 );
          break;
          }

        if ( Scope[0].Name == NULL ) {
          PRINTF( " [%s not defined]\n", Arg1 );
          break;
          }

        for ( Idx = 0; /* NO TEST */ ; Idx++ ) {
          if ( Scope[Idx].Name == NULL ) {
            PRINTF( " [%s not defined]\n", Arg1 );
            break;
            }

          if ( strcmp( Scope[Idx].Name, Arg1 ) == 0 ) {
            if ( !(Scope[Idx].Active) ) {
              PRINTF( " [%s not defined]\n", Arg1 );
              break;
              }

	    if ( !(Scope[Idx].ArrayType) ) {
	      PRINTF( " [%s is not an array]\n", Arg1 );
	      break;
	      }

	    PRINTF( " [lower=%d,upper=%d,size=%d]\n",
                    ((ARRAYP)(Scope[Idx].Value.PtR))->LoBound,
                    ((ARRAYP)(Scope[Idx].Value.PtR))->LoBound +
                    ((ARRAYP)(Scope[Idx].Value.PtR))->Size - 1,
                    ((ARRAYP)(Scope[Idx].Value.PtR))->Size );

	    break;
	    }
	  }

	break;

      case T_DISPLAY:
        DisplayText();
        break;

      case T_PRINT:
        if ( (Scope = SdbxCurrentScope) == NULL ) {
          PRINTF( " [%s not defined]\n", Arg1 );
          break;
          }

        if ( Scope[0].Name == NULL ) {
          PRINTF( " [%s not defined]\n", Arg1 );
          break;
          }

        for ( Idx = 0; /* NO TEST */ ; Idx++ ) {
          if ( Scope[Idx].Name == NULL ) {
            PRINTF( " [%s not defined]\n", Arg1 );
            break;
            }


          if ( strcmp( Scope[Idx].Name, Arg1 ) == 0 ) {
            if ( !(Scope[Idx].Active) ) {
              PRINTF( " [%s not defined]\n", Arg1 );
              break;
              }

            BackupFd = FibreOutFd;
            TmpFd    = NULL;

            if ( Arg2[0] != '\0' ) {
              if ( (TmpFd = fopen( Arg2, "a" )) == NULL ) {
                PRINTF( " [cannot open %s]\n", Arg2 );
                break;
                }

              FibreOutFd = TmpFd;
              }
            else
              FibreOutFd = stdout;

            switch( Scope[Idx].Kind ) {
              case SDBX_INT:
                FPRINTF( FibreOutFd, " %s = %d\n", 
                         Scope[Idx].Name, Scope[Idx].Value.InT );
                break;

              case SDBX_DBL:
                FPRINTF( FibreOutFd, " %s = %e\n",
                         Scope[Idx].Name, Scope[Idx].Value.DbL );
                break;

              case SDBX_PTR:
                FPRINTF( FibreOutFd, " %s =\n", Scope[Idx].Name );
                Scope[Idx].PrintRoutine( Scope[Idx].Value.PtR );
                break;

              default:
                SisalError( "SdbxMoniitor", "ILLEGAL VALUE KIND" );
              }

            if ( TmpFd != NULL )
              FCLOSE( TmpFd );

            FibreOutFd = BackupFd;
            break;
            }
          }

        break;

      case T_QUIT:
        AbortParallel();
        break;

      case T_FUNCTIONS:
        if ( SdbxCurrentFunctionList != NULL )
          DisplayFunctions();

        break;

      case T_STEP:
        SdbxAction.Action = A_STEP;
	SdbxAction.Line   = SdbxState.Line;
        return;

      case T_WHERE:
        if ( WhereTop < 0 )
          PRINTF( " [in program or module startup routine]\n" );

        for ( Idx = WhereTop; Idx >= 0; Idx-- )
          PRINTF( " [%s,%s]\n", WhereStack[Idx].Function,
                  (WhereStack[Idx].File == NULL)? "UNKNOWN.sis" : 
                  WhereStack[Idx].File );
        break;
  
      default:
        SisalError( "ProcessCommands", "ILLEGAL TOKEN!!!" );
      }

    PrintSdbxPrompt( (char*)NULL );
    }
}


void SdbxHandler( AtLine )
int AtLine;
{
  register char *Name;

  switch ( SdbxAction.Action ) {
    case A_NONE:
      break;

    case A_STEP:
      if ( SdbxMonitorCode == SDBX_SEXIT ) {
        SdbxAction.Action = A_NONE;
        Number1 = -1;
        DisplayText();
        PrintSdbxPrompt( (char*)NULL );
        ProcessCommands();
        break;
	}

      if ( SdbxMonitorCode == SDBX_POP ) {
        SdbxAction.Action = A_NONE;
        PrintSdbxPrompt( "end function" );
        ProcessCommands();
        break;
	}

      if ( SdbxAction.Line == SdbxState.Line )
	break;

      SdbxAction.Action = A_NONE;
      Number1 = -1;
      DisplayText();
      PrintSdbxPrompt( (char*)NULL );
      ProcessCommands();
      break;

    case A_BREAKF:
      if ( AtLine )
        break;

      Name = LowerCase( CopyString( SdbxState.Function ) );

      if ( SdbxAction.Function[0] != '*' ) {
        if ( strcmp( SdbxAction.Function, Name ) != 0 ) {
          DeAlloc( (POINTER)Name );
          break;
          }
        }

      Number1 = -1;
      DisplayText();
      DeAlloc( (POINTER)Name );
      DeAlloc( (POINTER)SdbxAction.Function );
      SdbxAction.Action = A_NONE;
      PrintSdbxPrompt( (char*)NULL );
      ProcessCommands();
      break;

    case A_BREAKR:
      if ( SdbxMonitorCode != SDBX_POP )
        break;

      SdbxAction.Action = A_NONE;
      PrintSdbxPrompt( "end function" );
      ProcessCommands();
      break;

    case A_BREAKE:
      if ( SdbxMonitorCode != SDBX_SEXIT )
        break;

      SdbxAction.Action = A_NONE;
      PrintSdbxPrompt( "end scope" );
      ProcessCommands();
      break;

    case A_BREAKL:
      if ( SdbxAction.Line != SdbxState.Line )
        break;

      Number1 = -1;
      DisplayText();
      SdbxAction.Action = A_NONE;
      PrintSdbxPrompt( (char*)NULL );
      ProcessCommands();
      SdbxState.Line = 0;    /* FORCE A CHANGE!! */
      break;

    default:
      SisalError( "SdbxHandler", "ILLEGAL ACTION" );
    }
}


void SdbxMonitor( SdbxCode )
int SdbxCode;
{
  register struct SdbxValue **ScopeStack;
  register struct SdbxValue *Scope;
  register int    ScopeStackTop;
  register int    ScopeSize;
  register int    Idx;

  SdbxMonitorCode = SdbxCode;

  if ( FirstSdbxCall ) {
    rewind( stdin );
    FirstSdbxCall = FALSE;
    }

  switch( SdbxCode ) {
    case SDBX_SENTER:
      ScopeStack    = WhereStack[WhereTop].ScopeStack;
      ScopeStackTop = WhereStack[WhereTop].ScopeStackTop;
      ScopeSize     = WhereStack[WhereTop].ScopeSize;

      if ( (++ScopeStackTop) >= MAX_SCOPES )
        SisalError( "SdbxMonitor", "ScopeStack OVERFLOW!!!" );

      Scope = (struct SdbxValue *) 
              Alloc( SIZEOF(struct SdbxValue)*(ScopeSize+1) );

      ScopeStack[ScopeStackTop] = Scope;

      WhereStack[WhereTop].ScopeStackTop = ScopeStackTop;

      for ( Idx = 0; Idx < ScopeSize; Idx++ )
        Scope[Idx] = SdbxCurrentScope[Idx];

      Scope[Idx].Name = NULL;

      SdbxCurrentScope = Scope;
      break;

    case SDBX_SEXIT:
      SdbxHandler( TRUE );

      ScopeStack    = WhereStack[WhereTop].ScopeStack;
      ScopeStackTop = WhereStack[WhereTop].ScopeStackTop;

      DeAlloc( (POINTER)ScopeStack[ScopeStackTop] );

      WhereStack[WhereTop].ScopeStackTop = (--ScopeStackTop);

      SdbxCurrentScope = ScopeStack[ScopeStackTop];
      break;

    case SDBX_PUSH:
      if ( SdbxScopeSize < 0 )
        SdbxScopeSize = 0;

      ScopeStack = (struct SdbxValue **) 
                   Alloc( SIZEOF(struct SdbxValue *) * MAX_SCOPES );

      ScopeStack[0] = (struct SdbxValue *) 
                      Alloc( SIZEOF(struct SdbxValue)*(SdbxScopeSize+1) );

      Scope = ScopeStack[0];

      for ( Scope = ScopeStack[0], Idx = 0; Idx < SdbxScopeSize; Idx++ ) {
        Scope[Idx] = SdbxScope[Idx];
        }

      Scope[Idx].Name = NULL;

      if ( WhereTop+1 >= MAX_WHERE )
        SisalError( "SdbxMonitor", "WhereStack OVERFLOW!!!" );

      WhereTop++;
      WhereStack[WhereTop].Function   = SdbxState.Function;
      WhereStack[WhereTop].Line       = SdbxState.Line;
      WhereStack[WhereTop].File       = SdbxState.File;

      WhereStack[WhereTop].ScopeStack    = ScopeStack;
      WhereStack[WhereTop].ScopeStackTop = 0;
      WhereStack[WhereTop].ScopeSize     = SdbxScopeSize;
      WhereStack[WhereTop].FunctionList  = SdbxCurrentFunctionList;

      SdbxCurrentScope = Scope;
      break;

    case SDBX_POP:
      SdbxHandler( TRUE );

      ScopeStack = WhereStack[WhereTop].ScopeStack;

      DeAlloc( (POINTER)ScopeStack[0] );
      DeAlloc( (POINTER)ScopeStack );
      WhereTop--;

      if ( WhereTop >= 0 ) {
        ScopeStack    = WhereStack[WhereTop].ScopeStack;
        ScopeStackTop = WhereStack[WhereTop].ScopeStackTop;
        SdbxCurrentFunctionList = WhereStack[WhereTop].FunctionList;

        SdbxCurrentScope = ScopeStack[ScopeStackTop];
        }
      else
        SdbxCurrentScope = NULL;

      break;

    case SDBX_INT:
      PrintSdbxPrompt( "user interrupt" );
      ProcessCommands();
      break;

    case SDBX_FPE:
      PrintSdbxPrompt( "floating point exception" );
      ProcessCommands();
      AbortParallel();
      break;

    case SDBX_ESTART:
      PrintSdbxPrompt( "entering sdbx" );
      UsingSdbx = TRUE;

      ProcessCommands();
      break;

    case SDBX_ESTOP:
      PrintSdbxPrompt( "sdbx processing complete" );
      UsingSdbx = FALSE;
      ProcessCommands();
      break;

    case SDBX_DB0:
      Number1 = -1;
      DisplayText();
      PrintSdbxPrompt( "division by zero" );
      ProcessCommands();
      PRINTF( " [unable to continue due to the division by zero]\n" );
      AbortParallel();
      break;

    case SDBX_ERR:
      Number1 = -1;
      DisplayText();
      PrintSdbxPrompt( "Sisal run time error" );
      ProcessCommands();
      PRINTF( " [unable to continue due to the Sisal run time error]\n" );
      AbortParallel();
      break;

    case SDBX_IERR:
      Number1 = -1;
      DisplayText();
      PrintSdbxPrompt( "interface array descriptor error" );
      ProcessCommands();
      PRINTF( " [unable to continue due to the descriptor error]\n" );
      AbortParallel();
      break;

    case SDBX_BDS:
      Number1 = -1;
      DisplayText();
      PrintSdbxPrompt( "subscript error" );
      ProcessCommands();
      PRINTF( " [unable to continue due to the subscript error]\n" );
      AbortParallel();
      break;

    default:
      SisalError( "SdbxMonitor", "ILLEGAL SdbxCode" );
    }

  SdbxMonitorCode = SDBX_NONE;
}
