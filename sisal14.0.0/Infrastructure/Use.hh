/**************************************************************************/
/* FILE   **************           Use.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <string>

#ifndef USE_HH
#define USE_HH

class IFObject;
class Module;

class Use : public IFObject {
public:
   static Use* NIL;

   // -----------------------------------------------
   // Constructor interface
   // -----------------------------------------------
   Use();
   Use(string);
   virtual ~Use();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual const char* value() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Module* module();
   const Module* module() const;
   void module(Module*);

protected:
   string mValue;
};


#endif
