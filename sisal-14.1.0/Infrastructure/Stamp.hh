/**************************************************************************/
/* FILE   **************          Stamp.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 20 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <string>

#ifndef STAMP_HH
#define STAMP_HH

class IFObject;
class Module;

class Stamp : public IFObject {
public:
   static Stamp* NIL;

   // -----------------------------------------------
   // Constructor interface
   // -----------------------------------------------
   Stamp();
   Stamp(string);
   virtual ~Stamp();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;
   virtual bool isLabelInteger() const;
   virtual const char* value() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Module* module();
   const Module* module() const;
   void module(Module*);

protected:
   string mCDollar;
   string mString;
};

#endif
