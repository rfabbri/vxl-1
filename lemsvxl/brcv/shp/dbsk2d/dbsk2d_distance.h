// This is basic/dbgl/algo/dbsk2d_distance.h

#ifndef dbsk2d_distance_h_
#define dbsk2d_distance_h_

//:
// \file
// \brief Algorithms to compute shortest distance between shock 2D objects
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

//#include <vgl/vgl_line_segment_2d.h>

#include <dbsk2d/dbsk2d_bnd_edge_sptr.h>
#include <dbsk2d/dbsk2d_bnd_vertex_sptr.h>
#include <vgl/vgl_point_2d.h>

class dbsk2d_distance
{
protected:
public:
  
  ~dbsk2d_distance(){};

  //: Return the min-distance between a point and an dbsk2d_bnd_edge
  // if `direction' = -1, s_start and s_end are measured from end vertex
  // edge that is closest to `query_pt'
  // need rewrite for case of an arc
  static double point_to_bnd_edge(const vgl_point_2d<double >& query_pt,
    const dbsk2d_bnd_edge_sptr& edge,
    int direction = 1,
    double s_start = 0,
    double s_end = 1e10
    );

  //: Return the min-distance between two LINE dbsk2d_bnd_edges.
  // Require: the internal bcurve inside the `bnd_edge' are `dbsk2d_ishock_bline'
  static double bnd_line_to_bnd_line(const dbsk2d_bnd_edge_sptr& bnd_line1,
    const dbsk2d_bnd_edge_sptr& bnd_line2);

  //: Return the min-distance between a vertex and a line-edge
  static double vertex_to_bnd_line(const dbsk2d_bnd_vertex_sptr& vertex,
    const dbsk2d_bnd_edge_sptr& bnd_line);

private:
  dbsk2d_distance(){};
  };

#endif // dbsk2d_distance_h_
