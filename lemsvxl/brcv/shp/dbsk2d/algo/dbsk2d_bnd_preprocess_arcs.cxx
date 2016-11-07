// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess_arcs.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>

#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbgl/algo/dbgl_closest_point.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>







// -------------  PREPROCESS ARCS ---------------------------------

// -----------------------------------------------------------------------
//: Detect and form all intersection between `arcset1' and `arcset2'
// if `arcset2' not given then intersect `arcset1' against itself
// Return: `arcset1' contains all arcs from both sets and `arcset2' will 
// be empty
// `tainted_arcs' contains all arcs affected by intersection and need
// further processing
void dbsk2d_bnd_preprocess::
intersect_bnd_arcs(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_arcs, 
                   vcl_list<dbsk2d_bnd_edge_sptr >* arcset1, 
                   vcl_list<dbsk2d_bnd_edge_sptr >* arcset2)
{
  // clear old stuffs
  tainted_arcs->clear();

  // For every pair of arcs, form an X junction when they intersect
  // The junctions are first marked, then formed at the end.

  bnd_edge_list* arcset2b = (arcset2) ? arcset2 : arcset1;
  
  // A list of modification to the edges to be made at the end
  vkey_vertex_map toadd_vertices;

  // scan through all pair of arcs
  for (bnd_edge_list::iterator eit1 = arcset1->begin(); 
    eit1 != arcset1->end(); ++eit1)
  {
    bnd_edge_list::iterator eit2;
    if (arcset2 && (arcset2 != arcset1))
    {
      eit2 = arcset2->begin();
    }
    // no need to start from begining when intersecting arcset1 against itself
    else
    {
      eit2 = eit1;
      ++eit2;
    }

    dbsk2d_bnd_edge_sptr e1 = *eit1;
    while (eit2 != arcset2b->end())
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;
      if (e1 == e2)
      {
        eit2 = arcset2b->erase(eit2);
        continue;
      }

      // ignore the case when two arcs share both vertices
      // because their intersections have been recorded, hence...`shared vertices'
      if (e2->is_endpoint(e1->v1()) && (e2->is_endpoint(e1->v2()))) 
      {
        ++eit2;
        continue;
      }

      // Do nothing when the two arcs don't intersect (fuzzily)
      // we also ignore intersection at endpoints because they will handle
      // in a different functions (dissolve_vertices_into_curves)
      vgl_point_2d<double > p11 = e1->point1();
      vgl_point_2d<double > p12 = e1->point2();
      dbsk2d_assert(e1->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc1 = static_cast<dbsk2d_ishock_barc* >(e1->left_bcurve());
      double k1 = barc1->curvature();

      vgl_point_2d<double > p21 = e2->point1();
      vgl_point_2d<double > p22 = e2->point2();
      dbsk2d_assert(e2->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc2 = static_cast<dbsk2d_ishock_barc* >(e2->left_bcurve());
      double k2 = barc2->curvature();

      // potential intersecting locations
      vcl_vector<double > arc1_ratios;
      vcl_vector<double > arc2_ratios;
      double d = dbgl_closest_point::circular_arc_to_circular_arc(
        p11, p12, k1, p21, p22, k2, arc1_ratios, arc2_ratios);

      // ignore distance above threshold
      if (d > dbsk2d_bnd_preprocess::distance_tol) 
      {
        ++eit2;
        continue;
      }

      // form two geometric arcs equivalent to the edges
      dbgl_circ_arc arc1(p11, p12, k1);
      dbgl_circ_arc arc2(p21, p22, k2);

      // form junction at intersection. 
      // ignore intersection at endpoints. It is handled in a different fnct.
      for (unsigned int i=0; i<arc1_ratios.size(); ++i)
      {
        // ignore if this intersection point is an endpoint
        if (vnl_math::abs(arc1_ratios[i]) < 1e-12 ||
          vnl_math::abs(arc1_ratios[i]-1) < 1e-12 ||
          vnl_math::abs(arc2_ratios[i]) < 1e-12 ||
          vnl_math::abs(arc2_ratios[i]-1) < 1e-12)
        {
          continue;
        }
       
        // form junction at closest point (intersection point)
        vgl_point_2d<double > new_pt = centre<double >( 
          //: ozge changed
          // arc1.point_at_length(arc1_ratios[i]), arc2.point_at_length(arc2_ratios[i]));

          //: Nhon changed back
          arc1.point_at(arc1_ratios[i]), arc2.point_at(arc2_ratios[i]));
      
        dbsk2d_bnd_vertex_sptr newv = 
          dbsk2d_bnd_utils::new_vertex(new_pt, this->boundary());

        // mark the new vertex to be added to both arcs
        // arc1
        vkey key1(e1, arc1_ratios[i]);
        toadd_vertices.insert(vkey_vertex_pair(key1, newv) );

        // arc 2
        vkey key2(e2, arc2_ratios[i]);
        toadd_vertices.insert(vkey_vertex_pair(key2, newv) );
      }
      
      // move to next edge on the list
      ++eit2;
    }
  }

  // combine the two list and save to `arcset1'
  if (arcset2 && (arcset2 != arcset1))
    arcset1->splice(arcset1->end(), *arcset2);

  // get the list of arc edges affected by this operation
  bnd_vertex_list affected_vertices;
  this->insert_new_vertices(toadd_vertices, *arcset1, affected_vertices);

  bnd_edge_list all_affected_edges;
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, all_affected_edges);
  this->classify_edges(all_affected_edges, 0, 0, tainted_arcs);
  return;
}



