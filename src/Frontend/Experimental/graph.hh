/**************************************************************************/
/* FILE   **************          graph.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef GRAPH_HH
#define GRAPH_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

// -----------------------------------------------
// A graph is like a node, but inputs to the
// graph are the results (because they flow
// into to the graph).  Initial conditions are
// the outputs (because they flow out of the
// graph).  Graphs have their own implementation
// set.
// -----------------------------------------------

class graph : public node {
public:
   graph();
   graph(unsigned int);
   graph(const char*);

   virtual bool valid() const;
   virtual void writeSelf(ostream&) const;
   int offset(const node*) const;

   virtual map<const char*,NodeImplementation*,node::ltCharP>* nameTable() const;
   virtual map<unsigned int,NodeImplementation*>* opcodeTable() const;
   static int registration(NodeImplementation*);

   void addNode(node*);
   void typeBinding();

   // -----------------------------------------------
   // Traversal
   // -----------------------------------------------
   typedef vector<node*>::iterator iterator;
   typedef vector<node*>::const_iterator const_iterator;
   iterator begin() { return mNodes.begin(); }
   const_iterator begin() const { return mNodes.begin(); }
   iterator end() { return mNodes.end(); }
   const_iterator end() const { return mNodes.end(); }

protected:
   virtual char letter() const { return 'G'; }
   virtual int i1() const;
   virtual int i2() const;

   vector< node* > mNodes;
};

#endif
