// This is file shp/dbsksp/algo/dbsksp_xgraph_algos.cxx

//:
// \file

#include "dbsksp_xgraph_algos.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>
#include <dbsksp/algo/dbsksp_interp_xshock_fragment.h>
#include <dbnl/dbnl_math.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
//#include <vcl_utility.h>



//: Close all the gaps between branches by splitting the error equally between
// its two adjacent branches, except when one of the branches is a terminal edge. 
// In that case the terminal edge absorbs all the errors.
void dbsksp_xgraph_algos::
close_angle_gaps_at_nodes(const dbsksp_xshock_graph_sptr& xgraph)
{
  // Check for gaps between adjacent edges that share the same end nodes
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
    xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // Nothing can be changed about degree-1 nodes
    if (xv->degree() == 1)
      continue;

    // For each edge incident at `xv', compute the gap between the angle gap between
    // the edge and its successor edge (counter-clockwise). If there is a gap, split it
    // between the two edges involved.
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      // the two edges
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_edge_sptr next_xe = xgraph->cyclic_adj_succ(cur_xe, xv);

      // their corresponding descriptor
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      dbsksp_xshock_node_descriptor* next_xdesc = xv->descriptor(next_xe);

      // contact shocks (normal to boundary) from the two edges
      vgl_vector_2d<double > cur_contact = rotated(cur_xdesc->bnd_tangent_left(), vnl_math::pi_over_2);
      vgl_vector_2d<double > next_contact = rotated(next_xdesc->bnd_tangent_right(), -vnl_math::pi_over_2);
      
      // The angle gap is between the two contact shocks
      double angle_gap = signed_angle(cur_contact, next_contact);

      // Split the gap between the two descriptors
      // If one edge is a terminal edge then let it absorb the whole gap. Otherwise
      // split evenly between the two edges
      if (cur_xe->is_terminal_edge())
      {
        // let cur_xe absorb the full gap
        cur_xdesc->set_phi(cur_xdesc->phi() + angle_gap / 2);
        cur_xdesc->set_shock_tangent(cur_xdesc->shock_tangent_angle() + angle_gap/2);
      }
      else if (next_xe->is_terminal_edge())
      {
        // let `next_xe' absorb the full gap
        next_xdesc->set_phi(next_xdesc->phi() + angle_gap/2);
        next_xdesc->set_shock_tangent(next_xdesc->shock_tangent_angle() -angle_gap/2);
      }
      else
      {      
        cur_xdesc->set_phi(cur_xdesc->phi() + angle_gap/4);
        cur_xdesc->set_shock_tangent(cur_xdesc->shock_tangent_angle() + angle_gap/4);

        next_xdesc->set_phi(next_xdesc->phi() + angle_gap/4);
        next_xdesc->set_shock_tangent(next_xdesc->shock_tangent_angle() -angle_gap/4);
      }
    }  
  }

  // last step - update the degree-1 nodes via its adjacent nodes
  xgraph->update_all_degree_1_nodes();
  return;
}







//------------------------------------------------------------------------------
//: Update descriptors around a degree-2 node, given the descriptor of an edge
void dbsksp_xgraph_algos::
update_degree2_node(const dbsksp_xshock_node_sptr& xv, 
                    const dbsksp_xshock_edge_sptr& xe_child, 
                    const dbsksp_xshock_node_descriptor& xdesc)
{
  assert(xv->degree() == 2);
  assert(xe_child->is_vertex(xv));

  // get its descriptor
  dbsksp_xshock_node_descriptor* xdesc_c = xv->descriptor(xe_child);
  xdesc_c->phi_ = xdesc.phi_;
  xdesc_c->psi_ = xdesc.psi_;

  // the opposite edge (parent edge)
  dbsksp_xshock_edge_sptr xe_p = *xv->edges_begin();
  if (xe_p == xe_child)
    xe_p = *(++xv->edges_begin());

  dbsksp_xshock_node_descriptor* xdesc_p = xv->descriptor(xe_p);
  xdesc_p->phi_ = vnl_math::pi - xdesc_c->phi_;
  xdesc_p->psi_ = vnl_math::pi + xdesc_c->psi_;

  // set location and radius (this affects all the descriptors)
  xv->set_pt(xdesc.pt());
  xv->set_radius(xdesc.radius_);

  return;
}




