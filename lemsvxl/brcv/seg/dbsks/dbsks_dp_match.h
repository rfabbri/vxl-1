// This is seg/dbsks/dbsks_dp_match.h
#ifndef dbsks_dp_match_h_
#define dbsks_dp_match_h_

//:
// \file
// \brief A class for energy of a shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Oct 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/vnl_matrix.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
//#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>

#include <dbsks/dbsks_circ_arc_grid.h>
#include <dbsks/dbsks_shapelet_grid.h>
#include <dbsks/dbsks_ocm_image_cost_sptr.h>
#include <dbsks/dbsks_shock_graph_stats.h>




// ============================================================================
class dbsks_dp_match : public vbl_ref_count
{
public:
  // Constructor / destructor -------------------------------------------------
  //: constructor
  dbsks_dp_match();

  //: destructor
  virtual ~dbsks_dp_match(){};

  // User-interface functions -------------------------------------------------

  //: Construct an arc grid and and initialize intermediate variables to image cost
  // associated with each arc
  void init_arc_image_cost(const vil_image_view<float >& edgemap, 
    float edge_threshold,
    const dbsks_circ_arc_grid& arc_grid, 
    double sampling_ds, 
    double chamfer_sigma,
    double angle_sigma = 0.5236); // pi / 6

  //: Carry out the computation of image costs for all circular arcs
  // Assume all intermediate variables have been set up
  void compute_arc_image_cost();

  // New saving format. There is a version number at the beginning
  void save_circ_arc_costs(const vcl_string& filename);

  //: Load computed circular arc costs from a file with NEW format, which has
  // a version number at the beginning
  void load_circ_arc_costs(const vcl_string& filename);

  //: Clear arc_grid and arc_grid costs 
  void clear_arc_cost_data();

  //: Set parameters necessary to set up a "working graph"
  void set_graph_params(const dbsksp_shock_graph_sptr& ref_graph,
    unsigned image_width, unsigned image_height);

  //: Set parameters necessary to run DP given a "working graph" and arc costs
  void set_dp_params(const dbsks_shapelet_grid_params& grid_params,
    double sigma_deform,
    const vcl_string& temp_file_prefix,
    const vcl_string& temp_data_folder);

  //: Set statistics of a shock graph
  // REQUIREMENT: shock graph inside "stats" must have the same topology as the
  // shock graph of this DP
  void set_shock_graph_stats(const dbsks_shock_graph_stats& stats);
  
  //: Run the optimization algorithm
  void run_optim_for_one_model_height(float model_height);

  //: Run the optimization algorithm
  void run_optim_for_one_model_size(float model_size);

  //: Refine the centers of the shapelet grids and run optimization again
  void refine_grid_centers_and_run_opt();

  //: Save optimization results (the shapelet grids and the optimal states)
  void save_dp_optim_results(const vcl_string& filename);

  //: Save optimization results (the shapelet grids and the optimal states)
  bool load_dp_optim_results(const vcl_string& filename);

  //: Trace the boundary of the optimal shock graph
  void trace_opt_graph_bnd(vcl_vector<vsol_spatial_object_2d_sptr >& vsol_list);



  // Access memeber variables -------------------------------------------------

  //: Instance shock graph - the one we're computing energy of
  dbsksp_shock_graph_sptr graph() const {return this->graph_; };
  void set_graph(const dbsksp_shock_graph_sptr& graph)
  { this->graph_ = graph; }

  //: The reference shock graph
  dbsksp_shock_graph_sptr ref_graph() const
  { return this->ref_graph_; }

  void set_ref_graph(const dbsksp_shock_graph_sptr& ref_graph)
  { this->ref_graph_ = ref_graph; }

  //: Return and set the circular arc grid
  dbsks_circ_arc_grid arc_grid() const
  { return this->arc_grid_; }

  void set_arc_grid(const dbsks_circ_arc_grid& arc_grid)
  { this->arc_grid_ = arc_grid; }

  //: Return the oriented chamfer matcher for the image
  dbsks_ocm_image_cost_sptr oriented_chamfer() const
  { return this->oriented_chamfer_; }

