/**************************************************************************/
/* FILE   **************       fromCdriver.c       ************************/
/**************************************************************************/
/* Author: Patrick Miller January  3 2001                                 */
/* Copyright (C) 2001 Patrick Miller                                      */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalInfo.h"

extern void foo PROTO((int*));
extern int NumWorkers;

int main(int argc, char** argv) {
   int x;
   if ( argc > 1 && argv[1][0] == '-' && argv[1][1] == 'w' ) {
      NumWorkers = atoi(argv[1]+2);
   }
   InitSisalRunTime();
   StartWorkers();
   foo(&x);
   StopWorkers();
   puts("THIS LINE IS SKIPPED IN COMPARISON");
   printf("X should be three, and is %d\n",x);
   return 0;
}
