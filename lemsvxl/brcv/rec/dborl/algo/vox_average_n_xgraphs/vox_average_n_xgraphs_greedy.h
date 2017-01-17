// This is rec/dborl/algo/vox_average_n_xgraphs/vox_average_n_xgraphs_one_to_many.h

#ifndef vox_average_n_xgraphs_one_to_many_h_
#define vox_average_n_xgraphs_one_to_many_h_

//:
// \file
// \brief This class performs one to one shape averaging
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 2, 2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include "vox_average_n_xgraphs_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

//==============================================================================
// vox_average_n_xgraphs_greedy
//==============================================================================
//:
class vox_average_n_xgraphs_greedy
{
public:

  // Default constructor
  vox_average_n_xgraphs_greedy(vox_average_n_xgraphs_params_sptr params);

  // Default destructor
  ~vox_average_n_xgraphs_greedy(){};

  // Process the shock graph pair
  bool process();

protected:

  //: Initialize parameter settings
  bool initialize();

  //: Actual matching performed
  bool perform_averaging();
  
  //: Write out results
  bool write_out();

  //: Load esf file
  bool load_esf(const vcl_string& esf_file, dbsk2d_shock_storage_sptr& shock_storage);

  //: Convert sk2d graph to sksp graph
  bool convert_sk2d_to_sksp(const dbsk2d_shock_storage_sptr& sk2d_storage,
    dbsksp_xgraph_storage_sptr& sksp_storage);

  //: Compute average of N xgraphs
  bool compute_average();

  //: save a screenshot of an to a file
  bool save_screenshot(const dbsksp_xshock_graph_sptr& xgraph, const vcl_string& out_png_filename);

  //: Compute edit cost and deformation cost from all parents to an xgraph
  void compute_edit_distance(const dbsksp_xshock_graph_sptr& query,
    vcl_vector<double >& distance_parent_to_query,
    vcl_vector<double >& deform_cost_parent_to_query);

  



  // Member variables-----------------------------------------------------------

  //1) Input--------------------------------------------------------------------
  
  //: Holds all dborl algo parameters
  vox_average_n_xgraphs_params_sptr params_;

  // Output folder
  vcl_string output_folder_;
  
  //2) Output--------------------------------------------------------------------
  
  //: Storage for average shock graph
  dbsksp_average_xgraph_storage_sptr average_xgraph_storage_;

  

  //3) Intermediate variables---------------------------------------------------

  //: List of object names
  vcl_vector<vcl_string > list_object_name_;

  //: List of esf files of the shock graphs
  vcl_vector<vcl_string > list_esf_file_;

  //: List of xgraphs to compute average on
  vcl_vector<dbsksp_xshock_graph_sptr > list_xgraph_;

  //: list distance from parent to average xgraph
  vcl_vector<double > list_distance_parent_to_average_;

  //: list of deform cost when matching parents with the average shape
  vcl_vector<double > list_deform_cost_parent_to_average_;

  // basename (includes folder) to save everything
  vcl_string base_name_;

  // optimization method name
  vcl_string method_name_;
};








#endif  //vox_average_n_xgraphs_one_to_many_h
