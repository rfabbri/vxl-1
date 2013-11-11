#include <vcl_iostream.h>
#include <vcl_fstream.h>
//#include <stl_config.h>
//#include <string>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>


#ifndef __TBS_NEW_DPMATCH_DEF__
#define __TBS_NEW_DPMATCH_DEF__

#include "DPMatch.h"
#include "utils.h"

template <class curveType,class floatType>
class NewDPMatch: public DPMatch<curveType,floatType>{
 public:
  NewDPMatch();
  NewDPMatch(curveType &c1,curveType &c2);
  NewDPMatch(curveType &c1,curveType &c2,floatType R1,floatType R2, vector<floatType> lambda, int numLenElems);
  ~NewDPMatch(){};

 protected:
  //Data
  int _numLenElems;
  floatType _R1;
  floatType _R2;
  vector<floatType> _lambda;

  //Functions
  floatType computeIntervalCost(int i, int ip, int j, int jp);
//  floatType computeIntervalCost1(int i, int ip, int j, int jp);
};

#endif
