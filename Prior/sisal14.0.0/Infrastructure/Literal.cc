/**************************************************************************/
/* FILE   **************         Literal.cc        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Edge.hh"
#include "Literal.hh"
#include "Node.hh"

Literal* Literal::NIL = 0;

Literal::Literal(Info* type,string rvalue)
   : Edge(type), mRValue(rvalue)
{
}

Literal::~Literal() {
   cerr << "Destroy literal " << this << endl;
}

const char* Literal::object() const {
   return "Literal";
}

const char* Literal::letter() const {
   return "L";
}
const char* Literal::value() const {
   return mRValue.c_str();
}
void Literal::value(string s) {
   mRValue = s;
}
bool Literal::isValueQuoted() const {
   return true;
}

int Literal::i1() const {
   return IFObject::BLANK;
}
int Literal::i2() const {
   return IFObject::BLANK;
}
