// This is seg/algo/dbsks/dbsks_train_geom_model.h
#ifndef dbsks_train_geom_model_h_
#define dbsks_train_geom_model_h_

//:
// \file
// \brief A class to collect data and train an xgraph geometric model
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date August 5, 2009
//
// \verbatim
//  Modifications
// \endverbatim



#include <dbsksp/dbsksp_xshock_fragment_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsks/dbsks_xnode_geom_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_vector.h>



// ============================================================================
//: List of parameters needed to train a geometric model
struct dbsks_train_geom_model_params
{
  vcl_string xshock_folder;
  vcl_string xshock_list_file;
  int root_vid;
  int pseudo_parent_eid;
  double normalized_xgraph_size;
  bool b_normalize;
  vcl_string output_file;

};


// ============================================================================
//: A class to collect and construct a model for xgraph geometric attributes
class dbsks_train_geom_model
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_train_geom_model(){};

  //: destructor
  virtual ~dbsks_train_geom_model(){};
  
  // Access member-variables ---------------------------------------------------

  
  // Utility functions ---------------------------------------------------------

  //: Set training info
  void set_training_info(const dbsks_train_geom_model_params& params)
  { this->params_ = params; }
  
  //: Collect positive training data
  bool collect_data();
  
  //: Construct model from collected data
  bool construct_geom_model();

  //: Save geom model to file
  bool save_geom_model_to_file();

  //: Collect list of xgraph sizes in the exemplar xgraphs
  bool save_list_of_xgraph_sizes();


  // Member variables ----------------------------------------------------------
public:
  dbsks_train_geom_model_params params_;
  
  // place holder for attributes of all edges and nodes, after normalization
  vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > > map_edge2frags;
  vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > > map_node2geom;
  dbsksp_xshock_graph_sptr prototype_xgraph;
  dbsks_xgraph_geom_model_sptr xgraph_geom;

protected:

};


#endif // seg/dbsks/algo/dbsks_train_geom_model.h


