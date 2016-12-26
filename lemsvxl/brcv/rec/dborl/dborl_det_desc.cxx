// This is file seg/dbsks/dborl_det_desc.cxx

//:
// \file

#include "dborl_det_desc.h"



//: Operator to compare two detection descriptions by comparing their confidence
bool dborl_decreasing_confidence(const dborl_det_desc_sptr& a, const dborl_det_desc_sptr& b)
{
  return a->confidence() > b->confidence();
}



//: Return true if two detection are in increasing confidence level
bool dborl_increasing_confidence(const dborl_det_desc_sptr& a, const dborl_det_desc_sptr& b)
{
  return a->confidence() < b->confidence();
}


