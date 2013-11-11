#ifndef _dbdp_cost_h
#define _dbdp_cost_h
//---------------------------------------------------------------------
// This is basic/dbdp/dbdp_cost.h
//:
// \file
// \brief Dynamic programming interval cost
//        This is an abstract class. The user should inherit from this
//        class and handle interval cost computation.
//
// \author H. Can Aras (can@lems.brown.edu)
// \date 2007-01-15
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//----------------------------------------------------------------------

#include "vcl_cmath.h"

class dbdp_cost
{
public:
  dbdp_cost() {};
  virtual ~dbdp_cost() {};
  virtual double compute_interval_cost(int i, int ip, int j, int jp) = 0;
};

#endif