//------------------------------------------------------------------------------
//: Update descriptors around a degree-3 node
void dbsksp_xgraph_algos::
update_degree3_node(const dbsksp_xshock_node_sptr& xv, 
                    const dbsksp_xshock_edge_sptr& xe_parent,
                    const dbsksp_xshock_edge_sptr& xe_child1,
                    const dbsksp_xshock_edge_sptr& xe_child2,
                    const dbsksp_xshock_node_descriptor& xdesc_parent,
                    double phi_child1)
{
  assert(xv->degree() == 3);
  assert(xe_parent->is_vertex(xv));
  assert(xe_child1->is_vertex(xv));
  assert(xe_child2->is_vertex(xv));

  // set descriptor of parent
  dbsksp_xshock_node_descriptor* xdesc_p = xv->descriptor(xe_parent);
  xdesc_p->phi_ = xdesc_parent.phi_;
  xdesc_p->psi_ = xdesc_parent.psi_;

  // set descriptor of child1
  dbsksp_xshock_node_descriptor* xdesc_c1 = xv->descriptor(xe_child1);
  xdesc_c1->phi_ = phi_child1;
  xdesc_c1->psi_ = xdesc_p->psi_ + xdesc_p->phi_ + xdesc_c1->phi_;

  // set descriptor of child2
  dbsksp_xshock_node_descriptor* xdesc_c2 = xv->descriptor(xe_child2);
  xdesc_c2->phi_ = vnl_math::pi - xdesc_p->phi_ - xdesc_c1->phi_;
  xdesc_c2->psi_ = xdesc_c1->psi_ + xdesc_c1->phi_ + xdesc_c2->phi_;

  // set location and radius (this affects all the descriptors)
  xv->set_pt(xdesc_parent.pt());
  xv->set_radius(xdesc_parent.radius_);

  return;
}








