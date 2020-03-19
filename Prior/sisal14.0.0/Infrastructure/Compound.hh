/**************************************************************************/
/* FILE   **************        Compound.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#ifndef COMPOUND_HH
#define COMPOUND_HH

class IFObject;
class Edge;
class Node;
class Graph;
class CompoundGraph;

class Compound: public Node {
public:
   static Compound* NIL;

   Compound(Graph* G=0);
   Compound(int, Graph* G=0);
   Compound(string, Graph* G=0);
   virtual ~Compound();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;

   CHILD_INTERFACE(Compound,compound,CompoundGraph,compoundGraph);
   CHILD_INTERFACE(Node,node,Edge,edge);

protected:
   virtual void endDump(ostream&,unsigned level=0) const;

   vector< IFauto<CompoundGraph> > mCompoundGraph;
};

#endif
