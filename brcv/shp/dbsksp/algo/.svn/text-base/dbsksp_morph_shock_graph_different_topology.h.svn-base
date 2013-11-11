// This is shp/dbsksp/dbsksp_morph_shock_graph_different_topology.h
#ifndef dbsksp_morph_shock_graph_different_topology_h_
#define dbsksp_morph_shock_graph_different_topology_h_

//:
// \file
// \brief A class to morph between two shock graphs with different topology
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 9, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <vnl/vnl_vector.h>


#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>


//: A data structure to hold data for each morphing time segment

class dbsksp_morph_segment
{
public:
  //: Constructor
  dbsksp_morph_segment(){}
  //: Destructor
  ~dbsksp_morph_segment(){}

  // member variables
  dbsksp_shock_graph_sptr start_graph;
  dbsksp_shock_graph_sptr end_graph;
  double start_time;
  double end_time;

  dbsksp_shock_graph_sptr ref_graph;
};




// ============================================================================
// dbsksp_morph_shock_graph_different_topology
// ============================================================================

//: A class to morph between two shock graphs with different topology
class dbsksp_morph_shock_graph_different_topology
{
public:
   double len_epsilon;

   double scurve_sample_ds_; // sampling rate of the scurve
   double shock_interp_sample_ds_; // sampling rate to interpolate the scurves

  // ------------------------------------------------------------------------
  // CONSTRUCTORS / DESTRUCTORS
  // ------------------------------------------------------------------------

  //: Constructor
  dbsksp_morph_shock_graph_different_topology(): 
    len_epsilon(0.01),
    scurve_sample_ds_(1.0),
    shock_interp_sample_ds_(12.0){};
  
  //: Destructor
  virtual ~dbsksp_morph_shock_graph_different_topology(){};


  // ------------------------------------------------------------------------
  // DATA ACCESS
  // ------------------------------------------------------------------------


  //: Get and set the shock matching data structure
  dbskr_sm_cor_sptr sm_cor() const {return this->sm_cor_; }
  void set_sm_cor(const dbskr_sm_cor_sptr& sm_cor) 
  { this->sm_cor_ = sm_cor; }

  //: Get the transition graphs
  vcl_vector<dbsksp_shock_graph_sptr > transitions_tree1() const
  {return this->transitions_tree1_; }

  vcl_vector<dbsksp_shock_graph_sptr > transitions_tree2() const
  {return this->transitions_tree2_; }
  
  

  // ------------------------------------------------------------------------
  // UTILITIES
  // ------------------------------------------------------------------------

  //: Morphing the two shapes
  // Building the data structure allowing for morphing
  bool morph();


  //: Determine which darts will survive and which darts will be removed
  void compute_surviving_darts();

  //: Generate intermediate trees, given a tree and a survial list
  // intermediate_graphs is a nx2 array
  // n is the number of times the graph changes topology
  void generate_intermediate_graphs(const dbskr_tree_sptr& tree,
    int ref_dart,
    const vcl_vector<bool > used_darts,
    const vcl_vector<int >& num_segments_to_interpolate,
    vcl_vector<dbsksp_shock_graph_sptr >& transition_graphs);


  //: Generate intermediate trees, given a tree and a survial list
  // intermediate_graphs is a nx2 array
  // n is the number of times the graph changes topology
  void generate_morph_sequence(const dbskr_tree_sptr& tree,
    int ref_dart,
    const vcl_vector<bool > used_darts,
    const vcl_vector<int >& num_segments_to_interpolate,
    vcl_vector<dbsksp_morph_segment >& morph_sequence);


  //: Applying editing sequence to the initial graph
  void generate_edited_graph(const dbskr_tree_sptr& tree,
    int ref_dart,
    const vcl_vector<bool > used_darts,
    const vcl_vector<int >& num_segments_to_interpolate,
    bool real_edge_deletion,
    dbsksp_shock_graph_sptr& start_graph,
    dbsksp_shock_graph_sptr& final_graph,
    vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& final_to_start_edge_map);

  //: compute final common graph
  void generate_final_common_graph();

  //: Propagate data from source to target, given an edge map from source to target
  void propagate_data(const dbsksp_shock_graph_sptr& start,
    const dbsksp_shock_graph_sptr& end,
    const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& start_to_end
    );



  // --------------------------------------------




  //: Generate the numbers of segments used to interpolate each dart of each tree
  // so that the common shock graphs between the two trees have EXACT same topology
  // This will allow us to do averaging between two shapes
  // sampling_ds is the approximate sampling rate
  void compute_num_segments_to_interpolate_trees(double sampling_ds);
  

  //: Compute the lengths of the darts for a tree
  void compute_dart_lengths(const dbskr_tree_sptr& tree,
    const vcl_vector<int >& query_dart_list,
    vcl_vector<double >& dart_lengths);


  //: Initial graph for each tree
  dbsksp_shock_graph_sptr tree1_init_graph_;
  dbsksp_shock_graph_sptr tree2_init_graph_;



  //: Mean graph for each tree
  dbsksp_shock_graph_sptr tree1_mean_graph_;
  dbsksp_shock_graph_sptr tree2_mean_graph_;

  dbsksp_shock_graph_sptr common_topology_graph_;


protected:
  dbskr_sm_cor_sptr sm_cor_;

  // tree1 survival list, 
  // used_darts_tree1_[i] = true: dart(i) will survive in the editing path
  // false: the dart will be removed in the editing path
  vcl_vector<bool > used_darts_tree1_;

  // tree2 survival list, similar to tree 1 above
  vcl_vector<bool > used_darts_tree2_;

  //: number of segments used to interpolate tree 1
  // num_segments_tree1_[i]: number of segments used to interpolate dart i
  // Each segment will result in two shock edges (due to interpolation method)
  vcl_vector<int > num_segments_tree1_;

  //: number of segments used to interpolate tree 1
  vcl_vector<int > num_segments_tree2_;


  

  // transition graphs for tree1
  vcl_vector<dbsksp_shock_graph_sptr > transitions_tree1_;
  vcl_vector<dbsksp_shock_graph_sptr > transitions_tree2_;
  
};


#endif // shp/dbsksp/dbsksp_morph_shock_graph_different_topology.h









