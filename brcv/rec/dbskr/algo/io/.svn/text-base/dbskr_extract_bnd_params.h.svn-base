//:
// \file
// \brief The extract bnd parameter class 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/08/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_extract_bnd_params_h)
#define _dbskr_extract_bnd_params_h

#include <bxml/bxml_document.h>

class dbskr_extract_bnd_params
{ public:

    bool extract_from_edge_img_;
    int  edge_img_threshold_;

    bool run_contour_tracing_;  //--> binarizes the image first

    double nrad_;
    double dx_;
    double dt_;
    double max_k_;
    int min_size_to_keep_;

    bool generic_linker_;
    bool require_appearance_consistency_;  // if generic linker
    double appearance_consistency_threshold_; // if generic linker, this threshold was 2.0f on the order of edge strength

    unsigned max_size_to_group_;  // if Amir's linker
    unsigned min_size_to_link_;   // if Amir's linker

    double edge_detection_sigma_;
    double edge_detection_thresh_;

    double avg_grad_mag_threshold_;
    double length_thresh_;
    double pruning_color_threshold_;
    int pruning_region_width_;

    bool smooth_bnds_;
    int smoothing_nsteps_;
    bool fit_lines_;
    double rms_;              // for line fitting

    vcl_string output_file_postfix_;

    bool parse_from_data(bxml_data_sptr root);
    static bxml_element *create_default_document_data();
    bxml_element *create_document_data();
};

#endif  //_dbskr_extract_bnd_params_h
