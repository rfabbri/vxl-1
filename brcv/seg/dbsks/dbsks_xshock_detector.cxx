// This is file seg/dbsks/dbsks_xshock_detector.cxx

//:
// \file

#include "dbsks_xshock_detector.h"


#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_dp.h>
#include <dbsks/dbsks_xshock_dp_new.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_xshock_likelihood.h>


#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>

#include <dbil/algo/dbil_gradient_color.h>
#include <vnl/vnl_math.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>

// ============================================================================
// dbsks_xshock_detector
// ============================================================================


// -----------------------------------------------------------------------------
//: Detect the existence of the given extrinsic shock graph in the given image
void dbsks_xshock_detector::
detect(const vgl_box_2d<int >& window, float min_acceptable_confidence)
{
  vcl_cout << "\n> Detecting xgraph in window " << window << vcl_endl;
  
  // 0) Retrieive root vertex from geometric model
  if (!this->xgraph_geom_)
    return;
  this->root_vid_ = this->xgraph_geom_->root_vid();
  this->major_child_eid_ = this->xgraph_geom_->major_child_eid();
  

  //> root vertex
  dbsksp_xshock_node_sptr xv_root = this->xgraph()->node_from_id(this->root_vid_);
  if (!xv_root)
  {
    vcl_cout << "\nERROR: Could not find root vertex, root_vid=" << this->root_vid_ 
      << "\nQuit now.\n";
    return;
  }
  this->xgraph()->compute_vertex_depths(this->root_vid_);

  //> major child edge
  dbsksp_xshock_edge_sptr major_child_edge = this->xgraph()->edge_from_id(this->major_child_eid_);
  if (!major_child_edge)
  {
    vcl_cout << "ERROR: Could not find major child edge, major_child_eid=" << this->major_child_eid_
      << "\nQuit now.\n";
    return;
  }

  // 1) Prepare space for solution
  this->list_solutions_.clear();
  this->list_solution_costs_.clear();
  this->list_solution_real_costs_.clear();

  // 2) Generate grids for the nodes
  this->build_xnode_grid(window);
  


  ////
  //bool hacking = false;
  //if (hacking)
  //{
  //  vcl_string folder = "V:/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-experiments/exp_103-swans_on_ethz-v1_05/swans_prototype_0+swans_aal/scale_91/ROI_0_0_255_239/";
  //  vcl_string fname = "swans_prototype_0+swans_aal+ROI_0_0_255_239.0.xml";
  //  dbsksp_xshock_graph_sptr xg = 0;
  //  x_read(folder + fname, xg);

  //  xg->compute_vertex_depths(this->root_vid_);

  //  float xg_cost = float(-this->xshock_likelihood_->loglike_xgraph(xg, vcl_vector<unsigned >(), true));

  //  vcl_cout << "xg_cost = " << xg_cost << "\n";
  //}




  // 3. Run DP on to find the best configuration
  //dbsks_xshock_dp xshock_dp;
  dbsks_xshock_dp_new xshock_dp;
  xshock_dp.set_graph(this->xgraph());

  
  // variables to keep
  xshock_dp.root_vid_ = this->root_vid_;
  xshock_dp.major_child_eid_ = this->major_child_eid_;
  xshock_dp.map_xnode_grid = &(this->map_xnode_grid_);
  xshock_dp.xgraph_geom = this->xgraph_geom_;
  xshock_dp.set_xshock_likelihood(this->xshock_likelihood_);
  xshock_dp.set_min_acceptable_confidence(min_acceptable_confidence);

  //4. THE MAIN FUNCTION /////////////////////////////////////////////////////////
  xshock_dp.init_cost_grids();
  xshock_dp.optimize();
  //////////////////////////////////////////////////////////////////////////////
  
  vcl_cout << "\n# detections after DP: " << xshock_dp.list_opt_xgraph_state.size() << "\n";
  vcl_cout << "\nValidate legality of solutions from DP ...\n";

  //5. Collect detection results

  // retrieve the solution shock graphs
  unsigned num_sols = xshock_dp.list_opt_xgraph_state.size();
  for (unsigned k =0; k < num_sols; ++k)
  {
    vcl_map<unsigned, int >& map_node_state = xshock_dp.list_opt_xgraph_state[k];
    dbsksp_xshock_graph_sptr new_graph = this->reconstruct_xgraph(map_node_state);

    // only accept as a solution when it corresponds to a real, legal xgraph
    // \todo check legality of "new_graph"
	if (!new_graph )
	{
      vcl_cout 
        << "\nERROR: couldn't reconstruct xraph for solution k=" << k 
        << "\n  DP cost = " << xshock_dp.list_opt_cost[k] << vcl_endl;
      continue;
    }	
	if(!is_sol_xgraph_legal(new_graph))
		continue;

	this->list_solution_costs_.push_back(xshock_dp.list_opt_cost[k]);
	this->list_solutions_.push_back(new_graph);

	// Real cost is the computed directly from the likelihood function
	// (or some other verification function)
	new_graph->compute_vertex_depths(this->root_vid_);
	float real_cost = float(-this->xshock_likelihood_->loglike_xgraph(new_graph, vcl_vector<unsigned >(), false));
	this->list_solution_real_costs_.push_back(real_cost);
  } // solution
  vcl_cout << "# detections after validating sols: " << this->list_solutions_.size() << vcl_endl;

  return;
}