//--------------------------------------------------------------------------
//: Remove (exact) duplicate arcs - 
// arcs with same end vertices and maximum distance is < threshold
void dbsk2d_bnd_preprocess::
remove_duplicate_arcs(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs)
{
  // Order N^2
  for (bnd_edge_list::iterator eit1 = bnd_arcs.begin();
    eit1 != bnd_arcs.end(); ++eit1)
  {
    dbsk2d_bnd_edge_sptr e1 = *eit1;
    
    // get the geometry of this edge
    // required
    dbsk2d_assert(e1->left_bcurve()->is_an_arc());

    dbsk2d_ishock_barc* barc1 = 
      static_cast<dbsk2d_ishock_barc* >(e1->left_bcurve());
    dbgl_circ_arc arc1(barc1->start(), barc1->end(), barc1->curvature());


    // iterate through the rest of the edges
    bnd_edge_list::iterator eit2 = eit1;
    ++eit2;
    while (eit2 != bnd_arcs.end())
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;

      // check if the line edges are exactly the same
      signed char dir = 1;
      if (e1->v1()==e2->v1() && e1->v2()==e2->v2())
      {
        dir = 1;
      }
      else if (e1->v1()==e2->v2() && e1->v2()==e2->v1())
      {
        dir = -1;
      }
      else
      {
        ++eit2;
        continue;
      }

      // geometry of the second arc
      dbsk2d_assert(e2->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc2 =
        (dir == 1) ? static_cast<dbsk2d_ishock_barc* >(e2->left_bcurve()) : 
        static_cast<dbsk2d_ishock_barc* >(e2->right_bcurve());
      dbgl_circ_arc arc2(barc2->start(), barc2->end(), barc2->curvature());

      // computing max distance difference depending on signs of curvatures 
      // of two arcs, given they share same end points
      double max_distance = -1;
      if ((arc1.k()<0 && arc2.k()>0) || (arc1.k()>0 && arc2.k()<0))
      {
        max_distance = arc1.height() + arc2.height();
      }
      else
      {
        max_distance = vnl_math::abs(arc1.height()-arc2.height());
      }
      dbsk2d_assert(max_distance >= 0);

      if (max_distance > dbsk2d_bnd_preprocess::distance_tol) 
      {
        ++eit2;
        continue;
      }

      // distance falls below threshold, one (e2) has to go. Replace e2 by e1. 
      e1->merge_with(*e2);

      // erase e2 from the overall list
      eit2 = bnd_arcs.erase(eit2);
    }
  }
  return;
}



