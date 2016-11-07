// This is file seg/dbsks/dbsks_xshock_detector.cxx

//:
// \file

#include <en250_project/dbsks_xshock_detector.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_dp.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_xshock_wcm_likelihood.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbil/algo/dbil_gradient_color.h>
#include <vnl/vnl_math.h>
#include <vcl_utility.h>

// ============================================================================
// dbsks_xshock_detector
// ============================================================================


// -----------------------------------------------------------------------------
//: Detect the existence of the given extrinsic shock graph in the given image
void dbsks_xshock_detector::
detect(const vgl_box_2d<int >& window)
{
  // 0. Preliminary checks: root vertex, major child edge, etc
  // assign root vertex and major child edge from geometric model, where available
  if (this->xgraph_geom_)
  {
    this->root_vid_ = this->xgraph_geom_->root_vid_;
    this->major_child_eid_ = this->xgraph_geom_->major_child_eid_;
  }

  // root vertex
  dbsksp_xshock_node_sptr xv_root = this->xgraph()->node_from_id(this->root_vid_);
  if (!xv_root)
  {
    vcl_cout << "ERROR: There is no vertex with id=root_vid=" << this->root_vid_ 
      << ". Please double check. Quit now." << vcl_endl;
    return;
  }
  this->xgraph()->compute_vertex_depths(this->root_vid_);

  // major child edge
  dbsksp_xshock_edge_sptr major_child_edge = this->xgraph()->edge_from_id(this->major_child_eid_);
  if (!major_child_edge)
  {
    vcl_cout << "ERROR: There is no edge with major_child_eid=" << this->major_child_eid_
      << ". Please double check. Quit now." << vcl_endl;
    return;
  }

  // 1. Prepare space for solution
  this->list_solutions_.clear();
  this->list_solution_costs_.clear();
  this->list_solution_real_costs_.clear();

  // 2. Generate grids for the nodes
  this->build_xnode_grid(window);
  
  // 3. Run DP on to find the best configuration
  dbsks_xshock_dp xshock_dp;
  xshock_dp.set_graph(this->xgraph());

  // \todo variables to be removed
  // xshock_dp.ocm = this->ocm_;
  // xshock_dp.shotton_ocm = this->shotton_ocm_;
  // xshock_dp.gray_ocm = this->gray_ocm_;
  // xshock_dp.ccm = this->ccm_;
  // xshock_dp.xgraph_ccm = this->xgraph_ccm_;
  // xshock_dp.biarc_sampler = this->biarc_sampler_;


  
  // variables to keep
  xshock_dp.root_vid_ = this->root_vid_;
  xshock_dp.major_child_eid_ = this->major_child_eid_;
  
  xshock_dp.map_xnode_grid = &(this->map_xnode_grid_);
  xshock_dp.xgraph_geom = this->xgraph_geom_;
  xshock_dp.set_xshock_likelihood(this->app_like_);


  //////////////////////////////////////////////////////////////////////
  //// \hack
  //// to temporary compute cost of one xgraph
  //bool hack = true;
  //if (hack)
  //{
  //  vcl_string temp_xshock_file = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph/applelogos_four.xgraph.3.prototype1.xml";
  //  dbsksp_xshock_graph_sptr temp_xgraph = 0;
  //  x_read(temp_xshock_file, temp_xgraph);
  //  temp_xgraph->compute_vertex_depths(this->root_vid_);
  //  //temp_xgraph->scale_up(0, 0, 2);
  //  double temp_wcm_confidence1 = this->app_like_->f_whole_contour(temp_xgraph, vcl_vector<unsigned >(), true);
  //  double temp_wcm_confidence = this->app_like_->f_whole_contour(temp_xgraph, vcl_vector<unsigned >(1, 10), true);
  //  vcl_cout << "temp_wcm_confidence = " << temp_wcm_confidence << "\n";

  //  return;

  //  
  //}

  ////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////
  //4. THE MAIN FUNCTION /////////////////////////////////////////////////////////
  xshock_dp.init_cost_grids();
  xshock_dp.optimize();
  //////////////////////////////////////////////////////////////////////////////
  

  //5. Collect detection results

  // retrieve the solution shock graphs
  unsigned num_sols = xshock_dp.list_opt_xgraph_state.size();
  for (unsigned k =0; k < num_sols; ++k)
  {
    vcl_map<unsigned, int >& map_node_state = xshock_dp.list_opt_xgraph_state[k];
    dbsksp_xshock_graph_sptr new_graph = this->reconstruct_xgraph(map_node_state);

    // only accept as a solution when it corresponds to a real, legal xgraph
    // \todo check legality of "new_graph"
    if (new_graph)
    {
      this->list_solution_costs_.push_back(xshock_dp.list_opt_cost[k]);
      this->list_solutions_.push_back(new_graph);
    
      // Real cost is the whole-contour matching cost
      new_graph->compute_vertex_depths(this->root_vid_);
      float real_cost = float(-this->app_like_->f_whole_contour(new_graph, vcl_vector<unsigned >(), false));
      vcl_cout<<"real_cost : "<<real_cost<<vcl_endl;
      this->list_solution_real_costs_.push_back(real_cost);
    }
    else
    {
      vcl_cout << "\nERROR: couldn't reconstruct xraph for solution k=" << k 
        << ". DP cost = " << xshock_dp.list_opt_cost[k] << vcl_endl;
      continue;
    }
  } // solution

  return;
}




