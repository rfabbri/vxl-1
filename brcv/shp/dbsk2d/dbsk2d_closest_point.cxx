// This is basic/dbgl/algo/dbsk2d_closest_point.cxx
//:
// \file

#include "dbsk2d_closest_point.h"

#include <vgl/vgl_closest_point.h>
#include <dbgl/algo/dbgl_closest_point.h>

#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_distance.h>



//---------------------------------------------------------------------------
//: Return parameter (arclength) of the point on bnd_edge that is 
// closest to point `pt'
// if `direction' = -1, s_start and s_end are measured from end vertex
// `arclength' returns the arclength of the point on
// edge that is closest to `query_pt'
// Returned arclength is always measured from start vertex
// need rewrite
double dbsk2d_closest_point::
point_to_bnd_edge(const vgl_point_2d<double >& query_pt,
                  const dbsk2d_bnd_edge_sptr& edge,
                  int direction,
                  double s_start,
                  double s_end)
{
  if (edge->is_a_point())
    return 0;
  
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
    // range of actual linesegment to compute closest point
    double smin = vnl_math::max((double)0, s_start) / bcurve->len() ;
    double smax = vnl_math::min(bcurve->len(), s_end) / bcurve->len();
    
    double px, py;
    vgl_closest_point_to_linesegment<double >(px, py,
      (1-smin)*start.x()+smin*end.x(), (1-smin)*start.y()+smin*end.y(), 
      (1-smax)*start.x()+smax*end.x(), (1-smax)*start.y()+smax*end.y(),
      query_pt.x(), query_pt.y());

    // starting point of edge is `end' when direction == -1
    return (direction == 1) ? vnl_math::hypot(px-start.x(), py-start.y()) :
    vnl_math::hypot(px-end.x(), py-end.y());
  }

  // case 2: internal bcurve is an arc
  // \TODO implement this case
  if (bcurve->is_a() == "dbsk2d_ishock_barc")
  {
    return -1;
  }

  //shoud not get here
  dbsk2d_assert(0);
  return -1;
}

//---------------------------------------------------------------------------
//: return the arclength parameter of the point on `contour'
// that is closest to the query point between s_start and s_end (inclusive)
double dbsk2d_closest_point::
point_to_bnd_contour(const vgl_point_2d<double>& query_pt,
                     const dbsk2d_bnd_contour_sptr& contour,
                     double s_start, 
                     double s_end)
{
  //0. Validity check
  // check validity of `s_start' and `s_end'
  if (s_end < 0) return -1;
  if (s_start > s_end) return -1;

  int start_index = 0;
  if (s_start > 0) start_index = contour->edge_index_at(s_start);
  
  int end_index = contour->edge_index_at(s_end);
  if (end_index >= contour->num_edges()) end_index = contour->num_edges()-1;

  //require
  dbsk2d_assert(start_index <= end_index);
  
  //1. track down index of edge where min distance occurs
  int index = 0;
  // the search range is within one edge
  if (start_index == end_index)
  {
    index = start_index;
  }
  // the search range spans more than one edge
  else
  {
    // locate index of edge at closest point
    double min_dist = vnl_huge_val((double)1);
    for (int i = start_index; i <= end_index; ++i)
    {
      // start and end arclength on edge to compute min distance
      double smin = vnl_math::max((double)0, s_start-contour->arclength_at(i));
      double smax = vnl_math::min(s_end, contour->arclength_at(i+1))- 
        contour->arclength_at(i);
      
      // compute distance to closest point and compare with previous ones
      double temp_dist = dbsk2d_distance::point_to_bnd_edge(query_pt, 
        contour->bnd_edge(i),contour->dir(i),smin,smax);
      if (temp_dist<min_dist)
      {
        min_dist = temp_dist;
        index = i;
      }
    }
  }

  // 2. Compute arclength of the closest point on contour
  // start and end arclength on edge to compute closest point
  double smin = vnl_math::max((double)0, s_start-contour->arclength_at(index));
  double smax = vnl_math::min(s_end, contour->arclength_at(index+1))- 
    contour->arclength_at(index);

  // arclength on edge of the closest point
  double eta = dbsk2d_closest_point::point_to_bnd_edge(query_pt, 
    contour->bnd_edge(index),
    contour->dir(index), smin, smax);

  // if direction of edge is reversed on contour
  if (contour->dir(index) == -1)
    return contour->arclength_at(index+1) - eta;
  // normal direction
  else
    return contour->arclength_at(index) + eta;

  //should not get here
  return -1;
}



//-----------------------------------------------------------------------
//: Compute closest point between two line dbsk2d_bnd_edges
// on the edges of the closest points
// Return distance between two line segment. Return 0 when they intersect.
double dbsk2d_closest_point::
bnd_line_to_bnd_line(const dbsk2d_bnd_edge_sptr& bnd_line1, 
                     const dbsk2d_bnd_edge_sptr& bnd_line2, 
                     double& ratio1, double& ratio2)
{
  dbsk2d_assert(bnd_line1->left_bcurve()->is_a_line());
  dbsk2d_assert(bnd_line2->left_bcurve()->is_a_line());
  return dbgl_closest_point::lineseg_lineseg(
    bnd_line1->left_bcurve()->start(), bnd_line1->left_bcurve()->end(),
    bnd_line2->left_bcurve()->start(), bnd_line2->left_bcurve()->end(),
    ratio1, ratio2);
}

