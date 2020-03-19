/**************************************************************************/
/* FILE   **************       EdgeCluster.hh      ************************/
/************************************************************************ **/
/* Author: Patrick Miller July  1 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFObject.hh"

#ifndef EDGECLUSTER_HH
#define EDGECLUSTER_HH

class Node;
class Edge;

class EdgeCluster : public IFObject {
public:
   static EdgeCluster* NIL;

   EdgeCluster(Node* N=0);
   virtual ~EdgeCluster();

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Node* node();
   const Node* node() const;
   void node(Node*);

   CHILD_INTERFACE(EdgeCluster,edgecluster,Edge,edge);

protected:
   vector< IF<Edge> > mEdge;
};

#endif