// -----------------------------------------------------------------------------
//: Build the xnode grid
void dbsks_xshock_detector::
build_xnode_grid(const vgl_box_2d<int >& window)
{
  this->map_xnode_grid_.clear();

  // different methods to construct the grid. These evolve over time. We just hard-code for now
  int generate_xnode_grid_option = 2; 

  // option 1: no other information is available other than the input graph
  if (generate_xnode_grid_option == 1)
  {
    this->build_xnode_grid_using_only_input_xgraph(window);
  }

  // option 2: geometric model is available
  else if (generate_xnode_grid_option == 2)
  {
    this->build_xnode_grid_using_xgraph_geom_model(window);
  }
  else
  {
    vcl_cout << "\nERROR: unknown generate_xnode_grid_option= " << generate_xnode_grid_option << vcl_endl;
    return;
  } // else

  return;
}


// -----------------------------------------------------------------------------
//: Build xnode grid using input xgraph as the only available info
// All parameters are heuristic, hard-coded.
void dbsks_xshock_detector::
build_xnode_grid_using_only_input_xgraph(const vgl_box_2d<int >& window)
{
  // clean up
  this->map_xnode_grid_.clear();

  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // set parameters for the grid
    dbsks_xnode_grid_params params;
    params.step_x = 8;
    params.num_x = vnl_math_rnd(window.width()/params.step_x); // allow some padding

    // if the image is too big, we only consider the center portion of 512 pixels
    params.num_x = vnl_math_min(params.num_x, 65);
    params.min_x = vnl_math_rnd(window.centroid_x() - params.step_x*(params.num_x-1)/2 ); // centering

    params.step_y = 8;
    params.num_y = vnl_math_rnd(window.height()/params.step_y); // allow some padding

    // if the image is too big, only consider the center portion of 512 pixels
    params.num_y = vnl_math_min(params.num_y, 65);
    params.min_y = vnl_math_rnd(window.centroid_y() - params.step_y*(params.num_y-1)/2 ); // centering

    params.step_psi = vnl_math::pi / 8;
    params.num_psi = 16;
    params.min_psi = 0;

    params.step_phi0 = vnl_math::pi / 16;
    params.num_phi0 = 9;
    params.min_phi0 = vnl_math::pi_over_2 - params.step_phi0 * (params.num_phi0-1)/2;

    params.r0 = xv->radius();
    params.step_log2r = 0.1;
    params.num_log2r = 5;
    params.min_log2r = -(params.num_log2r-1) * params.step_log2r /2;

    if (xv->degree() == 3)
    {
      params.step_phi1 = vnl_math::pi / 9;
      params.num_phi1 = 5;
      params.min_phi1 = - params.step_phi1 * (params.num_phi1-1)/2;
    }
    else
    {
      params.step_phi1 = vnl_math::pi / 9;
      params.num_phi1 = 1;
      params.min_phi1 = 0;
    }

    dbsks_xnode_grid xnode_grid;
    xnode_grid.compute(params);

    this->map_xnode_grid_.insert(vcl_make_pair(xv->id(), xnode_grid));
  }
  return;
}



