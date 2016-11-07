// This is file seg/dbsks/dbsks_utils.cxx

//:
// \file

#include "dbsks_utils.h"
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_math.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vgl/vgl_lineseg_test.h>



// -----------------------------------------------------------------------------
//: Assume all the memory have previously been allocated
bool dbsks_rotate_point_set(const vcl_vector<vgl_point_2d<double > >& source_pts,                                      
                            const vcl_vector<vgl_vector_2d<double > >& source_tangents,
                            const vgl_point_2d<double >& origin, double angle,
                            vcl_vector<vgl_point_2d<double > >& target_pts,
                            vcl_vector<vgl_vector_2d<double > >& target_tangents)
{
  unsigned num_pts = source_pts.size();
  assert(num_pts == source_tangents.size());
  assert(num_pts == target_pts.size());
  assert(num_pts == target_tangents.size());

  for (unsigned i =0; i <num_pts; ++i)
  {
    vgl_vector_2d<double > v0 = rotated(source_pts[i] - origin, angle);
    target_pts[i] = origin + v0;
    target_tangents[i] = rotated(source_tangents[i], angle);
  }

  return true;
}



// -----------------------------------------------------------------------------
//: Translate a set of point-tangents
//: Assume all the memory have previously been allocated
bool dbsks_translate_point_set(const vcl_vector<vgl_point_2d<double > >& source_pts,
                               const vcl_vector<vgl_vector_2d<double > >& source_tangents,
                               const vgl_vector_2d<double >& v,
                               vcl_vector<vgl_point_2d<double > >& target_pts,
                               vcl_vector<vgl_vector_2d<double > >& target_tangents)
{
  unsigned num_pts = source_pts.size();
  assert(num_pts == source_tangents.size());
  assert(num_pts == target_pts.size());
  assert(num_pts == target_tangents.size());

  for (unsigned i =0; i <num_pts; ++i)
  {
    target_pts[i] = source_pts[i] + v;
    target_tangents[i] = source_tangents[i];
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Compute bounding box of a set of shapelets
vsol_box_2d_sptr dbsks_compute_bounding_box(
  const vcl_vector<dbsksp_shapelet_sptr >& shapelet_list)
{
  vsol_box_2d_sptr bbox = new vsol_box_2d();
  for (unsigned i =0; i < shapelet_list.size(); ++i)
  {
    dbsksp_shapelet_sptr s = shapelet_list[i];
    vgl_point_2d<double > start = s->start();
    double r0 = s->radius_start();
    bbox->add_point(start.x() - r0, start.y() - r0);
    bbox->add_point(start.x() + r0, start.y() + r0);
    
    vgl_point_2d<double > end = s->end();
    double r1 = s->radius_end();
    bbox->add_point(end.x() - r1, end.y() - r1);
    bbox->add_point(end.x() + r1, end.y() + r1);
  }
  return bbox;
}



// -----------------------------------------------------------------------------
//: Compute bounding box of a set of xnodes
vsol_box_2d_sptr dbsks_compute_bounding_box(const
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)
{
  vsol_box_2d_sptr bbox = new vsol_box_2d();
  for (vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >::const_iterator
    it = xnode_map.begin(); it != xnode_map.end(); ++it)
  {
    dbsksp_shock_node_sptr v = it->first;
    dbsksp_xshock_node_descriptor xnode = it->second;

    vgl_point_2d<double > start = xnode.pt();
    double r0 = xnode.radius_;
    bbox->add_point(start.x() - r0, start.y() - r0);
    bbox->add_point(start.x() + r0, start.y() + r0);
  }
  return bbox;
}



// -----------------------------------------------------------------------------
//: Collect the the vertices of same depths and put them into bins
void dbsks_collect_shock_nodes_by_depth(const dbsksp_shock_graph_sptr& graph,
    vcl_vector<vcl_vector<dbsksp_shock_node_sptr > >& vertex_bins)
{

  int max_depth = 0;
  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    max_depth = vnl_math::max(max_depth, (*vit)->depth());
  }

  vertex_bins.clear();
  for (int i=0; i<= max_depth; ++i)
  {
    vertex_bins.push_back(vcl_vector<dbsksp_shock_node_sptr >() );
  }

  // Now assign the vertices to the bins
  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    vertex_bins[v->depth()].push_back(v);
  }
  return;
}




