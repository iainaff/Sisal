/**************************************************************************/
/* FILE   **************          Info.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 19 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include <string>

#ifndef INFO_HH
#define INFO_HH

class IFObject;
class Module;

class Info : public IFObject {
public:
   static Info* NIL;

   Info(Module* M=0);
   virtual ~Info();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Module* module();
   const Module* module() const;
   void module(Module* M);

   virtual string canonicalName() const;
protected:
   static string itoa(long);
};

#endif
