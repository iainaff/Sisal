/*****************************************************************************\
 * File:        spprun.c
 * Description: Preprocessor the Sisal source file for DOS DJGPP port.
 *		Uses CPP for second phase.
 * Command line:
 *              argv[0] spp             (invocation name)
 *              argv[1] spp2            
 *              argv[2] cc_command      (cc)
 *              argv[3] pig.sis         (source file)
 *              argv[4] temp.c
 *              argv[5] temp.spp
 *              argv[6] pig.i           (output file)
 *              argv[7] rest
\*****************************************************************************/

#include "../config.h"

#define NAMES_MAX_CHAR 40
#define MAX_ARGS 8
#define OK 0

static void ChangeExtension();
static void Remove();
static void StripQuotes();

int main(argc, argv)
  int argc;
  char *argv[];
{
  char cfile[NAMES_MAX_CHAR], 
       sppfile[NAMES_MAX_CHAR],
       cccmd[12],
       *sisfile, *result, *command,
       *args[MAX_ARGS];

  int r, i, sppid, resid;


  if (argc<5) {
    fprintf (stderr, "Invalid number of arguments for spprun.\n");
    exit(1);
  }

  sisfile=argv[3];
  result=argv[4];
  ChangeExtension (cfile,   argv[4], "c",   NAMES_MAX_CHAR);
  ChangeExtension (sppfile, argv[4], "spp", NAMES_MAX_CHAR);
  StripQuotes (cccmd, argv[2]);

  Remove (cfile);
  Remove (sppfile);

  r = spawnlp (P_WAIT, "cp", "cp", sisfile, cfile, NULL);

  if (r != OK) {
    fprintf (stderr, 
      "Cannot copy %s to %s\nReturned value: %d\n", sisfile, cfile, r);
    exit(1);
  }

  command = args[0] = cccmd;
  args[1] = "-E";
  args[2] = "-traditional-cpp";
  for (i = 3; i < argc-2; i++) args[i] = argv[i+2];
  args[i++] = cfile;
  args[i++] = "-o";
  args[i++] = sppfile;
  args[i] = NULL;

  r = spawnvp (P_WAIT, command, args);

  if (r != OK) {
    fprintf (stderr, "Error executing %s\n", cccmd);
    exit(1);
  }

  sppid = open (sppfile, O_RDONLY);
  if (sppid < 0) {
    fprintf (stderr, "Cannot open %s\n", sppfile);
    exit (1);
  }
  resid = open (result, O_WRONLY | O_CREAT);
  if (resid < 0) {
    fprintf (stderr, "Cannot open %s\n", result);
    exit (1);
  }

  /*** Redirecting stdin and stdout, for use by spp2         ***/
  /*** This is really a work for system(), but system() has  ***/
  /*** some problems under go32 1.11                         ***/

  close (fileno(stdin)); dup ( sppid );
  close (fileno(stdout)); dup ( resid );

  r = spawnlp (P_WAIT, argv[1], argv[1], "-B", cfile, "-R", sisfile, NULL);

  close ( resid );
  close ( sppid );

  if (r != OK) {
    fprintf (stderr, "Error executing %s\n", argv[1]);
    exit(1);
  }

  Remove (sppfile);
  Remove (cfile);

  return 0;
}


static void Remove(name)
  char *name;
{
  int r;

  if (access (name, F_OK) == OK) {
    if (access (name, W_OK) != OK) {
      fprintf (stderr, "spprun error: Cannot overwrite file %s\n", name);
      exit(1);
    }
    r = unlink (name);
    if (r != 0) { 
      fprintf (stderr, "Could not remove %s\n", name);
      exit(1);
    }
  }

}


static void ChangeExtension(result, source, newext, maxchar)
  char *result;
  char *source;
  char *newext;
  int maxchar;
{
  int i, j;
  char c;

  for (i = 0; (c=source[i]) != '.' && c != '\0'; i++)
    result[i] = c;
  if (i+strlen(newext)+1 > maxchar) {
    fprintf (stderr, "Internal overflow of string\n");
    exit(1);
  }
  if (c == '.') result[i++] = c;
  for (j = 0; (c=newext[j]) != '\0'; j++)
    result[i++] = c;
  result[i] = '\0';

}


static void StripQuotes(dest, source)
  char *dest;
  char *source;
{
  char c;
  int i, j = 0;

  for (i = 0; (c=source[i]) != '\0'; i++)
    if ( (c != '\'') && (c != '"') )
      dest[j++] = c;

}
