//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_extract_bnd_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_extract_bnd_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "parsing dbskr_extract_bnd_params\n";
  bxml_element query("dbskr_extract_bnd_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_extract_bnd_params::parse_from_data() - could not find the node dbskr_extract_bnd_params\n";
    return false;
  }

  bxml_element query2("bnd_data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_extract_bnd_params::parse_from_data() - could not find the node bnd_data\n";
    return false;
  }
   
  bxml_element * data1 = static_cast<bxml_element*>(result2.ptr());
  if (!data1)
    return false;

  vcl_string nrad_str, dx_str, dt_str, max_k_str, min_size_str, smoothing_nsteps_str, rms_str, edge_im_thres_str;
  data1->get_attribute("nrad", nrad_);
  data1->get_attribute("nrad", nrad_str);
  data1->get_attribute("dx", dx_);
  data1->get_attribute("dx", dx_str);
  data1->get_attribute("dt", dt_); 
  data1->get_attribute("dt", dt_str); 
  data1->get_attribute("max_k", max_k_); 
  data1->get_attribute("max_k", max_k_str); 
  data1->get_attribute("min_size_to_keep", min_size_to_keep_); 
  data1->get_attribute("min_size_to_keep", min_size_str); 
  data1->get_attribute("smoothing_nsteps", smoothing_nsteps_); 
  data1->get_attribute("smoothing_nsteps", smoothing_nsteps_str); 

  vcl_string val;
  data1->get_attribute("smooth_bnds", val);
  smooth_bnds_ = val.compare("off") == 0 ? false : true;

  data1->get_attribute("fit_lines", val);
  fit_lines_ = val.compare("off") == 0 ? false: true; // if val = off then localize_edit = false

  data1->get_attribute("rms", rms_);
  data1->get_attribute("rms", rms_str);

  data1->get_attribute("run_contour_tracing", val);
  run_contour_tracing_ = val.compare("off") == 0 ? false : true;

  data1->get_attribute("extract_from_edge_img", val);
  extract_from_edge_img_ = val.compare("off") == 0 ? false: true;
  data1->get_attribute("edge_img_threshold", edge_img_threshold_);
  data1->get_attribute("edge_img_threshold", edge_im_thres_str);
  data1->get_attribute("generic_linker", val);
  generic_linker_ = val.compare("off") == 0 ? false : true;

  bxml_element query3("bnd_data_new");
  bxml_data_sptr result3 = bxml_find_by_name(result, query3);

  if (!result3) {
    vcl_cout << "dbskr_extract_bnd_params::parse_from_data() - could not find the node bnd_data_new\n";
    return false;
  }
   
  bxml_element * data2 = static_cast<bxml_element*>(result3.ptr());
  if (!data2)
    return false;

  data2->get_attribute("require_appearance_consistency", val);
  require_appearance_consistency_ = val.compare("off") == 0 ? false : true;  // if generic linker
  //vcl_string appearance_consistency_threshold_str;
  data2->get_attribute("appearance_consistency_threshold", appearance_consistency_threshold_); // if generic linker, this threshold was 2.0f on the order of edge strength
//  data2->get_attribute("appearance_consistency_threshold", appearance_consistency_threshold_str); // if generic linker, this threshold was 2.0f on the order of edge strength

  data2->get_attribute("max_size_to_group", max_size_to_group_);  // if Amir's linker
  data2->get_attribute("min_size_to_link", min_size_to_link_);   // if Amir's linker
  data2->get_attribute("edge_detection_sigma", edge_detection_sigma_);
  data2->get_attribute("edge_detection_thresh", edge_detection_thresh_);
  data2->get_attribute("avg_grad_mag_threshold", avg_grad_mag_threshold_);
  data2->get_attribute("length_thresh", length_thresh_);
  data2->get_attribute("pruning_color_threshold", pruning_color_threshold_);
  data2->get_attribute("pruning_region_width", pruning_region_width_);
  
  output_file_postfix_ = "";
  if (extract_from_edge_img_) 
    output_file_postfix_ = output_file_postfix_ + "-edgeim-t-"+edge_im_thres_str;
    
  output_file_postfix_ = output_file_postfix_ + "-" + nrad_str + "-" + dx_str + "-" + dt_str + "-" + max_k_str + "-" + min_size_str + "-" + smoothing_nsteps_str;

  if (fit_lines_)
    output_file_postfix_ = output_file_postfix_ + "-fitted-" + rms_str;
  
  //vcl_cout << "output_file_postfix: " << output_file_postfix_ << vcl_endl;

  return true;
}