  //: Return the fragment grid associated with an edge
  dbsks_shapelet_grid& shapelet_grid(unsigned int edge_id);

  // Optimization functions----------------------------------------------------

  typedef vbl_array_1d<vnl_matrix<float > > grid_float;
  typedef vbl_array_1d<vnl_matrix<int > > grid_int;

  //: Initialize the intermediate variables
  void init_dp_vars(float model_height);

  //: Initialize the intermediate variables, given model size and shock graph statistcs
  void init_dp_vars_using_stats(float model_size);

  //: Optimize energy of shock graph
  void optimize_edge_based();

  //: Trace out the globally optimum solution
  void trace_global_solution(const dbsksp_shock_edge_sptr& e_root);

  //: Trace out sub-optimal solution by dividing the states into groups spatially
  void trace_suboptimal_solutions(const dbsksp_shock_edge_sptr& e_root,
    int cellgrid_width, int cellgrid_height, int num_top_picks);

  

  
  //: Optimize a degree-2 fragment, i.e. given its state, what is the optimal
  // energy and states of its child fragment.
  void optimize_degree_2_node(const dbsksp_shock_node_sptr& v_p);

  //: Optimize a degree-3 fragment, i.e. given its state, what is the optimal
  // energy and states of its child fragment.
  void optimize_degree_3_node(const dbsksp_shock_node_sptr& v_p);


  // Given a grid of shapelets and its (sub-tree)accumulative cost, 
  // for each configuration of the starting xnode of the shapelet, compute 
  // the shapelet that gives the minimum energy.
  void compute_min_cost_wrt_sxnode(const dbsks_shapelet_grid& grid,
    const grid_float& sum_bc, 
    grid_float& opt_cost_sxnode, 
    grid_int& opt_gridplane_sxnode);


  
  // Energy functions ----------------------------------------------------------

  // Grid-based functions /////////////////////////////////////////////

  //: Compute matching cost (image cost + deform cost)
  // for all shapelets in the grid, wrt to a reference shapelet
  void compute_matching_cost(const dbsks_shapelet_grid& grid,
    const dbsksp_shapelet_sptr& ref_s0,
    vbl_array_1d<vnl_matrix<float > >& matching_cost);

  //: Compute image cost for a grid of terminal shapelet using precomputed costs
  // of a grid of ciruclar acs
  // The return costs are the NORMALIZED image cost (total cost / total_length)
  void compute_image_cost_using_arc_cost(const dbsks_shapelet_grid& grid,
    vbl_array_1d<vnl_matrix<float > >& image_cost, 
    const dbsks_circ_arc_grid& arc_grid,
    const vbl_array_1d<vnl_matrix<float > >& arc_image_cost);

  //: Add normalized deformation cost (wrt a reference fragment) for a grid of shapelet
  void add_deform_cost(const dbsks_shapelet_grid& grid,
    const dbsksp_shapelet_sptr& ref_s0,
    vbl_array_1d<vnl_matrix<float > >& deform_cost);
  

  // Fragment-based functions //////////////////////////////////////////////////////

  //: sum of deform cost + image cost
  float matching_cost(const dbsksp_shapelet_sptr& s_ref, 
    bool include_front_arc,
    bool include_rear_arc,
    const dbsksp_shapelet_sptr& s,
    float* deform_cost = 0,
    float* image_cost = 0,
    float* fragment_size = 0) const;
  
  // Cost functions of a graph -------------------------------------------------

  //: Compute the cost of one hypothesized graph
  float compute_graph_cost(const dbsksp_shock_graph_sptr& test_graph,
    bool approx_frags_w_shapelet_grid = false,
    bool approx_bnd_w_circ_arc_grid = false) const;

  //: Compute the closest approximation of the given graph on the under the
  // discretization of this DP
  // Results are saved in the form of a map from edge ID --> approximated shapelet
  bool compute_closest_approx(const dbsksp_shock_graph_sptr& test_graph,
    vcl_map<unsigned int, dbsksp_shapelet_sptr > approx_frags);

