//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dbskr_match_patch_params.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

bool dbskr_match_patch_params::parse_from_data(bxml_data_sptr root)
{
  //vcl_cout << "dbskr_match_patch_params\n";

  bxml_element query("dbskr_match_patch_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  
  if (!result) {
    vcl_cout << "dbskr_match_patch_params::parse_from_data() - could not find the node dbskr_match_patch_params\n";
    return false;
  }

  bxml_element query2("shock_match_params");
  bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  if (!result2) {
    vcl_cout << "dbskr_match_patch_params::parse_from_data() - could not find the node shock_match_params\n";
    return false;
  }
   
  dbskr_match_shock_params dummy;
  dummy.parse_from_match_data(result2);
  shock_match_params_ = dummy.edit_params_;

  bxml_element query4("query_patches_params");
  bxml_data_sptr result4 = bxml_find_by_name(result, query4);

  if (!result4) {
    vcl_cout << "dbskr_match_patch_params::parse_from_data() - could not find the node query_patches_params\n";
    return false;
  }

  query_patch_params_.parse_from_data(result4);

  bxml_element query5("model_patches_params");
  bxml_data_sptr result5 = bxml_find_by_name(result, query5);

  if (!result5) {
    vcl_cout << "dbskr_match_patch_params::parse_from_data() - could not find the node model_patches_params\n";
    return false;
  }

  model_patch_params_.parse_from_data(result5);


  bxml_element query3("patch_match_dirs");
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

  data2->get_attribute("savematches", val);
  save_matches_ = val.compare("off") == 0 ? false: true; 
  data2->get_attribute("matchdir",match_folder_);

  data2->get_attribute("orlstyle", val);
  orl_format_folders_ = val.compare("off") == 0 ? false: true; 

  return true;
}

bxml_element *dbskr_match_patch_params::create_default_document_data() {
  
  bxml_element * root = new bxml_element("dbskr_match_patch_params");

  bxml_element * data1 = new bxml_element("shock_match_params");
  root->append_data(data1);
  root->append_text("\n");
  bxml_element * data1_1 = dbskr_match_shock_params::create_default_document_match_data();
  data1->append_data(data1_1);
  data1->append_text("\n");

  bxml_element * data3 = new bxml_element("query_patches_params");
  root->append_data(data3);
  root->append_text("\n");
  bxml_element * data3_1 = query_patch_params_.create_default_document_data();
  data3->append_data(data3_1);
  data3->append_text("\n");

  bxml_element * data4 = new bxml_element("model_patches_params");
  root->append_data(data4);
  root->append_text("\n");
  bxml_element * data4_1 = model_patch_params_.create_default_document_data();
  data4->append_data(data4_1);
  data4->append_text("\n");

  bxml_element * data2 = new bxml_element("patch_match_dirs");
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("traindir","/vision/projects/kimia/categorization/99-db/orl-exps/dataset/");
  data2->set_attribute("trainlist","/vision/projects/kimia/categorization/99-db/orl-exps/99-db-subset.out");
  data2->set_attribute("dbdir","/vision/projects/kimia/categorization/99-db/orl-exps/dataset/");
  data2->set_attribute("dblist","/vision/projects/kimia/categorization/99-db/orl-exps/99-db-subset.out");
  data2->set_attribute("savematches","on");  
  data2->set_attribute("matchdir","/vision/projects/kimia/categorization/99-db/orl-exps/patch_matches/");
  data2->set_attribute("orlstyle","on");  // if on assumes there exists a subfolder for each element in the main database folder
  data2->append_text("\n");

  return root;
}

