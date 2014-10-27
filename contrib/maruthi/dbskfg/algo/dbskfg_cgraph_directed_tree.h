// This is brcv/shp/dbskfg/algo/dbskfg_cgraph_directed_tree.h

#ifndef dbskfg_cgraph_directed_tree_h_
#define dbskfg_cgraph_directed_tree_h_

//:
// \file 
// \brief A directed tree constructed from a dbskfg_composite_graph to use in 
//        tree-edit-distance algorithm 
// \author Maruthi Narayanan (mn@lems.brown.edu)
//
// \date Sep 19 2010
//
// \verbatim
// Modifications
//  Maruthi Narayanan      Sep 19, 2010    Initial version
//
// \endverbatim

#include <dbskr/dbskr_directed_tree.h>

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>

#include <dbskfg/algo/dbskfg_sift_data.h>

#include <vsol/vsol_box_2d.h>

#include <vl/sift.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_algorithm.h>
#include <vil/vil_image_view.h>

//==============================================================================
// dbskfg_cgraph_directed_tree
//==============================================================================

//: A directed tree built from a dbsk2d_shock_graph
class dbskfg_cgraph_directed_tree : public dbskr_directed_tree
{
public:
  // Constructors / Destructors / Initializers----------------------------------
  
  //: Constructor
  dbskfg_cgraph_directed_tree(float scurve_sample_ds=5.0f, 
                              float interpolate_ds=1.0f, 
                              float matching_R=6.0f,
                              bool mirror=false,
                              double area_weight=0.0f,
                              vl_sift_pix* grad_data=0,
                              VlSiftFilt* sift_filter=0,
                              vl_sift_pix* red_grad_data=0,
                              vl_sift_pix* green_grad_data=0,
                              vl_sift_pix* blue_grad_data=0,
                              unsigned int id=0,
                              vil_image_view<double>* L_channel=0,
                              vil_image_view<double>* a_channel_=0,
                              vil_image_view<double>* b_channel_=0);

  //: Destructor;
  /* virtual */ ~dbskfg_cgraph_directed_tree();

  //: acquire tree from the given rag node
  bool acquire(const dbskfg_composite_graph_sptr& composite_graph,
               bool elastic_splice_cost, bool construct_circular_ends, 
               bool dpmatch_combined);
 
  //: acquire tree from the given rag node
  bool acquire(const dbskfg_composite_graph_sptr& composite_graph,
               bool elastic_splice_cost, bool construct_circular_ends, 
               bool dpmatch_combined,float cost);
  
  //: get tag
  unsigned int get_id(){return id_;}

  // Graph-related--------------------------------------------------------------

  // used in get_scurve(...)
  //: return a vector of pointers to the edges in underlying composite graph 
  //  for the given dart list
  void get_edge_list(const vcl_vector<int>& dart_list,  
                     dbskfg_composite_node_sptr& start_node,  
                     vcl_vector<dbskfg_composite_link_sptr>& path);

  //: acquire tree topology from a shock graph
  bool acquire_tree_topology(const dbskfg_composite_graph_sptr& 
                             composite_graph);

  //: Compute delete and contract costs for individual dart. 
  // Only call this function after acquire_tree_topology 
  // has been called and succeeded.
  void compute_delete_and_contract_costs(bool elastic_splice_cost,
                                         bool construct_circular_ends,
                                         bool dpmatch_combined);

  //: sets all delete and contract costs to a specified value
  void set_delete_and_contract_costs(bool elastic_splice_cost,
                                     bool construct_circular_ends,
                                     bool dpmatch_combined,
                                     float cost);

  //: find and cache the shock curve for this pair of darts, 
  // if not already cached
  /* virtual */ dbskr_scurve_sptr get_curve(int start_dart, int end_dart,
                                            bool construst_circular_ends);
  
  //: find and cache the sift along shock curve for this pair of darts, 
  // if not already cached
  vcl_vector<vnl_vector_fixed<vl_sift_pix,384> >& 
      get_sift_along_curve(int start_dart, int end_dart);

  //: returns both the coarse and dense version of shock curve
  /* virtual */ dbskr_sc_pair_sptr get_curve_pair(int start_dart, int end_dart,
                                                  bool construct_circular_ends);


  //: Clear the cache of scurve's for given pairs of darts
  void clear_dart_path_scurve_map()
  {
    this->dart_path_scurve_map_.clear();
  }

  //: get root node radius
  double get_root_node_radius(){return root_node_radius_;}

  //: set scale ratio for this tree
  void set_scale_ratio(double scale_ratio){scale_ratio_=scale_ratio;}

  //: get composite graph 
  dbskfg_composite_graph_sptr& get_cgraph(){return composite_graph_;}

  // File I/O-------------------------------------------------------------------