  bool compute_frag_cost(unsigned int edge_id, const dbsksp_shapelet_sptr& test_frag,
    float& total_cost,
    bool approx_frags_w_shapelet_grid = false,
    bool approx_bnd_w_circ_arc_grid = false,
    float* deform_cost = 0,
    float* image_cost = 0,
    float* frag_size = 0) const;


  //: Check if an edge fragment direction needs to be reversed to comply with the
  // depth order of the nodes
  bool need_reverse_frag_dir(unsigned edge_id) const;


  // Utility functions ----------------------------------------------------------

  //: Allocate memory for a scalar function defined on a shapelet grid
  // and set everything to a default value
  void allocate(grid_float& f, const dbsks_shapelet_grid& grid, float value);
  void allocate(grid_int& f, const dbsks_shapelet_grid& grid, int value);

  //: Allocate memory for a scalar function defined on the set of starting 
  // xnodes of a shapelet grid
  void allocate_sxnode(grid_float& f, const dbsks_shapelet_grid& grid, float value);
  void allocate_sxnode(grid_int& f, const dbsks_shapelet_grid& grid, int value);

  //: Release all memory inside the matrices of the grid, keep the matrices (for reuse)
  void release_matrix_memory(vcl_map<dbsksp_shock_edge_sptr, grid_float >& grid_map);
  void release_matrix_memory(vcl_map<dbsksp_shock_edge_sptr, grid_int >& grid_map);

  //: Save the optimal gridplane of an edge to a file
  void save_opt_gridplane_to_file(const dbsksp_shock_edge_sptr& e, 
    const grid_int& opt_gridplane);

  //: Load the optimal gridplane of an edge to a file
  bool load_opt_gridplane_from_file(const dbsksp_shock_edge_sptr& e,
    grid_int& opt_gridplane);

  //: Save the optimal sum_bc of an edge to a file
  void save_sum_bc_to_file(const dbsksp_shock_edge_sptr& e, 
    const grid_float& sum_bc);

  
  //: Load the optimal sum_bc of an edge to a file
  bool load_sum_bc_from_file(const dbsksp_shock_edge_sptr& e,
    grid_float& sum_bc );

  //: Save the match_cost of an edge to a file
  void save_matchcost_to_file(const dbsksp_shock_edge_sptr& e, 
    const grid_float& match_cost);

  //: Load the match_cost of an edge to a file
  bool load_matchcost_from_file(const dbsksp_shock_edge_sptr& e,
    grid_float& match_cost );




  //: Delete all temporary files
  void clear_all_temp_files();

  //: Delete everything that may take memory
  void clear();

  

  //: retrieve optimal states for all the nodes
  void trace_opt_state(const dbsksp_shock_edge_sptr& e_root, 
    vgl_point_2d<int > opt_i_state_root, 
    vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_opt_i_state_map);

  //: Convert from a map of state index of the edges to a map of shapelet of the edges
  void convert_i_state_map_to_shapelet_map(
    const vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& i_state_map,
    vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > & shapelet_map);

  //: Given a list of states for each edge, construct the graph
  void construct_graph(
    vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map,
    vcl_vector<dbsksp_shapelet_sptr >& fragment_list);

  //: Given a list of states for each edge, construct the graph
  void construct_graph(
    vcl_map<unsigned int, vgl_point_2d<int > >& graph_i_state_map,
    vcl_vector<dbsksp_shapelet_sptr >& fragment_list);

  //: Adjust the centers of the shapelet grids to the center of the starting points
  // of the shapelets in the provided maps
  void adjust_shapelet_grid_center(
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map);

  //: Adjust the number of points used for the shapelet grids by changing the
  // numbers of xA and yA
  // ``ratio_xA'' and ``ratio_yA'' are ratio between the numbers of points on each side
  // of the center for the new and old grid, which is slightly different from the ratios
  // between the literal num_xA and num_yA
  void change_shapelet_grids_num_xy(double ratio_xA, double ratio_yA);

  //: Build shapelet grid for each edge given a shock graph statistics and shapelet_grid_params
  // \TODO
  void build_shapelet_grids(const dbsks_shapelet_grid_params& params, 
    const dbsks_shock_graph_stats& stats,
    double graph_size,
    vcl_map<dbsksp_shock_edge_sptr, dbsks_shapelet_grid >& shapelet_grid_map);


