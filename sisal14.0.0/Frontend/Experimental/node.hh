/**************************************************************************/
/* FILE   **************          node.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef NODE_HH
#define NODE_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

// -----------------------------------------------
// A node is a collection of input edges (single
// input) and output edges (many).  All interesting
// work is done by a "implementation" which is
// a helper class that defines behaviors for the
// node.
// -----------------------------------------------

class node : public IFObject {
public:
   node();
   node(unsigned int);
   node(const char*,string fileName="",int sourceLine=0);
   node(NodeImplementation*);

   struct ltCharP {bool operator()(const char* s1, const char* s2) const {return strcmp(s1, s2) < 0;}};
   virtual map<const char*,NodeImplementation*,ltCharP>* nameTable() const;
   virtual map<unsigned int,NodeImplementation*>* opcodeTable() const;
  
   virtual bool valid() const;
   virtual void writeSelf(ostream& os) const;
   virtual int label() const;

   static int registration(NodeImplementation*);
   void attachInput(edge*,unsigned int);
   void attachOutput(edge*,unsigned int);
   void setGraph(graph*);

   graph* parent() { return mParent; }
   const graph* parent() const { return mParent; }

   Function* function();
   const Function* function() const;

   void typeBinding();

   int inArity() const { return mInputs.size(); }
   int outArity() const { return mOutputs.size(); }

   edge* input1() { assert(inArity() >= 1); return mInputs[0]; }
   edge* input2() { assert(inArity() >= 2); return mInputs[1]; }
   vector<edge*> output1() { assert(outArity() >= 1); return mOutputs[0]; }
   vector<const info*> signature() const;
protected:
   virtual char letter() const { return 'N'; }
   virtual int i1() const;
   virtual int i2() const;

   NodeImplementation* mImplementation;
   graph* mParent;
   vector< edge* > mInputs;
   vector< vector< edge* > > mOutputs;
};

#endif
