/**************************************************************************/
/* FILE   **************          Stamp.cc         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Module.hh"
#include "Stamp.hh"

Stamp* Stamp::NIL = 0;

// -----------------------------------------------
// Constructor interface
// -----------------------------------------------
// Stamp::Stamp();
Stamp::Stamp()
{
}

// Stamp::Stamp(int,string);
Stamp::Stamp(string value)
   : mString(value)
{
}

// Stamp::~Stamp();
Stamp::~Stamp() {
   cerr << "Destroy stamp " << this << endl;
}

// const char* Stamp::object() const;
const char* Stamp::object() const {
   return "Stamp";
}

// const char* Stamp::letter() const;
const char* Stamp::letter() const {
   return "C$";
}

// const char* Stamp::value() const;
const char* Stamp::value() const {
   return mString.c_str();
}

int Stamp::i1()  const {
   char tag = '?';
   if ( parent() ) {
      tag = static_cast<char>(offset());
   }
   assert(tag == '?' || (tag >= 'A' && tag <= 'Z'));
   return tag;
}


bool Stamp::isLabelInteger() const {
   return false;
}

// unsigned int Stamp::offset() const;
unsigned int Stamp::offset() const {
   assert(module());
   return module()->offsetOf(this);
}

// Module* Stamp::module();
Module* Stamp::module() {
   assert((!parent()) || dynamic_cast<Module*>(parent()));
   return dynamic_cast<Module*>(parent());
}

// const Module* Stamp::module() const;
const Module* Stamp::module() const {
   assert((!parent()) || dynamic_cast<const Module*>(parent()));
   return dynamic_cast<const Module*>(parent());
}


// void Stamp::parent(Module* M);
void Stamp::module(Module* M) { 
   IFObject::parent(M);
}
