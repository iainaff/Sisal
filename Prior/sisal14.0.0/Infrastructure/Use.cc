/**************************************************************************/
/* FILE   **************           Use.cc          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Module.hh"
#include "Use.hh"

Use* Use::NIL = 0;

// -----------------------------------------------
// Constructor interface
// -----------------------------------------------
// Use::Use();
Use::Use()
{
}

Use::Use(string value)
   : mValue(value)
{
}

// Use::~Use();
Use::~Use() {
   cerr << "Destroy use " << this << endl;
}

// const char* Use::object() const;
const char* Use::object() const {
   return "Use";
}

// const char* Use::letter() const;
const char* Use::letter() const {
   return "U";
}

// const char* Use::value() const;
const char* Use::value() const {
   return mValue.c_str();
}

// unsigned int Use::offset() const;
unsigned int Use::offset() const {
   assert(module());
   return module()->offsetOf(this);
}

// Module* Use::module();
Module* Use::module() {
   assert((!parent()) || dynamic_cast<Module*>(parent()));
   return dynamic_cast<Module*>(parent());
}

// const Module* Use::module() const;
const Module* Use::module() const {
   assert((!parent()) || dynamic_cast<const Module*>(parent()));
   return dynamic_cast<const Module*>(parent());
}


// void Use::parent(Module* M);
void Use::module(Module* M) { 
   IFObject::parent(M);
}
