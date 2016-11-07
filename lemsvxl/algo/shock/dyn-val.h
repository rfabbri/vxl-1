// DYN-VAL.H
//Dynamic Validation for Shock Detection

#ifndef _DYNVAL_H
#define _DYNVAL_H

#include <vcl_set.h>
#include <vcl_cassert.h>
#include "ishock-common.h"

//using namespace std;

class IVS
{
private:
  VECTOR_TYPE _start;
  VECTOR_TYPE _end;

public:
  VECTOR_TYPE start() { return _start; }
  void setStart(VECTOR_TYPE start) { _start = start; }
  VECTOR_TYPE end() { return _end; }
  void setEnd(VECTOR_TYPE end) { _end = end; }

  IVS () {}
  IVS (VECTOR_TYPE newstart, VECTOR_TYPE newend)  
    { _start=newstart; _end=newend; }
  ~IVS () {}

  inline bool operator==(IVS &ivs) const 
  { return (_start == ivs.start()); }
  inline bool operator!=(IVS &ivs) const 
  { return (_start != ivs.start()); }
  inline bool operator<(IVS &ivs)  const 
  { return (_start < ivs.start()); }
  inline bool operator>(IVS &ivs)  const 
  { return (_start > ivs.start()); }

  inline IVS& operator=(IVS &ivs)
  {
    if (this!=&ivs) {
      _start = ivs.start();
      _end = ivs.end();
    }
    return *this;
  }
};

class IVRegion
{
protected:
  VECTOR_TYPE _region_start;
  VECTOR_TYPE _region_end;
  vcl_set<IVS* > _ivsList;    //list of IVS formed by this BLine

  void addIVS (VECTOR_TYPE newstart, VECTOR_TYPE newend);

public:

  vcl_set<IVS* >* ivsList() { return &_ivsList; }
  typedef vcl_set<IVS* >::iterator ivsListIterator;

  VECTOR_TYPE regionStart() { return _region_start; }
  VECTOR_TYPE regionEnd() { return _region_end; }
  void setRegionStart(VECTOR_TYPE start) { _region_start = start; }
  void setRegionEnd(VECTOR_TYPE end) { _region_end = end; }

  IVRegion (VECTOR_TYPE start, VECTOR_TYPE end)
  {
    _region_start  = start;
    _region_end    = end;

    _ivsList.clear();
  }

  virtual ~IVRegion () {}


  bool isInIVS (VECTOR_TYPE value);
  bool updateEnd (VECTOR_TYPE v, VECTOR_TYPE newend, bool bCCW);
  void merge (IVS& ivs1, IVS& ivs2);
  bool merge (VECTOR_TYPE v1, VECTOR_TYPE v2);
  virtual bool isIVRegionCompleted ();
  void clear ()
  {
    _ivsList.clear();
  }


  //This is the function to call to update and Draw
  void updateIVS (VECTOR_TYPE newstart, VECTOR_TYPE newend);


};

class LineIVRegion : public IVRegion
{
public:
  //[0, length]
  LineIVRegion (double length=0) :
    IVRegion (0, length) { assert(length>0); }
  virtual ~LineIVRegion () {}

  void addLineIVS (double newstart, double newend)
    { addIVS (newstart, newend); }
};

class PointIVRegion : public IVRegion
{
public:
  //[0, 2*M_PI]
  PointIVRegion () :
    IVRegion (0, 2*M_PI) {}
  virtual ~PointIVRegion () {}

  void addPointIVS (VECTOR_TYPE newstart, VECTOR_TYPE newend)
  {
    if (newstart>newend) {
      addIVS (newstart, 2*M_PI);
      addIVS (0, newend);
    }
    else
      addIVS (newstart, newend);
  }
};

class ArcIVRegion : public IVRegion
{
private:
  VECTOR_TYPE _arc_region_start;
  VECTOR_TYPE _arc_region_end;
  bool  bAcross;

public:
  VECTOR_TYPE arcRegionStart() { return _arc_region_start; }
  VECTOR_TYPE arcRegionEnd()  { return _arc_region_end; }
  void setArcRegionStart (VECTOR_TYPE start) {
    _arc_region_start = start;
  }
  void setArcRegionEnd (VECTOR_TYPE end) {
    if (end==0)
      end = 2*M_PI; //extreme special case!!
    _arc_region_end = end;
  }

  //[0, 2*M_PI]
  ArcIVRegion (VECTOR_TYPE start=0, VECTOR_TYPE end=0) :
    IVRegion (0, 2*M_PI)
  {
    _arc_region_start  = start;
    if (end==0)
      end = 2*M_PI; //extreme special case!!
    _arc_region_end  = end;
    bAcross = (start>end) ? true : false;
  }
  virtual ~ArcIVRegion () {}

  void addArcIVS (VECTOR_TYPE newstart, VECTOR_TYPE newend)
  {
    assert (_arc_region_start <= newstart);
    assert (newend <= _arc_region_end);

    if (newstart>newend) {
      addIVS (newstart, 2*M_PI);
      addIVS (0, newend);
    }
    else
      addIVS (newstart, newend);
  }

  virtual bool isIVRegionCompleted ();
  virtual bool isVectorValid (VECTOR_TYPE v);
  virtual bool isVectorFuzzilyValid (VECTOR_TYPE v, double epsilon);
  virtual bool isVectorLessThanStart (VECTOR_TYPE v);
  virtual bool isVectorGreaterThanEnd (VECTOR_TYPE v);
  virtual bool isInIVS (VECTOR_TYPE v);
};

#endif
