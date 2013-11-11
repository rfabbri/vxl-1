// This is file seg/dbsks/dbsks_dp_match.cxx

//:
// \file

#include "dbsks_dp_match.h"

#include <dbsks/dbsks_io_matrix_array.h>
#include <dbsks/dbsks_io_circ_arc_grid.h>
#include <dbsks/dbsks_io_shapelet_grid.h>
#include <dbsks/dbsks_ocm_image_cost.h>

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <dbsksp/xio/dbsksp_xio_shock_graph.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <dbsks/dbsks_utils.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

#include <vnl/vnl_math.h>
#include <vul/vul_file.h>
#include <vul/vul_temp_filename.h>
#include <vcl_sstream.h>


// clipping functions
int dbsks_clip(int x, int x_min, int x_max)
{
  if (x < x_min) return x_min;
  if (x > x_max) return x_max;
  return x;
}


// ============================================================================
//  dbsks_dp_match
// ============================================================================

//: constructor
dbsks_dp_match::
dbsks_dp_match(): ds_shapelet_(3.0), sigma_deform_ (0.5 /2)
{
  this->oriented_chamfer_ = new dbsks_ocm_image_cost();
};


// ----------------------------------------------------------------------------
//: Construct an arc grid and compute their costs given an edgemap
void dbsks_dp_match::
init_arc_image_cost(const vil_image_view<float >& edgemap, float edge_threshold,
                       const dbsks_circ_arc_grid& arc_grid, double sampling_ds,
                       double chamfer_sigma, 
                       double angle_sigma)
{
  // Grid of circular arc
  this->set_arc_grid(arc_grid);

  // Make sure a Chamfer matcher has been created
  if (!this->oriented_chamfer())
  {
    this->oriented_chamfer_ = new dbsks_ocm_image_cost();
  }

  // Edgemap and threshold
  this->oriented_chamfer()->set_edgemap(edgemap, edge_threshold);

  // standard deviations oriented chamfer matching
  float sigma_distance = float(chamfer_sigma);
  float sigma_angle = float(angle_sigma); // float(vnl_math::pi/6);
  this->oriented_chamfer()->set_sigma(sigma_distance, sigma_angle);
  return;
}


// -----------------------------------------------------------------------------
//: Carry out the computation of image costs for all circular arcs
// Assume all intermediate variables have been set up
void dbsks_dp_match::
compute_arc_image_cost()
{
  // Clean up existing cost grid
  this->arc_image_cost_.clear();

  // Use oriented Chamfer matching to compute costs
  this->oriented_chamfer()->f(this->arc_grid_, float(this->ds_shapelet_), 
    this->arc_image_cost_);
}


// ----------------------------------------------------------------------------
// New saving format. There is a version number at the beginning
void dbsks_dp_match::
save_circ_arc_costs(const vcl_string& filename)
{ 
  vcl_cout << "Save image cost of an arc grid\n";
  vsl_b_ofstream os(filename);

  // >> >> save version number
  const short version = 1;
  vsl_b_write(os, version);

  // Image cost parameters
  // image dimension
  vsl_b_write(os, this->oriented_chamfer()->dt_.ni());
  vsl_b_write(os, this->oriented_chamfer()->dt_.nj());

  // edge threshold
  vsl_b_write(os, this->oriented_chamfer()->edge_threshold_);

  // sample distance
  vsl_b_write(os, this->ds_shapelet_);

  // sigma for computing potential
  vsl_b_write(os, this->oriented_chamfer()->sigma_distance_);
  vsl_b_write(os, this->oriented_chamfer()->sigma_angle_);

  vsl_b_write(os, this->arc_grid_);
  vsl_b_write(os, this->arc_image_cost_);
    
  os.close();
  
  return;

}



// -----------------------------------------------------------------------------
//: Load computed circular arc costs from a file with NEW format, which has
// a version number at the beginning
void dbsks_dp_match::
load_circ_arc_costs(const vcl_string& filename)
{
  vcl_cout << "Loading arc_grid image cost: \n";

  vsl_b_ifstream is(filename);
  if (!is) 
  {
    vcl_cout << "ERROR reading binary file " << filename 
      << ". Quit now.\n";
    return;
  }


  // read version number
  short version;
  vsl_b_read(is, version);

  switch (version)
  {
  case 1:
    
    // Image cost parameters
    // image dimension
    vsl_b_read(is, this->image_ni_);
    vsl_b_read(is, this->image_nj_);

    // edge threshold
    vsl_b_read(is, this->oriented_chamfer()->edge_threshold_);

    // sample distance
    vsl_b_read(is, this->ds_shapelet_);

    // sigma for computing potential
    vsl_b_read(is, this->oriented_chamfer()->sigma_distance_);
    vsl_b_read(is, this->oriented_chamfer()->sigma_angle_);

    // read arc grid and their costs
    vsl_b_read(is, this->arc_grid_);
    vsl_b_read(is, this->arc_image_cost_);

    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const dbsks_shapelet_grid& arc_grid) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  is.close();
    
  vcl_cout << "succeeded: \n";
  return;
}






// ----------------------------------------------------------------------------
//: Clear arc_grid and arc_grid costs 
void dbsks_dp_match::
clear_arc_cost_data()
{
  this->arc_image_cost_.clear();
  return;
}






// -----------------------------------------------------------------------------
//: Set parameters necessary to set up a "working graph"
void dbsks_dp_match::
set_graph_params(const dbsksp_shock_graph_sptr& ref_graph,
                 unsigned image_width, unsigned image_height)
{
  this->set_ref_graph(ref_graph);
  this->image_ni_ = image_width;
  this->image_nj_ = image_height;
}



// -----------------------------------------------------------------------------
//: Set parameters necessary to run DP given a "working graph" and arc costs
void dbsks_dp_match::
set_dp_params(const dbsks_shapelet_grid_params& grid_params,
              double sigma_deform,
              const vcl_string& temp_file_prefix,
              const vcl_string& temp_data_folder)
{
  this->sigma_deform_ = sigma_deform;
  this->shapelet_grid_params_ = grid_params;
  this->temp_file_prefix_ = temp_file_prefix;
  this->temp_data_folder_ = temp_data_folder;
}



//: Set statistics of a shock graph
// REQUIREMENT: shock graph inside "stats" must have the same topology as the
// shock graph of this DP
void dbsks_dp_match::
set_shock_graph_stats(const dbsks_shock_graph_stats& stats)
{
  this->shock_graph_stats_ = stats; 
}


// ----------------------------------------------------------------------------
//: Run the optimization algorithm
void dbsks_dp_match::
run_optim_for_one_model_height(float model_height)
{
  this->init_dp_vars(model_height);
  this->optimize_edge_based();
}



//: Run the optimization algorithm
void dbsks_dp_match::
run_optim_for_one_model_size(float model_size)
{
  this->init_dp_vars_using_stats(model_size);
  this->optimize_edge_based();
}



// -----------------------------------------------------------------------------
//: Refine the centers of the shapelet grids and run optimization again
void dbsks_dp_match::
refine_grid_centers_and_run_opt()
{
  this->adjust_shapelet_grid_center(this->opt_shapelet_map_);
  double ratio_num_xA = 0.5;
  double ratio_num_yA = 0.5;
  this->change_shapelet_grids_num_xy(ratio_num_xA, ratio_num_yA);
  this->optimize_edge_based();
}



// ----------------------------------------------------------------------------
//: Save optimization results (the shapelet grids and the optimal states)
void dbsks_dp_match::
save_dp_optim_results(const vcl_string& filename)
{
  // >> Save the reference graph using the same file name
  vcl_string graph_filename = vul_file::strip_extension(filename) + ".xml";

  if ( x_write(graph_filename, this->graph()) )
  {
    vcl_cout << "Saving reference shock graph to XML file ... succeeded.\n";
  }
  else
  {
    vcl_cout << "Saving shock graph XML file ... failed. Quit now.\n";
    return;
  };

  
  // >> Save optimized results data to a binary file
  
  vcl_cout << "Save optimization results:\n";
  vsl_b_ofstream os(filename);

  // >> >> save version number
  const short version = 1;
  vsl_b_write(os, version);

  // >> >> Save grid params
  vsl_b_write(os, this->shapelet_grid_params_);

  // >> >> save shapelet_scale_sigma
  vsl_b_write(os, this->sigma_deform_);

  // >> >> Save location of temp folder
  vsl_b_write(os, this->temp_file_prefix_);

  // >> >> save shapelet grid map
  vsl_b_write(os, this->shapelet_grid_map_.size());
  for (vcl_map<dbsksp_shock_edge_sptr, dbsks_shapelet_grid >::iterator it = 
    this->shapelet_grid_map_.begin(); it != this->shapelet_grid_map_.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    dbsks_shapelet_grid& grid_e = it->second;
    
    vsl_b_write(os, e->id());
    vsl_b_write(os, grid_e);
  }

  // >> >> save opt_graph_i_state_map
  vsl_b_write(os, this->graph_opt_i_state_.size());
  for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::iterator it =
    this->graph_opt_i_state_.begin(); it != this->graph_opt_i_state_.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    vgl_point_2d<int > i_state = it->second;

    vsl_b_write(os, e->id());
    vsl_b_write(os, i_state.x());
    vsl_b_write(os, i_state.y());
  }
  os.close();

  vcl_cout << "Succeeded:\n";
  
  return;
}


// ----------------------------------------------------------------------------
//: Save optimization results (the shapelet grids and the optimal states)
bool dbsks_dp_match::
load_dp_optim_results(const vcl_string& filename)
{
  // >> First load the reference graph from the filename but with extension .xml
  vcl_string graph_filename = vul_file::strip_extension(filename) + ".xml";
  
  dbsksp_shock_graph_sptr ref_graph = 0;
  if ( x_read(graph_filename, ref_graph ) )
  {
    vcl_cout << "Loading shock graph XML file... completed.\n";
    ref_graph->compute_all_dependent_params();
  }
  else
  {
    vcl_cout << "Loading shock graph XML file ... failed. Quit now.\n";
    return false;
  };
  this->set_ref_graph(ref_graph);

  

  vcl_cout << "Loading binary file of optimization results...\n";

  vsl_b_ifstream is(filename);
  if (!is) 
  {
    vcl_cout << "ERROR reading binary file " << filename 
      << ". Quit now.\n";
    return false;
  }

  // read version number
  short version;
  vsl_b_read(is, version);

  unsigned int num_shapelet_grids = 0;
  unsigned int num_opt_i_states = 0;
  switch (version)
  {
  case 1:
    
    // >> >> Load grid params
    vsl_b_read(is, this->shapelet_grid_params_);

    // >> >> Load shapelet_scale_sigma
    vsl_b_read(is, this->sigma_deform_);

    // >> >> Load location of temp folder
    vsl_b_read(is, this->temp_file_prefix_);

    // Initialize "working graph" and all the intermediate variables
    this->init_dp_vars(-1.0f);

    
    // >> >> Load shapelet grid map
    vsl_b_read(is, num_shapelet_grids);
    for (unsigned i =0; i <num_shapelet_grids; ++i)
    {
      unsigned int id_e = 0;
      vsl_b_read(is, id_e);

      // Find the edge with the given id
      dbsksp_shock_edge_sptr e = this->graph()->edge_from_id(id_e);
      dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
      vsl_b_read(is, grid_e);
    }

    // >> >> Load opt_graph_i_state_map
    this->graph_opt_i_state_.clear();
    vsl_b_read(is, num_opt_i_states);
    for (unsigned i =0; i < num_opt_i_states; ++i)
    {
      unsigned int id_e;
      int i_xy;
      int i_plane;
      vsl_b_read(is, id_e);
      vsl_b_read(is, i_xy);
      vsl_b_read(is, i_plane);
      vgl_point_2d<int > i_state(i_xy, i_plane);

      dbsksp_shock_edge_sptr e = this->graph()->edge_from_id(id_e);
      this->graph_opt_i_state_.insert(vcl_make_pair(e, i_state));    
    }
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream & is, const dbsks_shapelet_grid& arc_grid) \n"
      << "         Unknown version number " << version << "\n";
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
  is.close();

  return true;;
}



// -----------------------------------------------------------------------------
//: Trace the boundary of the optimal shock graph
void dbsks_dp_match::
trace_opt_graph_bnd(vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list)
{
  vsol_list = this->trace_graph_boundary(this->graph_opt_i_state_);
  return;
}










// =============================================================================
// Access memeber variables
// =============================================================================



// -----------------------------------------------------------------------------
//: Return the fragment grid associated with an edge
dbsks_shapelet_grid& dbsks_dp_match::
shapelet_grid(unsigned int edge_id)
{
  dbsksp_shock_edge_sptr e = this->graph()->edge_from_id(edge_id);
  return this->shapelet_grid_map_.find(e)->second;
}



// =============================================================================
// Optimization functions
// =============================================================================


// ----------------------------------------------------------------------------
//: Optimize energy of shock graph
void dbsks_dp_match::
optimize_edge_based()
{
  // Make sure the arc costs have been computed.
  if (this->arc_image_cost_.empty())
  {
    vcl_cout << "ERROR: Need to compute arc costs before doing DP optimization.\n";
    return;
  }

  

  // --------------------------------------------------------------------------
  // >> Collect all the vertices of the same depths
  vcl_vector<vcl_vector<dbsksp_shock_node_sptr > > vertex_bins;
  dbsks_collect_shock_nodes_by_depth(this->graph(), vertex_bins);
  int max_depth = vertex_bins.size()-1;


  // DYNAMIC PROGRAMMING TO MINIMIZE ENERGY ------------------------------------

  // >> Iterate through the bins and compute the optimize location function
  // The "DP nodes" are the fragments, i.e. shock edges
  // HACK: for now, assume the root "edge" is at the end
  // Since we ignore terminal edges, we start with the ones adjacent to them
  // We stop at the nodes right before the root fragment (depth > 1)
  // For each state of the parent, we want to find the best state of its children
  for (int depth = max_depth-2; depth >=1; --depth)
  {
    for (unsigned i_v =0; i_v<vertex_bins[depth].size(); ++i_v)
    {
      dbsksp_shock_node_sptr v_p = vertex_bins[depth][i_v];

      // ignore leaf-node for now
      if (v_p->degree() == 1)
        continue;

      // for now, only deal with cases when there is only one child
      if (v_p->degree() == 2)
      {
        // the "parent" edge
        dbsksp_shock_edge_sptr e_p = v_p->parent_edge();

        // the "child edge"
        dbsksp_shock_edge_sptr e_c = this->graph()->cyclic_adj_succ(e_p, v_p);
        
        // terminal edges will be merged into its adjacent edge as one entity.
        // don't need to consider here
        if (e_c->is_terminal_edge())
          continue;

        // Optimize this node
        this->optimize_degree_2_node(v_p);
      }
      else if (v_p->degree() == 3)
      {
        // the "parent" edge
        dbsksp_shock_edge_sptr e_p = v_p->parent_edge();

        // the "child edges"
        dbsksp_shock_edge_sptr e_c1 = this->graph()->cyclic_adj_succ(e_p, v_p);
        dbsksp_shock_edge_sptr e_c2 = this->graph()->cyclic_adj_succ(e_c1, v_p);

        // we don't handle A12_A_\infty as of now
        assert(!e_c1->is_terminal_edge() && !e_c2->is_terminal_edge());
        this->optimize_degree_3_node(v_p);
      }
      else
      {
        vcl_cout << "ERROR: Can't handle nodes with degree > 3 now.\n";
        continue;
      }
    } // i_v
  } // depth

  // Now optimize on the root "edge"
  // locate root edge
  dbsksp_shock_edge_sptr e_root = 0;
  for (unsigned i=0; i<vertex_bins[1].size(); ++i)
  {
    dbsksp_shock_node_sptr v= vertex_bins[1][i];
    if (v->degree() > 1)
    {
      e_root = v->parent_edge();
      break;
    }
  }

  // retrieve the bc_sum for the root node
  dbsks_shapelet_grid& grid_r = this->shapelet_grid_map_.find(e_root)->second;
  dbsksp_shapelet_sptr sref_r = this->ref_shapelet_map_.find(e_root)->second;
  grid_float& sum_bc_root = this->sum_bc_map_.find(e_root)->second;

  
  vcl_cout << "load_sum_bc_from_file of e_root \n";
  if (!this->load_sum_bc_from_file(e_root, sum_bc_root))
  {
    assert(sum_bc_root.empty());
    this->allocate(sum_bc_root, grid_r, 0.0f);
  }

  vcl_cout << "compute_matching_cost for root node \n";
  grid_float& matchcost_root = this->matchcost_map_.find(e_root)->second;
  this->compute_matching_cost(grid_r, sref_r, matchcost_root);
  

  // DEBUG 
  this->save_matchcost_to_file(e_root, matchcost_root);

  // add this cost to sum_bc_c and clear the memory
  int num_planes_r = grid_r.num_planes();
  for (int ir_plane =0; ir_plane < num_planes_r; ++ir_plane)
  {
    sum_bc_root[ir_plane] += matchcost_root[ir_plane];
  }
  matchcost_root.clear();

  // save sum_bc to file
  vcl_cout << "Save_sum_bc_root_to_file...\n";
  this->save_sum_bc_to_file(e_root, sum_bc_root);
  sum_bc_root.clear();


  // OPTIMAL SOLUTION ///////////////////////////////////////////////////////////
  this->trace_global_solution(e_root);
  //////////////////////////////////////////////////////////////////////////////


  // SUB-OPTIMAL SOLUTIONS ////////////////////////////////////////////////////
  // Divide the image into 3 x 3 grid, find the min cost within each cell
  // Then result the top 4
  int ni_cell = 3;
  int nj_cell = 3;

  // We take only the top four points
  int num_top_picks = 4;
  this->trace_suboptimal_solutions(e_root, ni_cell, nj_cell, num_top_picks);
  /////////////////////////////////////////////////////////////////////////////


  //vcl_cout << "4 \n";
  this->release_matrix_memory(this->matchcost_map_);
  this->release_matrix_memory(this->opt_e_gridplane_map_);
  this->release_matrix_memory(this->opt_e_gridxy_map_);
  this->release_matrix_memory(this->sum_bc_map_);

  //vcl_cout << "5 \n";
  this->clear_all_temp_files();
  //vcl_cout << "6 \n";
}







