// This is mw_dist.h
#ifndef mw_dist_h
#define mw_dist_h
//:
//\file
//\brief Distance-related utilities
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 03/14/2006 10:38:39 AM EST
//

#include <bmcsd/bmcsd_util.h>

class mw_dist {
  public:

  static double 
  dir_hausdorff_distance(vcl_vector<bmcsd_vector_3d> &c1, vcl_vector<bmcsd_vector_3d> &c2);

  // (squared) hausdorff distance
  static double
  hdist(vcl_vector<bmcsd_vector_3d> &c1, vcl_vector<bmcsd_vector_3d> &c2);
};

#endif // mw_dist_h