//------------------------------------------------------------------------------
//: Compute a list of shapelet to fit in a chain of shock edges
bool dbsksp_xgraph_algos::
fit_shapelet_chain(const dbsksp_xshock_node_sptr& start_node,
    const vcl_vector<dbsksp_xshock_edge_sptr>& path,
    vcl_vector<dbsksp_shapelet_sptr >& list_shapelet)
{
  list_shapelet.clear();
  list_shapelet.reserve(2* path.size());

  // sanity check
  if (path.empty())
    return true;

  vcl_vector<dbsksp_xshock_node_descriptor > list_xdesc;
  list_xdesc.reserve(path.size() + 1);
  
  // move along the edge path and add record descriptors at the break points
  // average if the two consective edges mismatch
  dbsksp_xshock_node_sptr cur_xv = start_node;
  for (unsigned k =0; k < path.size(); ++k)
  {
    dbsksp_xshock_edge_sptr cur_xe = path[k];

    if (!cur_xe->is_vertex(cur_xv))
    {
      vcl_cerr << "\nERROR: The edge path is not a connected chain.\n";
      return false;
    }

    // start descriptor
    dbsksp_xshock_node_descriptor cur_xdesc = *cur_xv->descriptor(cur_xe);

    // insert both descriptors in if there is a gap
    if (cur_xv->degree() > 2 && k > 0) // both conditions are necessary to indicate a gap
    {
      dbsksp_xshock_edge_sptr prev_xe = path[k-1];
      dbsksp_xshock_node_descriptor prev_xdesc = cur_xv->descriptor(prev_xe)->opposite_xnode();
      
      list_xdesc.push_back(prev_xdesc);
      list_xdesc.push_back(cur_xdesc);
    }


    //if (cur_xv->degree() > 2 && k > 0) // both conditions are necessary to indicate a gap
    //{
    //  dbsksp_xshock_edge_sptr prev_xe = path[k-1];
    //  dbsksp_xshock_node_descriptor prev_xdesc = cur_xv->descriptor(prev_xe)->opposite_xnode();
    //  
    //  // angle gap between two left contact shocks
    //  vgl_vector_2d<double > cur_n_left = rotated(cur_xdesc.shock_tangent(), cur_xdesc.phi());
    //  vgl_vector_2d<double > prev_n_left = rotated(prev_xdesc.shock_tangent(), prev_xdesc.phi());
    //  double gap_left = signed_angle(cur_n_left, prev_n_left);

    //  // average two left contact shocks
    //  vgl_vector_2d<double > average_n_left = rotated(cur_n_left, gap_left / 2);

    //  // angle gap between two right contact shocks
    //  vgl_vector_2d<double > cur_n_right = rotated(cur_xdesc.shock_tangent(), -cur_xdesc.phi());
    //  vgl_vector_2d<double > prev_n_right = rotated(prev_xdesc.shock_tangent(), - prev_xdesc.phi());
    //  double gap_right = signed_angle(cur_n_right, prev_n_right);

    //  // average two right contact shocks
    //  vgl_vector_2d<double > average_n_right = rotated(cur_n_right, gap_right / 2);
    //  
    //  // reset the xshock descriptor to the average descriptor
    //  vgl_point_2d<double > pt = cur_xdesc.pt();
    //  normalize(average_n_left);
    //  normalize(average_n_right);
    //  vgl_point_2d<double > pt_left = pt + cur_xdesc.radius() * average_n_left;
    //  vgl_point_2d<double > pt_right = pt + cur_xdesc.radius() * average_n_right;
    //  
    //  cur_xdesc = dbsksp_xshock_node_descriptor(pt, pt_left, pt_right);
    //  list_xdesc.push_back(cur_xdesc);
    //}

    else
    {
      list_xdesc.push_back(cur_xdesc);
    }

    // update the running node
    cur_xv = cur_xe->opposite(cur_xv);
  }

  // the last descriptor
  list_xdesc.push_back(cur_xv->descriptor(path.back())->opposite_xnode());

  // Now fit the shapelets to between the xnode descriptors
  for (unsigned k =1; k < list_xdesc.size(); ++k)
  {
    dbsksp_xshock_node_descriptor prev_xdesc = list_xdesc[k-1];
    dbsksp_xshock_node_descriptor cur_xdesc = list_xdesc[k];

    double d = vgl_distance(prev_xdesc.pt(), cur_xdesc.pt());

    // Special treatment when this is a terminal edge
    // Only one shapelet is necessary
    if (d < 1e-12)
    {
      //// The opposite of the leaf node
      //dbsksp_xshock_node_sptr xv_not_leaf = (cur_xe->source()->degree() == 1) ?
      //  cur_xe->target() : cur_xe->source();

      if (dbnl_math_near(prev_xdesc.phi(), vnl_math::pi, 1e-12)) // prev_xdesc is leaf node
      {
        dbsksp_shapelet_sptr terminal_xfrag = dbsksp_new_terminal_shapelet(cur_xdesc.x(), cur_xdesc.y(),
          cur_xdesc.shock_tangent_angle(), cur_xdesc.radius(), cur_xdesc.phi());
        list_shapelet.push_back(terminal_xfrag->reversed_dir());
      }
      else if (dbnl_math_near(cur_xdesc.phi(), 0.0, 1e-12))
      {

        dbsksp_shapelet_sptr terminal_xfrag = dbsksp_new_terminal_shapelet(prev_xdesc.x(), prev_xdesc.y(),
          prev_xdesc.shock_tangent_angle(), prev_xdesc.radius(), prev_xdesc.phi());
        list_shapelet.push_back(terminal_xfrag);
      }
      else
      {
        dbsksp_shapelet_sptr sh = dbsksp_interp_xfrag_with_zero_chord_using_one_shapelet(prev_xdesc, cur_xdesc);
        if (sh)
        {
          list_shapelet.push_back(sh);
        }
      }
    }
    else
    {
      // Fit shapelets to this fragment
      vcl_vector<dbsksp_shapelet_sptr > temp;
      dbsksp_interp_xfrag_with_max_three_shapelets(list_xdesc[k-1], list_xdesc[k], temp);
      list_shapelet.insert(list_shapelet.end(), temp.begin(), temp.end());
    }
  }
  return true;
}





