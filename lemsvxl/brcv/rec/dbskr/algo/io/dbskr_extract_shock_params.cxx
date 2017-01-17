//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_extract_shock_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_extract_shock_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "parsing dbskr_extract_shock_params\n";
  bxml_element query("dbskr_extract_shock_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_extract_shock_params::parse_from_data() - could not find the node dbskr_extract_shock_params\n";
    return false;
  }

  bxml_element query2("data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_extract_shock_params::parse_from_data() - could not find the node data\n";
    return false;
  }
   
  bxml_element * data1 = static_cast<bxml_element*>(result2.ptr());
  if (!data1)
    return false;

  vcl_string prune_threshold_str, sampling_ds_str, curve_length_gamma_str, cont_thres_str, app_thres_str, alpha_cont_str;
  data1->get_attribute("prune_threshold", prune_threshold_);
  data1->get_attribute("prune_threshold", prune_threshold_str);
  data1->get_attribute("sampling_ds", sampling_ds_);
  data1->get_attribute("sampling_ds", sampling_ds_str);
  vcl_string val;
  data1->get_attribute("add_noise_in_a_loop", val);  // if output shock has zero nodes then adds the following amount of noise randomly in a loop for at most 100 times
  add_noise_in_a_loop_ = val.compare("off") == 0 ? false : true;
  data1->get_attribute("noise_radius", noise_radius_);
  
  data1->get_attribute("perform_gap_transforms", val);
  perform_gap_transforms_ = val.compare("off") == 0 ? false: true; 

  data1->get_attribute("curve_length_gamma", curve_length_gamma_);
  data1->get_attribute("curve_length_gamma", curve_length_gamma_str);
  data1->get_attribute("cont_thres", cont_thres_);
  data1->get_attribute("cont_thres", cont_thres_str);
  data1->get_attribute("app_thres", app_thres_);
  data1->get_attribute("app_thres", app_thres_str);
  data1->get_attribute("alpha_cont", alpha_cont_);  
  data1->get_attribute("alpha_cont", alpha_cont_str);  
  data1->get_attribute("alpha_app", alpha_app_);

  output_file_postfix_ = "";
  output_file_postfix_ = output_file_postfix_ + "-pt-" + prune_threshold_str + "-ds-" + sampling_ds_str;

  if (perform_gap_transforms_) 
    output_file_postfix_ = output_file_postfix_ + "-gaptr-" + "-cg-" + curve_length_gamma_str + "-ct-" + cont_thres_str + "-at-" + app_thres_str + "-a-" + alpha_cont_str;
  
  //vcl_cout << "output_file_postfix: " << output_file_postfix_ << vcl_endl;

  return true;
}

bxml_element *dbskr_extract_shock_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_extract_shock_params");

  bxml_element * data1 = new bxml_element("data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("prune_threshold", "1.0");
  data1->set_attribute("sampling_ds", "1.0");
  data1->set_attribute("add_noise_in_a_loop", "on");  // if output shock has zero nodes then adds the following amount of noise randomly in a loop for at most 100 times
  data1->set_attribute("noise_radius", "0.002");
  
  data1->set_attribute("perform_gap_transforms", "on");
  data1->set_attribute("curve_length_gamma", "2.0");
  data1->set_attribute("cont_thres", "0.5");
  data1->set_attribute("app_thres", "0.5");
  data1->set_attribute("alpha_cont", "0.5");  // equal importance to contour and appearance continuity
  data1->set_attribute("alpha_app", "0.5");
  data1->append_text("\n");

  return root;
}

bxml_element *dbskr_extract_shock_params::create_document_data()
{
  bxml_element * root = new bxml_element("dbskr_extract_shock_params");

  bxml_element * data1 = new bxml_element("data");
  root->append_data(data1);
  root->append_text("\n");
  data1->set_attribute("prune_threshold", prune_threshold_);
  data1->set_attribute("sampling_ds", sampling_ds_);
  data1->set_attribute("add_noise_in_a_loop", add_noise_in_a_loop_ ? "on" : "off");
  data1->set_attribute("noise_radius", noise_radius_);
  
  data1->set_attribute("perform_gap_transforms", perform_gap_transforms_ ? "on" : "off");
  data1->set_attribute("curve_length_gamma", curve_length_gamma_);
  data1->set_attribute("cont_thres", cont_thres_);
  data1->set_attribute("app_thres", app_thres_);
  data1->set_attribute("alpha_cont", alpha_cont_);
  data1->set_attribute("alpha_app", alpha_app_);
  data1->append_text("\n");
    
  return root;
}

