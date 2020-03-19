/**************************************************************************/
/* FILE   **************          edge.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef EDGE_HH
#define EDGE_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

class edge : public IFObject {
public:
   // -----------------------------------------------
   // Constructors
   // -----------------------------------------------
   edge();
   edge(const info*);

   // -----------------------------------------------
   // Output
   // -----------------------------------------------
   virtual bool valid() const;

   // -----------------------------------------------
   // IF Labeling
   // -----------------------------------------------
protected:
   virtual char letter() const { return 'E'; }
   virtual int i1() const;
   virtual int i2() const;
   virtual int i3() const;
   virtual int i4() const;
   virtual int i5() const;

   // -----------------------------------------------
   // Interconnect
   // -----------------------------------------------
public:
   void setDestination(node*,int);
   void setSource(node*,int);
   void setType(const info*);

   const info* type() const { return mType; }

protected:
   node* mSource;
   int mSourcePort;
   node* mDestination;
   int mDestinationPort;
   const info* mType;

};

#endif
