//:
// \file
// \brief A command line executable to load two patch sets and their match 
//        to visualize patch match table 
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/12/09
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef vox_visualize_patch_matches_h_
#define vox_visualize_patch_matches_h_

#include "vox_visualize_patch_matches_params_sptr.h"
#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>
#include <vcl_string.h>

class vox_visualize_patch_matches
{

public:

    // Default ctor
    vox_visualize_patch_matches
        (vox_visualize_patch_matches_params_sptr params);
  
    // Default dtor
    ~vox_visualize_patch_matches();

    // Called by main statement to intiate algorithm on vox
    bool process();

private:
    
    // Private methods

    // Initialize parameter settings
    bool initialize();

    // Actually creates the html visualization
    bool visualize();

    // Private attributes
    
    // Holds all dborl algo parameters
    vox_visualize_patch_matches_params_sptr params_;

    // Holds the match data structure
    dbskr_shock_patch_match_sptr match_;

    // Holds the output html file
    vcl_string output_html_;

    // Good practice make copy ctor and assign operator private

    // Copy ctor
    vox_visualize_patch_matches
        (const vox_visualize_patch_matches&);

    // Assignment operator
    vox_visualize_patch_matches& 
        operator=(const vox_visualize_patch_matches);
};

#endif  //vox_visualize_patch_matches_h
