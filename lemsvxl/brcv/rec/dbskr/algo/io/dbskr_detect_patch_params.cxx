//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_detect_patch_params.h"
#include "dbskr_match_shock_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_detect_instance_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "parsing dbskr_detect_instance_params\n";
  bxml_element query("dbskr_detect_instance_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_detect_instance_params::parse_from_data() - could not find the node dbskr_detect_instance_params\n";
    return false;
  }

  bxml_element * data1 = static_cast<bxml_element*>(result.ptr());
  if (!data1)
    return false;

  vcl_string top_N_str, k_str;
  data1->get_attribute("top_N", top_N_);
  data1->get_attribute("top_N", top_N_str);
  data1->get_attribute("k", k_);
  data1->get_attribute("k", k_str);
  
  output_file_postfix_ = "";

  output_file_postfix_ = output_file_postfix_ + "-topN-" + top_N_str + "-k-" + k_str;
  //vcl_cout << "output_file_postfix: " << output_file_postfix_ << vcl_endl;

  return true;
}

bxml_element *dbskr_detect_instance_params::create_default_document_data() 
{  
  bxml_element * root = new bxml_element("dbskr_detect_instance_params");
  root->append_text("\n");
  root->set_attribute("top_N", "3");
  root->set_attribute("k", "1");
  
  return root;
}

bxml_element *dbskr_detect_instance_params::create_document_data()
{
  bxml_element * root = new bxml_element("dbskr_detect_instance_params");
  root->append_text("\n");
  root->set_attribute("top_N", top_N_);
  root->set_attribute("k", k_);
  return root;
}



bool dbskr_detect_patch_params::parse_from_data(bxml_data_sptr root)
{
  vcl_cout << "parsing dbskr_detect_patch_params\n";
  bxml_element query("dbskr_detect_patch_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node dbskr_detect_patch_params\n";
    return false;
  }

  bxml_element query2("detect_patch_data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node detect_patch_data\n";
    return false;
  }
   
  bxml_element * data0 = static_cast<bxml_element*>(result2.ptr());
  if (!data0)
    return false;

  vcl_string min_thres_str, thres_inc_str, max_thres_str, box_overlap_ratio_threshold_str;
  data0->get_attribute("min_thres", min_thres_);
  data0->get_attribute("min_thres", min_thres_str);
  data0->get_attribute("thres_increment", thres_inc_);
  data0->get_attribute("thres_increment", thres_inc_str);
  data0->get_attribute("max_thres", max_thres_); 
  data0->get_attribute("max_thres", max_thres_str); 
  data0->get_attribute("box_overlap_ratio_threshold", box_overlap_ratio_threshold_);  
  data0->get_attribute("box_overlap_ratio_threshold", box_overlap_ratio_threshold_str);  
  
  vcl_string val;
  data0->get_attribute("use_normalized_costs", val); 
  use_normalized_costs_ = val.compare("off") == 0 ? false: true; 
  
  data0->get_attribute("use_reconst_boundary_length", val);  // use this value computed by the dbskr_tree for normalization
  use_reconst_boundary_length_ = val.compare("off") == 0 ? false: true; 

  data0->get_attribute("use_total_splice_cost", val);             // use this value computed by the dbskr_tree for normalization
  use_total_splice_cost_ = val.compare("off") == 0 ? false: true; 

  if (use_reconst_boundary_length_ == use_total_splice_cost_) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - Error in input file: either both use_reconst_boundary_length and use_total_splice_cost ON or both are OFF\n";
    return false;
  }

  data0->get_attribute("do_categorization", val);
  do_categorization_ = val.compare("off") == 0 ? false : true;

  output_file_postfix_ = "";  
  output_file_postfix_ = output_file_postfix_ + "-mint-" + min_thres_str + "-ti-" + thres_inc_str + "-maxt-" + max_thres_str + "-bort-" + box_overlap_ratio_threshold_str;
  
  bxml_element query3("detect_patch_dirs");
  bxml_data_sptr result3 = bxml_find_by_name(result, query3);

  if (!result3) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node detect_patch_dirs\n";
    return false;
  }

  bxml_element * data6 = static_cast<bxml_element*>(result3.ptr());
  if (!data6)
    return false;
  
  data6->get_attribute("index_file",index_file_);
  data6->get_attribute("model_object_dir",model_dir_);
  data6->get_attribute("model_object_name",model_name_);
  data6->get_attribute("matches_dir",matches_dir_);
  data6->get_attribute("output_file", out_file_);

  det_params_.parse_from_data(result);

  bxml_element query4("shock_match_params");
  bxml_data_sptr result4 = bxml_find_by_name(result, query4);

  if (!result4) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node shock_match_params\n";
    return false;
  }
   
  dbskr_match_shock_params dummy;
  dummy.parse_from_match_data(result4);
  shock_match_params_ = dummy.edit_params_;

  bxml_element query5("query_patches_params");
  bxml_data_sptr result5 = bxml_find_by_name(result, query5);

  if (!result5) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node query_patches_params\n";
    return false;
  }

  query_patch_params_.parse_from_data(result5);

  bxml_element query6("model_patches_params");
  bxml_data_sptr result6 = bxml_find_by_name(result, query6);

  if (!result6) {
    vcl_cout << "dbskr_detect_patch_params::parse_from_data() - could not find the node model_patches_params\n";
    return false;
  }

  model_patch_params_.parse_from_data(result6);
  
  vcl_cout << "output_file_postfix: " << output_file_postfix_ << vcl_endl;
  return true;
}

