//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_match_shock_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_match_shock_params::parse_from_match_data(bxml_data_sptr result)
{
  bxml_element query2("match_data");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_match_shock_params::parse_from_match_data() - could not find the node match_data\n";
    return false;
  }
   
  bxml_element * data1 = static_cast<bxml_element*>(result2.ptr());
  if (!data1)
    return false;

  data1->get_attribute("scurve_sample_ds", edit_params_.scurve_sample_ds_);
  vcl_string sample_ds_str, interp_ds_str, R_str;
  data1->get_attribute("scurve_sample_ds", sample_ds_str);
  data1->get_attribute("scurve_interpolate_ds", edit_params_.scurve_interpolate_ds_); 
  data1->get_attribute("scurve_interpolate_ds", interp_ds_str);
  data1->get_attribute("scurve_matching_R", edit_params_.curve_matching_R_); 
  data1->get_attribute("scurve_matching_R", R_str);

  vcl_string val;
  data1->get_attribute("localized_edit", val);
  edit_params_.localized_edit_ = val.compare("off") == 0 ? false: true; // if val = off then localize_edit = false

  data1->get_attribute("elastic_splice_cost", val);
  edit_params_.elastic_splice_cost_ = val.compare("off") == 0 ? false: true; 

  data1->get_attribute("circular_ends", val);
  edit_params_.circular_ends_ = val.compare("off") == 0 ? false: true; 

  data1->get_attribute("combined_edit", val);
  edit_params_.combined_edit_ = val.compare("off") == 0 ? false: true; 

  data1->get_attribute("coarse_edit", val);
  edit_params_.coarse_edit_ = val.compare("off") == 0 ? false: true; 

  edit_params_.output_file_postfix_ = "";
  if (edit_params_.elastic_splice_cost_)
    edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-elastic";
  if (!edit_params_.circular_ends_)
    edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-nocirc";
  if (edit_params_.combined_edit_)
    edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-combined";
  if (edit_params_.coarse_edit_)
    edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-coarse";
  if (edit_params_.localized_edit_)
    edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-localized";

  edit_params_.output_file_postfix_ = edit_params_.output_file_postfix_ + "-s-" + sample_ds_str + "-i-" + interp_ds_str + "-R-" + R_str;
  //vcl_cout << "output_file_postfix: " << edit_params_.output_file_postfix_ << vcl_endl;

  return true;
}


bool dbskr_match_shock_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "dbskr_match_shock_params\n";

  bxml_element query("dbskr_match_shock_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_match_shock_params::parse_from_data() - could not find the node dbskr_match_shock_params\n";
    return false;
  }

  if (!parse_from_match_data(result))
    return false;

  bxml_element query3("match_dirs");
  bxml_data_sptr result3 = bxml_find_by_name(result, query3);

  if (!result3)
    return false;
   
  bxml_element * data2 = static_cast<bxml_element*>(result3.ptr());
  if (!data2)
    return false;

  data2->get_attribute("traindir", train_dir_);
  data2->get_attribute("trainlist", train_list_);
  data2->get_attribute("dbdir", db_dir_);
  data2->get_attribute("dblist", db_list_);

  vcl_string val;

  data2->get_attribute("saveshgms", val);
  save_shgms_ = val.compare("off") == 0 ? false: true; 
  data2->get_attribute("shgms",shgm_folder_);

  data2->get_attribute("generateoutfile", val);
  generate_output_ = val.compare("off") == 0 ? false: true; 
  data2->get_attribute("out",output_file_);

  data2->get_attribute("orlstyle", val);
  orl_format_folders_ = val.compare("off") == 0 ? false: true; 

  data2->get_attribute("db_train_same", val); 
  db_train_same_ = val.compare("off") == 0 ? false: true; 

  return true;
}

bxml_element *dbskr_match_shock_params::create_default_document_match_data() {
  bxml_element * data1 = new bxml_element("match_data");
  data1->set_attribute("scurve_sample_ds","1.0");
  data1->set_attribute("scurve_interpolate_ds","1.0");
  data1->set_attribute("localized_edit","off");
  data1->set_attribute("elastic_splice_cost","off");
  data1->set_attribute("scurve_matching_R","6.0");
  data1->set_attribute("circular_ends","on");
  data1->set_attribute("combined_edit","off");
  data1->set_attribute("coarse_edit","off");
  data1->append_text("\n");
  return data1;
}

bxml_element *dbskr_match_shock_params::create_document_match_data()
{
  bxml_element * data1 = new bxml_element("match_data");
  vcl_ostringstream oss;
  oss.setf(vcl_ios::fixed, vcl_ios::floatfield);
  oss.precision(1);
  oss << edit_params_.scurve_sample_ds_;

  //data1->set_attribute("scurve_sample_ds",edit_params_.scurve_sample_ds_);
  data1->set_attribute("scurve_sample_ds",oss.str());
  vcl_ostringstream oss2;
  oss2.setf(vcl_ios::fixed, vcl_ios::floatfield);
  oss2.precision(1);
  oss2 << edit_params_.scurve_interpolate_ds_;
  data1->set_attribute("scurve_interpolate_ds",oss2.str());
  data1->set_attribute("localized_edit",edit_params_.localized_edit_ ? "on" : "off");
  data1->set_attribute("elastic_splice_cost",edit_params_.elastic_splice_cost_ ? "on" : "off");
  vcl_ostringstream oss3;
  oss3.setf(vcl_ios::fixed, vcl_ios::floatfield);
  oss3.precision(1);
  oss3 << edit_params_.curve_matching_R_;
  data1->set_attribute("scurve_matching_R",oss3.str());
  data1->set_attribute("circular_ends",edit_params_.circular_ends_ ? "on" : "off");
  data1->set_attribute("combined_edit",edit_params_.combined_edit_ ? "on" : "off");
  data1->set_attribute("coarse_edit",edit_params_.coarse_edit_ ? "on" : "off");
  data1->append_text("\n");
  return data1;
}

bxml_element *dbskr_match_shock_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_match_shock_params");

  bxml_element * data1 = create_default_document_match_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_element * data2 = new bxml_element("match_dirs");
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("traindir","/vision/projects/kimia/categorization/99-db/dataset/");
  data2->set_attribute("trainlist","/vision/projects/kimia/categorization/99-db/99-db-subset.out");
  data2->set_attribute("dbdir","/vision/projects/kimia/categorization/99-db/dataset/");
  data2->set_attribute("dblist","/vision/projects/kimia/categorization/99-db/99-db-subset.out");
  data2->set_attribute("db_train_same","on"); 
  data2->set_attribute("saveshgms","on");  
  data2->set_attribute("shgms","/vision/projects/kimia/categorization/99-db/shgms/");
  data2->set_attribute("generateoutfile","on"); 
  data2->set_attribute("out","/vision/projects/kimia/categorization/99-db/out1");  // out file which will be appended a postfix 
  data2->set_attribute("orlstyle","on");  // if on assumes there exists a subfolder for each element in the main database folder
  data2->append_text("\n");

  return root;
}