// -----------------------------------------------------------------------------
//: Trace out the globally optimum solution
void dbsks_dp_match::
trace_global_solution(const dbsksp_shock_edge_sptr& e_root)
{
  // retrive relevanet information
  dbsks_shapelet_grid& grid_r = this->shapelet_grid_map_.find(e_root)->second;
  dbsksp_shapelet_sptr sref_r = this->ref_shapelet_map_.find(e_root)->second;
  grid_float& sum_bc_root = this->sum_bc_map_.find(e_root)->second;

  // load content of sum_bc_root from file
  this->load_sum_bc_from_file(e_root, sum_bc_root);


  // Find minimum energy among all possible state of root node
  float min_root_energy = vnl_numeric_traits<float >::maxval;
  int opt_i_plane_root = -1;
  int opt_i_xy_root = -1;

  
  // iterate thru all possible states
  int num_planes_r = grid_r.num_planes();
  for (int ir_plane =0; ir_plane < num_planes_r; ++ir_plane)
  {
    const vnl_matrix<float >& total_cost = sum_bc_root[ir_plane];
    for (int ir_xA =0; ir_xA < grid_r.num_xA_; ++ir_xA)
    {
      for (int ir_yA =0; ir_yA < grid_r.num_yA_; ++ir_yA)
      {
        if (total_cost(ir_xA, ir_yA) < min_root_energy)
        {
          min_root_energy = total_cost(ir_xA, ir_yA);
          opt_i_plane_root = ir_plane;
          opt_i_xy_root = grid_r.grid_to_linear(ir_xA, ir_yA);
        }
      } //ir_yA
    } //ir_xA
  } // ir_plane


  // save sum_bc to file
  vcl_cout << "Save_sum_bc_root_to_file...\n";
  this->save_sum_bc_to_file(e_root, sum_bc_root);
  sum_bc_root.clear();

  
  vcl_cout << "Optimal graph: \n"
    << "  Min graph energy = " << min_root_energy << "\n"
    << "  opt_i_plane_root = " << opt_i_plane_root << "\n"
    << "  opt_i_xy_root = " << opt_i_xy_root << "\n";

  // Save DP cost
  this->min_dp_cost_ = min_root_energy;
  this->min_real_cost_ = -1; // indicate this has not been evaluated

  vcl_cout << "Trace back to construct optimal graph\n";
  this->graph_opt_i_state_.clear();

  // debug texts
  vcl_cout << "    1 ";
  vgl_point_2d<int > opt_i_state_root(opt_i_xy_root, opt_i_plane_root);
  this->trace_opt_state(e_root, opt_i_state_root, this->graph_opt_i_state_); 

  // construct a graph corresponding to the optimal states
  vcl_cout << " 2 ";
  this->convert_i_state_map_to_shapelet_map(this->graph_opt_i_state_,
    this->opt_shapelet_map_);

  this->construct_graph(graph_opt_i_state_, this->shapelet_list);

  //this->construct_graph(this->graph_opt_i_state_);

  // Compute a bounding box for this result
  vcl_cout << " 3 ";
  vsol_box_2d_sptr bbox = dbsks_compute_bounding_box(this->shapelet_list);
  this->bbox_.set_size(4);
  this->bbox_[0] = float(bbox->get_min_x());
  this->bbox_[1] = float(bbox->get_min_y());
  this->bbox_[2] = float(bbox->get_max_x());
  this->bbox_[3] = float(bbox->get_max_y());

  // display the stored cost and real cost of the edges, given their states
  if (true)
  {
    this->display_real_cost(this->graph_opt_i_state_);
  }

  return;
}



// -----------------------------------------------------------------------------
//: Trace out sub-optimal solution by dividing the states into groups spatially
void dbsks_dp_match::
trace_suboptimal_solutions(const dbsksp_shock_edge_sptr& e_root,
                           int cellgrid_width, 
                           int cellgrid_height, 
                           int num_top_picks)
{
  // retrive relevanet information
  dbsks_shapelet_grid& grid_r = this->shapelet_grid_map_.find(e_root)->second;
  dbsksp_shapelet_sptr sref_r = this->ref_shapelet_map_.find(e_root)->second;
  grid_float& sum_bc_root = this->sum_bc_map_.find(e_root)->second;

  // load content of sum_bc_root from file
  this->load_sum_bc_from_file(e_root, sum_bc_root);

  // Divide the image into a grid, find the min cost within each cell
  // Only report the ``num_top_picks'' top results

  int ni_cell = cellgrid_width;
  int nj_cell = cellgrid_height;

  // width and height of a cell
  float num_xA_per_cell = float(grid_r.num_xA_) / ni_cell;
  float num_yA_per_cell = float(grid_r.num_yA_) / nj_cell;

  vcl_map<float, vgl_point_2d<int > > cell_energy_state_map;
  for (int i_cell =0; i_cell < ni_cell; ++i_cell)
  {
    for (int j_cell =0; j_cell < nj_cell; ++j_cell)
    {
      vcl_cout << "i_cell [ " << i_cell << " ] j_cell [ " << j_cell << " ]\n";
      // compute the boundary of the cell
      int cell_xA_start = vnl_math_rnd( i_cell    * num_xA_per_cell);
      int cell_xA_end   = vnl_math_rnd((i_cell+1)* num_xA_per_cell);
      
      int cell_yA_start = vnl_math_rnd( j_cell    * num_yA_per_cell);
      int cell_yA_end   = vnl_math_rnd((j_cell+1) * num_yA_per_cell);


      // cr = cell root
      float min_energy_cr = vnl_numeric_traits<float >::maxval;
      int opt_i_plane_cr = -1;
      int opt_i_xy_cr = -1;

      // search for minimum cost within the cell
      int num_planes_r = grid_r.num_planes();
      for (int ir_plane =0; ir_plane < num_planes_r; ++ir_plane)
      {
        vnl_matrix<float >& total_cost = sum_bc_root[ir_plane];
        for (int ir_xA =cell_xA_start; ir_xA < cell_xA_end; ++ir_xA)
        {
          for (int ir_yA =cell_yA_start; ir_yA < cell_yA_end; ++ir_yA)
          {
            if (total_cost(ir_xA, ir_yA) < min_energy_cr)
            {
              min_energy_cr = total_cost(ir_xA, ir_yA);
              opt_i_plane_cr = ir_plane;
              opt_i_xy_cr = grid_r.grid_to_linear(ir_xA, ir_yA);
            }
          } //ir_yA
        } //ir_xA
      } // ir_plane


      vcl_cout << "  optimal cell energy: \n"
        << "  min_energy_cr = " << min_energy_cr << "\n"
        << "  opt_i_plane_cr = " << opt_i_plane_cr << "\n"
        << "  opt_i_xy_cr = " << opt_i_xy_cr << "\n";
      
      // combined two params into a cpoint
      vgl_point_2d<int > opt_i_state_cr(opt_i_xy_cr, opt_i_plane_cr);
      cell_energy_state_map.insert(vcl_make_pair(min_energy_cr, opt_i_state_cr));
    }
  }

  // save sum_bc to file
  vcl_cout << "Save_sum_bc_root_to_file...\n";
  this->save_sum_bc_to_file(e_root, sum_bc_root);
  sum_bc_root.clear();

  
  // We take only ``num_top_picks'' top solutions
  vcl_cout << "Number of top picks = " << num_top_picks << "\n";
  int cell_count = 0;
  for (vcl_map<float, vgl_point_2d<int > >::iterator it = 
    cell_energy_state_map.begin(); it != cell_energy_state_map.end(); ++it)
  {
    ++cell_count;
    
    if (cell_count > num_top_picks)
    {
      break;
    }

    vcl_cout << "Cell count = " << cell_count << "\n";

    vcl_cout << "  optimal cell energy: \n"
      << "  min_energy_cr = " << it->first << "\n"
      << "  opt_i_plane_cr = " << it->second.y() << "\n"
      << "  opt_i_xy_cr = " << it->second.x() << "\n";

    // trace out the states and the optimal graphs
    vcl_cout << "Trace back to construct optimal graph\n";
    vgl_point_2d<int > opt_i_state_cr = it->second;
    vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > > graph_opt_i_state;

    ///////////////////////-----------------
    this->trace_opt_state(e_root, opt_i_state_cr, graph_opt_i_state); 
    ///////////////////////------------------

    this->list_graph_opt_i_state_.push_back(graph_opt_i_state);
  }




  vcl_cout << "Reconstruct shapelets of the edges from their sub-optimal states: \n";
  for (unsigned i =0; i < this->list_graph_opt_i_state_.size(); ++i)
  {
    vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > > graph_i_state_map = 
      this->list_graph_opt_i_state_[i];

    // shapelets of the edges for this sub-optimal solution
    vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > opt_shapelet_map;
    for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::iterator it =
      graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
    {
      dbsksp_shock_edge_sptr e = it->first;
      
      int i_xy_e = it->second.x();
      int i_plane_e = it->second.y();
      
      // retrieve the shapelet corresponding to this state
      dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
      dbsksp_shapelet_sptr shapelet_e = grid_e.shapelet(i_xy_e, i_plane_e);

      // ------------------
      opt_shapelet_map.insert(vcl_make_pair(e, shapelet_e));
      // ------------------

      //// for visualization purpose
      //this->shapelet_list.push_back(shapelet_e);

      //// add the front shapelet
      //if (grid_e.has_front_arc_)
      //{
      //  this->shapelet_list.push_back(shapelet_e->terminal_shapelet_front());
      //}

      //// add the rear shapelet
      //if (grid_e.has_rear_arc_)
      //{
      //  this->shapelet_list.push_back(shapelet_e->terminal_shapelet_rear());
      //}
    }
    this->list_opt_shapelet_map_.push_back(opt_shapelet_map);
  }
  return;
}







// ----------------------------------------------------------------------------
//: Initialize the intermediate variables
void dbsks_dp_match::
init_dp_vars(float model_height)
{
  this->model_height_ = model_height;

  // Set up the working graph
  // create a copy of reference graph as the starting point
  this->set_graph(new dbsksp_shock_graph(*this->ref_graph()));
  this->graph()->compute_all_dependent_params();

  // Now scale this graph to the desired size
  if (this->model_height_ > 0)
  {
    vsol_box_2d_sptr bbox = this->graph()->compute_approx_bounding_box();
    //double scale = this->rih_ * this->image_nj_ / bbox->height();
    double scale = this->model_height_ / bbox->height();
    this->graph()->scale_up(scale);
  }

  // clean up the intermediate variables
  this->shapelet_grid_map_.clear();
  this->ref_shapelet_map_.clear();
  this->matchcost_map_.clear();
  this->sum_bc_map_.clear();
  this->opt_e_gridplane_map_.clear();
  this->opt_e_gridxy_map_.clear();
  
  // --------------------------------------------------------------------------
  // Build a shapelet grid for each of the edges
  // terminal edges are grouped with the edge adjacent to them
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    // retrieve the current (reference) shapelet for this edge
    dbsksp_shapelet_sptr sref = e->fragment()->get_shapelet();

    // make sure the shapelet is in the right direction, i.e. facing downward to its children
    if (e->target()->parent_edge() != e )
    {
      sref = sref->reversed_dir();
    }
    // save it for future use
    this->ref_shapelet_map_.insert(vcl_make_pair(e, sref));


    // build shapelet_grid - set of all possible configurations for an edge

    // Determine whether this shapelet includes the front or rear arcs (end-fragments)
    bool has_rear_arc = false;
    bool has_front_arc = false;

    dbsksp_shock_node_sptr v_child = (e->source()->parent_edge() == e) ?
      e->source() : e->target();
    if (v_child->degree() == 2 && 
      this->graph()->cyclic_adj_succ(e, v_child)->is_terminal_edge())
    {
      has_rear_arc = true;
    }

    dbsksp_shock_node_sptr v_parent = e->opposite(v_child);
    if (v_parent->degree() == 2 && 
      this->graph()->cyclic_adj_succ(e, v_parent)->is_terminal_edge())
    {
      has_front_arc = true;
    }

    dbsks_shapelet_grid grid;
    grid.set_grid(sref, has_front_arc, has_rear_arc, this->shapelet_grid_params_);
    this->shapelet_grid_map_.insert(vcl_make_pair(e, grid));

    // compute matching costs for each of the shapelet
    vcl_map<dbsksp_shock_edge_sptr, grid_float >::iterator i_matchcost = 
      this->matchcost_map_.insert(vcl_make_pair(e, grid_float())).first;

    // allocate and fill sum_bc
    vcl_map<dbsksp_shock_edge_sptr, grid_float >::iterator i_sum_bc = 
      this->sum_bc_map_.insert(vcl_make_pair(e, grid_float())).first;
  }


  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // This set-up assumes the root "edge" is next to a terminal edge
  // and neither of its node is a degree-3 node.
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


  // Allocate space for each fragment to store its optimal state given its 
  // parent fragment's state
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;
    
    // identify this edge's parent edge and allocate memory to store optimal
    // state of the edge given its parent
    dbsksp_shock_node_sptr v_p = (e->source()->parent_edge() == e) ? 
      e->target() : e->source();
    
    dbsksp_shock_edge_sptr e_p = v_p->parent_edge();
    
    // if v_p is the root node than 'e' is the root fragment. No need to allocate
    // such space for it. In this case, e_p would be zero.
    if (!e_p)
      continue;
    
    // look up the shapelet grid of the parent edge
    dbsks_shapelet_grid& grid_p = this->shapelet_grid_map_.find(e_p)->second;

    // gridplane
    vcl_map<dbsksp_shock_edge_sptr, grid_int >::iterator i_gridplane = 
      this->opt_e_gridplane_map_.insert(vcl_make_pair(e, grid_int())).first;
    
    // gridxy
    vcl_map<dbsksp_shock_edge_sptr, grid_int >::iterator i_gridxy = 
      this->opt_e_gridxy_map_.insert(vcl_make_pair(e, grid_int())).first;
  }
  return;
}