bxml_element *dbskr_detect_patch_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_detect_patch_params");

  bxml_element * data0 = new bxml_element("detect_patch_data");
  root->append_data(data0);
  root->append_text("\n");
  
  data0->set_attribute("min_thres", "0.1");
  data0->set_attribute("thres_increment", "0.05");
  data0->set_attribute("max_thres", "0.9"); 
  data0->set_attribute("box_overlap_ratio_threshold", "0.5");  
  data0->set_attribute("use_normalized_costs", "on");
  data0->set_attribute("use_reconst_boundary_length", "on");  // use this value computed by the dbskr_tree for normalization
  data0->set_attribute("use_total_splice_cost", "off");             // use this value computed by the dbskr_tree for normalization
  data0->set_attribute("do_categorization", "off");

  data0->append_text("\n");

  bxml_element * data6 = new bxml_element("detect_patch_dirs");
  root->append_data(data6);
  root->append_text("\n");

  data6->set_attribute("index_file","/vision/projects/kimia/categorization/99-db/orl-exps/99-db-flat.xml");
  data6->set_attribute("model_object_dir","/vision/projects/kimia/categorization/99-db/orl-exps/dataset/bonefishes/");
  data6->set_attribute("model_object_name","bonefishes");  // will add param names of model patch extraction to locate the storage
  data6->set_attribute("matches_dir","/vision/projects/kimia/categorization/99-db/orl-exps/patch-matches-5-1/");
  data6->set_attribute("output_file","vision/projects/kimia/categorization/99-db/orl-exps/dborl_detect_out");
  data6->append_text("\n");
  
  bxml_element * data5 = dbskr_detect_instance_params::create_default_document_data();
  root->append_data(data5);
  root->append_text("\n");

  bxml_element * data1 = new bxml_element("shock_match_params");
  root->append_data(data1);
  root->append_text("\n");
  bxml_element * data1_1 = dbskr_match_shock_params::create_default_document_match_data();
  data1->append_data(data1_1);
  data1->append_text("\n");

  bxml_element * data3 = new bxml_element("query_patches_params");
  root->append_data(data3);
  root->append_text("\n");
  bxml_element * data3_1 = dbskr_extract_patch_params::create_default_document_data();
  data3->append_data(data3_1);
  data3->append_text("\n");

  bxml_element * data4 = new bxml_element("model_patches_params");
  root->append_data(data4);
  root->append_text("\n");
  bxml_element * data4_1 = dbskr_extract_patch_params::create_default_document_data();
  data4->append_data(data4_1);
  data4->append_text("\n");

  return root;
}

bxml_element *dbskr_detect_patch_params::create_document_data()
{
  bxml_element * root = new bxml_element("dbskr_detect_patch_params");

  bxml_element * data0 = new bxml_element("detect_patch_data");
  root->append_data(data0);
  root->append_text("\n");
  
  data0->set_attribute("min_thres", min_thres_);
  data0->set_attribute("thres_increment", thres_inc_);
  data0->set_attribute("max_thres", max_thres_); 
  data0->set_attribute("box_overlap_ratio_threshold", box_overlap_ratio_threshold_);  
  data0->set_attribute("use_normalized_costs", use_normalized_costs_ ? "on" : "off");
  data0->set_attribute("use_reconst_boundary_length", use_reconst_boundary_length_ ? "on" : "off");  // use this value computed by the dbskr_tree for normalization
  data0->set_attribute("use_total_splice_cost", use_total_splice_cost_ ? "on" : "off");             // use this value computed by the dbskr_tree for normalization
  data0->set_attribute("do_categorization", do_categorization_ ? "on" : "off");
  data0->append_text("\n");

  bxml_element * data6 = new bxml_element("detect_patch_dirs");
  root->append_data(data6);
  root->append_text("\n");

  data6->set_attribute("index_file",index_file_);
  data6->set_attribute("model_object_dir",model_dir_);
  data6->set_attribute("model_object_name",model_name_);  // will add param names of model patch extraction to locate the storage
  data6->set_attribute("matches_dir",matches_dir_);
  data6->set_attribute("output_file",out_file_);
  data6->append_text("\n");
  
  bxml_element * data5 = det_params_.create_document_data();
  root->append_data(data5);
  root->append_text("\n");

  dbskr_match_shock_params dummy;
  dummy.edit_params_ = shock_match_params_;
  
  bxml_element * data1 = new bxml_element("shock_match_params");
  root->append_data(data1);
  root->append_text("\n");
  bxml_element * data1_1 = dummy.create_document_match_data();
  data1->append_data(data1_1);
  data1->append_text("\n");

  bxml_element * data3 = new bxml_element("query_patches_params");
  root->append_data(data3);
  root->append_text("\n");
  bxml_element * data3_1 = query_patch_params_.create_document_data();
  data3->append_data(data3_1);
  data3->append_text("\n");

  bxml_element * data4 = new bxml_element("model_patches_params");
  root->append_data(data4);
  root->append_text("\n");
  bxml_element * data4_1 = model_patch_params_.create_document_data();
  data4->append_data(data4_1);
  data4->append_text("\n");

  return root;
}

