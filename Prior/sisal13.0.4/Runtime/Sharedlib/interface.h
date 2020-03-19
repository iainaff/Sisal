#define COL_MAJOR 0   /* ARRAY IS COLUMN MAJOR */
#define ROW_MAJOR 1   /*    ARRAY IS ROW MAJOR */

#define PRESERVE  0    /*   PRESERVE INDEX SET */
#define REVERSE   1    /*    REVERSE INDEX SET */


#define PLO  0         /* PHYSICAL LOWER BOUND */
#define PHI  1         /* PHYSICAL UPPER BOUND */
#define LLO  2         /*  LOGICAL LOWER BOUND */
#define LHI  3         /*  LOGICAL LOWER BOUND */
#define SLO  4         /*    SISAL LOWER BOUND */

#define DIMINFO  struct DimInfo
#define DIMINFOP struct DimInfo *

struct DimInfo { 
  int LSize;           /*   LOGICAL DIMENSION SIZE */
  int DSize;           /*  PHYSICAL DIMENSION SIZE */
  int SLow;            /*    LOWER BOUND FOR SISAL */
  int Offset;          /* OFFSET TO LOGICAL REGION */
  int Mutable;         /*    IS THE ARRAY MUTABLE? */
  };

extern char *SINFOFile;
extern void InitDimInfo();
extern void OptInitDimInfo();
extern char* ParseCEscapes();
extern void ParseCommandLine();
