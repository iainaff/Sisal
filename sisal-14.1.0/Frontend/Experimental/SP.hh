/**************************************************************************/
/* FILE   **************           SP.hh           ************************/
/**************************************************************************/
/* Author: Patrick Miller January 23 2001                                 */
/* Update: Made code more compliant with g++, but had to expose           */
/* more data.                                                             */
/**************************************************************************/
/* Derived from code by: Yonat Sharon                                                   */
/* URL: http://ootips.org/yonat/4dev/counted_ptr.h                        */
/* Copyright (C) None                                                     */
/**************************************************************************/
/* Yonat says: Here is some stuff I made for myself. Feel free to use it. */
/**************************************************************************/
#include <assert.h>

#ifndef SP_HH
#define SP_HH

template <class X>
class SP {
public:
   explicit SP(X* p = 0,bool locked=false)
      : itsCounter(0), mLocked(locked) {
      if (p) {
         itsCounter = new counter(p); // allocate a new counter
         p->self(*this);
      }
   }
   ~SP() {release();}
   SP(const SP& r) throw() {acquire(r.itsCounter);}
   SP& operator=(const SP& r) {
      if (this != &r) {
         assert(!mLocked);
         release();
         acquire(r.itsCounter);
      }
      return *this;
   }

   typedef struct counter {
      counter(X* p = 0, unsigned c = 1) : ptr(p), count(c) {}
      X*          ptr;
      unsigned    count;
   } counter_t;
   counter_t* itsCounter;

   template <class Y> SP(const SP<Y>& r) throw() {
      X* p = r.get();
      acquire((counter_t*)r.itsCounter);
   }
   template <class Y> SP& operator=(const SP<Y>& r) {
      if ( this != &r ) {
         assert(!mLocked);
         X* p = r.get();
         release();
         acquire((counter_t*)r.itsCounter);
      }
   }
   X& operator*()  const throw()   {return *itsCounter->ptr;}
   X* operator->() const throw()   {return itsCounter->ptr;}
   X* get()        const throw()   {return itsCounter ? itsCounter->ptr : 0;}
   bool unique()   const throw()
   {return (itsCounter ? itsCounter->count == 1 : true);}


   operator bool() const { return get() != 0; }
   bool operator ==(const SP& other) const {
      return get() == other.get();
   }

   void acquire(counter* c) throw()
   { // increment the count
      itsCounter = c;
      if (c) ++c->count;
   }

   void release()
   { // decrement the count, delete if it is 0
      if (itsCounter) {
         if (--itsCounter->count == 0) {
            delete itsCounter->ptr;
            delete itsCounter;
         }
         itsCounter = 0;
      }
   }
private:
   bool mLocked;
};
#endif