//: Initialize the intermediate variables, given model size and shock graph statistcs
void dbsks_dp_match::
init_dp_vars_using_stats(float model_size)
{
  // scale the model appropriately
  if (model_size > 0)
  {
    this->model_size_ = model_size;
  }
  else
  {
    // use the reference model
    this->model_size_ = float(dbsks_compute_graph_size(this->ref_graph()));
  }

  // Set up the working graph
  // create a copy of reference graph as the starting point
  this->set_graph(new dbsksp_shock_graph(*this->ref_graph()));
  this->graph()->compute_all_dependent_params();

  // Now scale this graph to the desired size
  double ref_size = dbsks_compute_graph_size(this->ref_graph());
  double scale = this->model_size_ / ref_size;
  this->graph()->scale_up(scale);

  // clean up the intermediate variables
  this->shapelet_grid_map_.clear();
  this->ref_shapelet_map_.clear();
  this->matchcost_map_.clear();
  this->sum_bc_map_.clear();
  this->opt_e_gridplane_map_.clear();
  this->opt_e_gridxy_map_.clear();

  // Construct shapelet grids
  this->build_shapelet_grids(this->shapelet_grid_params_, 
    this->shock_graph_stats_,
    model_size,
    this->shapelet_grid_map_);

  // --------------------------------------------------------------------------
  // Allocate intermediate cost variables for DP
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    // compute matching costs for each of the shapelet
    this->matchcost_map_.insert(vcl_make_pair(e, grid_float())).first;

    // allocate sum_bc
    this->sum_bc_map_.insert(vcl_make_pair(e, grid_float())).first;

    // Allocate space for each fragment to store its optimal state given its 
    // parent fragment's state

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This set-up assumes the root "edge" is next to a terminal edge
    // and neither of its node is a degree-3 node.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     
    // identify this edge's parent edge and allocate memory to store optimal
    // state of the edge given its parent
    dbsksp_shock_node_sptr v_p = (e->source()->parent_edge() == e) ? 
      e->target() : e->source();
    
    dbsksp_shock_edge_sptr e_p = v_p->parent_edge();
    
    // if v_p is the root node than 'e' is the root fragment. No need to allocate
    // such space for it. In this case, e_p would be zero.
    if (!e_p)
      continue;

    // gridplane
    this->opt_e_gridplane_map_.insert(vcl_make_pair(e, grid_int())).first;
    
    // gridxy
    this->opt_e_gridxy_map_.insert(vcl_make_pair(e, grid_int())).first;
  }
  return;
}










// -----------------------------------------------------------------------------
//: Optimize a degree-2 fragment, i.e. given its state, what is the optimal
// energy and states of child fragment.
void dbsks_dp_match::
optimize_degree_2_node(const dbsksp_shock_node_sptr& v_p)
{
  assert (v_p->degree() == 2);

  vcl_cout << "Optimize degree-2 node ID = " << v_p->id() << "\n";

  // the "parent" edge
  dbsksp_shock_edge_sptr e_p = v_p->parent_edge();

  // the "child" edge
  dbsksp_shock_edge_sptr e_c = this->graph()->cyclic_adj_succ(e_p, v_p);


  // \DEBUG
  unsigned id_e_p = e_p->id();
  unsigned id_e_c = e_c->id();

  // shapelet grids associated with the edges
  dbsks_shapelet_grid& grid_c = this->shapelet_grid_map_.find(e_c)->second;
  dbsks_shapelet_grid& grid_p = this->shapelet_grid_map_.find(e_p)->second;

  // the existing cost tables 
  
  // child wrt to child
  grid_float& matchcost_c = this->matchcost_map_.find(e_c)->second;
  dbsksp_shapelet_sptr sref_c = this->ref_shapelet_map_.find(e_c)->second;
  
  // Compute matching cost for this fragment
  // (new approach: compute on-the-fly to save memory)
  this->compute_matching_cost(grid_c, sref_c, matchcost_c);


  // \DEBUG
  this->save_matchcost_to_file(e_c, matchcost_c);

  ///////////////

  grid_float& sum_bc_c = this->sum_bc_map_.find(e_c)->second;

  // check if this sum_bc_c has been allocated and initialzed
  // if not (leaf fragments), allocate and initialze to 0
  //if (sum_bc_c.size() != grid_c.num_planes())
  if (!this->load_sum_bc_from_file(e_c, sum_bc_c))
  {
    assert(sum_bc_c.empty());
    this->allocate(sum_bc_c, grid_c, 0.0f);
  }

  // add this cost to sum_bc_c and clear the memory
  int num_planes_c = grid_c.num_planes();
  for (int i_plane_c =0; i_plane_c < num_planes_c; ++i_plane_c)
  {
    sum_bc_c[i_plane_c] += matchcost_c[i_plane_c];
  }
  matchcost_c.clear();


  // Compute minimum energy for each of the starting xnode configuration of 
  // child fragment. This is to reduce the computation to linear.

  // a mapping from the set of starting xnode of the child fragments
  // to the optimal cost given the starting xnode
  grid_float opt_cost_sxnode_c;

  // a mapping from the set of starting xnode of the child fragment
  // to the optimal gridplane of the child fragment that corresponds to the optimal cost.
  grid_int opt_gridplane_sxnode_c;

  this->compute_min_cost_wrt_sxnode(grid_c, sum_bc_c, 
    opt_cost_sxnode_c, opt_gridplane_sxnode_c);
  sum_bc_c.clear();


  // child wrt to parent    
  vcl_cout << "Allocate space for opt_gridplane_c...\n";
  grid_int& opt_gridplane_c = this->opt_e_gridplane_map_.find(e_c)->second;
  this->allocate(opt_gridplane_c, grid_p, -1);

  // parent wrt to parent
  vcl_cout << "Allocate space for sum_bc_p...\n";
  grid_float& sum_bc_p = this->sum_bc_map_.find(e_p)->second;
  this->allocate(sum_bc_p, grid_p, 1e10);
  
  // The BIG LOOPS
  vcl_cout << "THE BIG LOOPS ... \n";
  vcl_cout << "ip_phiA = ";
  for (int ip_phiA =0; ip_phiA < grid_p.num_phiA_; ++ip_phiA)
  {
    vcl_cout << " " << ip_phiA;
    double phiA_p = grid_p.phiA_[ip_phiA];
    for (int ip_phiB =0; ip_phiB < grid_p.num_phiB_; ++ip_phiB)
    {
      double phiB_p = grid_p.phiB_[ip_phiB];
      for (int ip_m =0; ip_m < grid_p.num_m_; ++ip_m)
      {
        double m_p = grid_p.m_[ip_m];
        for (int ip_rA =0; ip_rA < grid_p.num_rA_; ++ip_rA)
        {
          double rA_p = grid_p.rA_[ip_rA];
          for (int ip_len =0; ip_len < grid_p.num_len_; ++ip_len)
          {
            double len_p = grid_p.len_[ip_len];

            // temporary values to check if the shapelet is legal
            double x0_p = grid_p.xA_[0];
            double y0_p = grid_p.yA_[0];
            double psi0_p = grid_p.psiA_[0];

            vgl_point_2d<double > pt0_p(x0_p, y0_p);
            vgl_vector_2d<double > t0_p(vcl_cos(psi0_p), vcl_sin(psi0_p));

            dbsksp_shapelet_sptr s0_p = new dbsksp_shapelet;
            if (!s0_p->set_from(pt0_p, rA_p, t0_p, phiA_p, m_p, len_p, phiB_p) ||
              !s0_p->is_legal())
            {
              continue;
            }


            // compute intrinsic parameters of this configuration and their indices
            double rA_c = s0_p->radius_end();
            double phiA_c = s0_p->phi_end();
            int ic_rA = grid_c.i_rA(rA_c);
            int ic_phiA = grid_c.i_phiA(phiA_c);

            //// skip if these params are not in the allowed range of the child shapelet
            //if (ic_rA <0 || ic_rA >= grid_c.num_rA_ ||
            //  ic_phiA <0 || ic_phiA >= grid_c.num_phiA_)
            //{
            //  continue;
            //}

            // \DEBUG
            ic_rA = dbsks_clip(ic_rA, 0, grid_c.num_rA_-1);
            ic_phiA = dbsks_clip(ic_phiA, 0, grid_c.num_phiA_-1);

            double theta0 = s0_p->theta0();

            // Translate and rotate the parent fragment
            // Search for the best child edge to attach to
            dbsksp_shapelet_sptr srot_p = new dbsksp_shapelet;

            for (int ip_psiA =0; ip_psiA < grid_p.num_psiA_; ++ip_psiA)
            {
              double psiA_p = grid_p.psiA_[ip_psiA];
              double theta_p = theta0 + (psiA_p - psi0_p);
              

              // convert to [-pi, pi] range
              theta_p = vcl_fmod(theta_p, 2*vnl_math::pi);
              theta_p = (theta_p > vnl_math::pi) ? (theta_p-2*vnl_math::pi) : theta_p;

              // rotate the original shapelet
              srot_p->set(x0_p, y0_p, theta_p, rA_p, phiA_p, phiB_p, m_p, len_p);

              // Compute the end_xnode of this shapelet. This will be the start_xnode
              // for the child-shapeplet and thus will determine its parameters
              // When we translate the parent, these will just need to be translated
              // we don't have to recompute them

              // the xnode of child fragment
              vgl_point_2d<double > pt0_c = srot_p->end();
              vgl_vector_2d<double > tA_c = srot_p->shock_geom().tangent_at(1);
              double psiA_c = vcl_atan2(tA_c.y(), tA_c.x());

              // compute their associated parameters
              int ic_psiA = grid_c.i_psiA(psiA_c);
              

              // translate the parent shapelet around and find its optimal child shapelet
              // !!!!!!!!!!!!!!!!!!!
              // ASSUME the parent and child shapelets have the same step_x and step_y
              // !!!!!!!!!!!!!!!!!!!!!!!

              int ic0_xA = grid_c.i_xA(pt0_c.x());
              int ic0_yA = grid_c.i_yA(pt0_c.y());
              int ip_plane = 
                grid_p.grid_to_linear(ip_phiA, ip_phiB, ip_m, ip_rA, ip_len, ip_psiA);
              
              for (int ip_xA =0; ip_xA < grid_p.num_xA_; ++ip_xA)
              {
                // translate x-index of the child shapelet the same amount as the parent 
                // shapelet because they have the same step_x and step_y
                int ic_xA = ic0_xA + ip_xA;

                // skip when the child has to be outside the image range
                if (ic_xA <0 || ic_xA >= grid_c.num_xA_)
                  continue;

                for (int ip_yA =0; ip_yA < grid_p.num_yA_; ++ip_yA)
                {
                  int ic_yA = ic0_yA + ip_yA;

                  // skip when the child has to be outside the image range
                  if (ic_yA <0 || ic_yA >= grid_c.num_yA_)
                    continue;

                  // iterate thru all allowed child shapelets to find the optimal
                  // one for this parent shapelet
                  float min_cost_c = vnl_numeric_traits<float >::maxval;
                  int opt_ic_plane = -1;

                  
                  // Method: use results precomputed earlier
                  // This will save a lot of time
                  int ic_sxnode_plane = 
                    grid_c.sxnode_grid_to_linear(ic_phiA, ic_rA, ic_psiA);
                  min_cost_c = opt_cost_sxnode_c[ic_sxnode_plane](ic_xA, ic_yA);
                  opt_ic_plane = opt_gridplane_sxnode_c[ic_sxnode_plane](ic_xA, ic_yA);

                  assert (opt_ic_plane > -1);

                  // update opt_energy (optimal energy given parent's state) and opt_state
                  // (optimal plane and (x,y) coordinate of the children given the state
                  // of the parent
                  
                  opt_gridplane_c[ip_plane](ip_xA, ip_yA) = opt_ic_plane;
                  //opt_gridxy_c[ip_plane](ip_xA, ip_yA) = grid_c.grid_to_linear(ic_xA, ic_yA);
                  sum_bc_p[ip_plane](ip_xA, ip_yA) = min_cost_c;
                } // ip_yA
              } // ip_xA
            } // ip_psiA
          } // ip_len
        } // ip_rA          
      } // ip_m
    } // ip_phiB      
  } //ip_phiA
  vcl_cout << "\n";

  // save sum_bc to file
  vcl_cout << "Save_sum_bc_to_file...\n";
  this->save_sum_bc_to_file(e_p, sum_bc_p);
  sum_bc_p.clear();

  // save opt_gridplane to file and clear the function's memory
  vcl_cout << "Save_opt_gridplane_to_file...\n";
  this->save_opt_gridplane_to_file(e_c, opt_gridplane_c);
  opt_gridplane_c.clear();

  vcl_cout << "Done.\n";
  return;
}