// -----------------------------------------------------------------------------
//: compute shock-edit cost between two shape fragments
// TODO: take into account include_front_arc and include_rear_arc
float dbsks_deform_cost_shock_edit(const dbsksp_shapelet_sptr& sp_ref, 
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& sp_target)
{
  dbsksp_shapelet_sptr s_ref = sp_ref;
  dbsksp_shapelet_sptr s_target = sp_target;

  dbgl_circ_arc arc_left_ref = s_ref->bnd_arc_left();
  dbgl_circ_arc arc_right_ref = s_ref->bnd_arc_right();

  dbgl_circ_arc arc_left_target = s_target->bnd_arc_left();
  dbgl_circ_arc arc_right_target = s_target->bnd_arc_right();

  // Components of shock-edit deformation
  // 1. boundary length difference
  double len_diff_left = vnl_math::abs(arc_left_ref.length() - arc_left_target.length());
  double len_diff_right = vnl_math::abs(arc_right_ref.length() - arc_right_target.length());
  double len_diff = len_diff_left + len_diff_right;

  // 2. bending angle difference
  double bending_diff_left = vnl_math::abs(arc_left_ref.length()*arc_left_ref.k() -
    arc_left_target.length() * arc_left_target.k());
  double bending_diff_right = vnl_math::abs(arc_right_ref.length()*arc_right_ref.k() -
    arc_right_target.length() * arc_right_target.k());
  double bending_diff = bending_diff_left + bending_diff_right;

  // 3. starting radius difference
  double start_radius_diff = vnl_math::abs(s_ref->radius_start() - s_target->radius_start());

  // 4. radius increment difference
  double radius_increment_diff = vnl_math::abs(
    s_ref->radius_increment() - s_target->radius_increment());

  // 5. phiA difference
  double start_phi_diff = vnl_math::abs(
    s_ref->phi_start() - s_target->phi_start());

  // 6. phi increment difference
  double phi_increment_diff = vnl_math::abs(
    (s_ref->phi_end() - s_ref->phi_start()) - (s_target->phi_end() - s_target->phi_start()));


  // weighing coefficient (experience from shock matching)
  // in shock matching experiment R = 6 for size 120x120

  // R should be proportional to the size of two fragments
  // since the two have different size, take the average.

  double scale_ref = vcl_sqrt(sp_ref->bounding_quad()->area());
  double scale_target = vcl_sqrt(sp_target->bounding_quad()->area());
  double mean_size = vcl_sqrt(scale_ref * scale_target);
  
  double R = 6 * mean_size / 120;
  
  // Combining everything together
  double total_deform_cost = len_diff + R*bending_diff + 2*start_radius_diff + 
    2 * radius_increment_diff + 2 * R * start_phi_diff + 2*R * phi_increment_diff;

  double shock_edit_scale = mean_size;


  // Return the normalized cost
  return float(total_deform_cost / shock_edit_scale);
}





// -----------------------------------------------------------------------------
//: Compute the size of a fragment (square root of the total occupying area)
double dbsks_fragment_size(const dbsksp_shapelet_sptr& s0,
  bool include_front_arc,
  bool include_rear_arc)
{
  // define the scale of a fragment as the area of the quad connecting the 
  // boundary end points
  vsol_polygon_2d_sptr quad = s0->bounding_quad();

  double area = quad->area();

  if (include_front_arc)
  {
    area += s0->area_front_arc();
  }

  if (include_rear_arc)
  {
    area += s0->area_rear_arc();
  }

  return vcl_sqrt(area);
}


// -----------------------------------------------------------------------------
//: Compute size of the working graph by summing the size of the fragments
double dbsks_compute_graph_size(const dbsksp_shock_graph_sptr& graph)
{
  double sum_area = 0;
  for (dbsksp_shock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    dbsksp_shapelet_sptr s = e->fragment()->get_shapelet();
    sum_area += s->area();
  }
  return vcl_sqrt(sum_area);
}






// -----------------------------------------------------------------------------
//: compute deformation cost between two shape fragments
float dbsks_deform_cost_log2_scale_diff(const dbsksp_shapelet_sptr& s_ref, 
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& s_target)
{
  double length_ref = dbsks_fragment_size(s_ref, include_front_arc, include_rear_arc);
  double length_target = dbsks_fragment_size(s_target, include_front_arc, include_rear_arc);
  double length_ratio = length_ref / length_target;
  double log2_length_ratio = vcl_log(length_ratio) / vnl_math::ln2;
  return float(log2_length_ratio);
}