bxml_element *dbskr_extract_bnd_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_extract_bnd_params");

  bxml_element * data1 = new bxml_element("bnd_data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("nrad", "3.0");
  data1->set_attribute("dx", "0.2");
  data1->set_attribute("dt", "0.1"); 
  data1->set_attribute("max_k", "0.2"); 
  data1->set_attribute("min_size_to_keep", "5"); 
  data1->set_attribute("smooth_bnds", "on");
  data1->set_attribute("smoothing_nsteps", "10"); // used 100 for ethz shape dataset
  data1->set_attribute("fit_lines", "on");
  data1->set_attribute("rms", "0.05");
  data1->set_attribute("run_contour_tracing", "off");
  data1->set_attribute("extract_from_edge_img", "on");
  data1->set_attribute("edge_img_threshold", 20);
  data1->set_attribute("generic_linker", "on");
  data1->append_text("\n");

  bxml_element * data2 = new bxml_element("bnd_data_new");
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("require_appearance_consistency", "off");  // if generic linker
  data2->set_attribute("appearance_consistency_threshold", "2.0"); // if generic linker, this threshold was 2.0f on the order of edge strength

  data2->set_attribute("max_size_to_group", "7");  // if Amir's linker
  data2->set_attribute("min_size_to_link", "3");   // if Amir's linker

  data2->set_attribute("edge_detection_sigma", "1.0");
  data2->set_attribute("edge_detection_thresh", "1.0");

  data2->set_attribute("avg_grad_mag_threshold", "3.0");
  data2->set_attribute("length_thresh", "3.0");
  data2->set_attribute("pruning_color_threshold", "0.5");
  data2->set_attribute("pruning_region_width", "5");
  data2->append_text("\n");
  
  return root;
}

bxml_element *dbskr_extract_bnd_params::create_document_data()
{
  bxml_element * root = new bxml_element("dbskr_extract_bnd_params");

  bxml_element * data1 = new bxml_element("bnd_data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("nrad", nrad_);
  data1->set_attribute("dx", dx_);
  data1->set_attribute("dt", dt_); 
  data1->set_attribute("max_k", max_k_); 
  data1->set_attribute("min_size_to_keep", min_size_to_keep_); 
  data1->set_attribute("smooth_bnds", smooth_bnds_ ? "on" : "off");
  data1->set_attribute("smoothing_nsteps", smoothing_nsteps_); 
  data1->set_attribute("fit_lines", fit_lines_ ? "on" : "off");
  data1->set_attribute("rms", rms_);
  data1->set_attribute("run_contour_tracing", run_contour_tracing_ ? "on" : "off");
  data1->set_attribute("extract_from_edge_img", extract_from_edge_img_ ? "on" : "off" );
  data1->set_attribute("edge_img_threshold", edge_img_threshold_);
  data1->set_attribute("generic_linker", generic_linker_ ? "on" : "off");
  data1->append_text("\n");

  bxml_element * data2 = new bxml_element("bnd_data_new");
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("require_appearance_consistency", require_appearance_consistency_ ? "on" : "off");  // if generic linker
  data2->set_attribute("appearance_consistency_threshold", appearance_consistency_threshold_); // if generic linker, this threshold was 2.0f on the order of edge strength

  data2->set_attribute("max_size_to_group", max_size_to_group_);  // if Amir's linker
  data2->set_attribute("min_size_to_link", min_size_to_link_);   // if Amir's linker

  data2->set_attribute("edge_detection_sigma", edge_detection_sigma_);
  data2->set_attribute("edge_detection_thresh", edge_detection_thresh_);

  data2->set_attribute("avg_grad_mag_threshold", avg_grad_mag_threshold_);
  data2->set_attribute("length_thresh", length_thresh_);
  data2->set_attribute("pruning_color_threshold", pruning_color_threshold_);
  data2->set_attribute("pruning_region_width", pruning_region_width_);
  data2->append_text("\n");
  
  return root;
}