  //: create and write .shg file to debug splice and contract costs
  bool create_shg(vcl_string fname);

  //: comptue region descriptor
  void compute_region_descriptor(
      vcl_map<int,vcl_vector<dbskfg_sift_data> >& fragment,
      vsol_box_2d_sptr& bbox);


  //: Compute total length of the shape boundary
  // If "construct_circular_ends" is true, each branch is closed 
  // with a circular arc (instead of being left open). This depends on node
  // being virtual 
  double compute_total_reconstructed_boundary_length();

  //------------------------------------------------------------------------
  //: uses the already existing scurves, so if circular_ends = true 
  //while acquiring the tree then the outline will have circular completions
  void compute_reconstructed_boundary_polygon(vgl_polygon<double>& poly);

  // compute appeance
  bool compute_appearance()
  {
      return false;
      /* if ( grad_data_==0 || sift_filter_==0 ) */
      /* { */
      /*     return false; */
      /* } */
      /* else */
      /* { */
      /*     return true; */
      /* } */
  }

  // get bounding box
  vsol_box_2d_sptr bbox(){return bbox_;}

  // get grad data
  vl_sift_pix* get_grad_data(){return grad_data_;}
  
  // get sift filter
  VlSiftFilt* get_sift_filter(){return sift_filter_;}

  // get grad data
  vl_sift_pix* get_red_grad_data(){return red_grad_data_;}

  // get grad data
  vl_sift_pix* get_green_grad_data(){return green_grad_data_;}

  // get grad data
  vl_sift_pix* get_blue_grad_data(){return blue_grad_data_;}

  // get grad data
  vil_image_view<double>* get_channel1(){return channel1_;}

  // get grad data
  vil_image_view<double>* get_channel2(){return channel2_;}

  // get grad data
  vil_image_view<double>* get_channel3(){return channel3_;}

  // See if dart has a virtual node on it
  bool virtual_node_dart(int dart)
  {
      
      vcl_vector<int>::iterator find_it = vcl_find(
          darts_virtual_nodes_.begin(),
          darts_virtual_nodes_.end(),
          dart);
      
      bool circ_flag = (find_it == darts_virtual_nodes_.end())
          ? false : true;

      return circ_flag;

  }

  double get_scale_ratio(){return scale_ratio_;}

  void compute_average_ds();

  vcl_vector<double>& get_average_ds(){return average_ds_;}

protected:
  // Reset / Initalize /////////////////////////////////////////////////////////

  //: Reset all member variables to default
  void clear(); 

  
protected:
  
  // Member variables-----------------------------------------------------------
  
  // Composite-graph related //////////////////////////////////////

  // The shock graph this tree is constructed from
  dbskfg_composite_graph_sptr composite_graph_;

  //: each dart has a list of pointers to the actual edges 
  //  on the corresponding shock branch of the graph (in the correct order)
  vcl_vector<vcl_vector<dbskfg_composite_link_sptr> > shock_edges_;

  //: each dart also has a list of start nodes for the underlying edge to 
  // determine direction 
  vcl_vector<dbskfg_composite_node_sptr> starting_nodes_;

  //: Keep a vector of mapping of dart ids which have virtual nodes
  vcl_vector<int> darts_virtual_nodes_;

  //: Keep track of largest radius of root node
  double root_node_radius_;

  // Cache data////////////////////////////////////////////////////////////

  //: cache the shock curves for future use for each path of darts
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr> dart_path_scurve_map_;

  //: cache the sift points computed
  vcl_map<vcl_pair<int,int>, vcl_vector<
      vnl_vector_fixed<vl_sift_pix,384> > > dart_path_sift_map_;

  // Cost-related parameters /////////////////////////////////////////////
  // For now only, may change later
  
  // in get_curve we call the curve computation like this:
  float scurve_sample_ds_;
  float interpolate_ds_;  // made this into a parameter as well, default is 0.5
  float scurve_matching_R_;
  double scale_ratio_;

  vsol_box_2d_sptr bbox_;

  static bool compare_node_radius_pairs(
      const vcl_pair<double,unsigned int>& pair1,
      const vcl_pair<double,unsigned int>& pair2)
  {return pair1.first > pair2.first; }
  
  void compute_bounding_box();

  void compute_outer_shock(dbskr_scurve_sptr sc);

  bool mirror_;

  double area_weight_;

  vl_sift_pix* grad_data_;
  
  VlSiftFilt* sift_filter_;

  vl_sift_pix* red_grad_data_;

  vl_sift_pix* green_grad_data_;

  vl_sift_pix* blue_grad_data_;
  
  vcl_vector<double> average_ds_;

  vil_image_view<double>* channel1_;

  vil_image_view<double>* channel2_;

  vil_image_view<double>* channel3_;

  unsigned int id_;
};

#endif // dbskfg_cgraph_directed_tree_h_
