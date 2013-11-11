#ifndef poly_utils_h_
#define poly_utils_h_

#include <vcl_vector.h>

#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>


class poly_utils 
{ 
public:
  static vsol_polygon_3d_sptr move_points_to_plane(vsol_polygon_3d_sptr polygon);
  static vsol_polygon_3d_sptr move_points_to_plane(vcl_vector<vsol_point_3d_sptr> points);
};

#endif
