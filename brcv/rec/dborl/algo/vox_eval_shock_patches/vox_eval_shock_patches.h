//:
// \file
// \brief This class performs evaluation of shock patch detection results
//        outputs an xml file with the detection results
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/15/09
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#ifndef vox_eval_shock_patches_h_
#define vox_eval_shock_patches_h_

#include "vox_eval_shock_patches_params_sptr.h"
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_image_description_sptr.h>
#include <dborl/dborl_evaluation.h>
#include <bxml/bxml_document.h>
#include <vcl_string.h>
#include <vcl_vector.h>

class vox_eval_shock_patches
{

public:

    // Default ctor
    vox_eval_shock_patches(vox_eval_shock_patches_params_sptr 
                                       params);
  
    // Default dtor
    ~vox_eval_shock_patches();

    // Called by main statement to intiate algorithm on vox
    bool process();

private:
    
    // Private methods

    // Initialize parameter settings
    bool initialize();

    // Actual evaluation of results performed
    bool perform_eval();
    
    // Write out results
    // The results will be an xml format
    bool write_out();

    // Returns the bounding box for the threshold bxml element
    void bounding_box(bxml_element* threshold,
                      vsol_box_2d_sptr box);

    // Private attributes
    
    // Holds all dborl algo parameters
    vox_eval_shock_patches_params_sptr params_;

    // Holds the model id image description
    dborl_image_description_sptr model_id_;

    // Data structure that holds everthing in index file
    dborl_index_sptr ind_;

    // The output file path
    vcl_string output_file_;

    // The vector of all threshold bxml elements
    vcl_vector<bxml_data_sptr> thresholds_;

    // Vector of all stats at each threshold
    vcl_vector<dborl_exp_stat> stats_at_threshold_;

    // Vector all query image description objects
    vcl_vector< dborl_image_description_sptr> query_ids_;

    // Good practice make copy ctor and assign operator private

    // Copy ctor
    vox_eval_shock_patches
        (const vox_eval_shock_patches&);

    // Assignment operator
    vox_eval_shock_patches& 
        operator=(const vox_eval_shock_patches);
};

#endif  //vox_eval_shock_patches_h
