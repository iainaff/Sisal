#ifndef PROCESS_IMPLEMENTATION_H
#define PROCESS_IMPLEMENTATION_H

/**************************************************************************/
/* FILE   **************  process-implementation.h ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/* SGIModel */
#define GetProcId  *((int*)((PRDA->usr_prda.fill)))
#define GETPROCID(x) x = GetProcId
#endif