// -----------------------------------------------------------------------------
//: Build xnode grid using xgraph geometric model (assume available)
void dbsks_xshock_detector::
build_xnode_grid_using_xgraph_geom_model(const vgl_box_2d<int >& window)
{
  // clean up
  this->map_xnode_grid_.clear();

  if (!this->xgraph_geom_ || this->xgraph_geom_->map_node2geom_.empty())
  {
    vcl_cout << "\nERROR: xgraph geometric model is either not defined or empty.\n";
    return;
  }

  // size of the reference graph
  double cur_graph_size = vcl_sqrt(this->xgraph()->area());
  
  // set root id, etc
  this->root_vid_ = this->xgraph_geom_->root_vid_;
  this->major_child_eid_ = this->xgraph_geom_->major_child_eid_;

  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit)
  {
    // >> retrieve model information
    dbsksp_xshock_node_sptr xv = *vit;

    // ignore degree-1 nodes
    if (xv->degree()==1)
      continue;

    dbsks_xnode_geom_model_sptr xnode_geom = this->xgraph_geom_->map_node2geom_[xv->id()];
    double min_psi, max_psi;
    double min_radius, max_radius;
    double min_phi, max_phi;
    double min_phi_diff, max_phi_diff;
    double graph_size;
    xnode_geom->get_param_range(min_psi, max_psi, min_radius, max_radius, 
      min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);

    // scale up the radii up to match with current graph's size
    min_radius *= cur_graph_size / graph_size;
    max_radius *= cur_graph_size / graph_size;

    // set parameters for the grid
    // we insist on covering the full image spatially

    //>> x
    dbsks_xnode_grid_params params;
    params.step_x = 8;
    params.num_x = vnl_math_rnd(window.width()/params.step_x); // allow some padding

    // if the image is too big, we only consider the center portion of 512 pixels
    params.num_x = vnl_math_min(params.num_x, 65);
    params.num_x = vnl_math_max(params.num_x, 1); //>> make sure we get at least 1 sample

    params.min_x = vnl_math_rnd(window.centroid_x() - params.step_x*(params.num_x-1)/2 ); // centering

    //>> y
    params.step_y = 8;
    params.num_y = vnl_math_rnd(window.height()/params.step_y); // allow some padding
    // if the image is too big, only consider the center portion of 512 pixels
    params.num_y = vnl_math_min(params.num_y, 65);
    params.num_y = vnl_math_max(params.num_y, 1);

    params.min_y = vnl_math_rnd(window.centroid_y() - params.step_y*(params.num_y-1)/2 ); // centering

    //>> psi
    params.step_psi = vnl_math::pi / 8;
    double range_psi = vnl_math_min((max_psi-min_psi)*1.5, 2*vnl_math::pi);
    
    params.num_psi = vnl_math_rnd(range_psi / params.step_psi);
    params.num_psi = vnl_math_max(params.num_psi, 1);

    params.min_psi = (max_psi+min_psi)/2 - params.step_psi*(params.num_psi-1)/2;

    //>> phi0
    params.step_phi0 = vnl_math::pi / 16;
    double range_phi0 = vnl_math_min((max_phi-min_phi)*1.5, vnl_math::pi);
    
    params.num_phi0 = vnl_math_rnd( range_phi0 / params.step_phi0);
    params.num_phi0 = vnl_math_max(params.num_phi0, 1); //>> make sure we get at least 1 sample
    
    params.min_phi0 = (max_phi+min_phi)/2 - params.step_phi0 * (params.num_phi0-1)/2;

    //>> radius
    params.r0 = vcl_sqrt(max_radius*min_radius);
    double range_radius = 1.5*vcl_log(max_radius/min_radius) / vnl_math::ln2;
    params.step_log2r = 0.2;
    
    params.num_log2r = vnl_math_rnd( range_radius / params.step_log2r);
    params.num_log2r = vnl_math_max(params.num_log2r, 1);

    params.min_log2r = -(params.num_log2r-1)*params.step_log2r /2;

    if (xv->degree() == 3)
    {
      params.step_phi1 = vnl_math::pi / 16; // old - 9;
      double range_phi1 = vnl_math_min((max_phi_diff-min_phi_diff)*1.5, vnl_math::pi);
      
      params.num_phi1 = vnl_math_rnd(1.5*vnl_math_rnd(range_phi1 / params.step_phi1));
      params.num_phi1 = vnl_math_max(params.num_phi1, 1);

      params.min_phi1 = -params.step_phi1*(params.num_phi1-1)/2;
    }
    else
    {
      params.step_phi1 = vnl_math::pi / 16; // old - 9;
      params.num_phi1 = 1;
      params.min_phi1 = 0;
    }

    dbsks_xnode_grid xnode_grid;
    xnode_grid.compute(params);

    this->map_xnode_grid_.insert(vcl_make_pair(xv->id(), xnode_grid));
  }
  return;
}




















