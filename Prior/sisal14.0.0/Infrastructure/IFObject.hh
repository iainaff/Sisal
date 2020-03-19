/**************************************************************************/
/* FILE   **************        IFObject.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 19 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#ifndef IFOBJECT_HH
#define IFOBJECT_HH

class Pragma;
class IFObject;

template<class T>
class IF {
private:
public:
   IF();
   IF(T*);
   IF(int);
   IF(const IF<T>&);
   ~IF();

   bool operator==(const T*) const;
   bool operator==(const IF<T>&) const;

   T* operator ->();
   const T* operator ->() const;

   T& operator *();
   const T& operator *() const;

   IF& operator =(T*);
   const IF& operator =(const T*);

   T* pointer();
   const T* pointer() const;

   T* release();

   bool owned() const;
   void owned(bool);

   operator bool() const;

   void assign(T*);

protected:
   T* mPointer;
   bool mOwned;
};

template<class T>
class IFauto : public IF<T> {
public:
   IFauto();
   IFauto(int);
   IFauto(const IF<T>&);
   IFauto(const IFauto<T>&);
};

class IFObject {
protected:

   static void indent(ostream& os,unsigned int level);
   typedef enum { BLANK = -1, UNDEFINED = -2 } LabelType;
   void LabelEntry(ostream&, int, const char*,bool asInteger=true) const;

public:
   static IFObject* NIL;

   IFObject(IFObject* parent=0);
   virtual ~IFObject();

   virtual const char* object() const;
   virtual const char* letter() const;
   virtual void write(ostream&) const;
   virtual void dump(ostream&,unsigned level=0) const;

   virtual int i1() const;
   virtual bool isLabelInteger() const;
   virtual int i2() const;
   virtual int i3() const;
   virtual int i4() const;
   virtual int i5() const;
   virtual int i6() const;
   virtual const char* value() const;
   virtual bool isValueQuoted() const;

   virtual Pragma& pragma(string);
   virtual const Pragma& pragma(string) const;

   virtual void renumbered();
   virtual unsigned int offset() const;

   void interested(IFObject**);
   void disinterested(IFObject**);

protected:
   vector<void*> registered;

   virtual IFObject* parent();
   virtual const IFObject* parent() const;
   virtual void parent(IFObject*);

   virtual void endDump(ostream&,unsigned level=0) const;

   IF<IFObject> mParent;
};

template<class T>
static void dumpObjects(const T& begin, const T& end, ostream& os, unsigned int level) {
   for(T i = begin; i != end; ++i) {
      if ( *i ) (*i)->dump(os,level);
   }
}


#define CHILD_INTERFACE(Parent,parent,Child,child) \
   Child* child(unsigned int); \
   const Child* child(unsigned int) const; \
   static void renumber##Child(IF<Child>&); \
   void set(Child*,unsigned int position=0); \
   void insert(Child*,unsigned int position=0); \
   void erase(Child*,unsigned int position=0); \
   void replace(Child*,unsigned int position=0); \
   Child* release(Child*,unsigned int position=0); \
   unsigned int offsetOf(const Child*) const

#define CHILD_IMPLEMENTATION(Parent,parent,Child,child,VV) \
Child* Parent::child(unsigned int offset) { \
   assert(offset > 0); \
   while ( m##Child.size() < offset ) m##Child.push_back(0); \
   return m##Child[offset-1].pointer(); \
} \
const Child* Parent::child(unsigned int offset) const { \
   assert(offset > 0); \
   return (offset <= m##Child.size())?(m##Child[offset-1].pointer()):((Child*)0); \
} \
void Parent::renumber##Child(IF<Child>& kid) { \
  if ( kid ) kid->renumbered(); \
} \
void Parent::set(Child* kid,unsigned int offset) { \
   if ( kid && kid->parent() ) {\
      throw "Already has parent"; \
   }\
   if ( offset == 0 ) offset = m##Child.size()+1; \
   while( m##Child.size() < offset ) {\
      m##Child.push_back(0); \
   }\
   m##Child[offset-1].assign(kid); \
   if ( kid ) kid->parent(this); \
} \
void Parent::insert(Child* kid,unsigned int offset) { \
   if ( kid && kid->parent() ) {\
      throw "Already has parent"; \
   }\
   if ( offset == 0 ) offset = m##Child.size()+1; \
   while( m##Child.size() < offset-1 ) m##Child.push_back(0); \
   VV< Child > kk; \
   kk.assign(kid); \
   m##Child.insert(m##Child.begin()+(offset-1),kk); \
   for_each(m##Child.begin()+(offset-1),m##Child.end(), renumber##Child); \
   if ( kid ) kid->parent(this); \
} \
void Parent::erase(Child*,unsigned int offset) { \
   assert(offset > 0); \
   if ( offset <= m##Child.size() ) { \
      for_each(m##Child.erase(m##Child.begin()+(offset-1)), \
               m##Child.end(), renumber##Child); \
   } \
} \
void Parent::replace(Child* kid,unsigned int offset) { \
   if ( kid && kid->parent() ) {\
      throw "Already has parent"; \
   }\
   if ( offset == 0 ) offset = m##Child.size()+1; \
   while( m##Child.size() < offset ) m##Child.push_back(0); \
   m##Child[offset-1].assign(kid); \
   if ( kid ) kid->parent(this); \
} \
Child* Parent::release(Child*,unsigned int offset) { \
   if ( offset == 0 ) offset = m##Child.size()+1; \
   while( m##Child.size() < offset ) m##Child.push_back(0); \
   Child* kid = m##Child[offset-1].release(); \
   if ( kid ) kid->parent(0); \
   return kid; \
} \
unsigned int Parent::offsetOf(const Child* kid) const { \
   assert(kid); \
   vector< VV<Child> >::const_iterator position = find(m##Child.begin(), m##Child.end(), kid); \
   if ( position == m##Child.end() ) return 0; \
   return position - m##Child.begin() + 1; \
}

#define INHERITED_CHILD(Klass,Parent,parent,Child,child) \
Child* Klass::child(unsigned int offset) { return Parent::child(offset); } \
const Child* Klass::child(unsigned int offset) const { return Parent::child(offset); } \
void Klass::set(Child* kid,unsigned int offset) { Parent::set(kid,offset); } \
void Klass::insert(Child* kid,unsigned int offset) { Parent::insert(kid,offset); } \
void Klass::erase(Child* dummy,unsigned int offset) { Parent::erase(dummy,offset); } \
void Klass::replace(Child* kid,unsigned int offset) { Parent::replace(kid,offset); } \
unsigned int Klass::offsetOf(const Child* kid) const { return Parent::offsetOf(kid); }


ostream& operator<<(ostream& os, const IFObject& x);

template<class T>
IF<T>::IF()
   : mPointer(0), mOwned(false)
{
}

template<class T>
IF<T>::IF(int x) 
   : mPointer(0), mOwned(false) 
{
   assert(x == 0);
}

template<class T>
IF<T>::IF(T* x) 
   : mPointer(0), mOwned(false) 
{
   assign(x);
}

template<class T>
IF<T>::IF(const IF<T>& x) 
   : mPointer(0), mOwned(false)
{
   assign(x.mPointer);
   mOwned = x.mOwned;
   if ( x.mOwned ) const_cast<IF<T>&>(x).mOwned = false;
}


template<class T>
IF<T>::~IF() { 
   T* p = mPointer;
   assign(0);
   assert(mPointer == 0);
   if ( p && mOwned ) { 
      delete p;
   }
}


template<class T>
bool IF<T>::operator==(const T* x) const {
   return x == mPointer;
}

template<class T>
bool IF<T>::operator==(const IF<T>& x) const {
   return x.mPointer == mPointer;
}

template<class T>
T* IF<T>::operator ->() { return mPointer; }

template<class T>
const T* IF<T>::operator ->() const { return mPointer; }


template<class T>
T& IF<T>::operator *() { return *mPointer; }

template<class T>
const T& IF<T>::operator *() const { return *mPointer; }


template<class T>
IF<T>& IF<T>::operator =(T* p) { assign(p); return *this; }

template<class T>
const IF<T>& IF<T>::operator =(const T* p) {  assign(p); return *this;}

template<class T>
T* IF<T>::pointer() { return mPointer; }

template<class T>
const T* IF<T>::pointer() const { return mPointer; }

template<class T>
T* IF<T>::release() {
   bool owned = mOwned;
   mOwned = false;
   T* x = mPointer;
   assign(0);
   mOwned = owned;
   return x;
}

template<class T>
bool IF<T>::owned() const { return mOwned; }

template<class T>
void IF<T>::owned(bool flag) { mOwned = flag; }

template<class T>
void IF<T>::assign(T* x) {
   if ( mPointer ) mPointer->disinterested(reinterpret_cast<IFObject**>(&mPointer));
   mPointer = x;
   if ( mPointer ) mPointer->interested(reinterpret_cast<IFObject**>(&mPointer));
}

template<class T>
IF<T>::operator bool() const {
   return mPointer != 0;
}

template<class T>
IFauto<T>::IFauto()
{
   mOwned = true;
}
template<class T>
IFauto<T>::IFauto(int x)
   : IF<T>(x)
{
   assert(x == 0);
   assert(mPointer == 0);
   mOwned = true;
}
template<class T>
IFauto<T>::IFauto(const IF<T>& x)
{
   assign(x.mPointer);
   mOwned = x.mOwned;
   if ( x.mOwned ) const_cast<IF<T>&>(x).mOwned = false;
}
template<class T>
IFauto<T>::IFauto(const IFauto<T>& x)
{
   assign(x.mPointer);
   mOwned = x.mOwned;
   if ( x.mOwned ) const_cast<IFauto<T>&>(x).mOwned = false;
}

#endif