// -----------------------------------------------------------------------------
//: Optimize a degree-3 fragment, i.e. given its state, what is the optimal
// energy and states of its child fragment.
void dbsks_dp_match::
optimize_degree_3_node(const dbsksp_shock_node_sptr& v_p)
{
  assert (v_p->degree() == 3);

  // the "parent" edge
  dbsksp_shock_edge_sptr e_p = v_p->parent_edge();

  // the "child" edges
  dbsksp_shock_edge_sptr e_c1 = this->graph()->cyclic_adj_succ(e_p, v_p);
  dbsksp_shock_edge_sptr e_c2 = this->graph()->cyclic_adj_succ(e_c1, v_p);

  // shapelet grids associated with the edges
  dbsks_shapelet_grid& grid_p = this->shapelet_grid_map_.find(e_p)->second;
  dbsks_shapelet_grid& grid_c1 = this->shapelet_grid_map_.find(e_c1)->second;
  dbsks_shapelet_grid& grid_c2 = this->shapelet_grid_map_.find(e_c2)->second;

  // reference shapelet for each child edge
  dbsksp_shapelet_sptr sref_c1 = this->ref_shapelet_map_.find(e_c1)->second;
  dbsksp_shapelet_sptr sref_c2 = this->ref_shapelet_map_.find(e_c2)->second;

  // the existing cost tables 
  
  // child1 wrt child1 ----------------------
  grid_float& matchcost_c1 = this->matchcost_map_.find(e_c1)->second;
  grid_float& sum_bc_c1 = this->sum_bc_map_.find(e_c1)->second;  

  // Compute matching cost for this fragment (compute on-the-fly to save memory)
  this->compute_matching_cost(grid_c1, sref_c1, matchcost_c1);
  
  // \DEBUG
  this->save_matchcost_to_file(e_c1, matchcost_c1);

  ///////////////

  // if sum_bc_c1 hasn't been allocated and initialzed, i.e., leaf fragments, 
  // allocate and initialze to 0
  if (!this->load_sum_bc_from_file(e_c1, sum_bc_c1))
  {
    assert(sum_bc_c1.empty());
    this->allocate(sum_bc_c1, grid_c1, 0.0f);
  }

  // add this cost to sum_bc_c and clear the memory
  int num_planes_c1 = grid_c1.num_planes();
  for (int i_plane_c1 =0; i_plane_c1 < num_planes_c1; ++i_plane_c1)
  {
    sum_bc_c1[i_plane_c1] += matchcost_c1[i_plane_c1];
  }
  matchcost_c1.clear();


  // Compute minimum energy for each configuration of the childrens' start xnode
  // Child 1
  // a mapping from the starting xnode to the optimal cost
  grid_float opt_cost_sxnode_c1;
  
  // a mapping from starting xnode to the optimal gridplane of the child fragment 
  grid_int opt_gridplane_sxnode_c1;

  this->compute_min_cost_wrt_sxnode(grid_c1, sum_bc_c1, 
    opt_cost_sxnode_c1, opt_gridplane_sxnode_c1);
  sum_bc_c1.clear();

  // child2 wrt child2 ----------------------------
  grid_float& matchcost_c2 = this->matchcost_map_.find(e_c2)->second;
  grid_float& sum_bc_c2 = this->sum_bc_map_.find(e_c2)->second;  


  // Compute matching cost for this fragment (compute on-the-fly to save memory)
  this->compute_matching_cost(grid_c2, sref_c2, matchcost_c2);
  
  // \DEBUG
  this->save_matchcost_to_file(e_c2, matchcost_c2);
  ///////////////

  // if this grid hasn't been allocated and initialzed, i.e., leaf fragments, 
  // allocate and initialze to 0
  if (!this->load_sum_bc_from_file(e_c2, sum_bc_c2))
  {
    assert(sum_bc_c2.empty());
    this->allocate(sum_bc_c2, grid_c2, 0.0f);
  }

  // add this cost to sum_bc_c and clear the memory
  int num_planes_c2 = grid_c2.num_planes();
  for (int i_plane_c2 =0; i_plane_c2 < num_planes_c2; ++i_plane_c2)
  {
    sum_bc_c2[i_plane_c2] += matchcost_c2[i_plane_c2];
  }
  matchcost_c2.clear();


  // Compute minimum energy for each configuration of the childrens' start xnode
  // Child 2
  // mappings from the starting xnode to the optimal cost and optimal 
  // gridplane of the child fragment 
  grid_float opt_cost_sxnode_c2;
  grid_int opt_gridplane_sxnode_c2;

  this->compute_min_cost_wrt_sxnode(grid_c2, sum_bc_c2, 
    opt_cost_sxnode_c2, opt_gridplane_sxnode_c2);
  sum_bc_c2.clear();


  // optimal states of children wrt to parent      
  grid_int& opt_gridplane_c1 = this->opt_e_gridplane_map_.find(e_c1)->second;
  this->allocate(opt_gridplane_c1, grid_p, -1); 

  grid_int& opt_gridplane_c2 = this->opt_e_gridplane_map_.find(e_c2)->second;
  this->allocate(opt_gridplane_c2, grid_p, -1);
  
  
  // parent wrt to parent
  grid_float& sum_bc_p = this->sum_bc_map_.find(e_p)->second;
  this->allocate(sum_bc_p, grid_p, 1e10);


  // \DEBUG
  bool flag_has_legal = false;
  int edge_id_p = e_p->id();

  // The BIG LOOPS
  vcl_cout << "THE BIG LOOPS ... \n";
  vcl_cout << "ip_phiA = ";
  for (int ip_phiA =0; ip_phiA < grid_p.num_phiA_; ++ip_phiA)
  {
    vcl_cout << " " << ip_phiA;
    double phiA_p = grid_p.phiA_[ip_phiA];
    for (int ip_phiB =0; ip_phiB < grid_p.num_phiB_; ++ip_phiB)
    {
      double phiB_p = grid_p.phiB_[ip_phiB];
      for (int ip_m =0; ip_m < grid_p.num_m_; ++ip_m)
      {
        double m_p = grid_p.m_[ip_m];
        for (int ip_rA =0; ip_rA < grid_p.num_rA_; ++ip_rA)
        {
          double rA_p = grid_p.rA_[ip_rA];
          for (int ip_len =0; ip_len < grid_p.num_len_; ++ip_len)
          {
            double len_p = grid_p.len_[ip_len];

            // temporary values to check if the shapelet is legal
            double x0_p = grid_p.xA_[0];
            double y0_p = grid_p.yA_[0];
            double psi0_p = grid_p.psiA_[0];

            vgl_point_2d<double > pt0_p(x0_p, y0_p);
            vgl_vector_2d<double > t0_p(vcl_cos(psi0_p), vcl_sin(psi0_p));

            dbsksp_shapelet_sptr s0_p = new dbsksp_shapelet;
            if (!s0_p->set_from(pt0_p, rA_p, t0_p, phiA_p, m_p, len_p, phiB_p) ||
              !s0_p->is_legal())
            {
              continue;
            }

            // compute intrinsic parameters of this configuration and their indices
            double rB_p = s0_p->radius_end();
            int ic1_rA = grid_c1.i_rA(rB_p);
            int ic2_rA = grid_c2.i_rA(rB_p);
            
            //// skip if these params don't in the allowed range of the child shapelet
            //if (ic1_rA <0 || ic1_rA >= grid_c1.num_rA_ ||
            //  ic2_rA <0 || ic2_rA >= grid_c2.num_rA_)
            //{
            //  continue;
            //}

            // \DEBUG
            ic1_rA = dbsks_clip(ic1_rA, 0, grid_c1.num_rA_-1);
            ic2_rA = dbsks_clip(ic2_rA, 0, grid_c2.num_rA_-1);
            ////////////////////////////////////////////

            double theta0 = s0_p->theta0();

            // Translate and rotate the parent fragment
            // Search for the best child edge to attach to
            dbsksp_shapelet_sptr srot_p = new dbsksp_shapelet;

            for (int ip_psiA =0; ip_psiA < grid_p.num_psiA_; ++ip_psiA)
            {
              double psiA_p = grid_p.psiA_[ip_psiA];
              double theta_p = theta0 + (psiA_p - psi0_p);

              // convert to [-pi, pi] range
              theta_p = vcl_fmod(theta_p, 2*vnl_math::pi);
              theta_p = (theta_p > vnl_math::pi) ? (theta_p-2*vnl_math::pi) : theta_p;

              // rotate the original shapelet
              srot_p->set(x0_p, y0_p, theta_p, rA_p, phiA_p, phiB_p, m_p, len_p);

              // Compute the end_xnode of this shapelet. This will constrain the
              // of the two child-shapelets.
              // As we translate the parent, its end_xnode will be translated by the same amount

              // the xnode of child fragment
              vgl_point_2d<double > ptB_p = srot_p->end();
              vgl_vector_2d<double > tB_p = srot_p->shock_geom().tangent_at(1);
              double psiB_p = vcl_atan2(tB_p.y(), tB_p.x());

              // translate the parent shapelet around and find the optimal child
              // shapelet for position of the parent

              // !!!!!!!!!!!!!!!!!!!
              // ASSUME the parent and child shapelets have the same step_x and step_y
              // !!!!!!!!!!!!!!!!!!!!!!!

              int ic10_xA = grid_c1.i_xA(ptB_p.x());
              int ic10_yA = grid_c1.i_yA(ptB_p.y());

              int ic20_xA = grid_c2.i_xA(ptB_p.x());
              int ic20_yA = grid_c2.i_yA(ptB_p.y());

              int ip_plane = 
                grid_p.grid_to_linear(ip_phiA, ip_phiB, ip_m, ip_rA, ip_len, ip_psiA);
              
              for (int ip_xA =0; ip_xA < grid_p.num_xA_; ++ip_xA)
              {
                // translate x-index of the child shapelets the same amount as the parent 
                // shapelet because they have the same step_x and step_y
                int ic1_xA = ic10_xA + ip_xA;
                int ic2_xA = ic20_xA + ip_xA;

                // skip when one of the two children has to be outside the image range
                if (ic1_xA <0 || ic1_xA >= grid_c1.num_xA_ ||
                  ic2_xA <0 || ic2_xA >= grid_c2.num_xA_)
                {
                  continue;
                }

                for (int ip_yA =0; ip_yA < grid_p.num_yA_; ++ip_yA)
                {
                  int ic1_yA = ic10_yA + ip_yA;
                  int ic2_yA = ic20_yA + ip_yA;

                  // skip when the child has to be outside the image range
                  if (ic1_yA <0 || ic1_yA >= grid_c1.num_yA_ ||
                    ic2_yA <0 || ic2_yA >= grid_c2.num_yA_)
                  {
                    continue;
                  }

                  // iterate thru all allowed pairs of shapelets to find the optimal
                  // one for this parent shapelet
                  float min_cost_c = vnl_numeric_traits<float >::maxval;
                  int opt_ic1_plane = -1;
                  int opt_ic2_plane = -1;

                 // check all combinations of phiA_c1 and phiA_c2 such that their sum is
                  // phiB_p

                  double phiB_p = s0_p->phi_end();
                  for (int ic1_phiA =0; ic1_phiA < grid_c1.num_phiA_; ++ic1_phiA)
                  {
                    double phiA_c1 = grid_c1.phiA_[ic1_phiA];

                    // sum of the three phi's is pi
                    double phiA_c2 = phiB_p - phiA_c1;
                    int ic2_phiA = grid_c2.i_phiA(phiA_c2);

                    //// ignore out-of-bound cases
                    //if (ic2_phiA <0 || ic2_phiA >= grid_c2.num_phiA_)
                    //{
                    //  continue;
                    //}

                    // \DEBUG
                    ic2_phiA = dbsks_clip(ic2_phiA, 0, grid_c2.num_phiA_-1);
                    /////////////////////////////////////////////////////

                    // compute shock tangents for the two child fragments
                    double psiA_c1 = psiB_p - phiB_p + phiA_c1;
                    double psiA_c2 = psiA_c1 + phiA_c1 + phiA_c2;

                    int ic1_psiA = grid_c1.i_psiA(psiA_c1);
                    int ic2_psiA = grid_c2.i_psiA(psiA_c2);

                    // Use precomputed results to speed things up
                    int ic1_sxnode_plane = 
                      grid_c1.sxnode_grid_to_linear(ic1_phiA, ic1_rA, ic1_psiA);
                    int ic2_sxnode_plane = 
                      grid_c2.sxnode_grid_to_linear(ic2_phiA, ic2_rA, ic2_psiA);

                    // best costs
                    float min_cost_c1 = opt_cost_sxnode_c1[ic1_sxnode_plane](ic1_xA, ic1_yA);
                    float min_cost_c2 = opt_cost_sxnode_c2[ic2_sxnode_plane](ic2_xA, ic2_yA);

                    if (min_cost_c > (min_cost_c1 + min_cost_c2))
                    {
                      min_cost_c = min_cost_c1 + min_cost_c2;
                      opt_ic1_plane = opt_gridplane_sxnode_c1[ic1_sxnode_plane](ic1_xA, ic1_yA);
                      opt_ic2_plane = opt_gridplane_sxnode_c2[ic2_sxnode_plane](ic2_xA, ic2_yA);
                    }
                  }

                  //assert (opt_ic1_plane > -1);
                  //assert (opt_ic2_plane > -1);


                  // update opt_energy (optimal energy given parent's state) and opt_state
                  // (optimal plane and (x,y) coordinate of the children given the state
                  // of the parent
                  
                  opt_gridplane_c1[ip_plane](ip_xA, ip_yA) = opt_ic1_plane;

                  opt_gridplane_c2[ip_plane](ip_xA, ip_yA) = opt_ic2_plane;
  
                  sum_bc_p[ip_plane](ip_xA, ip_yA) = min_cost_c;

                  // \DEBUG
                  flag_has_legal = true;
                } // ip_yA
              } // ip_xA
            } // ip_psiA
          } // ip_len
        } // ip_rA          
      } // ip_m
    } // ip_phiB      
  } //ip_phiA
  vcl_cout << "\n";


  if (!flag_has_legal)
  {
    vcl_cout << "ERROR: There is no legal configuration at this step.\n";
  }

  // save sum_bc to file
  this->save_sum_bc_to_file(e_p, sum_bc_p);
  sum_bc_p.clear();

  // save opt_gridplane to file and clear the function's memory
  this->save_opt_gridplane_to_file(e_c1, opt_gridplane_c1);
  opt_gridplane_c1.clear();

  this->save_opt_gridplane_to_file(e_c2, opt_gridplane_c2);
  opt_gridplane_c2.clear();

  return;
}










// Given a grid of shapelets and its (sub-tree)accumulative cost, 
// for each configuration of the starting xnode of the shapelet, compute 
// the shapelet that gives the minimum energy.
void dbsks_dp_match::
compute_min_cost_wrt_sxnode(const dbsks_shapelet_grid& grid, const grid_float& sum_bc, 
                            grid_float& opt_cost_sxnode, 
                            grid_int& opt_gridplane_sxnode)
{
 
  // allocate memory for the two containers of returned results
  this->allocate_sxnode(opt_cost_sxnode, grid, vnl_numeric_traits<float >::maxval);
  this->allocate_sxnode(opt_gridplane_sxnode, grid, -1);
 
  vcl_cout << "Find minimum energy for each starting xnode configuration\n";
  vcl_cout << "  i_phiA = ";
  for (int i_phiA =0; i_phiA < grid.num_phiA_; ++i_phiA)
  {
    vcl_cout << " " << i_phiA;
    for (int i_rA =0; i_rA < grid.num_rA_; ++i_rA)
    {      
      for (int i_psiA =0; i_psiA < grid.num_psiA_; ++i_psiA)
      {
        // plane index for the grid containing optimal energy
        int i_sxnode_plane = grid.sxnode_grid_to_linear(i_phiA, i_rA, i_psiA);
        vnl_matrix<float >& opt_cost_plane = opt_cost_sxnode[i_sxnode_plane];
        vnl_matrix<int >& opt_gridplane_plane = opt_gridplane_sxnode[i_sxnode_plane];

        assert(grid.num_xA_ == opt_cost_plane.rows());
        assert(grid.num_yA_ == opt_cost_plane.cols());
        assert(grid.num_xA_ == opt_gridplane_plane.rows());
        assert(grid.num_yA_ == opt_gridplane_plane.cols());

        // iterate thru the other three parameters to find the best
        for (int i_phiB =0; i_phiB < grid.num_phiB_; ++i_phiB)
        {
          for (int i_m =0; i_m < grid.num_m_; ++i_m)
          {
            for (int i_len =0; i_len < grid.num_len_; ++i_len)
            {
              int i_plane = grid.grid_to_linear(i_phiA, i_phiB, i_m, 
                i_rA, i_len, i_psiA);
              const vnl_matrix<float >& sum_bc_plane = sum_bc[i_plane];

              assert(grid.num_xA_ == sum_bc_plane.rows());
              assert(grid.num_yA_ == sum_bc_plane.cols());

              // translate and find the min for each configuration of starting xnode
              for (int i_xA =0; i_xA < grid.num_xA_; ++i_xA)
              {
                for (int i_yA =0; i_yA < grid.num_yA_; ++i_yA)
                {
                  if (sum_bc_plane(i_xA, i_yA) < opt_cost_plane(i_xA, i_yA))
                  {
                    opt_cost_plane(i_xA, i_yA) = sum_bc_plane(i_xA, i_yA);
                    opt_gridplane_plane(i_xA, i_yA) = i_plane;
                  }
                }
              }                
            }
          }
        }
      }
    }
  }
  vcl_cout << "done.\n";
}



