  // MISCELLANEOUS functions -------------------------------------------------

  //: Print Optimization results to a stream
  void print_summary(vcl_ostream& str) const;

  // display real cost of the edges, given their states
  void display_real_cost(
    vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map, 
    vcl_ostream& os = vcl_cout);

  //: Trace the boundary of a graph given the states of its edges
  // - for visualization purpose
  vcl_vector<vsol_spatial_object_2d_sptr > trace_graph_boundary(
    const vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map);




  // grids of shapelets -------------------------------------------------------

  // parameters to determine structure of the shapelet grid
  dbsks_shapelet_grid_params shapelet_grid_params_;

  vcl_map<dbsksp_shock_edge_sptr, dbsks_shapelet_grid > shapelet_grid_map_;
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > ref_shapelet_map_;
  
  // the size of grid is number possible shapelets for 'key' edge
  vcl_map<dbsksp_shock_edge_sptr, grid_float > matchcost_map_;

  // the size of grid is number of possible shapelets for parent of 'key' edge
  vcl_map<dbsksp_shock_edge_sptr, grid_float > sum_bc_map_;

  // Prefix of filenames used to save temporary data
  vcl_string temp_file_prefix_;
  vcl_string temp_data_folder_;
  vcl_map<dbsksp_shock_edge_sptr, vcl_string > gridplane_filepath_map_;
  vcl_map<dbsksp_shock_edge_sptr, vcl_string > sum_bc_filepath_map_;
  vcl_map<dbsksp_shock_edge_sptr, vcl_string > matchcost_filepath_map_;


  // Computed optimized results --------------------------------------

  //: map each edge to the plane of its optimal state, given the state of the 
  // 'key' edge's parent (specified by the grid)
  vcl_map<dbsksp_shock_edge_sptr, grid_int > opt_e_gridplane_map_;

  // map each edge to the linear index of the (x,y) coordinate of its optimal 
  // state, given the state of the 'key' edge's parent (specified by the grid)
  vcl_map<dbsksp_shock_edge_sptr, grid_int > opt_e_gridxy_map_;

  
  
  // grids of circular arcs ---------------------------------------------------
  
  //: Grid of circular arcs defined
  dbsks_circ_arc_grid arc_grid_;

  //: Image costs of the circular arcs in the grid
  grid_float arc_image_cost_;


  // Global & final results ---------------------------------------------------
  
  //: list of indices of optimal states for each edge. To retrieve the extrinsic
  // states, need the shapelet_grid_map_
  vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > > graph_opt_i_state_;

  //: Computed optimal shapelets for each edge 
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > opt_shapelet_map_;

  //: list of shapelets to display
  vcl_vector<dbsksp_shapelet_sptr > shapelet_list;

  //: Cost of current DP
  float model_height_; // model height
  float model_size_; // size of model (square root of area)
  float min_dp_cost_;
  float min_real_cost_;
  vnl_vector<float > bbox_; // xmin, ymin, xmax, ymax


  // List of sub-optimal states for the edges
  vcl_vector<vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > > > list_graph_opt_i_state_;
  vcl_vector<vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > > list_opt_shapelet_map_;



  // Deformation cost parameters ----------------------------------------------

  //: Sigma for shape deformation cost
  double sigma_deform_;

  // Image and Image costs parameters ------------------------------------------

  // Oriented chamfer matching cost
  dbsks_ocm_image_cost_sptr oriented_chamfer_;
  
  //: Sampling rate when computing arc costs
  double ds_shapelet_;

  //////////////////////////////////////////////////////////////////////////////


  // Parameters to determine the size of the model ----------------------------
  // Image size
  unsigned int image_ni_;
  unsigned int image_nj_;

protected:
  //: Input graph
  dbsksp_shock_graph_sptr ref_graph_;

  // Working graph
  dbsksp_shock_graph_sptr graph_;

  // Statistics of shock graph
  dbsks_shock_graph_stats shock_graph_stats_;
};


#endif // seg/dbsks/dbsks_dp_match.h


