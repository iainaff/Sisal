/**************************************************************************/
/* FILE   **************        Function.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 21 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <string>

#ifndef FUNCTION_HH
#define FUNCTION_HH

class FunctionType;
class Graph;
class Module;

class Function : public Graph {
public:
   static Function* NIL;

   Function(string,FunctionType*,Module* M=0);
   virtual ~Function();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;
   virtual const char* value() const;
   virtual bool isValueQuoted() const;

   // -----------------------------------------------
   // Module interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Module* module();
   const Module* module() const;
   void module(Module*);

   // -----------------------------------------------
   // Function specific
   // -----------------------------------------------
   string name() const;
   string canonicalName() const;

protected:
   string mName;
   IF<Info> mType;
};


#endif
