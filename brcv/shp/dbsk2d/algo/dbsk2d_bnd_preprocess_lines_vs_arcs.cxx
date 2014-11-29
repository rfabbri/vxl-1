// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess_lines_vs_arcs.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbgl/algo/dbgl_closest_point.h>

#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>


// -------------  PREPROCESS LINES-ARCS ---------------------------------

//------------------------------------------------------------------------
//: Detect and form all intersection between `arcset' and `lineset'
// `tainted_edges' contains all arcs affected by intersection and need
// further processing
void dbsk2d_bnd_preprocess::
intersect_lines_against_arcs(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_edges, 
                             vcl_list<dbsk2d_bnd_edge_sptr >* arcset, 
                             vcl_list<dbsk2d_bnd_edge_sptr >* lineset)
{

  // clear old stuffs
  tainted_edges->clear();

  // For every pair of line and arcs, check closest distance
  // Intersect, form junction (X) when necessary. 
  // The junctions are first marked, then formed at the end.
  
  // A list of modification to the edges to be made at the end
  vkey_vertex_map line_toadd_vertices;
  vkey_vertex_map arc_toadd_vertices;

  // scan through all pair of arcs and lines
  for (bnd_edge_list::iterator eit1 = lineset->begin();
    eit1 != lineset->end(); ++eit1)
  {
    dbsk2d_bnd_edge_sptr e1 = (*eit1);
    // required
    dbsk2d_assert(e1->left_bcurve()->is_a_line());
    // geometry of the line
    vgl_point_2d<double > p11 = e1->point1();
    vgl_point_2d<double > p12 = e1->point2();
        
    for ( bnd_edge_list::iterator eit2 = arcset->begin();
      eit2 != arcset->end(); ++eit2)
    {
      dbsk2d_bnd_edge_sptr e2 = (*eit2);

      // ignore the case when the line-arc pair share both end-vertices
      // because we already know the two intersection points
      if (e2->is_endpoint(e1->v1()) && (e2->is_endpoint(e1->v2()))) continue;

      // Do nothing when the the line and arc don't intersect
      
      // geometry of arc
      vgl_point_2d<double > p21 = e2->point1();
      vgl_point_2d<double > p22 = e2->point2();
      dbsk2d_assert(e2->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc2 = static_cast<dbsk2d_ishock_barc* >(e2->left_bcurve());
      double k2 = barc2->curvature();

      // potential intersecting locations
      vcl_vector<double > arc_ratios;
      vcl_vector<double > line_ratios;
      double d = dbgl_closest_point::lineseg_to_circular_arc(
        p11, p12, p21, p22, k2, line_ratios, arc_ratios);

      
      // ignore distance above threshold
      if (d > dbsk2d_bnd_preprocess::distance_tol) continue;

      // form a geometric arc equivalent to barc2
      dbgl_circ_arc arc2(p21, p22, k2);
        
      // form junction at intersection. 
      // ignore intersection at endpoints. It is handled in a different fnct.
      for (unsigned int i=0; i<arc_ratios.size(); ++i)
      {
        // ignore if this intersection point is an endpoint
        if (vnl_math::abs(arc_ratios[i]) < 1e-12 ||
          vnl_math::abs(arc_ratios[i]-1) < 1e-12 ||
          vnl_math::abs(line_ratios[i]) < 1e-12 ||
          vnl_math::abs(line_ratios[i]-1) < 1e-12)
        {
          continue;
        }

        // form junction at closest point (intersection point)

        // create a new vertex for each intersection
        vgl_point_2d<double > new_pt = centre<double >(
          midpoint<double >(p11, p12, line_ratios[i]),
          
          //: ozge changed point_at to point_at_length
          //arc2.point_at_length(arc_ratios[i]));

          //: Nhon changed back to point_at(...) because point_at(t) takes in t \in [0,1]
          arc2.point_at(arc_ratios[i]));
      
        dbsk2d_bnd_vertex_sptr newv = 
          dbsk2d_bnd_utils::new_vertex(new_pt, this->boundary());

        // mark the new vertex to be added to both line and arc
        // line
        vkey key1(e1, line_ratios[i]);
        line_toadd_vertices.insert(vkey_vertex_pair(key1, newv) );

        // arc
        vkey key2(e2, arc_ratios[i]);
        arc_toadd_vertices.insert(vkey_vertex_pair(key2, newv) );
      }
  
    }
  }

  
  // get list of vertices affected by dividing the lines
  bnd_vertex_list affected_vertices;
  this->insert_new_vertices(line_toadd_vertices, *lineset, affected_vertices);

  // get list of vertices affected by dividing the arcs
  bnd_vertex_list affected_vertices_b;
  this->insert_new_vertices(arc_toadd_vertices, *arcset, affected_vertices_b);



  //// debug //////////////////
  //for (vcl_list<dbsk2d_bnd_edge_sptr >::iterator it = lineset->begin(); 
  //  it != lineset->end(); ++it)
  //{
  //  dbsk2d_bnd_edge_sptr line = *it;
  //  if (!line->left_bcurve() || !line->right_bcurve())
  //  {
  //    vcl_cerr << "ALERT: empty bnd line!!!!! \n";
  //  }
  //
  //}

  //for (vcl_list<dbsk2d_bnd_edge_sptr >::iterator it = arcset->begin(); 
  //  it != arcset->end(); ++it)
  //{
  //  dbsk2d_bnd_edge_sptr arc = *it;
  //  if (!arc->left_bcurve() || !arc->right_bcurve())
  //  {
  //    vcl_cerr << "ALERT: empty bnd line!!!!! \n";
  //  }
  //
  //}

  ////////////////////////////



  // combine the two vertex lists
  affected_vertices.splice(affected_vertices.end(), affected_vertices_b);

  // get the overall edge list affected by intersection operation
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, *tainted_edges);
  return;
}



// -----------------------------------------------------------------------
//: Remove arcs that share both vertices with another line and the maximum distance
// between the arc and the line is less than distance threshold
void dbsk2d_bnd_preprocess::
remove_arcs_duplicating_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_arcs, 
                              const vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines)
{
  bnd_edge_list::iterator eit1 = bnd_arcs.begin();
  while (eit1 != bnd_arcs.end())
  {
    dbsk2d_bnd_edge_sptr e1 = *eit1;
    bool arc_deleted = false;
    for (bnd_edge_list::const_iterator eit2 = bnd_lines.begin(); 
      eit2 != bnd_lines.end(); ++eit2)
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;

      // only consider pair of (line,arc) that share both vertices
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
        continue;
      }

      // check max distance between line and arc
      // get the geometry of the arc
      dbsk2d_assert(e1->left_bcurve()->is_an_arc());
      dbsk2d_ishock_barc* barc1 = 
        static_cast<dbsk2d_ishock_barc* >(e1->left_bcurve());
      dbgl_circ_arc arc1(barc1->start(), barc1->end(), barc1->curvature());

   
      // computing max distance
      double max_distance = arc1.height();

      if (max_distance > dbsk2d_bnd_preprocess::distance_tol) continue;

      // distance falls below threshold, replace the arc by line
      e2->merge_with(*e1);
      arc_deleted = true;
      break;
    }
    
    // erase arc `e1' from the overall arc list if it has been merged
    // with another line
    if (arc_deleted) eit1 = bnd_arcs.erase(eit1);
    else ++eit1;
  }

  return;
}





