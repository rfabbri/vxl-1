// DYN-VAL.CPP
//Dynamic Validation for Shock Detection

#include <extrautils/msgout.h>

#include <vcl_cmath.h>
//#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ishock.h"
#include "dyn-val.h"

//##########################################################
//IVRegionLine
//##########################################################

void IVRegion::addIVS (VECTOR_TYPE newstart, VECTOR_TYPE newend)
{
  assert (_region_start<=newstart && 
        newstart < newend &&
        newend <=_region_end);
  _ivsList.insert( new IVS(newstart, newend));
}

//EndPoint Included
bool IVRegion::isInIVS (VECTOR_TYPE v)
{
  assert (_region_start<=v && v <=_region_end);

  for (ivsListIterator i = _ivsList.begin(); i != _ivsList.end(); i++) {
    if ((*i)->start() <= v && v <= (*i)->end())
      return true;
  }
  return false;
}

//search in the _ivsList, update the IVS which contains v
//1)If CCW: update end, else CW: update start
//  because we know for sure that the other side is fixed.
//2)If nothing to update, then return false
//??EPSILON

bool IVRegion::updateEnd (VECTOR_TYPE v, VECTOR_TYPE newend, bool bCCW)
{
  assert (_region_start<=v && v <=_region_end);
  assert (_region_start<=newend && newend <=_region_end);

  for (ivsListIterator i = _ivsList.begin(); i != _ivsList.end(); i++) {
    if ( (AisLEq((*i)->start() , v) && AisLEq(v , (*i)->end())) !=
        (((*i)->start() <= v) && (v <= (*i)->end())) )
      vcl_cout<< "FUZZY OCCURS (eps="<<A_EPSILON<<") in IVRegion::updateEnd()!!" <<vcl_endl;
    if (AisLEq((*i)->start() , v) && AisLEq(v , (*i)->end())) {
      //update IVS (*i)
      if (bCCW)
        (*i)->setEnd (newend);
      else
        (*i)->setStart (newend);
      return true;
    }
  }

  //If nothing to update, then return false
  return false;
}

//Modify ivs1 and remove ivs2
void IVRegion::merge (IVS& ivs1, IVS& ivs2)
{
  ivs1.setEnd (ivs2.end());
  _ivsList.erase(&ivs2);
}

//Modify the two ivs which contains v1 and v2;
bool IVRegion::merge (VECTOR_TYPE v1, VECTOR_TYPE v2)
{
  IVS *ivs1 = NULL;
  IVS *ivs2 = NULL;
  ivsListIterator i;

  for (i = _ivsList.begin(); i != _ivsList.end(); i++) {
    if ((*i)->start() <= v1 && v1 <= (*i)->end()) {
      ivs1 = (*i);
    }
  }
  //ivs2 = *(++i);
  for (i = _ivsList.begin(); i != _ivsList.end(); i++) {
    if ((*i)->start() <= v2 && v2 <= (*i)->end()) {
      ivs2 = (*i);
    }
  }

  if (ivs1 == NULL || ivs2 == NULL) {
    assert(0);
    return false;
  }

  ivs1->setEnd (ivs2->end());
  _ivsList.erase(ivs2);

  return true;
}

bool IVRegion::isIVRegionCompleted ()
{
  if (_ivsList.size() == 1) {
    ivsListIterator i = _ivsList.begin();
    if ((*i)->start() == _region_start &&
       (*i)->end() == _region_end)
       return true;
  }
  return false;
}

bool ArcIVRegion::isVectorValid (VECTOR_TYPE v)
{
  if (bAcross)
    return (_region_start <= v && v <= _arc_region_end ||
          _arc_region_start <= v && v <= _region_end);
  else
    return (_arc_region_start <= v && v <= _arc_region_end);
}

//EPSILONISSUE 23:
//Make fuzzy condition invalid.
//Default epsilon is 1E-13
bool ArcIVRegion::isVectorFuzzilyValid (VECTOR_TYPE v, double epsilon)
{
  //Fuzzily valid near boundary.
  if (_isEq(v, _arc_region_start, epsilon) ||
     _isEq(v, _arc_region_end, epsilon))
    return false;

  return isVectorValid (v);
}

bool ArcIVRegion::isVectorLessThanStart (VECTOR_TYPE v)
{
  if (bAcross)
    return v<_arc_region_start && v>_arc_region_start-M_PI;
  else
    return (v < _arc_region_start);
}

bool ArcIVRegion::isVectorGreaterThanEnd (VECTOR_TYPE v)
{
  if (bAcross)
    return v>_arc_region_end && v<_arc_region_end+M_PI;
  else
    return (v > _arc_region_end);
}

bool ArcIVRegion::isInIVS (VECTOR_TYPE v)
{
  if (bAcross)
    assert (_region_start <= v && v <= _arc_region_end ||
          _arc_region_start <= v && v <= _region_end);
  else
    assert (_arc_region_start <= v && v <= _arc_region_end);

  return IVRegion::isInIVS (v);
}

bool ArcIVRegion::isIVRegionCompleted ()
{
  if (bAcross) {
    if (_ivsList.size() == 2) {
      ivsListIterator i = _ivsList.begin();
      ivsListIterator ii = i; ii++;
      if ((*i)->start() == _arc_region_start &&
         (*i)->end() == _region_end &&      //_region_end is 2Pi
         (*ii)->start() == _region_start &&    //_region_start is 0
         (*ii)->end() == _arc_region_end)
         return true;
    }
    return false;
  }
  else {
    if (_ivsList.size() == 1) {
      ivsListIterator i = _ivsList.begin();
      if ((*i)->start() == _arc_region_start &&
         (*i)->end() == _arc_region_end)
         return true;
    }
    return false;
  }
}
