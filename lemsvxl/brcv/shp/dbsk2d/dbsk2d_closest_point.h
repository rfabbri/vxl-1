// This is basic/dbgl/algo/dbsk2d_closest_point.h

#ifndef dbsk2d_closest_point_h_
#define dbsk2d_closest_point_h_

//:
// \file
// \brief Algorithms to compute closest point to dbsk2d objects
// (boundary, shock, visual fragments, etc)
//
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 07/27/05
//
// \verbatim  
//  Modifications:
//    Nhon Trinh   7/27/2005    Initial version
//  
// \endverbatim


#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>
#include <vgl/vgl_point_2d.h>

class dbsk2d_closest_point
{
protected:
public:
  dbsk2d_closest_point(){};
  ~dbsk2d_closest_point(){};

  //: Return parameter (arclength) of the point on bnd_edge that is 
  // closest to point `pt'
  // if `direction' = -1, s_start and s_end are measured from end vertex
  // `arclength' returns the arclength of the point on
  // edge that is closest to `query_pt'
  // Returned arclength is always measured from start vertex
  // need rewrite
  static double point_to_bnd_edge(const vgl_point_2d<double >& query_pt,
    const dbsk2d_bnd_edge_sptr& edge,
    int direction = 1,
    double s_start = 0,
    double s_end = 1e10
    );

  


  //: return the arclength parameter of the point on `contour'
  // that is closest to the query point between s_start and s_end (inclusive)
  static double point_to_bnd_contour(const vgl_point_2d<double>& query_pt,
    const dbsk2d_bnd_contour_sptr& contour,
    double s_start=0, double s_end=1e10);

  //: Compute closest point between two line dbsk2d_bnd_edges
  //`ratio1' and `ratio2' are parameters in [0, 1] of closest points
  // on the edges of the closest points
  // Return distance between two line segment. Return 0 when they intersect.
  static double bnd_line_to_bnd_line(const dbsk2d_bnd_edge_sptr& bnd_line1, 
    const dbsk2d_bnd_edge_sptr& bnd_line2, double& ratio1, double& ratio2);

  };

#endif // dbsk2d_closest_point_h_