// ----------------------------------------------------------------------------
//: Compute image cost for a grid of terminal shapelet using precomputed costs
// of a grid of ciruclar acs
// The return costs are the NORMALIZED image cost (total cost / total_length)
void dbsks_dp_match::
compute_image_cost_using_arc_cost(const dbsks_shapelet_grid& grid,
                                  vbl_array_1d<vnl_matrix<float > >& image_cost, 
                                  const dbsks_circ_arc_grid& arc_grid,
                                  const vbl_array_1d<vnl_matrix<float > >& arc_image_cost)
{
  // default value for average point cost for those not covered by the arc grid
  float max_pt_cost = this->oriented_chamfer()->max_cost();


  bool included[3];
  included[1] = true;
  included[0] = grid.has_front_arc_;
  included[2] = grid.has_rear_arc_;

  // Check the readiness of the circular arc grid
  if (arc_image_cost.size() != arc_grid.num_planes() ||
    arc_image_cost[0].size() != (arc_grid.num_x_*arc_grid.num_y_) )
  {
    vcl_cout << "ERROR in compute_image_cost_using_arc_cost: \n" 
      << "  arc_image_cost has not been computed.\n";
    return;
  }

  float ratio_step_x = float(grid.step_xA_ / arc_grid.step_x_);
  float ratio_step_y = float(grid.step_yA_ / arc_grid.step_y_);

  // compute image costs for all the arcs in the image
  vcl_cout << "Compute image costs for the shapelet grid\n";
  
  // allocate memory to save result
  // we organize the result as an array of a 2D matrices (planes)
  // each 2D matrix saves the cost for one shapelet configuration translated around the image

  this->allocate(image_cost, grid, max_pt_cost);

  int frag_count = 0;
  int legal_count = 0; // count the number of legal shapelets

  vcl_cout << "i_phiA = ";
  for (int i_phiA=0; i_phiA < grid.num_phiA_; ++i_phiA)
  {
    vcl_cout << "  " << i_phiA;
    
    double phiA = grid.phiA_[i_phiA];
    for (int i_phiB=0; i_phiB < grid.num_phiB_; ++i_phiB)
    {
      double phiB = grid.phiB_[i_phiB];
      for (int i_m=0; i_m < grid.num_m_; ++i_m)
      {
        double m = grid.m_[i_m];
        for (int i_rA=0; i_rA<grid.num_rA_; ++i_rA)
        {
          double rA = grid.rA_[i_rA];
          for (int i_len=0; i_len < grid.num_len_; ++i_len)
          {
            double len = grid.len_[i_len];
            double x0 = grid.xA_[0];
            double y0 = grid.yA_[0];
            double psi0 = grid.psiA_[0];

            vgl_point_2d<double > origin(x0, y0);
            vgl_vector_2d<double> t0(vcl_cos(psi0), vcl_sin(psi0));
            dbsksp_shapelet_sptr frag = new dbsksp_shapelet;
            
            // skip all the rotation if the fragment is illegal
            if (!frag->set_from(origin, rA, t0, phiA, m, len, phiB) || !frag->is_legal())
            {
              frag_count += grid.num_psiA_;
              continue;
            }

            ++legal_count;

            // iterate thru rotation and translation
            for (int i_psiA = 0; i_psiA < grid.num_psiA_; ++i_psiA, ++frag_count)
            {
              double psiA_diff = grid.psiA_[i_psiA] - psi0;

              
              frag->set_from(origin, rA, rotated(t0, psiA_diff), phiA, m, len, phiB);

              // the circular arcs on the left of the fragment, starting with front
              dbgl_circ_arc left_arcs[3];
              left_arcs[1] = frag->bnd_arc_left();
              // front arcs (at the starting shock point)
              left_arcs[0].set_from(left_arcs[1].start(), frag->start(), frag->tangent_start());
              // rear arcs (at the ending shock point)
              left_arcs[2].set_from(left_arcs[1].end(), frag->end(), -frag->tangent_end());

              // the circular arcs on the right of the fragment, starting with front
              dbgl_circ_arc right_arcs[3];
              right_arcs[1] = frag->bnd_arc_right();
              // front arcs (at the starting shock point)
              right_arcs[0].set_from(right_arcs[1].start(), frag->start(), 
                frag->tangent_start());
              // rear arcs (at the ending shock point)
              right_arcs[2].set_from(right_arcs[1].end(), frag->end(), -frag->tangent_end());


              // Deal with each portion of the boundary sequentially
              vnl_matrix<float> total_cost(grid.num_xA_, grid.num_yA_, 0.0f);
              float total_len = 0;
              for (int i_part = 0; i_part < 3; ++i_part)
              {
                if (!included[i_part]) 
                  continue;

                

                dbgl_circ_arc left_arc = left_arcs[i_part];
                dbgl_circ_arc right_arc = right_arcs[i_part];

                // Match the fragment's arcs to the pre-computed arcs

                // the indices of left arcs
                int i0_x_left, i0_y_left;
                int i_theta_left, i_chord_left, i_height_left;
                arc_grid.arc_to_grid(left_arc, i0_x_left, i0_y_left, 
                  i_chord_left, i_height_left, i_theta_left);
                double len_left = left_arc.length();
                
                // the indices of right arcs
                int i0_x_right, i0_y_right;
                int i_theta_right, i_chord_right, i_height_right;
                arc_grid.arc_to_grid(right_arc, i0_x_right, i0_y_right, 
                    i_chord_right, i_height_right, i_theta_right);
                double len_right = right_arc.length();

                // update total of length of all the parts
                total_len = float(len_left + len_right);

                // in-bound check
                bool out_of_bound = false;
                out_of_bound = i_chord_left < 0 || i_chord_left >= arc_grid.num_chord_ ||
                  i_height_left < 0 || i_height_left >= arc_grid.num_height_ ||
                  i_chord_right < 0 || i_chord_right >= arc_grid.num_chord_ ||
                  i_height_right < 0 || i_height_right >= arc_grid.num_height_;


                // default value for the cost
                float default_part_cost = 
                  float(len_left*max_pt_cost + len_right*max_pt_cost);
                vnl_matrix<float > part_cost(grid.num_xA_, grid.num_yA_, default_part_cost);


                if (!out_of_bound)
                {                
                  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                  // we translate the arcs by the amount
                  // x-direction = ratio_step_x * i_xA
                  // y-direction = ratio_step_y * i_yA 
                  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                  // look up the energy of the shapelet's arcs from the pre-computed arc costs
                  
                  // DEBUG !!!!!!!!!!!!!!!!!!!
                  assert(frag_count == grid.grid_to_linear(i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA));
                  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                  int i_plane_left =
                    arc_grid.grid_to_linear(i_chord_left, i_height_left, i_theta_left);
                  int i_plane_right =
                    arc_grid.grid_to_linear(i_chord_right, i_height_right, i_theta_right);
                  const vnl_matrix<float >& arc_plane_left = arc_image_cost[i_plane_left];
                  const vnl_matrix<float >& arc_plane_right = arc_image_cost[i_plane_right] ;                
                  for (int i_xA=0; i_xA < grid.num_xA_; ++i_xA)
                  {
                    int t_x = vnl_math_rnd(ratio_step_x * i_xA);
                    int i_x_left =  t_x + i0_x_left;
                    int i_x_right = t_x + i0_x_right;

                    // skip arcs outside the boundary
                    if (i_x_left < 0 || i_x_left >= arc_grid.num_x_ || 
                      i_x_right <0 || i_x_right >= arc_grid.num_x_)
                    {
                      continue;
                    }
                  
                    for (int i_yA=0; i_yA < grid.num_yA_; ++i_yA)
                    {
                      int t_y = vnl_math_rnd(ratio_step_y * i_yA);
                      int i_y_left = t_y + i0_y_left;
                      int i_y_right = t_y + i0_y_right;

                      // skip arcs outside the boundary
                      if (i_y_left < 0 || i_y_left >= arc_grid.num_y_ || 
                          i_y_right <0 || i_y_right >= arc_grid.num_y_)
                      {
                        continue;
                      }

                      float avg_cost_left = arc_plane_left(i_x_left, i_y_left);;
                      float avg_cost_right = arc_plane_right(i_x_right, i_y_right);
                      part_cost(i_xA, i_yA) = 
                        float(len_left*avg_cost_left + len_right*avg_cost_right);
                    } // i_yA
                  } // i_xA
                } // out_of_bound

                total_cost += part_cost;
              } // i_part

              vnl_matrix<float>& shapelet_plane = image_cost[frag_count].as_ref();
              shapelet_plane = total_cost / total_len;
            } // i_psiA
          } // i_len
        } // i_rA
      } // i_m
    } // i_phiB
  } // i_phiA
  vcl_cout << "\n";

  vcl_cout << "Number of legal configuration = " << legal_count << vcl_endl;
  return;
}




// ----------------------------------------------------------------------------
//: Compute deformation cost (wrt a reference fragment) for a grid of shapelet
void dbsks_dp_match::
add_deform_cost(const dbsks_shapelet_grid& grid,
                const dbsksp_shapelet_sptr& ref_s0,
                vbl_array_1d<vnl_matrix<float > >& existing_cost)
{
  vcl_cout << "Compute deformation cost and add to existing cost: \n";

  vcl_cout << "  Loop thru all shapelets configs and compute the deformation cost.\n";
  vcl_cout << "    sigma_deform = " << this->sigma_deform_ << "\n";

  int frag_count = 0;
  vcl_cout << "i_phiA = ";
  for (int i_phiA=0; i_phiA < grid.num_phiA_; ++i_phiA)
  {
    vcl_cout << " " << i_phiA;
    double phiA = grid.phiA_[i_phiA];
    for (int i_phiB=0; i_phiB < grid.num_phiB_; ++i_phiB)
    {
      //vcl_cout << " " << i_phiB;
      double phiB = grid.phiB_[i_phiB];
      for (int i_m=0; i_m < grid.num_m_; ++i_m)
      {
        double m = grid.m_[i_m];
        for (int i_rA=0; i_rA<grid.num_rA_; ++i_rA)
        {
          double rA = grid.rA_[i_rA];
          for (int i_len=0; i_len < grid.num_len_; ++i_len)
          {
            double len = grid.len_[i_len];
            double x0 = grid.xA_[0];
            double y0 = grid.yA_[0];
            double psi0 = grid.psiA_[0];

            vgl_point_2d<double > origin(x0, y0);
            vgl_vector_2d<double> t0(vcl_cos(psi0), vcl_sin(psi0));
            dbsksp_shapelet_sptr frag = new dbsksp_shapelet;
            
            // skip all the rotation if the fragment is illegal
            float deformation;

            if (frag->set_from(origin, rA, t0, phiA, m, len, phiB) && frag->is_legal())
            { 
              deformation = dbsks_deform_cost(ref_s0, grid.has_front_arc_, 
                grid.has_rear_arc_ ,frag, float(this->sigma_deform_));
            }
            else
            {
              deformation = 1e10;
            }

            // iterate thru rotation and translation
            for (int i_psiA = 0; i_psiA < grid.num_psiA_; ++i_psiA, ++frag_count)
            {
              vnl_matrix<float>& cost_plane = existing_cost[frag_count].as_ref();
              for (int i_xA = 0; i_xA < grid.num_xA_; ++i_xA)
              {
                for (int i_yA = 0; i_yA < grid.num_yA_; ++i_yA)
                {
                  cost_plane(i_xA, i_yA) += deformation;
                } //i_yA
              } // i_xA
            } // i_psiA
          } // len
        } // rA
      } // m
    } // phiB
  } // phiA
  vcl_cout << "\n";
  return;
}








// ----------------------------------------------------------------------------
//: Compute matching cost (image cost + deform cost)
// for all shapelets in the grid, wrt to a reference shapelet
void dbsks_dp_match::
compute_matching_cost(const dbsks_shapelet_grid& grid,
                      const dbsksp_shapelet_sptr& ref_s0,
                      vbl_array_1d<vnl_matrix<float > >& matching_cost)
{
  // compute normalized image cost
  this->compute_image_cost_using_arc_cost(grid, matching_cost, 
    this->arc_grid_, this->arc_image_cost_);

  // add normalized deformation cost
  this->add_deform_cost(grid, ref_s0, matching_cost);

  // Multiply the costs by the size of the fragment
  float fragment_size = float(
    dbsks_fragment_size(ref_s0, grid.has_front_arc_, grid.has_rear_arc_));
  
  int num_planes = grid.num_planes();
  for (int i_plane = 0; i_plane < num_planes; ++i_plane)
  {
    matching_cost[i_plane] *= fragment_size;
  }

  return;
}





// -----------------------------------------------------------------------------
//: Allocate memory for a scalar function defined on a shapelet grid
// and set everything to a default value
void dbsks_dp_match::
allocate(grid_float& f, const dbsks_shapelet_grid& grid, float value)
{
  f.clear();
  int num_planes = grid.size() / (grid.num_xA_*grid.num_yA_);
  f.reserve(num_planes);
  for (int i=0; i< num_planes; ++i)
  {
    f.push_back(vnl_matrix<float >());
  }

  for (int i=0; i<num_planes; ++i)
  {
    f[i].set_size(grid.num_xA_, grid.num_yA_);
    f[i].fill(value);
  }

  return;
}


// -----------------------------------------------------------------------------
void dbsks_dp_match::
allocate(grid_int& f, const dbsks_shapelet_grid& grid, int value)
{
  f.clear();
  int num_planes = grid.size() / (grid.num_xA_*grid.num_yA_);
  f.reserve(num_planes);
  for (int i=0; i< num_planes; ++i)
  {
    f.push_back(vnl_matrix<int >());
  }

  for (int i=0; i<num_planes; ++i)
  {
    f[i].set_size(grid.num_xA_, grid.num_yA_);
    f[i].fill(value);
  }

  return;
}







//: Allocate memory for a scalar function defined on the set of starting 
// xnodes of a shapelet grid
void dbsks_dp_match::
allocate_sxnode(grid_float& f, const dbsks_shapelet_grid& grid, float value)
{
  f.clear();
  int num_planes = grid.sxnode_num_planes();
  f.reserve(num_planes);
  for (int i=0; i< num_planes; ++i)
  {
    f.push_back(vnl_matrix<float >());
  }

  for (int i=0; i<num_planes; ++i)
  {
    f[i].set_size(grid.num_xA_, grid.num_yA_);
    f[i].fill(value);
  }
  return;
}



void dbsks_dp_match::
allocate_sxnode(grid_int& f, const dbsks_shapelet_grid& grid , int value)
{
  //
  f.clear();
  int num_planes = grid.sxnode_num_planes();
  f.reserve(num_planes);
  for (int i=0; i< num_planes; ++i)
  {
    f.push_back(vnl_matrix<int >());
  }

  for (int i=0; i<num_planes; ++i)
  {
    f[i].set_size(grid.num_xA_, grid.num_yA_);
    f[i].fill(value);
  }

  return;
}


// ----------------------------------------------------------------------------
//: Release all memory inside the matrices of the grid, keep the matrices (for reuse)
void dbsks_dp_match::
release_matrix_memory(vcl_map<dbsksp_shock_edge_sptr, grid_float >& grid_map)
{
  for (vcl_map<dbsksp_shock_edge_sptr, grid_float >::iterator git = 
    grid_map.begin(); git != grid_map.end(); ++git)
  {
    grid_float& f = git->second;
    for (unsigned i_plane =0; i_plane <f.size(); ++i_plane)
    {
      f[i_plane].clear();
    }
  }
  return;

}



// ----------------------------------------------------------------------------
//:
void dbsks_dp_match::
release_matrix_memory(vcl_map<dbsksp_shock_edge_sptr, grid_int >& grid_map)
{
  for (vcl_map<dbsksp_shock_edge_sptr, grid_int >::iterator git = 
    grid_map.begin(); git != grid_map.end(); ++git)
  {
    grid_int& f = git->second;
    for (unsigned i_plane =0; i_plane <f.size(); ++i_plane)
    {
      f[i_plane].clear();
    }
  }
  return;
}










// ----------------------------------------------------------------------------
//: retrieve optimal states for all the nodes
void dbsks_dp_match::
trace_opt_state(const dbsksp_shock_edge_sptr& e_root, 
                vgl_point_2d<int > opt_i_state_root, 
                vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_opt_i_state)
{
  vcl_cout << " aaa ";
  graph_opt_i_state.insert(vcl_make_pair(e_root, opt_i_state_root));

  vcl_cout << " aa ";
  dbsksp_shock_node_sptr v_c = (e_root->source()->parent_edge() == e_root) ?
    e_root->source() : e_root->target();

  vcl_cout << " a ";

  // More details optimal state of the root
  int opt_i_xy_root = opt_i_state_root.x();
  int opt_i_plane_root = opt_i_state_root.y();
  
  dbsks_shapelet_grid& grid_root = this->shapelet_grid_map_.find(e_root)->second;
  int opt_i_xA_root, opt_i_yA_root;
  grid_root.linear_to_grid(opt_i_xy_root, opt_i_xA_root, opt_i_yA_root);
  
  vcl_cout << " b ";


  dbsksp_shapelet_sptr s_root = grid_root.shapelet(opt_i_xy_root, opt_i_plane_root);
  vgl_point_2d<double > ptB_root = s_root->end();

  vcl_cout << " c ";

  // Recurve on all the child fragments
  for (dbsksp_shock_node::edge_iterator eit = v_c->edges_begin(); 
    eit != v_c->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    
    // we only want to consider the child edges
    if (e == e_root)
      continue;

    // there is no state for leaf nodes
    if (e->is_terminal_edge())
      continue;

    // retrieve optimal state for v given the state of root
    vcl_cout << " d" << e->id() << "d ";

    // plane
    grid_int& opt_gridplane_e = this->opt_e_gridplane_map_.find(e)->second;
    vcl_cout << " e ";

    // load the gridplane from a file
    this->load_opt_gridplane_from_file(e, opt_gridplane_e);
    vcl_cout << " f ";

    int opt_i_plane_e = opt_gridplane_e[opt_i_plane_root](opt_i_xA_root, opt_i_yA_root);
    vcl_cout << " g ";

    opt_gridplane_e.clear();

    // x and y
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
    vcl_cout << " h ";
    
    int opt_i_xA_e = grid_e.i_xA(ptB_root.x());
    int opt_i_yA_e = grid_e.i_yA(ptB_root.y());
    int opt_i_xy_e = grid_e.grid_to_linear(opt_i_xA_e, opt_i_yA_e);
    vcl_cout << " i ";

    vgl_point_2d<int > opt_i_state_e(opt_i_xy_e, opt_i_plane_e);
    
    this->trace_opt_state(e, opt_i_state_e, graph_opt_i_state);
    vcl_cout << " j ";
  }
  return;
}


