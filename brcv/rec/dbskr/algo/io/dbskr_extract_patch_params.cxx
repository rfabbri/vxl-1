//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_extract_patch_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_extract_patch_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "parsing dbskr_extract_patch_params\n";
  bxml_element query("dbskr_extract_patch_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_extract_patch_params::parse_from_data() - could not find the node dbskr_extract_patch_params\n";
    return false;
  }

  bxml_element query2("patch_data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_extract_patch_params::parse_from_data() - could not find the node patch_data\n";
    return false;
  }
   
  bxml_element * data1 = static_cast<bxml_element*>(result2.ptr());
  if (!data1)
    return false;

  vcl_string start_depth_str, end_depth_str, depth_interval_str;
  data1->get_attribute("start_depth", start_depth_);
  data1->get_attribute("start_depth", start_depth_str);
  data1->get_attribute("end_depth", end_depth_);
  data1->get_attribute("end_depth", end_depth_str);
  data1->get_attribute("depth_interval", depth_interval_); 
  data1->get_attribute("depth_interval", depth_interval_str); 
  
  vcl_string val;
  
  data1->get_attribute("extract_from_tree", val);
  extract_from_tree_ = val.compare("off") == 0 ? false: true; // if val = off then localize_edit = false

  data1->get_attribute("put_only_tree", val);
  put_only_tree_ = val.compare("off") == 0 ? false: true; // if val = off then localize_edit = false

  data1->get_attribute("circular_ends", val);
  circular_ends_ = val.compare("off") == 0 ? false: true; // if val = off then localize_edit = false

  ///////////////////
  bxml_element query3("patch_data_extra");
  bxml_data_sptr result3 = bxml_find_by_name(result, query3);

  if (!result3) {
    vcl_cout << "dbskr_extract_patch_params::parse_from_data() - could not find the node patch_data_extra\n";
    return false;
  }
   
  bxml_element * data2 = static_cast<bxml_element*>(result3.ptr());
  if (!data2)
    return false;

  //: extra params needed for extraction from real images
  vcl_string pruning_depth_str, sort_threshold_str, overlap_threshold_str;
  data2->get_attribute("pruning_depth", pruning_depth_);
  data2->get_attribute("pruning_depth", pruning_depth_str);
  data2->get_attribute("sort_threshold", sort_threshold_);
  data2->get_attribute("sort_threshold", sort_threshold_str);
  data2->get_attribute("contour_ratio", val);
  contour_ratio_ = val.compare("off") == 0 ? false : true;  // use contour ratio to sort the patches, if false use color contrast ratio i.e. sort_type = "_color_";
  data2->get_attribute("area_threshold_ratio", area_threshold_ratio_);  // usually fixed and 0.01% of the total image area 
  data2->get_attribute("overlap_threshold", overlap_threshold_);
  data2->get_attribute("overlap_threshold", overlap_threshold_str);
  data2->get_attribute("keep_pruned", val);            // keep pruned patches to save their images for debugging purposes
  keep_pruned_ = val.compare("off") == 0 ? false : true;
  data2->get_attribute("save_images", val);
  save_images_ = val.compare("off") == 0 ? false : true;
  data2->get_attribute("save_discarded_images", val);
  save_discarded_images_ = val.compare("off") == 0 ? false : true;
  data2->get_attribute("image_ext", image_ext_);

  //////////////////////////

  output_file_postfix_ = "";
  if (extract_from_tree_) {
    output_file_postfix_ = output_file_postfix_ + "-model";
    if (extract_from_tree_ && put_only_tree_)
      output_file_postfix_ = output_file_postfix_ + "-tree";
    if (!circular_ends_)
      output_file_postfix_ = output_file_postfix_ + "-nocirc";
    
    output_file_postfix_ = output_file_postfix_ + "-s-" + start_depth_str + "-e-" + end_depth_str + "-i-" + depth_interval_str;

  } else {
    if (!circular_ends_)
      output_file_postfix_ = output_file_postfix_ + "-nocirc";
    if (!contour_ratio_)
      output_file_postfix_ = output_file_postfix_ + "-color";

    output_file_postfix_ = output_file_postfix_ + "-s-" + start_depth_str + "-e-" + end_depth_str + "-i-" + depth_interval_str;
    output_file_postfix_ = output_file_postfix_ + "-pd-" + pruning_depth_str + "-st-" + sort_threshold_str + "-ot-" + overlap_threshold_str;
  }

  //vcl_cout << "output_file_postfix: " << output_file_postfix_ << vcl_endl;

  return true;
}

bxml_element *dbskr_extract_patch_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_extract_patch_params");

  bxml_element * data1 = new bxml_element("patch_data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("start_depth", "1");
  data1->set_attribute("end_depth", "1");
  data1->set_attribute("depth_interval", "1"); 
  data1->set_attribute("extract_from_tree", "off");
  data1->set_attribute("put_only_tree", "off");
  data1->set_attribute("circular_ends", "off");
  data1->append_text("\n");

  bxml_element * data2 = new bxml_element("patch_data_extra");
  //: extra params needed for extraction from real images
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("pruning_depth", "2");
  data2->set_attribute("sort_threshold", "0.5");
  data2->set_attribute("contour_ratio", "off");  // use contour ratio to sort the patches, if false use color contrast ratio i.e. sort_type = "_color_";
  data2->set_attribute("area_threshold_ratio", "0.01");  // usually fixed and 0.01% of the total image area 
  data2->set_attribute("overlap_threshold", "0.8");
  data2->set_attribute("keep_pruned", "on");            // keep pruned patches to save their images for debugging purposes
  data2->set_attribute("save_images", "on");
  data2->set_attribute("save_discarded_images", "off");
  data2->set_attribute("image_ext", "png");
  data2->append_text("\n");

  return root;
}

bxml_element *dbskr_extract_patch_params::create_document_data()
{
  bxml_element * root = new bxml_element("dbskr_extract_patch_params");

  bxml_element * data1 = new bxml_element("patch_data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("start_depth", start_depth_);
  data1->set_attribute("end_depth", end_depth_);
  data1->set_attribute("depth_interval", depth_interval_); 
  data1->set_attribute("extract_from_tree", extract_from_tree_ ? "on" : "off");
  data1->set_attribute("put_only_tree", put_only_tree_ ? "on" : "off");
  data1->set_attribute("circular_ends", circular_ends_ ? "on" : "off");
  data1->append_text("\n");

  bxml_element * data2 = new bxml_element("patch_data_extra");
  //: extra params needed for extraction from real images
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("pruning_depth", pruning_depth_);
  data2->set_attribute("sort_threshold", sort_threshold_);
  data2->set_attribute("contour_ratio", contour_ratio_ ? "on" : "off" );  // use contour ratio to sort the patches, if false use color contrast ratio i.e. sort_type = "_color_";
  data2->set_attribute("area_threshold_ratio", area_threshold_ratio_);  // usually fixed and 0.01% of the total image area 
  data2->set_attribute("overlap_threshold", overlap_threshold_);
  data2->set_attribute("keep_pruned", keep_pruned_ ? "on" : "off");            // keep pruned patches to save their images for debugging purposes
  data2->set_attribute("save_images", save_images_ ? "on" : "off");
  data2->set_attribute("save_discarded_images", save_discarded_images_ ? "on" : "off");
  data2->set_attribute("image_ext", image_ext_);
  data2->append_text("\n");

  return root;
}

