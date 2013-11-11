//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dbskr_visualize_patch_detections.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_evaluation.h>

#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_load.h>

//: read the files etc
bool dbskr_visualize_patch_detections::initialize()
{
  vcl_string storage_end = "patch_strg.bin";
  vcl_string model_storage_file = params_.model_dir_ + params_.model_name_ + params_.model_patch_params_.output_file_postfix_ + "-" + storage_end;
  //vcl_string model_storage_file = params_.model_dir_ + params_.model_name_ + params_.model_patch_params_.output_file_postfix_ + "/" + params_.model_name_ + params_.model_patch_params_.output_file_postfix_ + "-" + storage_end;

  if (!vul_file::exists(model_storage_file)) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << model_storage_file << " not found\n";
    return false;
  }

  //: load the model
  model_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(model_storage_file.c_str());
  model_st->b_read(ifs);
  ifs.close();

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  vcl_cout << model_st->size() << " patches in model storage, reading shocks..\n";
  dbsk2d_xshock_graph_fileio loader;
  //: load esfs for each patch
  for (unsigned iii = 0; iii < model_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = model_st->get_patch(iii);
    vcl_string patch_esf_name = model_storage_file.substr(0, model_storage_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
  }
#if 0
  //vcl_string model_gt_file = params_.model_dir_ + "groundtruth.xml";
  vcl_string model_gt_file = params_.model_dir_ + params_.model_name_ + ".xml";
  if (!vul_file::exists(model_gt_file)) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << model_gt_file << " not found\n";
    return false;
  }
    
  dborl_image_desc_parser parser;
  dborl_image_description_sptr model_id = dborl_image_description_parse(model_gt_file, parser);
  if (!model_id->has_single_category()) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - model image description has more than one category! image was not a valid map for a model\n";
    return false;
  }
  model_category_ = model_id->get_first_category();
#endif

  //vcl_string query_st_file = query_path_ + "/" + query_name_ + "/" + query_name_ + params_.query_patch_params_.output_file_postfix_ + "/" + query_name_ + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;  
  //vcl_string query_st_file = query_path_ + "/" + query_name_ + "/" + query_name_ + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;    
  vcl_string query_st_file = query_path_ + "/" + query_name_ + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;  
    
  if (!vul_file::exists(query_st_file)) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << query_st_file << " not found\n";
    return false;
  }
    
  query_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifsq(query_st_file.c_str());
  query_st->b_read(ifsq);
  ifsq.close();

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  vcl_cout << query_st->size() << " patches in test storage, reading shocks..\n";
  //: load esfs for each patch
  for (unsigned iii = 0; iii < query_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = query_st->get_patch(iii);
    vcl_string patch_esf_name = query_st_file.substr(0, query_st_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_);  // so that prepare_tree() and tree() methods of patch have the needed params
  }

  //: load the match instance which contains model vs query patch matches
  vcl_string match_name = params_.matches_dir_ + params_.model_name_ + params_.model_patch_params_.output_file_postfix_ + "-" + query_name_ + params_.query_patch_params_.output_file_postfix_ + params_.shock_match_params_.output_file_postfix_ + ".bin";
  if (!vul_file::exists(match_name.c_str())) {
    vcl_cout << "dborl_detect_patch_processor::process() - the file:" << match_name << " not found!\n";
    return false;
  }

  match = new dbskr_shock_patch_match();
  vsl_b_ifstream ifsm(match_name.c_str());
  match->b_read(ifsm);
  ifsm.close();

  //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < model_st->size(); ii++) 
    model_map[model_st->get_patch(ii)->id()] = model_st->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < query_st->size(); ii++) 
    query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
  match->set_id_map2(query_map);

  if (params_.use_normalized_costs_) {
    if (params_.use_reconst_boundary_length_)
      if (!match->compute_length_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
    else if (params_.use_total_splice_cost_)
      if (!match->compute_splice_norm_costs_of_cors()) // sorts the matches after recomputation of the normalized cost
        return false;
  }
  vcl_cout << " prepared id maps.. ";

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dbskr_visualize_patch_detections::process()
{
  vcl_string images_dir = "./images/";
  if (!create_only_html_) {
    //: prepare the images in the storages
    vul_file::make_directory(images_dir);
    vil_image_resource_sptr img_m = vil_load_image_resource((params_.model_dir_ + params_.model_name_ + ext_).c_str());
    //vil_image_resource_sptr img_q = vil_load_image_resource((query_path_ + query_name_ + "/" + query_name_ + ext_).c_str());
    vil_image_resource_sptr img_q = vil_load_image_resource((query_path_ + query_name_ + ext_).c_str());

    vil_rgb<int> color(1, 0, 0);
    model_st->create_ps_images(img_m, images_dir + params_.model_name_, false, color); 

    //vil_rgb<int> color(1, 0, 0);
    query_st->create_ps_images(img_q, images_dir + query_name_, false, color); 
      
    //: create the match images
    vcl_string model_shock = params_.model_dir_ + params_.model_name_ + ".esf";
    dbsk2d_xshock_graph_fileio loader;
    dbsk2d_shock_graph_sptr model_sg = loader.load_xshock_graph(model_shock);
    
    //vcl_string query_shock = query_path_ + query_name_ + "/" + query_name_ + ".esf";
    vcl_string query_shock = query_path_ + query_name_ + ".esf";
    dbsk2d_shock_graph_sptr query_sg = loader.load_xshock_graph(query_shock);
    
    match->create_match_ps_images(images_dir, params_.model_name_, model_sg, query_name_, query_sg);
  }

  //: create the html file with the match matrix between model and query patches
  match->create_html_table(images_dir, 
    params_.model_name_, 
    query_name_, 
    params_.out_file_ + ".html", 
    params_.model_name_ + "_" + query_name_ + "_" + params_.output_file_postfix_, html_image_ext_, html_image_width_, show_match_images_);

  return true;
}

bool dbskr_visualize_patch_detections::finalize()
{
  
  
  return true;
}

