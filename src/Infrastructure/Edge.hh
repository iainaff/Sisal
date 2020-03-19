/**************************************************************************/
/* FILE   **************          Edge.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFObject.hh"

#ifndef EDGE_HH
#define EDGE_HH

class Node;
class Info;
class EdgeCluster;

class Edge : public IFObject {
public:
   static Edge* NIL;

   Edge(Info* T=0);
   virtual ~Edge();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;
   virtual int i2() const;
   virtual int i3() const;
   virtual int i4() const;
   virtual int i5() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Node* node();
   const Node* node() const;
   void node(Node*);

   // -----------------------------------------------
   // Edge stuff
   // -----------------------------------------------
   Info* type();
   const Info* type() const;
   void type(Info*);

   EdgeCluster* edgecluster();
   const EdgeCluster* edgecluster() const;
   void edgecluster(EdgeCluster*);

protected:
   IF< Info > mType;
   IF< EdgeCluster > mEdgeCluster;
};

#endif
