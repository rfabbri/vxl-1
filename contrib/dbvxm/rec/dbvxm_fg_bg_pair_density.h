//:
// \file
// \brief dbvxm recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//


#if !defined(_DBVXM_FG_BG_PAIR_DENSITY_H)
#define _DBVXM_FG_BG_PAIR_DENSITY_H

#include "dbvxm_pair_density.h"

class dbvxm_fg_bg_pair_density : public dbvxm_pair_density
{
public:
  dbvxm_fg_bg_pair_density() : dbvxm_pair_density() {}

  virtual double operator()(const double y0, const double y1);

};

#endif  //_DBVXM_FG_BG_PAIR_DENSITY_H