//// -----------------------------------------------------------------------------
////: compute deformation cost between two shape fragments
//float dbsks_deform_cost(const dbsksp_shapelet_sptr& s_ref,
//  bool include_front_arc,
//  bool include_rear_arc,
//  const dbsksp_shapelet_sptr& s_target,
//  float scale_sigma)
//{
//
//
//  // START HERE - remove the scaling part ----------
//
//
//  float shock_edit_sigma = 0.5f;
//
//  float scale_diff = dbsks_deform_cost_log2_scale_diff(s_ref, include_front_arc,
//    include_rear_arc, s_target);
//
//  float shock_edit_deform = dbsks_deform_cost_shock_edit(s_ref, include_front_arc,
//    include_rear_arc, s_target);
//
//  float scale_cost = vnl_math::sqr(scale_diff / scale_sigma);
//  float shock_edit_cost = vnl_math::sqr(shock_edit_deform / shock_edit_sigma); 
//
//  return scale_cost + shock_edit_cost;
//}

// -----------------------------------------------------------------------------
//: compute deformation cost between two shape fragments
float dbsks_deform_cost(const dbsksp_shapelet_sptr& s_ref,
  bool include_front_arc,
  bool include_rear_arc,
  const dbsksp_shapelet_sptr& s_target,
  float sigma_deform)
{
  // we change the purpose of scale sigma a little bit :)
  float shock_edit_sigma = sigma_deform;

  float shock_edit_deform = dbsks_deform_cost_shock_edit(s_ref, include_front_arc,
    include_rear_arc, s_target);
  
  // multiply this cost by a constant, depending on how "regularized" the shape 
  // of the fragment is
  float shock_edit_cost = vnl_math::abs(shock_edit_deform / shock_edit_sigma); 

  return shock_edit_cost;
}


// ------------------------------------------------------------------------------
//: Trace out the boundary of a one-branch graph with its extrinsic nodes
vcl_vector<dbgl_circ_arc > dbsks_bnd_arc_list(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)
{
  vcl_vector<dbgl_circ_arc > arc_list;
  arc_list.clear();

  // Collect arcs from boundary of the associated with the fragment
  for (dbsksp_shock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge()) // A_infty edge
    {
      // Determine the geometric shape of the edge
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_xshock_node_descriptor xnode_parent = xnode_map.find(v_parent)->second;
      dbgl_circ_arc arc0;

      if (v_parent->depth() == 0)
      {
        xnode_parent = xnode_parent.opposite_xnode();
      }

      arc0.set_from(xnode_parent.bnd_pt_left(), xnode_parent.bnd_tangent_left(), 
        xnode_parent.pt_ + xnode_parent.radius_ * xnode_parent.shock_tangent());

      dbgl_circ_arc arc1;
      arc1.set_from(xnode_parent.bnd_pt_right(), xnode_parent.bnd_tangent_right(), 
        xnode_parent.pt_ + xnode_parent.radius_ * xnode_parent.shock_tangent());

      arc_list.push_back(arc0);
      arc_list.push_back(arc1);

    }
    else // regular A_12 edge
    {
      // Determine the geometric shape of the edge
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_shock_node_sptr v_child = e->child_node();

      dbsksp_xshock_node_descriptor xnode_parent = xnode_map.find(v_parent)->second;
      dbsksp_xshock_node_descriptor xnode_child = xnode_map.find(v_child)->second;

      // Since the biar interpolation codes are very sensity to bad input,
      // we'll be protective, avoid giving it bad inputs

      dbgl_biarc biarc_left;
      if ( biarc_left.compute_biarc_params(
        xnode_parent.bnd_pt_left(), 
        vcl_atan2(xnode_parent.bnd_tangent_left().y(), xnode_parent.bnd_tangent_left().x()),
        xnode_child.bnd_pt_left(), 
        vcl_atan2(xnode_child.bnd_tangent_left().y(), xnode_child.bnd_tangent_left().x()) ))
      {
        dbgl_circ_arc arc_l0(biarc_left.start(), biarc_left.mid_pt(), biarc_left.k1());
        dbgl_circ_arc arc_l1(biarc_left.mid_pt(), biarc_left.end(), biarc_left.k2());
        arc_list.push_back(arc_l0);
        arc_list.push_back(arc_l1);
      }

      dbgl_biarc biarc_right;
      if (biarc_right.compute_biarc_params(xnode_parent.bnd_pt_right(), 
        vcl_atan2(xnode_parent.bnd_tangent_right().y(), xnode_parent.bnd_tangent_right().x()),
        xnode_child.bnd_pt_right(), 
        vcl_atan2(xnode_child.bnd_tangent_right().y(), xnode_child.bnd_tangent_right().x()) ))
      {
        dbgl_circ_arc arc_r0(biarc_right.start(), biarc_right.mid_pt(), biarc_right.k1());
        dbgl_circ_arc arc_r1(biarc_right.mid_pt(), biarc_right.end(), biarc_right.k2());
        arc_list.push_back(arc_r0);
        arc_list.push_back(arc_r1);
      }
    }
  }
  return arc_list;
}



