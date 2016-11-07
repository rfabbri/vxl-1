// This is basic/dbgl/algo/dbsk2d_distance.cxx
//:
// \file

#include "dbsk2d_distance.h"

#include <vnl/vnl_math.h>
//#include <vgl/vgl_lineseg_test.h>
//#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_distance.h>

//#include <vgl/vgl_closest_point.h>
//
#include <dbgl/algo/dbgl_distance.h>

#include <dbsk2d/dbsk2d_bnd_edge.h>

//: Return the min-distance between a point and an dbsk2d_bnd_edge
// if `direction' = -1, s_start and s_end are measured from end vertex

// need rewrite case for arc
double dbsk2d_distance::
point_to_bnd_edge(const vgl_point_2d<double >& query_pt,
  const dbsk2d_bnd_edge_sptr& edge,
  int direction,
  double s_start,
  double s_end)
{
  //vgl_closest_point_to_linesegment 
  // degenerate case
  if (edge->is_a_point())
  {
    return vgl_distance(query_pt, edge->bnd_v1()->point());
  }
  
  // select the internal bcurve to compute distance to
  dbsk2d_ishock_bcurve* bcurve = 
    (direction == 1) ? edge->left_bcurve():edge->right_bcurve();
  
  // check input validity
  dbsk2d_assert (bcurve);
  dbsk2d_assert (s_start <= s_end);
  dbsk2d_assert (s_end >= 0);
  dbsk2d_assert (s_start <= bcurve->len());
  
  // now all numbers are guaranteed in proper range, i.e
  // 0<= s_min <= s_max <= bcurve->len();
  vgl_point_2d<double > start = bcurve->start();
  vgl_point_2d<double > end = bcurve->end();

  //case 1: internal bcurve is a line segment
  if (bcurve->is_a() == "dbsk2d_ishock_bline")
  {
    double smin = vnl_math::max((double)0, s_start) / bcurve->len() ;
    double smax = vnl_math::min(bcurve->len(), s_end) / bcurve->len();
    return vgl_distance_to_linesegment<double >(
      (1-smin)*start.x() + smin*end.x(), (1-smin)*start.y()+smin*end.y(), 
      (1-smax)*start.x()+smax*end.x(), (1-smax)*start.y()+smax*end.y(),
      query_pt.x(), query_pt.y());
    
  }

  //case 1: internal bcurve is a line segment
  // \TODO implement this case
  if (bcurve->is_a() == "dbsk2d_ishock_barc")
  {
    return vnl_huge_val((double)1);
  }

  //shoud not get here
  dbsk2d_assert(0);
  return vnl_huge_val((double)1);
}


//: Return the min-distance between two LINE dbsk2d_bnd_edges.
// Require: the internal bcurve inside the `bnd_edge' are `dbsk2d_ishock_bline'
double dbsk2d_distance::
bnd_line_to_bnd_line(const dbsk2d_bnd_edge_sptr& bnd_line1,
                     const dbsk2d_bnd_edge_sptr& bnd_line2)
{
  dbsk2d_assert (bnd_line1->left_bcurve()->is_a_line());
  dbsk2d_assert (bnd_line2->left_bcurve()->is_a_line());
  // the start and end of left_bcurve coincide with the start and end of the bnd_edge
  return dbgl_distance::lineseg_lineseg(bnd_line1->left_bcurve()->start(), bnd_line1->left_bcurve()->end(),
    bnd_line2->left_bcurve()->start(), bnd_line2->left_bcurve()->end());
}


//: Return the min-distance between a vertex and a line-edge
double dbsk2d_distance::
vertex_to_bnd_line(const dbsk2d_bnd_vertex_sptr& vertex,
  const dbsk2d_bnd_edge_sptr& bnd_line)
{
  dbsk2d_assert(bnd_line->left_bcurve()->is_a_line());
  vgl_point_2d<double > p = vertex->point();
  vgl_point_2d<double > p1 = bnd_line->point1();
  vgl_point_2d<double > p2 = bnd_line->point2();

  return vgl_distance_to_linesegment<double >(p1.x(), p1.y(), 
    p2.x(), p2.y(), p.x(), p.y());
}


