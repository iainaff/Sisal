/* CrayModel */

extern int ProcessorId();
#define GetProcId ProcessorId()
#define GETPROCID(v) { int x; TSKVALUE(&x); v = x; }