//------------------------------------------------------------------------------
//: Sample a shock path
// sample_ds is the sampling rate along the chordal curve
void dbsksp_xgraph_algos::
compute_xsamples(const dbsksp_xshock_node_sptr& start_node,
                 const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                 double sample_ds,
                 vcl_vector<dbsksp_xshock_node_descriptor >& list_sample_xdesc)
{
  // sanitize old data
  list_sample_xdesc.clear();

  if (path.empty())
    return;

  if (sample_ds <= 0)
    return;

  // estimate the number of intervals from the sampling arc length along the chordal curve
  if (!path.front()->is_vertex(start_node))
    return;

  bool is_valid_path = true;
  double total_length = 0;
  dbsksp_xshock_node_sptr cur_xv = start_node;
  for (unsigned k =0; k < path.size(); ++k)
  {
    dbsksp_xshock_edge_sptr xe = path[k];
    dbsksp_xshock_node_sptr next_xv = xe->opposite(cur_xv);

    dbsksp_xshock_node_descriptor cur_xdesc = *cur_xv->descriptor(xe);
    dbsksp_xshock_node_descriptor next_xdesc = next_xv->descriptor(xe)->opposite_xnode();

    // Approximate length as average of two boundary length
    dbsksp_xshock_fragment xfrag(cur_xdesc, next_xdesc);
    dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
    dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

    if (left_bnd.is_consistent() && right_bnd.is_consistent())
    {
      total_length += 0.5 * (left_bnd.len() + right_bnd.len());
    }
    else
    {
      // something is very wrong.
      is_valid_path = false;
      break;
    }


    //// Approximate the length of the chordal curve (connecting mid-point of symmetric point-pairs)
    //// with the length of the biarc interpolating between two end chordal points
    //dbgl_biarc chordal(cur_xdesc.bnd_mid_pt(), cur_xdesc.shock_tangent(), 
    //  next_xdesc.bnd_mid_pt(), next_xdesc.shock_tangent());

    //if (chordal.is_consistent())
    //{
    //  total_length += chordal.len();
    //}
    //else
    //{
    //  // something is very wrong.
    //  is_valid_path = false;
    //  break;
    //}

    cur_xv = next_xv;
  }

  if (!is_valid_path)
  {
    // take only the end-samples
    dbsksp_xgraph_algos::compute_xsamples(1, start_node, path, list_sample_xdesc);
  }
  else
  {

    // Estimate number of intervals - we need at least one interval
    int num_intervals = vnl_math::rnd(total_length / sample_ds);
    num_intervals = vnl_math::max(1, num_intervals);

    // Call a similar function that computes with pre-defined number of samples
    dbsksp_xgraph_algos::compute_xsamples(num_intervals, start_node, path, list_sample_xdesc);
  }
  return;
}










//------------------------------------------------------------------------------
//: Compute extrinsic samples of a shock fragment
// The samples are equally spaced along the chordal curve
// ``num_intervals'' is the number of intervals the path is divided into, which
// is one less than the total number of samples.
// Note that ``num_intervals'' is only suggestive. The returned result
// may be less that requested.
void dbsksp_xgraph_algos::
compute_xsamples(int num_intervals, const dbsksp_xshock_node_sptr& start_node,
                 const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                 vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)
{
  ////
  //dbsksp_xgraph_algos::compute_xsamples_by_fitting_shapelets(num_intervals,
  //  start_node, path, list_xsample);

  dbsksp_xgraph_algos::compute_xsamples_by_sampling_longer_bnd_arc(num_intervals,
    start_node, path, list_xsample);

  return;
}



//------------------------------------------------------------------------------
//: Compute extrinsic samples of a shock fragment by first fitting shapelets to the path
void dbsksp_xgraph_algos::
compute_xsamples_by_fitting_shapelets(int num_intervals,
                                      const dbsksp_xshock_node_sptr& start_node,
                                      const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                      vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)

