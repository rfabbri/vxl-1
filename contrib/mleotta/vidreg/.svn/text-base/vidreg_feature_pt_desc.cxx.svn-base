// This is contrib/mleotta/vidreg/vidreg_feature_pt_desc.cxx

//:
// \file


#include "vidreg_feature_pt_desc.h"
#include <rgrl/rgrl_cast.h>

//: Constructor
vidreg_feature_pt_desc::vidreg_feature_pt_desc(const vnl_vector<double>& loc,
                                               double mag,
                                               const vnl_vector<double>& desc)
  : rgrl_feature_point(loc), magnitude_(mag), descriptor_(desc)
{
}

//: Destructor
vidreg_feature_pt_desc::~vidreg_feature_pt_desc()
{
}


//: used for sorting by decreasing magnitude
bool vidreg_feature_pt_desc::dec_mag_order(const rgrl_feature_sptr& f1,
                                           const rgrl_feature_sptr& f2)
{
  return rgrl_cast<vidreg_feature_pt_desc*>(f1)->magnitude()
       > rgrl_cast<vidreg_feature_pt_desc*>(f2)->magnitude();
}


//: used for sorting by increasing magnitude
bool vidreg_feature_pt_desc::inc_mag_order(const rgrl_feature_sptr& f1,
                                           const rgrl_feature_sptr& f2)
{
  return rgrl_cast<vidreg_feature_pt_desc*>(f1)->magnitude()
       < rgrl_cast<vidreg_feature_pt_desc*>(f2)->magnitude();
}
