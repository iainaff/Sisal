#include "assert.h"

#include <stdio.h>
#include "IFObject.hh"
#include "Module.hh"
#include "Info.hh"

Info* Info::NIL = 0;

// -----------------------------------------------
// Constructor interface
// -----------------------------------------------

// Info::Info(Module* M=0);
Info::Info(Module* M)
   : IFObject(M)
{
}

// Info::~Info();
Info::~Info() {
   cerr << "Destroying info" << endl;
}

// const char* Info::object() const;
const char* Info::object() const {
   return "Info";
}

// const char* Info::letter() const;
const char* Info::letter() const {
   return "T";
}

// int Info::i1() const;
int Info::i1() const {
   if ( mParent ) {
      return offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

// unsigned int Info::offset() const;
unsigned int Info::offset() const {
   assert(module());
   return module()->offsetOf(this);
}

// Module* Info::module();
Module* Info::module() {
   assert((!parent()) || dynamic_cast<Module*>(parent()));
   return dynamic_cast<Module*>(parent());
}

// const Module* Info::module() const;
const Module* Info::module() const {
   assert((!parent()) || dynamic_cast<const Module*>(parent()));
   return dynamic_cast<const Module*>(parent());
}

// void Info::module(Module* M);
void Info::module(Module* M) { 
   parent(M);
}


string Info::canonicalName() const {
   return "???";
}

string Info::itoa(long x) {
   char buf[100]; // Better be long enough
   sprintf(buf,"%ld",x);
   return buf;
}