// -----------------------------------------------------------------------------
//: Reconstruct an xshock graph from a graph configuration
dbsksp_xshock_graph_sptr dbsks_xshock_detector::
reconstruct_xgraph(const vcl_map<unsigned, int >& map_xgraph_vid_to_state)
{
  // create a duplicate graph
  dbsksp_xshock_graph_sptr new_graph = new dbsksp_xshock_graph(*this->xgraph());
  new_graph->compute_vertex_depths(this->root_vid_);

  // set properties for the nodes
  for (vcl_map<unsigned, int >::const_iterator it = map_xgraph_vid_to_state.begin(); it != 
    map_xgraph_vid_to_state.end(); ++it)
  {
    unsigned vid = it->first;
    dbsksp_xshock_node_sptr xv = new_graph->node_from_id(vid);
    dbsks_xnode_grid& grid = this->map_xnode_grid_[vid];

    // degree-2
    if (xv->degree() == 2)
    {
      int i_x, i_y, i_psi, i_phi0, i_r;
      if (!grid.linear_to_grid(it->second, i_x, i_y, i_psi, i_phi0, i_r))
      {
        vcl_cout << "ERROR: point not in the grid.\n";
        return 0;
      };
      dbsksp_xshock_node_descriptor xdesc = grid.xdesc(i_x, i_y, i_psi, i_phi0, i_r);

      // Determine child edge
      dbsksp_xshock_edge_sptr xe_c = 0;
      
      // special treatment for the root node because it has 2 children
      if (xv->depth() == 0)
      {
        xe_c = new_graph->edge_from_id(this->major_child_eid_);
      }
      // for other cases, the child edge is just, well, the opposite of the parent edge
      else
      {
        xe_c = new_graph->cyclic_adj_succ(xv->parent_edge(), xv);
      }

      // update the node propery
      dbsks_update_degree2_node(xv, xe_c, xdesc);
    }

    // degree -3 nodes
    else if (xv->degree() == 3)
    {
      int i_x, i_y, i_psi, i_phi0, i_r, i_phi1;
      if (!grid.linear_to_grid(it->second, i_x, i_y, i_psi, i_phi0, i_r, i_phi1))
      {
        vcl_cout << "ERROR: node state not in the grid.\n";
        return 0;
      };


      // Determine the parent edge
      dbsksp_xshock_edge_sptr parent_edge = 0;

      // special treatment for the root node
      if (xv->id() == this->root_vid_)
      {
        dbsksp_xshock_edge_sptr major_child_edge = new_graph->edge_from_id(this->major_child_eid_);
        parent_edge = new_graph->cyclic_adj_pred(major_child_edge, xv);
      }
      else
      {
        parent_edge = xv->parent_edge();
      }
      
      if (parent_edge == 0)
      {
        vcl_cout << "ERROR: couldn't identify the parent edge. "
          << "Something is seriously wrong.\n";
        return 0;
      }

      // determine the two child edges
      dbsksp_xshock_edge_sptr child_edge1 = new_graph->cyclic_adj_succ(parent_edge, xv);
      dbsksp_xshock_edge_sptr child_edge2 = new_graph->cyclic_adj_succ(child_edge1, xv);

      // look up the node descriptors
      dbsksp_xshock_node_descriptor xdesc_parent = 
        grid.xdesc(i_x, i_y, i_psi, i_phi0, i_r).opposite_xnode();

      double phi0 = grid.phi0_[i_phi0];
      double phi1 = grid.phi1_[i_phi1];
      double phi_child1 = (phi1 + phi0) / 2;

      // update the node descriptors
      dbsks_update_degree3_node(xv, parent_edge, child_edge1, child_edge2, 
        xdesc_parent, phi_child1);
    }
    else
    {
      vcl_cout << "ERROR: degree not 2 or 3. Can't handle.\n";
      return 0;
    }
  }
  new_graph->update_all_degree_1_nodes();

  return new_graph;
}