{
  // sanitize old data
  list_xsample.clear();

  if (num_intervals == 0)
    return;


  //traverse through the path, compute the shapelets for each edge
  vcl_vector<dbsksp_shapelet_sptr > list_shapelet;
  dbsksp_xgraph_algos::fit_shapelet_chain(start_node, path, list_shapelet);

  // Compute a coarse sampling of the shapelets to have an estimate of t vs. arc length
  // Uniform sampling of t in [0, 1]
  int num_coarse_intervals = 50;
  vcl_vector<double > list_t_per_shapelet(num_coarse_intervals+1, 0);
  for (int i =0; i <= num_coarse_intervals; ++i)
  {
    list_t_per_shapelet[i] = double (i) / num_coarse_intervals;
  }

  vcl_vector<dbsksp_xshock_node_descriptor > list_xdesc;
  list_xdesc.reserve(list_shapelet.size() * list_t_per_shapelet.size());
  for (unsigned k =0; k < list_shapelet.size(); ++k)
  {
    vcl_vector<dbsksp_xshock_node_descriptor > temp;
    list_shapelet[k]->compute_xshock_samples(list_t_per_shapelet, temp);
    list_xdesc.insert(list_xdesc.end(), temp.begin(), temp.end());
  }



  // Construct a chordal line of the fragments via the samples
  vcl_vector<vgl_point_2d<double > > chordal_line;
  chordal_line.reserve(list_xdesc.size());
  for (unsigned i =0; i < list_xdesc.size(); ++i)
  {
    dbsksp_xshock_node_descriptor xdesc = list_xdesc[i];
    chordal_line.push_back(centre(xdesc.bnd_pt_left(), xdesc.bnd_pt_right()));
  }

  // Compute arclength along the chordal line
  vnl_vector<double > chordal_len(chordal_line.size(), 0);
  double running_len = 0;
  for (unsigned i =1; i < chordal_line.size(); ++i)
  {
    running_len += vgl_distance(chordal_line[i], chordal_line[i-1]);
    chordal_len[i] = running_len;
  }

  // Compute parameter t associated with the chordal arc length
  // Each shapelet corresponds to a 't' interval of 1
  // Shapelet 0: [0, 1]
  // Shapelet 1: (1, 2]
  vnl_vector<double > chordal_t(chordal_line.size(), 0);
  for (unsigned k =0; k < list_shapelet.size(); ++k)
  {
    int idx = k * list_t_per_shapelet.size();
    for (unsigned i =0; i < list_t_per_shapelet.size(); ++i)
    {
      chordal_t[idx + i] = k + list_t_per_shapelet[i];
    }
  }

  // We will interpolate these sample data to obtain a uniform sampling of the fragment
  double max_length = chordal_len.get(chordal_len.size()-1);
  
  //// we need at least one interval
  //int num_intervals = vnl_math::rnd(max_length / sample_ds);
  //num_intervals = vnl_math::max(1, num_intervals);
  
  vnl_vector<double > list_sample_length(num_intervals+1, 0);
  for (unsigned k =0; k < list_sample_length.size(); ++k)
  {
    list_sample_length[k] = k * max_length / num_intervals;
  }


  // Compute the parameter t associated with the desired sample arc length
  vnl_vector<double > list_sample_t(num_intervals+1, 0);

  // Linear interpolation using closest point
  {
    vnl_vector<double >& x0 = chordal_len;
    vnl_vector<double >& y0 = chordal_t;
    vnl_vector<double >& x = list_sample_length;
    vnl_vector<double >& y = list_sample_t;

    assert (x[0] >= x0[0]);

    // Compute the slope at each point
    unsigned num_data_pts = x0.size();
    vnl_vector<double > slope(num_data_pts, 0);
    for (unsigned k =1; k < num_data_pts; ++k)
    {
      slope[k-1] = (y0[k] - y0[k-1]) / (x0[k] - x0[k-1] + 1e-12); // avoid dividing by 0
    }
    // beyond the last point is extrapolation
    if (slope.size() > 1)
    {
      slope[num_data_pts-1] = slope[num_data_pts-2];
    }

    // Now we're ready to interpolate
    unsigned num_sample_pts = list_sample_length.size();
    y.set_size(num_sample_pts);

    // Assuming the query 'x' is increasing, keep last-used idx for speed-up
    unsigned last_idx = 1;
    for (unsigned k =0; k < num_sample_pts; ++k)
    {
      double s = x[k];

      // find the smallest data point that is greater than `s'
      unsigned idx = last_idx;
      for (;idx < x0.size() && x0[idx] < s; ++idx);

      // compute value for sample 's' using the previous data point
      last_idx = idx;
      --idx;
      y[k] = y0[idx] + slope[idx] * (s - x0[idx]);
    }
  }

  // Now compute the xshock samples from the parameter 't'
  // Note that each interval of length 1 corresponds to a shapelet
  list_xsample.reserve(list_sample_t.size());

  // Group the 't' by their integer parts. Each group corresponds to a shapelet
  vcl_vector<vcl_vector<double > > list_t_group(list_shapelet.size());
  for (unsigned k =0; k < list_sample_t.size(); ++k)
  {
    double t = list_sample_t[k];
    int shapelet_idx = vnl_math::floor(t);
    double shapelet_t = t - shapelet_idx;

    if (shapelet_idx < int(list_t_group.size()))
    {
      list_t_group[shapelet_idx].push_back(shapelet_t);
    }
    else if (shapelet_idx == list_t_group.size())
    {
      list_t_group.back().push_back(1.0);
    }
  }

  // Compute the samples
  for (unsigned k =0; k < list_t_group.size(); ++k)
  {
    vcl_vector<dbsksp_xshock_node_descriptor > temp;
    list_shapelet[k]->compute_xshock_samples(list_t_group[k], temp);
    list_xsample.insert(list_xsample.end(), temp.begin(), temp.end());
  }

  return;
}








