/**************************************************************************/
/* FILE   **************         Pragma.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef PRAGMA_HH
#define PRAGMA_HH

class Pragma {
public:
   Pragma();
   Pragma(long);
   Pragma(double);
   Pragma(string);
   ~Pragma();

   operator bool();
   operator int();
   operator long();
   operator double();
   operator string();
   operator const char*();
};

#endif
