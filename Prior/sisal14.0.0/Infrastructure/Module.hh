/**************************************************************************/
/* FILE   **************         Module.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 19 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <vector>
#include <string>

#ifndef MODULE_HH
#define MODULE_HH

class IFObject;
class Info;
class Stamp;
class Function;
class Use;

class Module : public IFObject {
public:
   static Module* NIL;

   Module();
   Module(string);
   virtual ~Module();

   virtual const char* object() const;
   virtual const char* letter() const;
   virtual const char* value() const;

   CHILD_INTERFACE(Module,module,Info,info);
   CHILD_INTERFACE(Module,module,Stamp,stamp);
   CHILD_INTERFACE(Module,module,Function,function);
   CHILD_INTERFACE(Module,module,Use,use);


protected:
   virtual void endDump(ostream&,unsigned level=0) const;

   string mName;
   vector< IFauto<Info> > mInfo;
   vector< IFauto<Stamp> > mStamp;
   vector< IFauto<Function> > mFunction;
   vector< IFauto<Use> > mUse;
};


#endif
  
  
