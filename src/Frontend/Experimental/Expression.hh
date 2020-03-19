/**************************************************************************/
/* FILE   **************       Expression.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef EXPRESSION_HH
#define EXPRESSION_HH

#include "semanticBase.hh"
using sisalc::semanticBase;

#include "IFCore.hh"
using sisalc::edge;
using sisalc::node;

class Expression: public semanticBase {
public:
   Expression() {}
   typedef vector<edge*>::iterator outputIterator;
   typedef vector<node*>::iterator nodeIterator;

   void mergeIn(Expression* E) {
      if ( E ) {
         copy(E->mOutputs.begin(),E->mOutputs.end(),
              inserter(mOutputs,mOutputs.end()));
         copy(E->mUnresolved.begin(),E->mUnresolved.end(),
              inserter(mUnresolved,mUnresolved.end()));
         copy(E->mNodes.begin(),E->mNodes.end(),
              inserter(mNodes,mNodes.end()));
      }
   }

   typedef vector<edge*>::iterator edge_iterator;
   typedef vector<edge*>::const_iterator const_edge_iterator;
   typedef vector<node*>::iterator node_iterator;
   typedef vector<node*>::const_iterator const_node_iterator;

   edge_iterator outputBegin() { return mOutputs.begin(); }
   const_edge_iterator outputBegin() const { return mOutputs.begin(); }
   edge_iterator outputEnd() { return mOutputs.end(); }
   const_edge_iterator outputEnd() const { return mOutputs.end(); }

   edge_iterator unresolvedBegin() { return mUnresolved.begin(); }
   const_edge_iterator unresolvedBegin() const { return mUnresolved.begin(); }
   edge_iterator unresolvedEnd() { return mUnresolved.end(); }
   const_edge_iterator unresolvedEnd() const { return mUnresolved.end(); }

   node_iterator nodeBegin() { return mNodes.begin(); }
   const_node_iterator nodeBegin() const { return mNodes.begin(); }
   node_iterator nodeEnd() { return mNodes.end(); }
   const_node_iterator nodeEnd() const { return mNodes.end(); }

   void addOutput(edge* E) { mOutputs.push_back(E); }
   void addUnresolved(edge* E) { mUnresolved.push_back(E); }
   void addNode(node* N) { mNodes.push_back(N); }

   //vector<edge*>& outputs() { return mOutputs; }
   //vector<edge*>& unresolved() { return mUnresolved; }
   //vector<node*>& nodes() { return mNodes; }

protected:
   vector<edge*> mOutputs;
   vector<edge*> mUnresolved;
   vector<node*> mNodes;
};

#endif
