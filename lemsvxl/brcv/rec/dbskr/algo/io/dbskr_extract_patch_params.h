//:
// \file
// \brief The extract patch parameter class 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 11/08/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_extract_patch_params_h)
#define _dbskr_extract_patch_params_h

#include <bxml/bxml_document.h>
#include <dborl/dborl_algo_params_base.h>

class dbskr_extract_patch_params
{ public:

    int start_depth_, end_depth_, depth_interval_;
    bool extract_from_tree_; 
    bool put_only_tree_;     // a special case of patch detections for simple closed contours (models) --> a single patch containing the entire tree
    bool circular_ends_;     // use circular completions at leaf branches during shock matching, def: true 

    //: extra params needed for extraction from real images
    int pruning_depth_;
    float sort_threshold_;
    bool contour_ratio_;  // use contour ratio to sort the patches, if false use color contrast ratio i.e. sort_type = "_color_";
    float area_threshold_ratio_;  // usually fixed and 0.01% of the total image area 
    float overlap_threshold_;
    bool keep_pruned_;            // keep pruned patches to save their images for debugging purposes
    bool save_images_;
    bool save_discarded_images_;
    vcl_string image_ext_;

    vcl_string output_file_postfix_;

    bool parse_from_data(bxml_data_sptr root);
    static bxml_element *create_default_document_data();
    bxml_element *create_document_data();
};

class dbskr_extract_patch_params2 : public dborl_algo_params_base
{ public:

    dborl_parameter<int> start_depth_;
    dborl_parameter<int> end_depth_;
    dborl_parameter<int> depth_interval_;
    dborl_parameter<bool> extract_from_tree_; 
    dborl_parameter<bool> put_only_tree_;     // a special case of patch detections for simple closed contours (models) --> a single patch containing the entire tree
    dborl_parameter<bool> circular_ends_;     // use circular completions at leaf branches during shock matching, def: true 

    //: extra params needed for extraction from real images
    dborl_parameter<int> pruning_depth_;
    dborl_parameter<float> sort_threshold_;
    dborl_parameter<bool> contour_ratio_;  // use contour ratio to sort the patches, if false use color contrast ratio i.e. sort_type = "_color_";
    dborl_parameter<float> area_threshold_ratio_;  // usually fixed and 0.01% of the total image area 
    dborl_parameter<float> overlap_threshold_;
    dborl_parameter<bool> keep_pruned_;            // keep pruned patches to save their images for debugging purposes
    dborl_parameter<bool> save_images_;
    dborl_parameter<bool> save_discarded_images_;
    dborl_parameter<vcl_string> image_ext_;

    dbskr_extract_patch_params2() : dborl_algo_params_base("ext_patch") { 
      start_depth_.set_values(param_list_, "data", "start_depth", "start_depth", 1, 1);
      end_depth_.set_values(param_list_, "data", "end_depth", "end_depth", 1, 1);
      depth_interval_.set_values(param_list_, "data", "depth_interval", "depth_interval", 1, 1);
      
      pruning_depth_.set_values(param_list_, "data", "pruning_depth", "pruning_depth", 2, 2);

      extract_from_tree_.set_values(param_list_, "data", "extract_from_tree", "set on when extracting from a shock tree, e.g. extracting prototype patches", false, false);
      put_only_tree_.set_values(param_list_, "data", "put_only_tree", "a special case of patch detections for simple closed contours (models): a single patch containing the entire tree", false, false);

      circular_ends_.set_values(param_list_, "data", "circular_ends", "use circular completions at leaf branches during shock matching", true, true);

      sort_threshold_.set_values(param_list_, "data", "sort_threshold", "sort_threshold", 0.5f, 0.5f);
      contour_ratio_.set_values(param_list_, "data", "contour_ratio", "use contour ratio to sort the patches, if false use color contrast ratio", false, false);
      area_threshold_ratio_.set_values(param_list_, "data", "area_threshold_ratio", "usually fixed and 0.01% of the total image area", 0.01f, 0.01f);
      overlap_threshold_.set_values(param_list_, "data", "overlap_threshold", "prune patches if they have this much node overlaps with a previously extracted patch", 0.8f, 0.8f);

      keep_pruned_.set_values(param_list_, "data", "keep_pruned", "keep pruned patches to save their images for debugging purposes", true, true);
      save_images_.set_values(param_list_, "data", "save_images", "save patch images", true, true);
      save_discarded_images_.set_values(param_list_, "data", "save_discarded_images", "save discarded patch images", false, false);
      image_ext_.set_values(param_list_, "data", "image_ext", "image_ext to save the images", "png", "png");
      
    }
};


#endif  //_dbskr_extract_patch_params_h
