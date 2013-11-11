// This is dbdet_med.h
#ifndef dbdet_med_h
#define dbdet_med_h
//:
//\file
//\brief multiscale edge detector using geometric evidence
//\author Amir Tamrakar
//\date 07/10/06
//

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <vnl/vnl_math.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include "dbdet_sel1.h"

//: Multiscale edge detector using geometric evidence
class dbdet_med : public vbl_ref_count
{
public:

  //: constructor
  dbdet_med(int nrad=2, double dtheta=0.1, double dpos=0.1);
  ~dbdet_med();

  //utility functions
  void build_curvelets();

  //debug functions
  void report_stats();

protected:

  unsigned nrad_; ///< radius of the grouping neighborhood around each edgel

  double dtheta_; ///< assumed uncertainty in orientation
  double dpos_;   ///< assumed uncertainty in position

  // for debugging only

};


#endif // dbdet_med_h