//: ----------------------------------------------------------------------------
void dbsks_dp_match::
convert_i_state_map_to_shapelet_map(
  const vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& i_state_map,
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > & shapelet_map)
{
  shapelet_map.clear();
  
  for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::const_iterator it =
    i_state_map.begin(); it != i_state_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    
    int i_xy_e = it->second.x();
    int i_plane_e = it->second.y();
    
    // retrieve the shapelet corresponding to this state
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
    dbsksp_shapelet_sptr shapelet_e = grid_e.shapelet(i_xy_e, i_plane_e);

    shapelet_map.insert(vcl_make_pair(e, shapelet_e));
  }
  return;
}



// -----------------------------------------------------------------------------
//: Given a list of states for each edge, construct the graph
void dbsks_dp_match::
construct_graph(vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map,
                vcl_vector<dbsksp_shapelet_sptr >& fragment_list)
{
  fragment_list.clear();

  for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::iterator it =
    graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;

    if (e->is_terminal_edge())
      continue;
    
    int i_xy_e = it->second.x();
    int i_plane_e = it->second.y();
    
    // retrieve the shapelet corresponding to this state
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
    dbsksp_shapelet_sptr shapelet_e = grid_e.shapelet(i_xy_e, i_plane_e);
    fragment_list.push_back(shapelet_e);

    // add the front shapelet
    if (grid_e.has_front_arc_)
    {
      fragment_list.push_back(shapelet_e->terminal_shapelet_front());
    }
    
    // add the rear shapelet
    if (grid_e.has_rear_arc_)
    {
      fragment_list.push_back(shapelet_e->terminal_shapelet_rear());
    }
  }


  // START HERE : simplify this thing !!!!!!!!!!!!!!!!!!!!



  //fragment_list.clear();

  //for (vcl_map<unsigned int, vgl_point_2d<int > >::iterator it =
  //  graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  //{
  //  dbsksp_shock_edge_sptr e = this->graph()->edge_from_id(it->first);

  //  if (e->is_terminal_edge())
  //    continue;
  //  
  //  int i_xy_e = it->second.x();
  //  int i_plane_e = it->second.y();
  //  
  //  // retrieve the shapelet corresponding to this state
  //  dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
  //  dbsksp_shapelet_sptr shapelet_e = grid_e.shapelet(i_xy_e, i_plane_e);
  //  fragment_list.push_back(shapelet_e);

  //  // add the front shapelet
  //  if (grid_e.has_front_arc_)
  //  {
  //    fragment_list.push_back(shapelet_e->terminal_shapelet_front());
  //  }
  //  
  //  // add the rear shapelet
  //  if (grid_e.has_rear_arc_)
  //  {
  //    fragment_list.push_back(shapelet_e->terminal_shapelet_rear());
  //  }
  //}


  ////// construct a new graph
  ////dbsksp_shock_graph_sptr g = new dbsksp_shock_graph(this->graph());

  ////// For now, this will only work for one-chain graph



  ////vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;

  ////// First construct the shapelets corresponding to the states of the graph
  ////vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > shapelet_map;
  ////for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::const_iterator it =
  ////  graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  ////{
  ////  dbsksp_shock_edge_sptr e = it->first;
  ////  int i_xy = it->second.x();
  ////  int i_plane = it->second.y();
  ////  dbsks_shapelet_grid& grid = this->shapelet_grid_map_.find(e)->second;

  ////  // compute the shapelet associated with this state
  ////  dbsksp_shapelet_sptr s_e = grid.shapelet(i_xy, i_plane);
  ////  shapelet_map.insert(vcl_make_pair(e, s_e));
  ////}

  ////for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::iterator it = 
  ////  shapelet_map.begin(); it != shapelet_map.end(); ++it)
  ////{
  ////  dbsksp_shock_edge_sptr e = it->first;
  ////  dbsksp_shapelet_sptr s_e = it->second;
  ////  dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;

  ////  // collect all the circular arcs we need to draw
  ////  vcl_vector<dbgl_circ_arc > bnd_arc_list;
  ////  vcl_vector<dbgl_conic_arc > shock_conic_list;

  ////  // left and right
  ////  dbsksp_shock_node_sptr v_child = e->child_node();
  ////  dbsksp_shock_node_sptr v_parent = e->parent_node();

  ////  // interpolate to get smooth boundary
  ////  
  ////  //if (e->child_node()->degree() == 2 && !e1->is_terminal_edge())
  ////  if (v_child->degree() == 2 && !grid_e.has_rear_arc_ )
  ////  {
  ////    // compute two shapelets from two extrinsic nodes
  ////    vgl_point_2d<double > left_start_e = s_e->bnd_start(0);
  ////    vgl_point_2d<double > right_start_e = s_e->bnd_start(1);
  ////    vgl_point_2d<double > start_e = s_e->start();

  ////    dbsksp_xshock_node_descriptor start_xnode(start_e, left_start_e, right_start_e);

  ////    // the rear shapelet should come from the children
  ////    dbsksp_shock_edge_sptr e1 = this->graph()->cyclic_adj_succ(e, v_child);
  ////    dbsksp_shapelet_sptr s_e1 = shapelet_map.find(e1)->second;
  ////    vgl_point_2d<double > left_start_e1 = s_e1->bnd_start(0);
  ////    vgl_point_2d<double > right_start_e1 = s_e1->bnd_start(1);
  ////    vgl_point_2d<double > start_e1 = s_e1->start();
  ////    dbsksp_xshock_node_descriptor end_xnode(start_e1, left_start_e1, right_start_e1);

  ////    dbsksp_twoshapelet_sptr ss_e = 0;
  ////    dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
  ////    ss_e = interpolator.optimize();
  ////    dbsksp_shapelet_sptr s0_e = ss_e->shapelet_start();
  ////    dbsksp_shapelet_sptr s1_e = ss_e->shapelet_end();

  ////    // add the boundary of the two interpolating shapelets
  ////    bnd_arc_list.push_back(s0_e->bnd_arc_left());
  ////    bnd_arc_list.push_back(s0_e->bnd_arc_right());

  ////    bnd_arc_list.push_back(s1_e->bnd_arc_left());
  ////    bnd_arc_list.push_back(s1_e->bnd_arc_right());

  ////    // add the shocks of the two shapelets
  ////    shock_conic_list.push_back(s0_e->shock_geom());
  ////    shock_conic_list.push_back(s1_e->shock_geom());
  ////  }
  ////  else
  ////  {
  ////    bnd_arc_list.push_back(s_e->bnd_arc_left());
  ////    bnd_arc_list.push_back(s_e->bnd_arc_right());
  ////    shock_conic_list.push_back(s_e->shock_geom());
  ////  }

  ////  // front
  ////  if (grid_e.has_front_arc_)
  ////  {
  ////    dbsksp_shapelet_sptr s_front = s_e->terminal_shapelet_front();
  ////    bnd_arc_list.push_back(s_front->bnd_arc_left());
  ////    bnd_arc_list.push_back(s_front->bnd_arc_right());
  ////  }

  ////  // rear
  ////  if (grid_e.has_rear_arc_)
  ////  {
  ////    dbsksp_shapelet_sptr s_rear = s_e->terminal_shapelet_rear();
  ////    bnd_arc_list.push_back(s_rear->bnd_arc_left());
  ////    bnd_arc_list.push_back(s_rear->bnd_arc_right());
  ////  }


  ////  // Now include all the circular arcs and conics as polyline
  ////  
  ////  for (unsigned i =0; i < bnd_arc_list.size(); ++i)
  ////  {
  ////    dbgl_circ_arc arc = bnd_arc_list[i];
  ////    
  ////    // an arc as a 11-vertex polyline
  ////    vcl_vector<vsol_point_2d_sptr > pt_list;
  ////    for (double t=0; t<1; t = t+0.1)
  ////    {
  ////      vgl_point_2d<double > pt = arc.point_at(t);
  ////      pt_list.push_back(new vsol_point_2d(pt));
  ////    }

  ////    vsol_list.push_back(new vsol_polyline_2d(pt_list));
  ////  }

  ////  for (unsigned i =0; i < shock_conic_list.size(); ++i)
  ////  {
  ////    dbgl_conic_arc conic = shock_conic_list[i];
  ////    
  ////    // an conic as a 11-vertex polyline
  ////    vcl_vector<vsol_point_2d_sptr > pt_list;
  ////    for (double t=0; t<1; t = t+0.1)
  ////    {
  ////      vgl_point_2d<double > pt = conic.point_at(t);
  ////      pt_list.push_back(new vsol_point_2d(pt));
  ////    }
  ////    vsol_list.push_back(new vsol_polyline_2d(pt_list));    
  ////  }

  ////  // add the contact shocks
  ////  // front
  ////  vcl_vector<vsol_point_2d_sptr > pts_front;
  ////  pts_front.push_back(new vsol_point_2d(s_e->bnd_start(0)));
  ////  pts_front.push_back(new vsol_point_2d(s_e->start()));
  ////  pts_front.push_back(new vsol_point_2d(s_e->bnd_start(1)));
  ////  vsol_list.push_back(new vsol_polyline_2d(pts_front));

  ////  // rear
  ////  // only add if this is the terminal fragment
  ////  if (grid_e.has_rear_arc_)
  ////  {
  ////    vcl_vector<vsol_point_2d_sptr > pts_rear;
  ////    pts_rear.push_back(new vsol_point_2d(s_e->bnd_end(0)));
  ////    pts_rear.push_back(new vsol_point_2d(s_e->end()));
  ////    pts_rear.push_back(new vsol_point_2d(s_e->bnd_end(1)));
  ////    vsol_list.push_back(new vsol_polyline_2d(pts_rear));    
  ////  }
  ////}

  ////return vsol_list;

  return;
}



// -----------------------------------------------------------------------------
//: Given a list of states for each edge, construct the graph
void dbsks_dp_match::
construct_graph(vcl_map<unsigned int, vgl_point_2d<int > >& graph_i_state_map,
                vcl_vector<dbsksp_shapelet_sptr >& fragment_list)
{
  // convert from map<unsigned, vgl_point_2d > to map<shock_edge_sptr, vgl_point_2d >
  vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > > edge_state_map;
  for (vcl_map<unsigned int, vgl_point_2d<int > >::iterator it =
    graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = this->graph()->edge_from_id(it->first);
    edge_state_map.insert(vcl_make_pair(e, it->second));
  }
  this->construct_graph(edge_state_map, fragment_list);
  return;
}





// ----------------------------------------------------------------------------
//: Save the optimal gridplane of an edge to a file
void dbsks_dp_match::
save_opt_gridplane_to_file(const dbsksp_shock_edge_sptr& e, const grid_int& opt_gridplane)
{
  vcl_ostringstream oss;
  oss << this->temp_file_prefix_
    << "_opt_gridplane_edgeid_"
    << e->id()
    << ".tmp";
  vcl_string filepath = oss.str();

  // we will use the system temp_file command to get a temporary filename
  // filepath will only save the file name of the temp file
  //vcl_string data_file = vul_temp_filename();

  vcl_string data_file = this->temp_data_folder_ + 
    vul_file::strip_extension(vul_file::strip_directory(filepath));

  
  // save the filename
  vcl_ofstream os(filepath.c_str());
  os << data_file;
  os.close();

  // write grid to data_file
  vsl_b_ofstream bos(data_file);
  vsl_b_write(bos, opt_gridplane);
  bos.close();

  //// for debugging purpose
  //unsigned int grid_size = 0;
  //for (unsigned int i =0; i < opt_gridplane.size(); ++i)
  //{
  //  grid_size += opt_gridplane[i].size();
  //}

  vcl_cout << "filepath = " << filepath << "\n"
    << "   data_file = " << data_file << "\n"
    << "   Size of data file " << vul_file::size(data_file) << "\n";
    //<< "   Size of the grid  " << grid_size << "\n";


  

  // save the filename for future retrieval
  this->gridplane_filepath_map_.insert(vcl_make_pair(e, filepath));
  
  return;
}


// ----------------------------------------------------------------------------
//: Load the optimal gridplane of an edge to a file
bool dbsks_dp_match::
load_opt_gridplane_from_file(const dbsksp_shock_edge_sptr& e,grid_int& opt_gridplane)
{
  // check whether the file has been saved
  vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = this->gridplane_filepath_map_.find(e);
  if (it == this->gridplane_filepath_map_.end())
    return false;

  vcl_string filepath = it->second;
  if (!vul_file::exists(filepath))
  {
    return false;
  }
  
  // load the name of the datafile from filepath
  vcl_string data_file;
  vcl_ifstream is(filepath.c_str());
  is >> data_file;
  is.close();

  if (!vul_file::exists(data_file))
  {
    return false;
  }

  // load sum_bc from data_file
  opt_gridplane.clear();
  vsl_b_ifstream bis(data_file);
  vsl_b_read(bis, opt_gridplane);
  bis.close();


  //// Delay the deletion step.
  //// We may need to load these files several times

  //// Delete the files
  //vul_file::delete_file_glob(data_file.c_str());
  //vul_file::delete_file_glob(filepath.c_str());
  //this->gridplane_filepath_map_.erase(e);

  return true;
}





// ----------------------------------------------------------------------------
//: Save the optimal gridplane of an edge to a file
void dbsks_dp_match::
save_sum_bc_to_file(const dbsksp_shock_edge_sptr& e, const grid_float& sum_bc)
{
  vcl_ostringstream oss;
  oss << this->temp_file_prefix_
    << "_sum_bc_edgeid_"
    << e->id()
    << ".tmp";
  vcl_string filepath = oss.str();

  // we will use the system temp_file command to get a temporary filename
  // filepath will only save the file name of the temp file
  //vcl_string data_file = vul_temp_filename();

  vcl_string data_file = this->temp_data_folder_ + 
    vul_file::strip_extension(vul_file::strip_directory(filepath));
  
  // save the filename
  vcl_ofstream os(filepath.c_str());
  os << data_file;
  os.close();

  // write grid to data_file
  vsl_b_ofstream bos(data_file);
  vsl_b_write(bos, sum_bc);
  bos.close();

  vcl_cout << "filepath = " << filepath << "\n"
    << "    data_file = " << data_file << "\n"
    << "    Size of data file " << vul_file::size(data_file) << "\n";

  // save the filename for future retrieval
  this->sum_bc_filepath_map_.insert(vcl_make_pair(e, filepath));
 
  return;
}


// ----------------------------------------------------------------------------
//: Load the optimal gridplane of an edge to a file
bool dbsks_dp_match::
load_sum_bc_from_file(const dbsksp_shock_edge_sptr& e, grid_float& sum_bc )
{
  // check whether the file has been saved
  vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = 
    this->sum_bc_filepath_map_.find(e);
  
  if (it == this->sum_bc_filepath_map_.end())
    return false;

  vcl_string filepath = it->second;
  if (!vul_file::exists(filepath))
  {
    return false;
  }
  
  // load the name of the datafile from filepath
  vcl_string data_file;
  vcl_ifstream is(filepath.c_str());
  is >> data_file;
  is.close();

  if (!vul_file::exists(data_file))
  {
    return false;
  }

  // load sum_bc from data_file
  sum_bc.clear();
  vsl_b_ifstream bis(data_file);
  vsl_b_read(bis, sum_bc);
  bis.close();

  // Delete the file
  vul_file::delete_file_glob(data_file.c_str());
  vul_file::delete_file_glob(filepath.c_str());
  this->sum_bc_filepath_map_.erase(e);

  return true;
}











