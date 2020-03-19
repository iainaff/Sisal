/**************************************************************************/
/* FILE   **************      CompoundGraph.hh     ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef COMPOUNDGRAPH_HH
#define COMPOUNDGRAPH_HH

class Compound;
class Graph;

class CompoundGraph: public Graph {
public:
   static CompoundGraph* NIL;

   CompoundGraph(Compound* C=0);
   virtual ~CompoundGraph();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Compound* compound();
   const Compound* compound() const;
   void compound(Compound*);

};

#endif