// -----------------------------------------------------------------------------
//: Build the xnode grid
void dbsks_xshock_detector::
build_xnode_grid(const vgl_box_2d<int >& window)
{
  this->map_xnode_grid_.clear();
  //: Image
  // different methods to construct the grid. These evolve over time.

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

   // option 3: using input xgraph to construct geom model
  else if (generate_xnode_grid_option == 3)
  {
	this->build_xnode_grid_using_prev_dets_window(window);	
  }

   // option 3: using input xgraph to construct geom model
  else if (generate_xnode_grid_option == 4)
  {
	this->build_xnode_grid_using_prev_dets_xgraphs(window);	
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
    params.num_x = vnl_math::rnd(window.width()/params.step_x); // allow some padding

    // if the image is too big, we only consider the center portion of 512 pixels
    params.num_x = vnl_math::min(params.num_x, 65);
    params.min_x = vnl_math::rnd(window.centroid_x() - params.step_x*(params.num_x-1)/2 ); // centering

    params.step_y = 8;
    params.num_y = vnl_math::rnd(window.height()/params.step_y); // allow some padding

    // if the image is too big, only consider the center portion of 512 pixels
    params.num_y = vnl_math::min(params.num_y, 65);
    params.min_y = vnl_math::rnd(window.centroid_y() - params.step_y*(params.num_y-1)/2 ); // centering

    params.step_psi = vnl_math::pi / 8;
    params.num_psi = 16;
    params.min_psi = 0;

    params.step_phi0 = vnl_math::pi / 16;
    params.num_phi0 = 9;
    params.min_phi0 = vnl_math::pi_over_2 - params.step_phi0 * (params.num_phi0-1)/2;

    params.step_r = 8;
    params.num_r = 5;
    params.min_r = xv->radius() -(params.num_r-1) * params.step_r /2;

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

void dbsks_xshock_detector::
build_xnode_grid_using_prev_dets_window(const vgl_box_2d<int >& window)
{
  // clean up
  this->map_xnode_grid_.clear();

  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

	double x, y, psi, phi, radius;
	//vcl_cout<< "read para" <<vcl_endl;
	xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);
	
	//vcl_cout << " x " << x << " y "<< y << " psi " << psi << " phi " << phi << " radius "<< radius << " degree "<< xv->degree()<< " id "<<xv->id()<< vcl_endl;
    // set parameters for the grid
    dbsks_xnode_grid_params params;

	//////////////////////////////////////////// x
    params.step_x = 4;
    //params.num_x = 20;
    //params.min_x = vnl_math::rnd(x - params.step_x*(params.num_x-1)/2 ); // centering
	
	params.num_x = vnl_math::rnd(window.width()/params.step_x); // allow some padding

    // if the image is too big, we only consider the center portion of 512 pixels
    params.num_x = vnl_math::min(params.num_x, 65);
    params.min_x = vnl_math::rnd(window.centroid_x() - params.step_x*(params.num_x-1)/2 ); // centering

	//////////////////////////////////////////// y
    params.step_y = 4;
    //params.num_y = 20;
    //params.min_y = vnl_math::rnd(y - params.step_y*(params.num_y-1)/2 ); // centering
	params.num_y = vnl_math::rnd(window.height()/params.step_y); // allow some padding

    // if the image is too big, only consider the center portion of 512 pixels
    params.num_y = vnl_math::min(params.num_y, 65);
    params.min_y = vnl_math::rnd(window.centroid_y() - params.step_y*(params.num_y-1)/2 ); // centering

	// griding of psi and phi should also based on the given xgraph, so that the grid space can be reduced

    // griding of psi within [0, 2pi], have to keep the whole possible range, because at branch node, the change of psi between frames can be huge(tail of mouse), but make it denser to increase detection localization
    params.step_psi = vnl_math::pi / 12;
    params.num_psi = 24;
    params.min_psi = 0;

	// center at pi/2
    params.step_phi0 = vnl_math::pi / 20;
    params.num_phi0 = 11;
    params.min_phi0 = vnl_math::pi_over_2 - params.step_phi0 * (params.num_phi0-1)/2;


/*	// sample griding of psi centered at the psi from prev detected graph, this range should still be reletively large because the branch node orientation change can be huge, but samples should be dense enough
	// change scope to [0 2pi] if necessuary
    params.step_psi = vnl_math::pi / 18;
    params.num_psi = 17;
	if(psi < 0)
	{
		psi += (2*vnl_math::pi);
	}
	params.min_psi = psi - params.step_psi * (params.num_psi-1)/2; */

/*	// sample griding of phi centered at the phi from prev detected graph
    params.step_phi0 = vnl_math::pi / 32;
    params.num_phi0 = 9;
    params.min_phi0 = phi - params.step_phi0 * (params.num_phi0-1)/2;*/

	// sample centered at current radius, make the step correpond to a ratio of current radius
    params.step_r = 0.05 * (xv->radius()); 
    params.num_r = 11;
    params.min_r = xv->radius() -(params.num_r-1) * params.step_r /2;

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

void dbsks_xshock_detector::
build_xnode_grid_using_prev_dets_xgraphs(const vgl_box_2d<int >& window)
{
  // clean up
  this->map_xnode_grid_.clear();

	
  

  int i = 0;
  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit, ++i)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    // ignore degree-1 nodes
    if (xv->degree()==1)
      continue;

	dbsks_xnode_geom_model_sptr xnode_geom = this->xgraph_geom_->map_node2geom()[xv->id()];
    double min_psi, max_psi;
    double min_radius, max_radius;
    double min_phi, max_phi;
    double min_phi_diff, max_phi_diff;
    double graph_size;
    xnode_geom->get_param_range(min_psi, max_psi, min_radius, max_radius, 
      min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);

	//double x, y, psi, phi, radius;
	//vcl_cout<< "read para" <<vcl_endl;
	//xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, radius);
	//vcl_cout << " x " << x << " y "<< y << " psi " << psi << " phi " << phi << " radius "<< radius << " degree "<< xv->degree()<< " id "<<xv->id()<< vcl_endl;
    
    vcl_cout << "vid: " <<xv->id()<< " ";
// set parameters for the grid
    dbsks_xnode_grid_params params;

	//////////////////////////////////////////// x
    params.step_x = 2;
	params.num_x = vnl_math::rnd((xgraph_vertices_max_x_[i]-xgraph_vertices_min_x_[i]+42)/params.step_x); // allow some padding
    // if the image is too big, we only consider the center portion of 512 pixels
    params.num_x = vnl_math::min(params.num_x, 65);
    params.min_x = vnl_math::rnd((xgraph_vertices_min_x_[i]+xgraph_vertices_max_x_[i])/2 - params.step_x*(params.num_x-1)/2 ); // centering

    vcl_cout << "min_x: "<<params.min_x <<" ";

	//////////////////////////////////////////// y
    params.step_y = 2;
	params.num_y = vnl_math::rnd((xgraph_vertices_max_y_[i]-xgraph_vertices_min_y_[i]+42)/params.step_y); // allow some padding
    // if the image is too big, only consider the center portion of 512 pixels
    params.num_y = vnl_math::min(params.num_y, 65);
    params.min_y = vnl_math::rnd((xgraph_vertices_min_y_[i]+xgraph_vertices_max_y_[i])/2 - params.step_y*(params.num_y-1)/2 ); // centering

    vcl_cout << "min_y: "<<params.min_y <<" ";
/*
	// sample psi using geom model
	params.step_psi = vnl_math::pi / 15;
    double range_psi = 2*vnl_math::pi;
    params.num_psi = vnl_math::floor(range_psi / params.step_psi) + 1;
    params.min_psi = (max_psi+min_psi)/2 - params.step_psi*(params.num_psi-1)/2;
*/
    // sample psi from previous dets
	params.step_psi = vnl_math::pi / 12;
    double range_psi = (xgraph_vertices_max_psi_[i] - xgraph_vertices_min_psi_[i] + 0.5*vnl_math::pi);
    params.num_psi = vnl_math::floor(range_psi / params.step_psi)+1;
    params.min_psi = (xgraph_vertices_max_psi_[i]+xgraph_vertices_min_psi_[i])/2 - params.step_psi*(params.num_psi-1)/2;
/*
	// center at pi/2
    params.step_phi0 = vnl_math::pi / 24;
    params.num_phi0 = 18;
    params.min_phi0 = vnl_math::pi_over_2 - params.step_phi0 * (params.num_phi0-1)/2;
*/
	// sample phi from prev dets 
    params.step_phi0 = vnl_math::pi / 20;
    params.num_phi0 = 9;
    params.min_phi0 = (xgraph_vertices_max_phi_[i]+xgraph_vertices_min_phi_[i])/2 - params.step_phi0 * (params.num_phi0-1)/2;
/*
	// sample phi center at pi/2 using geom model
    params.step_phi0 = vnl_math::pi / 16;
    double range_phi0 = vnl_math::min((max_phi-min_phi), vnl_math::pi);
    params.num_phi0 = 2*vnl_math::rnd( range_phi0/2 / params.step_phi0) + 1;
    params.min_phi0 = vnl_math::pi_over_2 - params.step_phi0 * (params.num_phi0-1)/2;
*/

/*
	// sample centered at prototype radius, make the step correpond to a ratio of current radius
    params.step_r = vnl_math::max(0.05 * (xv->radius()), double(1)); 
    params.num_r = vnl_math::floor(xv->radius()*1.2/params.step_r)+1;
    params.min_r = xv->radius() - params.step_r * (params.num_r-1) /2;
*/

	// sample radius from prev dets
    params.step_r = vnl_math::max(0.08 * (xv->radius()), double(1)); 
    params.num_r = vnl_math::max(vnl_math::floor((xgraph_vertices_max_r_[i]-xgraph_vertices_min_r_[i]+xv->radius()*0.48)/params.step_r)+1,3);
    params.min_r = (xgraph_vertices_max_r_[i]+xgraph_vertices_min_r_[i])/2 - params.step_r * vnl_math::floor(double(params.num_r-1)/2);
/*
	// sample radius using geometric model
	params.step_r = vnl_math::max((max_radius-min_radius)/20, double(1)); 
	params.num_r = vnl_math::floor((max_radius-min_radius)/params.step_r)+1;
    params.min_r =  (min_radius+max_radius)/2 - (params.num_r-1) * params.step_r /2;
*/
    vcl_cout << "min_r: "<<params.min_r <<vcl_endl;
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
  double length_padding_ratio = 1; // \debug, temporary, was 2;
  double angle_padding_ratio = 1;
  // clean up
  this->map_xnode_grid_.clear();

  if (!this->xgraph_geom_ || this->xgraph_geom_->map_node2geom().empty())
  {
    vcl_cout << "\nERROR: xgraph geometric model is either not defined or empty.\n";
    return;
  }

  // size of the reference graph
  double cur_graph_size = vcl_sqrt(this->xgraph()->area());
  
  // set root id, etc
  this->root_vid_ = this->xgraph_geom_->root_vid();
  this->major_child_eid_ = this->xgraph_geom_->major_child_eid();

  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit)
  {
    // >> retrieve model information
    dbsksp_xshock_node_sptr xv = *vit;

    // ignore degree-1 nodes
    if (xv->degree()==1)
      continue;

    dbsks_xnode_geom_model_sptr xnode_geom = this->xgraph_geom_->map_node2geom()[xv->id()];
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
    params.step_x = vnl_math::rnd(cur_graph_size / 16); // heuristic ratio between step size and graph size //8;
    params.num_x = vnl_math::floor(window.width()/params.step_x+1); // allow some padding

    // if the image is too big, we only consider the center portion of 65 points
    params.num_x = vnl_math::min(params.num_x, 65);
    params.num_x = vnl_math::max(params.num_x, 1); //>> make sure we get at least 1 sample

    params.min_x = vnl_math::rnd(window.centroid_x() - params.step_x*(params.num_x-1)/2 ); // centering

    //>> y
    params.step_y = vnl_math::rnd(cur_graph_size / 16); // heuristic ratio between step size and graph size //8;
    params.num_y = vnl_math::floor(window.height()/params.step_y+1);
    
    // if the image is too big, only consider the center portion of 65 points
    params.num_y = vnl_math::min(params.num_y, 65);
    params.num_y = vnl_math::max(params.num_y, 1);

    params.min_y = vnl_math::rnd(window.centroid_y() - params.step_y*(params.num_y-1)/2 ); // centering

    //>> psi
    params.step_psi = vnl_math::pi / 8;
    double range_psi = vnl_math::min(angle_padding_ratio*(max_psi-min_psi), 2*vnl_math::pi);
    params.num_psi = vnl_math::floor(range_psi / params.step_psi) + 1;
    params.min_psi = (max_psi+min_psi)/2 - params.step_psi*(params.num_psi-1)/2;

    //>> phi0
    params.step_phi0 = vnl_math::pi / 16;
    double range_phi0 = vnl_math::min(angle_padding_ratio*(max_phi-min_phi), vnl_math::pi);
    params.num_phi0 = 2*vnl_math::rnd( range_phi0/2 / params.step_phi0) + 1;

    // re-compute step size so that the final range matches to the [min-max] range
    if (params.num_phi0 > 1)
    {
      params.step_phi0 = angle_padding_ratio*(max_phi-min_phi) / (params.num_phi0-1);
    }
    params.min_phi0 = (max_phi+min_phi)/2 - params.step_phi0 * (params.num_phi0-1)/2;
    params.min_phi0 = vnl_math::max(1e-3, params.min_phi0); // phi0 is always >= 0


    //>> radius
    // We want the step in r to be of the same order as x and y
    params.step_r = params.step_x;
    // compute range for radius. Enforce radius >= 1
    double range_radius = vnl_math::min(length_padding_ratio * (max_radius-min_radius), (max_radius+min_radius-1));
    params.num_r = vnl_math::floor(range_radius / params.step_r) + 1;
    params.min_r = (min_radius+max_radius)/2 - (params.num_r-1)*params.step_r /2;

    if (xv->degree() == 3)
    {
      params.step_phi1 = vnl_math::pi / 8;
      double range_phi1 = vnl_math::min(angle_padding_ratio*(max_phi_diff-min_phi_diff), vnl_math::pi);
      
      params.num_phi1 = vnl_math::floor(range_phi1 / params.step_phi1) + 1;
      params.min_phi1 = -params.step_phi1*(params.num_phi1-1)/2;
    }
    else
    {
      params.step_phi1 = vnl_math::pi / 8;
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
      dbsksp_xgraph_algos::update_degree2_node(xv, xe_c, xdesc);
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
      dbsksp_xgraph_algos::update_degree3_node(xv, parent_edge, child_edge1, child_edge2, 
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

void dbsks_xshock_detector::compute_vertices_para_range()
{
	vcl_vector<double> vertices_min_x (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_max_x (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_min_y (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_max_y (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_min_psi (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_max_psi (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_min_phi (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_max_phi (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_min_r (this->xgraph()->number_of_vertices(), -1);
	vcl_vector<double> vertices_max_r (this->xgraph()->number_of_vertices(), -1);

	for(int i =0 ; i< vcl_min(int(prev_dets_.size()), 3); i++)
	{
		dbsksp_xshock_graph_sptr cur_xgraph = prev_dets_[i]->xgraph();
		int j=0;
		dbsksp_xshock_graph::vertex_iterator vit = cur_xgraph->vertices_begin();
  		for (;vit != cur_xgraph->vertices_end(); ++j, ++vit)	
		{
			dbsksp_xshock_node_sptr xv = *vit;

			double x, y, psi, phi, r;
			//vcl_cout<< "read para" <<vcl_endl;
			xv->descriptor(xv->edge_list().front())->get(x, y, psi, phi, r);

			if( xv->id()==14 || xv->id()==11)
				xv->descriptor(xv->edge_list().back())->get(x, y, psi, phi, r);
			if(vertices_min_x[j]==-1)
				vertices_min_x[j] = x;
			else if(x < vertices_min_x[j])
				vertices_min_x[j] = x;

			if(vertices_max_x[j]==-1)
				vertices_max_x[j] = x;
			else if(x > vertices_max_x[j])
				vertices_max_x[j] = x;

			if(vertices_min_y[j]==-1)
				vertices_min_y[j] = y;
			else if(y < vertices_min_y[j])
				vertices_min_y[j] = y;

			if(vertices_max_y[j]==-1)
				vertices_max_y[j] = y;
			else if(y > vertices_max_y[j])
				vertices_max_y[j] = y;

			if(vertices_min_psi[j]==-1)
				vertices_min_psi[j] = psi;
			else if(psi < vertices_min_psi[j])
				vertices_min_psi[j] = psi;

			if(vertices_max_psi[j]==-1)
				vertices_max_psi[j] = psi;
			else if(psi > vertices_max_psi[j])
				vertices_max_psi[j] = psi;

			if(vertices_min_phi[j]==-1)
				vertices_min_phi[j] = phi;
			else if(phi < vertices_min_phi[j])
				vertices_min_phi[j] = phi;

			if(vertices_max_phi[j]==-1)
				vertices_max_phi[j] = phi;
			else if(phi > vertices_max_phi[j])
				vertices_max_phi[j] = phi;

			if(vertices_min_r[j]==-1)
				vertices_min_r[j] = r;
			else if(r < vertices_min_r[j])
				vertices_min_r[j] = r;

			if(vertices_max_r[j]==-1)
				vertices_max_r[j] = r;
			else if(r > vertices_max_r[j])
				vertices_max_r[j] = r;
		}
	}

  xgraph_vertices_min_x_ = vertices_min_x;
  xgraph_vertices_max_x_ = vertices_max_x;
  xgraph_vertices_min_y_ = vertices_min_y;
  xgraph_vertices_max_y_ = vertices_max_y;	
  xgraph_vertices_min_psi_ = vertices_min_psi;
  xgraph_vertices_max_psi_ = vertices_max_psi;	
  xgraph_vertices_min_phi_ = vertices_min_phi;
  xgraph_vertices_max_phi_ = vertices_max_phi;
  xgraph_vertices_min_r_ = vertices_min_r;
  xgraph_vertices_max_r_ = vertices_max_r;		


  vcl_cout << "min_x: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_min_x_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "max_x: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_max_x_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "min_y: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_min_y_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "max_y: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_max_y_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "min_psi: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_min_psi_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "max_psi: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_max_psi_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "min_phi: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_min_phi_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "max_phi: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_max_phi_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "min_r: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_min_r_[i] << " ";
  }	
  vcl_cout <<vcl_endl;
  vcl_cout << "max_r: ";	
  for(int i = 0; i< this->xgraph()->number_of_vertices(); i++)
  {
	 vcl_cout << xgraph_vertices_max_r_[i] << " ";
  }	
  vcl_cout <<vcl_endl;

  vcl_cout << "Done Compute Vertices Parameters Range." << vcl_endl;

	return;
}

bool dbsks_xshock_detector::is_sol_xgraph_legal(dbsksp_xshock_graph_sptr xgraph)
{

for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=xgraph->edges_end(); ++eit)
{
	dbsksp_xshock_edge_sptr xe = *eit;
	dbsksp_xshock_node_descriptor start = *xe->source()->descriptor(xe);
	dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();

	if(xe->source()->degree()==1 || xe->target()->degree()==1)
		continue;
	dbsksp_xshock_fragment xfrag(start, end);

	//if(!xfrag.is_legal_new())
	if(!xfrag.is_legal())
		return false;
}
return true;
}