//------------------------------------------------------------------------------
//: Compute extrinsic samples of a shock fragment by moving along the boundary
// biarc and compute the symmetry points on the opposite boundary curve
void dbsksp_xgraph_algos::
compute_xsamples_by_sampling_longer_bnd_arc(int num_intervals,
                                            const dbsksp_xshock_node_sptr& start_node,
                                            const vcl_vector<dbsksp_xshock_edge_sptr>& path,
                                            vcl_vector<dbsksp_xshock_node_descriptor >& list_xsample)
{
  list_xsample.clear();

  if (path.empty())
    return;

  vcl_vector<dbsksp_xshock_node_descriptor > list_xdesc;
  list_xdesc.reserve(path.size() + 1);

  vcl_vector<dbsksp_xshock_node_sptr > node_list;
  node_list.reserve(path.size() + 1);

  
  // move along the edge path and add record descriptors at the break points
  // average if the two consective edges mismatch
  dbsksp_xshock_node_sptr cur_xv = start_node;
  for (unsigned k =0; k < path.size(); ++k)
  {
    dbsksp_xshock_edge_sptr cur_xe = path[k];

    if (!cur_xe->is_vertex(cur_xv))
    {
      vcl_cerr << "\nERROR: The edge path is not a connected chain.\n";
      return;
    }

    // start descriptor
    dbsksp_xshock_node_descriptor cur_xdesc = *cur_xv->descriptor(cur_xe);

    // Fill in the gap at degree-3 nodes which are at the midle
    if (cur_xv->degree() > 2 && k > 0) // both conditions are necessary to indicate a gap
    {
      dbsksp_xshock_edge_sptr prev_xe = path[k-1];
      dbsksp_xshock_node_descriptor prev_xdesc = cur_xv->descriptor(prev_xe)->opposite_xnode();
      
      // angle gap between two left contact shocks
      vgl_vector_2d<double > cur_n_left = rotated(cur_xdesc.shock_tangent(), cur_xdesc.phi());
      vgl_vector_2d<double > prev_n_left = rotated(prev_xdesc.shock_tangent(), prev_xdesc.phi());
      double gap_left = signed_angle(cur_n_left, prev_n_left);

      // average two left contact shocks
      vgl_vector_2d<double > average_n_left = rotated(cur_n_left, gap_left / 2);

      // angle gap between two right contact shocks
      vgl_vector_2d<double > cur_n_right = rotated(cur_xdesc.shock_tangent(), -cur_xdesc.phi());
      vgl_vector_2d<double > prev_n_right = rotated(prev_xdesc.shock_tangent(), - prev_xdesc.phi());
      double gap_right = signed_angle(cur_n_right, prev_n_right);

      // average two right contact shocks
      vgl_vector_2d<double > average_n_right = rotated(cur_n_right, gap_right / 2);
      
      // reset the xshock descriptor to the average descriptor
      vgl_point_2d<double > pt = cur_xdesc.pt();
      normalize(average_n_left);
      normalize(average_n_right);
      vgl_point_2d<double > pt_left = pt + cur_xdesc.radius() * average_n_left;
      vgl_point_2d<double > pt_right = pt + cur_xdesc.radius() * average_n_right;
      
      cur_xdesc = dbsksp_xshock_node_descriptor(pt, pt_left, pt_right);
    }

    
    
    list_xdesc.push_back(cur_xdesc);
    node_list.push_back(cur_xv);
    

    // update the running node
    cur_xv = cur_xe->opposite(cur_xv);
  }

  // the last descriptor
  list_xdesc.push_back(cur_xv->descriptor(path.back())->opposite_xnode());
  node_list.push_back(cur_xv);

  // remove descriptors corresponding to degree-1 node
  {
    vcl_vector<dbsksp_xshock_node_descriptor > temp = list_xdesc;
    list_xdesc.clear();
    for (unsigned i =0; i < node_list.size(); ++i)
    {
      if (node_list[i]->degree() == 1)
      {
        continue;
      }
      list_xdesc.push_back(temp[i]);
    }    
  }


  //// Space the xdesc'es out a little bit, make sure no adjacent xsamples coincide
  //for (unsigned i =1; i < list_xdesc.size(); ++i)
  //{
  //  dbsksp_xshock_node_descriptor& prev_xdesc = list_xdesc[i-1];
  //  dbsksp_xshock_node_descriptor& cur_xdesc = list_xdesc[i];
  //  
  //  // if the xsamples have the same shock point, pertub a little bit to avoid degeneracy
  //  if (prev_xdesc.pt() == cur_xdesc.pt())
  //  {
  //    // move a tiny bit along the tangent vector.

  //    // preserve location of end points
  //    if ( i == list_xdesc.size()-1 )
  //    {
  //      vgl_vector_2d<double > t = prev_xdesc.shock_tangent();
  //      prev_xdesc.set_pt(prev_xdesc.pt() - 1e-2 * t);
  //    }
  //    else
  //    {  
  //      vgl_vector_2d<double > t = cur_xdesc.shock_tangent();
  //      cur_xdesc.set_pt(cur_xdesc.pt() + 1e-2 * t);
  //    }
  //  }
  //}



  // Handle special case - only 1 interval - no sampling necessary
  if (num_intervals == 1)
  {
    list_xsample.push_back(list_xdesc.front());
    list_xsample.push_back(list_xdesc.back());
    return;
  }
  
  // sum up the chordal length
  double total_length = 0;
  vcl_vector<double > run_lengths;
  run_lengths.reserve(list_xdesc.size());
  run_lengths.push_back(total_length);

  for (unsigned k =1; k < list_xdesc.size(); ++k)
  {
    dbsksp_xshock_node_descriptor start = list_xdesc[k-1];
    dbsksp_xshock_node_descriptor end = list_xdesc[k];

    dbsksp_xshock_fragment xfrag(start, end);
    dbgl_biarc left_bnd = xfrag.bnd_left_as_biarc();
    dbgl_biarc right_bnd = xfrag.bnd_right_as_biarc();

    if (left_bnd.is_consistent() && right_bnd.is_consistent())
    {
      total_length += 0.5 * (left_bnd.len() + right_bnd.len());
    }
    run_lengths.push_back(total_length);

    ////
    //dbgl_biarc chordal(start.bnd_mid_pt(), start.shock_tangent(), end.bnd_mid_pt(), end.shock_tangent());
    //total_length += chordal.len();
  }


  // Now compute samples between consecutive anchor points
  for (unsigned k =1; k < list_xdesc.size(); ++k)
  {
    dbsksp_xshock_node_descriptor start = list_xdesc[k-1];
    dbsksp_xshock_node_descriptor end = list_xdesc[k];

    // number of intervals for this segment
    //dbgl_biarc chordal(start.bnd_mid_pt(), start.shock_tangent(), end.bnd_mid_pt(), end.shock_tangent()); 
    //int num_segments = vnl_math::rnd(num_intervals * chordal.len() / total_length);
    //num_segments = vnl_math::max(1, num_segments);

    double len = run_lengths[k] - run_lengths[k-1];
    int num_segments = vnl_math::rnd(num_intervals * len / total_length);
    num_segments = vnl_math::max(1, num_segments);

    // temp_xsamples only contains middle-samples, not the samples at the two end points
    vcl_vector<dbsksp_xshock_node_descriptor > temp_xsamples;
    dbsksp_compute_middle_xsamples_by_sampling_longer_bnd_biarc(num_segments, start, end, temp_xsamples);

    list_xsample.push_back(start);
    list_xsample.insert(list_xsample.end(), temp_xsamples.begin(), temp_xsamples.end());  
  }
  list_xsample.push_back(list_xdesc.back());
  return;
}










