/**************************************************************************/
/* FILE   **************        Function.cc        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include <iostream>
#include "assert.h"

#include "IFObject.hh"
#include "Node.hh"
#include "Graph.hh"
#include "Module.hh"
#include "Function.hh"
#include "Info.hh"
#include "FunctionType.hh"

Function* Function::NIL = 0;

// -----------------------------------------------
// Constructor interface
// -----------------------------------------------

// Function::Function(Module* M=0);
Function::Function(string name,FunctionType* F,Module* M)
   : mName(name), mType(F)
{
   parent(M);
}

string Function::name() const {
   return mName;
}

string Function::canonicalName() const {
   return mName;
}

// Function::~Function();
Function::~Function() {
   cerr << "Destroy Function " << this << endl;
}

// const char* Function::object() const;
const char* Function::object() const {
   return "Function";
}

// const char* Function::letter() const;
const char* Function::letter() const {
   return "X";
}

// int Function::i1() const;
int Function::i1() const {
   if ( mType ) {
      return mType->i1();
   } else {
      return IFObject::UNDEFINED;
   }
}

const char* Function::value() const {
   return mName.c_str();
}

// unsigned int Function::offset() const;
unsigned int Function::offset() const {
   assert(module());
   return module()->offsetOf(this);
}
 
// Module* Function::module();
Module* Function::module() {
   assert((!parent()) || dynamic_cast<Module*>(parent()));
   return dynamic_cast<Module*>(parent());
}

// const Module* Function::module() const;
const Module* Function::module() const {
   assert((!parent()) || dynamic_cast<const Module*>(parent()));
   return dynamic_cast<const Module*>(parent());
}

// void Function::module(Module* M);
void Function::module(Module* M) { 
   parent(M);
}

bool Function::isValueQuoted() const {
   return true;
}
