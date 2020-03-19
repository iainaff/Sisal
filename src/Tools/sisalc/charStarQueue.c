/**************************************************************************/
/* FILE   **************      charStarQueue.c      ************************/
/**************************************************************************/
/* Author: Patrick Miller December 31 2000                                */
/* Copyright (C) 2000 Patrick Miller                                      */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalInfo.h"
#include "charStarQueue.h"

/**************************************************************************/
/* GLOBAL **************          enqueue          ************************/
/**************************************************************************/
/* Append string to end of queue                                          */
/**************************************************************************/
void enqueue(charStarQueue** queueP, char* arg) {
   charStarQueue* newOne = (charStarQueue*)malloc(sizeof(charStarQueue));
   newOne->charStar = arg;
   newOne->next = 0;

   if ( *queueP == 0 ) {
      *queueP = newOne;
   } else {
      charStarQueue* queue = *queueP;
      while(queue->next) queue=queue->next;
      queue->next = newOne;
   }
}

/**************************************************************************/
/* GLOBAL **************       explodeEnqueue      ************************/
/**************************************************************************/
/* Break apart the white space delimited arg and push individual pieces.  */
/**************************************************************************/
void explodeEnqueue(charStarQueue** queue, char* arg) {
   char* leakingString;
   char* p;
   char* last;
   
   /* ----------------------------------------------- */
   /* ----------------------------------------------- */
   if ( !arg ) return;

   /* ----------------------------------------------- */
   /* Make a copy and obliterate whitespace           */
   /* ----------------------------------------------- */
   leakingString = malloc(strlen(arg)+1);
   strcpy(leakingString,arg);
   last = leakingString+strlen(leakingString);
   for(p=leakingString; *p; ++p) {
      if ( isspace(*p) ) *p = 0;
   }

   /* ----------------------------------------------- */
   /* Look through the string trying to find substr   */
   /* ----------------------------------------------- */
   for(p=leakingString; p<last; ++p) {
      if ( *p ) {
         enqueue(queue,p);
         while(*p) p++;
      }
   }
}

/**************************************************************************/
/* GLOBAL **************          dequeue          ************************/
/**************************************************************************/
/* Pull string off last element in the queue                              */
/**************************************************************************/
char* dequeue(charStarQueue** queueP) {
   charStarQueue* top = 0;
   char* result;
   
   if ( *queueP == 0 ) return 0;
   top = *queueP;
   *queueP = (*queueP)->next;
   result = top->charStar;
   free(top);
   return result;
}

/**************************************************************************/
/* GLOBAL **************         queueSize         ************************/
/**************************************************************************/
/* Count elements in the queue                                            */
/**************************************************************************/
int queueSize(charStarQueue* queue) {
   int size = 0;
   while(queue) { size++; queue = queue->next; }
   return size;
}

