#ifndef _CHARSTAR_QUEUE_H
#define _CHARSTAR_QUEUE_H

/* ----------------------------------------------- */
/* We need a lot of FIFO queue structures          */
/* ----------------------------------------------- */
typedef struct charStarQueue {
   char* charStar;
   struct charStarQueue* next;
} charStarQueue;

extern void enqueue(charStarQueue** queue, char* arg);
extern void explodeEnqueue(charStarQueue** queue, char* arg);
extern char* dequeue(charStarQueue** queue);
extern int queueSize(charStarQueue* queue);
#endif
