// This is file shp/dbsksp/dbsksp_simplify.cxx

//:
// \file

#include "dbsksp_simplify.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

//#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
//#include <dbnl/dbnl_angle.h>


// -----------------------------------------------------------------------------
//: Convert an A12_Ainfty node to an A12 node whenever possible, i.e., the
// discrepancy between the shape resulted from this conversion and the original
// shape boundary is less than a threshold
bool dbsksp_reduce_A12_Ainfty_nodes(const dbsksp_xshock_graph_sptr& xgraph, 
                                    const vsol_polygon_2d_sptr& boundary,
                                    double eps,
                                    double sampling_ds)
{
  if (!xgraph) return false;
  if (!boundary) return false;

  // convert the boundary polygon to the form usable in function
  // vgl_distance_to_closed_polygon(...)
  vnl_vector<double > bnd_x(boundary->size());
  vnl_vector<double > bnd_y(boundary->size());
  for (unsigned i =0; i < boundary->size(); ++i)
  {
    bnd_x[i] = boundary->vertex(i)->x();
    bnd_y[i] = boundary->vertex(i)->y();
  }

  // list of terminal nodes to remove 
  vcl_vector<dbsksp_xshock_node_sptr > terminal_nodes_to_remove;

  // locate the A12_Ainfty nodes and check each of them separately
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); 
    vit != xgraph->vertices_end(); ++vit)
  {
    // a) locate A12_Ainfty nodes
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree() != 3)
      continue;


    // an A12_Ainfty has one branch which is simply a terminal edge
    vcl_vector<dbsksp_xshock_edge_sptr > terminal_edges;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      if (xe->is_terminal_edge())
      {
        terminal_edges.push_back(xe);
      }
    }

    // only handle A12_Ainfty nodes
    if (terminal_edges.size() != 1)
      continue;

    // b) construct the new A12 node

    // We now haved located A12_Ainfty. We will check the discrepancy between the
    // shape resulting from converting it to A12 node and the origninal boundary
    // In theory, we can optimize the new A12 node to find the lowest discrepancy
    // before comparing it with threshold
    // But for now, we will simply pick the A12 node which has:
    // (i) one contact point at the original A12 contact point and 
    // (ii) the other contact point at the center of the A_infty branch

    // the edges in CCW order: xe0 --> xe1 (terminal branch) --> xe2.
    dbsksp_xshock_edge_sptr xe1 = terminal_edges.front();
    dbsksp_xshock_edge_sptr xe0 = xgraph->cyclic_adj_pred(xe1, xv);
    dbsksp_xshock_edge_sptr xe2 = xgraph->cyclic_adj_succ(xe1, xv);

    // retrieve the descriptors of the 3 branches
    dbsksp_xshock_node_descriptor* xdesc0 = xv->descriptor(xe0);
    dbsksp_xshock_node_descriptor* xdesc1 = xv->descriptor(xe1);
    dbsksp_xshock_node_descriptor* xdesc2 = xv->descriptor(xe2);

    // the node descriptors on the "other" side of xe0 and xe2 (opposite of xv)
    dbsksp_xshock_node_descriptor other_xdesc0 = *xe0->opposite(xv)->descriptor(xe0);
    dbsksp_xshock_node_descriptor other_xdesc2 = xe2->opposite(xv)->descriptor(xe2)->opposite_xnode();

    // The new A12 node
    vgl_point_2d<double > new_bnd_pt_left = xdesc2->bnd_pt_left();
    vgl_point_2d<double > new_bnd_pt_right = xv->pt() + xv->radius() * xdesc1->shock_tangent();
    dbsksp_xshock_node_descriptor new_xdesc(xv->pt(), new_bnd_pt_left, new_bnd_pt_right);

    // c) Two new boundary contours (biarcs) are formed due to this change.
    // We compute their maximum deviation from the (polygon) boundary
    dbsksp_xshock_fragment_sptr new_xfrag0 = new dbsksp_xshock_fragment(other_xdesc0, new_xdesc);
    dbsksp_xshock_fragment_sptr new_xfrag2 = new dbsksp_xshock_fragment(new_xdesc, other_xdesc2);

    // the two boundary contours
    dbgl_biarc new_bnd_right0 = new_xfrag0->bnd_right_as_biarc();
    dbgl_biarc new_bnd_right2 = new_xfrag2->bnd_right_as_biarc();

    // sample the two new boundary contours with ds = 1 (chosen arbitrarily)
    vcl_vector<vgl_point_2d<double > > new_bnd_pts;
    
    double bnd_len0 = new_bnd_right0.len();
    double bnd_len2 = new_bnd_right2.len();
    new_bnd_pts.reserve( vnl_math::ceil((bnd_len0 + bnd_len2) / sampling_ds) + 2 );

    // first contour
    int half_num_pts0 = vnl_math::floor(bnd_len0 / (2*sampling_ds));
    for (int idx = -half_num_pts0; idx <= half_num_pts0; ++idx)
    {
      double s = bnd_len0/2 + idx * sampling_ds;
      new_bnd_pts.push_back(new_bnd_right0.point_at(s));
    }

    int half_num_pts2 = vnl_math::floor(bnd_len2 / (2*sampling_ds));
    for (int idx = -half_num_pts2; idx <= half_num_pts2; ++idx)
    {
      double s = bnd_len2/2 + idx * sampling_ds;
      new_bnd_pts.push_back(new_bnd_right2.point_at(s));
    }
    
    // compute maximum distance to boundary polygon
    double max_distance = -1;
    for (unsigned i =0; i < new_bnd_pts.size(); ++i)
    {
      vgl_point_2d<double > query_pt = new_bnd_pts[i];
      double query_distance = vgl_distance_to_closed_polygon(bnd_x.data_block(), 
        bnd_y.data_block(), bnd_x.size(), query_pt.x(), query_pt.y());
      max_distance = vnl_math::max(max_distance, query_distance);
    }

    // d) If the max deviation is less than threshold, mark the node for conversion later
    if (max_distance < eps)
    {
      terminal_nodes_to_remove.push_back(xe1->opposite(xv));
    }
  }

  // remove the terminal nodes marked for deletion
  for (unsigned i =0; i < terminal_nodes_to_remove.size(); ++i)
  {
    dbsksp_xshock_node_sptr terminal_xv = terminal_nodes_to_remove[i];
    dbsksp_xshock_edge_sptr terminal_xe = *(terminal_xv->edges_begin());
    dbsksp_xshock_node_sptr degree_3_node = terminal_xe->opposite(terminal_xv);
    xgraph->remove_A_infty_branch(degree_3_node, terminal_xe, 0.5);
  }
  return true;
}


