//:
// \file
// \brief This class performs matching of model composite graph fragments
//        to a query set of composite graph fragments
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 10/26/2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef vox_match_bag_of_fragments_h_
#define vox_match_bag_of_fragments_h_

#include "vox_match_composite_fragments_params_sptr.h"
#include <vcl_string.h>

class vox_match_bag_of_fragments
{

public:

    // Default ctor
    vox_match_bag_of_fragments(vox_match_composite_fragments_params_sptr 
                               params);
  
    // Default dtor
    ~vox_match_bag_of_fragments();

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
    vox_match_composite_fragments_params_sptr params_;
   
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
    vox_match_bag_of_fragments
        (const vox_match_bag_of_fragments&);

    // Assignment operator
    vox_match_bag_of_fragments& 
        operator=(const vox_match_bag_of_fragments);
};

#endif  //vox_match_bag_of_fragments_h
