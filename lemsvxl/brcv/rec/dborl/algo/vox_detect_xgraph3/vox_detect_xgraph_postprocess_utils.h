// This is dborl/algo/vox_detect_xgraph/vox_detect_xgraph_postprocess_utils.h

#ifndef vox_detect_xgraph_postprocess_utils_h_
#define vox_detect_xgraph_postprocess_utils_h_

//:
// \file
// \brief Post-processing utilities for vox_detect_xgraph
// 
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 8, 2009
//
// \verbatim
//    Modifications:
// \endverbatim



#include <string>
#include <map>

// =============================================================================
// Main functions 
// =============================================================================


//: Re-evaluate the cost of each detection using whole-contour-matching cost
// This was not during DP so we do this to fine-tune the cost after-the-fact
bool dbsks_recompute_cost_using_wcm();


//: Combine detection results from multiple folders. Examples: detection with
// different scales or different prototypes. Do non-max suppression if 
// the detections overlap.
bool dbsks_combine_det_results(const std::string& orig_exp_folder, 
                               const std::string& exp_name1,
                               const std::string& exp_name2,
                               double min_overlap_ratio_for_rejection,
                               const std::string& dest_exp_path,
                               const std::string& prototype_name);


//: For each xshock detection record in txt format, add an equivalent .xml file
bool dbsks_convert_det_record_txt_to_xml(const std::string& exp_folder);

//: Create B/W of shape boundary for each detected shape and update detection record
bool dbsks_create_bnd_screenshot(const std::string& exp_folder);

//: Combine multiple detection records per image
bool dbsks_combine_det_records_per_image(const std::string& exp_folder, 
                                         const std::string& output_folder,
                                         const std::string& xgraph_prototype_name);

//: Filter detection results using geometric constraints;
bool dbsks_filter_dets_using_geometric_constraints();

// =============================================================================
// Helper functions 
// =============================================================================

//: Compute wcm cost of a list of xgraphs in a given image (object)
bool dbsks_compute_wcm_cost(std::string exp_input_xml_file, std::string object_name, 
                            std::string xgraph_file,
                            std::map<float, float >& map_unmatched_weight_to_wcm_confidence);


#endif // dbsks/cmdline_detect/dbsks_cmdline_detect_utils.h

