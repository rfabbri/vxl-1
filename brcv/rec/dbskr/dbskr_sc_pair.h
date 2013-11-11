// This is brcv/rec/dbskr/dbskr_sc_pair.h

#ifndef dbskr_sc_pair_h_
#define dbskr_sc_pair_h_


//:
// \file
// \brief A class for a shock curve represented in pair - coarse and dense.
//
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date   Oct 31, 2009
//
// \verbatim
// Modifications
//   Nhon Trinh - Oct 31, 2009 -  Initial version. This class was originally 
//                                in dbskr/dbskr_tree.h and was written by Ozge Ozcanli.
//   
// \endverbatim
//
//-------------------------------------------------------------------------




#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <dbskr/dbskr_scurve_sptr.h>


//: scurve_sample_ds_ parameter in dbskr_sc_pair controls the sampling rate of the coarse curve
//  the dense curve is the interpolated version with interpolate_ds_ 
//  if localized_matching option is on in dbskr_sc_pair_edit, elastic curve alignment is found via coarse curves for a given shock branch pair
//  then a new cost is computed using dense curve via the class dbskr_localize_match and the map of the coarse matching
//  this is a way of speed up and ends up to be equivalent to smoothing the curves
class dbskr_sc_pair : public vbl_ref_count 
{
public:
  dbskr_sc_pair() : coarse(0), dense(0) {}
  virtual ~dbskr_sc_pair() { c_d_map.clear(); }
  dbskr_scurve_sptr coarse;

  //: dense version linearly interpolates new samples on both shock branch
  //  and plus and minus boundary
  dbskr_scurve_sptr dense;
  //: correspondence map from coarse shock curve to dense 
  vcl_vector<int> c_d_map;
};


#endif // dbskr_sc_pair_h_