// -----------------------------------------------------------------------------
//: Save the match_cost of an edge to a file
void dbsks_dp_match::
save_matchcost_to_file(const dbsksp_shock_edge_sptr& e, 
                        const grid_float& matchcost)
{

  vcl_ostringstream oss;
  oss << this->temp_file_prefix_
    << "_match_cost_edgeid_"
    << e->id()
    << ".tmp";
  vcl_string filepath = oss.str();

  // we will use the system temp_file command to get a temporary filename
  // filepath will only save the file name of the temp file
  //vcl_string data_file = vul_temp_filename();

  vcl_string data_file = this->temp_data_folder_ + 
    vul_file::strip_extension(vul_file::strip_directory(filepath));
  
  // save the filename
  vcl_ofstream os(filepath.c_str());
  os << data_file;
  os.close();

  // write grid to data_file
  vsl_b_ofstream bos(data_file);
  vsl_b_write(bos, matchcost);
  bos.close();

  vcl_cout << "filepath = " << filepath << "\n"
    << "    data_file = " << data_file << "\n"
    << "    Size of data file " << vul_file::size(data_file) << "\n";


  assert(this->matchcost_filepath_map_.find(e) == this->matchcost_filepath_map_.end());

  // save the filename for future retrieval
  this->matchcost_filepath_map_.insert(vcl_make_pair(e, filepath));
 
  return;

}


// -----------------------------------------------------------------------------
//: Load the match_cost of an edge to a file
bool dbsks_dp_match::
load_matchcost_from_file(const dbsksp_shock_edge_sptr& e,
                          grid_float& matchcost )
{
  // check whether the file has been saved
  vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = 
    this->matchcost_filepath_map_.find(e);
  
  if (it == this->matchcost_filepath_map_.end())
  {
    vcl_cout << "Matchcost for edge id = " << e->id() << "does not exist.";
    return false;
  }

  vcl_string filepath = it->second;
  if (!vul_file::exists(filepath))
  {
    vcl_cout << "Could not load filepath for matchcost of edge id = " << e->id() << "\n.";
    return false;
  }
  
  // load the name of the datafile from filepath
  vcl_string data_file;
  vcl_ifstream is(filepath.c_str());
  is >> data_file;
  is.close();

  if (!vul_file::exists(data_file))
  {
    vcl_cout << "Could not datafile for matchcost of edge id = " << e->id() << "\n.";
    return false;
  }

  // load sum_bc from data_file
  matchcost.clear();
  vsl_b_ifstream bis(data_file);
  vsl_b_read(bis, matchcost);
  bis.close();

  //// Delete the file
  //vul_file::delete_file_glob(data_file.c_str());
  //vul_file::delete_file_glob(filepath.c_str());
  //this->matchcost_filepath_map_.erase(e);

  return true;


}



















// ----------------------------------------------------------------------------
//: Delete all temporary files
void dbsks_dp_match::
clear_all_temp_files()
{
  // go thru each filename and delete the files
  for (vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = 
    this->gridplane_filepath_map_.begin(); 
    it != this->gridplane_filepath_map_.end(); ++it)
  {
    vcl_string filepath = it->second;
    if (!vul_file::exists(filepath.c_str()))
    {
      continue;
    }

    // load the name of the datafile from filepath
    vcl_string data_file;
    vcl_ifstream is(filepath.c_str());
    is >> data_file;
    is.close();
    vul_file::delete_file_glob(filepath.c_str());

    if (vul_file::exists(data_file))
    {
      // Delete the file
      vul_file::delete_file_glob(data_file.c_str());
    }
  }

  // go thru each filename and delete the files
  for (vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = 
    this->sum_bc_filepath_map_.begin(); 
    it != this->sum_bc_filepath_map_.end(); ++it)
  {
    vcl_string filepath = it->second;
    if (!vul_file::exists(filepath.c_str()))
    {
      continue;
    }

    // load the name of the datafile from filepath
    vcl_string data_file;
    vcl_ifstream is(filepath.c_str());
    is >> data_file;
    is.close();
    vul_file::delete_file_glob(filepath.c_str());

    if (vul_file::exists(data_file))
    {
      // Delete the file
      vul_file::delete_file_glob(data_file.c_str());
    }
  }



  // go thru each filename and delete the files
  for (vcl_map<dbsksp_shock_edge_sptr, vcl_string >::iterator it = 
    this->matchcost_filepath_map_.begin(); 
    it != this->matchcost_filepath_map_.end(); ++it)
  {
    vcl_string filepath = it->second;
    if (!vul_file::exists(filepath.c_str()))
    {
      continue;
    }

    // load the name of the datafile from filepath
    vcl_string data_file;
    vcl_ifstream is(filepath.c_str());
    is >> data_file;
    is.close();
    vul_file::delete_file_glob(filepath.c_str());

    if (vul_file::exists(data_file))
    {
      // Delete the file
      vul_file::delete_file_glob(data_file.c_str());
    }
  }
  return;
}



// ----------------------------------------------------------------------------
//: Delete everything that may take memory
void dbsks_dp_match::
clear()
{
  this->clear_arc_cost_data();
  this->clear_all_temp_files();
  // TODO clear every other intermediate variables
}









// -----------------------------------------------------------------------------
//: Compute the cost of one hypothesized graph
// Assuming they have the same topology, with corresponding nodes and edges
// having the same id
float dbsks_dp_match::
compute_graph_cost(const dbsksp_shock_graph_sptr& test_graph,
                   bool approx_frags_w_shapelet_grid,
                   bool approx_bnd_w_circ_arc_grid) const
{
  float total_cost = 0;
  for (dbsksp_shock_graph::edge_iterator eit = test_graph->edges_begin();
    eit != test_graph->edges_end(); ++eit)
  {
    if ((*eit)->is_terminal_edge())
      continue;

    // test fragment
    dbsksp_shock_edge_sptr test_e = *eit;
    dbsksp_shapelet_sptr test_frag = test_e->fragment()->get_shapelet();

    // Determine whether we need to reverse its direction
    if (this->need_reverse_frag_dir(test_e->id()))
    {
      test_frag = test_frag->reversed_dir();
    }

    // model fragment
    dbsksp_shock_edge_sptr model_e = this->graph()->edge_from_id(test_e->id());
    dbsksp_shapelet_sptr model_frag = this->ref_shapelet_map_.find(model_e)->second;    
    const dbsks_shapelet_grid& grid = this->shapelet_grid_map_.find(model_e)->second;
    bool include_front_arc = grid.has_front_arc_;
    bool include_rear_arc = grid.has_rear_arc_;

    // total cost
    total_cost += this->matching_cost(model_frag, include_front_arc, 
      include_rear_arc, test_frag);    
  }
  return total_cost;
}



// -----------------------------------------------------------------------------
//: Compute the closest approximation of the given graph on the under the
// discretization of this DP
// Results are saved in the form of a map from edge ID --> approximated shapelet
bool dbsks_dp_match::
compute_closest_approx(const dbsksp_shock_graph_sptr& test_graph,
                       vcl_map<unsigned int, dbsksp_shapelet_sptr > approx_frags)
{
  for (dbsksp_shock_graph::edge_iterator eit = test_graph->edges_begin();
    eit != test_graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    dbsksp_shapelet_sptr s_e = e->fragment()->get_shapelet();
    if (this->need_reverse_frag_dir(e->id()))
    {
      s_e = s_e->reversed_dir();
    }

    dbsks_shapelet_grid& grid_e = this->shapelet_grid(e->id());

    // Computed indices of the approximated shapelet
    int i_xA = grid_e.i_xA(s_e->start().x());
    int i_yA = grid_e.i_yA(s_e->start().y());
    int i_psiA = grid_e.i_psiA(s_e->tangent_start());
    int i_rA = grid_e.i_rA(s_e->radius_start());
    int i_phiA = grid_e.i_phiA(s_e->phi_start());
    int i_len = grid_e.i_len(s_e->chord_length());
    int i_m = grid_e.i_m(s_e->m_start());
    int i_phiB = grid_e.i_phiB(s_e->phi_end());

    if (!grid_e.is_legal(i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA) ||
      !grid_e.is_legal(i_xA, i_yA))
      return false;

    int i_xy = grid_e.grid_to_linear(i_xA, i_yA);
    int i_plane = grid_e.grid_to_linear(i_phiA, i_phiB, i_m, i_rA, i_len, i_psiA);

    approx_frags.insert(vcl_make_pair(e->id(), grid_e.shapelet(i_xy, i_plane)));
  }
  return true;
}



// ---------------------------------------------------------------------------
// Compute cost of a fragment
bool dbsks_dp_match::
compute_frag_cost(unsigned int edge_id, const dbsksp_shapelet_sptr& edge_frag,
    float& total_cost,
    bool approx_frags_w_shapelet_grid,
    bool approx_bnd_w_circ_arc_grid,
    float* deform_cost,
    float* image_cost,
    float* frag_size) const
{
    // model fragment
    dbsksp_shock_edge_sptr model_e = this->graph()->edge_from_id(edge_id);
    assert ( !model_e->is_terminal_edge() );
    dbsksp_shapelet_sptr model_frag = this->ref_shapelet_map_.find(model_e)->second;    
    const dbsks_shapelet_grid& grid = this->shapelet_grid_map_.find(model_e)->second;
    bool include_front_arc = grid.has_front_arc_;
    bool include_rear_arc = grid.has_rear_arc_;


    // test fragment
    // Determine whether we need to reverse its direction
    dbsksp_shapelet_sptr test_frag = edge_frag;

    if (approx_frags_w_shapelet_grid)
    {
      test_frag = grid.approx_shapelet_on_grid(test_frag);
    }

    if (!approx_bnd_w_circ_arc_grid)
    {
      total_cost = this->matching_cost(model_frag, 
        include_front_arc,
        include_rear_arc,
        test_frag,
        deform_cost,
        image_cost,
        frag_size);

      return true;
    }
    else
    {
      // We don't handle the case of approximating boundary with circular arc grid yet
      return false;
    }
  return false;
}





// -----------------------------------------------------------------------------
//: Check if an edge fragment direction needs to be reversed to comply with the
// depth order of the nodes
bool dbsks_dp_match::
need_reverse_frag_dir(unsigned edge_id) const
{
  // Determine whether we need to reverse its direction
  dbsksp_shock_edge_sptr model_e = this->graph()->edge_from_id(edge_id);
  if (model_e->target()->parent_edge() != model_e )
    return true;
  else
    return false;
}

// -----------------------------------------------------------------------------
//: sum of deform cost + image cost
float dbsks_dp_match::
matching_cost(const dbsksp_shapelet_sptr& s_ref, 
              bool include_front_arc,
              bool include_rear_arc,
              const dbsksp_shapelet_sptr& s,
              float* deform_cost,
              float* image_cost,
              float* fragment_size) const
{
  float dc = dbsks_deform_cost(s_ref, include_front_arc, include_rear_arc, 
    s, float(this->sigma_deform_));
  float ic = this->oriented_chamfer()->f(s, include_front_arc, include_rear_arc,
    float(this->ds_shapelet_));


  // DEBUG - temporary - use arc cost//////////////////////
  bool compute_using_arc = true;
  if (compute_using_arc)
  {
    float max_pt_cost = this->oriented_chamfer()->max_cost();

    dbsksp_shapelet_sptr frag = s;
    ic = 0;

    bool included[3];
    included[1] = true;
    included[0] = include_front_arc;
    included[2] = include_rear_arc;

    float ratio_step_x = 1;
    float ratio_step_y = 1;

    const dbsks_circ_arc_grid& arc_grid = this->arc_grid_;
    const vbl_array_1d<vnl_matrix<float > >& arc_image_cost = this->arc_image_cost_;

    // the circular arcs on the left of the fragment, starting with front
    dbgl_circ_arc left_arcs[3];
    left_arcs[1] = frag->bnd_arc_left();
    // front arcs (at the starting shock point)
    left_arcs[0].set_from(left_arcs[1].start(), frag->start(), frag->tangent_start());
    // rear arcs (at the ending shock point)
    left_arcs[2].set_from(left_arcs[1].end(), frag->end(), -frag->tangent_end());

    // the circular arcs on the right of the fragment, starting with front
    dbgl_circ_arc right_arcs[3];
    right_arcs[1] = frag->bnd_arc_right();
    // front arcs (at the starting shock point)
    right_arcs[0].set_from(right_arcs[1].start(), frag->start(), 
      frag->tangent_start());
    // rear arcs (at the ending shock point)
    right_arcs[2].set_from(right_arcs[1].end(), frag->end(), -frag->tangent_end());


    // Deal with each portion of the boundary sequentially
    float total_cost = 0;
    float total_len = 0;
    for (int i_part = 0; i_part < 3; ++i_part)
    {
      if (!included[i_part]) 
        continue;

                

      dbgl_circ_arc left_arc = left_arcs[i_part];
      dbgl_circ_arc right_arc = right_arcs[i_part];

      // Match the fragment's arcs to the pre-computed arcs

      // the indices of left arcs
      int i0_x_left, i0_y_left;
      int i_theta_left, i_chord_left, i_height_left;
      arc_grid.arc_to_grid(left_arc, i0_x_left, i0_y_left, 
        i_chord_left, i_height_left, i_theta_left);
      double len_left = left_arc.length();
      
      // the indices of right arcs
      int i0_x_right, i0_y_right;
      int i_theta_right, i_chord_right, i_height_right;
      arc_grid.arc_to_grid(right_arc, i0_x_right, i0_y_right, 
          i_chord_right, i_height_right, i_theta_right);
      double len_right = right_arc.length();

      // update total of length of all the parts
      total_len = float(len_left + len_right);

      // in-bound check
      bool out_of_bound = false;
      out_of_bound = i_chord_left < 0 || i_chord_left >= arc_grid.num_chord_ ||
        i_height_left < 0 || i_height_left >= arc_grid.num_height_ ||
        i_chord_right < 0 || i_chord_right >= arc_grid.num_chord_ ||
        i_height_right < 0 || i_height_right >= arc_grid.num_height_;


      // default value for the cost
      float default_part_cost = 
        float(len_left*max_pt_cost + len_right*max_pt_cost);
      float part_cost = default_part_cost;


      if (!out_of_bound)
      {                
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // we translate the arcs by the amount
        // x-direction = ratio_step_x * i_xA
        // y-direction = ratio_step_y * i_yA 
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        // look up the energy of the shapelet's arcs from the pre-computed arc costs

        int i_plane_left =
          arc_grid.grid_to_linear(i_chord_left, i_height_left, i_theta_left);
        int i_plane_right =
          arc_grid.grid_to_linear(i_chord_right, i_height_right, i_theta_right);
        const vnl_matrix<float >& arc_plane_left = arc_image_cost[i_plane_left];
        const vnl_matrix<float >& arc_plane_right = arc_image_cost[i_plane_right] ;                
        ////
        //for (int i_xA=0; i_xA < grid.num_xA_; ++i_xA)
        //{
        int i_xA = 0;
          int t_x = vnl_math_rnd(ratio_step_x * i_xA);
          int i_x_left =  t_x + i0_x_left;
          int i_x_right = t_x + i0_x_right;

          // skip arcs outside the boundary
          if (i_x_left < 0 || i_x_left >= arc_grid.num_x_ || 
            i_x_right <0 || i_x_right >= arc_grid.num_x_)
          {
            continue;
          }
        
          ////
          //for (int i_yA=0; i_yA < grid.num_yA_; ++i_yA)
          //{
          int i_yA = 0;
            int t_y = vnl_math_rnd(ratio_step_y * i_yA);
            int i_y_left = t_y + i0_y_left;
            int i_y_right = t_y + i0_y_right;

            // skip arcs outside the boundary
            if (i_y_left < 0 || i_y_left >= arc_grid.num_y_ || 
                i_y_right <0 || i_y_right >= arc_grid.num_y_)
            {
              continue;
            }

            float avg_cost_left = arc_plane_left(i_x_left, i_y_left);;
            float avg_cost_right = arc_plane_right(i_x_right, i_y_right);
            part_cost =  float(len_left*avg_cost_left + len_right*avg_cost_right);
          //} // i_yA
        //} // i_xA
      } // out_of_bound

      total_cost += part_cost;
    } // i_part

    ic = total_cost / total_len;
}










  ////////////////////////////////



  float size = (float)dbsks_fragment_size(s_ref, include_front_arc, include_rear_arc);
  float total_cost = (dc + ic) * float(size);

  if (deform_cost)
    *deform_cost = dc;
  if (image_cost)
    *image_cost = ic;
  if (fragment_size)
    *fragment_size = size;
  
  return total_cost;
}










