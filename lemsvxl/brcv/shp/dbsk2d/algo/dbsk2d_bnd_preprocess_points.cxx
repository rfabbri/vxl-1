// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess_points.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <vgl/vgl_distance.h>

#include <dbgl/algo/dbgl_closest_point.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>


// ---------- PREPROCESS POINTS -----------------------------


//---------------------------------------------------------
//: Remove unreal stand-alone points in the list of edges.
// This operation should not affect the vertex list
void dbsk2d_bnd_preprocess::
remove_unreal_stand_alone_points(vcl_list<dbsk2d_bnd_edge_sptr >& edges)
{
  bnd_edge_list::iterator eit = edges.begin();
  while (eit != edges.end())
  {
    // Do nothing for real edges
    if (!(*eit)->is_a_point())
    {
      ++eit;
    }
    // for degenerate edges (points), remove them if they are not 
    // really stand-alone
    else
    {
      // check if this is really a stand-alone point
      edge_list elist;
      (*eit)->v1()->edges(elist);
      if (elist.size() > 1)
      {
        (*eit)->unlink();
        eit = edges.erase(eit);
      }
      else
      {
        ++eit;
      }
    }
  }
  return;
}




// -----------------------------------------------------------------------
//: Merge close points in a group of points
// coordinates of the points are not chaged. (should they?)
// Require: all bnd_edges in `bnd_pts' are degenerate, i.e. points
void dbsk2d_bnd_preprocess::
merge_close_points(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts)
{
  // scan through all pairs of points in the list
  for (bnd_edge_list::iterator pit1 = bnd_pts.begin(); 
    pit1 != bnd_pts.end(); ++pit1)
  {
    bnd_edge_list::iterator pit2 = pit1;
    ++pit2;
    while (pit2 != bnd_pts.end())
    {
      // if two are the same, remove one from the list
      if (*pit2 == *pit1)
      {
        pit2 = bnd_pts.erase(pit2);
        continue;
      }

      vgl_point_2d<double > p1 = (*pit1)->point1();
      vgl_point_2d<double > p2 = (*pit2)->point1();
      
      // Do nothing when the two points in question are far apart
      if ((p2-p1).length() > dbsk2d_bnd_preprocess::distance_tol) 
      {
        ++pit2;
        continue;
      }

      // When they are close, remove one of them 

      // Question: should we translate the points left ???????
      //// Translate the other to center position
      //vgl_point_2d<double > center_pt = centre(p1, p2);
      //(*pit1)->bnd_v1()->bpoint()->set_pt(center_pt.x(), center_pt.y());

      (*pit2)->unlink_from_cells();
      (*pit2)->unlink();
      
      // remove the unlinked point from the overall point list
      pit2 = bnd_pts.erase(pit2);
    }
  }
}











// -------------  PREPROCESS POINT-LINES -------------------------


//---------------------------------------------------------
//: Remove stand-alone points if they are too close to a line
void dbsk2d_bnd_preprocess::
remove_points_close_to_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts,
  const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines)
{
  bnd_edge_list::iterator pt_it = bnd_pts.begin();
  while (pt_it != bnd_pts.end())
  {
    vgl_point_2d<double > pt = (*pt_it)->point1();
    bool remove_pt = false;
    for (bnd_edge_list::const_iterator eit = bnd_lines.begin();
      eit != bnd_lines.end(); ++eit)
    {
      vgl_point_2d<double > p1 = (*eit)->point1();
      vgl_point_2d<double > p2 = (*eit)->point2();
      double d = vgl_distance_to_linesegment(p1.x(), p1.y(), 
        p2.x(), p2.y(), pt.x(), pt.y());

      // if too close, remove the point
      if (d <= dbsk2d_bnd_preprocess::distance_tol)
      {
        remove_pt = true;
        break;
      }
    }

    if (remove_pt)
    {
      (*pt_it)->unlink_from_cells();
      (*pt_it)->unlink();
      pt_it = bnd_pts.erase(pt_it);
    }
    else
      ++pt_it;
  }
  
  return;
}



// -------------  PREPROCESS POINT-ARCS ------------------------- 

//: Remove stand-alone points if they are too close to an arc
void dbsk2d_bnd_preprocess::
remove_points_close_to_arcs(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_pts,
  const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs)
{
  bnd_edge_list::iterator pt_it = bnd_pts.begin();
  while (pt_it != bnd_pts.end())
  {
    vgl_point_2d<double > pt = (*pt_it)->point1();
    bool remove_pt = false;
    for (bnd_edge_list::const_iterator eit = bnd_arcs.begin();
      eit != bnd_arcs.end(); ++eit)
    {
      // distance from the vertex to the arc edge
      dbsk2d_bnd_edge_sptr e = *eit;
      dbsk2d_assert(e->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc = 
        static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
      vgl_point_2d<double > arc_p1 = barc->start();
      vgl_point_2d<double > arc_p2 = barc->end();
      double arc_k = barc->curvature();
      
      // compute distance and closest points on arc
      double ratio_pt;
      double d = dbgl_closest_point::point_to_circular_arc(pt, 
        arc_p1, arc_p2, arc_k, ratio_pt);

      // if too close, remove the point
      if (d <= dbsk2d_bnd_preprocess::distance_tol)
      {
        remove_pt = true;
        break;
      }
    }

    if (remove_pt)
    {
      (*pt_it)->unlink_from_cells();
      (*pt_it)->unlink();
      pt_it = bnd_pts.erase(pt_it);
    }
    else
      ++pt_it;
  }

  return;
}






