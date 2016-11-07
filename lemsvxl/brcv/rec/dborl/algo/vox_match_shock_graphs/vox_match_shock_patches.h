//:
// \file
// \brief This class performs matching of model shock patches
//        to query shock patches
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/11/09
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef vox_match_shock_patches_h_
#define vox_match_shock_patches_h_

#include "vox_match_shock_graphs_params_sptr.h"
#include <vcl_string.h>

class vox_match_shock_patches
{

public:

    // Default ctor
    vox_match_shock_patches(vox_match_shock_graphs_params_sptr 
                                       params);
  
    // Default dtor
    ~vox_match_shock_patches();

    // Called by main statement to intiate algorithm on vox
    bool process();

private:
    
    // Private methods

    // Initialize parameter settings
    bool initialize();

    // Actual matching performed
    bool perform_matching();
    
    // Private attributes
    
    // Holds all dborl algo parameters
    vox_match_shock_graphs_params_sptr params_;
   
    // The file path of the model shock patch storage
    vcl_string model_file_;

    // The file path of the query shock patch storage
    vcl_string query_file_;

    // The file path of the where the matching results should go
    vcl_string output_match_folder_;

    // The output string attached to the results of matching
    vcl_string output_match_prefix_;

     // Good practice make copy ctor and assign operator private

    // Copy ctor
    vox_match_shock_patches
        (const vox_match_shock_patches&);

    // Assignment operator
    vox_match_shock_patches& 
        operator=(const vox_match_shock_patches);
};

#endif  //vox_match_shock_patches_h