// -----------------------------------------------------------------------------
//: Adjust the centers of the shapelet grids to the center of the starting points
// of the shapelets in the provided maps
void dbsks_dp_match::
adjust_shapelet_grid_center(
     const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map)
{
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::const_iterator mit =
    shapelet_map.begin(); mit != shapelet_map.end(); ++mit)
  {
    dbsksp_shock_edge_sptr e = mit->first;
    dbsksp_shapelet_sptr s_e = mit->second;

    // Retrieve the shapelet grid associated with this edge
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
    grid_e.translate_grid_center_to(s_e->start().x(), s_e->start().y());
  }
  return;
}




// ----------------------------------------------------------------------------
//: Adjust the number of points used for the shapelet grids by changing the
// numbers of xA and yA
void dbsks_dp_match::
change_shapelet_grids_num_xy(double ratio_xA, double ratio_yA)
{
  for (vcl_map<dbsksp_shock_edge_sptr, dbsks_shapelet_grid >::iterator it =
    this->shapelet_grid_map_.begin(); it != this->shapelet_grid_map_.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    dbsks_shapelet_grid& grid_e = it->second;

    int new_num_xA = vnl_math_rnd(ratio_xA*(grid_e.num_xA_-1)/2) *2 + 1;
    grid_e.change_num_xA(new_num_xA);

    int new_num_yA = vnl_math_rnd(ratio_yA*(grid_e.num_yA_-1)/2) *2 + 1;
    grid_e.change_num_yA(new_num_yA);
  }

  return;
}











// ---------------------------------------------------------------------
//: Build shapelet grid for each edge given a shock graph statistics and shapelet_grid_params
// \TODO
void dbsks_dp_match::
build_shapelet_grids(const dbsks_shapelet_grid_params& params, 
                     const dbsks_shock_graph_stats& stats,
                     double graph_size,
      vcl_map<dbsksp_shock_edge_sptr, dbsks_shapelet_grid >& shapelet_grid_map)
{
  //// build shapelet_grid - set of all possible configurations for an edge

  dbsks_shapelet_sampling_params sampling_params;
  sampling_params.center_x = params.center_x;
  sampling_params.center_y = params.center_y;
  sampling_params.step_x = params.step_x;
  sampling_params.step_y = params.step_y;
  sampling_params.num_x = (2*params.half_num_x + 1);
  sampling_params.num_y = (2*params.half_num_y + 1);
  
  sampling_params.num_psi = params.num_psi;
  sampling_params.num_phiA = (2*params.half_num_phiA + 1);
  sampling_params.num_phiB = (2*params.half_num_phiB + 1);
  sampling_params.num_m = (2*params.half_num_m + 1);
  sampling_params.num_len = (2*params.half_num_len + 1);
  sampling_params.num_rA = (2*params.half_num_rA + 1);

  // --------------------------------------------------------------------------
  // Build a shapelet grid for each of the edges
  // terminal edges are grouped with the edge adjacent to them
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;

    //////////////////////////////////////////////////////////////////////////////
    // The reference shapelet should also come from the shock graph statistics
    // \TODO

    // retrieve the current (reference) shapelet for this edge
    dbsksp_shapelet_sptr sref = e->fragment()->get_shapelet();

    // make sure the shapelet is in the right direction, i.e. facing downward to its children
    if (e->target()->parent_edge() != e )
    {
      sref = sref->reversed_dir();
    }
    // save it for future use
    this->ref_shapelet_map_.insert(vcl_make_pair(e, sref));

    ////////////////////////////////////////////////////////////////////////////
    // build shapelet grids from statistics!!!!!!!!!!!

    dbsks_shapelet_grid grid = stats.build_shapelet_grid(e->id(), sampling_params, graph_size);
    shapelet_grid_map.insert(vcl_make_pair(e, grid));
  }
  return;
}













// MISCELLANEOUS functions -------------------------------------------------


//: Print Optimization results to a stream
void dbsks_dp_match::
print_summary(vcl_ostream& str) const
{
  str << "dbsks_dp_match_summary 0\n"
    << "image_ni " << this->image_ni_ << "\n"
    << "image_nj " << this->image_nj_ << "\n"
    << "sigma_deform " << this->sigma_deform_ << "\n"
    << "ds_shapelet_ " << this->ds_shapelet_ << "\n"
    << "ocm_edge_threshold " << this->oriented_chamfer()->edge_threshold_ << "\n"
    << "sigma_distance " << this->oriented_chamfer()->sigma_distance_ << "\n"
    << "sigma_angle " << this->oriented_chamfer()->sigma_angle_ << "\n";

  // model_height_
  str << "model_height " << this->model_height_ << "\n";

  // min_dp_cost_list
  str << "min_dp_cost " << this->min_dp_cost_ << "\n";

  // min_real_cost_list
  str << "min_real_cost " << this->min_real_cost_ << "\n";

  // final cost break down
  str << "final_dp_cost_break_down 0\n";

  //for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
  //  eit != this->graph()->edges_end(); ++eit)
  //{
  //  if ((*eit)->is_terminal_edge())
  //    continue;

  //  dbsksp_shock_edge_sptr e = *eit;

  //  ////
  //  //matching_cost(const dbsksp_shapelet_sptr& s_ref, 
  //  //          bool include_front_arc,
  //  //          bool include_rear_arc,
  //  //          const dbsksp_shapelet_sptr& s,
  //  //          float* deform_cost,
  //  //          float* image_cost,
  //  //          float* fragment_size) const


  //  ////
  //  //str << "edge_id " << e->id() << "\n"
  //  //  << "init_image_cost " << this->init_image_cost_map_.find(e)->second << "\n"
  //  //  << "init_shape_cost " << this->init_shape_cost_map_.find(e)->second << "\n"
  //  //  << "final_image_cost " << this->final_image_cost_map_.find(e)->second << "\n"
  //  //  << "final_shape_cost " << this->final_shape_cost_map_.find(e)->second << "\n";
  //}



  // xmin
  str  << "bounding_box_x_min " << this->bbox_[0] << "\n";
  
  // ymin
  str  << "bounding_box_y_min " << this->bbox_[1] << "\n";

  // xmax
  str  << "bounding_box_x_max " << this->bbox_[2] << "\n";

  // ymax
  str  << "bounding_box_y_max " << this->bbox_[3] << "\n";

  str << "\n";
  return;
}




////: Compute histogram of a scalar value function defined on a grid
//void dbsks_dp_match::
//compute_histogram(const grid_float& grid_val, vnl_vector<float >& hist)
//{
//
//  //vcl_cout << "Number of legal configuration = " << legal_count << vcl_endl;
//
//  //// compute the histogram of this image cost grid
//  //// put everything into a 100 bins, range [0..9]
//  //vnl_vector<int > image_cost_hist(101, 0);
//  //for (int i_array=0; i_array < num_arrays; ++i_array)
//  //{
//  //  vnl_matrix<float >& plane = image_cost[i_array].as_ref();
//  //  for (int i_x=0; i_x < grid.num_x_; ++i_x)
//  //  {
//  //    for (int i_y=0; i_y < grid.num_y_; ++i_y)
//  //    {
//  //      float cost = vnl_math_min(plane(i_x, i_y), 10.0f);
//  //      if (cost > 9.0f) continue;
//
//  //      assert(cost >=0);
//  //      int bin = (int)vcl_floor(cost / 0.09);
//  //      ++image_cost_hist[bin];
//  //    }
//  //  }
//  //}
//
//  return;
//
//}




// ----------------------------------------------------------------------------
//: display real cost of the edges, given their states
void dbsks_dp_match::
display_real_cost(vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map, 
                  vcl_ostream& os)
{
  vcl_cout << "Compare real and stored cost for the graph\n";
  float total_stored_cost = 0;
  float total_real_cost = 0;

  // iterature thru all the edges
  for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::iterator it =
    graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;

    if (e->is_terminal_edge())
    {
      continue;
    }
    
    int i_xy_e = it->second.x();
    int i_plane_e = it->second.y();
    
    // retrieve the reference shapelet of this edge
    dbsksp_shapelet_sptr sref_e = this->ref_shapelet_map_.find(e)->second;

    // retrieve the shapelet corresponding to this state
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;
    dbsksp_shapelet_sptr shapelet_e = grid_e.shapelet(i_xy_e, i_plane_e);

    // real cost
    float image_cost_e, deform_cost_e, frag_size_e;
    float real_cost_e = this->matching_cost(sref_e, grid_e.has_front_arc_, 
      grid_e.has_rear_arc_, shapelet_e, &deform_cost_e, &image_cost_e, &frag_size_e);




    // DEBUG stored cost
    grid_float& matchcost_e = this->matchcost_map_.find(e)->second;
    this->load_matchcost_from_file(e, matchcost_e);

    int i_xA_e, i_yA_e;
    grid_e.linear_to_grid(i_xy_e, i_xA_e, i_yA_e);
    float stored_cost_e = matchcost_e[i_plane_e](i_xA_e, i_yA_e);

    matchcost_e.clear();
    

    vcl_cout << "  Fragment ID = " << e->id() << "\n"
      << "    Stored cost = " << stored_cost_e << "\n"
      << "    Real cost = " << real_cost_e << "\n"
      << "    Image cost = " << image_cost_e << "\n"
      << "    Deform cost = " << deform_cost_e << "\n"
      << "    Fragment size = " << frag_size_e << "\n";

    // update total costs
    total_real_cost += real_cost_e;
    total_stored_cost += stored_cost_e;  
  }

















  vcl_cout 
    << "  Total stored cost = " << total_stored_cost << "\n"
    << "  Total real cost = " << total_real_cost << "\n";

  // Save total real cost
  this->min_real_cost_ = total_real_cost;

  return;
}







// -----------------------------------------------------------------------------
//: Trace the boundary of a graph given the states of its edges
vcl_vector<vsol_spatial_object_2d_sptr > dbsks_dp_match::
trace_graph_boundary(  
  const vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map)
{
  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;

  // First construct the shapelets corresponding to the states of the graph
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > shapelet_map;
  this->convert_i_state_map_to_shapelet_map(graph_i_state_map, shapelet_map);

  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::iterator it = 
    shapelet_map.begin(); it != shapelet_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    dbsksp_shapelet_sptr s_e = it->second;
    dbsks_shapelet_grid& grid_e = this->shapelet_grid_map_.find(e)->second;

    // collect all the circular arcs we need to draw
    vcl_vector<dbgl_circ_arc > bnd_arc_list;
    vcl_vector<dbgl_conic_arc > shock_conic_list;

    // left and right
    dbsksp_shock_node_sptr v_child = e->child_node();
    dbsksp_shock_node_sptr v_parent = e->parent_node();

    // interpolate to get smooth boundary
    if (v_child->degree() == 2 && !grid_e.has_rear_arc_ )
    {
      // compute two shapelets from two extrinsic nodes
      vgl_point_2d<double > left_start_e = s_e->bnd_start(0);
      vgl_point_2d<double > right_start_e = s_e->bnd_start(1);
      vgl_point_2d<double > start_e = s_e->start();

      dbsksp_xshock_node_descriptor start_xnode(start_e, left_start_e, right_start_e);

      // the rear shapelet should come from the children
      dbsksp_shock_edge_sptr e1 = this->graph()->cyclic_adj_succ(e, v_child);
      dbsksp_shapelet_sptr s_e1 = shapelet_map.find(e1)->second;
      vgl_point_2d<double > left_start_e1 = s_e1->bnd_start(0);
      vgl_point_2d<double > right_start_e1 = s_e1->bnd_start(1);
      vgl_point_2d<double > start_e1 = s_e1->start();
      dbsksp_xshock_node_descriptor end_xnode(start_e1, left_start_e1, right_start_e1);

      dbsksp_twoshapelet_sptr ss_e = 0;
      dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
      ss_e = interpolator.optimize();
      dbsksp_shapelet_sptr s0_e = ss_e->shapelet_start();
      dbsksp_shapelet_sptr s1_e = ss_e->shapelet_end();

      // add the boundary of the two interpolating shapelets
      bnd_arc_list.push_back(s0_e->bnd_arc_left());
      bnd_arc_list.push_back(s0_e->bnd_arc_right());

      bnd_arc_list.push_back(s1_e->bnd_arc_left());
      bnd_arc_list.push_back(s1_e->bnd_arc_right());

      // add the shocks of the two shapelets
      shock_conic_list.push_back(s0_e->shock_geom());
      shock_conic_list.push_back(s1_e->shock_geom());
    }
    else
    {
      bnd_arc_list.push_back(s_e->bnd_arc_left());
      bnd_arc_list.push_back(s_e->bnd_arc_right());
      shock_conic_list.push_back(s_e->shock_geom());
    }

    // front
    if (grid_e.has_front_arc_)
    {
      dbsksp_shapelet_sptr s_front = s_e->terminal_shapelet_front();
      bnd_arc_list.push_back(s_front->bnd_arc_left());
      bnd_arc_list.push_back(s_front->bnd_arc_right());
    }

    // rear
    if (grid_e.has_rear_arc_)
    {
      dbsksp_shapelet_sptr s_rear = s_e->terminal_shapelet_rear();
      bnd_arc_list.push_back(s_rear->bnd_arc_left());
      bnd_arc_list.push_back(s_rear->bnd_arc_right());
    }


    // Now include all the circular arcs and conics as polyline
    for (unsigned i =0; i < bnd_arc_list.size(); ++i)
    {
      dbgl_circ_arc arc = bnd_arc_list[i];
      
      // an arc as a 11-vertex polyline
      vcl_vector<vsol_point_2d_sptr > pt_list;
      for (double t=0; t<1; t = t+0.1)
      {
        vgl_point_2d<double > pt = arc.point_at(t);
        pt_list.push_back(new vsol_point_2d(pt));
      }

      vsol_list.push_back(new vsol_polyline_2d(pt_list));
    }

    for (unsigned i =0; i < shock_conic_list.size(); ++i)
    {
      dbgl_conic_arc conic = shock_conic_list[i];
      
      // an conic as a 11-vertex polyline
      vcl_vector<vsol_point_2d_sptr > pt_list;
      for (double t=0; t<1; t = t+0.1)
      {
        vgl_point_2d<double > pt = conic.point_at(t);
        pt_list.push_back(new vsol_point_2d(pt));
      }
      vsol_list.push_back(new vsol_polyline_2d(pt_list));    
    }

    // add the contact shocks
    // front
    vcl_vector<vsol_point_2d_sptr > pts_front;
    pts_front.push_back(new vsol_point_2d(s_e->bnd_start(0)));
    pts_front.push_back(new vsol_point_2d(s_e->start()));
    pts_front.push_back(new vsol_point_2d(s_e->bnd_start(1)));
    vsol_list.push_back(new vsol_polyline_2d(pts_front));

    // rear
    // only add if this is the terminal fragment
    if (grid_e.has_rear_arc_)
    {
      vcl_vector<vsol_point_2d_sptr > pts_rear;
      pts_rear.push_back(new vsol_point_2d(s_e->bnd_end(0)));
      pts_rear.push_back(new vsol_point_2d(s_e->end()));
      pts_rear.push_back(new vsol_point_2d(s_e->bnd_end(1)));
      vsol_list.push_back(new vsol_polyline_2d(pts_rear));    
    }
  }

  return vsol_list;
}








