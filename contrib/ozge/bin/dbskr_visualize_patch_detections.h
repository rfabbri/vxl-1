//:
// \file
// \brief A command line executable to load two patch sets and their match to visualize patch match table 
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dbskr_visualize_patch_detections_h)
#define _dbskr_visualize_patch_detections_h

#include <dbskr/algo/io/dbskr_detect_patch_params.h>

#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>

#include <vcl_string.h>
#include <vcl_vector.h>

class dbskr_visualize_patch_detections 
{
public:
  dbskr_visualize_patch_detections(dbskr_detect_patch_params& params, vcl_string query_path, vcl_string query_name, vcl_string ext, vcl_string html_img_ext) : 
      params_(params), query_path_(query_path), query_name_(query_name), ext_(ext), html_image_ext_(html_img_ext), html_image_width_(10), create_only_html_(false), show_match_images_(false) {}
  bool initialize();
  bool process();
  bool finalize();

  int html_image_width_;
  bool create_only_html_;
  bool show_match_images_;

protected:
  dbskr_detect_patch_params params_;
  vcl_string query_name_;
  vcl_string query_path_;
  vcl_string ext_;
  vcl_string html_image_ext_;

  dbskr_shock_patch_storage_sptr model_st;
  dbskr_shock_patch_storage_sptr query_st;
  dbskr_shock_patch_match_sptr match;
};

#endif  //_dbskr_visualize_patch_detections_h
