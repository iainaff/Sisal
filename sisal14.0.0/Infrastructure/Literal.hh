/**************************************************************************/
/* FILE   **************         Literal.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <string>

#ifndef LITERAL_HH
#define LITERAL_HH


class Edge;
class Node;

class Literal : public Edge {
public:
   static Literal* NIL;

   Literal(Info*,string rvalue);
   virtual ~Literal();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual int i1() const;
   virtual int i2() const;
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual const char* value() const;
   virtual bool isValueQuoted() const;

   // -----------------------------------------------
   // Literal stuff
   // -----------------------------------------------
   virtual void value(string);

protected:
   string mRValue;
};

#endif
