#ifndef NODE_HH
#define NODE_HH

#include "SP.hh"
#include <vector>

#include "IFObject.hh"
namespace sisalc {

   // -----------------------------------------------
   // forward dependencies
   // -----------------------------------------------
   class graph;
   class edge;

   class node : public IFObject {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      node();
      static SP<node> ctor() {return SP<node>(new node);}

      node(unsigned);
      static SP<node> ctor(unsigned opcode) {return SP<node>(new node(opcode));}

      node(const char*);
      static SP<node> ctor(const char* name) {return SP<node>(new node(name));}

      ~node();

      // -----------------------------------------------
      // Reference counted registration
      // -----------------------------------------------
   protected:
      SP<node> mSelf;
   public:
      void self(SP<node>);
      SP<node> self() const;
      static SP<node> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      virtual void writeSelf(ostream&) const;
      virtual bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
      //int offset(?) const;
      virtual unsigned int label() const;
   protected:
      virtual char letter() const { return 'N'; }
      virtual int i1() const;
      virtual int i2() const;
      
      // -----------------------------------------------
      // Interconnect
      // -----------------------------------------------
   public:
      void setParent(SP<graph>);
      SP<graph> parent() { return mParent; }

      void attachInput(SP<edge>,int);
      void attachOutput(SP<edge>,int);

   protected:

      typedef struct { const char* name; unsigned int opCode;} operation_t;

      const char* name() const;
      unsigned int lookup(const char*);

      virtual const operation_t* getTable() const;

      unsigned mOpCode;
      SP<graph> mParent;
      vector< SP<edge> > mInputs;
      vector< vector< SP<edge> > > mOutputs;
   };

   typedef SP<node> NODE;
}
#endif
