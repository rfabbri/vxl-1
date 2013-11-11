// This is rec/dborl/algo/vox_average_two_xgraphs/vox_average_two_xgraphs_one_to_many.h

#ifndef vox_average_two_xgraphs_one_to_many_h_
#define vox_average_two_xgraphs_one_to_many_h_

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


#include "vox_average_two_xgraphs_params_sptr.h"

#include <dborl/dborl_index_sptr.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

//==============================================================================
// vox_average_two_xgraphs_one_to_one
//==============================================================================
//:
class vox_average_two_xgraphs_one_to_one
{
public:

  // Default constructor
  vox_average_two_xgraphs_one_to_one(vox_average_two_xgraphs_params_sptr params);

  // Default destructor
  ~vox_average_two_xgraphs_one_to_one(){};

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

  //: Compute average of two xgraphs
  bool compute_average(const dbsksp_xgraph_storage_sptr& xgraph1,
    const dbsksp_xgraph_storage_sptr xgraph2,
    dbsksp_average_xgraph_storage_sptr& average_xgraph);

  //: save a screenshot of an to a file
  bool save_screenshot(const dbsksp_xshock_graph_sptr& xgraph, const vcl_string& out_png_filename);



  // Private attributes
  
  // Holds all dborl algo parameters
  vox_average_two_xgraphs_params_sptr params_;

  // to make member variables
  vcl_string esf_file_[2];

  // Storage for average shock graph
  dbsksp_average_xgraph_storage_sptr average_xgraph_storage_;

  // Output folder
  vcl_string output_folder_;

  // basename to save everything
  vcl_string base_name_;
  
};








#endif  //vox_average_two_xgraphs_one_to_many_h