// -----------------------------------------------------------------------------
//: Trace boundary of a one-branch graph
vcl_vector<vsol_spatial_object_2d_sptr > dbsks_trace_boundary(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)

{
  vcl_vector<vsol_spatial_object_2d_sptr > bnd_list;

  // retrieve the boundary arcs
  vcl_vector<dbgl_circ_arc > arc_list = dbsks_bnd_arc_list(graph, xnode_map);

  // convert each arc to polygon
  for (unsigned i =0; i < arc_list.size(); ++i)
  {
    dbgl_circ_arc arc = arc_list[i];
    vcl_vector<vsol_point_2d_sptr > pts;
    
    // sample the arc using 10 segments
    double ds = arc.length() / 10;
    for (unsigned i =0; i < 11; ++i)
    {
      pts.push_back(new vsol_point_2d(arc.point_at_length(i*ds)));
    }

    bnd_list.push_back(new vsol_polyline_2d(pts));
  }
  return bnd_list;
}



// -----------------------------------------------------------------------------
//: Trace contact shocks
vcl_vector<vsol_spatial_object_2d_sptr > dbsks_trace_contact_shocks(const dbsksp_shock_graph_sptr& graph,
  const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)

{

  vcl_vector<vsol_spatial_object_2d_sptr > contact_shock_list;

  // get contact shocks from the xnodes
  for (vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >::const_iterator
    it = xnode_map.begin(); it != xnode_map.end(); ++it)
  {
    dbsksp_xshock_node_descriptor xnode = it->second;
    vcl_vector<vsol_point_2d_sptr > pts;
    pts.push_back(new vsol_point_2d(xnode.bnd_pt_left()));
    pts.push_back(new vsol_point_2d(xnode.pt_));
    pts.push_back(new vsol_point_2d(xnode.bnd_pt_right()));
    contact_shock_list.push_back(new vsol_polyline_2d(pts));
  }

  return contact_shock_list;
}


// -----------------------------------------------------------------------------
//: Determine whether two xnodes correspond to a LEGAL shape fragment
bool dbsks_is_legal_xfrag(const dbsksp_xshock_node_descriptor& xnode_parent,
                          const dbsksp_xshock_node_descriptor& xnode_child)
{

  // 1. shock point of child xnode has to be "in front of" parent xnode
  vgl_vector_2d<double > t_parent = xnode_parent.shock_tangent();
  vgl_vector_2d<double > v = xnode_child.pt() - xnode_parent.pt();

  // determine which half-plane the child xnode belong to
  if (inner_product(t_parent, v) < 0)
    return false;

  // 2. angle between the two shock tangent less pi/2.
  // This is not a strict legality condition. In theory, this condition can 
  // fail and there is still a legal fragment. But this is very unlikly.
  vgl_vector_2d<double > t_child = xnode_child.shock_tangent();
  if (angle(t_parent, t_child) > vnl_math::pi_over_2)
    return false;

  // 3. The four boundary points form a proper (legal) quadrilateral
  vgl_line_segment_2d<double > lineseg1(xnode_parent.bnd_pt_left(), xnode_child.bnd_pt_left());
  vgl_line_segment_2d<double > lineseg2(xnode_parent.bnd_pt_right(), xnode_child.bnd_pt_right());

  if (vgl_lineseg_test_lineseg(lineseg1, lineseg2))
    return false;

  return true;
}










// -----------------------------------------------------------------------------
//: Append a file to an out stream
vcl_ostream& dbsks_append_text_file(vcl_ostream& os, const vcl_string& filename)
{
  vcl_ifstream infp(filename.c_str());
  char linebuffer[1024];
  while (infp.getline(linebuffer,1024)) 
  {
    os << linebuffer << "\n";
  }
  return os;
}

//: Helper function to avoid the assertion failure reported by the 'set' method of 'pdf1d_flat' class due to equality of min and max values
void dbsks_regularize_min_max_values(double& min_val, double& max_val)
{
    if(min_val == max_val)
    {
        min_val -= 0.0000001;
        max_val += 0.0000001;
    }
}
