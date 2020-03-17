#ifndef CHARSTARQUEUE_H
#define CHARSTARQUEUE_H

/**************************************************************************/
/* FILE   **************      charStarQueue.h      ************************/
/**************************************************************************/
/* Author: Patrick Miller December 31 2000                                */
/* Copyright (C) 2000 Patrick Miller                                      */
/**************************************************************************/
/*
 * $Log:
*/
/**************************************************************************/


/* ----------------------------------------------- */
/* We need a lot of FIFO queue structures          */
/* ----------------------------------------------- */
typedef struct charStarQueue {
   char* charStar;
   struct charStarQueue* next;
} charStarQueue;

extern void enqueue PROTO((charStarQueue** queue, char* arg));
extern void explodeEnqueue PROTO((charStarQueue** queue, char* arg));
extern char* dequeue PROTO((charStarQueue** queue));
extern int queueSize PROTO((charStarQueue* queue));

#endif
