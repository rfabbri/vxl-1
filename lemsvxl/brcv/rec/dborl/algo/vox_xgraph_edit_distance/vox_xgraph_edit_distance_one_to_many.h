// This is rec/dborl/algo/vox_xgraph_edit_distance/vox_xgraph_edit_distance_one_to_many.h

#ifndef vox_xgraph_edit_distance_one_to_many_h_
#define vox_xgraph_edit_distance_one_to_many_h_

//:
// \file
// \brief This class compute edit distance between one query xgraph and all xgraphs in a dataset
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 18, 2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#include "vox_xgraph_edit_distance_params_sptr.h"




#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>

#include <dborl/dborl_index_sptr.h>
#include <string>
#include <vector>



//==============================================================================
// vox_xgraph_edit_distance_one_to_many
//==============================================================================
//:
class vox_xgraph_edit_distance_one_to_many
{
public:

  // Default constructor
  vox_xgraph_edit_distance_one_to_many(vox_xgraph_edit_distance_params_sptr params);

  // Default destructor
  ~vox_xgraph_edit_distance_one_to_many(){};

  // Process the shock graph pair
  bool process();

protected:

  //: Initialize parameter settings
  bool initialize();

  //: Actual matching performed
  bool perform_distance_computation();
  
  //: Write out results
  bool write_out();

  //: Load esf file
  bool load_esf(const std::string& esf_file, dbsksp_xshock_graph_sptr& xgraph, 
    bool use_existing_xgraph, const std::string& xgraph_extension);

  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
    const dbsksp_xshock_graph_sptr& xgraph2,
    dbsksp_edit_distance& work);

  //: Normalize xgraph
  dbsksp_xshock_graph_sptr normalize_xgraph(const dbsksp_xshock_graph_sptr& xgraph);

  //: Mirror an xgraph around y-axis
  dbsksp_xshock_graph_sptr mirror_xgraph(const dbsksp_xshock_graph_sptr& xgraph);



  // Member variables-----------------------------------------------------------

  //1) Input--------------------------------------------------------------------
  
  //: Holds all dborl algo parameters
  vox_xgraph_edit_distance_params_sptr params_;

  // Output folder
  std::string output_folder_;
  
  //2) Output--------------------------------------------------------------------

  //3) Intermediate variables---------------------------------------------------

  //: List of object names
  std::vector<std::string > model_object_names_;

  //: List of esf files of the shock graphs
  std::vector<std::string > model_esf_files_;

  // Query object name
  std::string query_object_name_;

  // Query esf file
  std::string query_esf_file_;

  //: List of xgraphs to compute average on
  std::vector<double > distance_query_to_models_;

  // basename (includes folder) to save everything
  std::string output_prefix_;
};








#endif  //vox_xgraph_edit_distance_one_to_many_h
