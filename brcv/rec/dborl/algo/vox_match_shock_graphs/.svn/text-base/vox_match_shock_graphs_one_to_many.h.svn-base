//:
// \file
// \brief This class performs one to many matching, this would 
//        be used for matching kimia 99 for example
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 06/28/09
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef vox_match_shock_graphs_one_to_many_h_
#define vox_match_shock_graphs_one_to_many_h_

#include "vox_match_shock_graphs_params_sptr.h"
#include <dborl/dborl_index_sptr.h>
#include <vcl_string.h>
#include <vcl_vector.h>

class vox_match_shock_graphs_one_to_many
{

public:

    // Default ctor
    vox_match_shock_graphs_one_to_many(vox_match_shock_graphs_params_sptr 
                                       params);
  
    // Default dtor
    ~vox_match_shock_graphs_one_to_many();

    // Called by main statement to intiate algorithm on vox
    bool process();

private:
    
    // Private methods

    // Initialize parameter settings
    bool initialize();

    // Actual matching performed
    bool perform_matching();
    
    // Write out results
    // The results will either be normalized or unormalized
    bool write_out();

    // Private attributes
    
    // Holds all dborl algo parameters
    vox_match_shock_graphs_params_sptr params_;

    // Holds unormalized costs or normalized costs depending on the type
    // of matching
    vcl_vector<float> final_costs_;
    
    // Data structure that holds everthing in index file
    dborl_index_sptr ind_;

    // The output file path
    vcl_string output_file_;

    // The query shock file that constitutes the one in the one to many
    // matching
    vcl_string query_shock_;

    // Good practice make copy ctor and assign operator private

    // Copy ctor
    vox_match_shock_graphs_one_to_many
        (const vox_match_shock_graphs_one_to_many&);

    // Assignment operator
    vox_match_shock_graphs_one_to_many& 
        operator=(const vox_match_shock_graphs_one_to_many);
};

#endif  //vox_match_shock_graphs_one_to_many_h
